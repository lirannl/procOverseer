#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "helperMethods.h"

typedef struct memCollection memCollection_t;

typedef struct memEntry {
    pid_t pid;
    char *time;
    int bytes;
    char *file;
    char *args;
} memEntry_t;

struct memCollection{
    memEntry_t *entry;
    memCollection_t *next;
};

memCollection_t *memOverseer;

memEntry_t *create_newEntry(memEntry_t* entry, pid_t newPID, char * newTime, int newBytes, char * newFile, char *newArgs){
    entry->pid=newPID;
    entry->time=newTime;
    entry->bytes=newBytes;
    entry->file=newFile;
    entry->args=newArgs;
}

//adds a new entry to the collection
memCollection_t *entry_add(memCollection_t *collection, memEntry_t *entry){
    memCollection_t *newEntry = (memCollection_t *)malloc(sizeof(memCollection_t));
    if(newEntry ==NULL){
        return NULL;
    }
    newEntry->entry = entry;
    newEntry->next = collection;
    return newEntry;
}

//request pid
memCollection_t *find_entry(memCollection_t *head, pid_t pid){
    for(; head!=NULL; head = head->next){
        if(pid == head->entry->pid){
            return head;
        }
    }
}

//deletes the entry based on pid
memCollection_t *entry_delete(memCollection_t *head, pid_t pid){
    memCollection_t *previous = NULL;
    memCollection_t *current = head;
    while(current != NULL){
        if (pid == current->entry->pid){
            memCollection_t *newhead = head;
            if(previous ==NULL){
                newhead = current->next;
            }else{
                previous->next = current->next;
                free(current);
                return newhead;
            }
            previous = current;
            current = current->next;
        }
    }
    return head;
}


//prints all entries in the list and sends them to the overseer
char *print_openEntry(memCollection_t *head,pid_t *pid, int fd){
  
    char *memOut = malloc(sizeof(char)*100);
    char memArray[100];
    for(int i = 0;i<5;i++){
        for(; head !=NULL; head =head->next){
            if(head->entry->pid == pid[i]){
                sprintf(memArray, "\n%d %d %s %s",head->entry->pid, head->entry->bytes, head->entry->file, head->entry->args);
                strcat(memOut, memArray);
                break;
            }
            
        }
    }
    
    return memOut;
     
}

//prints all entries in the list based on the pid. ## mem PID 
char *print_mempid(memCollection_t *head, pid_t pid, int fd){
    char *memOut = malloc(sizeof(char)*100);
    char memArray[100];
    for(; head !=NULL; head =head->next){
        if(head->entry->pid == pid){
            sprintf(memArray, "\n%s %d ",head->entry->time, head->entry->bytes);
            strcat(memOut, memArray);
        }
        
    }
     return memOut;
    
}

void memPid(memCollection_t *head, int fd){
    for(; head !=NULL; head =head->next){
        dprintf(fd,"%s %s", head->entry->time, head->entry->bytes);
    }
}