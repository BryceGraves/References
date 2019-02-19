/*
 * R Jesse Chaney
 */

#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define SEM_NAME "/sem_thread"
#define AVAILABLE_RESOURCES 2
#define MAX_THREADS 5

sem_t * my_semaphore = NULL;

void * do_work(void *void_arg);

int main(int argc, char *argv[])
{
  unsigned long i;
  pthread_t tids[MAX_THREADS];
  unsigned int ids[MAX_THREADS];

  my_semaphore = sem_open(SEM_NAME
                          , O_CREAT | O_EXCL
			  , S_IRUSR | S_IWUSR
			  , AVAILABLE_RESOURCES);

  if (my_semaphore < 0) {
    perror("sem_open failed");
    exit(EXIT_FAILURE);
  }

  for (i = 0; i < MAX_THREADS; i++) {
    ids[i] = i;
    pthread_create(&tids[i], NULL, do_work, (void *) &ids[i]);
  }

  for (i = 0; i < MAX_THREADS; i++) {
    pthread_join(tids[i], NULL);
  }

  sem_close(my_semaphore);
  sem_unlink(SEM_NAME);

  printf("All threads are done and joined\n");

  return(EXIT_SUCCESS);
}

void *
do_work(void *void_arg)
{
  unsigned int *id = (unsigned int *) void_arg;
  int spaces = (int) *id;
  unsigned got_times = 0;
  int sem_value;
  int j;
  int result;

  for (j = 0; j < 5; j++) {
    int sleep_time = (rand_r(id) % 4);

    if (sem_trywait(my_semaphore) < 0) {
      printf("   %*s %d: cannot get semaphore, waiting...\n", spaces, "", spaces);
    }
    else {
      result = sem_wait(my_semaphore);
      if (result > 0) {
	// failed getting the semaphore
      }
    }
    sem_getvalue(my_semaphore, &sem_value);
    printf("   %*s %d: got the semaphore %d, working...\n", spaces, "", spaces, sem_value);
    sleep(sleep_time);
    got_times ++;

    // release the semaphore
    sem_post(my_semaphore);
    printf("   %*s %d: released it\n", spaces, "", spaces);
  }
  printf("   %*s %d: DONE! Got it %d times\n", spaces, "", spaces, got_times);

  pthread_exit(NULL);
}
