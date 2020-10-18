#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int sleepTime;
    if (argc > 1) sleepTime = atoi(argv[1]);
    else sleepTime = 10;
    printf("Hello! I will now sleep for %d seconds.\n", sleepTime);
    sleep(sleepTime);
    printf("I'm awake! Bye!\n");
}