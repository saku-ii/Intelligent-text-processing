#include<stdio.h>
#include<string.h>
#include<time.h>
#define MAX_SIZE 10000
#define MAX_LINE 100
#define MAX_LEN  256
#define STACK_MAX 50
#define CATEGORY_NUM 3
#define KEYWORD_MAX 5
#define ERR_COUNT (sizeof(errLib)/sizeof(ErrDict))
#define USER_MAX 50
#define USER_NAME_LEN 32
#define PWD_LEN 32
#define ENCRYPT_SHIFT 5   //凯撒加密位移量，密钥5

typedef struct {
    char data[MAX_LINE][MAX_LEN];
    int length;
} SeqList;
SeqList textList; 
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

//用户信息结构体
typedef struct
{
    char username[USER_NAME_LEN];
    char encryptPwd[PWD_LEN];  //存储加密后的密码
    int power;                 //权限：0普通用户 1管理员
}User;

User userList[USER_MAX];
int userCount = 0;
int loginFlag = 0;
char currentUser[USER_NAME_LEN];
int currentPower = 0;
void loadUserFile();
void saveUserFile();
//密码加密：凯撒移位，仅对字母数字处理，符号不变
void encryptPwd(char* src, char* dest)
{
    int i = 0;
    while (src[i] != '\0')
    {
        if (src[i] >= 'a' && src[i] <= 'z')
        {
            dest[i] = (src[i] - 'a' + ENCRYPT_SHIFT) % 26 + 'a';
        }
        else if (src[i] >= 'A' && src[i] <= 'Z')
        {
            dest[i] = (src[i] - 'A' + ENCRYPT_SHIFT) % 26 + 'A';
        }
        else if (src[i] >= '0' && src[i] <= '9')
        {
            dest[i] = (src[i] - '0' + ENCRYPT_SHIFT) % 10 + '0';
        }
        else
        {
            dest[i] = src[i];
        }
        i++;
    }
    dest[i] = '\0';
}

//密码解密，用于登录校验
void decryptPwd(char* src, char* dest)
{
    int i = 0;
    while (src[i] != '\0')
    {
        if (src[i] >= 'a' && src[i] <= 'z')
        {
            dest[i] = (src[i] - 'a' - ENCRYPT_SHIFT + 26) % 26 + 'a';
        }
        else if (src[i] >= 'A' && src[i] <= 'Z')
        {
            dest[i] = (src[i] - 'A' - ENCRYPT_SHIFT + 26) % 26 + 'A';
        }
        else if (src[i] >= '0' && src[i] <= '9')
        {
            dest[i] = (src[i] - '0' - ENCRYPT_SHIFT + 10) % 10 + '0';
        }
        else
        {
            dest[i] = src[i];
        }
        i++;
    }
    dest[i] = '\0';
}
//查重用户名
int checkUserNameExist(char* name)
{
    for (int i = 0; i < userCount; i++)
    {
        if (strcmp(userList[i].username, name) == 0)
            return 1;
    }
    return 0;
}

//用户注册
void saveUserFile()
{
    FILE* fp = fopen("user.txt", "w");
    if (fp == NULL)
    {
        printf("用户数据保存失败！\n");
        return;
    }
    for (int i = 0; i < userCount; i++)
    {
        fprintf(fp, "%s %s %d\n", userList[i].username, userList[i].encryptPwd, userList[i].power);
    }
    fclose(fp);
}
void userRegister()
{
    char name[USER_NAME_LEN], pwd[PWD_LEN];
    char tempEncrypt[PWD_LEN];
    printf("请输入用户名：");
    scanf("%s", name);
    getchar();
    if (checkUserNameExist(name))
    {
        printf("用户名已被注册！\n");
        return;
    }
    printf("请输入密码：");
    scanf("%s", pwd);
    getchar();
    encryptPwd(pwd, tempEncrypt);
    //默认注册为普通用户
    userList[userCount].power = 0;
    strcpy(userList[userCount].username, name);
    strcpy(userList[userCount].encryptPwd, tempEncrypt);
    userCount++;
    saveUserFile();
    printf("注册成功！密码已完成加密存储\n");
}

//用户登录校验
int userLogin()
{
    char name[USER_NAME_LEN], pwd[PWD_LEN];
    char decryptPwdBuf[PWD_LEN];
    printf("请输入用户名：");
    scanf("%s", name);
    getchar();
    printf("请输入密码：");
    scanf("%s", pwd);
    getchar();

    for (int i = 0; i < userCount; i++)
    {
        if (strcmp(userList[i].username, name) == 0)
        {
            decryptPwd(userList[i].encryptPwd, decryptPwdBuf);
            if (strcmp(decryptPwdBuf, pwd) == 0)
            {
                strcpy(currentUser, name);
                currentPower = userList[i].power;
                loginFlag = 1;
                printf("登录成功！当前账号：%s，权限等级：%s\n", currentUser, currentPower == 1 ? "管理员" : "普通用户");
                return 1;
            }
        }
    }
    printf("用户名或密码错误！\n");
    return 0;
}

//权限拦截校验：管理员才可删除文件
int checkPowerDeleteFile()
{
    if (loginFlag == 0)
    {
        printf("请先登录账号！\n");
        return 0;
    }
    if (currentPower != 1)
    {
        printf("权限不足！仅管理员可以执行文件删除操作！\n");
        return 0;
    }
    return 1;
}

//管理员修改用户权限
void adminSetPower()
{
    if (currentPower != 1)
    {
        printf("仅管理员可以修改用户权限！\n");
        return;
    }
    char name[USER_NAME_LEN];
    int newPower;
    printf("请输入待修改权限的用户名：");
    scanf("%s", name);
    getchar();
    for (int i = 0; i < userCount; i++)
    {
        if (strcmp(userList[i].username, name) == 0)
        {
            printf("请设置权限（0普通用户 1管理员）：");
            scanf("%d", &newPower);
            userList[i].power = newPower;
            saveUserFile();
            printf("权限修改完成！\n");
            return;
        }
    }
    printf("未找到该用户！\n");
}



//BF暴力匹配算法
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
    return j == lenT ? i - j : -1;
}

//KMP算法预处理next数组
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

//KMP匹配算法
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
    return j == lenT ? i - j : -1;
}

//字符串替换（单条匹配替换）
void strReplace(char *src, char *find, char *replace, char *result)
{
    int findLen = strlen(find);
    int replaceLen = strlen(replace);
    (void)replaceLen;
    int pos = KMP(src, find); 
    
    if(pos == -1) 
    {
        strcpy(result, src);
        return;
    }

    //拼接替换后的字符串
    strncpy(result, src, pos); 
    result[pos] = '\0';
    strcat(result, replace); 
    strcat(result, src + pos + findLen); 
}

//批量替换文本中所有匹配内容
void batchReplace(char *findStr, char *replaceStr)
{
    if(textList.length == 0)
    {
        printf("当前无文本数据，无法批量替换！\n");
        return;
    }
    void backupText(); 
    
    //图的逻辑：将每行文本视为"节点"，替换关系视为"边"，遍历所有节点完成替换
    char tempLine[MAX_LEN * 2]; 
    int replaceCount = 0; 

    //遍历所有文本行
    for(int i = 0; i < textList.length; i++)
    {
        char *curLine = textList.data[i];
        //循环替换当前行的所有匹配项
        while(1)
        {
            strReplace(curLine, findStr, replaceStr, tempLine);
            if(strcmp(curLine, tempLine) == 0) 
                break;
            strcpy(curLine, tempLine); 
            replaceCount++;
        }
    }

    printf("批量替换完成！共替换 %d 处匹配内容,请手动保存\n", replaceCount);
	}
void replaceMenu()
{
    char find[MAX_LEN], replace[MAX_LEN];
    printf("请输入要查找的内容：");
    fgets(find, MAX_LEN, stdin);
    find[strcspn(find, "\n")] = '\0'; 
    printf("请输入要替换的内容：");
    fgets(replace, MAX_LEN, stdin);
    replace[strcspn(replace, "\n")] = '\0'; 

    if(strlen(find) == 0)
    {
        printf("查找内容不能为空！\n");
        return;
    }

    batchReplace(find, replace);
}



//效率对比
void compareBFKMP() {
    if (textList.length == 0) {
        printf("无文本内容，无法对比效率！\n");
        return;
    }

    clock_t start, end;
    double timeBF, timeKMP;
    int scoreBF[CATEGORY_NUM] = {0};
    int scoreKMP[CATEGORY_NUM] = {0};

//计时BF算法
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

//计时KMP算法
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
    printf("BF 算法总耗时：%.0f 微秒\n", timeBF);
    printf("KMP 算法总耗时：%.0f 微秒\n", timeKMP);
    printf("---------------------------------------------\n");
    printf("KMP 比 BF 快：%.2f 倍\n", timeBF / timeKMP);
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

//BF算法分类
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

//KMP算法分类
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

void modifyLine(char filename[]){
	printf("当前内容：\n");
	printf("---------------------------------------\n");
	for (int i = 0; i < textList.length; i++) {
	    printf("%s\n", textList.data[i]);
	}
	printf("---------------------------------------\n");
	if(textList.length==0){
		printf("无可修改的内容\n");
	}
	backupText();
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
	autoSave(filename);
	printf("修改完成\n");
}
void deleteLine(char filename[]){
	printf("当前内容：\n");
	printf("---------------------------------------\n");
	for (int i = 0; i < textList.length; i++) {
	    printf("%s\n", textList.data[i]);
	}
	printf("---------------------------------------\n");
	if(textList.length==0){
		printf("无可删除内容\n");
		return;
	}
	backupText();
	int line;
	printf("请输入需要删除的行号：");
	scanf("%d",&line);
	getchar();
	if(line<1||line>textList.length){
	    printf("行号错误\n");
		return;
	}
	for(int i=line-1;i<textList.length-1;i++){
		strcpy(textList.data[i],textList.data[i+1]);
	}
	textList.length--;
	autoSave(filename);
	printf("删除完成\n");
}

//智能纠错
void FixSingleLine(char line[])
{
    int len = strlen(line);
    int flagStack[STACK_MAX];
    int top = -1;
    flagStack[++top] = 1;
//大小写智能校正
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
		        if(line[i]>='a'&&line[i]<='z'){
		            line[i] -= 32;}
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
void IntelligentFix()
{
	if(textList.length == 0)
	{
	    printf("暂无文本数据，无法智能更正！\n");
	    return;
	}
	backupText();
	printf("====开始智能大小写校正====\n");
	for(int i=0;i<textList.length;i++)
	{
	    FixSingleLine(textList.data[i]);
	    printf("第%d行：%s\n",i+1,textList.data[i]);
	}
	printf("====智能修正完成！可使用撤销功能回退原文,需手动保存====\n");
}

//编辑菜单
void editText(){
	char filename[100];
	printf("请输入你要编辑的文件名");
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
		printf("4.撤销上一步操作\n");
		printf("5.智能文本大小写校正\n");
		printf("6.保存文件\n");		 
		printf("7.批量替换文本内容\n"); 
		printf("0.返回主菜单\n");
		printf("==============================\n");
		printf("请选择：\n");
		int c;
		scanf("%d",&c);
		while(getchar()!='\n');
		if(c==0)break;
		switch (c) {
			case 1:editWhole(filename);
			    break;
			case 2:modifyLine(filename);
				break;
			case 3:deleteLine(filename);
				break;
			case 4:undoText();
			       autoSave(filename);
				break;
			case 5: IntelligentFix();
			    break;
			case 6:saveFile();
			    break;		
			case 7: replaceMenu(); break;		   
			default:
			printf("输入失败\n");
				break;
		}
		
	}
}

void loadUserFile()
{
    FILE* fp = fopen("user.txt", "r");
    if (fp == NULL)
    {
        userCount = 0;
        return;
    }
    // 无用户时，自动创建初始管理员
	strcpy(userList[0].username, "admin");
	char temp[32];
	encryptPwd("123456", temp);
	strcpy(userList[0].encryptPwd, temp);
	userList[0].power = 1;
	userCount = 1;
	saveUserFile();
	printf("已自动初始化管理员账号：admin，密码：123456\n");
	return;
	userCount = 0;
    while (fscanf(fp, "%s %s %d", userList[userCount].username, userList[userCount].encryptPwd, &userList[userCount].power) != EOF)
    {
        userCount++;
        if (userCount >= USER_MAX) break;
    }
    fclose(fp);
}

//主菜单
int main(){
    loadUserFile();  //开机加载用户账户
	while(1){
	printf("===========文本处理===========\n");
	        printf("【账号系统】\n");
	        printf("1.账号注册\n");
	        printf("2.账号登录\n");
	        printf("3.管理员权限设置\n");
	        printf("【文件编辑】\n");
	        printf("4.新建空白文件\n");
	        printf("5.保存或另存为txt文件\n");
	        printf("6.读取txt文件\n");
	        printf("7.编辑与修改文本内容\n");
	        printf("8.删除整个文件\n");
	        printf("【算法文本分析】\n");
	        printf("9.BF算法文本分类\n");
	        printf("10.KMP算法文本分类\n");
	        printf("11.BF与KMP效率精确对比\n");
	        printf("0.退出\n");
	        printf("==============================\n");
	        printf("请选择：\n");
		int choose;
		scanf("%d",&choose);
		getchar();
		switch(choose){
		    case 1:userRegister();break;
		    case 2:userLogin();break;
		    case 3:adminSetPower();break;
		    case 4:newFile();break;
		    case 5:saveFile();break;
		    case 6:readFile();break;
		    case 7:editText();break;
		    case 8:
		        if(checkPowerDeleteFile()) deleteWholeFile();
		        break;
		    case 9: classByBF(); break;
		    case 10: classByKMP(); break;
		    case 11: compareBFKMP(); break;
		    case 0:return 0;
		    default:printf("输入错误\n请重新输入:\n");break;
		}
	}
}
