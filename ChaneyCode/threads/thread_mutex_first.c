/*
 * R Jesse Chaney
 */

// Compile with
//    gcc -Wall -o thread_mutex_first thread_mutex_first.c -pthread
//
// Run with something like this:
//    ./thread_mutex_first 5
//

#include <stdio.h>
#include <sys/times.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

static long num_threads = -1;
static pthread_mutex_t mutex_sum;

extern void *do_work( void *);

int 
main(int argc, char *argv[])
{
    pthread_t *thread;
    long i;
    int ret;

    if (argc < 2) {
        printf("Must provide values for number of threads\n");
        printf("  Something like:\n    %s 5\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    // Pull the number of threads and work per thread off
    // the command line. Some range checks would be nice.
    num_threads = (long) strtol(argv[1], NULL, 10);
    printf("Running with %d threads\n"
           , (int) num_threads);

    // Initialize the mutex. You MUST do this before you use
    // the mutex.
    pthread_mutex_init(&mutex_sum, NULL);
 
    // Create the array that will hold the thread ids.
    thread = (pthread_t *) malloc(num_threads * sizeof(pthread_t));

    for (i = 0; i < num_threads; i++) {
        ret = pthread_create(&thread[i], NULL, do_work, (void *) i);
        if (ret != 0) {
            perror("Cannot create the mutex threads.");
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all the threads to complete.
    for (i = 0; i < num_threads; i++) {
        pthread_join(thread[i], NULL);
    }

    pthread_mutex_destroy(&mutex_sum);

    free(thread);  // Be good and free the allocated memory.

    return(EXIT_SUCCESS);
}

void *do_work(
    void *arg
    ) {
    long id = (long) arg;
    int i;

    for (i = 0; i < 3; i++) {
        printf("Thread %ld requesting the lock\n", id);
        pthread_mutex_lock(&mutex_sum);
        printf("  Thread %ld now has the lock\n", id);

        sleep(2 * i);

        pthread_mutex_unlock(&mutex_sum);
    }
    printf("  Thread %ld EXITING\n", id);
  
    pthread_exit(EXIT_SUCCESS);
}
