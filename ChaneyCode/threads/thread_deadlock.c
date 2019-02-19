/*
 * R Jesse Chaney
 */

#include <stdio.h> 
#include <pthread.h> 
#include <stdlib.h>
#include <unistd.h>

extern void *thread_func1( void * );
extern void *thread_func2( void * );

static pthread_mutex_t mutex1;
static pthread_mutex_t mutex2;

void * 
thread_func1(
      void *temp
) {
  pthread_mutex_lock(&mutex1);
  printf("mutex1 acquired: %s\n", __FUNCTION__);
  sleep(1);
  pthread_mutex_lock(&mutex2);

  // Assuming that the threads are started close together, this
  // thread will never get to this point.
  printf("mutex2 acquired: %s\n", __FUNCTION__);
  printf("Both mutex-es acquired: %s\n", __FUNCTION__);

  pthread_mutex_unlock(&mutex2);
  pthread_mutex_unlock(&mutex1);

  printf("Done: %s\n", __FUNCTION__);

  pthread_exit(EXIT_SUCCESS);
}


void * 
thread_func2(
     void *temp
) {
  pthread_mutex_lock(&mutex2);
  printf("mutex2 acquired: %s\n", __FUNCTION__);
  sleep(1);
  pthread_mutex_lock(&mutex1);

  // Assuming that the threads are started close together, this
  // thread will never get to this point.
  printf("mutex1 acquired: %s\n", __FUNCTION__);
  printf("Both mutex-es acquired: %s\n", __FUNCTION__);

  pthread_mutex_unlock(&mutex1);
  pthread_mutex_unlock(&mutex2);

  printf("Done: %s\n", __FUNCTION__);

  pthread_exit(EXIT_SUCCESS);
}

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
