//2.0版本新增了更便捷的文件修改，可选择整体覆盖或逐行更改与删除，同时也增添了修改后的撤销与自动保存功能。
//填补了上一版本中出现的无法主动在页面内进行删除文件的功能，同时整合了上一位同学能够更便捷的创建空白文件的功能
//同时也调整了主菜单的选项顺序，便于更流畅地编辑文件。
//（如果有机会也考虑在下一版本中看能不能实现在编辑文件时自由选择是编辑上次修改过的文件还是另选其他文件使编辑更加简便ovo）


#include<stdio.h>
#include<string.h>
#define MAX_SIZE 10000
#define MAX_LINE 100
#define MAX_LEN  256

typedef struct {
    char data[MAX_LINE][MAX_LEN];
    int length;
} SeqList;
SeqList textList; 
SeqList backupList;
void backupText(){
	backupList.length=textList.length;
	for(int i=0;i<textList.length;i++)
	{
		strcpy(backupList.data[i],textList.data[i]);
	}
}
//撤销上次操作
void undoText(){
	textList.length=backupList.length;
	for(int i=0;i<backupList.length;i++){
		strcpy(textList.data[i],backupList.data[i]);
	}
	printf("已撤销上一次修改\n");
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
	    printf("\n修改完成，是否撤销(1=撤销，0=不撤销并保存）");
        int op;
        scanf("%d",&op);
        if(op==1){
			undoText();
		}
		else{
			autoSave(filename);
		}
}
void modifyLine(char filename[]){
	printf("请输入你要打开的文件名");
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
	printf("当前内容：\n");
	printf("---------------------------------------\n");
	for (int i = 0; i < textList.length; i++) {
	    printf("%s\n", textList.data[i]);
	}
	printf("---------------------------------------\n");
	if(textList.length==0){
		printf("无可修改的内容\n");
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
	    if(op==1){
			undoText();
		}
		else{
			autoSave(filename);
		}
}
void deleteLine(char filename[]){
	printf("请输入你要打开的文件名");
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
	if(line<1||line>textList.length){
	    printf("行号错误\n");
		return;
	}
	for(int i=line-1;i<textList.length-1;i++){
		strcpy(textList.data[i],textList.data[i+1]);
	}
	textList.length--;
	printf("\n是否撤销删除(1=撤销，0=不撤销并保存）");
	int op;
	scanf("%d",&op);
	if(op==1){
		undoText();
	}
	else{
		autoSave(filename);
	}
}

//编辑菜单
void editText(){
	char filename[100];
	printf("请输入你要打开的文件名");
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
			case 1:editWhole(filename);
			    break;
			case 2:modifyLine(filename);
				break;
			case 3:deleteLine(filename);
				break;
			default:
			printf("输入失败\n");
				break;
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
		printf("0.退出\n");
		printf("==============================\n");
		printf("请选择：\n");
		int choose;
		scanf("%d",&choose);
		switch(choose){
			case 1:newFile();break;
			case 2:saveFile();break;
			case 3:readFile();break;
			case 4:editText();break;
			case 5:deleteWholeFile();break;
			case 0:return 0;
			default:printf("输入错误\n请重新输入:\n");break;
		}
	}
}
