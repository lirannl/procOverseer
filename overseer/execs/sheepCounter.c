#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void greetUser() {
    printf("Hello user!\n");
}

int main(int argc, char *argv[]) {
    greetUser();

    srand(time(NULL));
    int sleepPoint = rand() % 8192;
    int sheep = 0;
    while (sheep < sleepPoint)
    {
        printf("%d sheep, ", sheep);
        sheep++;
    }
    printf("\n\nGood night!\n");
}