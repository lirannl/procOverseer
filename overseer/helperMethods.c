#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int trimEndingWhitespace(char *string)
{
    char finalChar = string[strlen(string) - 1];
    if (finalChar == '\n' || finalChar == ' ')
    {
        string[strlen(string) - 1] = '\0';
        return 1; // Return 1 if there was a whitespace
    }
    return 0;
}

// Free memory allocated to arrays of pointers
void cleanup_arr(void **__arr)
{
    for(int i = 0; i < (sizeof __arr / sizeof (void *)); i++)
    {
        free(__arr[i]);
    }
}