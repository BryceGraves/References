#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

// The number of times the handler will run
static volatile sig_atomic_t breakflag = 5;
#define DELAY 2

void alarmHandler(int sig);

int 
main(
     int argc
     , char *argv[]
) {

  // establish a handler for SIGALRM.
  signal(SIGALRM, alarmHandler);

  alarm(DELAY);
  while(breakflag) {
    // don't use sleep() or usleep() in here. the interaction
    // with alarm() could be bad.
    pause();
  }

  printf("All done\n");
  return 0;
}

void alarmHandler(int sig) 
{
  time_t t;

  breakflag--;
  t = time(NULL);
  printf("The time is: %s", ctime(&t));
  printf("  %d calls remaining\n", breakflag);

  // Set the alarm handler again.
  if (breakflag) {
    alarm(DELAY);
  }
}
