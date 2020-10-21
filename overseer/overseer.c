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
#include "requestQueue.h"
#define ARRAY_SIZE 30
#define BACKLOG 10
#define RETURNED_ERROR -1
#define NUM_THREADS 5

void term(int signum)
{
    printf("\b\bCaught %d, terminating...\n", signum);
    global->termination_triggered = 1;
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

    char * time = getTime();
    printf("%s - connection received from %s\n", time, inet_ntoa(ip));
    free(time);
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

    //######## CREATE THREADPOOL #############
    pthread_t threadPool[NUM_THREADS];
    int threadIds[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) threadIds[i] = pthread_create(&threadPool[i], NULL, req_handler, (void *)global);
    
    //######### START UP SERVER #################

    //######### LISTENING ################clea#
    while (!global->termination_triggered)
    {
        //######## ASSIGN CONTROLLER STUFFS ###########
        sin_size = sizeof(struct sockaddr_in);
        if ((newfd = accept(sockfd, (struct sockaddr *)&controller_addr, &sin_size)) == -1)
        {
            if (!global->termination_triggered)
                perror("accept()");
            continue;
        }
        // Log the connection
        connectionMade(controller_addr.sin_addr);
        add_request(newfd, &request_mutex, &got_request);
    }

    //######## CLOSE EVERYTHING AND PERFORM CLEANUP #################
    for (int i = 0; i < NUM_THREADS; i++)
    {
        //pthread_join(threadPool[i], NULL);
    }
}

int main(int argc, char *argv[])
{
    // Launch arg validation
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
    global = malloc(sizeof (struct global));
    global->termination_triggered = 0;
    pthread_mutex_init(&request_mutex, NULL);
    pthread_cond_init(&got_request, NULL);
    setSignals();
    runOverseer(port);
    free(global);
    printf("Have a good day\n");
}
