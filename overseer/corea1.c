#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <math.h>

#define ARRAY_SIZE 30
#define BACKLOG 10
#define RETURNED_ERROR -1


void getTime(){
    time_t timer;
    char buffer[26];
    struct tm* conTime;
    char *dates= "yeet";
    char dateFormat[11];
    timer = time(NULL);
    conTime = localtime(&timer);
        int y = conTime->tm_year+1900;
        int m = conTime->tm_mon+1;
        int d = conTime->tm_mday;
        int h = conTime->tm_hour;
        int mi = conTime->tm_min;
        int s = conTime->tm_sec;
        
        sprintf(dateFormat, "%i-%i-%i %i:%i:%i", y,m,d,h,mi,s);
        dates = dateFormat;
        printf("%s", dates);
         
        
        
}
void connectionMade(struct in_addr ip){
     
   getTime();
     printf(" - connection received from <%s>\n", inet_ntoa(ip));
}
void executeFileStart(char *fileName){
   getTime();
 printf(" - attempting to execute %s\n", fileName);
}
void executeFileFinish(char *fileName, char *pids){
   getTime();
 printf(" - %s has been executed with pid %s\n", fileName, pids);
}
void terminateFile(char *pids, char *statusCode){
   getTime();
 printf(" - %s  has terminated with status code %s\n", pids, statusCode);
}
void executeFileFail(char *fileName){
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
    if (recvLen != sizeof(netLen)) {
        fprintf(stderr, "recv got invalid length value (got %d)\n", recvLen);
        exit(1);
    }
    int len = ntohl(netLen);
    msg = malloc(len + 1);
    if (recv(fd, msg, len, 0) != len) {
        fprintf(stderr, "recv got invalid length msg\n");
        exit(1);
    }
    msg[len] = '\0';

    return msg;
}

int runOverseer(int port){
    printf("Overseer now listening on port: %i\n", port);


    //######### START UP SERVER #################
    int sockfd, newfd;
    struct sockaddr_in overseer_addr;
    struct sockaddr_in controller_addr;
    socklen_t sin_size;
    int i = 0;

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))== -1)
    {
        perror("socket");
        exit(1);
    }
    int opt_enable = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt_enable, sizeof(opt_enable));
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &opt_enable, sizeof(opt_enable));


    //######## ASSIGN OVERSEER STUFFS ###########
    memset(&overseer_addr, 0, sizeof(overseer_addr));
    overseer_addr.sin_family = AF_INET; // asign overseers byte order
    overseer_addr.sin_port = htons(port); // Asign overseers Port
    overseer_addr.sin_addr.s_addr = INADDR_ANY; // Asign the overseer's IP
    //######## ASSIGN OVERSEER STUFFS ###########


    if (bind(sockfd, (struct sockaddr *)&overseer_addr, sizeof(struct sockaddr)) ==-1)
    {
        perror("bind()");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) ==-1)
    {
        perror("listen()");
        exit(1);
    }
    //######### START UP SERVER #################

    //######### LISTENING #################
    while(1){

        //######## ASSIGN CONTROLLER STUFFS ###########
        sin_size = sizeof(struct sockaddr_in);
        if((newfd = accept(sockfd, (struct sockaddr*)&controller_addr, &sin_size))== -1){
            perror("accept()");
            continue;
        }
        connectionMade(controller_addr.sin_addr);
        
        //######## ASSIGN CONTROLLER STUFFS ###########

        if (!fork())
        { /* this is the child process */

            /* Call method to recieve array data */
            
            char *results = recvMessage(newfd);
            printf("%s \n",results);
            free(results);
        if (send(newfd, "All of array data received by server\n", 40, 0) == -1)
                perror("send");
            close(newfd);
            exit(0);
        }
        else
        {
            close(newfd); /* parent doesn't need this */
        }
        // while (waitpid(-1, NULL, WNOHANG) > 0)
        //     ; /* clean up child processes */



    }
    //######### LISTENING #################
    
            


    //######## CLOSE EVERYTHING #################
    
}

int main(int argc, char * argv[]) {
    if (argc != 2) {
        printf("No port supplied. Please only provide a port number.\n");
        return 1;
    }
    int number = atoi(argv[1]);
    if (number > 65535 || number <= 0) {
        printf("Invalid port number. Must be between 1 and 65535.\n");
        return 1;
    }
    
    runOverseer(number);
    printf("Have a good day\n");
}

