#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "inputReader.h"
#include "handlers.h"

#define CMD_MAX_LENGTH 1000
#define INPUT_MAX_LENGTH 10000
#define MAX_ARGS 100
#define MAX_OPTIONALS 3

volatile __sig_atomic_t done = 0;
int am_overseer = 1;

void term(int signum)
{
    printf("Caught %d, terminating...\n", signum);
    done = 1;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("No port supplied. Please only provide a port number.\n");
        return 1;
    }
    int port = atoi(argv[1]);
    if (port > 65535 || port <= 0)
    {
        printf("Invalid port number. Must be between 1 and 65535.\n");
        return 1;
    }
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = term;
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGINT, &action, NULL);
    printf("Listening on port %d\n", port);
    char *cmd = malloc(CMD_MAX_LENGTH);
    // Make NULL-terminated arrays of arguments
    char **args = malloc(MAX_ARGS);
    args[MAX_ARGS] = NULL;
    char **opts = malloc(MAX_OPTIONALS * 2 + 1);
    opts[MAX_OPTIONALS * 2 + 1] = NULL;
    char buf[INPUT_MAX_LENGTH];
    int valid_input = 0;
    for (; !done;)
    {
        cmd = '\0';
        // Wait for input here
        valid_input = interpret_input(&cmd, &args, &opts);
        Fork
        {
            if (valid_input != 2)
            {
                if (valid_input)
                {
                    // Let the child process know that it isn't the actual overseer
                    am_overseer = 0;
                    // Append the execs folder to the path
                    char cmdPath[CMD_MAX_LENGTH * 2];
                    strcat(cmdPath, "./execs/");
                    strcat(cmdPath, cmd);
                    execv(cmdPath, args);
                }
                else if (!valid_input)
                {
                    printf("Invalid input.\n");
                }
            }
            exit(0);
        }
        else if (valid_input == 2) // Special handlers do not fork into a new process
        {

            if (!strcmp(cmd, "mem"))
                memHandler();
            else if (!strcmp(cmd, "memkill"))
                memkillHandler();
        }
    }
    if (am_overseer)
    {
        free(cmd);
        cleanup_arr((void **)args);
        free(args);
        cleanup_arr((void **)opts);
        free(opts);
    }
}