#ifndef CONN_METHODS_H
#define CONN_METHODS_H

#include "connectionMethods.c"
void executeFileStart(char *, int);
void executeFileFinish(char *, int, int);
void terminateFile(int, int, int);
void executeFileFail(char *, int);

#endif