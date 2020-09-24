#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc > 1)
    {
        printf("Hello, %s", argv[1]);
        return 0;
    }
    else 
    {
        printf("Please supply an argument.");
        return 1;
    }
}