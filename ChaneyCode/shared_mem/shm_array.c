/*
 * R Jesse Chaney
 */

// Compile:
//    gcc -o shm_array shm_array.c -lrt

#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#define USE_SEMAPHORES
#include "shm.h"

static void signal_handler(int);
static void onexit_function(void);
static CreatedShmArray created = CREATED;

static void 
signal_handler(
       int signum
) {

  // Terminate program
  exit(signum);
}

void 
onexit_function(
     void 
) {
  char sharedMemName[100];

  if (created == CREATED) {
    sprintf(sharedMemName, "%s_%s", SHMOBJ_PATH, getenv("LOGNAME"));
    if (shm_unlink(sharedMemName) != 0) {
      perror("Failed in shm_unlink()");
    }
  }
}

int 
main(
     int argc
     , char *argv[]
     , char *envp[]
) {
  int ret_val;
  int shmfd;
  char sharedMemName[100];
  // want shared segment capable of storing elements
  int shared_seg_size = -1;
  int counter = 0;
  int i;
  int j;
  // the shared segment, and head of the elements list
  void *shared_void;
  struct shared_data *shared_base;

  umask(0);
  atexit(onexit_function);
  signal(SIGINT, signal_handler);

  sprintf(sharedMemName, "%s_%s", SHMOBJ_PATH, getenv("LOGNAME"));
  // creating the shared memory object -- shm_open()
  shmfd = shm_open(sharedMemName
		   , (O_CREAT | O_RDWR | O_EXCL)
		   , (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
		   );
  if (shmfd < 0) {
    perror("In shm_open()");
    //exit(EXIT_FAILURE);
    shmfd = shm_open(sharedMemName
		     , O_RDWR
		     , (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
		     );
    created = NOT_CREATED;
  }
  fprintf(stderr, "Created shared memory object %s\n", sharedMemName);
  // Adjust the mapped file size.
  //   (make room for the whole segment to map) -- ftruncate()
  shared_seg_size = (NUM_ELEMENTS * sizeof(struct shared_data));
  if (CREATED == created) {
    ret_val = ftruncate(shmfd, shared_seg_size);
    if (ret_val < 0) {
      perror("ftruncate failed");
      exit(EXIT_FAILURE);
    }
    fprintf(stdout, "shared memory object truncated to %d bytes\n"
	    , shared_seg_size);
  }
  // Attach to the shared segment -- mmap()
  // You need to cast the return type of this to be
  // somthing you like.
  shared_void = mmap(NULL
		     , shared_seg_size
		     , PROT_READ | PROT_WRITE
		     , MAP_SHARED
		     , shmfd
		     , 0);
  if (shared_void == NULL) {
    perror("Unable to map shared memory segment");
    exit(EXIT_FAILURE);
  }

  if (created) {
    fprintf(stdout
	    , "Shared memory segment mapped "
	    "and connected correctly (%d bytes).\n\n"
	    , shared_seg_size);
  }
  else {
    fprintf(stdout
	    , "Shared memory segment mapped\n");
  }
  srandom(time(NULL));

  shared_base = (struct shared_data *) shared_void;

  if (created) {
    for (i = 0; i < NUM_ELEMENTS; i++) {
      ret_val = sem_init(&(shared_base[i].lock), PROCESS_SHARED, 2);
      if (ret_val < 0) {
	perror("Failed to init semaphore");
      }

      shared_base[i].counter = 0;
      shared_base[i].ele_type = 0;
      shared_base[i].var1 = 0;
      shared_base[i].var2 = 0;
      strcpy(shared_base[i].text, "empty");
    }
  }
  fprintf(stderr, "Initialized shared memory object %s\n", sharedMemName);

  for ( ; ; ) {
    for (j = 0; (j < NUM_ELEMENTS); j++) {
      struct timespec ts;

      // We can treat the shared memory object like an array too.
      // This is how you would use memory from a malloc() call.
      printf("Working on index %d\n", j);

      ts.tv_sec = 1;
      ts.tv_nsec = 0;
      if (sem_timedwait(&(shared_base[j].lock), &ts) < 0) {
	printf("    giving up on lock %d\n", j);
	printf("       skipping\n");
	continue;
      }

      printf("    got index %d\n", j);

      shared_base[j].counter = counter++;
      shared_base[j].ele_type = random() % ELE_TYPES;
      shared_base[j].var1 = random() % 1000;
      shared_base[j].var2 = random() % 100;
      snprintf(shared_base[j].text
	       , sizeof(shared_base[j].text)
	       , "  My element, type %d, num %ld"
	       , shared_base[j].ele_type
	       , random() % 10);

      sleep(5);
      sem_post(&(shared_base[j].lock));
      printf("released index %d\n", j);
    }
  }

  //  munmap(shared_void, shared_seg_size);
  //close(shmfd);
  return(EXIT_SUCCESS);
}
