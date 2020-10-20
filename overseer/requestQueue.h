#ifndef reqQueue
#define reqQueue

#include <pthread.h>
#include "requestQueue.c"
pthread_mutex_t request_mutex;
pthread_cond_t got_request;
void add_request(int, pthread_mutex_t*, pthread_cond_t*);
struct request *get_request();


#endif