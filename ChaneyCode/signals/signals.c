/*
 * R Jesse Chaney
 */

#ifndef _GNU_SOURCE
// This is so we can get to a couple helpful GNU-isms.
# define _GNU_SOURCE
#endif // _GNU_SOURCE

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

// Allows us to print a signal. If we actually extern it, it will cause
// a warning about "redundant redeclaration", so I don't.
//extern void psignal(int sig, const char *msg);

//#define MASSIVE_DEBUG
#ifdef MASSIVE_DEBUG
# define D_FILE (void) fprintf(stderr \
		  , "** DEBUG **  File: %s  Function: %s  Line: %d \n" \
		  , __FILE__, __FUNCTION__, __LINE__);
#else /* MASSIVE DEBUG */
# define D_FILE
#endif /* MASSIVE_DEBUG */

//#define OKAY
//#define TRY_SIGKILL
//#define BETTER
#define BEST
#define SLEEP_DURATION 3

extern void handler_simple(int);
extern void handler_better(int, siginfo_t *, void *);

// This simple handler can be use with either the 
//  signal() or the basic sig_action() calls
void handler_simple(int sig)
{
  D_FILE;
  // Printing from within a signal handler is not a good idea.
  // For this demonstration, it is useful though.
  psignal(sig, "\nCaught handler_simple");

  // sys_siglist is an array of strings into which you can index
  // to obtain the name of the signal. We print the number
  // of the signal caught and the name of the signal using
  // the sys_siglist array.
  printf("Caught a handler_simple %d: %s\n"
	 , sig, sys_siglist[sig]);

  D_FILE;
  // exit(EXIT_SUCCESS);
  // abort();
}

// Used with sigaction sa_sigaction
void handler_better(
    int sig
    , siginfo_t *siginfo
    , void *ucontext
) {
  D_FILE;
  psignal(sig, "\nCaught handler_better!");
  printf("Caught handler_better %d: %s\n"
	 , sig, sys_siglist[sig]);

  // What is the signal number and the source of the signal that was
  // caught by the handler. The si_code is a value, NOT a bitmask.
  // You can check it to see the source of the signal. In addition
  // to SI_USER and SI_KERNEL, there are a couple other potential
  // sources of signals. The value of SI_KERNEL means that the signal
  // was probably generated from the user pounding on the keyboard
  // trying the get the process to stop. The value of SI_USER means
  // that signal was from a call to kill, kill(), or raise().
  printf("\tsi_signo = %d, si_code = %d "
	 , siginfo->si_signo
	 , siginfo->si_code
  );
  switch (siginfo->si_code) {
  case SI_USER:
    // Sent using kill().
    printf("SI_USER\n");
    break;
  case SI_KERNEL:
    // Could be from the controling tty.
    printf("SI_KERNEL\n");
    break;
  case SI_QUEUE:
    // These are signals sent with the sigqueue() call.
    printf("SI_QUEUE\n");
    break;
  case SI_TIMER:
    printf("SI_TIMER\n");
    break;
  case SI_MESGQ:
    printf("SI_MESGQ\n");
    break;
  case SI_ASYNCIO:
    printf("SI_ASYNCIO\n");
    break;
  case SI_SIGIO:
    printf("SI_SIGIO\n");
    break;
  case SI_TKILL:
    // Could be from raise().
    printf("SI_TKILL\n");
    break;
  default:
    printf("other ***\n");
    break;
  }
  // If the source of the signal was SI_USER, then there is some additional
  // information we can pull out of the structure. We can find out the uid
  // and the pid that sent the signal.
  if (siginfo->si_code == SI_USER) {
    printf("\tsignal sent by user %d  pid %d\n"
	   , siginfo->si_uid, siginfo->si_pid);
  }
  D_FILE;
  // exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) 
{
#if defined(BETTER) || defined(BEST)
  struct sigaction new_handler;
  struct sigaction old_handler;
  int siga_ret;
#endif // BETTER || BEST
  int i;
  int sleep_val;
#ifdef OKAY
  // This type is only available of the _GNU_SOURCE feature-test
  // macro is defined. You can create it yourself if you prefer,
  // look through the man page for signal(2).
  sighandler_t sig_ret;
  
#endif // OKAY

  D_FILE;

  printf("This is my pid: %d\n", getpid());

#ifdef OKAY
  // The value of sig_ret is the pointer to the old value of the
  // signal handler function. We could restore that if we want to
  // to do so. In our case here (just for fun), we check to see if
  // there has ever been a signal handler established for the
  // signals.
  sig_ret = signal(SIGINT, handler_simple);
  if (NULL == sig_ret) {
    printf("  ** No previous signal handler established for SIGINT\n");
  }

  sig_ret = signal(SIGTERM, handler_simple);
  if (NULL == sig_ret) {
    printf("  ** No previous signal handler established for SIGTERM\n");
  }

# ifdef TRY_SIGKILL
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
    printf("You can't change the SIGKILL handler\n");
  }
# endif // TRY_SIGKILL
#endif // OKAY

#ifdef BETTER
  // What makes this better?  The call signal() is much older
  // and less portable than the calls to sigaction(). In addition,
  // you can do more with sigaction() than with the simple call
  // to signal().
  // In this case, we are not taking advantage of all of the
  // additional capabilities the sigaction() call offers.
  //
  // Set the handler function to be the existing simple signal
  // handler function. You should NOT use both the sa_handler
  // and the sa_sigaction field of the sigaction structure.
  new_handler.sa_handler = handler_simple;

  // We'll talk about sa_flags in a moment.
  // Restart the system calls, if possible.
  new_handler.sa_flags = SA_RESTART;

  // Block every signal during the handler. We don't want a signal to
  // interrupt this signal handler.
  sigfillset(&new_handler.sa_mask);

  // If we wanted to allow other signals to interrupt this signal handler
  // we would do the following.
  //sigemptyset(&new_handler.sa_mask);

  // When we provide a non-NULL argument as the fourth argument to
  // sigaction(), the old values of the handler, flags, ... are filled
  // into the structure. This allows us to easily restore the old
  // values if we want to. This is useful when you want to change
  // signal handler only for a short period of time or in a piece
  // code and restore it.
  siga_ret = sigaction(SIGINT, &new_handler, &old_handler);
  siga_ret = sigaction(SIGTERM, &new_handler, &old_handler);
#endif // BETTER

#ifdef BEST
  // Now we are going to use some of the additional features of the
  // sigaction() call. That makes this BEST.
  new_handler.sa_sigaction = handler_better;

  sigfillset(&new_handler.sa_mask);
  // We set the SA_SIGINFO value as the the flags for the handler. When
  // do this, we need to use a different handler function, that takes
  // more arguments. Notice we also used sa_sigaction instead of
  // sa_sighandler to contain the name of the handler function.
  new_handler.sa_flags = SA_SIGINFO;

  // We also want to restart the system calls, if possible.
  new_handler.sa_flags |= SA_RESTART;

  siga_ret = sigaction(SIGINT, &new_handler, &old_handler);
  siga_ret = sigaction(SIGTERM, &new_handler, &old_handler);

# ifdef TRY_SIGKILL
  siga_ret = sigaction(SIGKILL, &new_handler, &old_handler);
  if (-1 == siga_ret) {
    perror(" ** sigaction is unhappy we tried to have a handler for SIGKILL");
  }
# endif // TRY_SIGKILL
#endif // BEST

  for(i = 0; i < 10 ; i++) {
    printf("loop i = %d\n", i);
    sleep_val = sleep(SLEEP_DURATION);
    // The return value from a call to sleep() tells us how many seconds
    // the process did NOT sleep from the requested value. If a signal
    // arrives, it interrupts the call to sleep.
    printf("  slept for %d seconds of %d\n"
	   , SLEEP_DURATION - sleep_val, SLEEP_DURATION);

#define RAISE
#define KILL
//#define SIGQUEUE
#ifdef KILL
    kill(getpid(), SIGINT);
    kill(getpid(), SIGTERM);
#endif // KILL
#ifdef RAISE
    // Interestingly, at least on our server, the si_code value when using raise()
    // is different than using kill() to send the signal. That is NOT what the
    // man page on claims.
    raise(SIGINT);
    raise(SIGTERM);
#endif // RAISE
#ifdef SIGQUEUE
    // This is another way to send signals to a process. It gives you
    // the opportunity to add a bit more data to the signal.
    {
      union sigval qval;
      
      qval.sival_int = i;
      sigqueue(getpid(), SIGINT, qval);
      sigqueue(getpid(), SIGTERM, qval);
    }
#endif // SIGQUEUE
  }

  // Reset the signal handler to the default disposition.
  //signal(SIGINT, SIG_DFL);
  //signal(SIGTERM, SIG_DFL); // SIG_IGN

  return(EXIT_SUCCESS);
}
