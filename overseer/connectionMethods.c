#include "helperMethods.h"
#include <stdio.h>

void executeFileStart(char *fileName)
{
    char *time = getTime();
    printf( "%s - attempting to execute %s\n",time, fileName);
    free(time);
}
void executeFileFinish(char *fileName, int pid)
{
    char *time = getTime();
    printf("%s - %s has been executed with pid %d\n",time, fileName, pid);
    free(time);
}
void terminateFile(int pid, int statusCode)
{
    char *time = getTime();
    printf("%s - %d has terminated with status code %d\n",time, pid, statusCode);
    free(time);
}
void executeFileFail(char *fileName, int log)
{
    char *time = getTime();
    fprintf(log, "%s - could not execute %s\n",time, fileName);
    free(time);
}