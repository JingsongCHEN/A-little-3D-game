#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h> 
#include <dirent.h> 
#include <ctype.h> 

#define MAX_BUFFER 1024 //һ���������ַ��� 
#define MAX_ARGS 64 //һ��ָ�������Ĳ������� 
#define MAX_PATH 100 //һ��·���������ַ��� 
//�����ض���Ľṹ�� 
typedef struct
{
	char *filename;//�ض������ļ��� 
	char opentype[3];//�ļ������� 
	char open[3];//�ض������� 
} Redirect;

extern char **environ;//�������� 
//�������� 
int Trans(char *buf,char **args,int *states,Redirect *Input,Redirect *Output);//���뺯�� 
int Execute(char *buf);//ִ�к��� 
int Shell(FILE *input,const int *states);//shell��� 
int Select(char **args,const Redirect Input,const Redirect Output,int *states);//�жϺ��� 
void Fullpath(char *fullpath,char *shortpath);//ȫ·������ 
//�ڲ�ָ��� 
int Cd(char **args, int *states);//cd
void Clear(void);//clear
int Dir(char **args,const Redirect Output,int *states);//dir
int Echo(char **args,const Redirect Input,const Redirect Output,int *states);//echo
int Environ(const Redirect Output,int *states); //environ
int Pwd(void);//pwd  
int Help(char **args,const Redirect Output,int *states);//help
int Bat(char **args,int *states);//myshell  
