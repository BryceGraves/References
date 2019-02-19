/*
 * R Jesse Chaney
 */

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

// These are a couple global variable that are used to keep track
// of how many SIGTERM signals have been received and if the 
// SIGINT signal has been received, indicating that the process
// should print the count of SIGTERM signals and exit.
static sig_atomic_t signalCounter = 0;
static sig_atomic_t signalExit = 0;

extern void handler_counter(int, siginfo_t *, void *);
extern void handler_show(int, siginfo_t *, void *);

// This demonstrates that signals are NOT queued. Signals can
// be lost.
void 
handler_counter(
    int sig
    , siginfo_t *siginfo
    , void *ucontext
) {
  //#define SLOW
#ifdef SLOW
  // This takes a little bit of time to process.
  if (siginfo->si_code == SI_USER) {
    printf("\tsignal sent by user %d  pid %d\n"
  	   , siginfo->si_uid, siginfo->si_pid);
  }
#endif // SLOW
  //#define SLOWER
#ifdef SLOWER
  usleep(10);
#endif // SLOWER
  // We know that this is NOT really an atomic operation;
  signalCounter++;
}

// When this signal is received, the process will print the number 
// of SIGTERM signals that have been counted and then allow the
// the process to gracefully exit.
void 
handler_show(
    int sig
    , siginfo_t *siginfo
    , void *ucontext
) {
  if (siginfo->si_code == SI_USER) {
    printf("\tshow signal sent by user %d  pid %d\n"
	   , siginfo->si_uid, siginfo->si_pid);
  }
  printf("The number of SIGTERM signals counted is: %d\n", signalCounter);
  // This actuallt is an atomic operation.
  signalExit = 1;
}

int 
main(
     int argc
     , char *argv[]
     , char *envp[]
) {
  struct sigaction new_sig;
  struct sigaction old_sig;

  printf("This is my pid: %d\n", getpid());

  // Setup 2 signal handlers.
  // The SIGTERM signal handler will keep track of how many SIGTERM
  // SIGTERM signals have been received.
  new_sig.sa_sigaction = handler_counter;
  sigfillset(&new_sig.sa_mask);
  new_sig.sa_flags = SA_SIGINFO | SA_RESTART;
  sigaction(SIGTERM, &new_sig, &old_sig);

  // The SIGINT signal will allow the process to gracefully exit.
  new_sig.sa_sigaction = handler_show;
  sigfillset(&new_sig.sa_mask);
  new_sig.sa_flags = SA_SIGINFO | SA_RESTART;
  sigaction(SIGINT, &new_sig, &old_sig);

  // The loop will exit when the SIGINT signal has been received.
  for( ; !signalExit ; ) {
    // A call to pause will cause the process to block until 
    // a signal (any signal) has been received.
    pause();
  }

  return(EXIT_SUCCESS);
}
