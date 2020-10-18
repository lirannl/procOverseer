#ifndef LIRAN_HELPER
#define LIRAN_HELPER

#define CMD_MAX_LENGTH 1000
#define INPUT_MAX_LENGTH 10000
#define MAX_ARGS 100
#define MAX_OPTIONALS 3
#include "helperMethods.c"

int trimEndingWhitespace(char *);
void cleanup_arr(void **);

#define Fork if(!fork())

#endif