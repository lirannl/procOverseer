#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

struct thread_info {
    int thread_num;
};

volatile int termination_triggered;

struct global *global = NULL;


// Returns -1 if item isn't in array - array of strings must be null-terminated
int findElemIndex(char **str_arr, char *target) {
    for (int i = 0; str_arr[i] != NULL; i++) {
        if (!strcmp(str_arr[i], target))
            return i;
    }
    return -1; // Not found
}

char *uniteStrArr(char **strArr) {
    int totalLength = strlen(strArr[0]);
    for (int i = 1; strArr[i] != NULL; i++)
        totalLength += strlen(strArr[i]);
    char *out = calloc(totalLength, sizeof ' ');
    strcpy(out, strArr[0]);
    if (strArr[1] == NULL)
        return out;
    for (int i = 1; strArr[i] != NULL; i++) {
        strcat(out, " ");
        strcat(out, strArr[i]);
    }
    return out;
}

#define DATEBUF_SIZE 20

char *getTime() {
    time_t timer;
    struct tm *conTime;
    char dateFormat[DATEBUF_SIZE];
    timer = time(NULL);
    conTime = localtime(&timer);
    int y = conTime->tm_year + 1900;
    int m = conTime->tm_mon + 1;
    int d = conTime->tm_mday;
    int h = conTime->tm_hour;
    int mi = conTime->tm_min;
    int s = conTime->tm_sec;

    snprintf(dateFormat, DATEBUF_SIZE, "%d-%02d-%02d %02d:%02d:%02d", y, m, d, h, mi, s);
    char *dates = malloc(sizeof(char) * strlen(dateFormat));
    strcpy(dates, dateFormat);
    return dates;
}

void freeStrArr(char **strArr) {
    for (int i = 0; strArr[i] != NULL; i++) {
        free(strArr[i]);
    }
}