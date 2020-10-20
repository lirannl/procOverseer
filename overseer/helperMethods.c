#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

struct global {
    volatile int termination_triggered;
};

struct global *global = NULL;

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

// Returns -1 if item isn't in array - array of strings must be null-terminated
int findElemIndex(char **str_arr, char *target)
{
    for (int i = 0; str_arr[i] != NULL; i++)
    {
        if (!strcmp(str_arr[i], target)) return i;
    }
    return -1; // Not found
}

void getTime()
{
    time_t timer;
    char buffer[26];
    struct tm *conTime;
    char *dates = "yeet";
    char dateFormat[11];
    timer = time(NULL);
    conTime = localtime(&timer);
    int y = conTime->tm_year + 1900;
    int m = conTime->tm_mon + 1;
    int d = conTime->tm_mday;
    int h = conTime->tm_hour;
    int mi = conTime->tm_min;
    int s = conTime->tm_sec;

    snprintf(dateFormat, 6, "%i-%i-%i %i:%i:%i", y, m, d, h, mi, s);
    dates = dateFormat;
    printf("%s", dates);
}

void freeStrArr(char **strArr)
{
    for (int i = 0; strArr[i] != NULL; i++)
    {
        free(strArr[i]);
    }
}