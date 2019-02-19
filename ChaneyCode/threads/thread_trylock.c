 /*
 * R Jesse Chaney
 */

#define _BSD_SOURCE // For usleep().

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
  pthread_t thread_id3;
  pthread_t thread_id4;
  pthread_t thread_id5;
  pthread_t thread_id6;
  pthread_t thread_id7;
  pthread_t thread_id8;
  int ids[] = {1, 2, 3, 4, 5, 6, 7, 8};

  pthread_mutex_init(&mutex1, NULL);
  pthread_mutex_init(&mutex2, NULL);

  pthread_create(&thread_id1, NULL, thread_func1, (void *) &ids[0]);
  pthread_create(&thread_id3, NULL, thread_func1, (void *) &ids[1]);
  pthread_create(&thread_id2, NULL, thread_func2, (void *) &ids[2]);
  pthread_create(&thread_id4, NULL, thread_func2, (void *) &ids[3]);
  pthread_create(&thread_id5, NULL, thread_func1, (void *) &ids[4]);
  pthread_create(&thread_id6, NULL, thread_func1, (void *) &ids[5]);
  pthread_create(&thread_id7, NULL, thread_func2, (void *) &ids[6]);
  pthread_create(&thread_id8, NULL, thread_func2, (void *) &ids[7]);

  printf("Threads created.  Waiting...\n");

  pthread_join(thread_id1, NULL);
  pthread_join(thread_id2, NULL);
  pthread_join(thread_id3, NULL);
  pthread_join(thread_id4, NULL);
  pthread_join(thread_id5, NULL);
  pthread_join(thread_id6, NULL);
  pthread_join(thread_id7, NULL);
  pthread_join(thread_id8, NULL);

  pthread_exit(EXIT_SUCCESS);
}

void * 
thread_func1(
      void *i
) {
  int lock_result2 = 0;
  int id = *((int *) i);

  for ( ; ; ) {
    pthread_mutex_lock(&mutex1);
    printf("mutex1 acquired: %s  id: %d\n", __FUNCTION__, id);
    sleep(1);
    lock_result2 = pthread_mutex_trylock(&mutex2);
    if (lock_result2 == EBUSY) {
      pthread_mutex_unlock(&mutex1);
      printf("mutex2 trylock failed: %s  id: %d\n", __FUNCTION__, id);
      usleep(10 * id);
    }
    else {
      break;
    }
  }

  printf("mutex2 acquired: %s  id: %d\n", __FUNCTION__, id);
  printf("Both mutex-es acquired: %s  id: %d\n", __FUNCTION__, id);

  pthread_mutex_unlock(&mutex2);
  pthread_mutex_unlock(&mutex1);

  printf("Done: %s  id: %d\n", __FUNCTION__, id);
  pthread_exit(EXIT_SUCCESS);
}


void * 
thread_func2(
     void *i
) {
  int lock_result1 = 0;
  int id = *((int *) i);

  for ( ; ; ) {
    pthread_mutex_lock(&mutex2);
    printf("    mutex2 acquired: %s  id: %d\n", __FUNCTION__, id);
    sleep(1);
    lock_result1 = pthread_mutex_lock(&mutex1);
    if (lock_result1 == EBUSY) {
      pthread_mutex_unlock(&mutex2);
      printf("    mutex2 trylock failed: %s  id: %d\n", __FUNCTION__, id);
      usleep(10 * id);
    }
    else {
      break;
    }
  }


  printf("    mutex1 acquired: %s  id: %d\n", __FUNCTION__, id);
  printf("    Both mutex-es acquired: %s  id: %d\n", __FUNCTION__, id);

  pthread_mutex_unlock(&mutex1);
  pthread_mutex_unlock(&mutex2);

  printf("  Done: %s  id: %d\n", __FUNCTION__, id);
  pthread_exit(EXIT_SUCCESS);
}
