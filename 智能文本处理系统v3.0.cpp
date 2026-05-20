// v3.0版本新增功能：根据关键词自动进行文本分类
// 1.将新闻、报道、官方判定为新闻类
// 2.将课程、作业、考试判定为学习类
// 3.将日常、吃饭、心情判定为生活类

#include <stdio.h>
#include <string.h>

#define MAX_SIZE    10000
#define MAX_LINE    100
#define MAX_LEN     256
#define CATEGORY_NUM 3
#define KEYWORD_MAX 5

// 文本线性表结构
typedef struct {
    char data[MAX_LINE][MAX_LEN];
    int length;
} SeqList;

SeqList textList;    // 主文本
SeqList backupList; // 撤销备份

// 分类与关键词配置
char categories[CATEGORY_NUM][20] = {"新闻", "学习", "生活"};
char keywords[CATEGORY_NUM][KEYWORD_MAX][20] = {
    {"新闻", "报道", "发布", "消息", "官方"},
    {"学习", "课程", "作业", "考试", "笔记"},
    {"生活", "吃饭", "睡觉", "日常", "心情"}
};
int keywordCount[CATEGORY_NUM] = {5, 5, 5};


// 备份文本内容
void backupText(void) {
    backupList.length = textList.length;
    for (int i = 0; i < textList.length; i++) {
        strcpy(backupList.data[i], textList.data[i]);
    }
}

// 撤销上一次操作
void undoText(void) {
    textList.length = backupList.length;
    for (int i = 0; i < backupList.length; i++) {
        strcpy(textList.data[i], backupList.data[i]);
    }
    printf("已撤销上一次修改\n");
}

// 自动保存到原文件
void autoSave(char filename[]) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("自动保存失败\n");
        return;
    }
    for (int i = 0; i < textList.length; i++) {
        fprintf(fp, "%s\n", textList.data[i]);
    }
    fclose(fp);
    printf("已自动保存到原文件\n");
}

// 新建空白文件
void newFile(void) {
    textList.length = 0;
    printf("已新建空白文本，请手动保存为文件使用\n");
}

// 删除整个文件
void deleteWholeFile(void) {
    char filename[100];
    printf("请输入要删除的文件名：");
    scanf("%s", filename);

    if (remove(filename) == 0)
        printf("文件删除成功！\n");
    else
        printf("删除失败，文件不存在或被占用！\n");
}

// 打开并读取文件
void readFile(void) {
    char filename[100];
    printf("请输入你要打开的文件名：");
    scanf("%s", filename);
    getchar();

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("打开失败，文件不存在\n");
        return;
    }

    textList.length = 0;
    while (fgets(textList.data[textList.length], MAX_LEN, fp)) {
        textList.data[textList.length][strcspn(textList.data[textList.length], "\n")] = 0;
        textList.length++;
    }
    fclose(fp);

    printf("读取成功，内容如下：\n");
    printf("---------------------------------------\n");
    for (int i = 0; i < textList.length; i++) {
        printf("%s\n", textList.data[i]);
    }
    printf("---------------------------------------\n");
}

// 保存文件
void saveFile(void) {
    char filename[100];
    printf("请输入保存的文件名：");
    scanf("%s", filename);

    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("保存失败\n");
        return;
    }
    for (int i = 0; i < textList.length; i++) {
        fprintf(fp, "%s\n", textList.data[i]);
    }
    fclose(fp);
    printf("保存成功！\n\n");
}

// 文本自动分类
void textClassification(void) {
    if (textList.length == 0) {
        printf("当前无文本内容，无法分类！\n");
        return;
    }

    int score[CATEGORY_NUM] = {0};

    // 遍历所有文本行，统计关键词匹配得分
    for (int i = 0; i < textList.length; i++) {
        for (int c = 0; c < CATEGORY_NUM; c++) {
            for (int k = 0; k < keywordCount[c]; k++) {
                if (strstr(textList.data[i], keywords[c][k]) != NULL) {
                    score[c]++;
                }
            }
        }
    }

    // 找出最高分分类
    int maxScore = score[0];
    int bestIndex = 0;
    for (int c = 1; c < CATEGORY_NUM; c++) {
        if (score[c] > maxScore) {
            maxScore = score[c];
            bestIndex = c;
        }
    }

    // 输出分类结果
    printf("\n=====================================\n");
    printf("          文本自动分类结果\n");
    printf("=====================================\n");
    printf("分类匹配得分：\n");
    for (int c = 0; c < CATEGORY_NUM; c++) {
        printf("【%s】：%d 分\n", categories[c], score[c]);
    }
    printf("-------------------------------------\n");

    if (maxScore == 0) {
        printf("未匹配到任何关键词 → 【未分类】\n");
    } else {
        printf("最终判定：文本属于 → 【%s】类\n", categories[bestIndex]);
    }
    printf("=====================================\n\n");
}

// 整体覆盖修改文本
void editWhole(char filename[]) {
    backupText();
    printf("请输入新的内容覆盖(空行结束):\n");
    textList.length = 0;
    getchar();

    char line[MAX_LEN];
    while (1) {
        fgets(line, MAX_LEN, stdin);
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) == 0) break;

        strcpy(textList.data[textList.length], line);
        textList.length++;
    }

    printf("\n修改完成，是否撤销(1=撤销，0=不撤销并保存)：");
    int op;
    scanf("%d", &op);

    if (op == 1) {
        undoText();
    } else {
        autoSave(filename);
    }
}

// 逐行修改内容
void modifyLine(char filename[]) {
    printf("请输入你要打开的文件名：");
    scanf("%s", filename);
    FILE *fp = fopen(filename, "r");

    if (fp == NULL) {
        printf("打开失败，文件不存在\n");
        return;
    }

    textList.length = 0;
    while (fgets(textList.data[textList.length], MAX_LEN, fp)) {
        textList.data[textList.length][strcspn(textList.data[textList.length], "\n")] = 0;
        textList.length++;
    }
    fclose(fp);

    printf("当前内容：\n");
    printf("---------------------------------------\n");
    for (int i = 0; i < textList.length; i++) {
        printf("%s\n", textList.data[i]);
    }
    printf("---------------------------------------\n");

    if (textList.length == 0) {
        printf("无可修改的内容\n");
        return;
    }

    int line;
    printf("请输入需要修改的行号：");
    scanf("%d", &line);
    getchar();

    if (line < 1 || line > textList.length) {
        printf("行号错误\n");
        return;
    }

    char newLine[MAX_LEN];
    printf("请输入新的内容：");
    fgets(newLine, MAX_LEN, stdin);
    newLine[strcspn(newLine, "\n")] = 0;
    strcpy(textList.data[line - 1], newLine);

    printf("\n是否撤销修改(1=撤销，0=不撤销并保存)：");
    int op;
    scanf("%d", &op);

    if (op == 1) {
        undoText();
    } else {
        autoSave(filename);
    }
}

// 逐行删除内容
void deleteLine(char filename[]) {
    printf("请输入你要打开的文件名：");
    scanf("%s", filename);
    FILE *fp = fopen(filename, "r");

    if (fp == NULL) {
        printf("打开失败，文件不存在\n");
        return;
    }

    textList.length = 0;
    while (fgets(textList.data[textList.length], MAX_LEN, fp)) {
        textList.data[textList.length][strcspn(textList.data[textList.length], "\n")] = 0;
        textList.length++;
    }
    fclose(fp);

    printf("当前内容：\n");
    printf("---------------------------------------\n");
    for (int i = 0; i < textList.length; i++) {
        printf("%s\n", textList.data[i]);
    }
    printf("---------------------------------------\n");

    if (textList.length == 0) {
        printf("无可删除内容\n");
        return;
    }

    backupText();
    int line;
    printf("请输入需要删除的行号：");
    scanf("%d", &line);

    if (line < 1 || line > textList.length) {
        printf("行号错误\n");
        return;
    }

    for (int i = line - 1; i < textList.length - 1; i++) {
        strcpy(textList.data[i], textList.data[i + 1]);
    }
    textList.length--;

    printf("\n是否撤销删除(1=撤销，0=不撤销并保存)：");
    int op;
    scanf("%d", &op);

    if (op == 1) {
        undoText();
    } else {
        autoSave(filename);
    }
}

// 文本编辑子菜单
void editText(void) {
    char filename[100];
    printf("请输入你要打开的文件名：");
    scanf("%s", filename);
    FILE *fp = fopen(filename, "r");

    if (fp == NULL) {
        printf("打开失败，文件不存在\n");
        return;
    }

    textList.length = 0;
    while (fgets(textList.data[textList.length], MAX_LEN, fp)) {
        textList.data[textList.length][strcspn(textList.data[textList.length], "\n")] = 0;
        textList.length++;
    }
    fclose(fp);

    while (1) {
        printf("\n==========文本编辑==========\n");
        printf("1.整体覆盖修改\n");
        printf("2.逐行修改\n");
        printf("3.逐行删除\n");
        printf("0.返回主菜单\n");
        printf("==============================\n");
        printf("请选择：");

        int c;
        scanf("%d", &c);
        while (getchar() != '\n');

        if (c == 0) break;

        switch (c) {
            case 1: editWhole(filename); break;
            case 2: modifyLine(filename); break;
            case 3: deleteLine(filename); break;
            default: printf("输入失败\n"); break;
        }
    }
}

// 主函数
int main(void) {
    while (1) {
        printf("===========文本处理===========\n");
        printf("1.新建空白文件\n");
        printf("2.另存为txt文件\n");
        printf("3.读取txt文件\n");
        printf("4.编辑与修改文本内容\n");
        printf("5.删除整个文件\n");
        printf("6.文本自动分类\n");
        printf("0.退出\n");
        printf("==============================\n");
        printf("请选择：");

        int choose;
        scanf("%d", &choose);

        switch (choose) {
            case 1: newFile(); break;
            case 2: saveFile(); break;
            case 3: readFile(); break;
            case 4: editText(); break;
            case 5: deleteWholeFile(); break;
            case 6: textClassification(); break;
            case 0: return 0;
            default: printf("输入错误，请重新输入！\n"); break;
        }
    }
}
