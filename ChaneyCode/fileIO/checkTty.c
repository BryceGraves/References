#include <fcntl.h>     // file control
#include <unistd.h>    // POSIX header
#include <stdlib.h>    // This is where exit() is declared.
#include <stdio.h>     // We want to use printf() and friends.
#include <errno.h>     // So we can use the errno value.

int 
main(
     int argc
     , char *argv[]
) 
{
  if (isatty(STDIN_FILENO)) {
    fprintf(stderr, "standard in is a tty\n");
  }
  else {
    fprintf(stderr, "standard in is not a tty\n");
  }
  if (isatty(STDOUT_FILENO)) {
    fprintf(stderr, "standard out is a tty\n");
  }
  else {
    fprintf(stderr, "standard out is not a tty\n");
  }
  if (isatty(STDERR_FILENO)) {
    fprintf(stderr, "standard error is a tty\n");
  }
  else {
    fprintf(stderr, "standard error is not a tty\n");
  }

  return(0);
}
