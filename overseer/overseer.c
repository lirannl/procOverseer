#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "inputReader.h"

#define CMD_MAX_LENGTH 1000
#define INPUT_MAX_LENGTH 10000
#define MAX_ARGS 100

volatile __sig_atomic_t done = 0;
int am_overseer = 1;

void term(int signum)
{
    printf("Caught %d, terminating...\n", signum);
    done = 1;
}

int main(int argc, char *argv[])
{
    /*if (argc != 2)
    {
        printf("No port supplied. Please only provide a port number.\n");
        return 1;
    }
    int port = atoi(argv[1]);
    if (port > 65535 || port <= 0)
    {
        printf("Invalid port number. Must be between 1 and 65535.\n");
        return 1;
    }*/
    int port = 3000;
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = term;
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGINT, &action, NULL);
    printf("Listening on port %d\n", port);
    char *cmd = malloc(CMD_MAX_LENGTH);
    // Make args an array of at least 1 string
    char **args = malloc(MAX_ARGS);
    char buf[INPUT_MAX_LENGTH];
    for (; !done;)
    {
        cmd = '\0';
        // Wait for input here
        interpret_input(&cmd, &args);
        if (!fork())
        {
            // Let the child process know that it isn't the actual overseer
            am_overseer = 0;
            // Append the execs folder to the path
            char cmdPath[CMD_MAX_LENGTH * 2];
            strcat(cmdPath, "./execs/");
            strcat(cmdPath, cmd);
            execv(cmdPath, args);
            exit(0);
        }
    }
    if (am_overseer)
    {
        free(cmd);
        cleanup_arr((void **)args);
        free(args);
    }
}