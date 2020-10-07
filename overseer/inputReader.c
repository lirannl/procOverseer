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

#ifndef MAX_ARGS
#define MAX_ARGS 100
#endif

#ifndef MAX_OPTIONALS
#define MAX_OPTIONALS 3
#endif

typedef struct strNode
{
    char content[CMD_MAX_LENGTH];
    struct strNode *next;
} argList;

// Free the memory allocated to the list
void cleanup_list(argList *head)
{
}

// Follow the list up until NULL and return its length (excluding the final NULL)
int determineLength(argList *head)
{
    int length = 1;
    while (head != NULL)
    {
        length++;
        head = head->next;
    }
    return length - 1;
}

// Finds the first non '-'-headed argument and sets a pointer to it
void findCmdArg(argList **cmdPtr, argList *list);

// Returns 1 if the input was successful
int interpret_input(char *str, char **args_arr, char **opts_arr)
{
    char inBuf[INPUT_MAX_LENGTH];
    // Temporary direct input - will be replaced with reading the input from the controller
    fgets((char *__restrict)(&inBuf), INPUT_MAX_LENGTH, stdin);
    trimEndingWhitespace(inBuf);
    if (inBuf[0] == '\0')
        return 0;
    argList args_list;
    char *token = strtok(inBuf, " ");
    argList *current_item = &args_list;
    // Go through the entire string
    int num_args;
    for (num_args = 0; token != NULL; num_args++)
    {
        // Copy the current token into the list
        strcpy(current_item->content, token);
        argList *newItem = malloc(sizeof *newItem);
        current_item->next = newItem;
        token = strtok(NULL, " ");
        current_item = current_item->next; // Go to the next item
    }
    argList *cmdElement = NULL;
    findCmdArg(&cmdElement, &args_list);
    if (cmdElement == NULL || !strcmp(cmdElement->content, ""))
    {
        cleanup_list(&args_list);
        return 0;
    }
    // Set the command variable to the command
    //str = cmdElement->content;
    strcpy(str, cmdElement->content);
    // Go to the first argument
    current_item = &args_list;
    // Loop through all the optionals
    int curr_arg;
    for (curr_arg = 0; current_item != cmdElement; curr_arg++)
    {
        if (curr_arg >= MAX_OPTIONALS * 2)
        {
            cleanup_list(&args_list);
            return 0;
        }
        opts_arr[curr_arg] = realloc(opts_arr[curr_arg], sizeof current_item->content);
        strcpy(opts_arr[curr_arg], current_item->content);
        current_item = current_item->next;
    }
    opts_arr[curr_arg] = NULL;
    // Go to the first command argument in the list
    current_item = cmdElement;
    for (curr_arg = 0; curr_arg < determineLength(cmdElement); curr_arg++)
    {
        args_arr[curr_arg] = realloc(args_arr[curr_arg], sizeof current_item->content);
        strcpy(args_arr[curr_arg], current_item->content);
        current_item = current_item->next;
    }
    args_arr[determineLength(cmdElement) - 1] = NULL;
    int validReturn;
    if (!strcmp(cmdElement->content, "mem") || !strcmp(cmdElement->content, "memkill"))
        validReturn = 2;
    else
        validReturn = 1;
    cleanup_list(&args_list);
    return validReturn;
}

void findCmdArg(argList **cmdPtr, argList *list)
{
    // Set this to 1 to ignore the next argument
    int ignoreArg = 0;
    // Start searching from element 0
    *cmdPtr = list;
    while (*cmdPtr != NULL)
    {
        if ((*cmdPtr)->content[0] == '-')
        {
            if (!ignoreArg)
                ignoreArg = 1;
            else
            {
                // invalid input
                *cmdPtr = NULL;
                break;
            }
        }
        else
        {
            // Stop searching, command argument has been found
            if (!ignoreArg)
                break;
            // Stop ignoring the next arguments
            else
                ignoreArg = 0;
        }
        *cmdPtr = (*cmdPtr)->next;
    }
}