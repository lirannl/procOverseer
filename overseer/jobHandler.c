#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <fcntl.h>
#include <pthread.h>
#include "helperMethods.h"
#include "handlers.h"
#include "inputReader.h"
#include "connectionMethods.h"

// Data that gets passed into the function
typedef struct
{
    int newfd;
} threadData;

char *recvMessage(int);

void *handle_job(void *data)
{ /* this is the child process */
    threadData *passedData = (threadData *)data;
    /* Call method to recieve array data */
    char *results = recvMessage(passedData->newfd);
    char *args[MAX_ARGS];
    char *opts[(MAX_OPTIONALS * 2) + 1];
    int valid_input = 0;
    valid_input = interpret_input(results, args, opts);
    if (!valid_input)
    {
        fprintf(stderr, "Invalid input.\n");
        return 0;
    }
    // -o handling
    int out;
    int oIndex = findElemIndex(opts, "-o");
    if (oIndex != -1)
    {
        out = open(opts[oIndex + 1], O_WRONLY | O_CREAT, 0666);
    }
    if (valid_input == 1)
    {
        // Append the execs folder to the path
        char cmdPath[CMD_MAX_LENGTH + 10];
        strcpy(cmdPath, "\0"); // Clear the cmdPath var
        strcat(cmdPath, "./execs/");
        strcat(cmdPath, args[0]);
        int childPid = fork();
        if (!childPid)
        {
            executeFileStart(args[0]);
            int stdoutBkp;
            int stderrBkp;
            dup2(fileno(stdout), stdoutBkp);
            dup2(fileno(stderr), stderrBkp);
            if (oIndex != -1)
            {
                dup2(out, fileno(stdout));
                dup2(out, fileno(stderr));
                close(out);
            }
            execv(cmdPath, args);
            // This will only happen if the file failed to execute
            dup2(stdoutBkp, fileno(stdout));
            dup2(stderrBkp, fileno(stderr));
            executeFileFail(args[0]);
            exit(1);
        }
        else //
        {   
            printf("PID is %d\n", childPid);
            int status;
            wait(&status);
            terminateFile(childPid, status);
        }
    }
    if (valid_input == 2) // Special handlers do not fork into a new process
    {
        if (!strcmp(args[0], "mem"))
            memHandler(args);
        else if (!strcmp(args[0], "memkill"))
            memkillHandler(args);
    }
    free(results);
    if (send(passedData->newfd, "All of array data received by server\n", 40, 0) == -1)
        perror("send");
    close(passedData->newfd);
    return 0;
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
    msg = (char*)malloc(len + 1);
    if (recv(fd, msg, len, 0) != len)
    {
        fprintf(stderr, "recv got invalid length msg\n");
        exit(1);
    }
    msg[len] = '\0';

    return msg;
}