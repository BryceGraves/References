/*
 * R Jesse Chaney
 */

#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#define SEM_NAME "/sem_simple"

int main(int argc, char *argv[])
{
  sem_t * my_semaphore;

  my_semaphore = sem_open(SEM_NAME
                          , O_CREAT, S_IRUSR | S_IWUSR, 10);

  if (my_semaphore < 0) {
    perror("sem_open failed");
    exit(1);
  }

  if (argc > 1) {
    sem_close(my_semaphore);

    sem_unlink(SEM_NAME);
  }

  return(0);
}
