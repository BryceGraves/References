 /*
 * R Jesse Chaney
 */

#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_LEN 100


int
main(
     int argc
     , char *argv[]
     , char *envp[]
) {
  struct timeval begin_tv;
  struct timeval end_tv;
  struct tm* localtm;
  char time_string[BUF_LEN];
  unsigned long milliseconds;
  int retval;
  int i;

  // Fetch the time of day, minutes and MICROseconds.
  retval = gettimeofday(&begin_tv, NULL);
  if (retval < 0) {
    perror("count not gettimeofday");
    exit(EXIT_FAILURE);
  }

  // Take the tv_sec portion and convert it to a tm struct,
  // which contains the broken-down time.
  localtm = localtime(&begin_tv.tv_sec);

  // Format the date and time, down to the single second.
  //   Y -> year as 4 digits
  //   m -> month as a 2 digit number (01-12)
  //   d -> day of month as 2 digit (01-31)
  //   H -> 24 hour clock
  //   M -> minute as 00-59
  //   S -> seconds 00-60 (notice leap second)
  // See table 10-1 or te man page for more info.
  // The string created will be NULL terminated.
  strftime(time_string, BUF_LEN, "%Y-%m-%d %H:%M:%S", localtm);

  // Calculate MILLIseconds from MICROseconds.
  milliseconds = begin_tv.tv_usec / 1000;

  // Print the formatted time, in seconds, followed by a decimal 
  // point and the MILLIseconds.
  printf("\n\tBegin time: %s.%06ld\n\n", time_string, milliseconds);

  for(i = 0; i < 5; i++) {
    // Think of this as some complex code doing a long
    // operation that you want to optimize.
    sleep(1);
  }

  retval = gettimeofday(&end_tv, NULL);
  localtm = localtime(&end_tv.tv_sec);
  strftime(time_string, BUF_LEN, "%Y-%m-%d %H:%M:%S", localtm);
  milliseconds = end_tv.tv_usec / 1000;
  printf("\n\tEnd time:   %s.%06ld\n\n", time_string, milliseconds);

  return(EXIT_SUCCESS);
}
