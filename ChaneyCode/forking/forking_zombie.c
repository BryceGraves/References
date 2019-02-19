/*
 * R Jesse Chaney
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

void simple_SIGCHLD(int sig);
void simple_SIGINT(int sig);

int 
main(
     int argc
     , char *argv[]
     , char *envp[]
)
{
  pid_t cpid;
  pid_t mypid = getpid();
  int i;
  int wait_for_children = 0;

  // With this line commented out, the child processes will
  // exit, but they will not be reaped by the parent procress.
  // That will leave them as zombies. When the parent process
  // is killed, the child process become orphans, they are
  // adoped by the init process, which does call wait() and
  // the child processes are reaped and go away
  //(void) signal(SIGCHLD, simple_SIGCHLD);
  signal(SIGINT, simple_SIGINT);
  if (argc > 1) {
    if (strcmp(argv[1], "-i") == 0) {
      printf("The SIGCHLD signal will be received and ignored with SIG_IGN\n");
      signal(SIGCHLD, SIG_IGN);
    }
    else if (strcmp(argv[1], "-c") == 0) {
      printf("The SIGCHLD signal will be caught and handled with a handler\n");
      signal(SIGCHLD, simple_SIGCHLD);
    }
    else if (strcmp(argv[1], "-w") == 0) {
      printf("The wait() call will be used to prevent Zombies\n");
      wait_for_children = 1;
    }
    else {
      printf("Unknown command line argument >%s<." 
	     "No handlers established, there be Zombies here\n", argv[1]);
    }
  }
  else {
    printf("No handlers established, there be Zombies here.\n");
  }

  fprintf(stderr, "PARENT: pid = %d  line: %d\n", mypid, __LINE__);

  for (i = 0; i < 5; i++) {
    switch((cpid = fork())) {
    // Both parent and child processes continue executing here
    case -1:
      // We had a failure to fork.
      fprintf(stderr, "*** Failure to fork() *** %d \n", errno);
      perror("\t**fork() failure ***\n");
      break;
    case 0:
      mypid = getpid();
      fprintf(stderr, "   CHILD: pid = %d loop value: %d line: %d\n"
	      , mypid, i, __LINE__);
      // In this case, we are just going to let the child processes
      // continue and exit.
      break;
    default:
      // Parent specific code goes here.
      break;
    }
    if (0 == cpid) {
      // This is a child.
      // We are going to break out of the loop and exit.
      break;
    }
  }

  if (0 != cpid) {
    if (wait_for_children) {
      cpid = 1;
      sleep(3);
      while (-1 != cpid) {
	int status;

	cpid = wait(&status);
	if (cpid > 0) {
	  printf("Child process %d has exited with status %d\n"
		 , cpid
		 , WEXITSTATUS(status)
		 );
	}
      }
    }
    else {
      sleep(1);
      fprintf(stderr, "PARENT paused: pid = %d  line: %d\n", mypid, __LINE__);
      // This is the parent process only.
      pause();
    }
  }
  else {
    // Create an interesting exit value.
    int exit_status = getpid() - getppid();
    printf("   CHILD: exiting with value %d\n", exit_status);
    fflush(stdout);
    fflush(stderr);
    _exit(exit_status);
  }
  return(EXIT_SUCCESS);
}

void 
simple_SIGCHLD(
    int sig
) {
  pid_t cpid;
  int status;

  while ((cpid = waitpid(-1, &status, WNOHANG)) > 0) {
    printf("    PARENT signal handler: Found child exit %d: pid: %d exit value: %d  line: %d\n"
	   , sig, cpid, WEXITSTATUS(status), __LINE__);
  }
}

void
simple_SIGINT(
       int sig
) {
  printf("  SIGNAL HANDLER: %d  signal: %d  line: %d\n"
         , (int) getpid(), sig, __LINE__);
}
