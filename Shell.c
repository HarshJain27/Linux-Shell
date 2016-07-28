#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "DataBase.h"
#include <fcntl.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void decide();
char* getCmdLine(int argc,char** argv);
void pipeProcessing();
int isBuiltIn(char*);
void changeDir(char*,char*);
void getPwd();
void export(struct command*);
void echoMe(struct command*);
void historyFind(struct command*);
void generateHistory();
void callHistory_Substitue();
char* givePath();
void callExit();
void handleMe(int);

struct command* commandVar;
struct command* commandVar1;
struct parsingInfo* data;
char* cmdLine=NULL;
char cwd[100];
int flag=0;
char* home;
int main(int argc, char* argv[])
{
home=getenv("HOME");
signal(SIGINT, handleMe);
do
{
getPwd();
printf("\nMy_Shell:%s$ ",cwd);
ssize_t size=0;
getline(&cmdLine, &size, stdin);
generateHistory();
data=parse(cmdLine);
commandVar1=&(data->cmds[0]);
if(commandVar1->cmd[0]=='!') callHistory_Substitue();
else if((commandVar1->paramNum >=3) && strcmp(commandVar1->params[3],"&")==0)
{
printf("ok");
}
else
{
        decide();        
}

}while(1);
return 0;
}
        void decide()
        {
        if((data->numOfPipe)==0 && isBuiltIn(home)) { }
        else { pipeProcessing(); }
        }

void pipeProcessing()
{   

    int status;
    int z = 0,totalPipe=data->numOfPipe;

    int pipe_desc[2*totalPipe];

    for(z = 0; z < (totalPipe); z++){
        if(pipe(pipe_desc + z*2) < 0) {
            perror("couldn't pipe");
            exit(EXIT_FAILURE);
        }
    }

    commandVar=&(data->cmds[0]);
     
    int k = 0,i=0,num=0;
    while((commandVar->cmd)!=NULL) {
      pid_t  pid = fork();
        if(pid == 0) {              
                
            
            if(num<totalPipe){
                if(dup2(pipe_desc[k + 1], 1) < 0){
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            if(k != 0 ){
                if(dup2(pipe_desc[k-2], 0) < 0){
                    perror(" dup2");
                    exit(EXIT_FAILURE);

                }
            }

            for(z = 0; z < 2*totalPipe; z++){
                    close(pipe_desc[z]);
            }
                                 
            int c=0;char* ptr[100];
            while(c<commandVar->paramNum)
            {
            ptr[c]=commandVar->params[c];
            c++;
            }
            ptr[c]=NULL;
            
            if(data->isBackgnd == 1) {
            setsid();
            }
            
            if( execvp(commandVar->cmd,ptr) < 0 ){
                    perror(commandVar->cmd);
                    exit(EXIT_FAILURE);
            }
        } else if(pid < 0){
            perror("error");
            exit(EXIT_FAILURE);
        }

        commandVar=&(data->cmds[++i]);
        num++;
        k+=2;
    }
        
    for(z = 0; z < 2 * totalPipe; z++){
        close(pipe_desc[z]);
    }

    for(z = 0; z < totalPipe + 1; z++)
        wait(&status);

}

int isBuiltIn(char* home)
{
char* builtIn[7]={"cd","pwd","export","echo","history","exit"};
int i=0;
char* name=commandVar1->cmd;
while(builtIn[i])
{
if(strcmp(name,builtIn[i++])==0)
{
if(strcmp(name,"cd")==0)changeDir(commandVar1->params[1],home);
else if(strcmp(name,"pwd")==0) { flag=1; getPwd(); }
else if(strcmp(name,"export")==0) export(commandVar1);
else if(strcmp(name,"echo")==0) echoMe(commandVar1);
else if(strcmp(name,"history")==0) historyFind(commandVar1);
else if(strcmp(name,"exit")==0) callExit();
return 1;
}
}
return 0;
}

void getPwd()
{
getcwd(cwd, sizeof(cwd));
if(flag==1)
{ printf("%s",cwd); flag=0;}
}
void changeDir(char* dir,char* home)
{
char cwd[100];
char* tempPath=getcwd(cwd, sizeof(cwd));
if(strcmp(dir,"..")==0)
chdir("..");
else if(strcmp(dir,"~")==0)
chdir(home);
else if(strcmp(dir,"/")==0)chdir("/");
else
{
strcat(tempPath,"/");
strcat(tempPath,dir);
if(chdir(cwd)==0){}
else
printf("cd: %s: No such file or directory",dir);
}
}

void export(struct command* commandVar1)
{
char* str=commandVar1->params[1];
char* delimit="=";
char* token1 = strtok(str, delimit);
char* token2 = strtok(NULL, delimit);
setenv(token1,token2,1);
}

void echoMe(struct command* commandVar1)
{
char* str=commandVar1->params[1];
if(str[0]=='$')
{
str++;
char* value=getenv(str);
if(value!=NULL)
printf("%s",value);
}
else
printf("%s",str); 
}

char* givePath()
{
        int len1,len2;
        char* homePath=getenv("HOME");
        len1=strlen(homePath);

        char* fileName="/history.txt";
        len2=strlen(fileName);

        int total=len1+len2+1;

        char* fullPath=malloc(sizeof(char)*total);
        strcpy(fullPath,homePath);
        strcat(fullPath,fileName);
        return fullPath;
}

void historyFind(struct command* commandVar1)
{
char* fullPath=givePath();
FILE *fp=fopen( fullPath,"r" );
        if (fp == NULL) 
        {   printf("Error! Could not open file\n"); } 
        char line[1000];
        int lno=0; 
        
        if(commandVar1->params[1]==NULL)
        {
        while ( fgets( line, 1000, fp ) != NULL ) 
            { 
              printf("%d %s",++lno, line); 
            } 
        }
        else
        {
        int count=0,i=0;
        int num=atoi(commandVar1->params[1]);
        while (fgets( line, 1000, fp ) != NULL)
            { 
              count++;               
            }
        fseek(fp, 0, SEEK_SET);    
        while(fgets( line, 1000, fp ) != NULL)
        {
                i++;
                if(i<=count-num){}
                else{ printf("%d %s",i, line); }
        }
        }
}

void generateHistory()
{
        
        char* fullPath=givePath();
        FILE *fp=fopen( fullPath,"a+" );        
        if(fp<0){ printf("Error!"); }
        fprintf(fp,"%s",cmdLine);
        fclose(fp);

}
void handleMe(int a)
{
signal(SIGINT, handleMe);
//exit(0);
}
void callExit()
{
printf("Bye...\n");
exit(EXIT_SUCCESS);
}

void callHistory_Substitue()
{
char* cmdLine=NULL;
int i=0;
char line[1000];
char* fullPath=givePath();
FILE *fp=fopen( fullPath,"r" );

if(commandVar1->cmd[1]>48 && commandVar1->cmd[1]<=57)
{
char* temp=commandVar1->cmd;
temp++;
int num=atoi(temp);
    
        if (fp == NULL) 
        {   printf("Error! Could not open file\n"); } 

        while ( i<num && (fgets( line, 1000, fp ) != NULL) ) 
            { 
              i++;               
            }
cmdLine=line;
data=parse(cmdLine);
commandVar1=&(data->cmds[0]);
decide();
}
else if(commandVar1->cmd[1]=='!' && strlen(commandVar1->cmd)==2 )
{
int count=0;
        while (fgets( line, 1000, fp ) != NULL)
            { 
              count++;               
            }
        fseek(fp, 0, SEEK_SET);
        while ( i<count-1 && (fgets( line, 1000, fp ) != NULL) ) 
            { 
              i++;               
            }
cmdLine=line;
data=parse(cmdLine);
commandVar1=&(data->cmds[0]);
decide();
}
else if((commandVar1->cmd[1]>=65 && commandVar1->cmd[1]<=90)||(commandVar1->cmd[1]>=97 && commandVar1->cmd[1]<=122))
{
char* temp=commandVar1->cmd;temp++;
char* curr1=malloc(100*sizeof(char));
int len=strlen(temp);
fseek(fp, 0, SEEK_SET);
        while (fgets( line, 1000, fp ) != NULL)
            { 
            i++;
            if(strncmp(temp,line,len)==0)
            {
            strcpy(curr1,line);
            }  
            }
printf("%s",curr1);
cmdLine=curr1;
data=parse(cmdLine);
commandVar1=&(data->cmds[0]);
decide();
}
else if(commandVar1->cmd[1]=='-')
{
char* temp=commandVar1->cmd;
temp+=2;
int num=atoi(temp);
int count=0;
        while (fgets( line, 1000, fp ) != NULL)
            { 
              count++;               
            }
int offset=count-num;
        fseek(fp, 0, SEEK_SET);
        while ( i<offset && (fgets( line, 1000, fp ) != NULL) ) 
            { 
              i++;               
            }
cmdLine=line;
data=parse(cmdLine);
commandVar1=&(data->cmds[0]);
decide();
}
}
