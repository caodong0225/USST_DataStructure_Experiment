#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 400            // �û���������
#define NODEMAX 5          // B���Ľ���
#define MAX_DEPARTMENTS 10 // ���ŵ�������
#define MAX_CHILDREN 10    // ���ŵ��Ӳ�������
#define NODEMIN (NODEMAX / 2)

// �û��Ľṹ�嶨��
typedef struct
{
    char id[20];           // �û���ѧ��
    char name[40];         // �û�������
    char email[40];        // �û�������
    char phone[20];        // �û��ĵ绰
    char departmentId[20]; // �û����ڲ��ŵ�ID
    char gender[10];       // �û����Ա�
} User;

// ���ŵĽṹ�嶨��
typedef struct
{
    char id[20];                          // ���ŵ�ID
    char name[40];                        // ���ŵ�����
    char leaderId[20];                    // ���ŵ��쵼ID
    char parentId[20];                    // ���ŵĸ�����ID
    char childrenId[MAX_DEPARTMENTS][20]; // ���ŵ��Ӳ���ID
} Department;

// ���Žṹ�嶨��
typedef struct DepartmentNode
{
    char id[20];                                   // ���ŵ�ID
    char name[40];                                 // ���ŵ�����
    User *leader;                                  // ָ�����쵼��ָ�� (User)
    struct DepartmentNode *parent;                 // ָ�򸸲��ŵ�ָ��
    struct DepartmentNode *children[MAX_CHILDREN]; // ָ���Ӳ��ŵ�ָ������
    int numChildren;                               // �Ӳ�������
    struct DepartmentNode *next;                   // �����������һ���ڵ�ָ��
} DepartmentNode;

// ��Ϣ�ڵ�Ľṹ�嶨��
typedef struct MessageNode
{
    User *sender;             // ָ�����ߵ�ָ��
    char content[256];        // ��Ϣ����
    struct MessageNode *next; // ָ����һ����Ϣ��ָ��
} MessageNode;

// �û��ڵ�Ľṹ�嶨��
typedef struct
{
    char id[20];                // �û���ѧ��
    char name[40];              // �û�������
    char email[40];             // �û�������
    char phone[20];             // �û��ĵ绰
    DepartmentNode *department; // ָ���û����ڲ��ŵ�ָ��
    char gender[10];            // �û����Ա�
    MessageNode *messages;      // ָ����Ϣ�����ָ��
} UserNode;

// ����B���Ľṹ��
typedef struct BTreeNode
{
    User *users[NODEMAX];
    struct BTreeNode *children[NODEMAX + 1];
    int count;
    int isLeaf;
} BTreeNode;

// ��ѧ����Ϣ����Ϊȫ�ֱ���
User users[MAX];
UserNode userNodes[MAX];
// ��������Ϣ����Ϊȫ�ֱ���
Department departments[MAX];
// ����ǰ��¼���û���Ϣ����Ϊȫ�ֱ���
User *currentUser;
DepartmentNode *currentDepartment;      // ��ǰ�û�����Ͻ�Ĳ���
int userCount = 0, departmentCount = 0; // �û��Ͳ��ŵ�����
BTreeNode *idRoot = NULL;               // ��ѧ��Ϊ�ؼ��ֵ�B�����ڵ�
BTreeNode *nameRoot = NULL;             // ������Ϊ�ؼ��ֵ�B�����ڵ�
BTreeNode *phoneRoot = NULL;            // ���ֻ���Ϊ�ؼ��ֵ�B�����ڵ�
DepartmentNode *departmentHead = NULL;  // ���������ͷָ��

// ���ӻ���庯������
void defaultPannel();                                                                                          // ��ҳ�����
void studentSelfManagePannel();                                                                                // ѧ����Ϣ�༭���
void studentManagePannel();                                                                                    // ѧ����Ϣ�������
void loginPannel();                                                                                            // ��¼���
void searchPannel();                                                                                           // �������
void init(User users[], Department departments[], UserNode userNodes[], int *userCount, int *departmentCount); // ��ʼ������
void loadUsers(User users[], int *userCount);                                                                  // �����û�����
BTreeNode *createNode(int isLeaf);                                                                             // ����B���ڵ�
void splitChild(BTreeNode *parent, int i);                                                                     // �����ӽڵ�
void insertNonFull(BTreeNode *node, User *user, int (*compare)(User *, User *));                               // �������ݵ������ڵ�
void insert(BTreeNode **root, User *user, int (*compare)(User *, User *));                                     // ���뺯��
int compareByID(User *user1, User *user2);                                                                     // �ȽϺ���ʾ������ѧ������
int compareByName(User *user1, User *user2);                                                                   // �ȽϺ���ʾ��������������
int compareByPhone(User *user1, User *user2);                                                                  // �ȽϺ���ʾ�������ֻ�������
User *searchById(BTreeNode *node, User *user, int (*compare)(User *, User *));                                 // �����û����ݲ�����Userָ��
UserNode *searchNodeById(BTreeNode *node, User *user, int (*compare)(User *, User *));                         // �����û����ݲ�����UserNodeָ��
void printUser(UserNode *user);                                                                                // ��ӡ�û���Ϣ
void searchByIDRange(BTreeNode *node, const char *startID, const char *endID);                                 // ��Χ���ң�����ѧ�ŷ�Χ�ڵ��û�
void searchByIDPrefix(BTreeNode *node, const char *prefix);                                                    // ǰ׺ģ�����ң��û�ѧ��
void searchByNameRange(BTreeNode *node, const char *startName, const char *endName);                           // ��Χ���ң�����������Χ�ڵ��û�
void searchByNamePrefix(BTreeNode *node, const char *prefix);                                                  // ǰ׺ģ�����ң��û�����
void searchByPhoneRange(BTreeNode *node, const char *startPhone, const char *endPhone);                        // ��Χ���ң������ֻ��ŷ�Χ�ڵ��û�
void searchByPhonePrefix(BTreeNode *node, const char *prefix);                                                 // ǰ׺ģ�����ң��û��ֻ���
void loadDepartments(Department departments[], int *departmentCount);                                          // ���ز�������
void convertDepartments(Department *departments, int departmentCount, DepartmentNode **head);                  // ��Department����ת��Ϊ����ڵ㲢��������
DepartmentNode *createDepartmentNode(const char *id, const char *name, User *leader);                          // �����²��Žڵ�
void insertDepartment(DepartmentNode **head, DepartmentNode *newNode);                                         // ���벿�Žڵ㵽����ͷ��
void setParent(DepartmentNode *department, DepartmentNode *parent);                                            // ���ø�����
void addChild(DepartmentNode *parent, DepartmentNode *child);                                                  // ����Ӳ���
void printDepartment(DepartmentNode *department);                                                              // ��ӡ������Ϣ
DepartmentNode *findDepartmentById(DepartmentNode *head, const char *id);                                      // ���Ҳ��Žڵ�ͨ������ID
DepartmentNode *findDepartmentByName(DepartmentNode *head, const char *name);                                  // ���Ҳ��Žڵ�ͨ����������
DepartmentNode *userDepartment(char *userId);                                                                  // �ж��û��Ƿ��ǲ����쵼
int isUserInControl(User *user);                                                                               // �ж��û��Ƿ��յ���ǰ�û�����
int isDepartmentInControl(DepartmentNode *department);                                                         // �жϲ����Ƿ��ܵ���ǰ�û�����
void printSelfInfo();                                                                                          // ��ӡ��ǰ�û�����Ϣ
void editSelfInfo();                                                                                           // �޸ĵ�ǰ�û���Ϣ
void updateUserInfo(BTreeNode *node, User *userOld, User *userNew, int (*compare)(User *, User *));            // �����û���Ϣ
void merge(BTreeNode *node, int idx);                                                                          // �ϲ��ӽڵ�
void deleteFromNonLeaf(BTreeNode *node, int idx, int (*compare)(User *, User *));                              // �ӷ�Ҷ�ӽڵ�ɾ���û�
void deleteFromLeaf(BTreeNode *node, int idx);                                                                 // ��Ҷ�ӽڵ�ɾ���û�
void borrowFromPrev(BTreeNode *node, int idx);                                                                 // ������ֵܽڵ���û�
void borrowFromNext(BTreeNode *node, int idx);                                                                 // ���ұ��ֵܽڵ���û�
void deleteUser(BTreeNode *node, User *user, int (*compare)(User *, User *));                                  // ɾ���û�
void printBTree(BTreeNode *node, int level);                                                                   // ��ӡB���Ľڵ�����
void printBTreeRoot(BTreeNode *node);                                                                          // ��ӡB���ĸ��ڵ�����
void editStudentInfoPannel();                                                                                  // �޸�ѧ����Ϣ
void editStudentInfo(User *user);                                                                              // �޸�ѧ����Ϣ���
void editDepartmentInfoPannel();                                                                               // �޸Ĳ�����Ϣ���
void addStudentInfo(User *user);                                                                               // ���ѧ����Ϣ
void editDepartmentName();                                                                                     // �޸Ĳ�������
void editDepartmentLeader();                                                                                   // �޸Ĳ����쵼
void pushGroupMessage();                                                                                       // ����Ⱥ��Ϣ
void printMessageReceived();                                                                                   // ��ӡ��ǰ�û��յ�����Ϣ
void sendMessageToLeader();                                                                                    // ������Ϣ�������쵼
void sendMessageToTargetUser();                                                                                // ������Ϣ��ָ���û�

// �����²��Žڵ�
DepartmentNode *createDepartmentNode(const char *id, const char *name, User *leader)
{
    DepartmentNode *newNode = (DepartmentNode *)malloc(sizeof(DepartmentNode));
    if (newNode == NULL)
    {
        printf("�ڴ����ʧ��\n");
        exit(1);
    }
    strcpy(newNode->id, id);
    strcpy(newNode->name, name);
    newNode->leader = leader; // �����쵼�û�ָ��
    newNode->parent = NULL;   // ���ڵ��ʼ��Ϊ��
    newNode->numChildren = 0; // �Ӳ���������ʼ��Ϊ0
    newNode->next = NULL;
    for (int i = 0; i < MAX_CHILDREN; i++)
    {
        newNode->children[i] = NULL; // ��ʼ���ӽڵ�ָ��Ϊ��
    }
    return newNode;
}

// ������Ϣ���ض��û�
void sendMessageToTargetUser()
{
    char targetId[20];
    printf("������Ŀ���û�ѧ��: ");
    scanf("%s", targetId);
    User queryUser;
    strcpy(queryUser.id, targetId);
    UserNode *targetUser = searchNodeById(idRoot, &queryUser, compareByID);
    if (targetUser != NULL)
    {
        char content[256];
        printf("��������Ϣ����: ");
        scanf("%s", content);
        MessageNode *newMessage = (MessageNode *)malloc(sizeof(MessageNode));
        if (newMessage == NULL)
        {
            printf("�ڴ����ʧ��\n");
            exit(1);
        }
        newMessage->sender = currentUser;
        strcpy(newMessage->content, content);
        // ������Ϣ�ڵ㵽����ͷ��
        newMessage->next = targetUser->messages; // ������Ϣ�ڵ���뵽��Ϣ����ͷ��
        targetUser->messages = newMessage;
        printf("���ͳɹ�\n");
    }
    else
    {
        printf("δ�ҵ��û�\n");
    }
}

// Ⱥ����Ϣ
void pushGroupMessage()
{
    char content[256];
    printf("��������Ϣ����: ");
    scanf("%s", content);
    for (int i = 0; i < userCount; i++)
    {
        User *user = &users[i]; // ��ȡ�û�ָ��
        if (isUserInControl(user) && strcmp(user->id, currentUser->id) != 0)
        {
            MessageNode *newMessage = (MessageNode *)malloc(sizeof(MessageNode));
            if (newMessage == NULL)
            {
                printf("�ڴ����ʧ��\n");
                exit(1);
            }
            newMessage->sender = currentUser;
            strcpy(newMessage->content, content);
            // ������Ϣ�ڵ㵽����ͷ��
            newMessage->next = userNodes[i].messages; // ������Ϣ�ڵ���뵽��Ϣ����ͷ��
            userNodes[i].messages = newMessage;
        }
    }
    printf("���ͳɹ�\n");
}

// ����Ϣ�������쵼
void sendMessageToLeader()
{
    int userIndex = currentUser - users; // ��ȡ��ǰ�û���users�����е�����
    DepartmentNode *department = userNodes[userIndex].department;
    if (department->leader != NULL)
    {
        char content[256];
        printf("��������Ϣ����: ");
        scanf("%s", content);
        MessageNode *newMessage = (MessageNode *)malloc(sizeof(MessageNode));
        if (newMessage == NULL)
        {
            printf("�ڴ����ʧ��\n");
            exit(1);
        }
        newMessage->sender = currentUser;
        strcpy(newMessage->content, content);
        // ������Ϣ�ڵ㵽����ͷ��
        newMessage->next = userNodes[department->leader - users].messages; // ������Ϣ�ڵ���뵽��Ϣ����ͷ��
        userNodes[department->leader - users].messages = newMessage;
        printf("���ͳɹ�\n");
    }
    else
    {
        printf("����û���쵼\n");
    }
}

// ��ӡ��ǰ�û��յ�����Ϣ
void printMessageReceived()
{
    int userIndex = currentUser - users;                  // ��ȡ��ǰ�û���users�����е�����
    MessageNode *current = userNodes[userIndex].messages; // ��ȡ��ǰ�û�����Ϣ����ͷָ��
    if (current == NULL)
    {
        printf("û����Ϣ\n");
        return;
    }
    printf("��Ϣ�б�\n");
    while (current != NULL)
    {
        printf("������: %s\n", current->sender->name);
        printf("��Ϣ����: %s\n", current->content);
        current = current->next;
    }
}

// ���벿�Žڵ㵽����ͷ��
void insertDepartment(DepartmentNode **head, DepartmentNode *newNode)
{
    newNode->next = *head;
    *head = newNode;
}

// ��ӡ��ǰ�û�����Ϣ
void printSelfInfo()
{
    int userIndex = currentUser - users; // ��ȡ��ǰ�û���users�����е�����
    printf("��ǰ�û���Ϣ��\n");
    printf("ѧ��: %s\n", currentUser->id);
    printf("����: %s\n", currentUser->name);
    printf("����: %s\n", currentUser->email);
    printf("�绰: %s\n", currentUser->phone);
    printf("���ڲ���: %s\n", userNodes[userIndex].department->name);
    printf("�Ա�: %s\n", currentUser->gender);
    printf("�ϼ�����: %s\n", userNodes[userIndex].department->parent == NULL ? "��" : userNodes[userIndex].department->parent->name);
}

// ����ѧ����Ϣ
void addStudentInfo(User *user)
{
    insert(&idRoot, user, compareByID);
    insert(&nameRoot, user, compareByName);
    insert(&phoneRoot, user, compareByPhone);
}

// ��ӡ�û���Ϣ
void printUser(UserNode *user)
{
    if (user == NULL)
    {
        printf("δ�ҵ��û�\n");
        return;
    }
    printf("ѧ��: %s\n", user->id);
    printf("����: %s\n", user->name);
    printf("����: %s\n", user->email);
    printf("�绰: %s\n", user->phone);
    printf("����: %s\n", user->department->name);
    printf("�Ա�: %s\n", user->gender);
}

// �޸Ĳ����쵼
void editDepartmentLeader()
{
    printf("�����벿������: ");
    char departmentName[40];
    scanf("%s", departmentName);
    DepartmentNode *department = findDepartmentByName(departmentHead, departmentName);
    if (department != NULL)
    {
        if (strcmp(department->leader->id, currentUser->id) == 0)
        {
            printf("����ת��Ȩ��\n");
            return;
        }
        printf("�ҵ�������Ϣ��\n");
        printDepartment(department);
        printf("�������µĲ����쵼ѧ��: ");
        char leaderId[40];
        scanf("%s", leaderId);
        User queryUser;
        strcpy(queryUser.id, leaderId);
        UserNode *leader = searchNodeById(idRoot, &queryUser, compareByID);
        if (leader != NULL)
        {
            int userIndex = leader - userNodes;
            department->leader = &users[userIndex];
            printf("�޸ĳɹ�\n");
        }
        else
        {
            printf("δ�ҵ��û�\n");
        }
    }
    else
    {
        printf("δ�ҵ����Ż�Ȩ�޲���\n");
    }
}

// �޸�ѧ����Ϣ
void editStudentInfo(User *user)
{
    char name[40], email[40], phone[40];
    int userIndex = user - users;
    printf("��ǰѧ����Ϣ��\n");
    printUser(&userNodes[userIndex]);
    printf("�������µ���Ϣ��\n");
    printf("����: ");
    scanf("%s", name);
    printf("����: ");
    scanf("%s", email);
    printf("�绰: ");
    scanf("%s", phone);
    strcpy(users[userIndex].name, name);
    strcpy(users[userIndex].email, email);
    strcpy(users[userIndex].phone, phone);
    strcpy(userNodes[userIndex].name, name);
    strcpy(userNodes[userIndex].email, email);
    strcpy(userNodes[userIndex].phone, phone);
    updateUserInfo(nameRoot, user, &users[userIndex], compareByName);
    updateUserInfo(phoneRoot, user, &users[userIndex], compareByPhone);
    printf("�޸ĳɹ�\n");
}

// �޸�ѧ����Ϣ���
void editStudentInfoPannel()
{
    printf("��������Ҫ���еĲ���:\n");
    printf("1. �޸�ѧ����Ϣ\n");
    printf("2. ɾ��ѧ����Ϣ\n");
    printf("3. ����ѧ����Ϣ\n");
    printf("4. ����\n");
    char studentId[20];
    int choice;
    printf("��ѡ��: ");
    scanf("%d", &choice);
    if (choice != 4)
    {
        printf("������ѧ��ѧ��: ");
        scanf("%s", studentId);
        User queryUser;
        switch (choice)
        {
        case 1:
            strcpy(queryUser.id, studentId);
            // ��Ȩ���鿴�ܷ��޸�ѧ��
            UserNode *editedUserNode = searchNodeById(idRoot, &queryUser, compareByID);
            if (editedUserNode != NULL)
            {
                User *editedUser = searchById(idRoot, &queryUser, compareByID);
                editStudentInfo(editedUser);
            }
            else
            {
                printf("δ�ҵ�ѧ��\n");
            }
            break;
        case 2:
            strcpy(queryUser.id, studentId);
            UserNode *deletedUser = searchNodeById(idRoot, &queryUser, compareByID);
            if (deletedUser != NULL)
            {
                int userIndex = deletedUser - userNodes;
                deleteUser(idRoot, &users[userIndex], compareByID);
                deleteUser(nameRoot, &users[userIndex], compareByName);
                deleteUser(phoneRoot, &users[userIndex], compareByPhone);
                printf("ɾ���ɹ�\n");
            }
            else
            {
                printf("δ�ҵ�ѧ��\n");
            }
            break;
        case 3:
            User *userExist = searchById(idRoot, &queryUser, compareByID);
            if (userExist != NULL)
            {
                printf("ѧ���Ѵ��ڣ���������\n");
                break;
            }
            printf("������ѧ����Ϣ��\n");
            char name[40];
            char email[40];
            char phone[40];
            char departmentName[40];
            char gender[10];
            char id[20];
            printf("����: ");
            scanf("%s", name);
            printf("����: ");
            scanf("%s", email);
            printf("�绰: ");
            scanf("%s", phone);
            printf("��������: ");
            scanf("%s", departmentName);
            printf("�Ա�: ");
            scanf("%s", gender);
            strcpy(queryUser.id, studentId);
            DepartmentNode *department = findDepartmentByName(departmentHead, departmentName); // ���Ҳ���
            if (department != NULL)
            {
                User newUser;
                strcpy(newUser.id, studentId);
                strcpy(newUser.name, name);
                strcpy(newUser.email, email);
                strcpy(newUser.phone, phone);
                strcpy(newUser.departmentId, department->id);
                strcpy(newUser.gender, gender);
                // ��ӵ�users���鵱��
                users[userCount] = newUser;
                UserNode userNode;
                strcpy(userNode.id, studentId);
                strcpy(userNode.name, name);
                strcpy(userNode.email, email);
                strcpy(userNode.phone, phone);
                userNode.department = department;
                strcpy(userNode.gender, gender);
                userNode.messages = NULL;
                userNodes[userCount] = userNode;
                addStudentInfo(&users[userCount]); // ���ѧ����Ϣ
                userCount++;
                printf("��ӳɹ�\n");
            }
            else
            {
                printf("δ�ҵ����Ż�Ȩ�޲��㣬��������\n");
            }
            break;
        default:
            printf("�����������������\n");
            editStudentInfoPannel();
            break;
        }
    }
    else
    {
        studentManagePannel();
    }
}

// �޸ĵ�ǰ�û���Ϣ
void editSelfInfo()
{
    int userIndex = currentUser - users; // ��ȡ��ǰ�û���users�����е�����
    char name[40], email[40], phone[40];
    printf("��ǰ�û���Ϣ��\n");
    printUser(&userNodes[userIndex]);
    printf("�������µ���Ϣ��\n");
    printf("����: ");
    scanf("%s", name);
    printf("����: ");
    scanf("%s", email);
    printf("�绰: ");
    scanf("%s", phone);
    char prefixNew[40];
    strcpy(users[userIndex].name, name);
    strcpy(users[userIndex].email, email);
    strcpy(users[userIndex].phone, phone);
    updateUserInfo(phoneRoot, currentUser, &users[userIndex], compareByPhone); // �޸�B���ṹ�еĵ绰��Ϣ
    updateUserInfo(nameRoot, currentUser, &users[userIndex], compareByName);   // �޸�B���ṹ�е�������Ϣ
    strcpy(currentUser->name, name);
    strcpy(currentUser->email, email);
    strcpy(currentUser->phone, phone);
    strcpy(userNodes[userIndex].name, name);
    strcpy(userNodes[userIndex].email, email);
    strcpy(userNodes[userIndex].phone, phone);
    strcpy(users[userIndex].name, name);
    strcpy(users[userIndex].email, email);
    strcpy(users[userIndex].phone, phone);
    // �޸�B���ṹ�е���Ϣ
    printf("�޸ĳɹ�\n");
}

// ���ø�����
void setParent(DepartmentNode *department, DepartmentNode *parent)
{
    department->parent = parent;
}

// ����Ӳ���
void addChild(DepartmentNode *parent, DepartmentNode *child)
{
    if (parent->numChildren < MAX_CHILDREN)
    {
        parent->children[parent->numChildren] = child;
        parent->numChildren++;
    }
    else
    {
        printf("�Ӳ���������������\n");
    }
}

// �ϲ��ӽڵ�
void merge(BTreeNode *node, int idx)
{
    BTreeNode *child = node->children[idx];
    BTreeNode *sibling = node->children[idx + 1];

    // �����ڵ��еļ��½����ӽڵ�
    child->users[NODEMIN] = node->users[idx];

    // ���ֵܽڵ��еļ����ӽڵ��Ƶ���ǰ�ӽڵ�
    for (int i = 0; i < sibling->count; i++)
    {
        child->users[i + NODEMIN + 1] = sibling->users[i];
    }
    if (!child->isLeaf)
    {
        for (int i = 0; i <= sibling->count; i++)
        {
            child->children[i + NODEMIN + 1] = sibling->children[i];
        }
    }

    // �ƶ����ڵ��еļ����ӽڵ�
    for (int i = idx; i < node->count - 1; i++)
    {
        node->users[i] = node->users[i + 1];
        node->children[i + 1] = node->children[i + 2];
    }

    child->count += sibling->count + 1;
    node->count--;

    free(sibling);
}

// �ӷ�Ҷ�ӽڵ�ɾ���û�
void deleteFromNonLeaf(BTreeNode *node, int idx, int (*compare)(User *, User *))
{
    User *user = node->users[idx];

    if (node->children[idx]->count >= NODEMIN + 1)
    {
        // �����������ֵ
        BTreeNode *current = node->children[idx];
        while (!current->isLeaf)
        {
            current = current->children[current->count];
        }
        node->users[idx] = current->users[current->count - 1];
        deleteUser(node->children[idx], current->users[current->count - 1], compare);
    }
    else if (node->children[idx + 1]->count >= NODEMIN + 1)
    {
        // ����������Сֵ
        BTreeNode *current = node->children[idx + 1];
        while (!current->isLeaf)
        {
            current = current->children[0];
        }
        node->users[idx] = current->users[0];
        deleteUser(node->children[idx + 1], current->users[0], compare);
    }
    else
    {
        merge(node, idx);
        deleteUser(node->children[idx], user, compare);
    }
}

// ��Ҷ�ӽڵ�ɾ���û�
void deleteFromLeaf(BTreeNode *node, int idx)
{
    for (int i = idx + 1; i < node->count; i++)
    {
        node->users[i - 1] = node->users[i];
    }
    node->count--;
}

void editDepartmentName()
{
    printf("������ԭ���������ƣ�");
    char departmentName[40];
    scanf("%s", departmentName);
    DepartmentNode *department = findDepartmentByName(departmentHead, departmentName);
    if (department != NULL)
    {
        printf("�ҵ�������Ϣ��\n");
        printDepartment(department);
        printf("�������µĲ�������: ");
        char name[40];
        scanf("%s", name);
        if (findDepartmentByName(departmentHead, name) != NULL)
        {
            printf("���������Ѵ��ڣ���������\n");
            return;
        }
        strcpy(department->name, name);
        printf("�޸ĳɹ�\n");
    }
    else
    {
        printf("δ�ҵ����Ż�Ȩ�޲���\n");
    }
}

// �޸Ĳ�����Ϣ���
void editDepartmentInfoPannel()
{
    printf("��������Ҫ���еĲ���:\n");
    printf("1. �޸Ĳ�������\n");
    printf("2. �޸Ĳ��Ź���Ա\n");
    printf("3. ����\n");
    int choice;
    printf("��ѡ��: ");
    scanf("%d", &choice);
    switch (choice)
    {
    case 1:
        // �޸Ĳ�������
        editDepartmentName();
        break;
    case 2:
        // �޸Ĳ��Ź���Ա
        editDepartmentLeader();
        break;
    case 3:
        // ����
        studentManagePannel();
        break;
    default:
        printf("�����������������\n");
        editDepartmentInfoPannel();
        break;
    }
}

// ���ֵܽڵ��е��û�
void borrowFromPrev(BTreeNode *node, int idx)
{
    BTreeNode *child = node->children[idx];
    BTreeNode *sibling = node->children[idx - 1];

    for (int i = child->count - 1; i >= 0; i--)
    {
        child->users[i + 1] = child->users[i];
    }
    if (!child->isLeaf)
    {
        for (int i = child->count; i >= 0; i--)
        {
            child->children[i + 1] = child->children[i];
        }
    }

    child->users[0] = node->users[idx - 1];
    if (!node->isLeaf)
    {
        child->children[0] = sibling->children[sibling->count];
    }

    node->users[idx - 1] = sibling->users[sibling->count - 1];
    child->count++;
    sibling->count--;
}

// ���ұ��ֵܽڵ���û�
void borrowFromNext(BTreeNode *node, int idx)
{
    BTreeNode *child = node->children[idx];
    BTreeNode *sibling = node->children[idx + 1];

    child->users[child->count] = node->users[idx];
    if (!child->isLeaf)
    {
        child->children[child->count + 1] = sibling->children[0];
    }

    node->users[idx] = sibling->users[0];
    for (int i = 1; i < sibling->count; i++)
    {
        sibling->users[i - 1] = sibling->users[i];
    }
    if (!sibling->isLeaf)
    {
        for (int i = 1; i <= sibling->count; i++)
        {
            sibling->children[i - 1] = sibling->children[i];
        }
    }

    child->count++;
    sibling->count--;
}

void printBTree(BTreeNode *node, int level)
{
    if (node == NULL)
    {
        return;
    }

    // ��ӡ��ǰ�ڵ�������û�����
    for (int i = 0; i < node->count; i++)
    {
        for (int j = 0; j < level; j++)
        {
            printf("\t"); // ���ݲ㼶����
        }
        int userIndex = node->users[i] - users; // ��ȡ�û���users�����е�����
        printf("In:%d", userIndex);
        printf("Index %d: Name: %s\n",
               i, node->users[i]->name); // ��ӡ�û���
    }

    // ��ӡ�ӽڵ�
    if (!node->isLeaf)
    {
        for (int i = 0; i <= node->count; i++)
        {
            printBTree(node->children[i], level + 1);
        }
    }
}

// ���ú�����ӡ����B��
void printBTreeRoot(BTreeNode *root)
{
    printf("Printing B-Tree:\n");
    printBTree(root, 0); // �Ӹ��ڵ㿪ʼ���㼶Ϊ0
}

// ɾ������
void deleteUser(BTreeNode *node, User *user, int (*compare)(User *, User *))
{
    int idx = 0;
    while (idx < node->count && compare(user, node->users[idx]) > 0)
    {
        idx++;
    }

    if (idx < node->count && compare(user, node->users[idx]) == 0)
    {
        if (node->isLeaf)
        {
            deleteFromLeaf(node, idx);
        }
        else
        {
            deleteFromNonLeaf(node, idx, compare);
        }
    }
    else
    {
        if (node->isLeaf)
        {
            // printf("�û�δ�ҵ�\n");
            return;
        }

        int flag = (idx == node->count);

        if (node->children[idx]->count < NODEMIN + 1)
        {
            if (idx != 0 && node->children[idx - 1]->count >= NODEMIN + 1)
            {
                borrowFromPrev(node, idx);
            }
            else if (idx != node->count && node->children[idx + 1]->count >= NODEMIN + 1)
            {
                borrowFromNext(node, idx);
            }
            else
            {
                if (idx != node->count)
                {
                    merge(node, idx);
                }
                else
                {
                    merge(node, idx - 1);
                }
            }
        }

        if (flag && idx > node->count)
        {
            deleteUser(node->children[idx - 1], user, compare);
        }
        else
        {
            deleteUser(node->children[idx], user, compare);
        }
    }
}

// �޸��û���Ϣ�ĺ���
void updateUserInfo(BTreeNode *anyRoot, User *oldUser, User *newUser, int (*compareBy)(User *, User *))
{
    // ɾ�����û���Ϣ
    deleteUser(anyRoot, oldUser, compareBy);
    // ������º���û���Ϣ
    insert(&anyRoot, newUser, compareBy);
}

// ��Department����ת��Ϊ����ڵ㲢��������
void convertDepartments(Department *departments, int departmentCount, DepartmentNode **head)
{
    // �������Žڵ㲢��������
    for (int i = 0; i < departmentCount; i++)
    {
        Department *dep = &departments[i];

        User queryUser;
        // ��ѧ�ž�׼�����û�
        strcpy(queryUser.id, dep->leaderId);
        User *leader = searchById(idRoot, &queryUser, compareByID);
        // �����µĲ��Žڵ�
        DepartmentNode *newNode = createDepartmentNode(dep->id, dep->name, leader);

        // ���벿�Žڵ㵽����ͷ��
        newNode->next = *head;
        *head = newNode;
    }

    // �ٴα��������ø����ź��Ӳ��Ź�ϵ
    DepartmentNode *current = *head;
    while (current != NULL)
    {
        // ���Ҹ�����
        for (int i = 0; i < departmentCount; i++)
        {
            if (strcmp(current->id, departments[i].id) == 0)
            {
                // ���ø�����
                if (strcmp(departments[i].parentId, "NULL") != 0)
                {
                    DepartmentNode *parent = findDepartmentById(*head, departments[i].parentId);
                    if (parent != NULL)
                    {
                        current->parent = parent;
                    }
                }

                // �����Ӳ���
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

// �ݹ��������������ӡ���ż����Ӳ���
void printDepartmentHierarchy(DepartmentNode *dept, int level) {
    if (dept == NULL) {
        return;
    }

    // ��ӡ��ǰ���ŵ����ƺͲ㼶
    for (int i = 0; i < level; i++) {
        printf("  ");  // ������ʾ�㼶
    }
    printf("��������: %s (����: %d)\n", dept->name, level);

    // �ݹ���������Ӳ���
    for (int i = 0; i < dept->numChildren; i++) {
        printDepartmentHierarchy(dept->children[i], level + 1);
    }
}

// ��ӡ������Ϣ
void printDepartment(DepartmentNode *department)
{
    if (department == NULL)
    {
        printf("��û�й�Ͻ�κβ���\n");
        return;
    }
    printf("����ID: %s\n", department->id);
    printf("��������: %s\n", department->name);

    // ��ӡ�����쵼��Ϣ
    if (department->leader != NULL)
    {
        printf("�����쵼����: %s\n", department->leader->name);
        printf("�����쵼ѧ��: %s\n", department->leader->id);
        printf("�����쵼����: %s\n", department->leader->email);
        printf("�����쵼�绰: %s\n", department->leader->phone);
    }
    else
    {
        printf("�����쵼: ��\n");
    }

    // ��ӡ��������Ϣ
    if (department->parent != NULL)
    {
        printf("������: %s\n", department->parent->name);
    }
    else
    {
        printf("������: ��\n");
    }

    // ��ӡ�Ӳ�����Ϣ
    printf("�Ӳ���:\n ");
    printDepartmentHierarchy(department, 1); // �ݹ��ӡ�Ӳ���
    printf("\n");
}

// ���Ҳ��Žڵ�ͨ����������
DepartmentNode *findDepartmentByName(DepartmentNode *head, const char *name)
{
    DepartmentNode *current = head;
    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0 && isDepartmentInControl(current))
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// ���Ҳ��Žڵ�ͨ������ID
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
        printf("�û���Ϣ�ļ���ʧ��\n");
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
        printf("������Ϣ�ļ���ʧ��\n");
        exit(1);
    }

    char line[256]; // ���ڴ洢ÿһ�еĶ�ȡ����
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        char childrenStr[200]; // ��ʱ�洢�Ӳ���ID���ַ���
        int numChildren = 0;
        sscanf(line, "%s %s %s %s %[^\n]", departments[*departmentCount].id, departments[*departmentCount].name, departments[*departmentCount].leaderId, departments[*departmentCount].parentId, childrenStr);

        // �����Ӳ���ID
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

        // ���û���Ӳ��ţ��� childrenId ��ʼ��Ϊ��
        for (int i = numChildren; i < MAX_DEPARTMENTS; i++)
        {
            strcpy(departments[*departmentCount].childrenId[i], "NULL");
        }

        (*departmentCount)++;
    }

    fclose(fp);
}

// B�������ڵ㺯��
BTreeNode *createNode(int isLeaf)
{
    BTreeNode *newNode = (BTreeNode *)malloc(sizeof(BTreeNode));
    if (newNode == NULL)
    {
        printf("�ڴ����ʧ��\n");
        exit(1);
    }
    newNode->isLeaf = isLeaf;
    newNode->count = 0;
    for (int i = 0; i < NODEMAX + 1; i++)
    {
        newNode->children[i] = NULL;
    }
    return newNode;
}

// �ж��û��Ƿ��ǲ����쵼
DepartmentNode *userDepartment(char *userId)
{
    DepartmentNode *current = departmentHead;
    while (current != NULL)
    {
        if (current->leader != NULL && strcmp(current->leader->id, userId) == 0)
        {
            return current; // �ҵ������쵼�����ظò��Žڵ�
        }
        current = current->next;
    }
    return NULL; // δ�ҵ������쵼�����ؿ�
}

// �����ӽڵ�
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

// �������ݵ������ڵ�
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

// ���뺯�������ڲ����û�����
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

// �ȽϺ���ʾ������ѧ������
int compareByID(User *user1, User *user2)
{
    return strcmp(user1->id, user2->id);
}

// �ȽϺ���ʾ��������������
int compareByName(User *user1, User *user2)
{
    return strcmp(user1->name, user2->name);
}

// �ȽϺ���ʾ�������ֻ�������
int compareByPhone(User *user1, User *user2)
{
    return strcmp(user1->phone, user2->phone);
}

// �жϲ����Ƿ��ܵ���ǰ�û�����
int isDepartmentInControl(DepartmentNode *department)
{
    // ���������鿴�����Ƿ��ǵ�ǰ�û�����Ͻ�Ĳ���
    DepartmentNode *current = department;
    DepartmentNode *target = currentDepartment;
    while (current != NULL)
    {
        if (strcmp(current->id, target->id) == 0)
        {
            return 1;
        }
        current = current->parent;
    }
    return 0;
}

// �ж��û��Ƿ��յ���ǰ�û�����
int isUserInControl(User *user)
{
    // ���������鿴�û����ڲ����Ƿ��ǵ�ǰ�û�����Ͻ�Ĳ���
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

// �����û����ݲ�����Userָ��
User *searchById(BTreeNode *node, User *user, int (*compare)(User *, User *))
{
    int i = 0;
    // �����ڵ�ǰ�ڵ��е�λ��
    while (i < node->count && compare(user, node->users[i]) > 0)
    {
        i++;
    }

    // ����ҵ����û�
    if (i < node->count && compare(user, node->users[i]) == 0)
    {
        return node->users[i]; // �����ҵ����û�ָ��
    }
    // �����Ҷ�ӽڵ���û���ҵ��û�
    else if (node->isLeaf)
    {
        return NULL; // �û�δ�ҵ�
    }
    // �������Ҷ�ӽڵ㣬�������ӽڵ��в���
    else
    {
        return searchById(node->children[i], user, compare);
    }
}

// �����û����ݲ�����UserNodeָ��
UserNode *searchNodeById(BTreeNode *node, User *user, int (*compare)(User *, User *))
{
    User *foundUser = searchById(node, user, compare);
    if (foundUser == NULL)
    {
        return NULL;
    }
    // �û����ٵ�ǰ�û����Ʒ�Χ�ڣ�����NULL
    if (!isUserInControl(foundUser))
    {
        return NULL;
    }
    int userIndex = foundUser - users;
    return &userNodes[userIndex];
}

// ��Χ���ң�����ѧ�ŷ�Χ�ڵ��û�
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
            printf("ѧ��: %s, ����: %s, ����: %s, �绰: %s, ����: %s, �Ա�: %s\n", user->id, user->name, user->email, user->phone, userNodes[userIndex].department->name, user->gender);
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

// ǰ׺ģ�����ң��û�ѧ��
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
            printf("ѧ��: %s, ����: %s, ����: %s, �绰: %s, ����: %s, �Ա�: %s\n", user->id, user->name, user->email, user->phone, userNodes[userIndex].department->name, user->gender);
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

// ��Χ���ң�����������Χ�ڵ��û�
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
            printf("ѧ��: %s, ����: %s, ����: %s, �绰: %s, ����: %s, �Ա�: %s\n", user->id, user->name, user->email, user->phone, userNodes[userIndex].department->name, user->gender);
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

// ǰ׺ģ�����ң��û�����
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
            printf("ѧ��: %s, ����: %s, ����: %s, �绰: %s, ����: %s, �Ա�: %s\n", user->id, user->name, user->email, user->phone, userNodes[userIndex].department->name, user->gender);
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
// ��Χ���ң������ֻ��ŷ�Χ�ڵ��û�
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
            printf("ѧ��: %s, ����: %s, ����: %s, �绰: %s, ����: %s, �Ա�: %s\n", user->id, user->name, user->email, user->phone, userNodes[userIndex].department == NULL ? "��" : userNodes[userIndex].department->name, user->gender);
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
// ǰ׺ģ�����ң��û��ֻ���
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
            printf("ѧ��: %s, ����: %s, ����: %s, �绰: %s, ����: %s, �Ա�: %s\n", user->id, user->name, user->email, user->phone, userNodes[userIndex].department == NULL ? "��" : userNodes[userIndex].department->name, user->gender);
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

// �������
void searchPannel()
{
    printf("��ѡ��������ʽ��\n");
    printf("1. ��ѧ������\n");
    printf("2. ����������\n");
    printf("3. ���ֻ�������\n");
    printf("4. ������һ��\n");
    printf("��ѡ��");
    int choice;
    scanf("%d", &choice);
    if (choice != 4)
    {
        printf("��ѡ��������ʽ��\n");
        printf("1. ��ȷ����\n");
        printf("2. ��Χ����\n");
        printf("3. ǰ׺ģ������\n");
        printf("��ѡ��");
        int searchChoice;
        scanf("%d", &searchChoice);
        choice = choice * 10 + searchChoice;
        char prefix[40]; // ǰ׺����
        User queryUser;  // ��ѯ�û�
        switch (choice)
        {
        case 11:
            // ��ѧ�ž�ȷ����
            char id[20];
            printf("������ѧ�ţ�");
            scanf("%s", id);
            // ��ѧ�ž�׼�����û�
            strcpy(queryUser.id, id);
            printUser(searchNodeById(idRoot, &queryUser, compareByID));
            break;
        case 12:
            // ��ѧ�ŷ�Χ����
            char startID[20], endID[20];
            printf("������ѧ�ŷ�Χ����С�Ҵ��ÿո�ָ�\n");
            scanf("%s %s", startID, endID);
            searchByIDRange(idRoot, startID, endID);
            break;
        case 13:
            // ��ѧ��ǰ׺ģ������
            printf("������ѧ��ǰ׺��");
            scanf("%s", prefix);
            searchByIDPrefix(idRoot, prefix);
            break;
        case 21:
            // ��������ȷ����
            char name[40];
            printf("������������");
            scanf("%s", name);
            // ��������׼�����û�
            strcpy(queryUser.name, name);
            printUser(searchNodeById(nameRoot, &queryUser, compareByName));
            break;
        case 22:
            // ��������Χ����
            char startName[40], endName[40];
            printf("������������Χ����С�Ҵ��ÿո�ָ�\n");
            scanf("%s %s", startName, endName);
            searchByNameRange(nameRoot, startName, endName);
            break;
        case 23:
            // ������ǰ׺ģ������
            printf("����������ǰ׺��");
            scanf("%s", prefix);
            searchByNamePrefix(nameRoot, prefix);
            break;
        case 31:
            // ���ֻ��ž�ȷ����
            char phone[20];
            printf("�������ֻ��ţ�");
            scanf("%s", phone);
            // ���ֻ��ž�׼�����û�
            strcpy(queryUser.phone, phone);
            printUser(searchNodeById(phoneRoot, &queryUser, compareByPhone));
            break;
        case 32:
            // ���ֻ��ŷ�Χ����
            char startPhone[20], endPhone[20];
            printf("�������ֻ��ŷ�Χ����С�Ҵ��ÿո�ָ�\n");
            scanf("%s %s", startPhone, endPhone);
            searchByPhoneRange(phoneRoot, startPhone, endPhone);
            break;
        case 33:
            // ���ֻ���ǰ׺ģ������
            printf("�������ֻ���ǰ׺��");
            scanf("%s", prefix);
            searchByPhonePrefix(phoneRoot, prefix);
            break;
        default:
            printf("�������\n");
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

// ��¼���
void defaultPannel()
{
    system("cls");
    printf("************************************************\n");
    printf("��ӭ��¼ѧ����Ϣ����ϵͳ!��ѡ��\n");
    printf("1. ��¼\n");
    printf("2. �˳�ϵͳ\n");
    printf("************************************************\n");
    int choice;
    while (1)
    {
        printf("��ѡ��");
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            // ��¼
            loginPannel();
            break;
        case 2:
            // ������һ��
            printf("�˳�ϵͳ\n");
            exit(0);
            break;
        default:
            printf("�������\n");
            break;
        }
    }
}

// ѧ��������Ϣ�༭���
void studentSelfManagePannel()
{
    system("cls");
    printf("��½�ɹ�\n");
    printf("************************************************\n");
    // ��ӡ�û�����
    while (1)
    {
        printf("���ã�%s����ѡ��\n", currentUser->name);
        printf("1. �鿴������Ϣ\n");
        printf("2. �޸ĸ�����Ϣ\n");
        printf("3. ���Ÿ����˷�����Ϣ\n");
        printf("4. �鿴��Ϣ�б�\n");
        printf("5. �鿴������Ϣ\n");
        printf("6. ������һ��\n");
        printf("************************************************\n");
        printf("��ѡ��");
        int choice;
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            // �鿴������Ϣ
            printSelfInfo();
            break;
        case 2:
            editSelfInfo();
            // �޸ĸ�����Ϣ
            break;
        case 3:
            // ���Ÿ����˷�����Ϣ
            sendMessageToLeader();
            break;
        case 4:
            // �鿴��Ϣ�б�
            printMessageReceived();
            break;
        case 5:
            // �鿴������Ϣ
            int userIndex = currentUser - users;
            printDepartment(userNodes[userIndex].department);
            break;
        case 6:
            // ������һ��
            defaultPannel();
            break;
        default:
            printf("�������\n");
            break;
        }
    }
}

// ѧ���������
void studentManagePannel()
{
    printf("************************************************\n");
    // ��ӡ�û�����
    printf("���ã�%s����ѡ��\n", currentUser->name);
    printf("1. �鿴������Ϣ\n");
    printf("2. �޸ĸ�����Ϣ\n");
    printf("3. ���Ÿ����˷�����Ϣ\n");
    printf("4. �鿴��Ϣ�б�\n");
    printf("5. ���ض��û�������Ϣ\n");
    printf("6. ������Ͻ���û�Ⱥ����Ϣ\n");
    printf("7. ��ѯ�û���Ϣ\n");
    printf("8. �޸��û���Ϣ\n");
    printf("9. �޸Ĳ�����Ϣ\n");
    printf("10. �鿴�Լ���Ͻ������Ϣ\n");
    printf("11. �鿴�Լ�����������Ϣ\n");
    printf("12. ������һ��\n");
    printf("************************************************\n");
    printf("��ѡ��");
    int choice;
    scanf("%d", &choice);
    switch (choice)
    {
    case 1:
        // �鿴������Ϣ
        printSelfInfo();
        break;
    case 2:
        // �޸ĸ�����Ϣ
        editSelfInfo();
        break;
    case 3:
        // ���Ÿ����˷�����Ϣ
        sendMessageToLeader();
        break;
    case 4:
        // �鿴��Ϣ�б�
        printMessageReceived();
        break;
    case 5:
        // ���ض��û�������Ϣ
        sendMessageToTargetUser();
        break;
    case 6:
        // ���û�Ⱥ����Ϣ
        pushGroupMessage();
        break;
    case 7:
        // ��ѯ�û���Ϣ
        searchPannel();
        break;
    case 8:
        // �޸��û���Ϣ
        editStudentInfoPannel();
        break;
    case 9:
        // �޸Ĳ�����Ϣ
        editDepartmentInfoPannel();
        break;
    case 10:
        // �鿴�Լ����Թ���Ĳ���
        printDepartment(currentDepartment);
        break;
    case 11:
        // �鿴�Լ�����������Ϣ
        int userIndex = currentUser - users;
        printDepartment(userNodes[userIndex].department);
        break;
    case 12:
        // ������һ��
        defaultPannel();
        break;
    default:
        printf("�������\n");
        break;
    }
    studentManagePannel();
}

// ��¼���
void loginPannel()
{
    printf("����������ѧ�ţ�");
    char id[20];
    scanf("%s", id);
    // �����û�
    User queryUser;
    // ��ѧ�ž�׼�����û�
    strcpy(queryUser.id, id);
    User *foundUser = searchById(idRoot, &queryUser, compareByID);
    currentDepartment = NULL; // ���õ�ǰ����
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

    printf("�û������ڣ�\n");
    loginPannel();
}

// ��ʼ������
void init(User users[], Department departments[], UserNode userNodes[], int *userCount, int *departmentCount)
{
    loadUsers(users, userCount);
    for (int i = 0; i < *userCount; i++)
    {
        insert(&idRoot, &users[i], compareByID);       // ��ӵ�ѧ����
        insert(&nameRoot, &users[i], compareByName);   // ��ӵ�������
        insert(&phoneRoot, &users[i], compareByPhone); // ��ӵ��绰��
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
        userNodes[i].messages = NULL; // ��ʼ����Ϣ����Ϊ��
    }
}

int main()
{
    init(users, departments, userNodes, &userCount, &departmentCount);
    defaultPannel();
    return 0;
}