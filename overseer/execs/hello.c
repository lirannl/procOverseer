#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc > 1)
    {
        printf("Hello, %s", argv[1]);
    }
    else 
    {
        printf("Please supply an argument.");
    }
}