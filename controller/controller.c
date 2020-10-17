#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ctype.h>

#define MAXDATASIZE 1000 /* max number of bytes we can get at once */


int main(int argc, char *argv[]) {
    struct hostent *he;
    struct sockaddr_in server_address;
    int socketfd, numbytes;
    double percent;
    char buf[MAXDATASIZE];

    if (argc < 3) {
        printf("Usage: controller <address> <port> {[-o out_file] [-log log_file] [-t seconds] <file> [arg...] "
               "| mem [pid] | memkill <percent>}\n");
        return 0;
    }

    if (!strcmp(argv[1], "--help")) {
        printf("Usage: controller <address> <port> {[-o out_file] [-log log_file] [-t seconds] "
               "<file> [arg...] | mem [pid] | memkill <percent>}\n");
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

    // if argv > 3 have to check for -o, -log, -t
    // then get file and collect arguments until |

    if (argc == 4) {
        // if argc == 4 it can be mem or file with no args
        // if argv is 4 and contains 'mem'
        if (strcmp(argv[3], "mem") == 0) {

        }


    } else if (argc == 5) {
        // can be memkill or file with 1 arg or mem with [pid]
        // if argv is 4 and contains 'memkill' it is memkill <percent>
        if (strcmp(argv[3], "memkill") == 0) {
            // verify <percent> , int

        } else if (strcmp(argv[3], "mem") == 0) {
            //verify [pid]
            int length = strlen(argv[3]);
            for (int i = 0; i < length; i++) {
                if (!isdigit(argv[3][i])) {
                    printf("Usage: controller <address> <port> {[-o out_file] [-log log_file] [-t seconds] <file> [arg...] "
                           "| mem [pid] | memkill <percent>}\n");
                    return 0;
                }
            }
        } else if (strcmp(argv[3], "-o") == 0 || strcmp(argv[3], "-log") == 0 || strcmp(argv[3], "-t") == 0) {
            printf("Usage: controller <address> <port> {[-o out_file] [-log log_file] [-t seconds] <file> [arg...] "
                   "| mem [pid] | memkill <percent>}\n");
            return 0;
        }
    } else if (argc == 6 || argc == 7) {
        if (strcmp(argv[3], "-o") == 0) {
            //check that log and t does not exist
            if (strcmp(argv[5], "-log") == 0 || strcmp(argv[5], "-t") == 0) {
                printf("Usage: controller <address> <port> {[-o out_file] [-log log_file] [-t seconds] <file> [arg...] "
                       "| mem [pid] | memkill <percent>}\n");
                return 0;
            }
        } else if (strcmp(argv[3], "-log") == 0) {
            if (strcmp(argv[5], "-o") == 0 || strcmp(argv[5], "-t") == 0) {
                printf("Usage: controller <address> <port> {[-o out_file] [-log log_file] [-t seconds] <file> [arg...] "
                       "| mem [pid] | memkill <percent>}\n");
                return 0;
            }

        } else if (strcmp(argv[3], "-t") == 0) {
            if (strcmp(argv[5], "-log") == 0 || strcmp(argv[5], "-o") == 0) {
                printf("Usage: controller <address> <port> {[-o out_file] [-log log_file] [-t seconds] <file> [arg...] "
                       "| mem [pid] | memkill <percent>}\n");
                return 0;
            }
        }
    } else if (argc == 8 || argc == 9) {
        if (strcmp(argv[3], "-o") == 0) {

        } else if (strcmp(argv[3], "-log") == 0) {
            if (strcmp(argv[5], "-o") == 0) {
                printf("Usage: controller <address> <port> {[-o out_file] [-log log_file] [-t seconds] <file> [arg...] "
                       "| mem [pid] | memkill <percent>}\n");
                return 0;
            }

        } else if (strcmp(argv[3], "-t") == 0) {
            if (strcmp(argv[5], "-log") == 0 || strcmp(argv[5], "-o") == 0) {
                printf("Usage: controller <address> <port> {[-o out_file] [-log log_file] [-t seconds] <file> [arg...] "
                       "| mem [pid] | memkill <percent>}\n");
                return 0;
            }
        }
    }
    else if (argc == 10 || argc == 11) {
        if (strcmp(argv[3], "-o") == 0) {
            if(strcmp(argv[3], "-o") == 0)

        } else if (strcmp(argv[3], "-log") == 0) {
            if (strcmp(argv[5], "-o") == 0) {
                printf("Usage: controller <address> <port> {[-o out_file] [-log log_file] [-t seconds] <file> [arg...] "
                       "| mem [pid] | memkill <percent>}\n");
                return 0;
            }

        } else if (strcmp(argv[3], "-t") == 0) {
            if (strcmp(argv[5], "-log") == 0 || strcmp(argv[5], "-o") == 0) {
                printf("Usage: controller <address> <port> {[-o out_file] [-log log_file] [-t seconds] <file> [arg...] "
                       "| mem [pid] | memkill <percent>}\n");
                return 0;
            }
        }
    }




    // check if first one is -o
    // if it is, check if out_file exist
    // if you have -o need to have characters after that
    //-o has to be first, ensure order is correct
    // then get file name and arguments

    //if you have -log something has to be after

    // if you have -t must be followed by int (seconds)

    // after this make sure there is a file name

    // once verified pass to server

    // if any of the checks fail print the --help (usage) message

    // if any of the above arguments are present cant have the below arguments

    // check for mem[pid]
    // check for memkill

    //send back something

    close(socketfd);

    return 0;

}
