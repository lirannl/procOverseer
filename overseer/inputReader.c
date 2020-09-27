#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helperMethods.h"

#ifndef CMD_MAX_LENGTH
#define CMD_MAX_LENGTH 1000
#endif

#ifndef INPUT_MAX_LENGTH
#define INPUT_MAX_LENGTH 10000
#endif

void interpret_input(char** str_1, char** str_2)
{
    char inBuf[INPUT_MAX_LENGTH];
    fgets((char *__restrict)(&inBuf), INPUT_MAX_LENGTH, stdin);
    // Temporary direct input - will be replaced with reading the input from the controller
    *str_1 = strtok(inBuf, " ");
    char *test = strtok(NULL, inBuf);
    trimEndingWhitespace(&test);
    *str_2 = test;
}