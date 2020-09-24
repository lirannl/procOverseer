//
// Created by Cecil Hammett on 9/24/20.
//
#include <stdio.h>

int main(int argc, char *argv[]) {

    for (int i = 0; i < argc; i++) {
        printf("the %2d arg is: %s\n", i, argv[i]);
    }
    return 1;
}
