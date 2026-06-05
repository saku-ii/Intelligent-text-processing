#include<stdio.h>
#include<string.h>
#include<time.h>
#include<stdlib.h>
#define MAX_SIZE 10000
#define MAX_LINE 100
#define MAX_LEN  256
#define STACK_MAX 50
#define CATEGORY_NUM 3
#define KEYWORD_MAX 5
#define ERR_COUNT (sizeof(errLib)/sizeof(ErrDict))

//顺序表结构
typedef struct {
    char data[MAX_LINE][MAX_LEN];
    int length;
} SeqList;
SeqList textList; 

//栈结构
typedef struct{
	SeqList data[STACK_MAX];
	int top;
}Stack;
Stack undoStack;
Stack correctUndoStack; 
//支持中文纠错的错误字典结构
typedef struct {
    char wrong[MAX_LEN];   
    char right[MAX_LEN];   
} ErrDict;

//二叉搜索树结构（存储纠错字典）
typedef struct BSTNode {
    ErrDict dict;          
    struct BSTNode *lchild;
    struct BSTNode *rchild;
} BSTNode, *BSTree;

// 新增：中/英文纠错字典
ErrDict errLib[] = {
    {"既将", "即将"},
    {"好象", "好像"},
    {"穿带", "穿戴"},
    {"松驰", "松弛"},
    {"必竟", "毕竟"},
    {"再接再励", "再接再厉"},
    {"迫不急待", "迫不及待"},
    {"谈笑风声", "谈笑风生"},
    {"滥芋充数", "滥竽充数"},
    {"teh","the"},
	{"wrold","world"},
	{"studey","study"},
	{"happpy","happy"},
	{"frieng","friend"},
	{"becuse","because"},
	{"diffcult","difficult"},
	{"intresting","interesting"},
	{"recieve","receive"},
	{"decieve","deceive"}
};

// 栈操作函数
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

//分类关键词
char categories[CATEGORY_NUM][20] = {"工作", "学习", "生活"};
char keywords[CATEGORY_NUM][KEYWORD_MAX][20] = {
    {"任务", "目标", "会议", "沟通", "效率"},
    {"学习", "课程", "作业", "考试", "笔记"},
    {"娱乐", "旅行", "睡眠", "饮食", "运动"}
};
int keywordCount[CATEGORY_NUM] = {5,5,5};

//BF算法（中文）
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

//KMP算法
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

//BF/KMP效率对比
void compareBFKMP() {
    if (textList.length == 0) {
        printf("当前无文本数据，无法对比效率！\n");
        return;
    }

    clock_t start, end;
    double timeBF, timeKMP;
    int scoreBF[CATEGORY_NUM] = {0};
    int scoreKMP[CATEGORY_NUM] = {0};

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
    printf("           BF 与 KMP 效率对比（微秒）\n");
    printf("=============================================\n");
    printf(" BF 算法总耗时：%.0f 微秒\n", timeBF);
    printf(" KMP 算法总耗时：%.0f 微秒\n", timeKMP);
    printf("---------------------------------------------\n");
    printf(" KMP 是 BF 的：%.2f 倍\n", timeBF / timeKMP);
    printf("---------------------------------------------\n");
    printf("(匹配次数一致，数值越小效率越高)\n");
    printf("=============================================\n\n");
}

//文本备份
void backupText(){
	Push(&undoStack,textList);
}

//文本回退
void undoText(){
	SeqList oldState;
	if(Pop(&undoStack,&oldState))
	{
		textList.length=oldState.length;
	    for(int i=0;i<oldState.length;i++)
     	{
	    	strcpy(textList.data[i],oldState.data[i]);
    	}
    	printf("已回退到上一个版本！\n");
    }
    else{
		printf("无历史记录，无法回退！\n");
	}
}

//自动保存
void autoSave(char filename[]){
	FILE*fp=fopen(filename,"w");
	if(fp==NULL){
		printf("自动保存失败！\n");
		return;
	}
	for(int i=0;i<textList.length;i++){
		fprintf(fp,"%s\n",textList.data[i]);
	}
	fclose(fp);
	printf("已自动保存到原文件！\n ");
}

//新建文件
void newFile(){
	textList.length=0;
	InitStack(&undoStack);
    InitStack(&correctUndoStack); 
	printf("新建空白文本成功！可直接编辑后保存为文件使用。\n");
}

//删除文件
void deleteWholeFile(){
	char filename[100];
	printf("请输入要删除的文件名：");
	scanf("%s",filename);
	if(remove(filename)==0)
		printf("文件删除成功！\n");
	else
	    printf("删除失败！文件不存在或被占用！\n");
}

//读取文件
void readFile(){
	char filename[100];
	printf("请输入要打开的文件名：");
	scanf("%s",filename);
	getchar();
	FILE*fp=fopen(filename,"r");
	if(fp==NULL){
		printf("读取失败！文件不存在！\n");
		return;
	}
	textList.length = 0;
	while (fgets(textList.data[textList.length], MAX_LEN, fp)) {
	textList.data[textList.length][strcspn(textList.data[textList.length], "\n")] = 0;
	textList.length++;
	}
	fclose(fp);
	printf("读取成功！文本内容：\n");
printf("---------------------------------------\n");
    for (int i = 0; i < textList.length; i++) {
        printf("%s\n", textList.data[i]);
    }
printf("---------------------------------------\n");
}

//保存文件
void saveFile(){
	char filename[100];
	printf("请输入保存的文件名：");
	scanf("%s",filename);
	FILE*fp=fopen(filename,"w");
	if(fp==NULL){
		printf("保存失败！\n");
		return;
	}
    for (int i = 0; i < textList.length; i++) {
        fprintf(fp, "%s\n", textList.data[i]);
    }
    fclose(fp);
    printf("保存成功！\n\n");
}

//BF分类
void classByBF()
{
    if (textList.length == 0)
    {
        printf("当前无文本数据，无法分类！\n");
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
    printf("        文本分类结果(BF匹配算法)\n");
    printf("=====================================\n");
    for (int c = 0; c < CATEGORY_NUM; c++)
        printf("与【%s】类匹配度：%d 次\n", categories[c], score[c]);
    printf("-------------------------------------\n");
    if (maxScore == 0)
        printf("未匹配到任何关键词，无法分类！\n");
    else
        printf("综合判断该文本属于：【%s】类\n", categories[bestIndex]);
    printf("=====================================\n\n");
}

//KMP分类
void classByKMP()
{
    if (textList.length == 0)
    {
        printf("当前无文本数据，无法分类！\n");
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
        printf("与【%s】类匹配度：%d 次\n", categories[c], score[c]);
    printf("-------------------------------------\n");
    if (maxScore == 0)
        printf("未匹配到任何关键词，无法分类！\n");
    else
        printf("综合判断该文本属于：【%s】类\n", categories[bestIndex]);
    printf("=====================================\n\n");
}

// 原整段编辑（不变）
void editWhole(char filename[]){
	backupText();
	printf("请输入新的内容，空行结束输入：\n");
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
	    printf("内容修改成功！\n");
}

//逐行修改
void modifyLine(char filename[]){
	printf("当前数据：\n");
	printf("---------------------------------------\n");
	for (int i = 0; i < textList.length; i++) {
	    printf("%s\n", textList.data[i]);
	}
	printf("---------------------------------------\n");
	if(textList.length==0){
		printf("无可用修改的数据！\n");
		return;
	}
	backupText();
	int line;
	printf("请输入要修改的行号：");
	scanf("%d",&line);
	getchar();
	if(line<1||line>textList.length){
		printf("行号越界！\n");
		return;
	}
	char newLine[MAX_LEN];
	printf("请输入新的内容：");
	fgets(newLine,MAX_LEN,stdin);
	newLine[strcspn(newLine,"\n")]=0;
	strcpy(textList.data[line-1],newLine);
	autoSave(filename);
	printf("修改成功！\n");
}

//逐行删除
void deleteLine(char filename[]){
	printf("当前数据：\n");
	printf("---------------------------------------\n");
	for (int i = 0; i < textList.length; i++) {
	    printf("%s\n", textList.data[i]);
	}
	printf("---------------------------------------\n");
	if(textList.length==0){
		printf("无可用删除的数据！\n");
		return;
	}
	backupText();
	int line;
	printf("请输入要删除的行号：");
	scanf("%d",&line);
	getchar();
	if(line<1||line>textList.length){
	    printf("行号越界！\n");
		return;
	}
	for(int i=line-1;i<textList.length-1;i++){
		strcpy(textList.data[i],textList.data[i+1]);
	}
	textList.length--;
	autoSave(filename);
	printf("删除成功！\n");
}

//英文大小写修正
void FixSingleLine(char line[])
{
    int len = strlen(line);
    int flagStack[STACK_MAX];
    int top = -1;
    flagStack[++top] = 1;
	for(int i=0;i<len;i++)
	{
		if(line[i] == '.' || line[i] == '!' || line[i] == '?')
		{
		    flagStack[++top] = 1;
		    continue;
		}
		if((line[i]>='a'&&line[i]<='z')||(line[i]>='A'&&line[i]<='Z'))
		{
		    if(top>=0 && flagStack[top]==1)
	     	{
		        if(line[i]>='a'&&line[i]<='z')
		            line[i] -= 32;
		            top--;
		    }
		    else
		    {
		        if(line[i]>='A'&&line[i]<='Z')
		            line[i] += 32;
		    }
		}
	}
}

//文本智能修正
void IntelligentFix()
{
	if(textList.length == 0)
	{
	    printf("无文本数据，无法进行智能大小写修正！\n");
	    return;
	}
	backupText();
	printf("====开始智能大小写修正====\n");
	for(int i=0;i<textList.length;i++)
	{
	    FixSingleLine(textList.data[i]);
	    printf("第%d行：%s\n",i+1,textList.data[i]);
	}
	printf("====修正完成！使用撤销功能可恢复原版，按回车继续====\n");
	getchar();
}

//二叉搜索树初始化（中文）
void InitBST(BSTree *T) {
    *T = NULL;
    for (int i = 0; i < ERR_COUNT; i++) {
        BSTNode *newNode = (BSTNode *)malloc(sizeof(BSTNode));
        strcpy(newNode->dict.wrong, errLib[i].wrong);
        strcpy(newNode->dict.right, errLib[i].right);
        newNode->lchild = newNode->rchild = NULL;

        if (*T == NULL) {
            *T = newNode;
            continue;
        }

        BSTNode *p = *T, *parent = NULL;
        while (p != NULL) {
            parent = p;
            int cmp = strcmp(errLib[i].wrong, p->dict.wrong);
            if (cmp < 0) {
                p = p->lchild;
            } else if (cmp > 0) {
                p = p->rchild;
            } else {
                free(newNode);
                break;
            }
        }
        if (p == NULL) {
            int cmp = strcmp(errLib[i].wrong, parent->dict.wrong);
            if (cmp < 0) {
                parent->lchild = newNode;
            } else {
                parent->rchild = newNode;
            }
        }
    }
}

//二叉搜索树查找（中文）
BSTNode *BSTSearch(BSTree T, char *key) {
    BSTNode *p = T;
    while (p != NULL) {
        int cmp = strcmp(key, p->dict.wrong);
        if (cmp == 0) {
            return p; 
        } else if (cmp < 0) {
            p = p->lchild;
        } else {
            p = p->rchild;
        }
    }
    return NULL;
}

//中文分词（字符遍历）
void splitChinesePhrases(char *line, char phrases[][MAX_LEN], int *phraseCount) {
    *phraseCount = 0;
    char temp[MAX_LEN] = {0};
    int idx = 0;
    for (int i = 0; line[i] != '\0'; i++) {
        if (line[i] != ' ' && line[i] != '\t' && line[i] != '\n') {
            temp[idx++] = line[i];
        } else {
            if (idx > 0) {
                temp[idx] = '\0';
                strcpy(phrases[*phraseCount], temp);
                (*phraseCount)++;
                idx = 0;
                memset(temp, 0, sizeof(temp));
            }
        }
    }
    if (idx > 0) {
        temp[idx] = '\0';
        strcpy(phrases[*phraseCount], temp);
        (*phraseCount)++;
    }
}

void correctSpelling() {
    if (textList.length == 0) {
        printf("无文本数据，无法进行纠错！\n");
        return;
    }

    // 纠错前备份到回退栈
    Push(&correctUndoStack, textList);
    // 初始化纠错二叉搜索树
    BSTree errTree;
    InitBST(&errTree);

    printf("====开始中文/英文纠错====\n");
    for (int i = 0; i < textList.length; i++) {
        char lineCopy[MAX_LEN];
        strcpy(lineCopy, textList.data[i]); 
        char phrases[MAX_LEN][MAX_LEN];
        int phraseNum = 0;
        // 拆分中文短语（分割）
        splitChinesePhrases(lineCopy, phrases, &phraseNum);
        // 逐短语匹配纠错字典
        char newLine[MAX_LEN] = "";
        for (int j = 0; j < phraseNum; j++) {
            BSTNode *found = BSTSearch(errTree, phrases[j]);
            if (found != NULL) {
                strcat(newLine, found->dict.right);
                printf("第%d行：%s → %s\n", i+1, phrases[j], found->dict.right);
            } else {
                strcat(newLine, phrases[j]);
            }
            if (j != phraseNum - 1) {
                strcat(newLine, " "); 
            }
        }
        strcpy(textList.data[i], newLine); 
    }
    printf("====纠错完成！====\n");
}

//纠错回退
void undoCorrect() {
    SeqList oldState;
    if (Pop(&correctUndoStack, &oldState)) {
        textList.length = oldState.length;
        for (int i = 0; i < oldState.length; i++) {
            strcpy(textList.data[i], oldState.data[i]);
        }
        printf("已回退到纠错前的原始版本！\n");
    } else {
        printf("无纠错历史记录，无法回退！\n");
    }
}

// 新增：智能更正后自动保存（可回退）
void autoSaveAfterCorrect(char filename[]) {
    //保存纠错后的内容到文件
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("纠错后自动保存失败！\n");
        return;
    }
    for (int i = 0; i < textList.length; i++) {
        fprintf(fp, "%s\n", textList.data[i]);
    }
    fclose(fp);

    char bakFilename[MAX_LEN];
    sprintf(bakFilename, "%s_bak.txt", filename);
    SeqList oldState;
    if (Pop(&correctUndoStack, &oldState)) { 
        FILE *bakFp = fopen(bakFilename, "w");
        if (bakFp != NULL) {
            for (int i = 0; i < oldState.length; i++) {
                fprintf(bakFp, "%s\n", oldState.data[i]);
            }
            fclose(bakFp);
            Push(&correctUndoStack, oldState); 
            printf("纠错后自动保存到：%s\n", filename);
            printf("原文已备份到：%s（可通过回退功能恢复）\n", bakFilename);
        } else {
            printf("原文备份失败！\n");
        }
    } else {
        printf("纠错后自动保存到：%s（无原文备份）\n", filename);
    }
}

//编辑菜单
void editText(){
	char filename[100];
	printf("请输入要编辑的文件名：");
	scanf("%s",filename);
	FILE*fp=fopen(filename,"r");
	if(fp==NULL){
		printf("读取失败！文件不存在！\n");
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
		printf("1.整段内容修改\n");
        printf("2.单行修改\n");
		printf("3.单行删除\n");
		printf("4.撤销上一次修改\n");
		printf("5.智能大小写修正\n");
		printf("6.保存文件\n");
        printf("7.中文/英文纠错\n");  
        printf("8.回退纠错前原文\n");
		printf("0.退出编辑菜单\n");
		printf("==============================\n");
		printf("请选择功能：\n");
		int c;
		scanf("%d",&c);
		while(getchar()!='\n');
		if(c==0)break;
		switch (c) {
			case 1:editWhole(filename); break;
			case 2:modifyLine(filename); break;
			case 3:deleteLine(filename); break;
			case 4:undoText(); autoSave(filename); break;
			case 5:IntelligentFix(); break;
			case 6:saveFile(); break;
            case 7:correctSpelling(); autoSaveAfterCorrect(filename); break; // 纠错+自动保存+备份
            case 8:undoCorrect(); autoSaveAfterCorrect(filename); break;   // 回退+自动保存
			default: printf("输入错误！\n"); break;
		}
	}
}

//主函数
int main(){
    InitStack(&undoStack);
    InitStack(&correctUndoStack);
	while(1){
		printf("===========文本处理系统===========\n");
		printf("1.新建空白文件\n");
		printf("2.保存当前内容为txt文件\n");
		printf("3.读取txt文件\n");
		printf("4.编辑/修改文本内容\n");
		printf("5.删除指定文件\n");
		printf("6.BF算法文本分类\n");
		printf("7.KMP算法文本分类\n");
		printf("8.BF与KMP效率对比验证\n");
		printf("0.退出系统\n");
		printf("==============================\n");
		printf("请选择功能：\n");
		int choose;
		scanf("%d",&choose);
		switch(choose){
			case 1:newFile();break;
			case 2:saveFile();break;
			case 3:readFile();break;
			case 4:editText();break;
			case 5:deleteWholeFile();break;
			case 6: classByBF(); break;
			case 7: classByKMP(); break;
			case 8: compareBFKMP(); break;
			case 0:return 0;
			default:printf("输入错误！请重新选择！\n");break;
		}
	}
}
