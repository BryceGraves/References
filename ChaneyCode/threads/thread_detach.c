 /*
 * R Jesse Chaney
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/wait.h>

// We are going to pass an instance of this structure to each thread.
struct work_args_s {
  int id;
  long sleep_time;
};

static volatile sig_atomic_t val = -1;

extern void *do_work(void *a);

int 
main(
     int argc
     , char *argv[]
     , char *envp[]
) {
  int i;
  int num_threads;
  int ret;

  pthread_t tid;
  // Allocate some space to send things to the threads.
  struct work_args_s *wargs;

  // Initialize the random number generator with something sort of random.
  srandom((unsigned int) time(NULL));
  
  if (argc < 2) {
    printf("** Must give number of threads to create **\n");
    exit(EXIT_FAILURE);
  }
  num_threads = strtol(argv[1], NULL, 10);

  wargs = (struct work_args_s *) malloc(num_threads * sizeof(struct work_args_s));
  val = -1;

  for(i = 0; i < num_threads; i++) {
    // Assign an easy to read thread id and a random amount of work to do in
    // each thread.
    wargs[i].id = i;
    wargs[i].sleep_time = (random() % num_threads) + 1;

    printf("Creating thread %d with work value %ld\n"
	   , i, wargs[i].sleep_time);
    // Create a new thread and have it call the do_work() function.
    // The function to call for work could be named anything.
    // Since I am not going to join with the new threads, I don't
    // need to keep track of the thread ids.
    ret = pthread_create(&tid
			 , NULL // things like joinable, or detached...
			 , do_work // The function to call.
			 , (void *) &wargs[i]); // Pointer to data.
    if (ret != 0) {
      // Rats...
      printf("Failed thread creation %d\n", ret);
      exit(EXIT_FAILURE);
    }	
  }

  // In this case, we do not want to do this.
  //free(wargs);

  //return(EXIT_SUCCESS);
  pthread_exit(EXIT_SUCCESS);
}

void *
do_work(
	void *a
) {
  // We need to cast the pointer passed to the work function
  // to be the correct type.
  struct work_args_s *arg = (struct work_args_s *) a;

  // Detach this thread.  It cannot be joined with another thread
  // after this call.
  pthread_detach(pthread_self());

  printf("  Detached thread %d doing %ld units of work.\n"
	 , arg->id, arg->sleep_time);
  // Simulate doing some useful work with a sleep()
  sleep(arg->sleep_time);
  // The work is done.
  printf("    Thread %d done working\n", arg->id);

  val = arg->id;

  pthread_exit(EXIT_SUCCESS);
}
