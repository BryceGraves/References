/*
 * R Jesse Chaney
 */

#pragma once

# define SHMOBJ_PATH "/shm_demo"

// Don't get your shm's and sem's mixed up.
# define SEMOBJ_PATH "/sem_demo"

# define ELE_TYPES 100
# define NUM_ELEMENTS 100

# ifndef TRUE
#  define TRUE 1
# endif // TRUE
# ifndef FALSE
#  define FALSE 0
# endif // FALSE

// Data element structure for the shared memory object.
struct shared_data {
  int counter;
  int ele_type;
  int var1;
  int var2;
  char text[100];
#ifdef USE_SEMAPHORES
  sem_t lock;
#endif // USE_SEMAPHORES
};

typedef enum {NOT_CREATED = 0, CREATED} CreatedShmArray;
typedef enum {THREAD_SHARED = 0, PROCESS_SHARED} SemSharing;
