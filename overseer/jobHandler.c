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
#include "memCollect.h"

pthread_mutex_t pidMutex = PTHREAD_MUTEX_INITIALIZER;
pid_t pidChild[NUM_THREADS];
char *memInfo[NUM_THREADS];

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
        dprintf(fd, results);
        close(fd);
        return 0;
    }
    int timeout = tHandler(opts);
    // -o handling
    int out;
    int oIndex = findElemIndex(opts, "-o");
    if (oIndex != -1) {
        out = open(opts[oIndex + 1], O_WRONLY | O_APPEND | O_CREAT, 0666);
    }
    // -log handling
    int log = fileno(stdout);
    int lIndex = findElemIndex(opts, "-log");
    if (lIndex != -1) {
        log = open(opts[lIndex + 1], O_WRONLY | O_APPEND | O_CREAT, 0666);
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
            // WRITE childPid TO PID ARRAY HERE
            pthread_mutex_lock(&pidMutex);
             memEntry_t *TempEntry = create_newEntry(TempEntry, childPid, getTime(), get_memory_usage(childPid), args[0], "args");
                memOverseer = entry_add(memOverseer, TempEntry);
            for (int i = 0; i < NUM_THREADS; i++) {
                if (pidChild[i] == 0) {
                    pidChild[i] = childPid;
                    memInfo[i] = results;
                    break;
                }
               
            }
                    
            pthread_mutex_unlock(&pidMutex);
            
            int status;
            wait(&status);
            if (success)
                terminateFile(childPid, status, log);
            else
                executeFileFail(fullExec, log);
            free(fullExec);
            // Remove childPid from array
            pthread_mutex_lock(&pidMutex);
            for (int i = 0; i < NUM_THREADS; i++) {
                //creates temprary memEntry and adds to the global linked list
                memEntry_t *TempEntry = create_newEntry(TempEntry, childPid, getTime(), get_memory_usage(childPid), args[0], "args");
                memOverseer = entry_add(memOverseer, TempEntry);
                
                if (pidChild[i] == childPid) {
                    pidChild[i] = 0;
                    //memInfo[i] = "nothing";
                    break;
                }
            }
            
            pthread_mutex_unlock(&pidMutex);
        }
    }
    if (valid_input == 2) // Special handlers do not fork into a new process
    {
        if (!strcmp(args[0], "mem")){
            pthread_mutex_lock(&pidMutex);
                //sends the linked list values to the controller
                if (send(fd, memHandler(pidChild), 40, 0) == -1){
                    perror("send");
                } 
            pthread_mutex_unlock(&pidMutex);
        }
        else if (!strcmp(args[0], "memkill")) {
            pthread_mutex_lock(&pidMutex);
            memkill_handler(args, pidChild, NUM_THREADS);
            pthread_mutex_unlock(&pidMutex);
        }
    }
    if (lIndex != -1) {
        close(log);
    }
    free(results);
    if (send(fd, "This is what you are looking for\n", 40, 0) == -1)
        perror("send");
    close(fd);
    return 0;
}

/**
 * Recieves message from controller and converts to a string for processing.
 * @param fd Controller socket file descriptor
 * @return message received from server
 */
char *recvMessage(int fd) {
    char received_buffer[500];
    char *msg;
    int recvLen;

    // Clear the buffer ...
    memset(received_buffer, 0, 500);

    // If nothing was received from controller print error ...
    if ((recvLen = recv(fd, received_buffer, 500, 0)) < 0) {
        fprintf(stderr, "Received invalid length value from controller: %d\n", recvLen);
        exit(1);
    } else {
        // Convert buffer to string ...
        msg = malloc(sizeof(received_buffer) + 1);
        for (int i = 0; i < recvLen; i++) {
            msg[i] = received_buffer[i];
        }
        msg[sizeof(received_buffer) + 1] = '\0';
        return msg;
    }
}

void *req_handler(void *data) {
    struct request *a_request;
    struct thread_info *info = (struct thread_info *) data;
    /* lock the mutex, to access the requests list exclusively. */
    pthread_mutex_lock(&request_mutex);

    int exit = 0;
    while (!exit) {
        if (num_requests > 0) {
            a_request = get_request();
            if (a_request) { /* got a request - handle it and free it */
                //TO DO - UNLOCCK MUTEX, CALL FUNCTION TO HANDLE REQUEST AND RELOCK MUTEX
                pthread_mutex_unlock(&request_mutex);
                if (a_request->fd > 0) // Only try to handle jobs with valid fds
                    handle_job(a_request->fd);
                else if (a_request->fd == -2)
                    exit = 1;
                free(a_request);
                pthread_mutex_lock(&request_mutex);
            }
        } else if (!exit) {
            pthread_cond_wait(&got_request, &request_mutex);
        }
    }
    free(info);
    pthread_mutex_unlock(&request_mutex);
    if (termination_triggered)
        add_request(-2, &request_mutex, &got_request); // Free the next thread
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
