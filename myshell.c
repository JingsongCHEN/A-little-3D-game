#include "myshell.h"

int main(void)
{
	char pwd[MAX_ARGS];//��ǰ�Ĺ�����ַ 
	char shell_path[MAX_ARGS]="shell=";//myshell�ľ��Ե�ַ 
	char readme_path[MAX_ARGS]="readme_path=";//readme�ľ��Ե�ַ 
	char newpath[MAX_ARGS*1000];
	strcpy(newpath,getenv("PATH"));
	strcat(newpath,":");
	strcpy(pwd,getenv("PWD"));
	strcat(newpath,pwd);
	setenv("PATH",newpath,1);//��PATH����ӵ�ǰ��ַ 
	strcat(shell_path,pwd);
	strcat(shell_path,"/myshell");
	putenv(shell_path);//���myshell�ĵ�ַΪ�������� 
	strcat(readme_path,pwd);
	strcat(readme_path,"/readme");
	putenv(readme_path);//���readme�ĵ�ַΪ�������� 
	Clear();//���� 
	fprintf(stderr,"Welcome to use myshell!\n");
	Shell(stdin,NULL);//����shell 
	return 0; 
}
