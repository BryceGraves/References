 /*
 * R Jesse Chaney
 */

//#define _POSIX_C_SOURCE 200809L

#include <stdio.h> 
#include <pthread.h> 
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

extern void *thread_func1( void * );
extern void *thread_func2( void * );

static pthread_mutex_t mutex1;
static pthread_mutex_t mutex2;

int 
main(
     int argc
     , char *argv[]
     , char *envp[]
) {

  pthread_t thread_id1;
  pthread_t thread_id2;

  pthread_mutex_init(&mutex1, NULL);
  pthread_mutex_init(&mutex2, NULL);

  pthread_create(&thread_id1, NULL, thread_func1, NULL);
  pthread_create(&thread_id2, NULL, thread_func2, NULL);

  printf("Threads created.  Waiting...\n");

  pthread_join(thread_id1, NULL);
  pthread_join(thread_id2, NULL);

  pthread_exit(EXIT_SUCCESS);
}

void * 
thread_func1(
      void *temp
) {
  struct timespec tout;
  struct tm *tmp;
  int lock_result1 = 0;
  int lock_result2 = 0;
  char buf[64];
  int iterations = 0;
  
  do {
    clock_gettime(CLOCK_REALTIME, &tout);
    tmp = localtime(&tout.tv_sec);
    strftime(buf, sizeof(buf), "%r", tmp);
    printf("Current time is %s  %s\n", buf, __FUNCTION__);
    tout.tv_sec += 5;  /* 5 seconds from now */
    lock_result1 = lock_result2 = 0;

    lock_result1 = pthread_mutex_timedlock(&mutex1, &tout);
    if (lock_result1 == ETIMEDOUT) {
      printf("mutex1 timeout: %s\n", __FUNCTION__);
    }
    else {
      printf("mutex1 acquired: %s\n", __FUNCTION__);

      sleep(1);
      // Try to get the next mutex.
      lock_result2 = pthread_mutex_timedlock(&mutex2, &tout);
      if (lock_result2 == ETIMEDOUT) {
	printf("  mutex2 timeout, mutex1 will be unlocked: %s\n", __FUNCTION__);
	pthread_mutex_unlock(&mutex1);
	usleep(100);
      }
      else {
	printf("mutex2 acquired: %s\n", __FUNCTION__);
      }
    }
    iterations++;
  } while (lock_result1 || lock_result2);
  printf("Both mutex-es acquired: %s\n", __FUNCTION__);

  pthread_mutex_unlock(&mutex2);
  pthread_mutex_unlock(&mutex1);
  printf("Both mutex-es unlocked: %s\n", __FUNCTION__);

  printf("Done after iterations %d: %s\n", iterations, __FUNCTION__);

  pthread_exit(EXIT_SUCCESS);
}


void * 
thread_func2(
     void *temp
) {
  struct timespec tout;
  struct tm *tmp;
  int lock_result1 = 0;
  int lock_result2 = 0;
  char buf[64];
  int iterations = 0;
  
  do {
    clock_gettime(CLOCK_REALTIME, &tout);
    tmp = localtime(&tout.tv_sec);
    strftime(buf, sizeof(buf), "%r", tmp);
    printf("    Current time is %s  %s\n", buf, __FUNCTION__);
    tout.tv_sec += 7;  /* 7 seconds from now */
    lock_result1 = lock_result2 = 0;

    lock_result2 = pthread_mutex_timedlock(&mutex2, &tout);
    if (lock_result2 == ETIMEDOUT) {
      printf("    mutex2 timeout: %s\n", __FUNCTION__);
    }
    else {
      printf("    mutex2 acquired: %s\n", __FUNCTION__);

      sleep(1);
      // Try to get the next mutex.
      lock_result1 = pthread_mutex_timedlock(&mutex1, &tout);
      if (lock_result1 == ETIMEDOUT) {
	printf("    mutex1 timeout, mutex2 will be unlocked: %s\n", __FUNCTION__);
	pthread_mutex_unlock(&mutex2);
	usleep(500);
	//nanosleep((struct timespec[]){{0, 500000000}}, NULL);
      }
      else {
	printf("    mutex1 acquired: %s\n", __FUNCTION__);
      }
    }
    iterations++;
  } while (lock_result1 || lock_result2);
  printf("    Both mutex-es acquired: %s\n", __FUNCTION__);

  pthread_mutex_unlock(&mutex2);
  pthread_mutex_unlock(&mutex1);
  printf("    Both mutex-es unlocked: %s\n", __FUNCTION__);

  printf("  Done after iterations %d: %s\n", iterations, __FUNCTION__);

  pthread_exit(EXIT_SUCCESS);
}
