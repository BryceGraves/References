/*
 * R Jesse Chaney
 */

// Compile with
//    gcc -Wall -o thread_mutex thread_mutex.c -pthread
//
// Run with something like this:
//    ./thread_mutex 5 1000000
//

#include <stdio.h>
#include <sys/times.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

static double *x_array;
static double *y_array;
static long num_threads = -1;
static long work_per_thread = -1;
static long length = -1;
static volatile double sum;
static pthread_mutex_t mutex_sum;
static pthread_spinlock_t spin_sum;

extern void serial( void );
extern void *thread_nomutex( void *);
extern void *thread_horridmutex( void *);
extern void *thread_badmutex( void *);
extern void *thread_goodmutex( void *);
extern void *thread_spin( void *);

int 
main(
     int argc
     , char *argv[]
     , char *envp[]
) {
  pthread_t *thread;
  clock_t start;      /* start time for measuring program performance */
  clock_t stop;       /* stop time for measuring program performance */
  long i;
  int ret;

  if (argc < 3) {
    printf("Must provide values for number of threads and work per thread\n");
    printf("  Something like:\n    %s 5 1000000\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  // Pull the number of threads and work per thread off
  // the command line.  Some range checks would be nice.
  num_threads = (long) atoi(argv[1]);
  work_per_thread = (long) atoi(argv[2]);
  length = num_threads * work_per_thread;
  printf("Running with %d threads and %d work per thread\n"
	 , (int) num_threads, (int) work_per_thread);

  // Initialize the mutex.  You MUST do this before you use
  // the mutex.
  pthread_mutex_init(&mutex_sum, NULL);
  pthread_spin_init(&spin_sum, PTHREAD_PROCESS_PRIVATE);
 
  // Initialize the random number generator.
  srandom((unsigned int) time(NULL));
  // Create the arrays that will hold some values.
  x_array = (double *) malloc(length * sizeof(double));
  y_array = (double *) malloc(length * sizeof(double));
  // Initialize the arrays.
  sum = 0.0;
  for (i = 0; i < length; ++i) {
    x_array[i] = ((double) (random() % 100)) + 1.0;
    y_array[i] = ((double) (random() % 100)) + 1.0;
  }

  // Create the array that will hold the thread ids.
  thread = (pthread_t *) malloc(num_threads * sizeof(pthread_t));

  {
    double val_serial = 0.0;
    double time_serial = 0.0;

    // ***************************************************************
    // This is just the simple serial calculation.  It will be
    // our baseline for the other functions.
    sum = 0.0;
    start = clock();
    serial();
    stop = clock();
    val_serial = sum;
    time_serial = ((double) stop - (double) start) / (double) CLOCKS_PER_SEC;
    // ***************************************************************
    printf("  Serial time value \t\t%18.2f \t%10.4f seconds\n"
	   , val_serial, time_serial);
  }

#define THOUGHTLESS_THREADED
#ifdef THOUGHTLESS_THREADED
  {
    double val_nomutex = 0.0;
    double time_nomutex = 0.0;

    // ***************************************************************
    // Just take the serial routine and call it with a bunch of threads.
    // There is a small problem.
    sum = 0.0;
    start = clock();
    for (i = 0; i < num_threads; i++) {
      ret = pthread_create(&thread[i], NULL, thread_nomutex, (void *) i);
      if (ret != 0) {
	perror("Cannot create the nomutex threads.");
	exit(EXIT_FAILURE);
      }
    }

    // Wait for all the threads to complete.
    for (i = 0; i < num_threads; i++) {
      pthread_join(thread[i], NULL);
    }
    stop = clock();
    val_nomutex = sum;
    time_nomutex = (double) (stop - start) / (double) CLOCKS_PER_SEC;
    // ***************************************************************
    printf("  Threaded with no mutex \t%18.2f \t%10.4f seconds\n"
	   , val_nomutex, time_nomutex);
  }
#endif // THOUGHTLESS_THREADED

#define HORRID_MUTEX
#ifdef HORRID_MUTEX
  {
    double val_badmutex = 0.0;
    double time_badmutex = 0.0;

    // ***************************************************************
    // Put the mutex around the piece of code that modifies the
    // the global variable.
    // There is a small problem.
    sum = 0.0;
    start = clock();
    for (i = 0; i < num_threads; i++) {
      ret = pthread_create(&thread[i], NULL, thread_badmutex, (void *) i);
      if (ret != 0) {
	perror("Cannot create the bad mutex threads.");
	exit(EXIT_FAILURE);
      }
    }

    // Wait for all the threads to complete.
    for (i = 0; i < num_threads; i++) {
      pthread_join(thread[i], NULL);
    }
    stop = clock();
    val_badmutex = sum;
    time_badmutex = (double) (stop - start) / (double) CLOCKS_PER_SEC;
    // ***************************************************************
    printf("  Threaded with horrid mutex \t%18.2f \t%10.4f seconds\n"
	   , val_badmutex, time_badmutex);
  }
#endif // HORRID_MUTEX


#define BAD_MUTEX
#ifdef BAD_MUTEX
  {
    double val_badmutex = 0.0;
    double time_badmutex = 0.0;

    // ***************************************************************
    // Put the mutex around the piece of code that modifies the
    // the global variable.
    // There is a small problem.
    sum = 0.0;
    start = clock();
    for (i = 0; i < num_threads; i++) {
      ret = pthread_create(&thread[i], NULL, thread_badmutex, (void *) i);
      if (ret != 0) {
	perror("Cannot create the bad mutex threads.");
	exit(EXIT_FAILURE);
      }
    }

    // Wait for all the threads to complete.
    for (i = 0; i < num_threads; i++) {
      pthread_join(thread[i], NULL);
    }
    stop = clock();
    val_badmutex = sum;
    time_badmutex = (double) (stop - start) / (double) CLOCKS_PER_SEC;
    // ***************************************************************
    printf("  Threaded with bad mutex \t%18.2f \t%10.4f seconds\n"
	   , val_badmutex, time_badmutex);
  }
#endif // BAD_MUTEX

#define GOOD_MUTEX
#ifdef GOOD_MUTEX
  {
    double val_goodmutex = 0.0;
    double time_goodmutex = 0.0;

    // ***************************************************************
    // Isolate the code that changes the global variable using a
    // partial sum.
    sum = 0.0;
    start = clock();
    for (i = 0; i < num_threads; i++) {
      ret = pthread_create(&thread[i], NULL, thread_goodmutex, (void *) i);
      if (ret != 0) {
	perror("Cannot create the mutex threads.");
	exit(EXIT_FAILURE);
      }
    }

    // Wait for all the threads to complete.
    for (i = 0; i < num_threads; i++) {
      pthread_join(thread[i], NULL);
    }
    stop = clock();
    val_goodmutex = sum;
    time_goodmutex = (double) (stop - start) / (double) CLOCKS_PER_SEC;
    // ***************************************************************
    printf("  Threaded with good mutex \t%18.2f \t%10.4f seconds\n"
	   , val_goodmutex, time_goodmutex);
  }
#endif // GOOD_MUTEX

#define DO_SPIN
#ifdef DO_SPIN
  {
    double val_spin = 0.0;
    double time_spin = 0.0;

    // ***************************************************************
    // Isolate the code that changes the global variable using a
    // partial sum.
    sum = 0.0;
    start = clock();
    for (i = 0; i < num_threads; i++) {
      ret = pthread_create(&thread[i], NULL, thread_spin, (void *) i);
      if (ret != 0) {
	perror("Cannot create the mutex threads.");
	exit(EXIT_FAILURE);
      }
    }

    // Wait for all the threads to complete.
    for (i = 0; i < num_threads; i++) {
      pthread_join(thread[i], NULL);
    }
    stop = clock();
    val_spin = sum;
    time_spin = (double) (stop - start) / (double) CLOCKS_PER_SEC;
    // ***************************************************************
    printf("  Threaded with spin lock \t%18.2f \t%10.4f seconds\n"
	   , val_spin, time_spin);
  }
#endif // DO_SPIN

  pthread_mutex_destroy(&mutex_sum);
  pthread_spin_destroy(&spin_sum);

  free(x_array);
  free(y_array);
  free(thread);  // Be good and free the allocated memory.

  return(EXIT_SUCCESS);
}

void
serial( 
       void
) {
  long i;

  // Compute the dot product of the array elements.
  // This is just a serial (single threaded) implementation that
  // we use to generate our baseline value and time.
  for (i = 0; i < length; ++i) {
    sum += x_array[i] * y_array[i];
  }  

  return;
}

void *
thread_nomutex(
       void *arg 
) {
  // I'm being a bit careless on this as I pass the id as if it
  // is a pointer, but then cast it back to being a long.
  long id = (long) arg;
  long i;

  // We use a "bunch-o-threads" to calculate the value.  It is
  // fast, but also ... ... ... WRONG!
  for (i = id; i < length; i += num_threads) {
    // Just add the value to the global variable, no critical
    // region declared or used.
    sum += x_array[i] * y_array[i];
  }

  pthread_exit(EXIT_SUCCESS);
}

void *
thread_badmutex(
	void *arg
) {
  long id = (long) arg;
  long i;
  
  // We throw in a mutex to make sure the value we get out
  // is correct (and it is), but the takes a looooooong time.
  for (i = id; i < length; i += num_threads) {
    pthread_mutex_lock(&mutex_sum);
    // Put the mutex inside the loop.  This places a lock on
    // the global variable EVERY time we want to update it.
    // That means that all the threads wait while one thread
    // is updating the value.
    sum += x_array[i] * y_array[i];
    pthread_mutex_unlock(&mutex_sum);
  }
  
  pthread_exit(EXIT_SUCCESS);
}

void *
thread_horridmutex(
	void *arg
) {
  long id = (long) arg;
  long i;
  
  // We throw in a mutex to make sure the value we get out
  // is correct (and it is), but the takes a looooooong time.
  pthread_mutex_lock(&mutex_sum);
  for (i = id; i < length; i += num_threads) {
    // Put the mutex inside the loop.  This places a lock on
    // the global variable EVERY time we want to update it.
    // That means that all the threads wait while one thread
    // is updating the value.
    sum += x_array[i] * y_array[i];
  }
  pthread_mutex_unlock(&mutex_sum);
  
  pthread_exit(EXIT_SUCCESS);
}

void *
thread_goodmutex(
	 void *arg
) {
  long id = (long) arg;
  long i;
  double partial_sum = 0;

  // We move the mutex outside the loop and calculate
  // a partial sum without the need of a lock.
  for (i = id; i < length; i += num_threads) {
    partial_sum += x_array[i] * y_array[i];
  }
 
  // Only when we need to update the global value with the
  // partial sum do we actually need to lock the mutex.
  {
    pthread_mutex_lock(&mutex_sum);
    // Put a mutex only around the portion of code that
    // needs to modify the global value.
    sum += partial_sum;
    pthread_mutex_unlock(&mutex_sum);
  }
  
  pthread_exit(EXIT_SUCCESS);
}

void *
thread_spin(
	 void *arg
) {
  long id = (long) arg;
  long i;
  double partial_sum = 0;

  // We move the mutex outside the loop and calculate
  // a partial sum without the need of a lock.
  for (i = id; i < length; i += num_threads) {
    partial_sum += x_array[i] * y_array[i];
  }
 
  // Only when we need to update the global value with the
  // partial sum do we actually need to lock the mutex.
  {
    pthread_spin_lock(&spin_sum);
    // Put a mutex only around the portion of code that
    // needs to modify the global value.
    sum += partial_sum;
    pthread_spin_unlock(&spin_sum);
  }
  
  pthread_exit(EXIT_SUCCESS);
}
