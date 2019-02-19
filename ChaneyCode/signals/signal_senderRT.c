/*
 * R Jesse Chaney
 */

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_SIGNALS_TO_SEND 100

int 
main(
     int argc
     , char *argv[]
     , char *envp[]
) {
  int i;
  pid_t p;
  union sigval qval;

  if (argc < 2) {
    printf("Must give pid of target process\n");
    exit(EXIT_FAILURE);
  }
  p = strtol(argv[1], NULL, 10);

  printf("\nSending %d Realtime SIGRTMIN signals to pid %d\n"
	 , NUM_SIGNALS_TO_SEND, p);
  for(i = 0; i < NUM_SIGNALS_TO_SEND ; i++) {
    qval.sival_int = i;
    // When we send realtime signals to a process, we need to use
    // a different call.
    sigqueue(p, SIGTERM, qval);
    sigqueue(p, SIGRTMIN, qval);
  }
  printf("sent %d SIGRTMIN signals to pid %d\n\n"
	 , NUM_SIGNALS_TO_SEND, p);

  sleep(1);
  kill(p, SIGINT);

  return(EXIT_SUCCESS);
}
