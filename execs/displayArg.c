//
// Created by Cecil Hammett on 9/24/20.
//
#include <stdio.h>

int main(int argc, char *argv[]) {

    for (int i = 0; i < argc; i++) {
        printf("the %2d arg is: %s\n", i, argv[i]);
    }
    printf("the 2nd arg entered is: %s\n", argv[2]);
    return 1;
}
