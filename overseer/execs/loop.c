//
// Created by Cecil Hammett on 9/24/20.
//

#include <stdio.h>

int main(int argc, char *argv[]) {

    if (argc == 2) {
        printf("The argument given is %s\n", argv[1]);
    } else if (argc > 2) {
        printf("Too many arguments have been given.\n");
    } else {
        printf("At least one argument is expected.\n");
    }
}
