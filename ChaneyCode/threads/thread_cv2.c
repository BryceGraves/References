 /*
 * R Jesse Chaney
 */

// Taken from:
//  http://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

static pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;
// A condition variable must always be associated with a 
// mutex to avoid a race condition created by one thread
// preparing to wait and another thread which may signal
// the condition before the first thread actually waits 
// on it resulting in a deadlock.
static pthread_cond_t condition_cond = PTHREAD_COND_INITIALIZER;

static void *functionCount1(void *);
static void *functionCount2(void *);

static volatile sig_atomic_t count = 0;

#define COUNT_DONE 10
#define COUNT_HALT1 3
#define COUNT_HALT2 6

int
main(
     int argc
     , char *argv[]
     , char *envp[]
) {
  pthread_t thread1;
  pthread_t thread2;
  int t1 = 1;
  int t2 = 2;

  pthread_create(&thread1, NULL, functionCount1, &t1);
  pthread_create(&thread2, NULL, functionCount2, &t2);

  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);

  pthread_exit(EXIT_SUCCESS);
}

static void *
functionCount1(
       void *a
) {
  for ( ; ; ) {
    pthread_mutex_lock(&condition_mutex);

    // If count is between COUNT_HALT1 and COUNT_HALT2, the this
    // thread will loop, waiting on the condition variable.
    while ((count >= COUNT_HALT1) && (count <= COUNT_HALT2)) {
      printf("  %s waiting\n", __FUNCTION__);
      pthread_cond_wait(&condition_cond, &condition_mutex);
      printf("  %s waking\n", __FUNCTION__);
    }
    pthread_mutex_unlock(&condition_mutex);

    // This is a critical region.
    {
      pthread_mutex_lock(&count_mutex);
      count++;
      printf("%s: count = %d\n", __FUNCTION__, count);
      pthread_mutex_unlock(&count_mutex);
    }

    sleep(1); // do some work
    if (count >= COUNT_DONE) {
      break;
    }
  }
  pthread_exit(EXIT_SUCCESS);
}

static void *
functionCount2(
       void *a
) {
  for ( ; ; ) {
    pthread_mutex_lock(&condition_mutex);

    // If count is less than COUNT_HALT1 or greater than
    // COUNT_HALT2, then this thread will signal the condtion
    // variable.
    if ((count < COUNT_HALT1) || (count > COUNT_HALT2)) {
      printf("**  %s signaling\n", __FUNCTION__);
      pthread_cond_signal(&condition_cond);
    }
    pthread_mutex_unlock(&condition_mutex);

    // This is a critical region.
    {
      pthread_mutex_lock(&count_mutex);
      count++;
      printf("** %s: count = %d\n", __FUNCTION__, count);
      pthread_mutex_unlock(&count_mutex);
    }

    sleep(1);  // do some work
    if (count >= COUNT_DONE) {
      break;
    }
  }
  pthread_exit(EXIT_SUCCESS);
}
