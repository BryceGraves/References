/*
 * R Jesse Chaney
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

//#define NUM_THREADS 10

static void *threadFunction(void *);
static pthread_barrier_t barrier; // = PTHREAD_BARRIER_INITIALIZER(NUM_THREADS);

int
main(
     int argc
     , char *argv[]
) {
  // Those is just gross!
  // Oh the things I do to demonstrate something.
  int NUM_THREADS = atoi(argv[1]);
  {
    pthread_t threads[NUM_THREADS];
    int i;
    int ids[NUM_THREADS];
    pthread_attr_t attr;

    memset(ids, 0, NUM_THREADS * sizeof(int));
    pthread_barrier_init(&barrier, NULL, NUM_THREADS);

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    for (i = 0; i < NUM_THREADS; i++) {
      ids[i] = i;
      pthread_create(&threads[i], &attr, threadFunction, &ids[i]);
    }

    //for (i = 0; i < NUM_THREADS; i++) {
    //    pthread_join(threads[i], NULL);
    //}

    //pthread_attr_destroy(&attr);
    //pthread_barrier_destroy(&barrier);
    pthread_exit(EXIT_SUCCESS);
  }
}

// In this (simple) example, all threads are calling the same thread 
// function.  That is not a requirement.
void *
threadFunction(void *a)
{
  int id = *(int *) a;

  printf("thread %d starting\n", id);
  sleep(id % 2);  // Simulation of work being done.
  printf("  thread %d to barier\n", id);

  pthread_barrier_wait(&barrier);
  printf("    Weeeehaaaaaa!!! Thread %d going again!!!\n", id);

  pthread_exit(EXIT_SUCCESS);
}
