#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include "DataBase.h"

void initialize(struct parsingInfo *p) 
{
  int x;
  p->isOutfile=0;
  p->isInfile=0;
  p->isBackgnd=0;
  p->numOfPipe=0;

  for (x=0; x<100; x++) 
        {
            p->cmds[x].cmd=NULL;
            p->cmds[x].params[0]=NULL;
            p->cmds[x].paramNum=0;
        }
}

void freeInfo (struct parsingInfo *info) {
  int i,j;
  struct command *comm;

  if (info == NULL) return;
  for (i=0; i<100;i++) {
    comm=&(info->cmds[i]);
    for (j=0; j< comm->paramNum; j++) {
      free(comm->params[j]);
    }
    if (comm->cmd != NULL){
      free(comm->cmd);
    }
  }
  free(info);
}


void parseCommand(char * cmd, struct command *comm) {
  int i=0;
  int pos=0;
  char word[100];

  comm->paramNum=0;
  comm->cmd=NULL;
  comm->params[0]=NULL;
  while(isspace(cmd[i]))
    i++; 
  if (cmd[i] == '\0') 
    return;
  while (cmd[i] != '\0') {
    while (cmd[i] != '\0' && !isspace(cmd[i])) {
      word[pos++]=cmd[i++];
    }
    word[pos]='\0';
    comm->params[comm->paramNum]=malloc((strlen(word)+1)*sizeof(char));
    strcpy(comm->params[comm->paramNum], word);
    comm->paramNum++;
    word[0]='\0';
    pos=0;
    while(isspace(cmd[i]))
      i++; 
  }
  comm->cmd=malloc((strlen(comm->params[0])+1)*sizeof(char));
  strcpy(comm->cmd, comm->params[0]);
  comm->params[comm->paramNum]=NULL;
}
   
struct parsingInfo *parse (char *input) 
{ 
  struct parsingInfo *Final;
  int i=0;
  int pos;
  int end=0;
  char command[100];
  int position;

  if (input[i] == '\n' && input[i] == '\0')
    return NULL;

  Final = malloc(sizeof(struct parsingInfo));
  if (Final == NULL)
  {
    return NULL;
  }
  
  initialize(Final);
  position=0;
  while (input[i] != '\n' && input[i] != '\0') 
  {
    if (input[i] == '&') 
    {
      Final->isBackgnd=1;
      if (input[i+1] != '\n' && input[i+1] != '\0') 
      {
      }
      break;
    }
    else if (input[i] == '<') 
    {
      Final->isInfile=1;
      while (isspace(input[++i]));
      pos=0;
      while (input[i] != '\0' && !isspace(input[i])) 
      {
	Final->inFile[pos++] = input[i++];
      }
      Final->inFile[pos]='\0';
      end =1;
      while (isspace(input[i])) 
      {
	if (input[i] == '\n') 
	  break;
	i++;
      }
    }
    else if (input[i] == '>') 
    {
      Final->isOutfile=1;
      while (isspace(input[++i]));
      pos=0;
      while (input[i] != '\0' && !isspace(input[i])) 
      {
	if (pos==100) 
	{
	  freeInfo(Final);
	  return NULL;
	}
	Final->outFile[pos++] = input[i++];
      }
      Final->outFile[pos]='\0';
      end =1;
      while (isspace(input[i])) 
      {
	if (input[i] == '\n') 
	  break;
	i++;
      } 
    }
    else if (input[i] == '|') 
    {
      command[position]='\0';
      parseCommand(command, &Final->cmds[Final->numOfPipe]);
      position=0;
      end =0;
      Final->numOfPipe++;
      i++;
    }
    else {
      if (end == 1) 
      {
	 fprintf(stderr, "Error.Wrong format of input\n");
	 freeInfo(Final);
	 return NULL;
      }
      
      command[position++] = input[i++];
    }
  }
  command[position]='\0';
  parseCommand(command, &Final->cmds[Final->numOfPipe]);
  return Final;
}

