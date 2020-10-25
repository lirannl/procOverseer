#include <unistd.h>
#include <stdio.h>
#include "helperMethods.h"

// Follow the array up until NULL and return its length (excluding the final NULL)
int determineLength(char **arr) {
    int i;
    for (i = 0; arr[i] != NULL; i++) {}
    return i;
}


// Finds the first non '-'-headed argument and sets a pointer to it
int findCmdArg(char **);

// Returns 1 if the input was successful
int interpret_input(char *input, char **args_arr, char **opts_arr) {
    char *split_input[MAX_ARGS];
    char *token = strtok(input, " ");
    // Go through the entire string
    int num_args;
    for (num_args = 0; token != NULL; num_args++) {
        // Copy the current token into the list
        split_input[num_args] = (char *) malloc(strlen(token));
        strcpy(split_input[num_args], token);
        token = strtok(NULL, " ");
    }
    split_input[num_args] = NULL; // Set the last item's next to NULL
    int cmdElement = findCmdArg(split_input);
    if (split_input[cmdElement] == NULL || !strcmp(split_input[cmdElement], "")) {
        freeStrArr(split_input);
        return 0;
    }
    char **opts = opts_arr;
    char **args = args_arr;
    // Loop through all the optionals
    int curr_arg;
    for (curr_arg = 0; curr_arg < cmdElement; curr_arg++) {
        if (curr_arg >= MAX_OPTIONALS * 2) return 0;
        opts[curr_arg] = (char *) malloc(strlen(split_input[curr_arg]));
        strcpy(opts[curr_arg], split_input[curr_arg]);
    }
    opts[curr_arg] = NULL;
    for (curr_arg = 0; curr_arg < determineLength(&split_input[cmdElement]); curr_arg++) {
        args[curr_arg] = (char *) malloc(strlen(split_input[cmdElement + curr_arg]));
        strcpy(args[curr_arg], split_input[cmdElement + curr_arg]);
    }
    args[curr_arg] = NULL;
    int validReturn;
    if (!strcmp(split_input[cmdElement], "mem") || !strcmp(split_input[cmdElement], "memkill"))
        validReturn = 2;
    else
        validReturn = 1;
    freeStrArr(split_input);
    return validReturn;
}

// returns the position of the command arg, or -1 if there isn't one
int findCmdArg(char **split_input) {
    // Set this to 1 to ignore the next argument
    int ignoreArg = 0;
    int i;
    for (i = 0; split_input[i] != NULL; i++) {
        if (split_input[i][0] == '-') {
            if (!ignoreArg)
                ignoreArg = 1;
            else {
                // invalid input
                i = -1;
                break;
            }
        } else {
            // Stop searching, command argument has been found
            if (!ignoreArg)
                break;
                // Stop ignoring the next arguments
            else
                ignoreArg = 0;
        }
    }
    return i;
}