/*
 * R Jesse Chaney
 */

#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#ifndef MAX
# define MAX(_a,_b) ((_a) > (_b) ? (_a) : (_b))
#endif // MAX

#define WAIT_TIME 10

static int fd_count = 0;

void fifo_cleanup(void);
void handler_sigint(int);

int
main(
     int argc
     , char *argv[]
) {
  fd_set rfds;
  fd_set tmpset;
  struct timeval tv;
  int retval;
  int i;
  int read_bytes;
  int fd[FD_SETSIZE];
  int fd_max = STDIN_FILENO;
  char fifoName[20];
  char buffer[101];

  if (argc < 2) {
    fd_count = 5;
  }
  else {
    fd_count = strtol(argv[1], NULL, 10);
  }

  /* Watch stdin (fd 0) to see when it has input. */
  FD_ZERO(&rfds);
  FD_SET(STDIN_FILENO, &rfds);

  for (i = 0; i < fd_count; i++) {
    int flags;

    sprintf(fifoName, "FIFO_%04d", i);
    fd[i] = mkfifo(fifoName, 0644);
    // Open the fifo in non-blocking mode. */
    fd[i] = open(fifoName, O_RDONLY | O_TRUNC | O_NONBLOCK, 0644);
    if (fd[i] < 0) {
      perror("open failure");
      exit(1);
    }
    flags = fcntl(fd[i], F_GETFL, 0);
    fcntl(fd[i], F_SETFL, flags & (~O_NONBLOCK));

    FD_SET(fd[i], &rfds);
    fd_max = MAX(fd[i], fd_max);
  }
  fd_max++;
  atexit(fifo_cleanup);
  signal(SIGINT, handler_sigint);

  for ( ; ; ) {
    //memcpy(&tmpset, &rfds, sizeof(rfds));
    tmpset = rfds;
    // Wait seconds.
    tv.tv_sec = WAIT_TIME;
    tv.tv_usec = 0;
    retval = select(fd_max, &tmpset, NULL, NULL, &tv);

    if (retval == -1) {
      perror("Error on select()");
    }
    else if (retval) {
      printf("Data are available now.\n");
      if (FD_ISSET(STDIN_FILENO, &tmpset)) {
	// received input on stdin
	memset(buffer, 0, sizeof(buffer));
	read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
	printf("Read from stdin: <%s>\n", buffer);
	FD_CLR(STDIN_FILENO, &tmpset);
      }
      for (i = 0; i < fd_max; i++) {
	if (FD_ISSET(fd[i], &tmpset)) {
	  memset(buffer, 0, sizeof(buffer));
	  read_bytes = read(fd[i], buffer, sizeof(buffer) - 1);
	  if (read_bytes > 0) {
	    printf("Read from fd %d: <%s>\n", fd[i], buffer);
	    FD_CLR(fd[i], &tmpset);
	  }
	  else if (read_bytes == 0) {
	    printf("Zero bytes read on fd %d\n", fd[i]);
	    FD_CLR(fd[i], &rfds);
	  }
	  else {
	    printf("Error reading %d\n", fd[i]);
	    perror("error readind fd");
	  }
	}
      }
    }
    else {
      printf("No data within %d seconds timeout.\n", WAIT_TIME);
    }
  }

  return(EXIT_SUCCESS);
}

void fifo_cleanup(void)
{
  int i;
  char fifoName[20];

  printf("cleaning up all those FIFOs\n");
  for (i = 0; i < fd_count; i++) {
    sprintf(fifoName, "FIFO_%04d", i);
    unlink(fifoName);
  }  
}

void handler_sigint(
  int sig
) {
  printf("signal caught\n");
  exit(EXIT_SUCCESS);
}
