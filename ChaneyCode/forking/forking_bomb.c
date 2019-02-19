/*
 * R Jesse Chaney
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main( void ) 
{
  pid_t pid = -1;
  unsigned count = 0;

  // Only do this if you want to create a LOT of processes.
  exit(0);
  while ((pid = fork()) != 0) {
    printf("Fork bomb new_child=%d self=%d parent=%d count=%d\n"
	   , pid, getpid(), getppid(), count);
    if (pid < 0) {
      break;
    }
    count++;
  }

  return(0);
}
