#ifndef CONN_METHODS_H
#define CONN_METHODS_H

#include "connectionMethods.c"
void executeFileStart(char *);
void executeFileFinish(char *, int);
void terminateFile(int, int);
void executeFileFail(char *, int);

#endif