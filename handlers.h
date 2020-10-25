#ifndef OVERSEER_HANDLERS
#define OVERSEER_HANDLERS

#include "handlers.c"

char *memHandler(pid_t *);

char *mempid_handler(pid_t);

void memkillHandler(char **, pid_t pInt[5]);

#endif