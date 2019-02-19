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
  pid_t pid;
  printf("The original process with PID %d and PPID %d.\n" 
	 , getpid(), getppid());
  pid = fork();
  if (0 == pid) { /* The child process */
    printf("  >> The return value from fork() %d\n", pid);
    printf("  >> The child process with PID %d and PPID %d.\n" 
	   , getpid(), getppid());
  }
  else { /* Parent process */
    printf("  << The return value from fork() %d\n", pid);
    printf("  << The parent process with PID %d and PPID %d.\n" 
	   , getpid(), getppid());
    printf("  << Child PID is %d\n", pid);
  }
  /* Both Processes run this */
  printf("%s PID %d terminates.\n", (pid == 0) ? "child " : "parent ", getpid());

  return(0);
}
