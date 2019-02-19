/*
 * R Jesse Chaney
 */

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

sig_atomic_t signalCounter = 0;
sig_atomic_t signalRTCounter = 0;
sig_atomic_t signalExit = 0;

extern void handler_counter(int, siginfo_t *, void *);
extern void handler_counterRT(int, siginfo_t *, void *);
extern void handler_show(int, siginfo_t *, void *);

void 
handler_counterRT(
    int sig
    , siginfo_t *siginfo
    , void *ucontext
) {
//#define SLOWER
#ifdef SLOWER
  if (siginfo->si_code == SI_USER) {
    printf("\tsignal sent by user %d  pid %d\n"
  	   , siginfo->si_uid, siginfo->si_pid);
  }
#endif // SLOWER
  // We know that this is not an atomic operation;
  signalRTCounter++;
}

// This is the non-realtime signal handler.
void 
handler_counter(
    int sig
    , siginfo_t *siginfo
    , void *ucontext
) {
#ifdef SLOWER
  if (siginfo->si_code == SI_USER) {
    printf("\tsignal sent by user %d  pid %d\n"
  	   , siginfo->si_uid, siginfo->si_pid);
  }
#endif // SLOWER
  // We know that this is not an atomic operation;
  signalCounter++;
}

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
  printf("The number of Realtime signals received is: %d\n", signalRTCounter);
  printf("The number of non-Realtime signals received is: %d\n\n", signalCounter);
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

  printf("\nRealtime signal processing send some SIGRTMIN pid: %d\n", getpid());

  // This is a non-realtime signal.
  new_sig.sa_sigaction = handler_counter;
  sigfillset(&new_sig.sa_mask);
  new_sig.sa_flags = SA_SIGINFO | SA_RESTART;
  sigaction(SIGTERM, &new_sig, &old_sig);

  // This is a realtime signal.
  new_sig.sa_sigaction = handler_counterRT;
  sigfillset(&new_sig.sa_mask);
  new_sig.sa_flags = SA_SIGINFO | SA_RESTART;
  sigaction(SIGRTMIN, &new_sig, &old_sig);

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
