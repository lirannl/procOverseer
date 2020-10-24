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

// Max number of bytes we can get at once ...
#define MAXDATASIZE 1000

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

    // If the first argument provided is --help display the usage message ...
    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        print_help(stdout);
        return 0;
    }

    // If an invalid number of arguments have been provided display the usage message ...
    if (argc < 4) {
        print_help(stderr);
        return 0;
    }

    // If host or port provided provided is invalid print the usage message ...
    if (!check_valid_host(argv[1]) || !check_valid_port(argv[2])) {
        print_help(stderr);
        return 0;
    }

    // Can only be mem or file with no args ...
    if (argc == 4) {

        // If the fourth argument is not "mem" or a filename print usage message ...
        if (strcmp(argv[3], "mem") != 0 && check_for_flag(argv[3])) {
            print_help(stderr);
            return 0;
        }
    }

    // Can only be mem [pid], memkill <percent>, or file with args ...
    if (argc == 5) {

        // If the fourth argument is mem but the fifth argument is not a valid int print usage message ...
        if (strcmp(argv[3], "mem") == 0) {
            if (!check_valid_int(argv[4])) {
                print_help(stderr);
                return 0;
            }

            // Else if the fourth argument is memkill but the fifth argument is not a valid percent print usage message ...
        } else if (strcmp(argv[3], "memkill") == 0) {
            if (!check_valid_percent(argv[4])) {
                print_help(stderr);
                return 0;
            }

            // Else if the fourth argument is invalid print usage message ...
        } else if (check_for_flag(argv[3])) {
            print_help(stderr);
            return 0;
        }
    }


    bool outfile_found, logfile_found, time_found;
    outfile_found = 0;
    logfile_found = 0;
    time_found = 0;

    // Can only be -o outfile, -log logfile, -t seconds, or a valid combination of the three where order matters ...
    if (argc >= 6) {
        for (int i = 3; i < argc; i++) {

            // If argv[3] is not a filename break out of loop ...
            if (i == 3 && !check_for_flag(argv[i])) {
                break;

                // If the current input is -o ...
            } else if (strcmp(argv[i], "-o") == 0) {

                // Ensure only one -o has been provided, there is at least two arguments following it,
                // and the following argument is not a input flag ...
                if (!outfile_found && i == 3 && i + 2 < argc && !check_for_flag(argv[i + 1])) {
                    outfile_found = 1;
                } else {
                    print_help(stderr);
                    return 0;
                }

                // If the current input is -log ...
            } else if (strcmp(argv[i], "-log") == 0) {

                // Ensure only one -log has been provided in the right location, there is at least two arguments following it,
                // if -o has been found then -log must be at argv[5],
                // and the following argument is not a input flag ...
                if (!logfile_found && ((outfile_found && i == 5) || i == 3) && i + 2 < argc &&
                    !check_for_flag(argv[i + 1])) {
                    logfile_found = 1;
                } else {
                    print_help(stderr);
                    return 0;
                }

                // Ensure only one -t has been provided in the right location, there is at least two arguments following it,
                // if -o or -log has been found then -t must be at argv[5],
                // if -o and -log has been found then -t must be at argv[7],
                // and the following argument is not a input flag ...
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

    // Putting output message into string to send to overseer ...
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

    // Ensure the output message is not blank ...
    if (!strlen(output_message)) {
        print_help(stderr);
        exit(1);
    }

    // If host is null print error ...
    if ((he = gethostbyname(argv[1])) == NULL) {
        herror("gethostbyname");
        exit(1);
    }

    // If socket creation fails print error ...
    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    // Clear address struct ...
    memset(&server_address, 0, sizeof(server_address));

    // Host byte order ...
    server_address.sin_family = AF_INET;

    // Network byte order
    server_address.sin_port = htons(atoi(argv[2]));
    server_address.sin_addr = *((struct in_addr *) he->h_addr);

    // If connecting to server fails print error ...
    if (connect(socketfd, (struct sockaddr *) &server_address,
                sizeof(struct sockaddr)) == -1) {
        fprintf(stderr, "Could not connect to overseer %s %s\n", argv[1], argv[2]);
        exit(1);
    }

    // Sending data to server failed ...
    if (send(socketfd, output_message, strlen(output_message), 0) < 0) {
        puts("Send failed");
        return 1;
    }

    // If no message was received from server close connection and exit ...
    if ((numbytes = recv(socketfd, buf, MAXDATASIZE, 0)) == -1) {
        fprintf(stderr, "Could not receive message from overseer");
        close(socketfd);
        exit(1);
    }

    buf[numbytes] = '\0';

    buf[numbytes] = '\0';

    // Print message received from server ...
    printf("Message received from overseer: %s\n", buf);

    // Closing socket ...
    close(socketfd);

    return 0;
}

/**
 * Checking if all characters in string are valid digits.
 * @param string: Input argument string
 * @return True if valid int
 */
bool check_valid_int(char *string) {
    u_long length = strlen(string);
    for (int i = 0; i < length; i++) {
        if (!isdigit(string[i])) {
            return 0;
        }
    }
    return 1;
}

/**
 * Checking if input string matches one of the argument flags.
 * @param string: Input argument string
 * @return True if input is a valid argument flag
 */
bool check_for_flag(char *string) {
    return strcmp(string, "-o") == 0 || strcmp(string, "-log") == 0 || strcmp(string, "-t") == 0;
}

/**
 *  Checking if all characters in string are valid digits and string is allowed to contain one decimal point.
 * @param string: Input argument string
 * @return True if input is a valid percent
 */
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

/**
 * Checking if input port is a valid int and within valid port range.
 * @param string: Input argument string
 * @return True if input is valid port
 */
bool check_valid_port(char *string) {
    if (!check_valid_int(string)) {
        return 0;
    }
    char *ptr;
    long port = strtol(string, &ptr, 10);
    return 1 <= port && port <= 65535;
}

/**
 * Checking if input is a valid host.
 * @param string : Input argument string
 * @return True if input is valid host
 */
bool check_valid_host(char *string) {
    struct in_addr addr = {0};
    return inet_pton(AF_INET, string, &addr) != 1;
}