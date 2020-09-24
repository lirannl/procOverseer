#include <stdio.h>
#include <stdlib.h>

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
    printf("Listening on port %d\n", number);
}