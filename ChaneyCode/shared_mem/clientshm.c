/*
 * R Jesse Chaney
 */

// Compile:
//    gcc -o clientshm clientshm.c -lrt

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

extern void signal_handler(int);

void 
signal_handler(
    int signum
) {
  // Terminate program
  exit(signum);
}

int 
main(
     int argc
     , char *argv[]
     , char *envp[]
) {
  int shmfd;
  int j;
  // A shared segment capable of storing elements
  int shared_seg_size = (NUM_ELEMENTS * sizeof(struct shared_data));
  // The shared segment, and head of the elements list
  void *shared_void;
  struct shared_data *shared_base;
  //struct shared_data *shared_msg1;
  //struct shared_data *shared_msg2;
  char sharedMemName[100];

  signal(SIGINT, signal_handler);
  sprintf(sharedMemName, "%s_%s", SHMOBJ_PATH, getenv("LOGNAME"));
  // Create the shared memory object -- shm_open()
  shmfd = shm_open(sharedMemName
		   , O_RDWR
		   , S_IRUSR | S_IWUSR | S_IRGRP
		   );
  if (shmfd < 0) {
    perror("In shm_open()");
    exit(EXIT_FAILURE);
  }
  fprintf(stdout
	  , "Connected to shared memory object %s\n"
	  , sharedMemName);
  // Adjusting mapped memory size -- ftruncate()
  //ret_val = ftruncate(shmfd, shared_seg_size);
  //if (ret_val < 0) {
  //  perror("ftruncate failed");
  //  exit(1);
  //}
  // Attach to the shared segment -- mmap()
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
  shared_base = (struct shared_data *) shared_void;

  fprintf(stdout
	  , "Shared memory segment connected correctly (%d bytes).\n\n"
	  , shared_seg_size);
  for (j = 0; (j < 30); j++) {
    fprintf(stdout, "The ele_type is %d \n", shared_base->ele_type);
    fprintf(stdout, "  The counter is %d \n", shared_base->counter);
    fprintf(stdout, "  The var1 is %d \n", shared_base->var1);
    fprintf(stdout, "  The var2 is %d \n", shared_base->var2);
    fprintf(stdout, "  The text is '%s' \n\n", shared_base->text);

    fprintf(stdout, "  The ele_type is %d \n", shared_base[1].ele_type);
    fprintf(stdout, "    The counter is %d \n", shared_base[1].counter);
    fprintf(stdout, "    The var1 is %d \n", shared_base[1].var1);
    fprintf(stdout, "    The var2 is %d \n", shared_base[1].var2);
    fprintf(stdout, "    The text is '%s' \n\n", shared_base[1].text);

    fprintf(stdout, "  The ele_type is %d \n", shared_base[2].ele_type);
    fprintf(stdout, "    The counter is %d \n", shared_base[2].counter);
    fprintf(stdout, "    The var1 is %d \n", shared_base[2].var1);
    fprintf(stdout, "    The var2 is %d \n", shared_base[2].var2);
    fprintf(stdout, "    The text is '%s' \n\n", shared_base[2].text);

    fprintf(stdout, "-----------------------------------------------------\n");

    sleep(2);
  }

  munmap(shared_void, shared_seg_size);
  close(shmfd);
  return(EXIT_SUCCESS);
}
