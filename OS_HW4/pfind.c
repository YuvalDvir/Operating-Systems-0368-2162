#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdatomic.h>
#include <limits.h>
#include <errno.h>

/******************************************************************************
*                                                                             *
*        Operating Systems (0368-2162) - Parallel File Find Assignment        *
*                                                                             *
*******************************************************************************/

#define EMPTY (1)
#define NON_EMPTY (0)

static pthread_mutex_t lock;
static pthread_cond_t cond;
static int waiting_to_start = 1;
static long NUM_OF_THREADS = 0;
static atomic_int dead_threads = 0;
static atomic_int waiting_threads = 0;
static atomic_int matching_files = 0;

enum {
    FINISH_LOOP,
    REPEAT_LOOP,
    PROCEED_LOOP
};
// ------------------------- QUEUE IMPLEMENTATION --------------------------- //
typedef struct queue {
    struct node *head;
    struct node *tail;
} queue_t;

static queue_t *queue;

typedef struct node {
    char path[PATH_MAX];
    struct node *next;
} node_t;

queue_t* init_queue(){
    queue_t *self;
    self = malloc(sizeof(queue_t));
    if (!self){
        return NULL;
    }
    self->head = NULL;
    self->tail = NULL;

    return self;
}

node_t* init_node(char *path){
    node_t *node;
    node = malloc(sizeof(node_t));
    if (!node){
        return NULL;
    }

    node->next = NULL;
    snprintf(node->path, sizeof(node->path), "%s",path);

    return node;
}

int enqueue(node_t *node){
    if (!node){
        return 0;
    }

    if (!queue->head && !queue->tail){
        queue->head = node;
        queue->tail = node;
        node->next = NULL;
    } else {
        queue->tail->next = node;
        queue->tail = node;
    }

    return 1;
}

node_t *dequeue(){
    node_t *node = queue->head;

    if (!queue->head && !queue->tail){
        return NULL;
    }

    if (node->next){
        queue->head = node->next;
    } else {
        queue->head = NULL;
        queue->tail = NULL;
    }

    return node;
}

int is_empty(){
    if (!queue->head && !queue->tail){
        return EMPTY;
    }
    return NON_EMPTY;
}
// ------------------------------ MAIN LOOP --------------------------------- //
// Handles the case where the file is a regular file
int handle_regular_file(struct dirent *sd, char* term, char* path){

    // If the file name contains the search term print the path of the file
    if (strstr(sd->d_name,term)){
        matching_files++;
        printf("%s\n",path);
    }
    return 0;
}

// Handles the case where the file is a directory
int handle_directory(struct stat buf, struct dirent *sd, char* path){
    node_t *node;

    // If the file is one of the directories "." or "..", ignore it
    if (!strcmp(sd->d_name, ".") || !strcmp(sd->d_name, "..")) {
    }
    else {
        // If the directory can be searched, add that directory to the tail of the shared FIFO queue
        if (S_IRUSR & buf.st_mode) {

            pthread_mutex_lock(&lock);
            node = init_node(path);
            if (!node){
                fprintf(stderr,"Error: could not initialized node properly.\n");
                return 1;
            }
            enqueue(node);
            pthread_cond_signal(&cond);
            pthread_mutex_unlock(&lock);
        }
        // If the directory can’t be searched, print the following message and continue
        else {
            printf("Directory %s: Permission denied.\n",sd->d_name);
        }
    }
    return 0;
}

// In case all threads are waiting to start -> stay here and wait for signal.
// In case the queue is empty - > stay here and wait until it's not
// In case all threads are dead or idle -> exit.
int status(){
    if (waiting_to_start || is_empty()){

        waiting_threads++;
        if (waiting_threads + dead_threads == NUM_OF_THREADS){
            pthread_cond_broadcast(&cond);
            waiting_threads--;
            return FINISH_LOOP;
        }

        pthread_cond_wait(&cond, &lock);
        waiting_threads--;
        return REPEAT_LOOP;
    }
    else{
        return PROCEED_LOOP;
    }
}

// Open directory and search for a term in it
int search_in_directory(node_t *node, char *curr_path, char *term){
    DIR *dir;
    struct dirent *sd;
    struct stat buf;

    dir = opendir(node->path);
    if (!dir){
        printf("Error: could not open dir.\n");
        return 1;
    }

    while ((sd = readdir(dir)) != NULL) {

        // Concatenating current node path with current file name to obtain the current path
        snprintf(curr_path, PATH_MAX, "%s/%s",node->path,sd->d_name);
        lstat(curr_path, &buf);
        // If the current file is a regular file handle it accordingly
        if (S_ISREG(buf.st_mode) || S_ISLNK(buf.st_mode)){
            if (handle_regular_file(sd, term, curr_path)){
                return 1;
            }
        }

        // If the current file is a directory handle it accordingly
        // and do not match its name to the search term
        else if (S_ISDIR(buf.st_mode)) {
            if (handle_directory(buf, sd, curr_path)){
                return 1;
            }
        }
    }

    closedir(dir);
    return 0;
}

// Execute main loop
void *dequeue_and_search(char *term){
    node_t *node;
    char curr_path[PATH_MAX];
    fflush(stdout);
    int rc;

    pthread_mutex_lock(&lock);

    Loop:
        // Check how a thread should behave in this iteration:
        // PROCEED_LOOP     -> keep moving forward in main loop
        // REPEAT_LOOP      -> thread waited on a cv and woke up, need to check the queue
        // FINISH_LOOP      -> all threads are idle or dead so quit the loop
        rc = status();
        switch(rc){
            case PROCEED_LOOP:
                goto Pass;
            case REPEAT_LOOP:
                goto Loop;
            case FINISH_LOOP:
                goto Quit;
            default:;
        }

    Pass:
        // Dequeue the head directory from the FIFO queue
        node = dequeue();
        pthread_mutex_unlock(&lock);

        // Open the directory and search for the term
        if (search_in_directory(node, curr_path, term)){
            dead_threads++;
            pthread_exit(NULL);
        }
        pthread_mutex_lock(&lock);
        goto Loop;

    Quit:
        pthread_mutex_unlock(&lock);
        dead_threads++;
        pthread_exit(NULL);
}
// -------------------------------------------------------------------------- //
int main(int argc, char *argv[]) {
    pthread_t *thread;
    node_t *node;
    DIR *dir;
    void *status;
    long t;
    int rc;

    // Validate that the correct number of command line arguments is passed
    if (argc < 4){
        errno = EINVAL;
        fprintf(stderr,"Error: invalid number of command line arguments is passed.\n");
        exit(1);
    }

    // Validate that argv[1] is a searchable directory
    dir = opendir(argv[1]);
    if (!dir){
        fprintf(stderr,"Error: argv[1] is a searchable directory.\n");
        exit(1);
    }
    closedir(dir);

    // Create a FIFO queue that holds directories
    queue = init_queue();
    if (!queue){
        fprintf(stderr,"Error: could not initialized queue properly.\n");
        exit(1);
    }

    // Put the search root directory in the queue
    node = init_node(argv[1]);
    if (!node){
        fprintf(stderr,"Error: could not initialized node properly.\n");
        exit(1);
    }
    enqueue(node);

    // Create n searching threads, each searching thread removes directories from
    // the queue and searches for file names containing the search term specified
    NUM_OF_THREADS = strtol(argv[3], NULL, 10);
    thread = malloc(sizeof(pthread_t)*NUM_OF_THREADS);
    if (!thread){
        fprintf(stderr,"Error: could not allocate thread array properly.\n");
        exit(1);
    }

    // Initialize mutex and condition variable
    rc = pthread_mutex_init(&lock, NULL);
    if (rc) {
        fprintf(stderr, "ERROR in pthread_mutex_init(): %s\n",strerror(rc));
        exit(1);
    }

    // Initialize condition variable
    rc = pthread_cond_init(&cond, NULL);
    if (rc) {
        fprintf(stderr,"ERROR in pthread_cond_init(): %s\n",strerror(rc));
        exit(1);
    }

    // Launch threads
    for (t = 0; t < NUM_OF_THREADS; ++t) {
        rc = pthread_create(&thread[t], NULL, (void *)dequeue_and_search, argv[2]);
        if (rc) {
            fprintf(stderr,"ERROR in pthread_create(): %s\n",strerror(rc));

            exit(1);
        }
    }

    // After all searching threads are created, signal them to start searching
    waiting_to_start = 0;
    pthread_cond_broadcast(&cond);

    // Wait for threads to finish
    for (t = 0; t < NUM_OF_THREADS; ++t) {
        rc = pthread_join(thread[t], &status);
        if (rc) {
            fprintf(stderr,"ERROR in pthread_join(): %s\n",strerror(rc));

            exit(1);
        }
    }

    // Epilogue
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);

    printf("Done searching, found %d files\n",matching_files);
    exit(0);
}
