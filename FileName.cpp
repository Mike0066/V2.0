//学生
//王 12345 123456
//李 111111 111111
//老师
//张 123456 123456
//管理员
//123456 123456
#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include <windows.h>
#include <conio.h>

using namespace std;

#define MAX_STUDENTS 1000   // 最大学生数量
#define MAX_COURSES 8       // 最大课程数量
#define MAX_NAME_LEN 50     // 姓名最大长度
#define MAX_ID_LEN 10       // 学号最大长度
#define MAX_CLASS_LEN 30    // 班级最大长度
#define PASSWORD_LEN 10         // 密码长度
#define PWD_MASK '*'
#define STUDENT_FILE "students.bin"
#define TEACHER_FILE "teachers.bin"
#define ADMIN_FILE "admins.bin"
void loadStudentsToLinkList();
//////////////////////////////////////////////////////////////////////////////////////
//隐藏式输入密码，输入结果存入pwd，max_len为密码最大长度
void inputPassword(char* pwd, int max_len) {
    int i = 0;
    char ch;
    memset(pwd, 0, max_len);  // 清空密码数组
    printf("请输入密码：");
    while (1) {
        ch = _getch();  // 无回显获取按键
        if (ch == '\r' || ch == '\n') {  // 按回车结束输入
            break;
        }
        else if (ch == '\b' && i > 0) {  // 按退格键删除
            printf("\b \b");  // 删掉屏幕上的*
            i--;
        }
        else if (i < max_len - 1) {  // 正常输入字符
            pwd[i++] = ch;
            printf("%c", PWD_MASK);    // 显示*
        }
    }
    printf("\n");
}

// 清空输入缓冲区
void clearInputBuffer() {
    while (getchar() != '\n'); // 读取并丢弃缓冲区所有字符，直到回车
}
// 判断字符串是否为数字（用于学号/成绩验证）
int isNumber(char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i]))
            return 0;
    }
    return 1;
}
//////////////////////////////////////////////////////////////////////////////////////////

typedef struct Student {
    char id[MAX_ID_LEN];           //学号
    char password[PASSWORD_LEN];   //密码
    char name[MAX_NAME_LEN];       //姓名
    char classname[MAX_CLASS_LEN]; //班级
    int age;                       //年龄
    char sex;                   //性别(M/F)
    int chinese;                //语文成绩
    int math;                   //数学成绩
    int english;                //英语成绩
    int physics;                //物理成绩
    int chemistry;              //化学成绩
    int biology;                //生物成绩
    float total_score;           //总分
    int rank;                    //排名
    struct Student* next;
};
Student loginStudent;
// 计算总分
void calculateTotalScore(Student* stu) {
    stu->total_score = (float)(stu->chinese + stu->math + stu->english +
        stu->physics + stu->chemistry + stu->biology);
}

// 学生登录验证：返回1表示登录成功，0失败，-1文件错误
int studentLogin(char* inputId, char* inputPwd) {
    FILE* fp = fopen(STUDENT_FILE, "rb");
    if (fp == NULL) {
        printf("账号文件不存在或打开失败！\n");
        return -1;
    }
    Student stu;
    // 循环读取文件中的每个学生信息
    while (fread(&stu, sizeof(Student), 1, fp) == 1) {
        // 比对学号和密码
        if (strcmp(stu.id, inputId) == 0 && strcmp(stu.password, inputPwd) == 0) {
            // 新增：将登录学生信息赋值给全局变量
            memcpy(&loginStudent, &stu, sizeof(Student));
            // 新增：加载学生链表（确保成绩数据可用）
            loadStudentsToLinkList();
            fclose(fp);
            printf("登录成功！欢迎你，%s同学\n", stu.name);
            return 1;
        }
    }
    fclose(fp);
    printf("账号或密码错误！\n");
    return 0;
}

// 学生账号注册：返回1成功，0失败（账号已存在），-1文件错误
int studentRegister() {
    Student newStu;
    FILE* fp = fopen(STUDENT_FILE, "ab+");  // 追加+二进制模式
    if (fp == NULL) {
        printf("文件打开失败！\n");
        return -1;
    }
    // 1. 输入账号（学号）并检查是否已存在
    printf("请输入学号（账号）：");
    scanf("%s", newStu.id);
    // 先检查账号是否重复
    Student temp;
    fseek(fp, 0, SEEK_SET);  // 回到文件开头
    while (fread(&temp, sizeof(Student), 1, fp) == 1) {
        if (strcmp(temp.id, newStu.id) == 0) {
            fclose(fp);
            printf("该学号已注册！\n");
            return 0;
        }
    }
    // 2. 输入其他信息
    printf("请输入姓名：");
    scanf("%s", newStu.name);
    printf("请输入班级（如：高一(1)班）：");
    scanf("%s", newStu.classname);
    printf("请输入年龄：");
    scanf("%d", &newStu.age);
    printf("请输入性别（M/F）：");
    scanf(" %c", &newStu.sex);
    newStu.chinese = 0;
    newStu.math = 0;
    newStu.english = 0;
    newStu.physics = 0;
    newStu.chemistry = 0;
    newStu.biology = 0;
    // 计算总分（初始为0）
    calculateTotalScore(&newStu);
    newStu.rank = 0;
    newStu.next = NULL;
    // 3. 隐藏输入密码
    char pwd1[20], pwd2[20];
    inputPassword(pwd1, 20);
    printf("请再次输入密码：");
    inputPassword(pwd2, 20);
    if (strcmp(pwd1, pwd2) != 0) {
        fclose(fp);
        printf("两次密码输入不一致！\n");
        return 0;
    }
    strcpy(newStu.password, pwd1);

    // 4. 写入文件
    fwrite(&newStu, sizeof(Student), 1, fp);
    fclose(fp);
    printf("注册成功！\n");
    return 1;
}

// 修改学生密码：输入原账号和原密码，验证通过后修改
int changeStudentPwd(char* inputId, char* oldPwd) {
    FILE* fp;
    errno_t err = fopen_s(&fp, STUDENT_FILE, "rb+");
    if (err != 0 || fp == NULL) {
        printf("文件打开失败！\n");
        return -1;
    }

    Student stu;
    int found = 0;
    while (fread(&stu, sizeof(Student), 1, fp) == 1) {
        if (strcmp(stu.id, inputId) == 0 && strcmp(stu.password, oldPwd) == 0){
            found = 1;
            char newPwd1[20], newPwd2[20];
            printf("请输入新密码：");
            inputPassword(newPwd1, 20);
            printf("请再次输入新密码：");
            inputPassword(newPwd2, 20);
            if (strcmp(newPwd1, newPwd2) != 0) {
                fclose(fp);
                printf("两次密码不一致！\n");
                return 0;
            }
            fseek(fp, -(long)sizeof(Student), SEEK_CUR);
            strcpy(stu.password, newPwd1);
            fwrite(&stu, sizeof(Student), 1, fp);
            break;
        }
    }
    fclose(fp);
    if (!found) {
        printf("账号或原密码错误！\n");
        return 0;
    }
    printf("密码修改成功！\n");
    return 1;
}

Student* stuListHead = NULL;    // 学生链表头节点

//查询个人成绩
void queryMyScore(){
    Student* temp = stuListHead;
    while (temp != NULL) {
        if (strcmp(temp->id, loginStudent.id) == 0) {
            printf("\n================== 个人成绩详情 =================\n");
            printf("学号：%s\t姓名：%s\t班级：%s\n", temp->id, temp->name, temp->classname);
            printf("年龄：%d\t性别：%c\n", temp->age, temp->sex);
            printf("-------------------------------------------------\n");
            printf("语文：%d\t数学：%d\t英语：%d\n", temp->chinese, temp->math, temp->english);
            printf("物理：%d\t化学：%d\t生物：%d\n", temp->physics, temp->chemistry, temp->biology);
            printf("-------------------------------------------------\n");
            printf("总分：%.1f\t班级排名：第%d名\n", temp->total_score, temp->rank);
            return;
        }
        temp = temp->next;
    }
    printf("暂无你的成绩数据！\n");
}

// 按班级分组计算排名（每个班级独立排名）
void sortStudentByTotalScoreByClass() {
    if (stuListHead == NULL) return;

    // 1. 先按班级分组，存储每个班级的学生链表
    char currentClass[MAX_CLASS_LEN];
    Student* classHead = NULL; // 临时存储当前班级的学生

    Student* temp = stuListHead;
    while (temp != NULL) {
        strcpy(currentClass, temp->classname);
        // 提取当前班级的所有学生到classHead
        classHead = NULL;
        Student* classTail = NULL;
        Student* p = stuListHead;
        while (p != NULL) {
            if (strcmp(p->classname, currentClass) == 0) {
                Student* newNode = (Student*)malloc(sizeof(Student));
                memcpy(newNode, p, sizeof(Student));
                newNode->next = NULL;
                if (classHead == NULL) {
                    classHead = newNode;
                    classTail = newNode;
                }
                else {
                    classTail->next = newNode;
                    classTail = newNode;
                }
            }
            p = p->next;
        }

        // 2. 对当前班级的学生排序（降序）
        if (classHead != NULL && classHead->next != NULL){
            int swapped;
            Student* a;
            Student* b = NULL;
            do {
                swapped = 0;
                a = classHead;
                while (a->next != b){
                    if (a->total_score < a->next->total_score) {
                        // 交换两个节点的数据
                        char tempId[MAX_ID_LEN], tempPwd[PASSWORD_LEN], tempName[MAX_NAME_LEN], tempClass[MAX_CLASS_LEN];
                        int tempAge;
                        char tempSex;
                        int tempChinese, tempMath, tempEnglish, tempPhysics, tempChemistry, tempBiology;
                        float tempTotal;

                        strcpy(tempId, a->id); strcpy(a->id, a->next->id); strcpy(a->next->id, tempId);
                        strcpy(tempPwd, a->password); strcpy(a->password, a->next->password); strcpy(a->next->password, tempPwd);
                        strcpy(tempName, a->name); strcpy(a->name, a->next->name); strcpy(a->next->name, tempName);
                        strcpy(tempClass, a->classname); strcpy(a->classname, a->next->classname); strcpy(a->next->classname, tempClass);
                        tempAge = a->age; a->age = a->next->age; a->next->age = tempAge;
                        tempSex = a->sex; a->sex = a->next->sex; a->next->sex = tempSex;

                        tempChinese = a->chinese; a->chinese = a->next->chinese; a->next->chinese = tempChinese;
                        tempMath = a->math; a->math = a->next->math; a->next->math = tempMath;
                        tempEnglish = a->english; a->english = a->next->english; a->next->english = tempEnglish;
                        tempPhysics = a->physics; a->physics = a->next->physics; a->next->physics = tempPhysics;
                        tempChemistry = a->chemistry; a->chemistry = a->next->chemistry; a->next->chemistry = tempChemistry;
                        tempBiology = a->biology; a->biology = a->next->biology; a->next->biology = tempBiology;
                        tempTotal = a->total_score; a->total_score = a->next->total_score; a->next->total_score = tempTotal;

                        swapped = 1;
                    }
                    a = a->next;
                }
                b = a;
            } while (swapped);

            // 3. 更新当前班级学生的排名（仅在本班内排名）
            int rank = 1;
            Student* classTemp = classHead;
            while (classTemp != NULL) {
                // 找到原链表中对应的学生，更新排名
                Student* origin = stuListHead;
                while (origin != NULL) {
                    if (strcmp(origin->id, classTemp->id) == 0) {
                        origin->rank = rank;
                        break;
                    }
                    origin = origin->next;
                }
                rank++;
                classTemp = classTemp->next;
            }

            // 释放临时班级链表内存
            classTemp = classHead;
            while (classTemp != NULL) {
                Student* next = classTemp->next;
                free(classTemp);
                classTemp = next;
            }
        }

        // 跳到下一个不同的班级（避免重复处理）
        while (temp != NULL && strcmp(temp->classname, currentClass) == 0) {
            temp = temp->next;
        }
    }
}

// 查询本班成绩
void queryClassScore() {
    if (stuListHead == NULL) {
        printf("暂无学生成绩数据！\n");
        return;
    }
    sortStudentByTotalScoreByClass();
    printf("\n========================= 本班（%s）成绩排名 ========================\n", loginStudent.classname);
    printf("排名\t学号\t姓名\t语文\t数学\t英语\t物理\t化学\t生物\t总分\n");
    printf("==============================================================================\n");

    Student* temp = stuListHead;
    int hasClassData = 0;
    while (temp != NULL) {
        if (strcmp(temp->classname, loginStudent.classname) == 0) {
            hasClassData = 1;
            printf("%d\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%.1f\n",
                temp->rank, temp->id, temp->name,
                temp->chinese, temp->math, temp->english,
                temp->physics, temp->chemistry, temp->biology,
                temp->total_score);
        }
        temp = temp->next;
    }
    if (!hasClassData) {
        printf("本班暂无成绩数据！\n");
    }
}

// 成绩分析
void analyzeMyScore() {
    Student* myScore = NULL;
    Student* temp = stuListHead;
    while (temp != NULL) {
        if (strcmp(temp->id, loginStudent.id) == 0) {
            myScore = temp;
            break;
        }
        temp = temp->next;
    }
    if (myScore == NULL) {
        printf("暂无你的成绩数据，无法分析！\n");
        return;
    }
    // 统计本班数据
    temp = stuListHead;
    int classCount = 0;
    int classChinese = 0, classMath = 0, classEnglish = 0;
    int classPhysics = 0, classChemistry = 0, classBiology = 0;
    float classTotal = 0;
    while (temp != NULL) {
        if (strcmp(temp->classname, loginStudent.classname) == 0) {
            classCount++;
            classChinese += temp->chinese;
            classMath += temp->math;
            classEnglish += temp->english;
            classPhysics += temp->physics;
            classChemistry += temp->chemistry;
            classBiology += temp->biology;
            classTotal += temp->total_score;
        }
        temp = temp->next;
    }
    if (classCount == 0) {
        printf("本班暂无成绩数据，无法分析！\n");
        return;
    }
    // 计算平均分
    float avgChinese = (float)classChinese / classCount;
    float avgMath = (float)classMath / classCount;
    float avgEnglish = (float)classEnglish / classCount;
    float avgPhysics = (float)classPhysics / classCount;
    float avgChemistry = (float)classChemistry / classCount;
    float avgBiology = (float)classBiology / classCount;
    float avgTotal = classTotal / classCount;
    // 展示分析报告
    printf("\n====================== 成绩分析报告 ======================\n");
    printf("1. 基础信息：\n");
    printf("   姓名：%s\t班级：%s\t本班总人数：%d\n", myScore->name, myScore->classname, classCount);
    printf("   总分排名：第%d名\t总分：%.1f\t班级平均分：%.1f\n", myScore->rank, myScore->total_score, avgTotal);
    printf("\n2. 各科成绩与班级平均分对比：\n");
    printf("   科目\t你的成绩\t班级平均分\t差距（高/低）\n");
    printf("   语文\t%d\t\t%.1f\t\t%.1f\n", myScore->chinese, avgChinese, myScore->chinese - avgChinese);
    printf("   数学\t%d\t\t%.1f\t\t%.1f\n", myScore->math, avgMath, myScore->math - avgMath);
    printf("   英语\t%d\t\t%.1f\t\t%.1f\n", myScore->english, avgEnglish, myScore->english - avgEnglish);
    printf("   物理\t%d\t\t%.1f\t\t%.1f\n", myScore->physics, avgPhysics, myScore->physics - avgPhysics);
    printf("   化学\t%d\t\t%.1f\t\t%.1f\n", myScore->chemistry, avgChemistry, myScore->chemistry - avgChemistry);
    printf("   生物\t%d\t\t%.1f\t\t%.1f\n", myScore->biology, avgBiology, myScore->biology - avgBiology);
    // 薄弱科目
    printf("\n3. 薄弱科目建议（低于班级平均分）：\n");
    int hasWeak = 0;
    if (myScore->chinese < avgChinese) {
        printf("   语文（低于平均分%.1f分）\n", avgChinese - myScore->chinese);
        hasWeak = 1;
    }
    if (myScore->math < avgMath) {
        printf("   数学（低于平均分%.1f分）\n", avgMath - myScore->math);
        hasWeak = 1;
    }
    if (myScore->english < avgEnglish) {
        printf("   英语（低于平均分%.1f分）\n", avgEnglish - myScore->english);
        hasWeak = 1;
    }
    if (myScore->physics < avgPhysics) {
        printf("   物理（低于平均分%.1f分）\n", avgPhysics - myScore->physics);
        hasWeak = 1;
    }
    if (myScore->chemistry < avgChemistry) {
        printf("   化学（低于平均分%.1f分）\n", avgChemistry - myScore->chemistry);
        hasWeak = 1;
    }
    if (myScore->biology < avgBiology) {
        printf("   生物（低于平均分%.1f分）\n", avgBiology - myScore->biology);
        hasWeak = 1;
    }
    if (!hasWeak) {
        printf("   恭喜！你的所有科目均高于班级平均分！\n");
    }
}

// 从文件加载所有学生数据到链表（核心修复函数）
void loadStudentsToLinkList() {
    // 1. 先清空原有链表，避免重复加载
    Student* temp = stuListHead;
    while (temp != NULL) {
        Student* next = temp->next;
        free(temp);
        temp = next;
    }
    stuListHead = NULL;

    // 2. 打开学生数据文件
    FILE* fp = fopen(STUDENT_FILE, "rb");
    if (fp == NULL) {
        // 文件不存在时创建空文件，避免后续操作报错
        fp = fopen(STUDENT_FILE, "wb");
        if (fp == NULL) {
            printf("学生数据文件创建失败！\n");
            return;
        }
        fclose(fp);
        return;
    }

    // 3. 循环读取文件中的学生数据，构建链表
    Student* tail = NULL; // 链表尾指针（用于尾插法）
    Student stuBuf;       // 临时存储读取的学生数据

    while (fread(&stuBuf, sizeof(Student), 1, fp) == 1){
        // 为每个学生节点分配内存
        Student* newNode = (Student*)malloc(sizeof(Student));
        if (newNode == NULL) {
            printf("内存分配失败！\n");
            break;
        }

        // 复制文件中的学生数据到新节点
        memcpy(newNode, &stuBuf, sizeof(Student));
        newNode->next = NULL; // 尾节点next置空

        // 尾插法添加到链表
        if (stuListHead == NULL) {
            stuListHead = newNode; // 链表为空时，头节点=新节点
            tail = newNode;
        }
        else {
            tail->next = newNode; // 链表非空时，尾节点指向新节点
            tail = newNode;
        }
    }

    fclose(fp);
    // 加载完成后自动排序，保证排名最新
    sortStudentByTotalScoreByClass();
}

///////////////////////////////////////////////////////////////////////////////////////
// 教师账号文件路径

typedef struct Teacher {
    char id[MAX_ID_LEN];           //工号
    char password[PASSWORD_LEN];   //密码
    char name[MAX_NAME_LEN];       //姓名
    char manageClass[MAX_CLASS_LEN]; //班级
    int age;                       //年龄
    char sex;                   //性别(M/F)
};

// 教师登录验证：返回1成功，0失败，-1文件错误
int teacherLogin(char* inputId, char* inputPwd) {
    FILE* fp;
    errno_t err = fopen_s(&fp, TEACHER_FILE, "rb");
    if (err != 0 || fp == NULL) {
        printf("教师账号文件不存在或打开失败！\n");
        return -1;
    }
    Teacher tea;
    // 循环读取每个教师信息
    while (fread(&tea, sizeof(Teacher), 1, fp) == 1) {
        // 比对工号和密码
        if (strcmp(tea.id, inputId) == 0 && strcmp(tea.password, inputPwd) == 0) {
            fclose(fp);
            printf("登录成功！欢迎你，%s老师\n", tea.name);
            // 额外返回教师管理的班级信息（可选）
            printf("你管理的班级ID：%d\n", tea.manageClass);
            return 1;
        }
    }
    fclose(fp);
    printf("教师工号或密码错误！\n");
    return 0;
}

// 教师账号注册：返回1成功，0失败（账号已存在），-1文件错误
int teacherRegister() {
    Teacher newTea;
    FILE* fp;
    errno_t err = fopen_s(&fp, TEACHER_FILE, "ab+");  // 追加+二进制模式
    if (err != 0 || fp == NULL) {
        printf("文件打开失败！\n");
        return -1;
    }
    // 1. 输入工号并检查是否重复
    printf("请输入教师工号（账号）：");
    scanf("%s", newTea.id);
    Teacher temp;
    fseek(fp, 0, SEEK_SET);  // 回到文件开头
    while (fread(&temp, sizeof(Teacher), 1, fp) == 1) {
        if (strcmp(temp.id, newTea.id) == 0) {
            fclose(fp);
            printf("该教师工号已注册！\n");
            return 0;
        }
    }

    // 2. 输入教师信息
    printf("请输入教师姓名：");
    scanf("%s", newTea.name);
    printf("请输入你管理的班级ID：");
    scanf("%d", &newTea.manageClass);

    // 3. 隐藏输入密码（两次确认）
    char pwd1[20], pwd2[20];
    inputPassword(pwd1, 20);
    printf("请再次输入密码：");
    inputPassword(pwd2, 20);
    if (strcmp(pwd1, pwd2) != 0) {
        fclose(fp);
        printf("两次密码输入不一致！\n");
        return 0;
    }
    strcpy(newTea.password, pwd1);

    // 4. 写入文件
    fwrite(&newTea, sizeof(Teacher), 1, fp);
    fclose(fp);
    printf("教师账号注册成功！\n");
    return 1;
}

// 修改教师密码：输入原工号和原密码，验证通过后修改
int changeTeacherPwd(char* inputId, char* oldPwd) {
    FILE* fp;
    errno_t err = fopen_s(&fp, TEACHER_FILE, "rb+");  // 读写模式
    if (err != 0 || fp == NULL) {
        printf("文件打开失败！\n");
        return -1;
    }
    Teacher tea;
    int found = 0;
    while (fread(&tea, sizeof(Teacher), 1, fp) == 1) {
        if (strcmp(tea.id, inputId) == 0 && strcmp(tea.password, oldPwd) == 0) {
            found = 1;
            // 输入新密码
            char newPwd1[20], newPwd2[20];
            printf("请输入新密码：");
            inputPassword(newPwd1, 20);
            printf("请再次输入新密码：");
            inputPassword(newPwd2, 20);
            if (strcmp(newPwd1, newPwd2) != 0) {
                fclose(fp);
                printf("两次密码不一致！\n");
                return 0;
            }
            // 修复fseek类型警告：显式转换为long
            fseek(fp, -(long)sizeof(Teacher), SEEK_CUR);
            strcpy(tea.password, newPwd1);
            fwrite(&tea, sizeof(Teacher), 1, fp);
            break;
        }
    }
    fclose(fp);
    if (!found) {
        printf("教师工号或原密码错误！\n");
        return 0;
    }
    printf("密码修改成功！\n");
    return 1;
}
///////////////////////////////////////////////////////////////////////////////////////////////

typedef struct Admin {
    char id[MAX_ID_LEN];           //学号
    char password[PASSWORD_LEN];   //密码
    char name[MAX_NAME_LEN];       //姓名
};

// 管理员登录验证：返回1成功，0失败，-1文件错误
int adminLogin(char* inputId, char* inputPwd) {
    FILE* fp;
    errno_t err = fopen_s(&fp, ADMIN_FILE, "rb");
    if (err != 0 || fp == NULL) {
        printf("管理员账号文件不存在或打开失败！\n");
        return -1;
    }
    Admin admin;
    while (fread(&admin, sizeof(Admin), 1, fp) == 1) {
        // 比对管理员工号和密码
        if (strcmp(admin.id, inputId) == 0 && strcmp(admin.password, inputPwd) == 0) {
            fclose(fp);
            printf("登录成功！欢迎进入管理员后台系统\n");
            printf("【提示】你拥有最高权限，可管理所有账号和成绩数据\n");
            return 1;
        }
    }
    fclose(fp);
    printf("管理员工号或密码错误！\n");
    return 0;
}

// 管理员账号注册：返回1成功，0失败（账号已存在），-1文件错误
// 注：实际项目中管理员账号通常由超级管理员创建，此函数仅用于测试
int adminRegister() {
    Admin newAdmin;
    FILE* fp;
    errno_t err = fopen_s(&fp, ADMIN_FILE, "ab+");
    if (err != 0 || fp == NULL) {
        printf("文件打开失败！\n");
        return -1;
    }
    // 1. 输入管理员工号并检查重复
    printf("请输入管理员工号（账号）：");
    scanf("%s", newAdmin.id);
    Admin temp;
    fseek(fp, 0, SEEK_SET);
    while (fread(&temp, sizeof(Admin), 1, fp) == 1) {
        if (strcmp(temp.id, newAdmin.id) == 0) {
            fclose(fp);
            printf("该管理员工号已注册！\n");
            return 0;
        }
    }
    // 2. 隐藏输入密码（两次确认）
    char pwd1[20], pwd2[20];
    inputPassword(pwd1, 20);
    printf("请再次输入密码：");
    inputPassword(pwd2, 20);
    if (strcmp(pwd1, pwd2) != 0) {
        fclose(fp);
        printf("两次密码输入不一致！\n");
        return 0;
    }
    strcpy(newAdmin.password, pwd1);
    // 3. 写入文件
    fwrite(&newAdmin, sizeof(Admin), 1, fp);
    fclose(fp);
    printf("管理员账号注册成功！\n");
    return 1;
}

// 修改管理员密码：输入原工号和原密码，验证通过后修改
int changeAdminPwd(char* inputId, char* oldPwd) {
    FILE* fp;
    errno_t err = fopen_s(&fp, ADMIN_FILE, "rb+");
    if (err != 0 || fp == NULL) {
        printf("文件打开失败！\n");
        return -1;
    }
    Admin admin;
    int found = 0;
    while (fread(&admin, sizeof(Admin), 1, fp) == 1) {
        if (strcmp(admin.id, inputId) == 0 && strcmp(admin.password, oldPwd) == 0) {
            found = 1;
            // 输入新密码
            char newPwd1[20], newPwd2[20];
            printf("请输入新密码：");
            inputPassword(newPwd1, 20);
            printf("请再次输入新密码：");
            inputPassword(newPwd2, 20);
            if (strcmp(newPwd1, newPwd2) != 0) {
                fclose(fp);
                printf("两次密码不一致！\n");
                return 0;
            }
            // 修复fseek类型警告：显式转换为long
            fseek(fp, -(long)sizeof(Admin), SEEK_CUR);
            strcpy(admin.password, newPwd1);
            fwrite(&admin, sizeof(Admin), 1, fp);
            break;
        }
    }

    fclose(fp);
    if (!found) {
        printf("管理员工号或原密码错误！\n");
        return 0;
    }
    printf("密码修改成功！\n");
    return 1;
}
/////////////////////////////////////////////////////////////////////////////////////////
void welcomeScreen() {
    cout << "*************************************************" << endl;
    cout << "*************************************************" << endl;
    cout << "**                                             **" << endl;
    cout << "**                  欢迎使用！                 **" << endl;
    cout << "**               学生管理系统V1.0              **" << endl;
    cout << "**                                             **" << endl;
    cout << "**   1.学生登录   2.教师登录   3.管理员登录    **" << endl;
    cout << "**                                             **" << endl;
    cout << "**                                             **" << endl;
    cout << "**                  Welcome！                  **" << endl;
    cout << "**                                             **" << endl;
    cout << "*************************************************" << endl;
    cout << "*************************************************" << endl;
}

///////////////////////////////////////////////////////////////////////////////////////
void loginscreen() {
    cout << "*************************************************" << endl;
    cout << "*************************************************" << endl;
    cout << "**                     登录                    **" << endl;
    cout << "**                                             **" << endl;
    cout << "**              1.输入账号与密码               **" << endl;
    cout << "**                2. 密码找回                  **" << endl;
    cout << "**                3. 密码修改                  **" << endl;
    cout << "**                4. 密码注册                  **" << endl;
    cout << "**                5.返回上一级                 **" << endl;
    cout << "**                                             **" << endl;
    cout << "*************************************************" << endl;
    cout << "*************************************************" << endl;
}
////////////////////////////////////////////////////////////////////////////////////
void studentMainscreen() {
    cout << "*************************************************" << endl;
    cout << "*************************************************" << endl;
    cout << "**                   Student                   **" << endl;
    cout << "**                                             **" << endl;
    cout << "**                1. 成绩查询                  **" << endl;
    cout << "**                2.查看本班成绩               **" << endl;
    cout << "**                3. 成绩分析                  **" << endl;
    cout << "**                4.返回上一级                 **" << endl;
    cout << "**                                             **" << endl;
    cout << "*************************************************" << endl;
    cout << "*************************************************" << endl;
}
//////////////////////////////////////////////////////////////////////////////////
void teacherMainscreen() {
    cout << "*************************************************" << endl;
    cout << "*************************************************" << endl;
    cout << "**                   Teacher                   **" << endl;
    cout << "**                                             **" << endl;
    cout << "**           1.  增删改查学生信息              **" << endl;
    cout << "**           2.    查看班内成绩                **" << endl;
    cout << "**           3.      成绩分析                  **" << endl;
    cout << "**           4. 学生信息下载至文件             **" << endl;
    cout << "**           5.     返回上一级                 **" << endl;
    cout << "**                                             **" << endl;
    cout << "*************************************************" << endl;
    cout << "*************************************************" << endl;
}
///////////////////////////////////////////////////////////////////////////////////
void managerMainscreen() {
    cout << "*************************************************" << endl;
    cout << "*************************************************" << endl;
    cout << "**                   Manager                   **" << endl;
    cout << "**                                             **" << endl;
    cout << "**           1.      查看代办                  **" << endl;
    cout << "**           2.  增删改查账密及信息            **" << endl;
    cout << "**           3.    从文件录入账密              **" << endl;
    cout << "**           4.    从文件导出账密              **" << endl;
    cout << "**           5.     返回上一级                 **" << endl;
    cout << "**                                             **" << endl;
    cout << "*************************************************" << endl;
    cout << "*************************************************" << endl;
}

int main() {
first:
    welcomeScreen();
    cout << "请输入您所需的操作：";
    int op;
    char inID[20];
    char inpassWord[20];
    cin >> op;
    system("cls");
    switch (op) {
    case 1://student端
        loginscreen();
        cout << "请输入您所需的操作：";
        int op1;
        cin >> op1;
        switch (op1) {
        case 1://直接登录
            cout << "请输入账号：";
            cin >> inID;
            inputPassword(inpassWord, 20);
            studentLogin(inID, inpassWord);
            break;
        case 2://密码找回
            break;
        case 3://密码修改
            printf("请输入学号：");
            scanf("%s", inID);
            printf("请输入原密码：");
            inputPassword(inpassWord, 20);
            changeStudentPwd(inID, inpassWord);
            break;
        case 4://密码注册
            studentRegister();
            break;
        case 5://返回上一级
            system("cls");
            goto first;
            break;
        }
        break;
    case 2://teacher端
        loginscreen();
        cout << "请输入您所需的操作：";
        int op2;
        cin >> op2;
        switch (op2){
        case 1://直接登录
            cout << "请输入账号：";
            cin >> inID;
            inputPassword(inpassWord, 20);
            teacherLogin(inID, inpassWord);
            break;
        case 2://密码找回
            break;
        case 3://密码修改
            printf("请输入工号：");
            scanf("%s", inID);
            printf("请输入原密码：");
            inputPassword(inpassWord, 20);
            changeTeacherPwd(inID, inpassWord);
            break;
        case 4://密码注册
            teacherRegister();
            break;
        case 5://返回上一级
            system("cls");
            goto first;
            break;
        }
        break;
    case 3://manager端
        loginscreen();
        cout << "请输入您所需的操作：";
        int op3;
        cin >> op3;
        switch (op3) {
        case 1://直接登录
            cout << "请输入管理员工号：";
            cin >> inID;
            inputPassword(inpassWord, 20);
            adminLogin(inID, inpassWord);
            break;
        case 2://密码找回
            break;
        case 3://密码修改
            printf("请输入工号：");
            scanf("%s", inID);
            printf("请输入原密码：");
            inputPassword(inpassWord, 20);
            changeAdminPwd(inID, inpassWord);
            break;
        case 4://密码注册
            adminRegister();
            break;
        case 5://返回上一级
            system("cls");
            goto first;
            break;
        }
        break;
    }
    return 0;
}