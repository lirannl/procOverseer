#include <pthread.h>
#include <stdio.h>

pthread_mutex_t request_mutex;
pthread_cond_t got_request;

int num_requests = 0;

struct request {
    int fd; // File descriptor of input stream
    struct request *next; // Pointer to next request (or null)
};

struct request *requests = NULL;
struct request *last_request = NULL;

// Adds a request to the list
void add_request(int fd, pthread_mutex_t *p_mutex, pthread_cond_t *p_cond_var) {
    struct request *a_request;

    a_request = (struct request *) malloc(sizeof(struct request));
    if (!a_request) {
        // Malloc failure
        fprintf(stderr, "add_request: out of memory\n");
        exit(1);
    }
    a_request->fd = fd;
    a_request->next = NULL;

    pthread_mutex_lock(p_mutex);

    if (num_requests == 0) {
        // If the list is empty
        requests = a_request;
        last_request = a_request;
    } else {
        last_request->next = a_request;
        last_request = a_request;
    }

    num_requests++;

    pthread_mutex_unlock(p_mutex);

    pthread_cond_signal(p_cond_var);
}

// gets the first pending request from the requests list - removing it from the list.
// returns a pointer to the removed request, or NULL if none.
struct request *get_request() {
    struct request *a_request; // The request's pointer

    if (num_requests > 0) {
        a_request = requests;
        requests = a_request->next;
        if (requests == NULL) { // In case of final request
            last_request = NULL;
        }
        // Decrease the number of pending requests
        num_requests--;
    } else {
        // Empty list
        a_request = NULL;
    }

    return a_request;
}