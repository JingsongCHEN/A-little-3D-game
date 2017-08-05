#include "myshell.h"

int back_bat=0;//��̨�������ж� 
char batchfile[MAX_PATH];//�������ļ��� 
int isbat=0;//�������־ 
char *open;//�ض������� 

int Trans(char *buf,char **args,int *states,Redirect *Input,Redirect *Output)//���뺯�� 
{
	int i,flag,argc,error;
	char c;
	states[0]=states[1]=states[2]=states[3]=0;//״̬���㣬states[0]�Ǻ�̨��־��states[1]�������ض����־��states[2]������ض����־��states[3]�ǲ������� 
	error=argc=0;
	args[0]=NULL;
	open=NULL;
	flag=1;
	i=-1;
	while(buf[++i]&&buf[i]!='#')//ѭ����ȡһ��ָ�� 
	{
		c=buf[i];
		switch(c)
		{
			case '<'://�����ض��� 
				if(flag==0)
				{
					flag=1;
					buf[i]='\0';
				}
				open="<";
				while(buf[++i]==' '|| buf[i]=='\t');//�������Ŀո��tab 
				if(buf[i]<32|| buf[i]=='#')//�ض������ַ����� 
				{
				 	error=1;
					printf("Error!\n");  
				 	break;
				}
				else if(buf[i]=='&'|| buf[i]=='<'|| buf[i]=='>'|| buf[i]=='|'|| buf[i]==';') //�ض������ַ����� 
                {      
                    error=1;
		    		printf("Error!\n");     
                    break;   
                }                     
				states[1]=1;//�����ض����� 
				i--;
				break;
			case '>'://����ض��� 
				if(flag==0)
				{
					flag=1;
					buf[i]='\0';
				} 
				if(buf[i+1]=='>')//�ж��Ƿ��ǡ�>>�� 
				{
					buf[++i]='\0';
					open=">>";
				}
				else
					open=">";
				while(buf[++i]==' '|| buf[i]=='\t');//�������Ŀո��tab 
				if(buf[i]<32|| buf[i]=='#')//�ж��ض�����ź���ַ��Ƿ���ȷ 
				{
				 	error=1;
					printf("Error!\n");  
				 	break;
				}
				else if(buf[i]=='&'|| buf[i]=='<'|| buf[i]=='>'|| buf[i]=='|'|| buf[i]==';') 
                {       
                    error=1; 
	            printf("Error!\n");    
                    break;   
                }
				i--;
				states[2]=1;//����ض�����
				break;
			case'&'://��̨ģʽ 
				if(flag==0)
				{
					flag=1;
					buf[i]='\0';
			    }
				if(states[0])
				{
				error=1;
            			printf("Error!\n");                             
                break;    	
				}
				states[0]=1;//��̨ģʽ��� 
				break;
			case' '://����ո��tab 
			case'\t':
				if(flag==0)
				{
					flag=1;
					buf[i]='\0';
			    }
				while(buf[++i]==' '|| buf[i]=='\t');
				i--;
				break;
			case'\n':
			case'\r':
				buf[i]='\0';
				i--;
				break;//���м�����ѭ�� 
			default:
				if(flag)
				{
					flag=0;
					if(open)//���ض���ṹ�帳ֵ 
					{
						if(!strcmp(open,"<" ))
							Input->filename=buf+i;
						else  if( !strcmp(open,">>")) 
						{  
                        	strcpy(Output->opentype,"a");   
                            strcpy(Output->open,">>");   
                        	Output->filename=buf+i;  
                        }  
                        else  if( !strcmp(open,">" ) )  
                        {  
                        	strcpy(Output->opentype,"w");   
                            strcpy(Output->open,">");    
                            Output->filename=buf+i;  
                    	}   
                        open=NULL;  	
					}
					else
						args[argc++]=buf+i;
				}
		}
	}
	args[argc]=NULL;
	states[3]=argc;//��ȡ��������			
	if(error||argc==0)//�����������Ϊ���򱨴� 
		printf("Error!\n");  ;		  
	return error;	
}
int Execute(char *buf)//ִ�к��� 
{
	pid_t pid;//���̺� 
	char *args[MAX_ARGS];//���� 
	int error;
	int states[4];//״̬��� 
	Redirect Input;//�ض���      
	Redirect Output;
	error=Trans(buf,args,states,&Input,&Output);
	if(error||args[0]==NULL) //����޲����ͷ��� 
		return -1;
	if(!strcmp(args[0],"quit"))//�����quitָ��ͽ���myshell 
	{
		if(args[1])
			printf("Error!\n"); 
		fprintf(stderr,"Thanks for using!\n");
		exit(0);
	}
	else if(states[0])//����Ǻ�̨ģʽ 
	{
		switch (pid=fork())//fork�����ӽ��� 
		{
			case -1: printf("Error!\n");  
			case 0: sleep(1);//��ʱ1�� 
					fprintf(stderr,"\n");
					Select(args,Input,Output,states);//�����жϺ��� 
					exit(1);
			default: if(isbat==0)
					 	fprintf(stderr,"pid=%d\n",pid); //��ʾ���̺�				
		}
	}
	else
		Select(args,Input,Output,states);//�����жϺ��� 
	return 0;
}
int Shell(FILE *input,const int *states)//��ں��� 
{
	char buf[MAX_BUFFER];
	do
	{
		if(input==stdin)
			fprintf(stderr,"\n[%s@%s]:$",getenv("USERNAME"),getenv("PWD"));//����������ǰ����ʾ 
		if(fgets(buf,MAX_BUFFER,input))//���ж�ȡ 
		{
			Execute(buf);//ִ�� 
		}
	}while (!feof(input));
	return 0;
}
int Select(char **args,const Redirect Input,const Redirect Output,int *states)//�жϺ��� 
{
	char filepath[MAX_PATH],parent[MAX_ARGS];
	FILE *output=NULL,*input;
	pid_t newpid;
	int flag;
	if(!strcmp(args[0],"myshell"))//�����myshell 
	{
		flag=0;
		if(isbat)//����������� 
		{
			switch(newpid=fork())//fork�����ӽ��� 
			{
				case -1:printf("Error!\n");  
				case 0:     
                    if(states[0]&&(args[1]))   
                    {  
						back_bat++;    
                        flag=1;   
                    }                                                                                
                    Bat(args,states);   //�ں�ִ̨��        
                    if(flag)   back_bat--;      
                    exit (0);   
                default:  waitpid(newpid, NULL, WUNTRACED); //�ȵ��ӽ��̽�����ִ��   
			}
		}
		else
		{
		if(states[0]&&(args[1])) //����Ǻ�̨����  
        {            
			back_bat++;   
            flag=1;   
        }     
        Bat(args,states);       //ִ��   
        if(flag)   back_bat--;   	
		}
		if(states[0]) 
			exit(1);      
        else   
			return 0;     
    }   
	if(states[2])  //�����������¶���  
    {   
    	Fullpath(filepath, Output.filename);   //ȡ��������ַ 
    	output=freopen(filepath, Output.opentype,stdout);   //���ļ� 
        if(output==NULL)    
    	{       
			printf("Error!\n");                   
            if(states[0]) 
				exit(1);      
            else    
				return -4;     
    	}                        
    }       
	if(!strcmp(args[0],"cd"))    //cd                
    	Cd(args,states);    
   
    else if(!strcmp(args[0],"clr"))  //clr 
    {             
            Clear();                                              
        if(args[1]||states[1]||states[2]) //����в������ض���ͱ���   
        	printf("Error!\n");       
    }   
    
    else if (!strcmp(args[0],"dir"))   //dir
        Dir(args,Output,states);   
                   
    else if (!strcmp(args[0],"echo"))    //echo 
        Echo(args,Input,Output,states);    
    else if (!strcmp(args[0],"environ"))  //environ
    {     
        Environ(Output,states);   
        if(states[1])   //����������ض���ͱ���  
            printf("Error!\n");    
        if(args[1])    //����в����ͱ��� 
            printf("Error!\n");     
    }                   
    else if (!strcmp(args[0],"help"))   //help 
    {        
	if(args[1])	//����в�����ִ�У����򱨴� 
		Help(args,Output,states);
	else
		 printf("Error!\n");
        if(states[1])      
            printf("Error!\n");    
    }    
    else  if (!strcmp(args[0],"pwd"))  //pwd     
    {          
        Pwd();  
        if(states[1])//����������ض��������ͱ��� 
            printf("Error!\n");  
        if(args[1])    
            printf("Error!\n");                   
    }   
    else 
	{   
        strcpy(parent,"parent=") ;  //������·��         
        strcat(parent, getenv("shell"));        
        switch (newpid=fork( ))  //fork���������� 
        {      
            case -1:   
				 printf("Error!\n");     
            case 0:                                                  
                if(states[1])   //����������ض��� 
                {            
					Fullpath(filepath,Input.filename);   //ȡ��ȫ·��                                                                       
					input=freopen(filepath,"r",stdin);  //���ļ� 
                    if(input==NULL)    
                    {    
				printf("Error!\n");       
                        exit(1);   
					}   
                }        
                putenv(parent);   //��Ӹ�����·��                                     
                execvp(args[0],args);                                                                                          
                printf("Error!\n");     
                sleep(1);        
                if(input)  
					fclose(input);                                 
                exit(0);   
            default:     
                if(states[0]==0)  //������Ǻ�̨ 
                    waitpid(newpid, NULL, WUNTRACED);                                                                      
        }     
    }          
           
    if(output)   //���������ض�����ر��ļ� 
    {    
		fclose(output);   
        freopen("/dev/tty","w",stdout);   
    }            
    if(states[0]) 
		exit(0);      
    else 
		return 0;    
}
void Fullpath(char *fullpath,char *shortpath) //ȫ·������ 
{
	int i,j;    
    i=j=0;         
    fullpath[0]=0;    
    char *old_dir, *current_dir; //��·���͵�ǰ·��  
    if(shortpath[0]=='~')   //�����ǡ�~�� 
    {   
    	strcpy(fullpath, getenv("HOME"));//�����HOME��ַ   
        j=strlen(fullpath);        
        i=1;    
    }      
    else  if(shortpath[0]=='.'&&shortpath[1]=='.') //�����ǡ�..�� 
    {                    
        old_dir=getenv("PWD");  
        chdir("..");                   
        current_dir=(char *)malloc(MAX_BUFFER);
		getcwd(current_dir,MAX_BUFFER);   
        strcpy(fullpath, current_dir); //�������һ����ַ   
        j=strlen(fullpath);        
        i=2;     
        chdir(old_dir);  
    }  
    else   if(shortpath[0]=='.')//�����'.' 
    {      
        strcpy(fullpath, getenv("PWD")); //����ӵ�ǰĿ¼  
    	j=strlen(fullpath);        
        i=1;   
    }  
    else if(shortpath[0]!='/')    //�������������� 
    {         
        strcpy(fullpath, getenv("PWD")); //����ӵ�ǰ��ַ 
        strcat(fullpath, "/");   
        j=strlen(fullpath);                       
        i=0;   
    }  
    strcat(fullpath+j,shortpath+i);    
    return;    
}
int Cd(char **args,int *states)//cd 
{      
    char dirpath[MAX_PATH],filepath[MAX_PATH],dirname[MAX_PATH];  
    char  *current_dir;    
    int i,flag;   
    FILE *inputfile;               
    if(args[1])   //����в��� 
    {                        
    	if(args[2]) //�����������ͱ���                       
            printf("Error!\n");            
        Fullpath(dirpath,args[1]);    //��ò���1��ȫ��ַ                                                                                         
    }   
    else   //������ʾ��ǰĿ¼ 
	{    
		fprintf(stdout,"%s\n",getenv("PWD"));   
        return 0;   
    }   
    flag=chdir(dirpath);  
    if(flag)  //���Ŀ¼������                         
    {      
		 printf("Error!\n");              
        return -2;    
    }     
    current_dir=(char *)malloc(MAX_BUFFER);   
    if(!current_dir)           
		printf("Error!\n");                 
    getcwd(current_dir,MAX_BUFFER);   
    setenv("PWD",current_dir,1);  //�޸�PWDΪ��ǰĿ¼ 
    free(current_dir);   
    return 0;   
}
void Clear(void)//clear 
{         
    pid_t newpid;   //���̺� 
    switch (newpid=fork( ))   //fork�����ӽ��� 
    {   
        case -1:   
            printf("Error!\n");     
        case 0:       
            execlp("clear","1",NULL);//�ӽ���ִ��    
            printf("Error!\n");                                            
        default:      
			waitpid(newpid, NULL, WUNTRACED);   
            fprintf(stderr,"Clear!\n");    
    }     
	return;   
}
int Dir(char **args,const Redirect Output,int *states)//dir 
{   
	   
    pid_t newpid;  //���̺� 
    DIR *pdir;  
    int i;   
    char filepath[MAX_PATH],dirpath[MAX_PATH],dirname[MAX_PATH];  
    if(args[1])     //����в��� 
    {                        
    	if(args[2])   // ����һ�������ͱ���                        
               printf("Error!\n");              
        Fullpath(dirpath,args[1]);  //ȡȫ·��                                                                                           
    }  
    else 
	strcpy(dirpath, ".");   
    pdir=opendir(dirpath);                
    if(pdir==NULL)//���Ŀ¼������Ҳ����                      
    {         
		printf("Error!\n");           
        return -2 ;   
    }   
    switch (newpid=fork( ))   //fork�����ӽ��� 
    {   
        case -1:   
            printf("Error!\n");    
        case 0:                                                                                       
            execlp( "ls","ls" ,"-al", dirpath, NULL);  //���ӽ�����ִ��                                   
            printf("Error!\n");                                                        
        default:   
			waitpid(newpid, NULL, WUNTRACED);   
    }           
    return 0;   
} 
int Echo(char **args,const Redirect Input,const Redirect Output,int *states)//echo 
{   
	FILE *input,*output;    
    char filepath[MAX_PATH];   
    char buf[MAX_BUFFER];   
    int k;
	if(states[2])//�������ض��� 
	{
		Fullpath(filepath,Output.filename); //ȡȫ·��  
    	output=freopen(filepath,Output.opentype,stdout);  //���ļ�      	
		fprintf(stderr,"\nThe results will be writen into file \"%s\".\n",Output.filename);                
	
	}  
    if(states[1]) //��������ض���   
    {   
		if(args[1])    // ������в����ͱ���      
             printf("Error!\n");                         
        Fullpath(filepath,Input.filename);  //ȡȫ·�� 
        input=fopen(filepath,"r");   
        if(input==NULL)    
        {     
		printf("Error!\n");                     
            return -2;    
        }   
        if(states[2]==0)//���ֻ�������ض���     
        	fprintf(stderr,"The contents of file \"%s\" is as follows:\n",Input.filename);   
        while (!feof(input))    // ���ļ�   
        {   
            if(fgets(buf, MAX_BUFFER, input))   
            	fprintf(stdout,"%s",buf); //��ӡ   
        }                             
        fclose(input);//�رն��ļ�   
        fprintf(stdout,"\n");                                
    }   
           
    else 
	{       
        if(args[1])   //����в��� 
        {    
			for(k=1;k<states[3]-1;k++)     //ѭ����ʾ 
                fprintf(stdout,"%s ",args[k]);     
            fprintf(stdout,"%s",args[k]);     
        }     
        fprintf(stdout,"\n");                                                                                                    
    }       
    return 0;   
}   
int Environ(const Redirect Output,int *states)//environ 
{
	FILE *output;         
    char filepath[MAX_PATH];    
    char ** env = environ;//ȡȫ�������� 
    if(states[2])
	{
		Fullpath(filepath,Output.filename);   //ȡȫ·�� 
    	output=freopen(filepath,Output.opentype,stdout); //���ļ�       	
		fprintf(stderr,"\nThe results will be writen into file \"%s\".\n",Output.filename);                
	}
	while(*env) 
		fprintf(stdout,"%s\n",*env++); //���                                     
    return 0;           
}   
int Pwd(void)//pwd
{                             
    fprintf(stdout,"%s\n",getenv("PWD"));//��PWD                          
    return 0;   
}
int Help(char **args,const Redirect Output,int *states)//help 
{           
	FILE *readme;   
    char buffer[MAX_BUFFER];     
    char keyword[MAX_BUFFER]="<help ";      
    int i,len;       
    strcat(keyword,args[1]);   
    strcat(keyword," ");      
    len=strlen(keyword);   
    keyword[len-1]='>';   
    keyword[len]='\0';  //ȡ�����keyword           
    if(!strcmp(keyword,"<help more>"))   //�����help more 
    {  
        strcpy(buffer,"more ");  
        strcat(buffer,getenv("readme_path"));  //����moreָ�� 
        for(i=0;i<states[2];i++)  
        {  
			strcat(buffer,Output.open);  
            strcat(buffer,Output.filename);  
        }    
        Execute(buffer);                         
        return 0;    
    }  
    readme=fopen(getenv("readme_path"),"r");   //��readme   
    while(!feof(readme)&&fgets(buffer,MAX_BUFFER,readme))   
    {   
		if(strstr(buffer,keyword))   //����������Ϊ��ͣ�� 
    		break;  
    }      
    while(!feof(readme)&&fgets(buffer,MAX_BUFFER,readme))  //���ļ�ֱ��'#' 
    {   
        if(buffer[0]=='#')   
            break;   
        fputs(buffer,stdout);      
    }      
    if(feof(readme))  //��������׾ͱ��� 
    {    
		keyword[len-1]='\0';     
          printf("Error!\n");            
    }   
    if(readme) 
		fclose(readme);   
    return 0;   
}   
int Bat(char **args,int *states)//myshell 
{      
	FILE *input;  
    char filepath[MAX_PATH];    
    int i=0;  
    char fullpath_batchfile[MAX_PATH];   
    pid_t  newpid;    
    if(isbat)   //����������� 
        fprintf(stderr,"***Line of inputfile \"%s\": ",batchfile);         
    if(args[1]) //����в��� 
    {         
        if(args[2])  //����һ���ͱ���                        
             printf("Error!\n");                                 
		Fullpath(filepath,args[1]);  //ȡȫ·��      
        Fullpath(fullpath_batchfile,batchfile);  
        if(isbat && !strcmp(fullpath_batchfile,filepath)) 
        {   
            fprintf(stderr,"Warning: commands not execute, it will cause infinite loop!!!\n");                
            return -5;    
        }   
        input=fopen(filepath,"r");//���ļ� 
        if(input==NULL) 
        {         
		printf("Error!\n");         
            return -2;    
        }   
		isbat=1;     
        strcpy(batchfile,args[1]);  //�����ļ���ַ   
        fprintf(stderr,"Turn to execute the commands in batch file \"%s\" :\n",batchfile);                                         
    	Shell(input,states);     //��ʼѭ����ȡ 
        fclose(input);   
        fprintf(stderr,"\nExecution of batch file \"%s\" is finished!\n",batchfile);   
	isbat=0;          
    }     
    else       //������ʾ�����ļ���ַ 
        fprintf(stdout,"Please input the filename!\n");         
    return 0;   
}   
