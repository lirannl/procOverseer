#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <math.h>
#include "inputReader.h"
#include "handlers.h"

#define ARRAY_SIZE 30
#define BACKLOG 10
#define RETURNED_ERROR -1
#define CMD_MAX_LENGTH 1000
#define INPUT_MAX_LENGTH 10000
#define MAX_ARGS 100
#define MAX_OPTIONALS 3

volatile sig_atomic_t termination_triggered = 0;

void term(int signum)
{
    printf("\b\bCaught %d, terminating...\n", signum);
    termination_triggered = 1;
}

void setSignals()
{
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = term;
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGINT, &action, NULL);
}

void getTime()
{
    time_t timer;
    char buffer[26];
    struct tm *conTime;
    char *dates = "yeet";
    char dateFormat[11];
    timer = time(NULL);
    conTime = localtime(&timer);
    int y = conTime->tm_year + 1900;
    int m = conTime->tm_mon + 1;
    int d = conTime->tm_mday;
    int h = conTime->tm_hour;
    int mi = conTime->tm_min;
    int s = conTime->tm_sec;

    sprintf(dateFormat, "%i-%i-%i %i:%i:%i", y, m, d, h, mi, s);
    dates = dateFormat;
    printf("%s", dates);
}

void connectionMade(struct in_addr ip)
{

    getTime();
    printf(" - connection received from <%s>\n", inet_ntoa(ip));
}
void executeFileStart(char *fileName)
{
    getTime();
    printf(" - attempting to execute %s\n", fileName);
}
void executeFileFinish(char *fileName, char *pids)
{
    getTime();
    printf(" - %s has been executed with pid %s\n", fileName, pids);
}
void terminateFile(char *pids, char *statusCode)
{
    getTime();
    printf(" - %s  has terminated with status code %s\n", pids, statusCode);
}
void executeFileFail(char *fileName)
{
    getTime();
    printf(" - could not execute %s\n", fileName);
}

int *Receive_Array_Int_Data(int socket_identifier, int size)
{
    int number_of_bytes, i = 0;
    uint16_t statistics;

    int *results = malloc(sizeof(int) * size);
    for (i = 0; i < size; i++)
    {
        if ((number_of_bytes = recv(socket_identifier, &statistics, sizeof(uint16_t), 0)) == RETURNED_ERROR)
        {
            perror("recv");
            exit(EXIT_FAILURE);
        }
        results[i] = ntohs(statistics);
    }
    return results;
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

int runOverseer(int port)
{
    printf("Overseer now listening on port: %i\n", port);

    //######### START UP SERVER #################
    int sockfd, newfd;
    struct sockaddr_in overseer_addr;
    struct sockaddr_in controller_addr;
    socklen_t sin_size;
    int i = 0;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }
    int opt_enable = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt_enable, sizeof(opt_enable));
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &opt_enable, sizeof(opt_enable));

    //######## ASSIGN OVERSEER STUFFS ###########
    memset(&overseer_addr, 0, sizeof(overseer_addr));
    overseer_addr.sin_family = AF_INET;         // asign overseers byte order
    overseer_addr.sin_port = htons(port);       // Asign overseers Port
    overseer_addr.sin_addr.s_addr = INADDR_ANY; // Asign the overseer's IP
    //######## ASSIGN OVERSEER STUFFS ###########

    if (bind(sockfd, (struct sockaddr *)&overseer_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("bind()");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("listen()");
        exit(1);
    }
    //######### START UP SERVER #################

    //######### LISTENING #################
    while (!termination_triggered)
    {

        //######## ASSIGN CONTROLLER STUFFS ###########
        sin_size = sizeof(struct sockaddr_in);
        if ((newfd = accept(sockfd, (struct sockaddr *)&controller_addr, &sin_size)) == -1)
        {
            if (!termination_triggered)
                perror("accept()");
            continue;
        }
        connectionMade(controller_addr.sin_addr);

        //######## ASSIGN CONTROLLER STUFFS ###########

        //Fork
        { /* this is the child process */

            /* Call method to recieve array data */
            char *results = recvMessage(newfd);
            char *cmd = calloc(CMD_MAX_LENGTH, sizeof(char));
            char **args = calloc(MAX_ARGS, sizeof(char *));
            char **opts = calloc((MAX_OPTIONALS * 2) + 1, sizeof(char *));
            int valid_input = 0;
            valid_input = interpret_input(results, cmd, args, opts);
            //Fork
            {
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
            //else if (valid_input == 2) // Special handlers do not fork into a new process
            {
                if (!strcmp(cmd, "mem"))
                    memHandler();
                else if (!strcmp(cmd, "memkill"))
                    memkillHandler();
            }
            free(args);
            free(opts);
            free(results);
            if (send(newfd, "All of array data received by server\n", 40, 0) == -1)
                perror("send");
            close(newfd);
            exit(0);
        }
        /*else
        {*/
            //close(newfd); /* parent doesn't need this */
        //}
    }
    //######### LISTENING #################

    //######## CLOSE EVERYTHING AND PERFORM CLEANUP #################
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("No port supplied. Please only provide a port number.\n");
        return 1;
    }
    int number = atoi(argv[1]);
    if (number > 65535 || number <= 0)
    {
        printf("Invalid port number. Must be between 1 and 65535.\n");
        return 1;
    }
    setSignals();
    runOverseer(number);
    printf("Have a good day\n");
}
