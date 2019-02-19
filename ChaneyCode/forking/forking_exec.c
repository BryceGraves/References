/*
 * R Jesse Chaney
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int 
main(
     int argc
     , char *argv[]
     , char *envp[]
) 
{
  pid_t cpid = -1;
  pid_t mypid = -1;
  pid_t parentpid = -1;
  int i;
  int status = 0;

  // Set up a simple signal handler to accept and ignore 
  // the SIGCHLD signal from child processes when they
  // terminate.
  (void) signal(SIGCHLD, SIG_IGN);
  for (i = 0; i < 2; i++) {
    switch((cpid = fork())) {
      // Both parent and child processes continue executing here.
    case -1:
      // We had a failure to fork.
      perror("failed to fork new process");
      exit(EXIT_FAILURE);
      break;
    case 0:
      mypid = getpid();
      parentpid = getppid();
      fprintf(stderr, "    CHILD: pid = %d  parent pid: %d  loop value: %d line: %d\n"
	      , mypid, parentpid, i, __LINE__);
      // Child specific code goes here.
      // We can exec a new program at this spot.
      status = execlp("ls", "ls", "-l", "-F", "-Bh", "-t", (char *) NULL);

      // The child process should never execute the lines of code below here,
      // unless there has been an error on the execlp() call.
      perror("execlp failed");
      fprintf(stderr, "    CHILD: pid = %d  line: %d  %d\n"
	      , mypid, __LINE__, status);
      fflush(stderr);
      _exit(EXIT_FAILURE);
      break;
    default:
      // Parent specific code goes here.
      mypid = getpid();
      fprintf(stderr, "PARENT: pid = %d %d  child pid = %d  loop = %d\n"
	      , mypid, __LINE__, cpid, i);
      break;
    }
    if (cpid == 0) {
      // This is the child process, but only if the execl() failed.
      break;
    }
  }
  if (0 == cpid) {
    // Empty because the child processes should never get here.
  } else {
    fprintf(stderr, "  >>>PARENT: pid = %d line: %d\n", mypid, __LINE__);

    for ( ; ; ) {
      // Make a call to the wait() function. The call to wait() will
      // block until a child process terminates.
      cpid = wait(&status);
      if (cpid > 0) {
	int exit_status;

	exit_status = WEXITSTATUS(status);
	// The wait() call returned a child process id.
	fprintf(stderr, "  >>>PARENT: pid = %d  child exited: %d %d  %s\n"
		, mypid, cpid, exit_status
		, (WIFEXITED(status) ? "child exited normally" : "child had abnormal exit"));
	if (WIFSIGNALED(status)) {
	  fprintf(stderr, "     child exited from signal %d\n", WTERMSIG(status));
	}
      } else {
	// The call to wait() returned a negative value, indicating
	// that there are no child processes to wait on.
	// We could have used a count down on the child_value
	// varialbe, but this actually works better for this case.
	fprintf(stderr, "  >>>PARENT: pid = %d  all children exited\n"
		, mypid);
	break;
      }
    }
  }

  return(EXIT_SUCCESS);
}
