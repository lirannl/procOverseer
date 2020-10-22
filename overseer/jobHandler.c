#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <fcntl.h>
#include <pthread.h>
#include "helperMethods.h"
#include "handlers.h"
#include "inputReader.h"
#include "connectionMethods.h"
#include "requestQueue.h"

struct timer_data {
    int timeout;
    int logfd;
    pid_t pid;
};

char *recvMessage(int);

int tHandler(char **);

void *killProc(void *);

int handle_job(int fd) {
    /* Call method to recieve array data */
    char *results = recvMessage(fd);
    char *args[MAX_ARGS];
    char *opts[(MAX_OPTIONALS * 2) + 1];
    int valid_input = 0;
    valid_input = interpret_input(results, args, opts);
    if (!valid_input) {
        fprintf(stderr, "Invalid input.\n");
        return 0;
    }
    int timeout = tHandler(opts);
    // -o handling
    int out;
    int oIndex = findElemIndex(opts, "-o");
    if (oIndex != -1) {
        out = open(opts[oIndex + 1], O_WRONLY | O_CREAT, 0666);
    }
    // -log handling
    int log = fileno(stdout);
    int lIndex = findElemIndex(opts, "-l");
    if (lIndex != -1) {
        log = open(opts[lIndex + 1], O_WRONLY | O_CREAT, 0666);
    }
    char *fullExec = uniteStrArr(args);
    if (valid_input == 1) {
        // Append the execs folder to the path
        char cmdPath[CMD_MAX_LENGTH + 10];
        strcpy(cmdPath, "\0"); // Clear the cmdPath var
        strcat(cmdPath, "./execs/");
        strcat(cmdPath, args[0]);
        int fds[2];
        pipe(fds);
        write(fds[1], "T", 1);
        pid_t childPid = fork();
        if (!childPid) {
            executeFileStart(fullExec, log);
            free(fullExec);
            int stdoutBkp;
            int stderrBkp;
            dup2(fileno(stdout), stdoutBkp);
            dup2(fileno(stderr), stderrBkp);
            if (oIndex != -1) {
                dup2(out, fileno(stdout));
                dup2(out, fileno(stderr));
                close(out);
            }
            execv(cmdPath, args);
            // This will only happen if the file failed to execute
            dup2(stdoutBkp, fileno(stdout));
            dup2(stderrBkp, fileno(stderr));
            write(fds[1], "F", 1);
            close(fds[1]);
            exit(1);
        } else {
            pthread_t timer;
            struct timer_data tData;
            tData.timeout = timeout;
            tData.logfd = log;
            tData.pid = childPid;
            pthread_create(&timer, NULL, killProc, &tData);
            pthread_detach(timer);
            usleep(1600);
            char buf[3];
            buf[2] = '\0';
            read(fds[0], buf, 2);
            int success = strcmp(buf, "TF") != 0;
            if (success)
                executeFileFinish(fullExec, childPid, log);
            dprintf(fds[1], "TTT"); // Flush the pipe by writing 3 Ts, to prevent false-failures
            close(fds[0]);
            close(fds[1]);
            int status;
            wait(&status);
            if (success)
                terminateFile(childPid, status, log);
            else
                executeFileFail(fullExec, log);
            free(fullExec);
        }
    }
    if (valid_input == 2) // Special handlers do not fork into a new process
    {
        if (!strcmp(args[0], "mem"))
            memHandler(args);
        else if (!strcmp(args[0], "memkill"))
            memkillHandler(args);
    }
    if (lIndex != -1) {
        close(log);
    }
    free(results);
    if (send(fd, "All of array data received by server\n", 40, 0) == -1)
        perror("send");
    close(fd);
    return 0;
}

char *recvMessage(int fd) {
    char chunk[500];
    char *msg;
    memset(chunk, 0, 500);  //clear the variable
    int recvLen;

    if ((recvLen = recv(fd, chunk, 500, 0)) < 0) {
        fprintf(stderr, "recv got invalid length value (got %d)\n", recvLen);
        exit(1);
    } else {
        msg = malloc(sizeof(chunk) + 1);
        for (int i = 0; i < recvLen; i++) {
            msg[i] = chunk[i];
        }
        msg[sizeof(chunk) + 1] = '\0';
        return msg;
    }
}

void *req_handler(void *data) {
    struct request *a_request;
    struct global *globalData = (struct global *) data;

    /* lock the mutex, to access the requests list exclusively. */
    pthread_mutex_lock(&request_mutex);

    while (!globalData->termination_triggered) {
        if (num_requests > 0) {
            a_request = get_request();
            if (a_request) { /* got a request - handle it and free it */
                //TO DO - UNLOCCK MUTEX, CALL FUNCTION TO HANDLE REQUEST AND RELOCK MUTEX
                pthread_mutex_unlock(&request_mutex);
                handle_job(a_request->fd);
                free(a_request);
                pthread_mutex_lock(&request_mutex);
            }
        } else {
            pthread_cond_wait(&got_request, &request_mutex);
        }
    }
    printf("Quitting thread...\n");
}

int tHandler(char **opts) {
    int tIndex = findElemIndex(opts, "-t");
    if (tIndex != -1) {
        return atoi(opts[tIndex + 1]);
    } else
        return 10;
}

void *killProc(void *data) {
    struct timer_data *args = (struct timer_data *) data;
    sleep(args->timeout);
    if (kill(args->pid, 0) == -1)
        return NULL;
    kill(args->pid, SIGTERM);
    logSig(args->pid, "SIGTERM", args->logfd);
    sleep(5);
    if (kill(args->pid, 0) != -1) {
        kill(args->pid, SIGKILL);
        logSig(args->pid, "SIGKILL", args->logfd);
    }
}