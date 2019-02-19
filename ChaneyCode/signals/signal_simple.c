/*
 * R Jesse Chaney
 */

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

extern void handler_simple(int);

// This simple handler can be use with either the 
//  signal() or the basic sig_action() calls
void handler_simple(
    int sig
) {
  // Printing from within a signal handler is not a good idea.
  // For this demonstration, it is useful though.
  psignal(sig, "\n handler_simple");

  // sys_siglist is an array of strings into which you can index
  // to obtain the name of the signal. We print the number
  // of the signal caught and the name of the signal using
  // the sys_siglist array.
  printf("Caught a %d: %s (in function %s)\n"
	 , sig, sys_siglist[sig], __func__);
}

int main(
     int argc
     , char *argv[]
     , char *envp[]
) {
  void *sig_ret;

  printf("This is my pid: %d\n", getpid());

  sig_ret = signal(SIGINT, handler_simple);
  if (sig_ret == SIG_ERR) {
    perror("An error trying to set signal on SIGINT");
  }
  sig_ret = signal(SIGQUIT, handler_simple);
  if (sig_ret == SIG_ERR) {
    perror("An error trying to set signal on SIGQUIT");
  }

  // Not all signals can be caught. Even though we
  // setup a signal handler for SIGKILL, the signal
  // will terminate the process. Our signal handler
  // will never be called. In fact, it will generate an
  // error when we even try to establish a handler for this
  // signal. All the better reason to check the return
  // value from the call to signal.
  sig_ret = signal(SIGKILL, handler_simple);
  if (sig_ret == SIG_ERR) {
    perror("An error trying to set signal on SIGKILL");
    printf("You can't change the SIGKILL handler\n\n\n");
  }

  for ( ; ; ) {
    sleep(1);
  }

  return(EXIT_SUCCESS);
}
