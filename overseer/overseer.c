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
#include <pthread.h>
#include "jobHandler.h"
#include "helperMethods.h"
#define ARRAY_SIZE 30
#define BACKLOG 10
#define RETURNED_ERROR -1

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

void connectionMade(struct in_addr ip)
{

    getTime();
    printf(" - connection received from <%s>\n", inet_ntoa(ip));
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

    pthread_t threadPool[THREADS_NUM];
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
        // The data-passing struct
        threadData dataToPass;
        // Load data onto the struct
        dataToPass.newfd = newfd;
        pthread_create(&threadPool[0], NULL, handle_job, &dataToPass);
        pthread_detach(threadPool[0]);
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
    int port = atoi(argv[1]);
    if (port > 65535 || port <= 0)
    {
        printf("Invalid port number. Must be between 1 and 65535.\n");
        return 1;
    }
    setSignals();
    runOverseer(port);
    printf("Have a good day\n");
}
