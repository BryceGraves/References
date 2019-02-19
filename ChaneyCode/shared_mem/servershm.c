/*
 * R Jesse Chaney
 */

// Compile:
//    gcc -o servershm servershm.c -lrt

#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>

#include "shm.h"

static void signal_handler(int);
static void onexit_function(void);

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
//#define CLEAN_SHM
#ifdef CLEAN_SHM
  char sharedMemName[100];

  sprintf(sharedMemName, "%s_%s", SHMOBJ_PATH, getenv("LOGNAME"));
  if (shm_unlink(sharedMemName) != 0) {
    perror("Failed in shm_unlink()");
  }
#endif // CLEAN_SHM
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
  int j;
  // the shared segment, and head of the elements list
  void *shared_void;
  struct shared_data *shared_base;
  struct shared_data *shared_ele1;

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
    exit(EXIT_FAILURE);
  }
  fprintf(stderr, "Created shared memory object %s\n", sharedMemName);
  // Adjust the mapped file size.
  //   (make room for the whole segment to map) -- ftruncate()
  shared_seg_size = (NUM_ELEMENTS * sizeof(struct shared_data));
  ret_val = ftruncate(shmfd, shared_seg_size);
  if (ret_val < 0) {
    perror("ftruncate failed");
    exit(EXIT_FAILURE);
  }
  fprintf(stdout, "shared memory object truncated to %d bytes\n"
	  , shared_seg_size);

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
    perror("In mmap()");
    exit(EXIT_FAILURE);
  }

  fprintf(stdout
	  , "Shared memory segment allocated "
	    "and connected correctly (%d bytes).\n\n"
	  , shared_seg_size);
  srandom(time(NULL));

  shared_base = (struct shared_data *) shared_void;
  shared_ele1 = shared_base;

  for (j = 0; (j < 30); j++) {
    int i;

    fprintf(stdout, "Updating counter %d\n", j);
    shared_ele1->counter = counter ++;
    shared_ele1->ele_type = random() % ELE_TYPES;
    shared_ele1->var1 = random() % 1000;
    shared_ele1->var2 = random() % 100;
    snprintf(shared_ele1->text
	     , sizeof(shared_ele1->text)
	     , "My element, type %d, num %ld"
	     , shared_ele1->ele_type
	     , random() % 10);

    for (i = 1; i < NUM_ELEMENTS; i++) {
      // We can treat the shared memory object like an array too.
      // This is how you would use memory from a malloc() call.
      shared_base[i].counter = counter++;
      shared_base[i].ele_type = random() % ELE_TYPES;
      shared_base[i].var1 = random() % 1000;
      shared_base[i].var2 = random() % 100;
      snprintf(shared_base[i].text
	       , sizeof(shared_base[i].text)
	       , "  My element, type %d, num %ld"
	       , shared_base[i].ele_type
	       , random() % 10);
    }
    //printf("-----------------------------------------------------\n");
    sleep(3);
  }

  munmap(shared_void, shared_seg_size);
  close(shmfd);
  return(EXIT_SUCCESS);
}
