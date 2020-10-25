#ifndef memCellect
#define memCellect


#include "memCollect.c"



memCollection_t *memOverseer;
memCollection_t *entry_add(memCollection_t *, memEntry_t *);
memCollection_t *find_entry(memCollection_t *, pid_t);
memCollection_t *entry_delete(memCollection_t *, pid_t);

memEntry_t *create_newEntry(memEntry_t *, pid_t, char *, int, char *, char *);
char *print_entry(memCollection_t *, int);
void print_mempid(memCollection_t *, pid_t, int);
#endif

