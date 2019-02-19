 /*
 * R Jesse Chaney
 */

/////////////////////////////////////////////////////////////////
// Compile with:
//    gcc -o thread_simple thread_simple.c -pthread
/////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/wait.h>

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
// We are going to pass an instance of this structure to each 
//   thread we create.
typedef struct thread_args_s {
  int id;
  long sleep_time;
  char name[50];
  int exit_value;
} thread_args_t;

// We are going to use this to keep track of each thread as it 
//   completes. As described in the GNU docs:
//      This is an integer data type. Objects of this type are 
//      always accessed atomically.
// All reads from and writes to this type are atomic.
static volatile sig_atomic_t last_thread = -1;

// This is the the function that the threads will run.
extern void *do_work(void *a);

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
// This is the function that the threads will run when started.
// Once started, each thread pretty much runs on its own. There
//   may be synchronization points, but each thread is independent.
void *
do_work(
	void *void_arg
) {
  // We need to cast the pointer passed to the work function
  //   to be the correct type. Until we cast it, we cannot
  //   access the values within it.
  thread_args_t *arg = (thread_args_t *) void_arg;
  
  printf("  Thread %s starting\n", arg->name);
  // Show that the thread has started. The actual system
  //   thread id is unique within the process, but _may_ not be
  //   unique within the entire system. A thread id may be reused 
  //   within a  process.
  printf("    Thread %d doing %ld units of work.\n"
	 , arg->id, arg->sleep_time);
  // Simulate doing some useful work with a call to sleep(). In
  //   the real world, sleeping is rarely considered work.
  sleep(arg->sleep_time);
  // The work is done.
  printf("    Thread %d done working\n", arg->id);

  // Update the global last_thread with the id for the thread. Since
  //   there is only 1 last_thread, each thread updates the same variable.
  //   Since writes to variables of type sig_atomic_t are atomic,
  //   this is a safe thing to do.
  last_thread = arg->id;

  printf("  Thread %s complete\n", arg->name);
  // We could just return here, but pthread_exit() is the better 
  //   thing to do.
  //return;
  // If you return a value in the pthread_exit() call it should not 
  //   be a value from the thread's stack, since  the contents of 
  //   that stack become undefined on thread termination.
  //   In this case, we are going to use a field in the structure
  //   that was passed to the thread start function. The pthread_exit()
  //   function returns a pointer to a value, so I have to prefix
  //   the name with a &.
  arg->exit_value = arg->id;
  pthread_exit((void *) &(arg->exit_value));

  // Once the thread has exited, the main thread will call 
  //   pthread_join() to do the final cleanup of the thread 
  //   resourses.
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
int main(
     int argc
     , char *argv[]
     , char *envp[]
) 
{
  int i;
  int num_threads;
  int ret;

  // These are the system thread ids for each of the threads we are 
  //   about to create. We need to store them. We'll make use of the
  //   the thread ids when we join the threads back together.
  pthread_t *threads;
  // Allocate some space to send things to the threads.
  thread_args_t *thread_data;

  // Initialize the random number generator with something a 
  //   little bit random.
  srandom((unsigned int) time(NULL));
  
  if (argc < 2) {
    printf("** Must give number of threads to create **\n");
    exit(EXIT_FAILURE);
  }
  // Convert the value from the command line into an integer.
  num_threads = strtol(argv[1], NULL, 10);

  // Allocate some space to save the thread ids of the threads
  //   as they are created. We'll use those thread ids when 
  //   we join back with the completed threads.
  threads = (pthread_t *) malloc(num_threads * sizeof(pthread_t));

  // We are going to pass some data to each thread as it is
  //   created. We need to allocate spaced for that data as
  //   well.
  thread_data = (thread_args_t *) malloc(num_threads * sizeof(thread_args_t));

  for(i = 0; i < num_threads; i++) {
    // Assign an easy to read thread id and a random amount of work to do to
    //   each thread.
    thread_data[i].id = i;
    // Generate a random number bwtween 1 and the number
    //   of threads.
    thread_data[i].sleep_time = (random() % num_threads) + 1;
    sprintf(thread_data[i].name, "Thread%d", i);

    printf("Creating thread %d with work value %ld\n"
	   , i, thread_data[i].sleep_time);
    // Create a new thread and have it call the do_work() function.
    //   The function to call for work could be named anything, in
    //   this case we call it do_work.
    ret = pthread_create(
			 // Create the thread and store the tid in the array.
			 &(threads[i])
			 , NULL // For attributes like joinable, ...

			 // The function listed below is what each thread 
			 //   will run when the thread is created. The
			 //   threads will run asynchronously within the
			 //   given function.
			 , do_work 

			 // Pointer to data that is passed to the start
			 //   function for each thread. The start function
			 //   only takes a single argument, but we can pass
			 //   a pointer to a structure that contains many
			 //   items.
			 , (void *) &thread_data[i]); 
    if (ret != 0) {
      // Ooopsie...
      perror("Thread creation failed");
      printf("Failed thread creation %d\n", ret);
      exit(EXIT_FAILURE);
    }	
  }

  // Wait for each of threads to return. Just as we don't want 
  //   zombie processes we don't want zombie threads.
  // Notice that this will join with a specific thread. There 
  //   is not a generic "join with any thread" call, like 
  //   wait(). If you are wishing there was a "join with any thread"
  //   call or option, check out what the man paged for pthread_join()
  //   says about it.
  // Joining with a thread ID that has been already joined can 
  //   lead to unpredictable behavior (aka bad).
  // Unlike processes, threads do not have a parent child 
  //   relationship. All threads are peers. Any thread can 
  //   join with any other thread.
  for(i = 0; i < num_threads; i++) {
#define THREAD_RETURN
#ifdef THREAD_RETURN
    // If you don't care about the return value from the threads,
    //   you can just pass a NULL as the return value pointer. It
    //   is a lot easier to do that, but you miss out on the fun
    //   of more (void *) casts and (more importantally) can't
    //   determine the return status of the thread.
    int *thread_return_value;

    pthread_join(threads[i], (void **) &thread_return_value);
    printf("Joined with thread %d, name: %s  exit status: %d\n"
	   , i, thread_data[i].name, *thread_return_value);
#else // THREAD_RETURN
    // Don't bother with looking at the return value from the
    //  the threads.
    pthread_join(threads[i], NULL);
    printf("Joined with thread %d, name: %s\n"
      , i, thread_data[i].name);
#endif // THREAD_RETURN
  }
  // What was the last thread to complete. This is just for
  //   fun.
  printf("The last thread to exit was %d\n", last_thread); 
  
  // Return the memory we allocated back to the free list.
  free(threads);
  free(thread_data);

  // A call to exit()/return() and a call to pthread_exit could do
  //   different things at this point. A call to exit()/return()
  //   will immediately cause the entire process to terminate and
  //   all active threads will be killed. A call to to pthread_exit()
  //   will cause this thread (the main thread) to exit, but
  //   allows other running threads (and therefore the process)
  //   to continue. In this case, we know that all the other
  //   threads are already done, so the process will just
  //   terminate either way.
  //return(EXIT_SUCCESS);
  pthread_exit(EXIT_SUCCESS);
}
