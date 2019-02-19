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
#include <sys/select.h>
#include <sys/epoll.h>
#include <errno.h>

#define MAXEVENTS 64
#define WAIT_TIME 1000

static int fd_count = 0;

void fifo_cleanup(void);
void handler_sigint(int);

int
main(
     int argc
     , char *argv[]
) {
  int retval;
  int i;
  int read_bytes;
  int num_events;
  char fifoName[20];
  char buffer[101];
  int epfd;
  int fd;
  struct epoll_event *events;

  if (argc < 2) {
    fd_count = 5;
  }
  else {
    fd_count = strtol(argv[1], NULL, 10);
  }

  epfd = epoll_create(1);
  if (epfd < 0) {
    perror("  ** error on epoll_create");
  }

  for (i = 0; i < fd_count; i++) {
    struct epoll_event ev;

    sprintf(fifoName, "FIFO_%04d", i);
    fd = mkfifo(fifoName, 0644);
    // Open the fifo in non-blocking mode. */
    fd = open(fifoName, O_RDONLY | O_TRUNC | O_NONBLOCK, 0644);
    if (fd < 0) {
      perror("open failure");
      exit(1);
    }

    ev.events = 
      EPOLLIN 
      //EPOLLET
      | EPOLLPRI | EPOLLERR | EPOLLHUP;
    ev.data.fd = fd;
    retval = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
    if (retval < 0) {
      perror("  ** error on epoll_ctl");
      exit(1);
    }
  }
  atexit(fifo_cleanup);
  signal(SIGINT, handler_sigint);

  events = calloc(MAXEVENTS, sizeof(struct epoll_event));
  for ( ; ; ) {
    num_events = epoll_wait(epfd, events, MAXEVENTS, -1);
    printf("epoll() events %d\n", num_events);
    if (num_events < 0) {
      perror("  ** error on epoll_wait");
      exit(1);
    }
    if (num_events == 0) {
      continue;
    }
    for (i = 0; i < num_events; i++) {
      if ((events[i].events & EPOLLERR) ||
	  (events[i].events & EPOLLHUP) ||
	  (!(events[i].events & EPOLLIN))) {
	perror("  ** error on epoll");
	fprintf (stderr, "  ** epoll error >%d<  %d\n", i, errno);
	printf("%s  %s\n"
	       , ((events[i].events & EPOLLERR) ? "EPOLLERR" : "")
	       , ((events[i].events & EPOLLHUP) ? "EPOLLHUP" : "")
	       );
	close(events[i].data.fd);	
	continue;
      }
      else {
	for ( ; ; ) {
	  memset(buffer, 0, sizeof(buffer));
	  read_bytes = read(events[i].data.fd, buffer, sizeof(buffer) - 1);
	  if (0 == read_bytes) {
	    break;
	  }
	  if (0 > read_bytes) {
	    perror("  ** error on read bytes");
	    break;
	  }
	  printf("  Read from FIFO %d %d >%s<\n", i, read_bytes, buffer);
	}
      }
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
