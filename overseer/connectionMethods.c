#include "helperMethods.h"

void executeFileStart(char *fileName)
{
    getTime();
    printf(" - attempting to execute %s\n", fileName);
}
void executeFileFinish(char *fileName, int pid)
{
    getTime();
    printf(" - %s has been executed with pid %d\n", fileName, pid);
}
void terminateFile(int pid, int statusCode)
{
    getTime();
    printf(" - %d has terminated with status code %d\n", pid, statusCode);
}
void executeFileFail(char *fileName)
{
    getTime();
    printf(" - could not execute %s\n", fileName);
}