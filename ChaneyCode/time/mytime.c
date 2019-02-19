 /*
 * R Jesse Chaney
 */

#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

//#define DEBUG
#ifdef DEBUG
// This can be a very handy macro to use for debugging.
# define D_FILE fprintf(stderr, "** File: %s  Function: %s  Line: %d **\n" \
			, __FILE__, __FUNCTION__, __LINE__);
#else
# define D_FILE
#endif /* DEBUG */

int
main(
     int argc
     , char *argv[]
     , char *envp[]
) {
  time_t t;
  struct tm *gmt;
  struct tm *local;
  char time_string[100];

  D_FILE;
  // Fetch the epoch time, seconds since midnight Jan 1, 1970.
  t = time(NULL);
  fprintf(stdout
	  , "Epoch time = %ld (seconds since midnight Jan 1 1970, GMT)\n\n"
	  , (long) t);
  
  // Be aware of this comment in the man page:
  //
  // NOTES
  //  The  four  functions asctime(), ctime(), gmtime() and localtime() return a
  //  pointer to static data and hence are not  thread-safe. Thread-safe
  //  versions  asctime_r(),  ctime_r(), gmtime_r() and localtime_r() are specified
  //  by SUSv2, and available since libc 5.2.5.
  D_FILE;
  // Print out the brokendown time, for GMT.
  gmt = gmtime(&t);
  // Print the UTC broken-down time.
  fprintf(stdout, "GMT Time\n");
  fprintf(stdout, "  Seconds:\t%02d\n", gmt->tm_sec);
  fprintf(stdout, "  Minutes:\t%02d\n", gmt->tm_min);
  fprintf(stdout, "  Hours:\t%02d\n", gmt->tm_hour);
  fprintf(stdout, "  Day of month:\t%2d\n", gmt->tm_mday);
  fprintf(stdout, "  Month:\t%2d (January is 0)\n", gmt->tm_mon);
  fprintf(stdout, "  Year:\t\t%d\n", gmt->tm_year + 1900);
  fprintf(stdout, "  Day of week:\t%2d (Sunday is 0)\n", gmt->tm_wday);
  fprintf(stdout, "  Day of year:\t%3d\n", gmt->tm_yday);
  fprintf(stdout, "  Is DST?:\t%d\n\n", gmt->tm_isdst);
  D_FILE;

  // Print out the brokendown time, for local time.
  local = localtime(&t);
  // Print the local broken-down time.
  fprintf(stdout, "Local time\n");
  fprintf(stdout, "  Seconds:\t%02d\n", local->tm_sec);
  fprintf(stdout, "  Minutes:\t%02d\n", local->tm_min);
  fprintf(stdout, "  Hours:\t%02d\n", local->tm_hour);
  fprintf(stdout, "  Day of month:\t%2d\n", local->tm_mday);
  fprintf(stdout, "  Month:\t%2d (January is 0)\n", local->tm_mon);
  fprintf(stdout, "  Year:\t\t%d\n", local->tm_year + 1900);
  fprintf(stdout, "  Day of week:\t%2d (Sunday is 0)\n", local->tm_wday);
  fprintf(stdout, "  Day of year:\t%3d\n", local->tm_yday);
  fprintf(stdout, "  Is DST?:\t%d\n\n", local->tm_isdst);
  D_FILE;

  fprintf(stdout, "ctime():\t%s\n", ctime(&t));
  fprintf(stdout, "asctime():\t%s\n", asctime(local));

  strftime(time_string, 100, "%c", local);
  fprintf(stdout, "strftime local:\t%s \n", time_string);

  strftime(time_string, 100, "%D %T %a %b %d %Y", local);
  fprintf(stdout, "strftime local:\t%s \n", time_string);

  D_FILE;
  return(EXIT_SUCCESS);
}


