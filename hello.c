#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc > 1) {
        printf("Hello, %s\n", argv[1]);
        return 0;
    } else {
        printf("Please supply an argument.\n");
        return 1;
    }
}