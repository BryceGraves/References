 /*
 * R Jesse Chaney
 */

// from:
//    http://publib.boulder.ibm.com/iseries/v5r2/ic2928/index.htm?info/apis/users_86.htm

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

extern void *readlockThread(void *);
extern void *writelockThread(void *);
extern void checkResults(char *, int);

pthread_rwlock_t rwlock;

int 
main(
     int argc
     , char *argv[]
) {
  int rc = 0;
  pthread_t thread;
  pthread_t thread1;

  printf("Enter Testcase - %s\n", argv[0]);

  printf("Main, initialize the read write lock\n");
  rc = pthread_rwlock_init(&rwlock, NULL);
  checkResults("pthread_rwlock_init()", rc);

  printf("Main, grab a read lock\n");
  rc = pthread_rwlock_rdlock(&rwlock);
  checkResults("pthread_rwlock_rdlock()",rc);

  printf("Main, grab the same read lock again\n");
  rc = pthread_rwlock_rdlock(&rwlock);
  checkResults("pthread_rwlock_rdlock() second", rc);


  printf("Main, create the read lock thread\n");
  rc = pthread_create(&thread, NULL, readlockThread, NULL);
  checkResults("pthread_create", rc);

  printf("Main - unlock the first read lock\n");
  rc = pthread_rwlock_unlock(&rwlock);
  checkResults("pthread_rwlock_unlock()", rc);

  printf("Main, create the write lock thread\n");
  rc = pthread_create(&thread1, NULL, writelockThread, NULL);
  checkResults("pthread_create", rc);

  sleep(5);

  printf("Main - unlock the second read lock\n");
  rc = pthread_rwlock_unlock(&rwlock);
  checkResults("pthread_rwlock_unlock()", rc);

  printf("Main, wait for the threads\n");
  rc = pthread_join(thread, NULL);
  checkResults("pthread_join", rc);

  rc = pthread_join(thread1, NULL);
  checkResults("pthread_join", rc);

  rc = pthread_rwlock_destroy(&rwlock);
  checkResults("pthread_rwlock_destroy()", rc);

  printf("Main completed\n");

  //return(EXIT_SUCCESS);
  pthread_exit(EXIT_SUCCESS);
}

void
checkResults(
     char *str
     , int rc
) {
  if (rc > 0) {
    printf("\tERROR: %d: %s\n", rc, str);
  }
}

void *
readlockThread(
     void *arg
) {
  int rc;

  printf("  + Entered thread, getting read lock\n");
  rc = pthread_rwlock_rdlock(&rwlock);
  checkResults("pthread_rwlock_rdlock()\n", rc);
  printf("    + got the rwlock read lock\n");

  sleep(5);

  printf("    + unlock the read lock\n");
  rc = pthread_rwlock_unlock(&rwlock);
  checkResults("pthread_rwlock_unlock()\n", rc);
  printf("    + Secondary thread unlocked\n");

  //return(NULL);
  pthread_exit(EXIT_SUCCESS);
}

void *
writelockThread(
     void *arg
) {
  int rc;

  printf("  - Entered thread, getting write lock\n");
  rc = pthread_rwlock_wrlock(&rwlock);
  checkResults("pthread_rwlock_wrlock()\n", rc);

  printf("    - Got the rwlock write lock, now unlock\n");
  rc = pthread_rwlock_unlock(&rwlock);
  checkResults("pthread_rwlock_unlock()\n", rc);
  printf("    - Secondary thread unlocked\n");

  //return(NULL);
  pthread_exit(EXIT_SUCCESS);
}
