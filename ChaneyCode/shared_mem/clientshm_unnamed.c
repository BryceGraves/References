/*
 * R Jesse Chaney
 */

// Compile:
//    gcc -o clientshm_unnamed clientshm_unnamed.c -lrt

#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>

#include "shm.h"

static void signal_handler(int);
static sig_atomic_t signal_received = 0;

static void 
signal_handler(
       int signum
) {
  // We do this so that this process will not receive a
  // signal and exit while it has the sempahore locked.
  signal_received = 1;
}

int 
main(
     int argc
     , char *argv[]
     , char *envp[]
) {
  int res;
  int shmfd;
  char sharedMemName[100];
  sem_t *sem;
  int shared_seg_size = -1;
  void *shared_void;
  struct shared_data *shared_base;

  signal(SIGINT, signal_handler);
  sprintf(sharedMemName, "%s_%s", SHMOBJ_PATH, getenv("LOGNAME"));
  // Open the shared memory object.
  shmfd = shm_open(sharedMemName
		   , O_RDWR
		   , (S_IRUSR | S_IWUSR | S_IRGRP));
  if (shmfd < 0) {
    perror("In shm_open()");
    exit(EXIT_FAILURE);
  }
  fprintf(stdout
	  , "Connected to shared memory object %s\n"
	  , sharedMemName);

  // Attach to the shared segment.
  shared_seg_size = sizeof(sem_t) + (NUM_ELEMENTS * sizeof(struct shared_data));
  shared_void = mmap(NULL
		     , shared_seg_size
		     , (PROT_READ | PROT_WRITE)
		     , MAP_SHARED
		     , shmfd, 0);
  if (shared_void == NULL) {
    perror("In mmap()");
    exit(EXIT_FAILURE);
  }
  fprintf(stdout
	  , "Shared memory segment connected correctly (%d bytes).\n\n"
	  , shared_seg_size);
  // Get the semaphore from the beginning of the shared memory segment.
  // The semaphore SHOULD NOT be initialized again.
  sem = (sem_t *) shared_void;
  shared_base = (struct shared_data *) (shared_void + sizeof(sem_t));

  for ( ; !signal_received ; ) {
    time_t t;
    int i;
    fprintf(stdout, "++ Waiting for semaphore lock\n");
    res = sem_wait(sem);
    if (res < 0) {
      perror("Failed on lock semaphore");
      break;
    }
    fprintf(stdout, "  ++ Got the semaphore lock\n");

    for (i = 0; i < 2; i++) {
      fprintf(stdout, "The ele_type is %d \n", shared_base[i].ele_type);
      fprintf(stdout, "  The counter is %d \n", shared_base[i].counter);
      fprintf(stdout, "  The var1 is %d \n", shared_base[i].var1);
      fprintf(stdout, "  The var2 is %d \n", shared_base[i].var2);
      fprintf(stdout, "  The text is '%s' \n\n", shared_base[i].text);
    }

    t = time(NULL);
    fprintf(stdout, "\t%s\n", ctime(&t));
    
    sleep(3);

    res = sem_post(sem);
    if (res < 0) {
      perror("failed on unlock semaphore");
      break;
    }
    fprintf(stdout, "  -- Unlocked the semaphore\n");
    sleep(1);
  }

  res = munmap(shared_void, shared_seg_size);
  if (res < 0) {
    perror("Failed un-mapping shared memory object");
  }
  res = close(shmfd);
  if (res < 0) {
    perror("Failed closing shared memory object");
  }
  // No semaphore to close here.
  return(EXIT_SUCCESS);
}
