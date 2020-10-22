#include <stdio.h>
#include <stdlib.h>

int factorial(int n) {
    if (n == 0)
        return 0;
    if (n == 1)
        return 1;
    return n * factorial(n - 1);
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("No number supplied.\n");
        return 1;
    }
    int number = atoi(argv[1]);
    if (argc == 2 && number >= 0 && number < 13) {
        printf("The factorial of %d is %d.\n", number, factorial(number));
        return 0;
    } else if (argc != 2)
        printf("Please input exactly one number as an argument.\n");
    else
        printf("The number you have entered is invalid. It must be a positive integer up to 12.\n");
    return 1;
}