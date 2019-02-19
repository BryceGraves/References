 /*
 * R Jesse Chaney
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>

#ifndef TRUE 
# define TRUE 1
#endif // TRUE
#ifndef FALSE
# define FALSE 0
#endif // FALSE

static unsigned short use_locks = FALSE;
static sig_atomic_t glob = 0;
static pthread_mutex_t glob_mutex = PTHREAD_MUTEX_INITIALIZER;

static void *do_work(void *a);
static void incr(unsigned int loops, unsigned long id);

int 
main(
     int argc
     , char *argv[]
     , char *envp[]
) {
  unsigned long i;
  int num_threads = 40;
  int ret;
  pthread_t *threads;
  
  srand(num_threads);
  if (argc > 1) {
    use_locks = TRUE;
  }
  threads = (pthread_t *) malloc(num_threads * sizeof(pthread_t));

  for(i = 0; i < num_threads; i++) {
    printf("Creating thread %lu\n", i);

    ret = pthread_create(
			 // Create the thread and store the tid in the array.
			 &(threads[i])
			 , NULL // For attributes like joinable, ...
			 , do_work 
			 , (void *) (num_threads - i));
    if (ret != 0) {
      // Ooopsie...
      perror("Thread creation failed");
      printf("Failed thread creation %d\n", ret);
      exit(EXIT_FAILURE);
    }	
  }
  printf("threads created\n");
  for(i = 0; i < num_threads; i++) {
    printf("  waiting on %lu\n", i);
    pthread_join(threads[i], NULL);
    printf("Joined with thread %lu\n", i);
  }
  printf("\nThe final value of glob is %d\n", glob);

  free(threads);
  pthread_exit(EXIT_SUCCESS);
}

static void incr(unsigned int loops, unsigned long id)
{
  int loc, j;
  useconds_t s;

  printf("  >> thread %lu\n", id);

  //  pthread_mutex_lock(&glob_mutex);
  //  loc = glob;
  //  pthread_mutex_unlock(&glob_mutex);

  for (j = 0; j < loops; j++) {
    if (use_locks == TRUE) {
      pthread_mutex_lock(&glob_mutex);
      //printf("    >> thread %lu locking\n", id);
    }
    loc = glob;
    loc++;
    s = rand() % id;
    usleep(s);
    glob = loc;
    if (use_locks == TRUE) {
      //printf("    >> thread %lu unlocking\n", id);
      pthread_mutex_unlock(&glob_mutex);
    }
  }
  //  pthread_mutex_lock(&glob_mutex);
  //  glob += loc;
  //  pthread_mutex_unlock(&glob_mutex);
}

void *
do_work(
	void *void_arg
) {
  unsigned long id = (unsigned long) void_arg;

  printf("  >> thread %lu\n", id);
  usleep((useconds_t) id);
  incr(5000, id);

  pthread_exit(EXIT_SUCCESS);
}
