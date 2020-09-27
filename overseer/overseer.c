#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "inputReader.h"

#define CMD_MAX_LENGTH 1000
#define INPUT_MAX_LENGTH 10000

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
    printf("Listening on port %d\n", port);
    char *cmd = malloc(CMD_MAX_LENGTH);
    char *args = malloc(INPUT_MAX_LENGTH);
    char buf[INPUT_MAX_LENGTH];
    for (;;)
    {
        cmd = '\0';
        args = '\0';
        // Wait for input here
        interpret_input(&cmd, &args);
        if (!fork())
        {
            char *argsarr[] = {cmd, args, NULL};
            // Append the execs folder to the path
            char cmdPath[CMD_MAX_LENGTH * 2];
            strcat(cmdPath, "./execs/");
            strcat(cmdPath, cmd);
            execv(cmdPath, argsarr);
            exit(0);
        }
    };
    free(cmd);
    free(args);
}