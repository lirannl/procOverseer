#ifndef OVERSEER_HELPER
#define OVERSEER_HELPER

#define CMD_MAX_LENGTH 1000
#define INPUT_MAX_LENGTH 10000
#define MAX_ARGS 100
#define MAX_OPTIONALS 3
#define THREADS_NUM 5
#include "helperMethods.c"

int trimEndingWhitespace(char *);
void cleanup_arr(void **);
int findElemIndex(char **, char *);
void freeStrArr(char **);
char *getTime();
char *uniteStrArr(char **);

#define Fork if(!fork())

#endif