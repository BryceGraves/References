#include <stdio.h>
#include <time.h>
#include <locale.h>
#include <stdlib.h>

#define BUF_SIZE 200

// TZ=":Pacific/Auckland" ./show_time
// TZ=":Navajo" ./show_time

int
main(
     int argc
     , char *argv[])
{
  time_t t;
  struct tm *loc;
  char buf[BUF_SIZE];
 
  if (setlocale(LC_ALL, "") == NULL) {
    perror("cannot set locale");
    exit(1);
  }
  
  t = time(NULL);
  printf("ctime() of time() value is: %s", ctime(&t));
  loc = localtime(&t);

  if (loc == NULL) {
    perror("cannot get the localtime");
    exit(2);
  }
  printf("asctime() of local time is: %s", asctime(loc));
  if (strftime(buf, BUF_SIZE, "%A, %d %B %Y, %H:%M:%S %Z", loc) == 0) {
    perror("cannot display time");
    exit(3);
  }

  printf("strftime() of local time is: %s\n", buf);

  exit(EXIT_SUCCESS);
}
