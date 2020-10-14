#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAXDATASIZE 1000 /* max number of bytes we can get at once */


int main(int argc, char *argv[]) {
    struct hostent *he;
    struct sockaddr_in server_address;
    int socketfd, numbytes;
    char buf[MAXDATASIZE];

    if (argc < 3) {
        printf("Usage: controller <address> <port> {[-o out_file] [-log log_file] [-t seconds] <file> [arg...] | mem [pid] | memkill <percent>}\n");
        return 0;
    }

    if (!strcmp(argv[1], "--help")) {
        printf("Usage: controller <address> <port> {[-o out_file] [-log log_file] [-t seconds] <file> [arg...] | mem [pid] | memkill <percent>}\n");
        return 0;
    }

    if ((he = gethostbyname(argv[1])) == NULL) { /* get the host info */
        herror("gethostbyname");
        exit(1);
    }

    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }
    /* clear address struct */

    memset(&server_address, 0, sizeof(server_address));

    server_address.sin_family = AF_INET;            /* host byte order */
    server_address.sin_port = htons(atoi(argv[2])); /* short, network byte order */
    server_address.sin_addr = *((struct in_addr *) he->h_addr);

    if (connect(socketfd, (struct sockaddr *) &server_address,
                sizeof(struct sockaddr)) == -1) {
        fprintf(stderr, "Could not connect to overseer %s %s", argv[1], argv[2]);
        exit(1);
    }

    if ((numbytes = recv(socketfd, buf, MAXDATASIZE, 0)) == -1) {
        fprintf(stderr, "Could not receive message from overseer");
        exit(1);
    }

    buf[numbytes] = '\0';

    buf[numbytes] = '\0';

    printf("Message received from overseer: %s", buf);

    // do something to process message

    //send back something

    close(socketfd);

    return 0;

}
