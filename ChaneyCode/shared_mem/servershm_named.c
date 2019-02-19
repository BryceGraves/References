/*
 * R Jesse Chaney
 */

// Compile:
//    gcc -o servershm_named servershm_named.c -lrt

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
#define CLEAN_SHM
#ifdef CLEAN_SHM
  char sharedMemName[100];
  char sharedSemName[100];

  // Cleanup and remove all the objects created by this process.
  sprintf(sharedMemName, "%s_%s", SHMOBJ_PATH, getenv("LOGNAME"));
  if (shm_unlink(sharedMemName) != 0) {
    perror("Failed in shm_unlink()");
  }

  // We need to unlink the named sempahore as well.
  sprintf(sharedSemName, "%s_%s", SEMOBJ_PATH, getenv("LOGNAME"));
  if (sem_unlink(sharedSemName) != 0) {
    perror("Failed in sem_unlink()");
  }
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
  char sharedSemName[100];
  sem_t *sem;
  int shared_seg_size = -1;
  void *shared_void;
  struct shared_data *shared_base;

  umask(0);
  atexit(onexit_function);
  signal(SIGINT, signal_handler);
  srandom(time(NULL));

  sprintf(sharedMemName, "%s_%s", SHMOBJ_PATH, getenv("LOGNAME"));
  // Creating the shared memory object. It sets the
  //   permissions exactly the same as the permissions on files
  //   you create with the open() call.
  shmfd = shm_open(sharedMemName
		   , (O_CREAT | O_RDWR | O_EXCL)
		   , (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH));
  if (shmfd < 0) {
    perror("In shm_open()");
    exit(EXIT_FAILURE);
  }
  fprintf(stderr, "Created shared memory object %s\n", sharedMemName);
  // Adjust the mapped object size using ftruncate(). It defaults to
  //   a size of zero bytes.
  shared_seg_size = (NUM_ELEMENTS * sizeof(struct shared_data));
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
  // We want Updates to the object to be visible to other processes 
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
  // Now set the base of the array of shared_data. Remember 
  //   that the memory returned by the call to mmap() is 
  //   just like memory returned by malloc().
  shared_base = (struct shared_data *) (shared_void);

  // Just as the shared memory object has a name in the file system,
  //   a named semahore has a name in the file system. You can find
  //   the named semaphore in the /dev/shm directory.
  sprintf(sharedSemName, "%s_%s", SEMOBJ_PATH, getenv("LOGNAME"));

  // This will be the number of processes allowed through the semaphore
  //   at a time. Or, how many units of a resource can be accessed at 
  //   a time. IN this example, we create a binary semaphore. If you
  //   wanted to allow mor than one process at a time through a semaphore
  //   you'd just initialized with that value.
# define NUM_RESOURCES          1

  // Create and open the named semaphore.
  sem = sem_open(sharedSemName
		 , (O_CREAT | O_RDWR | O_EXCL)
		 , (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
		 , NUM_RESOURCES);
  fprintf(stdout, "Created named semaphore %s\n", sharedSemName);

  // We'll use the signal_received variable to make sure we don't
  //   exit the process while holding the semaphore locked. If
  //   the process exits while it holds the semaphore locked,
  //   no other process will be able to unlocked it. The result
  //   will be deadlock for the other processes.
  for (j = 0; (j < 30) && (!signal_received); j++) {
    time_t t;
    int i;

    fprintf(stdout, "Updating data %d\n", j);
    fprintf(stdout, "  ++ Waiting for semaphore lock\n");
    // Try to __decrement__ (aka lock) the semaphore. If sem > 0,
    //   then the lock succeeds and proceed, otherwise block until
    //   it can get the lock.
    // There is nothing about "fairness" associated with getting the
    //   lock on a semaphore. Sometimes you are lucky and other
    //   times... ... ...
    // There are sem_trywait() and sem_timedwait() calls for
    //   locking a semaphore. As with the pthread_mutex_timedlock()
    //   call, the timeout specifies an absolute time in the future
    //   not just a timeout in seconds (or [milli,micro,fempto]-seconds).
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
    fprintf(stdout, "     %s\n", ctime(&t));

    sleep(3);

    // Increment (aka unlock) the semaphore.
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
  res = sem_close(sem);
  if (res < 0) {
    perror("Failed closing named semaphore");
  }

  return(EXIT_SUCCESS);
}
