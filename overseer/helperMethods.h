#ifndef LIRAN_HELPER
#define LIRAN_HELPER

#include "helperMethods.c"

int trimEndingWhitespace(char *);
void cleanup_arr(void **);

#define Fork if(!fork())

#endif