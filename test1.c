#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 400            // 用户的数据量
#define NODEMAX 5          // B树的阶数
#define MAX_DEPARTMENTS 10 // 部门的数据量
#define MAX_CHILDREN 10    // 部门的子部门数量

// 用户的结构体定义
typedef struct
{
    char id[20];           // 用户的学号
    char name[40];         // 用户的姓名
    char email[40];        // 用户的邮箱
    char phone[20];        // 用户的电话
    char departmentId[20]; // 用户所在部门的ID
    char gender[10];       // 用户的性别
} User;

// 部门的结构体定义
typedef struct
{
    char id[20];                          // 部门的ID
    char name[40];                        // 部门的名称
    char leaderId[20];                    // 部门的领导ID
    char parentId[20];                    // 部门的父部门ID
    char childrenId[MAX_DEPARTMENTS][20]; // 部门的子部门ID
} Department;

// 部门结构体定义
typedef struct DepartmentNode
{
    char id[20];                                   // 部门的ID
    char name[40];                                 // 部门的名称
    User *leader;                                  // 指向部门领导的指针 (User)
    struct DepartmentNode *parent;                 // 指向父部门的指针
    struct DepartmentNode *children[MAX_CHILDREN]; // 指向子部门的指针数组
    int numChildren;                               // 子部门数量
    struct DepartmentNode *next;                   // 用于链表的下一个节点指针
} DepartmentNode;

// 用户节点的结构体定义
typedef struct
{
    char id[20];                // 用户的学号
    char name[40];              // 用户的姓名
    char email[40];             // 用户的邮箱
    char phone[20];             // 用户的电话
    DepartmentNode *department; // 指向用户所在部门的指针
    char gender[10];            // 用户的性别
} UserNode;

// 定义消息的结构体
typedef struct
{
    struct User *sender;   // 发送者的用户指针
    struct User *receiver; // 接收者的用户指针
    char content[100];     // 消息的内容
    char time[20];         // 消息的发送时间
} Message;

// 定义B树的结构体
typedef struct BTreeNode
{
    User *users[NODEMAX];
    struct BTreeNode *children[NODEMAX + 1];
    int count;
    int isLeaf;
} BTreeNode;

// 将学生信息定义为全局变量
User users[MAX];
UserNode userNodes[MAX];
// 将部门信息定义为全局变量
Department departments[MAX];
// 将当前登录的用户信息定义为全局变量
User *currentUser;
DepartmentNode *currentDepartment;      // 当前用户所管辖的部门
int userCount = 0, departmentCount = 0; // 用户和部门的数量
BTreeNode *idRoot = NULL;               // 以学号为关键字的B树根节点
BTreeNode *nameRoot = NULL;             // 以姓名为关键字的B树根节点
BTreeNode *phoneRoot = NULL;            // 以手机号为关键字的B树根节点
DepartmentNode *departmentHead = NULL;  // 部门链表的头指针

// 可视化面板函数声明
void defaultPannel();                                                                                          // 主页面面板
void studentSelfManagePannel();                                                                                // 学生信息编辑面板
void studentManagePannel();                                                                                    // 学生信息管理面板
void loginPannel();                                                                                            // 登录面板
void searchPannel();                                                                                           // 搜索面板
void init(User users[], Department departments[], UserNode userNodes[], int *userCount, int *departmentCount); // 初始化数据
void loadUsers(User users[], int *userCount);                                                                  // 加载用户数据
BTreeNode *createNode(int isLeaf);                                                                             // 创建B树节点
void splitChild(BTreeNode *parent, int i);                                                                     // 分裂子节点
void insertNonFull(BTreeNode *node, User *user, int (*compare)(User *, User *));                               // 插入数据到非满节点
void insert(BTreeNode **root, User *user, int (*compare)(User *, User *));                                     // 插入函数
int compareByID(User *user1, User *user2);                                                                     // 比较函数示例：按学号排序
int compareByName(User *user1, User *user2);                                                                   // 比较函数示例：按姓名排序
int compareByPhone(User *user1, User *user2);                                                                  // 比较函数示例：按手机号排序
User *searchById(BTreeNode *node, User *user, int (*compare)(User *, User *));                                 // 查找用户数据并返回User指针
UserNode *searchNodeById(BTreeNode *node, User *user, int (*compare)(User *, User *));                         // 查找用户数据并返回UserNode指针
void printUser(UserNode *user);                                                                                // 打印用户信息
void searchByIDRange(BTreeNode *node, const char *startID, const char *endID);                                 // 范围查找：查找学号范围内的用户
void searchByIDPrefix(BTreeNode *node, const char *prefix);                                                    // 前缀模糊查找：用户学号
void searchByNameRange(BTreeNode *node, const char *startName, const char *endName);                           // 范围查找：查找姓名范围内的用户
void searchByNamePrefix(BTreeNode *node, const char *prefix);                                                  // 前缀模糊查找：用户姓名
void searchByPhoneRange(BTreeNode *node, const char *startPhone, const char *endPhone);                        // 范围查找：查找手机号范围内的用户
void searchByPhonePrefix(BTreeNode *node, const char *prefix);                                                 // 前缀模糊查找：用户手机号
void loadDepartments(Department departments[], int *departmentCount);                                          // 加载部门数据
void convertDepartments(Department *departments, int departmentCount, DepartmentNode **head);                  // 将Department数组转换为链表节点并插入链表
DepartmentNode *createDepartmentNode(const char *id, const char *name, User *leader);                          // 创建新部门节点
void insertDepartment(DepartmentNode **head, DepartmentNode *newNode);                                         // 插入部门节点到链表头部
void setParent(DepartmentNode *department, DepartmentNode *parent);                                            // 设置父部门
void addChild(DepartmentNode *parent, DepartmentNode *child);                                                  // 添加子部门
void printDepartment(DepartmentNode *department);                                                              // 打印部门信息
DepartmentNode *findDepartmentById(DepartmentNode *head, const char *id);                                      // 查找部门节点通过部门ID
DepartmentNode *userDepartment(char *userId);                                                                  // 判断用户是否是部门领导
int isUserInControl(User *user);                                                                               // 判断用户是否收到当前用户控制

// 创建新部门节点
DepartmentNode *createDepartmentNode(const char *id, const char *name, User *leader)
{
    DepartmentNode *newNode = (DepartmentNode *)malloc(sizeof(DepartmentNode));
    if (newNode == NULL)
    {
        printf("内存分配失败\n");
        exit(1);
    }
    strcpy(newNode->id, id);
    strcpy(newNode->name, name);
    newNode->leader = leader; // 设置领导用户指针
    newNode->parent = NULL;   // 父节点初始化为空
    newNode->numChildren = 0; // 子部门数量初始化为0
    newNode->next = NULL;
    for (int i = 0; i < MAX_CHILDREN; i++)
    {
        newNode->children[i] = NULL; // 初始化子节点指针为空
    }
    return newNode;
}

// 插入部门节点到链表头部
void insertDepartment(DepartmentNode **head, DepartmentNode *newNode)
{
    newNode->next = *head;
    *head = newNode;
}

// 打印用户信息
void printUser(UserNode *user)
{
    if (user == NULL)
    {
        printf("未找到该用户\n");
        return;
    }
    printf("学号: %s\n", user->id);
    printf("姓名: %s\n", user->name);
    printf("邮箱: %s\n", user->email);
    printf("电话: %s\n", user->phone);
    printf("部门: %s\n", user->department->name);
    printf("性别: %s\n", user->gender);
}

// 设置父部门
void setParent(DepartmentNode *department, DepartmentNode *parent)
{
    department->parent = parent;
}

// 添加子部门
void addChild(DepartmentNode *parent, DepartmentNode *child)
{
    if (parent->numChildren < MAX_CHILDREN)
    {
        parent->children[parent->numChildren] = child;
        parent->numChildren++;
    }
    else
    {
        printf("子部门数量超过限制\n");
    }
}

// 将Department数组转换为链表节点并插入链表
void convertDepartments(Department *departments, int departmentCount, DepartmentNode **head)
{
    // 创建部门节点并插入链表
    for (int i = 0; i < departmentCount; i++)
    {
        Department *dep = &departments[i];

        User queryUser;
        // 按学号精准查找用户
        strcpy(queryUser.id, dep->leaderId);
        User *leader = searchById(idRoot, &queryUser, compareByID);
        // 创建新的部门节点
        DepartmentNode *newNode = createDepartmentNode(dep->id, dep->name, leader);

        // 插入部门节点到链表头部
        newNode->next = *head;
        *head = newNode;
    }

    // 再次遍历，设置父部门和子部门关系
    DepartmentNode *current = *head;
    while (current != NULL)
    {
        // 查找父部门
        for (int i = 0; i < departmentCount; i++)
        {
            if (strcmp(current->id, departments[i].id) == 0)
            {
                // 设置父部门
                if (strcmp(departments[i].parentId, "NULL") != 0)
                {
                    DepartmentNode *parent = findDepartmentById(*head, departments[i].parentId);
                    if (parent != NULL)
                    {
                        current->parent = parent;
                    }
                }

                // 设置子部门
                for (int j = 0; j < MAX_CHILDREN; j++)
                {
                    if (strcmp(departments[i].childrenId[j], "NULL") != 0)
                    {
                        DepartmentNode *child = findDepartmentById(*head, departments[i].childrenId[j]);
                        if (child != NULL && current->numChildren < MAX_CHILDREN)
                        {
                            current->children[current->numChildren] = child;
                            current->numChildren++;
                        }
                    }
                }
            }
        }
        current = current->next;
    }
}

// 打印部门信息
void printDepartment(DepartmentNode *department)
{
    if (department == NULL)
    {
        printf("您没有管辖任何部门\n");
        return;
    }
    printf("部门ID: %s\n", department->id);
    printf("部门名称: %s\n", department->name);

    // 打印部门领导信息
    if (department->leader != NULL)
    {
        printf("部门领导姓名: %s\n", department->leader->name);
        printf("部门领导学号: %s\n", department->leader->id);
        printf("部门领导邮箱: %s\n", department->leader->email);
        printf("部门领导电话: %s\n", department->leader->phone);
    }
    else
    {
        printf("部门领导: 无\n");
    }

    // 打印父部门信息
    if (department->parent != NULL)
    {
        printf("父部门: %s\n", department->parent->name);
    }
    else
    {
        printf("父部门: 无\n");
    }

    // 打印子部门信息
    printf("子部门: ");
    for (int i = 0; i < department->numChildren; i++)
    {
        printf("%s ", department->children[i]->name);
    }
    printf("\n");
}

// 查找部门节点通过部门ID
DepartmentNode *findDepartmentById(DepartmentNode *head, const char *id)
{
    DepartmentNode *current = head;
    while (current != NULL)
    {
        if (strcmp(current->id, id) == 0)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void loadUsers(User users[], int *userCount)
{
    FILE *fp = fopen("./user.txt", "r");
    *userCount = 0;
    if (fp == NULL)
    {
        printf("用户信息文件打开失败\n");
        exit(1);
    }
    while (fscanf(fp, "%s %s %s %s %s %s\n", users[*userCount].id, users[*userCount].name, users[*userCount].email, users[*userCount].phone, users[*userCount].departmentId, users[*userCount].gender) != EOF)
    {
        (*userCount)++;
    }
    fclose(fp);
}

void loadDepartments(Department departments[], int *departmentCount)
{
    FILE *fp = fopen("./department.txt", "r");
    *departmentCount = 0;
    if (fp == NULL)
    {
        printf("部门信息文件打开失败\n");
        exit(1);
    }

    char line[256]; // 用于存储每一行的读取数据
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        char childrenStr[200]; // 临时存储子部门ID的字符串
        int numChildren = 0;
        sscanf(line, "%s %s %s %s %[^\n]", departments[*departmentCount].id, departments[*departmentCount].name, departments[*departmentCount].leaderId, departments[*departmentCount].parentId, childrenStr);

        // 解析子部门ID
        if (strcmp(childrenStr, "NULL") != 0)
        {
            char *token = strtok(childrenStr, " ");
            while (token != NULL && numChildren < MAX_DEPARTMENTS)
            {
                strcpy(departments[*departmentCount].childrenId[numChildren], token);
                numChildren++;
                token = strtok(NULL, " ");
            }
        }

        // 如果没有子部门，则将 childrenId 初始化为空
        for (int i = numChildren; i < MAX_DEPARTMENTS; i++)
        {
            strcpy(departments[*departmentCount].childrenId[i], "NULL");
        }

        (*departmentCount)++;
    }

    fclose(fp);
}

// B树创建节点函数
BTreeNode *createNode(int isLeaf)
{
    BTreeNode *newNode = (BTreeNode *)malloc(sizeof(BTreeNode));
    newNode->isLeaf = isLeaf;
    newNode->count = 0;
    for (int i = 0; i < NODEMAX + 1; i++)
    {
        newNode->children[i] = NULL;
    }
    return newNode;
}

// 判断用户是否是部门领导
DepartmentNode *userDepartment(char *userId)
{
    DepartmentNode *current = departmentHead;
    while (current != NULL)
    {
        if (current->leader != NULL && strcmp(current->leader->id, userId) == 0)
        {
            return current; // 找到部门领导，返回该部门节点
        }
        current = current->next;
    }
    return NULL; // 未找到部门领导，返回空
}

// 分裂子节点
void splitChild(BTreeNode *parent, int i)
{
    BTreeNode *fullNode = parent->children[i];
    BTreeNode *newNode = createNode(fullNode->isLeaf);
    newNode->count = NODEMAX / 2;

    for (int j = 0; j < NODEMAX / 2; j++)
    {
        newNode->users[j] = fullNode->users[j + NODEMAX / 2 + 1];
    }

    if (!fullNode->isLeaf)
    {
        for (int j = 0; j <= NODEMAX / 2; j++)
        {
            newNode->children[j] = fullNode->children[j + NODEMAX / 2 + 1];
        }
    }

    fullNode->count = NODEMAX / 2;

    for (int j = parent->count; j >= i + 1; j--)
    {
        parent->children[j + 1] = parent->children[j];
    }

    parent->children[i + 1] = newNode;

    for (int j = parent->count - 1; j >= i; j--)
    {
        parent->users[j + 1] = parent->users[j];
    }

    parent->users[i] = fullNode->users[NODEMAX / 2];
    parent->count++;
}

// 插入数据到非满节点
void insertNonFull(BTreeNode *node, User *user, int (*compare)(User *, User *))
{
    int i = node->count - 1;

    if (node->isLeaf)
    {
        while (i >= 0 && compare(user, node->users[i]) < 0)
        {
            node->users[i + 1] = node->users[i];
            i--;
        }
        node->users[i + 1] = user;
        node->count++;
    }
    else
    {
        while (i >= 0 && compare(user, node->users[i]) < 0)
        {
            i--;
        }
        i++;
        if (node->children[i]->count == NODEMAX)
        {
            splitChild(node, i);
            if (compare(user, node->users[i]) > 0)
            {
                i++;
            }
        }
        insertNonFull(node->children[i], user, compare);
    }
}

// 插入函数，用于插入用户数据
void insert(BTreeNode **root, User *user, int (*compare)(User *, User *))
{
    if (*root == NULL)
    {
        *root = createNode(1);
    }

    if ((*root)->count == NODEMAX)
    {
        BTreeNode *newRoot = createNode(0);
        newRoot->children[0] = *root;
        splitChild(newRoot, 0);
        *root = newRoot;
    }

    insertNonFull(*root, user, compare);
}

// 比较函数示例：按学号排序
int compareByID(User *user1, User *user2)
{
    return strcmp(user1->id, user2->id);
}

// 比较函数示例：按姓名排序
int compareByName(User *user1, User *user2)
{
    return strcmp(user1->name, user2->name);
}

// 比较函数示例：按手机号排序
int compareByPhone(User *user1, User *user2)
{
    return strcmp(user1->phone, user2->phone);
}

// 判断用户是否收到当前用户控制
int isUserInControl(User *user)
{
    // 利用链表，查看用户所在部门是否是当前用户所管辖的部门
    int userIndex = user - users;
    UserNode userNode = userNodes[userIndex];
    DepartmentNode *userDep = userNodes[userIndex].department;
    while (userDep != NULL)
    {
        if (userDep == currentDepartment)
        {
            return 1;
        }
        userDep = userDep->parent;
    }
    return 0;
}

// 查找用户数据并返回User指针
User *searchById(BTreeNode *node, User *user, int (*compare)(User *, User *))
{
    int i = 0;
    // 查找在当前节点中的位置
    while (i < node->count && compare(user, node->users[i]) > 0)
    {
        i++;
    }

    // 如果找到了用户
    if (i < node->count && compare(user, node->users[i]) == 0)
    {
        return node->users[i]; // 返回找到的用户指针
    }
    // 如果是叶子节点且没有找到用户
    else if (node->isLeaf)
    {
        return NULL; // 用户未找到
    }
    // 如果不是叶子节点，继续在子节点中查找
    else
    {
        return searchById(node->children[i], user, compare);
    }
}

// 查找用户数据并返回UserNode指针
UserNode *searchNodeById(BTreeNode *node, User *user, int (*compare)(User *, User *))
{
    User *foundUser = searchById(node, user, compare);
    if (foundUser == NULL)
    {
        return NULL;
    }
    // 用户不再当前用户控制范围内，返回NULL
    if (!isUserInControl(foundUser))
    {
        return NULL;
    }
    int userIndex = foundUser - users;
    return &userNodes[userIndex];
}

// 范围查找：查找学号范围内的用户
void searchByIDRange(BTreeNode *node, const char *startID, const char *endID)
{
    int i = 0;
    while (i < node->count && strcmp(startID, node->users[i]->id) > 0)
    {
        i++;
    }

    while (i < node->count && strcmp(node->users[i]->id, endID) <= 0)
    {
        if (isUserInControl(node->users[i]))
        {
            User *user = node->users[i];
            int userIndex = user - users;
            printf("学号: %s, 姓名: %s, 邮箱: %s, 电话: %s, 部门: %s, 性别: %s\n", user->id, user->name, user->email, user->phone, userNodes[userIndex].department->name, user->gender);
        }
        if (!node->isLeaf)
        {
            searchByIDRange(node->children[i], startID, endID);
        }
        i++;
    }

    if (!node->isLeaf)
    {
        searchByIDRange(node->children[i], startID, endID);
    }
}

// 前缀模糊查找：用户学号
void searchByIDPrefix(BTreeNode *node, const char *prefix)
{
    int i = 0;
    while (i < node->count && strncmp(prefix, node->users[i]->id, strlen(prefix)) > 0)
    {
        i++;
    }

    while (i < node->count && strncmp(node->users[i]->id, prefix, strlen(prefix)) == 0)
    {
        if (isUserInControl(node->users[i]))
        {
            User *user = node->users[i];
            int userIndex = user - users;
            printf("学号: %s, 姓名: %s, 邮箱: %s, 电话: %s, 部门: %s, 性别: %s\n", user->id, user->name, user->email, user->phone, userNodes[userIndex].department->name, user->gender);
        }
        if (!node->isLeaf)
        {
            searchByIDPrefix(node->children[i], prefix);
        }
        i++;
    }

    if (!node->isLeaf)
    {
        searchByIDPrefix(node->children[i], prefix);
    }
}

// 范围查找：查找姓名范围内的用户
void searchByNameRange(BTreeNode *node, const char *startName, const char *endName)
{
    int i = 0;
    while (i < node->count && strcmp(startName, node->users[i]->name) > 0)
    {
        i++;
    }
    while (i < node->count && strcmp(node->users[i]->name, endName) <= 0)
    {
        if (isUserInControl(node->users[i]))
        {
            User *user = node->users[i];
            int userIndex = user - users;
            printf("学号: %s, 姓名: %s, 邮箱: %s, 电话: %s, 部门: %s, 性别: %s\n", user->id, user->name, user->email, user->phone, userNodes[userIndex].department->name, user->gender);
        }
        if (!node->isLeaf)
        {
            searchByNameRange(node->children[i], startName, endName);
        }
        i++;
    }

    if (!node->isLeaf)
    {
        searchByNameRange(node->children[i], startName, endName);
    }
}

// 前缀模糊查找：用户姓名
void searchByNamePrefix(BTreeNode *node, const char *prefix)
{
    int i = 0;
    while (i < node->count && strncmp(prefix, node->users[i]->name, strlen(prefix)) > 0)
    {
        i++;
    }

    while (i < node->count && strncmp(node->users[i]->name, prefix, strlen(prefix)) == 0)
    {
        if (isUserInControl(node->users[i]))
        {
            User *user = node->users[i];
            int userIndex = user - users;
            printf("学号: %s, 姓名: %s, 邮箱: %s, 电话: %s, 部门: %s, 性别: %s\n", user->id, user->name, user->email, user->phone, userNodes[userIndex].department->name, user->gender);
        }
        if (!node->isLeaf)
        {
            searchByNamePrefix(node->children[i], prefix);
        }
        i++;
    }

    if (!node->isLeaf)
    {
        searchByNamePrefix(node->children[i], prefix);
    }
}
// 范围查找：查找手机号范围内的用户
void searchByPhoneRange(BTreeNode *node, const char *startPhone, const char *endPhone)
{
    int i = 0;
    while (i < node->count && strcmp(startPhone, node->users[i]->phone) > 0)
    {
        i++;
    }

    while (i < node->count && strcmp(node->users[i]->phone, endPhone) <= 0)
    {
        if (isUserInControl(node->users[i]))
        {
            User *user = node->users[i];
            int userIndex = user - users;
            printf("学号: %s, 姓名: %s, 邮箱: %s, 电话: %s, 部门: %s, 性别: %s\n", user->id, user->name, user->email, user->phone, userNodes[userIndex].department == NULL ? "无" : userNodes[userIndex].department->name, user->gender);
        }
        if (!node->isLeaf)
        {
            searchByPhoneRange(node->children[i], startPhone, endPhone);
        }
        i++;
    }

    if (!node->isLeaf)
    {
        searchByPhoneRange(node->children[i], startPhone, endPhone);
    }
}
// 前缀模糊查找：用户手机号
void searchByPhonePrefix(BTreeNode *node, const char *prefix)
{
    int i = 0;
    while (i < node->count && strncmp(prefix, node->users[i]->phone, strlen(prefix)) > 0)
    {
        i++;
    }

    while (i < node->count && strncmp(node->users[i]->phone, prefix, strlen(prefix)) == 0)
    {
        if (isUserInControl(node->users[i]))
        {
            User *user = node->users[i];
            int userIndex = user - users;
            printf("学号: %s, 姓名: %s, 邮箱: %s, 电话: %s, 部门: %s, 性别: %s\n", user->id, user->name, user->email, user->phone, userNodes[userIndex].department == NULL ? "无" : userNodes[userIndex].department->name, user->gender);
        }
        if (!node->isLeaf)
        {
            searchByPhonePrefix(node->children[i], prefix);
        }
        i++;
    }

    if (!node->isLeaf)
    {
        searchByPhonePrefix(node->children[i], prefix);
    }
}

// 搜索面板
void searchPannel()
{
    printf("请选择搜索方式：\n");
    printf("1. 按学号搜索\n");
    printf("2. 按姓名搜索\n");
    printf("3. 按手机号搜索\n");
    printf("4. 返回上一级\n");
    printf("请选择：");
    int choice;
    scanf("%d", &choice);
    if (choice != 4)
    {
        printf("请选择搜索方式：\n");
        printf("1. 精确查找\n");
        printf("2. 范围查找\n");
        printf("3. 前缀模糊查找\n");
        printf("请选择：");
        int searchChoice;
        scanf("%d", &searchChoice);
        choice = choice * 10 + searchChoice;
        char prefix[40]; // 前缀查找
        User queryUser;  // 查询用户
        switch (choice)
        {
        case 11:
            // 按学号精确查找
            char id[20];
            printf("请输入学号：");
            scanf("%s", id);
            // 按学号精准查找用户
            strcpy(queryUser.id, id);
            printUser(searchNodeById(idRoot, &queryUser, compareByID));
            break;
        case 12:
            // 按学号范围查找
            char startID[20], endID[20];
            printf("请输入学号范围，左小右大，用空格分隔\n");
            scanf("%s %s", startID, endID);
            searchByIDRange(idRoot, startID, endID);
            break;
        case 13:
            // 按学号前缀模糊查找
            printf("请输入学号前缀：");
            scanf("%s", prefix);
            searchByIDPrefix(idRoot, prefix);
            break;
        case 21:
            // 按姓名精确查找
            char name[40];
            printf("请输入姓名：");
            scanf("%s", name);
            // 按姓名精准查找用户
            strcpy(queryUser.name, name);
            printUser(searchNodeById(nameRoot, &queryUser, compareByName));
            break;
        case 22:
            // 按姓名范围查找
            char startName[40], endName[40];
            printf("请输入姓名范围，左小右大，用空格分隔\n");
            scanf("%s %s", startName, endName);
            searchByNameRange(nameRoot, startName, endName);
            break;
        case 23:
            // 按姓名前缀模糊查找
            printf("请输入姓名前缀：");
            scanf("%s", prefix);
            searchByNamePrefix(nameRoot, prefix);
            break;
        case 31:
            // 按手机号精确查找
            char phone[20];
            printf("请输入手机号：");
            scanf("%s", phone);
            // 按手机号精准查找用户
            strcpy(queryUser.phone, phone);
            printUser(searchNodeById(phoneRoot, &queryUser, compareByPhone));
            break;
        case 32:
            // 按手机号范围查找
            char startPhone[20], endPhone[20];
            printf("请输入手机号范围，左小右大，用空格分隔\n");
            scanf("%s %s", startPhone, endPhone);
            searchByPhoneRange(phoneRoot, startPhone, endPhone);
            break;
        case 33:
            // 按手机号前缀模糊查找
            printf("请输入手机号前缀：");
            scanf("%s", prefix);
            searchByPhonePrefix(phoneRoot, prefix);
            break;
        default:
            printf("输入错误！\n");
            searchPannel();
            break;
        }
    }
    else
    {
        if (currentDepartment != NULL)
        {
            studentManagePannel();
        }
        else
        {
            studentSelfManagePannel();
        }
    }
}

// 登录面板
void defaultPannel()
{
    // system("cls");
    printf("************************************************\n");
    printf("欢迎登陆学生信息管理系统!请选择：\n");
    printf("1. 登录\n");
    printf("2. 退出系统\n");
    printf("************************************************\n");
    int choice;
    while (1)
    {
        printf("请选择：");
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            // 登录
            loginPannel();
            break;
        case 2:
            // 返回上一级
            printf("退出系统\n");
            exit(0);
            break;
        default:
            printf("输入错误！\n");
            break;
        }
    }
}

// 学生个人信息编辑面板
void studentSelfManagePannel()
{
    system("cls");
    printf("登陆成功\n");
    printf("************************************************\n");
    // 打印用户姓名
    while (1)
    {
        printf("您好，%s，请选择：\n", currentUser->name);
        printf("1. 查看个人信息\n");
        printf("2. 修改个人信息\n");
        printf("3. 向部门负责人发送消息\n");
        printf("4. 查看消息列表\n");
        printf("5. 查看部门信息\n");
        printf("6. 返回上一级\n");
        printf("************************************************\n");
        printf("请选择：");
        int choice;
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            // 查看个人信息
            break;
        case 2:
            // 修改个人信息
            break;
        case 3:
            // 向部门负责人发送消息
            break;
        case 4:
            // 查看消息列表
            break;
        case 5:
            // 查看部门信息
            int userIndex = currentUser - users;
            printDepartment(userNodes[userIndex].department);
            break;
        case 6:
            // 返回上一级
            defaultPannel();
            break;
        default:
            printf("输入错误！\n");
            break;
        }
    }
}

// 学生管理面板
void studentManagePannel()
{
    printf("************************************************\n");
    // 打印用户姓名
    printf("您好，%s，请选择：\n", currentUser->name);
    printf("1. 查看个人信息\n");
    printf("2. 修改个人信息\n");
    printf("3. 向部门负责人发送消息\n");
    printf("4. 查看消息列表\n");
    printf("5. 向特定用户发送消息\n");
    printf("6. 向用户群发消息\n");
    printf("7. 查询用户信息\n");
    printf("8. 修改用户信息\n");
    printf("9. 修改部门名称\n");
    printf("10. 查看自己管辖部门信息\n");
    printf("11. 查看自己所处部门信息\n");
    printf("12. 返回上一级\n");
    printf("************************************************\n");
    printf("请选择：");
    int choice;
    scanf("%d", &choice);
    switch (choice)
    {
    case 1:
        // 查看个人信息
        break;
    case 2:
        // 修改个人信息
        break;
    case 3:
        // 向部门负责人发送消息
        break;
    case 4:
        // 查看消息列表
        break;
    case 5:
        // 向特定用户发送消息
        break;
    case 6:
        // 向用户群发消息
        break;
    case 7:
        // 查询用户信息
        searchPannel();
        break;
    case 8:
        // 修改用户信息
        break;
    case 9:
        // 修改部门名称
        break;
    case 10:
        // 查看自己可以管理的部门
        printDepartment(currentDepartment);
        break;
    case 11:
        // 查看自己所处部门信息
        int userIndex = currentUser - users;
        printDepartment(userNodes[userIndex].department);
        break;
    case 12:
        // 返回上一级
        defaultPannel();
        break;
    default:
        printf("输入错误！\n");
        break;
    }
    studentManagePannel();
}

// 登录面板
void loginPannel()
{
    printf("请输入您的学号：");
    char id[20];
    scanf("%s", id);
    // 查找用户
    User queryUser;
    // 按学号精准查找用户
    strcpy(queryUser.id, id);
    User *foundUser = searchById(idRoot, &queryUser, compareByID);
    currentDepartment = NULL; // 重置当前部门
    if (foundUser != NULL)
    {
        currentUser = foundUser;
        currentDepartment = userDepartment(currentUser->id);
        system("cls");
        if (currentDepartment != NULL)
        {
            studentManagePannel();
        }
        else
        {
            studentSelfManagePannel();
        }
    }

    printf("用户不存在！\n");
    loginPannel();
}

// 初始化数据
void init(User users[], Department departments[], UserNode userNodes[], int *userCount, int *departmentCount)
{
    loadUsers(users, userCount);
    for (int i = 0; i < *userCount; i++)
    {
        insert(&idRoot, &users[i], compareByID);       // 添加到学号树
        insert(&nameRoot, &users[i], compareByName);   // 添加到姓名树
        insert(&phoneRoot, &users[i], compareByPhone); // 添加到电话树
        // printf("%s %s %s %s %s %s\n", users[i].id, users[i].name, users[i].email, users[i].phone, users[i].departmentId, users[i].gender);
    }

    loadDepartments(departments, departmentCount);

    convertDepartments(departments, *departmentCount, &departmentHead);

    for (int i = 0; i < *userCount; i++)
    {
        strcpy(userNodes[i].id, users[i].id);
        strcpy(userNodes[i].name, users[i].name);
        strcpy(userNodes[i].email, users[i].email);
        strcpy(userNodes[i].phone, users[i].phone);
        DepartmentNode *current = departmentHead;
        userNodes[i].department = findDepartmentById(current, users[i].departmentId);
        strcpy(userNodes[i].gender, users[i].gender);
    }
    // 打印部门信息
    // printDepartment(findDepartmentById(departmentHead, "291144669141"));

    User queryUser;
    // 按学号精准查找用户
    // strcpy(queryUser.name, "曹东");
    // printf("Search user by name 曹东:\n");
    // User *user = searchById(nameRoot, &queryUser, compareByName);
    // printf("学号: %s, 姓名: %s, 邮箱: %s, 电话: %s, 部门: %s, 性别: %s\n", user->id, user->name, user->email, user->phone, userNodes[user - users].department->name, user->gender);

    // printf("Search ID range 2235062001 to 2235062030:\n");
    // searchByIDRange(idRoot, "2235062001", "2235062030");

    // printf("\nSearch ID prefix '213505':\n");
    // searchByIDPrefix(idRoot, "213505");
}

int main()
{
    init(users, departments, userNodes, &userCount, &departmentCount);
    defaultPannel();
    return 0;
}