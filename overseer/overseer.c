#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
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
    printf("\b\bCaught %d, terminating...\n", signum);
    done = 1;
}

void *executeInput(void *data)
{
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
    char cmd[CMD_MAX_LENGTH];
    // Make NULL-terminated arrays of arguments
    char **args = calloc(MAX_ARGS, sizeof(char *));
    char **opts = calloc((MAX_OPTIONALS * 2) + 1, sizeof(char *));
    char buf[INPUT_MAX_LENGTH];
    int valid_input = 0;
    for (; !done;)
    {
        // Wait for input here
        char inBuf[INPUT_MAX_LENGTH];
        // Temporary direct input - will be replaced with reading the input from the controller
        fgets((char *__restrict)(&inBuf), INPUT_MAX_LENGTH, stdin);
        trimEndingWhitespace(inBuf);
        if (inBuf[0] == '\0')
            return 0;
        valid_input = interpret_input(inBuf, cmd, args, opts);
        if (done)
            break;
        Fork
        {
            // Let the child process know that it isn't the actual overseer
            am_overseer = 0;
            if (valid_input != 2)
            {
                if (valid_input)
                {
                    // Append the execs folder to the path
                    char cmdPath[CMD_MAX_LENGTH + 10];
                    strcat(cmdPath, "./execs/");
                    strcat(cmdPath, cmd);
                    execv(cmdPath, args);
                    printf("No such executable.\n"); // If this same process is still running on the fork - no executable was run.
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
        cleanup_arr((void **)args);
        free(args);
        cleanup_arr((void **)opts);
        free(opts);
    }
}
