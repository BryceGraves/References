/*
 * R Jesse Chaney
 */

// Compile:
//    gcc -o servershm_unnamed servershm_unnamed.c -lrt

#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <semaphore.h>

#include "shm.h"

static void signal_handler(int);
static void onexit_function(void);
static sig_atomic_t signal_received = FALSE;

static void 
signal_handler(
       int signum
) {
  // Use this to know that a signal has arrived and we want to exit.
  // However, we don't want to exit with the semaphore locked.
  signal_received = TRUE;
}

static void 
onexit_function(
     void 
) {
  // We want to cleanup the shared memory object from the file
  //   system.
#define CLEAN_SHM
#ifdef CLEAN_SHM
  char sharedMemName[100];

  sprintf(sharedMemName, "%s_%s", SHMOBJ_PATH, getenv("LOGNAME"));
  if (shm_unlink(sharedMemName) != 0) {
    perror("Failed in shm_unlink()");
  }

  // No semaphore to cleanup here.
#endif // CLEAN_SHM
}

int 
main(
     int argc
     , char *argv[]
     , char *envp[]
) {
  int res;
  int shmfd;
  int j;
  int counter = 0;
  char sharedMemName[100];
  sem_t *sem;
  int shared_seg_size = -1;
  void *shared_void;
  struct shared_data *shared_base;

  umask(0);
  atexit(onexit_function);
  signal(SIGINT, signal_handler);
  srandom(time(NULL));

  sprintf(sharedMemName, "%s_%s", SHMOBJ_PATH, getenv("LOGNAME"));
  // Creating the shared memory object. It sets  permissions 
  //   exactly the same as the permissions on files you create
  //   with the open() call.
  shmfd = shm_open(sharedMemName
		   , (O_CREAT | O_RDWR | O_EXCL)
		   , (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH));
  if (shmfd < 0) {
    perror("In shm_open()");
    exit(EXIT_FAILURE);
  }
  fprintf(stdout, "Created shared memory object %s\n", sharedMemName);
  // Adjust the mapped file size.
  // We want the shared segment capable of storing elements + the semaphore.
  // In addition to putting the data elements into the shared memory
  //   object, we want to put the semaphore in there as well.
  shared_seg_size = sizeof(sem_t) + (NUM_ELEMENTS * sizeof(struct shared_data));
  res = ftruncate(shmfd, shared_seg_size);
  if (res < 0) {
    perror("ftruncate failed");
    exit(EXIT_FAILURE);
  }

  // Map to the shared memory segment into the process space 
  //   of this process.
  // Using an addres of NULL (the first parameter) means that kernel can
  //   put the segment anywhere it sees fit.
  // The prot argument describes the desired memory protection of 
  //   the mapping. In our case, we want to both read and write to the
  //   segment.
  // We want to updates to the object to be visible to other processes 
  //   that map this object. So, we map it as MAP_SHARED.
  shared_void = mmap(NULL
		     , shared_seg_size
		     , (PROT_READ | PROT_WRITE)
		     , MAP_SHARED
		     , shmfd, 0);
  if (NULL == shared_void) {
    perror("In mmap()");
    exit(EXIT_FAILURE);
  }
  fprintf(stdout
	  , "Shared memory segment allocated "
	    "and connected correctly (%d bytes).\n\n"
	  , shared_seg_size);

  // In this case, the semaphore is just the initial portion of the
  //   the shared memory object. We've put the semaphore right into
  //   the shared memory object.
  sem = (sem_t *) shared_void;

  // Before a semaphore can be used, it must be initialized.
# define SHARE_WITHIN_PROCESS    0  // Share for threads within a single process
# define SHARE_BETWEEN_PROCESSES 1  // Share between multiple processes

  // This will be the number of processes allowed through the semaphore
  //   at a time. Or, how many units of a resource can be accessed at 
  //   a time. In this case, we are creating a binary semaphore (locked
  //   or unlocked).
# define NUM_RESOURCES          1

  // Initialize the semaphore. It must be initalized before it
  //   is used.
  //
  //   int sem_init(sem_t *sem, int pshared, unsigned int value);
  // Quoting from the man page for sem_init:
  //     If  pshared  is  non-zero, then the semaphore is shared between 
  //     processes, and should be located in a  region  of  shared  memory...
  // In this case, we are creating an un-named semaphore IN the
  //   shared memory segment.
  sem_init(sem, SHARE_BETWEEN_PROCESSES, NUM_RESOURCES);

  // Now set the base of the array of shared_data just PAST the
  //   semaphore. Remember that the memory returned by the call
  //   to mmap() is just like memory returned by malloc().
  shared_base = (struct shared_data *) (shared_void + sizeof(sem_t));

  // We'll use the signal_received variable to make sure we don't
  //   exit the process while holding the semaphore locked.
  for (j = 0; (j < 30) && (!signal_received); j++) {
    time_t t;
    int i;

    fprintf(stdout, "Updating data %d\n", j);
    fprintf(stdout, "  ++ Waiting for semaphore lock\n");
    // Try to __decrement__ (aka lock) the semaphore. If sem > 0,
    //   then the lock succeeds and we proceed, otherwise block until
    //   it can get the lock.
    // There is nothing about "fairness" associated with getting the
    //   lock on a semaphore. Sometimes you are lucky and other
    //   times... ... ... You wait (aka block).
    // There are sem_trywait() and sem_timedwait() calls for
    //   locking/checking a semaphore.
    // There is also a sem_getvalue() call to return the current 
    //   value of a semaphore, but you don't know how long that
    //   value will be correct. The sem_getvalue() call can be
    //   useful if you want to know how many processes are blocked
    //   waiting on the semaphore.
    res = sem_wait(sem);
    if (res < 0) {
      perror("Failed on lock semaphore");
      break;
    }
    fprintf(stdout, "  ++ Got the semaphore lock\n");

    // Now just set some random values into the elements of
    //   the array of elements.
    for (i = 0; i < NUM_ELEMENTS; i++) {
      shared_base[i].counter = counter ++;
      shared_base[i].ele_type = random() % ELE_TYPES;
      shared_base[i].var1 = random() % 1000;
      shared_base[i].var2 = random() % 100;
      snprintf(shared_base[i].text
	       , sizeof(shared_base[i].text)
	       , "My message, type %d, num %ld"
	       , shared_base[i].ele_type
	       , random() % 10);
    }

    t = time(NULL);
    fprintf(stdout, "\t%s\n", ctime(&t));

    sleep(3);

    // Increment (aka unlock) the semaphore. We "post" on a
    //   semaphore to "unlock" it because there may be mor 
    //   than one process blocked on it.
    res = sem_post(sem);
    if (res < 0) {
      perror("could not unlock semaphore");
      break;
    }
    fprintf(stdout, "  -- Unlocked the semaphore\n");
    sleep(1);
  }

  // Start to clean things up.
  res = munmap(shared_void, shared_seg_size);
  if (res < 0) {
    perror("Failed un-mapping shared memory object");
  }
  // Notice that the function to close a shared memory object is our
  //   old friend close(), but to close a named semaphore, we have to
  //   call a different function.
  res = close(shmfd);
  if (res < 0) {
    perror("Failed closing shared memory object");
  }
  // No semaphore to close here. Unnamed semaphores do not need
  //   to be closed.

  return(EXIT_SUCCESS);
}
