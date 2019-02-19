 /*
 * R Jesse Chaney
 */

#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>

// How many times should this drain the bucket.
#define DRAIN_COUNT 5

// What is the maximum amount to put into the bucket before
// the drain is opended.
#define MAX_FILL_LEVEL 10

// How many fill threads are created.
#define FILL_THREADS 3

// The multplier used for usleep duration.
#define USLEEP_MULT 100000

#ifndef TRUE
# define TRUE 1
#endif // TRUE
#ifndef FALSE
# define FALSE 0
#endif // FALSE

// The current level of the bucket.
static volatile sig_atomic_t bucket_level = 0;

// Is everything done.
static volatile sig_atomic_t done = FALSE;

// These could be statically initialized.
static pthread_mutex_t count_mutex; // = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t count_threshold_cv; // = PTHREAD_COND_INITIALIZER;

static void *drain_func(void *);
static void *fill_func(void *);

int main(int argc, char* argv[])
{
  int i;
  pthread_t fill_threads[FILL_THREADS];
  pthread_t drain_thread;
  pthread_attr_t attr;
  int sleep_base[FILL_THREADS];
  time_t start_time = time(NULL);

  printf("%s pid = %u\n", argv[0], getpid());
  srand((unsigned int) start_time);
  // Initialize mutex and condition variable objects
  pthread_mutex_init(&count_mutex, NULL);
  pthread_cond_init(&count_threshold_cv, NULL);

  // Create an attribute that can be used when creating the threads.
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  for (i = 0; i < FILL_THREADS; i++) {
    sleep_base[i] = i + 2;
    // Crank out all the filler threads.
    pthread_create(&fill_threads[i], &attr, fill_func, (void *) &sleep_base[i]);
  }

  // Start the drain thread.
  // The drain thread should be able to drain the bucker faster than the
  // fill threads combined.
  pthread_create(&drain_thread, &attr, drain_func, (void *) ((long) 1));

  // Collect all the threads.
  for (i = 0; i < FILL_THREADS; i++) {
    pthread_join(fill_threads[i], NULL);
    printf("joined with fill thread %d\n", i);
  }
  pthread_join(drain_thread, NULL);
  printf("All threads joined\n");

  // A little cleanup.
  pthread_attr_destroy(&attr);
  pthread_mutex_destroy(&count_mutex);
  pthread_cond_destroy(&count_threshold_cv);

  pthread_exit(EXIT_SUCCESS);
}

void *drain_func(void *ptr)
{
  long sleep_base = ((long) ptr);
  int rez = 0;
  int drain_count = 0;
  int drained = FALSE;
  unsigned int seedp;
  useconds_t usleep_time;

  // Lock mutex and wait for signal.
  //
  // Note that the pthread_cond_wait routine will automatically and
  // atomically unlock mutex while it waits.
  //
  pthread_mutex_lock(&count_mutex);
  while ( done == FALSE && drain_count < DRAIN_COUNT) {
    // Establish the conditional wait.
    pthread_cond_wait(&count_threshold_cv, &count_mutex);

    // This is sorta ugly magic. The fill threads may have sent multiple
    // signals while the drain thread was already draining the bucket.
    // This tries to ignore those signals by making sure the bucket has
    // enough in it to drain.
    // When pthread_cond_wait() returns, this thread has the lock. Do start
    // draining the bucket.
    if (bucket_level > (MAX_FILL_LEVEL / 2)) {
      drained = FALSE;
      while (drained == FALSE) {
	bucket_level--;
	printf("%d ", bucket_level);
	fflush(stdout);
	if (bucket_level <= 0) {
	  printf("drained\n\n");
	  fflush(stdout);
	  drain_count++;
	  drained = TRUE;
	}
	rez = pthread_mutex_unlock(&count_mutex);
	if (rez != 0) {
	  perror("drain_thread unlock in loop failed");
	  done = TRUE;
	}
	usleep_time = (((rand_r(&seedp) % sleep_base) + 1) * sleep_base) * USLEEP_MULT;
	usleep(usleep_time);
	if (drained == FALSE) {
	  rez = pthread_mutex_lock(&count_mutex);
	  if (rez != 0) {
	    perror("drain_thread lock in loop failed");
	    done = TRUE;
	  }
	}
      }
      usleep_time = (((rand_r(&seedp) % sleep_base) + 1) * sleep_base) * USLEEP_MULT;
      usleep(usleep_time);
    }
    else {
      rez = pthread_mutex_unlock(&count_mutex);
      if (rez != 0) {
	perror("drain_thread unlock bucket_level > (MAX_FILL_LEVEL - 3) failed");
	done = TRUE;
      }
    }
  }
  printf("\ndrain done\n");
  done = TRUE;

  pthread_exit(EXIT_SUCCESS);
}

// The fill threads are pretty simple.
//   while not done
//     acquire the lock
//     add to the bucket
//     if the bucket is full
//       signal the drain thread
//     release the lock
//     sleep for a little bit
//   repeat
void *fill_func(void *ptr)
{
  int sleep_base = *(int *) ptr;
  int rez = 0;
  unsigned int seedp;
  useconds_t usleep_time;

  while (done == FALSE) {
    // Acquire the lock.
    rez = pthread_mutex_lock(&count_mutex);
    if (rez != 0) {
      perror("fill_thread lock failed");
      done = TRUE;
    }

    // A drop in the bucket.
    bucket_level++;
    printf("%d ", bucket_level);
    fflush(stdout);

    if (bucket_level >= MAX_FILL_LEVEL) {
      // Signal the drains that the bucket is full.
      // A signal means that one of the waiting threads will be awoken.
      rez = pthread_cond_signal(&count_threshold_cv);
      if (rez != 0) {
	perror("fill_thread cond signal failed");
	done = TRUE;
      }
    }
    rez = pthread_mutex_unlock(&count_mutex);
    if (rez != 0) {
      perror("fill_thread unlock failed");
      done = TRUE;
    }
    usleep_time = (((rand_r(&seedp) % sleep_base) + 1) * sleep_base) * USLEEP_MULT;
    usleep(usleep_time);
  }
  done = TRUE;

  printf("\nfill %d done\n", sleep_base);
  // Make sure enough signals have been sent to drain the bucket.
  pthread_cond_signal(&count_threshold_cv);
  pthread_exit(EXIT_SUCCESS);
}
