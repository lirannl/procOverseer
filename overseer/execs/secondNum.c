//
// Created by Cecil Hammett on 9/24/20.
//
#include <stdio.h>

int main(int numArgs, char *listArgs[]) {
    for (int i = 0; i < numArgs; i++) {
        printf("the %2d arg is: %s\n", i, listArgs[i]);
    }
}
