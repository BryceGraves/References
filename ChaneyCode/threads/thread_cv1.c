 /*
 * R Jesse Chaney
 */

#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#define TCOUNT 10
#define WATCH_COUNT 5
#define THREAD_COUNT 6

static volatile sig_atomic_t count = 0;
static volatile sig_atomic_t done = 0;
static pthread_mutex_t count_mutex; // = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t count_threshold_cv; // = PTHREAD_COND_INITIALIZER;

static void *watch_count(void *);
static void *inc_count(void *);

// An example of when something like this can be used is when the 
// inc_count threads are acquiring data and periodically the 
// watch_count threads produce partial sums of the current data.
// Calculation of the partial sums takes a "long time".
int
main(
     int argc
     , char* argv[]
     , char* envp[]
) {
  int i;
  pthread_t threads[THREAD_COUNT];
  pthread_attr_t attr;
  int thread_ids[THREAD_COUNT] = {0, 1, 2, 3, 4, 5};

  // Initialize mutex and condition variable objects
  pthread_mutex_init(&count_mutex, NULL);
  pthread_cond_init (&count_threshold_cv, NULL);

  // Create an attribute that can be used when creating the threads.
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  pthread_create(&threads[0], &attr, inc_count, &thread_ids[0]);
  pthread_create(&threads[1], &attr, inc_count, &thread_ids[1]);
  pthread_create(&threads[2], &attr, inc_count, &thread_ids[2]);
  pthread_create(&threads[3], &attr, inc_count, &thread_ids[3]);

  pthread_create(&threads[4], &attr, watch_count, &thread_ids[4]);
  pthread_create(&threads[5], &attr, watch_count, &thread_ids[5]);

  // Collect all the threads.
  for (i = 0; i < THREAD_COUNT; i++) {
    pthread_join(threads[i], NULL);
    printf("joined with thread %d\n", i);
  }
  printf("All threads joined\n");

  pthread_attr_destroy(&attr);
  pthread_mutex_destroy(&count_mutex);
  pthread_cond_destroy(&count_threshold_cv);

  pthread_exit(EXIT_SUCCESS);
}

void *
watch_count(
    void *v
) {
  int id = *(int *) v;

  // Lock mutex and wait for signal.
  //
  // Note that the pthread_cond_wait routine will automatically and
  // atomically unlock mutex while it waits.
  //
  pthread_mutex_lock(&count_mutex);
  // This is a critical region.
  while ( !done ) {
    printf("  ** watch_count waiting: Thread %d\n", id);

    pthread_cond_wait(&count_threshold_cv, &count_mutex);

    printf("  ** watch_count signaled: Thread %d, count is %d\n", id, count);
    sleep(1);  // Do some work.
  }
  pthread_mutex_unlock(&count_mutex);

  pthread_exit(EXIT_SUCCESS);
}

void *
inc_count(
  void *v
) {
  int id = *(int *) v;
  int i;

  for (i = 0; i < TCOUNT; i++) {
    // Acquire the lock.
    {
      // This is a critical region.
      pthread_mutex_lock(&count_mutex);
      // Increment the value.
      count++;

      printf("inc_count got lock: Thread %d count %d\n", id, count);
      if ((count % WATCH_COUNT) == 0) {
	// Periodically, signal the watchers that the count has changed.
	// In this case, each time the (count mod 5) == 0, signal the
	// watcher threads.
	printf("  inc_count signaling: %d\n", count);
	// A signal means that one of the waiting threads will be
	// awoken.
	pthread_cond_signal(&count_threshold_cv);
	// A boardcast means that all of the watchers will be
	// awoken.
	//pthread_cond_broadcast(&count_threshold_cv);
      }
      pthread_mutex_unlock(&count_mutex);
    }
    sleep(1);  // Do some work outside the critical region.
  }

  done = 1;
  // Make sure enough signals have been sent to stop the watchers.
  pthread_cond_signal(&count_threshold_cv);
  pthread_exit(EXIT_SUCCESS);
}
