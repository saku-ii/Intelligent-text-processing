//3.0版本新增文本自动分类
//手动实现BF、KMP算法完成关键词匹配，显示BF与KMP算法效率差距

#include<stdio.h>
#include<string.h>
#include<time.h>      
#define MAX_SIZE 10000
#define MAX_LINE 100
#define MAX_LEN  256
#define STACK_MAX 50
#define CATEGORY_NUM 3
#define KEYWORD_MAX 5

typedef struct {
    char data[MAX_LINE][MAX_LEN];
    int length;
} SeqList;
SeqList textList;
SeqList backupList;

//顺序栈部分
typedef struct{
	SeqList data[STACK_MAX];
	int top;
}Stack;
Stack undoStack;
void InitStack(Stack*s)
{
	s->top=-1;
}
int IsEmpty(Stack*s)
{
	return s->top==-1;
}
int IsFull(Stack*s)
{
	return s->top==STACK_MAX-1;
}
int Push(Stack*s,SeqList cur)
{
	if(IsFull(s))
	   return 0;
	s->top++;
	s->data[s->top].length=cur.length;
	for(int i=0;i<cur.length;i++)
	{
		strcpy(s->data[s->top].data[i],cur.data[i]);
	}
	return 1;
}
int Pop(Stack*s,SeqList*res)
{
	if(IsEmpty(s))
		return 0;
	res->length=s->data[s->top].length;
	for(int i=0;i<res->length;i++)
	{
		strcpy(res->data[i],s->data[s->top].data[i]);
	}
	s->top--;
	return 1;
}

char categories[CATEGORY_NUM][20] = {"新闻", "学习", "生活"};
char keywords[CATEGORY_NUM][KEYWORD_MAX][20] = {
    {"新闻", "报道", "发布", "消息", "官方"},
    {"学习", "课程", "作业", "考试", "笔记"},
    {"生活", "吃饭", "睡觉", "日常", "心情"}
};
int keywordCount[CATEGORY_NUM] = {5,5,5};

// BF暴力匹配算法
int BF(char *str, char *sub)
{
    int i = 0, j = 0;
    int lenS = strlen(str);
    int lenT = strlen(sub);
    while(i < lenS && j < lenT)
    {
        if(str[i] == sub[j])
        {
            i++;
            j++;
        }
        else
        {
            i = i - j + 1;
            j = 0;
        }
    }
    return j == lenT ? 1 : 0;
}

// KMP算法预处理next数组
void getNext(char *sub, int next[])
{
    int len = strlen(sub);
    int i = 0, j = -1;
    next[0] = -1;
    while(i < len - 1)
    {
        if(j == -1 || sub[i] == sub[j])
        {
            i++;
            j++;
            next[i] = j;
        }
        else
            j = next[j];
    }
}

// KMP匹配算法
int KMP(char *str, char *sub)
{
    int lenS = strlen(str);
    int lenT = strlen(sub);
    if(lenT == 0) return 0;
    int next[MAX_LEN];
    getNext(sub, next);
    int i = 0, j = 0;
    while(i < lenS && j < lenT)
    {
        if(j == -1 || str[i] == sub[j])
        {
            i++;
            j++;
        }
        else
            j = next[j];
    }
    return j == lenT ? 1 : 0;
}

//新增：效率对比
void compareBFKMP() {
    if (textList.length == 0) {
        printf("无文本内容，无法对比效率！\n");
        return;
    }

    clock_t start, end;
    double timeBF, timeKMP;
    int scoreBF[CATEGORY_NUM] = {0};
    int scoreKMP[CATEGORY_NUM] = {0};

// 计时BF算法
    start = clock();
    for (int t = 0; t < 10000; t++) {  
        memset(scoreBF, 0, sizeof(scoreBF));
        for (int i = 0; i < textList.length; i++)
            for (int c = 0; c < CATEGORY_NUM; c++)
                for (int k = 0; k < keywordCount[c]; k++)
                    if (BF(textList.data[i], keywords[c][k]))
                        scoreBF[c]++;
    }
    end = clock();
    timeBF = (double)(end - start) * 1000000 / CLOCKS_PER_SEC;

// 计时KMP算法
    start = clock();
    for (int t = 0; t < 10000; t++) {
        memset(scoreKMP, 0, sizeof(scoreKMP));
        for (int i = 0; i < textList.length; i++)
            for (int c = 0; c < CATEGORY_NUM; c++)
                for (int k = 0; k < keywordCount[c]; k++)
                    if (KMP(textList.data[i], keywords[c][k]))
                        scoreKMP[c]++;
    }
    end = clock();
    timeKMP = (double)(end - start) * 1000000 / CLOCKS_PER_SEC;

    printf("\n=============================================\n");
    printf("           BF 与 KMP 效率对比（高精度）\n");
    printf("=============================================\n");
    printf("? BF 算法总耗时：%.0f 微秒\n", timeBF);
    printf("? KMP 算法总耗时：%.0f 微秒\n", timeKMP);
    printf("---------------------------------------------\n");
    printf("?? KMP 比 BF 快：%.2f 倍\n", timeBF / timeKMP);
    printf("---------------------------------------------\n");
    printf("(匹配结果一致，仅对比执行效率)\n");
    printf("=============================================\n\n");
}

//自动备份
void backupText(){
	Push(&undoStack,textList);
}
//撤销操作
void undoText(){
	SeqList oldState;
	if(Pop(&undoStack,&oldState))
	{
		textList.length=oldState.length;
	    for(int i=0;i<oldState.length;i++)
     	{
	    	strcpy(textList.data[i],oldState.data[i]);
    	}
    	printf("已撤销上次操作\n");
    }
    else{
		printf("暂无操作记录，无法撤销\n");
	}
}

//自动保存
void autoSave(char filename[]){
	FILE*fp=fopen(filename,"w");
	if(fp==NULL){
		printf("自动保存失败\n");
		return;
	}
	for(int i=0;i<textList.length;i++){
		fprintf(fp,"%s\n",textList.data[i]);
	}
	fclose(fp);
	printf("已自动保存到原文件\n ");
}

//新建空白文件
void newFile(){
	textList.length=0;
	InitStack(&undoStack);
	printf("已新建空白文本，请手动保存为文件使用\n");
}

//删除文件
void deleteWholeFile(){
	char filename[100];
	printf("请输入要删除的文件名：");
	scanf("%s",filename);
	if(remove(filename)==0)
		printf("文件删除成功！\n");
	else
	    printf("删除失败，文件不存在或被占用！\n");
}

//打开文件
void readFile(){
	char filename[100];
	printf("请输入你要打开的文件名");
	scanf("%s",filename);
	getchar();
	FILE*fp=fopen(filename,"r");
	if(fp==NULL){
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

//保存文件
void saveFile(){
	char filename[100];
	printf("请输入保存的文件名");
	scanf("%s",filename);
	FILE*fp=fopen(filename,"w");
	if(fp==NULL){
		printf("保存失败\n");
		return;
	}
    for (int i = 0; i < textList.length; i++) {
        fprintf(fp, "%s\n", textList.data[i]);
    }
    fclose(fp);
    printf("保存成功！\n\n");
}

//修改文件(覆盖或逐行修改删除)
void editWhole(char filename[]){
	backupText();
	printf("请输入新的内容覆盖(空行结束):\n");
	textList.length = 0;
	getchar();
	char line[MAX_LEN];
		while (1){
			fgets(line,MAX_LEN,stdin);
			line[strcspn(line,"\n")]=0;
			if(strlen(line)==0) break;
            strcpy(textList.data[textList.length],line);
	    	textList.length++;
		}
		autoSave(filename);
	    printf("整体修改完成\n");
}

// BF算法分类
void classByBF()
{
    if (textList.length == 0)
    {
        printf("当前无文本内容，无法分类！\n");
        return;
    }
    int score[CATEGORY_NUM] = {0};
    for (int i = 0; i < textList.length; i++)
    {
        for (int c = 0; c < CATEGORY_NUM; c++)
        {
            for (int k = 0; k < keywordCount[c]; k++)
            {
                if(BF(textList.data[i], keywords[c][k]))
                    score[c]++;
            }
        }
    }
    int maxScore = score[0], bestIndex = 0;
    for (int c = 1; c < CATEGORY_NUM; c++)
    {
        if (score[c] > maxScore)
        {
            maxScore = score[c];
            bestIndex = c;
        }
    }
    printf("\n=====================================\n");
    printf("        文本分类结果(BF暴力算法)\n");
    printf("=====================================\n");
    for (int c = 0; c < CATEGORY_NUM; c++)
        printf("【%s】：%d 分\n", categories[c], score[c]);
    printf("-------------------------------------\n");
    if (maxScore == 0)
        printf("未匹配到任何关键词 → 【未分类】\n");
    else
        printf("最终判定：文本属于 → 【%s】类\n", categories[bestIndex]);
    printf("=====================================\n\n");
}

// KMP算法分类
void classByKMP()
{
    if (textList.length == 0)
    {
        printf("当前无文本内容，无法分类！\n");
        return;
    }
    int score[CATEGORY_NUM] = {0};
    for (int i = 0; i < textList.length; i++)
    {
        for (int c = 0; c < CATEGORY_NUM; c++)
        {
            for (int k = 0; k < keywordCount[c]; k++)
            {
                if(KMP(textList.data[i], keywords[c][k]))
                    score[c]++;
            }
        }
    }
    int maxScore = score[0], bestIndex = 0;
    for (int c = 1; c < CATEGORY_NUM; c++)
    {
        if (score[c] > maxScore)
        {
            maxScore = score[c];
            bestIndex = c;
        }
    }
    printf("\n=====================================\n");
    printf("        文本分类结果(KMP算法)\n");
    printf("=====================================\n");
    for (int c = 0; c < CATEGORY_NUM; c++)
        printf("【%s】：%d 分\n", categories[c], score[c]);
    printf("-------------------------------------\n");
    if (maxScore == 0)
        printf("未匹配到任何关键词 → 【未分类】\n");
    else
        printf("最终判定：文本属于 → 【%s】类\n", categories[bestIndex]);
    printf("=====================================\n\n");
}


void modifyLine(char filename[]){
	printf("请输入你要打开的文件名：");
	scanf("%s",filename);
	FILE*fp=fopen(filename,"r");
	if(fp==NULL){
		printf("打开失败，文件不存在\n");
		return;
	}
	textList.length = 0;
	while (fgets(textList.data[textList.length], MAX_LEN, fp)) {
		textList.data[textList.length][strcspn(textList.data[textList.length], "\n")] = 0;
		textList.length++;
	}
	fclose(fp);
	printf("当前内容：\n---------------------------------------\n");
	for (int i = 0; i < textList.length; i++)
	    printf("%s\n", textList.data[i]);
	printf("---------------------------------------\n");
	if(textList.length==0){
		printf("无可修改的内容\n");
        return;
	}
	int line;
	printf("请输入需要修改的行号：");
	scanf("%d",&line);
	getchar();
	if(line<1||line>textList.length){
		printf("行号错误\n");
		return;
	}
	char newLine[MAX_LEN];
	printf("请输入新的内容：");
	fgets(newLine,MAX_LEN,stdin);
	newLine[strcspn(newLine,"\n")]=0;
	strcpy(textList.data[line-1],newLine);
	printf("\n是否撤销修改(1=撤销，0=不撤销并保存）");
	int op;
	scanf("%d",&op);
	if(op==1)
		undoText();
	else
		autoSave(filename);
}

void deleteLine(char filename[]){
	printf("请输入你要打开的文件名：");
	scanf("%s",filename);
	FILE*fp=fopen(filename,"r");
	if(fp==NULL){
		printf("打开失败，文件不存在\n");
		return;
	}
	textList.length = 0;
	while (fgets(textList.data[textList.length], MAX_LEN, fp)) {
	    textList.data[textList.length][strcspn(textList.data[textList.length], "\n")] = 0;
	    textList.length++;
	}
	fclose(fp);
	printf("当前内容：\n---------------------------------------\n");
	for (int i = 0; i < textList.length; i++)
	    printf("%s\n", textList.data[i]);
	printf("---------------------------------------\n");
	if(textList.length==0){
		printf("无可删除内容\n");
		return;
	}
	backupText();
	int line;
	printf("请输入需要删除的行号：");
	scanf("%d",&line);
	if(line<1||line>textList.length){
	    printf("行号错误\n");
		return;
	}
	for(int i=line-1;i<textList.length-1;i++)
		strcpy(textList.data[i],textList.data[i+1]);
	textList.length--;
	printf("\n是否撤销删除(1=撤销，0=不撤销并保存）");
	int op;
	scanf("%d",&op);
	if(op==1)
		undoText();
	else
		autoSave(filename);
}

//编辑菜单
void editText(){
	char filename[100];
	printf("请输入你要打开的文件名：");
	scanf("%s",filename);
	FILE*fp=fopen(filename,"r");
	if(fp==NULL){
		printf("打开失败，文件不存在\n");
		return;
	}
	textList.length = 0;
	while (fgets(textList.data[textList.length], MAX_LEN, fp)) {
	    textList.data[textList.length][strcspn(textList.data[textList.length], "\n")] = 0;
	    textList.length++;
	}
	fclose(fp);
	while(1){
		printf("\n==========文本编辑==========\n");
		printf("1.整体覆盖修改\n");
        printf("2.逐行修改\n");
		printf("3.逐行删除\n");
		printf("0.返回主菜单\n");
		printf("==============================\n");
		printf("请选择：\n");
		int c;
		scanf("%d",&c);
		while(getchar()!='\n');
		if(c==0)break;
		switch (c) {
			case 1:editWhole(filename); break;
			case 2:modifyLine(filename); break;
			case 3:deleteLine(filename); break;
			default: printf("输入失败\n"); break;
		}
	}
}
//主菜单
int main(){
	while(1){
		printf("===========文本处理===========\n");
		printf("1.新建空白文件\n");
		printf("2.另存为txt文件\n");
		printf("3.读取txt文件\n");
		printf("4.编辑与修改文本内容\n");
		printf("5.删除整个文件\n");
		printf("6.BF算法文本分类\n");
		printf("7.KMP算法文本分类\n");
		printf("8.BF与KMP效率精确对比\n");   // 新增菜单
		printf("0.退出\n");
		printf("==============================\n");
		printf("请选择：\n");
		int choose;
		scanf("%d",&choose);
		switch(choose){
			case 1: newFile(); break;
			case 2: saveFile(); break;
			case 3: readFile(); break;
			case 4: editText(); break;
			case 5: deleteWholeFile(); break;
			case 6: classByBF(); break;
			case 7: classByKMP(); break;
			case 8: compareBFKMP(); break;   // 新增
			case 0: return 0;
			default: printf("输入错误\n请重新输入:\n"); break;
		}
	}
}
