struct command
{
int paramNum;
char* cmd;
char* params[20];
};

struct parsingInfo
{
int   numOfPipe;
int   isBackgnd;
int   isOutfile;
int   isInfile;		       
char  outFile[2];	
char  inFile[2];	       
struct command cmds[1000];
};
struct parsingInfo* parse(char *);
