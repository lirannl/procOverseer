#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
#include <regex.h>
#include <arpa/inet.h>

#define MAXDATASIZE 1000 /* max number of bytes we can get at once */


bool check_valid_host();

bool check_valid_port(char *string);

bool check_valid_percent(char *string);

bool check_for_flag(char *string);

bool check_valid_int(char *string);

void print_help(FILE *output) {
    fprintf(output, "Usage: controller <address> <port> {[-o out_file] [-log log_file] [-t seconds] <file> [arg...] "
                    "| mem [pid] | memkill <percent>}\n");
}

int main(int argc, char *argv[]) {
    struct hostent *he;
    struct sockaddr_in server_address;
    int socketfd, numbytes;
    char buf[MAXDATASIZE];

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        print_help(stdout);
        return 0;
    }

    if (argc < 4) {
        print_help(stderr);
        return 0;
    }

    if (!check_valid_host(argv[1]) || !check_valid_port(argv[2])) {
        print_help(stderr);
        return 0;
    }

    if (argc == 4) {
        if (strcmp(argv[3], "mem") != 0 || check_for_flag(argv[3])) {
            print_help(stderr);
            return 0;
        }
    }

    if (argc == 5) {
        if (strcmp(argv[3], "mem") == 0) {
            if (!check_valid_int(argv[4])) {
                print_help(stderr);
                return 0;
            }
        } else if (strcmp(argv[3], "memkill") == 0) {
            if (!check_valid_percent(argv[4])) {
                print_help(stderr);
                return 0;
            }
        } else if (check_for_flag(argv[3])) {
            print_help(stderr);
            return 0;
        }
    }


    bool outfile_found, logfile_found, time_found;
    outfile_found = 0;
    logfile_found = 0;
    time_found = 0;

    if (argc >= 6) {
        for (int i = 3; i < argc; i++) {
            if (i == 3 && !check_for_flag(argv[i])) {
                break;
            } else if (strcmp(argv[i], "-o") == 0) {
                if (!outfile_found && i == 3 && i + 2 < argc && !check_for_flag(argv[i + 1])) {
                    outfile_found = 1;
                } else {
                    print_help(stderr);
                    return 0;
                }
            } else if (strcmp(argv[i], "-log") == 0) {
                if (!logfile_found && ((outfile_found && i == 5) || i == 3) && i + 2 < argc &&
                    !check_for_flag(argv[i + 1])) {
                    logfile_found = 1;

                } else {
                    print_help(stderr);
                    return 0;
                }
            } else if (strcmp(argv[i], "-t") == 0) {
                if (!time_found && (i == 3 || ((outfile_found || logfile_found) && i == 5) ||
                                    (outfile_found && logfile_found && i == 7)) &&
                    (i + 2 < argc && check_valid_int(argv[i + 1]))) {
                    time_found = 1;
                } else {
                    print_help(stderr);
                    return 0;
                }
            }
        }
    }

    char output_message[10000];
    memset(output_message, '\0', sizeof(char) * 1000);

    for (int i = 3; i < argc; i++) {
        if (i == 0) {
            strcpy(output_message, argv[i]);
        } else {
            strcat(output_message, " ");
            strcat(output_message, argv[i]);
        }
    }

    if (!strlen(output_message)) {
        print_help(stderr);
        exit(1);
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
        fprintf(stderr, "Could not connect to overseer %s %s\n", argv[1], argv[2]);
        exit(1);
    }

    fprintf(stdout, "Message to server: %s \n", output_message);

    //Send some data
    if (send(socketfd, output_message, strlen(output_message), 0) < 0) {
        puts("Send failed");
        return 1;
    }

    if ((numbytes = recv(socketfd, buf, MAXDATASIZE, 0)) == -1) {
        fprintf(stderr, "Could not receive message from overseer");
        close(socketfd);
        exit(1);
    }

    buf[numbytes] = '\0';

    buf[numbytes] = '\0';

    printf("Message received from overseer: %s\n", buf);

    close(socketfd);

    return 0;
}

bool check_valid_int(char *string) {
    u_long length = strlen(string);
    for (int i = 0; i < length; i++) {
        if (!isdigit(string[i])) {
            return 0;
        }
    }
    return 1;
}

bool check_for_flag(char *string) {
    return strcmp(string, "-o") == 0 || strcmp(string, "-log") == 0 || strcmp(string, "-t") == 0;
}

bool check_valid_percent(char *string) {
    u_long length = strlen(string);
    bool found_decimal = 0;

    for (int i = 0; i < length; i++) {
        if ((string[i] == '.' && !found_decimal)) {
            found_decimal = 1;
        } else if (!isdigit(string[i])) {
            return 0;
        }
    }
    return 1;
}

bool check_valid_port(char *string) {
    if (!check_valid_int(string)) {
        return 0;
    }
    char *ptr;
    long port = strtol(string, &ptr, 10);
    return 1 <= port && port <= 65535;
}

bool check_valid_host(char *string) {
    struct in_addr addr = {0};
    return inet_pton(AF_INET, string, &addr) != 1;
}