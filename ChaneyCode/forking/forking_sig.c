/*
 * R Jesse Chaney
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

volatile sig_atomic_t child_value = 1;

extern void child_exited(int, siginfo_t *, void *);

int 
main(
     int argc
     , char *argv[]
     , char *envp[]
) {
  pid_t cpid;
  pid_t mypid;
  int i;

  for (i = 0; i < 3; i++) {
    switch((cpid = fork())) {
    // Both parent and child processes continue executing here
    case -1:
      // We had a failure to fork.
      perror("failed to fork new process");
      exit(EXIT_FAILURE);
      break;
    case 0:
      mypid = getpid();
      fprintf(stderr, "    CHILD: pid = %d child value: %d line: %d\n"
	      , mypid, child_value, __LINE__);
      // Child specific code goes here
      // We could exec a new program here.
      break;
    default:
      // Parent specific code goes here.
      mypid = getpid();
      fprintf(stderr, "PARENT: pid = %d  child pid = %d\n"
	      , mypid, cpid);
      child_value++; // This is not an atomic operation.
      break;
    }
    if (cpid == 0) {
      // This is a child
      // We are going to break out of the loop.
      break;
    }
  }

  if (0 == cpid) {
    sleep(5 * child_value);
    exit(child_value);
  } else {
    struct sigaction new_handler;
    struct sigaction old_handler;
    child_value--;

    fprintf(stderr, "PARENT: pid = %d using signals to wait \n", mypid);

    // Establish a signal handler for the SIGCHLD signal. The SIGCHLD
    // signal is sent to the parent process any time one of its child
    // processes terminates.
    //(void) signal(SIGCHLD, child_exited);
    // This is the better way to setup a sigal handler for a process.
    new_handler.sa_sigaction = child_exited;
    sigemptyset(&new_handler.sa_mask);
    new_handler.sa_flags = SA_SIGINFO;
    sigaction(SIGCHLD, &new_handler, &old_handler);

    for ( ; ; ) {
      fprintf(stderr, "  PARENT: pid = %d   child processes running: %d \n"
	      , mypid, child_value);
      if (child_value < 1) {
	break;
      }
      sleep(1);
    }
    fprintf(stderr, "PARENT: pid = %d all children exited\n"
	    , mypid);
  }

  return(EXIT_SUCCESS);
}

void 
child_exited(
    int sig
    , siginfo_t *siginfo
    , void *ucontext
) {
  int status;
  pid_t cpid;

  // Use the waitpid() call, but wait for any child to exit.
  // Don't block when calling waitpid(), if there are no
  // child processes.
  // We need to do a loop in here in case more than one child
  // exited and we only received a single signal. Remember that
  // signals are not queued.
  while ((cpid = waitpid(-1, &status, WNOHANG)) > 0) {
    printf("    PARENT signal handler: Found child exit %d: pid: %d exit value: %d\n"
	   , sig, cpid, WEXITSTATUS(status));
    child_value--; // this is not really an atomic operation
  }
}
