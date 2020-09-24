//
// Created by Cecil Hammett on 9/24/20.
//
#include <stdio.h>
#include <stdlib.h>

int rolls[71];

void roll_dice(void) {
    srand(102161);
    for (int i = 0; i < 71; ++i) {
        rolls[i] = 1 + rand() % 5;
    }
}

int main(void) {

    for (int i = 0; i < 71; i++) {
        rolls[i] = 0xCAB202;
    }

    roll_dice();

    //  Display contents of array rolls.
    for (int i = 0; i < 71; i++) {
        if (i > 0) {
            printf(",");
        }
        printf("%d", rolls[i]);
    }

    printf("\n");

    return 0;
}