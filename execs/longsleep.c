#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

void term(int signum)
{
    printf("\b\bOkay, 20 more seconds!\n", signum);
    sleep(20);
}

void setSignals()
{
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = term;
    sigaction(SIGTERM, &action, NULL);
}

int main(int argc, char *argv[])
{
    int sleepTime;
    if (argc > 1) sleepTime = atoi(argv[1]);
    if (argc > 2) setSignals();
    else sleepTime = 10;
    printf("Hello! I will now sleep for %d seconds.\n", sleepTime);
    sleep(sleepTime);
    printf("I'm awake! Bye!\n");
}