#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <ctype.h>
#include <unistd.h>
#include <wchar.h>

extern float delay;
extern int update;
extern int len;
extern int forceRotate;
extern char* separator;

extern char* full;
extern int offset;

extern int originalLen;
extern int i3;

extern char* pid;
extern char* module;
extern char* player;
extern char* script;
extern char* prefix;

extern int maxLength;
extern int commandLength;

void removeNL(char* arr);
char* getStdout(char* command);
void invalidArgs(char* reason);

void printArgs(int argc, char* argv[]);
void _printArgs_(int argc, char* argv[]);
void parseArgs(int argc, char* argv[]);
void _parseArgs_(int argc, char* argv[]);

void printHelp();
void _printHelp_();

void setDelay(char* d);
void setLength(char* l);
void seti3(char* i);
void setUpdate(char* u);
void setSeparator(char* s);
void setPlayer(char* p);
void setPid(char* p);
void setModule(char* m);
void setScript(char* s);

void rotateText(int dontRotate);
void updatei3();
void updatePrefix();
void updateArgs(int argc, char* argv[]);
void _updateArgs_(int argc, char* argv[], char* dest);
void updateButton(int playing, int paused);

int main(int argc, char* argv[]);
int _main_(int argc, char* argv[]);
