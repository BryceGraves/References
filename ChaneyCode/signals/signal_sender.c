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
  int kill_result;

  if (argc < 2) {
    printf("Must give pid of target process\n");
    exit(EXIT_FAILURE);
  }
  // Convert the command line argument from a string to a (base 10)
  // number. This is preferred over atoi().
  p = strtol(argv[1], NULL, 10);

  printf("\nSending %d SIGTERM signals to pid: %d\n"
	 , NUM_SIGNALS_TO_SEND, p);
  for(i = 0; i < NUM_SIGNALS_TO_SEND ; i++) {
    // Send a SIGTERM signal to the target process. The target process
    // should count the number of SIGTERM signals is receives.
    kill_result = kill(p, SIGTERM);
    if (-1 == kill_result) {
      perror("Unable to send signal to process");
      fprintf(stderr, "Unable to send signal to process %d\n", p);
      exit(EXIT_FAILURE);
    }
  }
  printf("Sent %d SIGTERM signals to pid %d\n\n"
	 , NUM_SIGNALS_TO_SEND, p);
  // Give the target process a chance to catch and process ALL the
  // sent signals.
  sleep(1);

  printf("Sending SIGINT signals to pid %d so it will terminate.\n", p);
  // Send one more signal that causes the target process to print
  // the number of signals processed.
  kill(p, SIGINT);

  return(EXIT_SUCCESS);
}
