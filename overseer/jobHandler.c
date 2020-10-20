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
#include "requestQueue.h"

char *recvMessage(int);

int handle_job(int fd)
{
    /* Call method to recieve array data */
    char *results = recvMessage(fd);
    char *args[MAX_ARGS];
    char *opts[(MAX_OPTIONALS * 2) + 1];
    int valid_input = 0;
    valid_input = interpret_input(results, args, opts);
    if (!valid_input)
    {
        fprintf(stderr, "Invalid input.\n");
        return 0;
    }
    int timeout = 10;
    // -t handling
    int tIndex;
    if (tIndex = findElemIndex(opts, "-t") != -1)
    {
        timeout = atoi(opts[tIndex + 1]);
    }
    // -o handling
    int out;
    int oIndex = findElemIndex(opts, "-o");
    if (oIndex != -1)
    {
        out = open(opts[oIndex + 1], O_WRONLY | O_CREAT, 0666);
    }
    // -log handling
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
        else
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
    if (send(fd, "All of array data received by server\n", 40, 0) == -1)
        perror("send");
    close(fd);
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
    msg = (char *)malloc(len + 1);
    if (recv(fd, msg, len, 0) != len)
    {
        fprintf(stderr, "recv got invalid length msg\n");
        exit(1);
    }
    msg[len] = '\0';

    return msg;
}

void *req_handler(void *data)
{
    struct request *a_request;
    struct global *globalData = (struct global *)data;

    /* lock the mutex, to access the requests list exclusively. */
    pthread_mutex_lock(&request_mutex);

    while (!globalData->termination_triggered)
    {
        if (num_requests > 0)
        {
            a_request = get_request();
            if (a_request)
            { /* got a request - handle it and free it */
                //TO DO - UNLOCCK MUTEX, CALL FUNCTION TO HANDLE REQUEST AND RELOCK MUTEX
                pthread_mutex_unlock(&request_mutex);
                handle_job(a_request->fd);
                free(a_request);
                pthread_mutex_lock(&request_mutex);
            }
        }
        else
        {
            pthread_cond_wait(&got_request, &request_mutex);
        }
    }
    printf("Quitting thread...\n");
}