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

typedef struct strNode
{
    char content[CMD_MAX_LENGTH];
    struct strNode *next;
} argList;

// Free the memory allocated to the list
void cleanup_list(argList *head)
{
    argList *curr = head;
    while (curr->next != NULL)
    {
        argList *next = curr->next;
        printf("Freeing \"%s\".\n", curr->content);
        curr->next = NULL;
        //free(curr);
        curr = next;
    }
}

void interpret_input(char **str, char ***args_arr)
{
    char inBuf[INPUT_MAX_LENGTH];
    // Temporary direct input - will be replaced with reading the input from the controller
    fgets((char *__restrict)(&inBuf), INPUT_MAX_LENGTH, stdin);
    trimEndingWhitespace(inBuf);
    argList args_list;
    char *token = strtok(inBuf, " ");
    argList *current_item = &args_list;
    //strcpy(args_list.content, token);
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
    *str = args_list.content;
    // Go back to the beginning of the list
    current_item = &args_list;
    // Create an accessor variable
    char **arr_acc = *args_arr;
    for (int curr_arg = 0; curr_arg < num_args; curr_arg++)
    {
        arr_acc[curr_arg] = malloc(sizeof current_item->content);
        strcpy(arr_acc[curr_arg], current_item->content);
        current_item = current_item->next;
    }
    arr_acc[num_args] = NULL;
    cleanup_list(&args_list);
}