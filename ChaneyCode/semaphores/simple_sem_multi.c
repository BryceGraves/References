/*
 * R Jesse Chaney
 */

#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define SEM_NAME "/sem_multi_proc"
#define AVAILABLE_RESOURCES 2
#define MAX_PROCSS 5

int main(int argc, char *argv[])
{
  sem_t * my_semaphore;
  int i;

  my_semaphore = sem_open(SEM_NAME
                          , O_CREAT | O_EXCL
			  , S_IRUSR | S_IWUSR
			  , AVAILABLE_RESOURCES);

  if (my_semaphore < 0) {
    perror("sem_open failed");
    exit(EXIT_FAILURE);
  }

  for (i = 0; i < MAX_PROCSS; i++) {
    int j;
    int result;
    pid_t cpid = fork();

    if (0 == cpid) {
      unsigned int seed = getpid();
      unsigned got_times = 0;
      int sem_value;

      for (j = 0; j < 5; j++) {
	int sleep_time = (rand_r(&seed) % 4);

	if (sem_trywait(my_semaphore) < 0) {
	  printf("   %*s %d: cannot get semaphore, waiting...\n", i, "", getpid());
	}
	else {
	  result = sem_wait(my_semaphore);
	  if (result > 0) {
	    // failed getting the semaphore
	  }
	}
	sem_getvalue(my_semaphore, &sem_value);
	printf("   %*s %d: got the semaphore %d, working...\n", i, "", getpid(), sem_value);
	sleep(sleep_time);
	got_times ++;

	// release the semaphore
	sem_post(my_semaphore);
	printf("   %*s %d: released it\n", i, "", getpid());
      }
      printf("   %*s %d: DONE! Got it %d times\n", i, "", getpid(), got_times);

      sem_close(my_semaphore);
      _exit(EXIT_SUCCESS);
    }
    else {
    }
  }

  while (wait(NULL) >= 0) {
    // reap all the child processes
    // Zombies be gone.
  }

  sem_close(my_semaphore);
  sem_unlink(SEM_NAME);

  printf("All child processes are done and reaped\n");

  return(EXIT_SUCCESS);
}
