#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "helperMethods.h"
#include "handlers.h"
#include "inputReader.h"

typedef struct
{
    int newfd;
} threadData;

char *recvMessage(int);

void *handle_client(void *data)
{ /* this is the child process */
    threadData *passedData = (threadData *)data;
    /* Call method to recieve array data */
    char *results = recvMessage(passedData->newfd);
    char **args = calloc(MAX_ARGS, sizeof(char *));
    char **opts = calloc((MAX_OPTIONALS * 2) + 1, sizeof(char *));
    int valid_input = 0;
    valid_input = interpret_input(results, args, opts);
    Fork
    {
        if (valid_input != 2)
        {
            if (valid_input)
            {
                // Append the execs folder to the path
                char cmdPath[CMD_MAX_LENGTH + 10];
                strcpy(cmdPath, "\0"); // Clear the cmdPath var
                strcat(cmdPath, "./execs/");
                strcat(cmdPath, args[0]);
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
        if (!strcmp(args[0], "mem"))
            memHandler(args);
        else if (!strcmp(args[0], "memkill"))
            memkillHandler(args);
    }
    free(args);
    free(opts);
    free(results);
    if (send(passedData->newfd, "All of array data received by server\n", 40, 0) == -1)
        perror("send");
    close(passedData->newfd);
    exit(0);
}

char *recvMessage(int fd)
{
    char *msg;
    uint32_t netLen;
    int recvLen = recv(fd, &netLen, sizeof(netLen), 0);
    if (recvLen != sizeof(netLen))
    {
        fprintf(stderr, "recv got invalid length value (got %d)\n", recvLen);
        exit(1);
    }
    int len = ntohl(netLen);
    msg = malloc(len + 1);
    if (recv(fd, msg, len, 0) != len)
    {
        fprintf(stderr, "recv got invalid length msg\n");
        exit(1);
    }
    msg[len] = '\0';

    return msg;
}