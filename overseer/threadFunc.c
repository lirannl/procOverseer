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

pthread_mutex_t request_mutex;

pthread_cond_t got_request;

int num_requests = 0;

struct request
{
    int number;
    char message[INPUT_MAX_LENGTH];
    struct request *next;
};

struct request *requests = NULL;
struct request *last_req = NULL;

void add_request(int request_num,
                 pthread_mutex_t *p_mutex,
                 pthread_cond_t *p_cond_var)
{
    struct request *a_request; /* pointer to newly added request.     */

    /* create structure with new request */
    a_request = (struct request *)malloc(sizeof(struct request));
    if (!a_request)
    { /* malloc failed?? */
        fprintf(stderr, "add_request: out of memory\n");
        exit(1);
    }
    a_request->number = request_num;
    a_request->next = NULL;

    /* lock the mutex, to assure exclusive access to the list */
    pthread_mutex_lock(p_mutex);

    /* add new request to the end of the list, updating list */
    /* pointers as required */
    if (num_requests == 0)
    { /* special case - list is empty */
        requests = a_request;
        last_req = a_request;
    }
    else
    {
        last_req->next = a_request;
        last_req = a_request;
    }

    /* increase total number of pending requests by one. */
    num_requests++;

    /* unlock mutex */
    pthread_mutex_unlock(&request_mutex);

    /* signal the condition variable - there's a new request to handle */
    pthread_cond_signal(&got_request);
}

// Data that gets passed into the function
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
    free(args);
    free(opts);
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
    msg = malloc(len + 1);
    if (recv(fd, msg, len, 0) != len)
    {
        fprintf(stderr, "recv got invalid length msg\n");
        exit(1);
    }
    msg[len] = '\0';

    return msg;
}