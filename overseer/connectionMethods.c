#include "helperMethods.h"
#include <stdio.h>

void executeFileStart(char *fileName, int log)
{
    char *time = getTime();
    dprintf(log, "%s - attempting to execute %s\n",time, fileName);
    free(time);
}
void executeFileFinish(char *fileName, int pid, int log)
{
    char *time = getTime();
    dprintf(log, "%s - %s has been executed with pid %d\n",time, fileName, pid);
    free(time);
}
void terminateFile(int pid, int statusCode, int log)
{
    char *time = getTime();
    dprintf(log, "%s - %d has terminated with status code %d\n",time, pid, statusCode);
    free(time);
}
void executeFileFail(char *fileName, int log)
{
    char *time = getTime();
    dprintf(log, "%s - could not execute %s\n",time, fileName);
    free(time);
}