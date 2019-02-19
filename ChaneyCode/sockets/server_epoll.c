 /*
 * R Jesse Chaney
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <sys/epoll.h>

// This will be the backlog argument.  The maximum length
// to which the queue of pending connections for sockfd
// may grow.
#define LISTENQ 1024      // 2nd argument to listen()

#define MAXLINE 4096      // max text line length

//#define MAX_BUF     1000
#define MAX_EVENTS     5

#define NOT_IN_USE -1

#define QUOTE(_NAME_) #_NAME_
#define STR(_NAME_) QUOTE(_NAME_)

// Define some port number that can be used for client-servers
#define SERV_PORT 10001    // TCP and UDP client-servers
#define SERV_PORT_STR STR(SERV_PORT)

// FS_SETSIZE maximum number of file descriptors that a
// fd_set object can hold information about.
//   from http://www.delorie.com/gnu/docs/glibc/libc_248.html
#define MAX_CLIENTS FD_SETSIZE
//#define MAX_CLIENTS 2

int
main(
     int argc
     , char *argv[]
) {
  int i;
  int maxfd;
  int listenfd;
  int connfd;
  int sockfd;
  int nready;
  int client[MAX_CLIENTS]; // max size of a fd set for select
  int nclients;
  int epfd;
  fd_set rset;
  fd_set allset;
  char buf[MAXLINE];
  ssize_t nbytes;
  socklen_t clilen;
  struct sockaddr_in cliaddr;
  struct sockaddr_in servaddr;
  //struct timeval timeout;
  struct epoll_event ev;
  struct epoll_event evlist[MAX_EVENTS];

  // Since Linux 2.6.8, the size argument is ignored, 
  //   but must be greater than zero
  epfd = epoll_create(1);

  listenfd = socket(AF_INET, SOCK_STREAM, 0);

  // Make sure everything is nice and clean.
  memset(&servaddr, 0, sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(SERV_PORT);

  if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0) {
    perror("cannot bind address");
    exit(EXIT_FAILURE);
  }

  if (listen(listenfd, LISTENQ) != 0) {
    perror("cannot listen on port address");
    exit(EXIT_FAILURE);
  }
  printf("server listening on %s\n", SERV_PORT_STR);  

  // For now, this is out highest numbered file desc that
  // we will check.
  maxfd = listenfd; // initialize
  nclients = 0;

  // Go through the entire array and initialize the entries
  // to a "not used" state.  We know that all file descriptors
  // are a non-negative value, so using -1 indicates it is 
  // unused.
  for (i = 0 ; i < MAX_CLIENTS ; i++) {
    client[i] = NOT_IN_USE; // A value of -1 indicates available entry
  }

  // Clean out the set of all file descriptors.
  FD_ZERO(&allset);

  // Add the listen file desc into the list of all descriptors.
  FD_SET(listenfd, &allset);
  //  timeout.tv_sec = 5;  // 5 seconds
  for ( ; ; ) {
    rset = allset;

    printf("sever: calling select with %d clients\n>> ", nclients);
    fflush(stdout);

    // In this case, we are only looking at the readfds.
    // We are looking at either the writefds or the exceptfds.
    // We are not specifying a time out value.
    //
    // Select will return the total number of fds that have
    // become ready for i/o.
    //
    // A file descriptor is considered ready if it is possible
    // to perform the corresponding I/O operation without blocking.
    //
    //              VVVVVVVVV the highest numbered file desc + 1.
    nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
    //timeout.tv_sec = 5;  // 5 seconds
    //nready = select(maxfd + 1, &rset, NULL, NULL, &timeout);
    printf("\n");

    //    if (nready == 0) {
    //      printf("  server: timeout occured\n");
    //      continue;
    //    }

    printf("  server: select found %d ready fds\n", nready);

    if (nready == -1) {
      perror("select returned failure");
      exit(EXIT_FAILURE);
    }

    // Check the listenfd first.  If it is ready, then it is
    // a new client connection request.
    if (FD_ISSET(listenfd, &rset)) {
      // A new client connection...  Woooo  Whoooo!!!
      nclients ++;
      printf("  server: a new client connection request\n");
      clilen = sizeof(cliaddr);

      // Get the new connection file desc.
      connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);

      // Find a place to save the new file descriptor in
      // our set of file descriptors.
      for (i = 0 ; i < MAX_CLIENTS ; i++) {
	if (client[i] < 0) {
	  client[i] = connfd; // save descriptor
	  break;              // not need to keep looking
	}
      }

      if (i == MAX_CLIENTS) {
	printf("!!! TOO MANY CLIENTS  !!!!\n");
	// This might not be the best way to handle this condition,
	// but it is a good demonstration.
	//close(connfd);
	exit(EXIT_FAILURE);
      }

      FD_SET(connfd, &allset); // add new descriptor to  the all set

      // Make sure that we keep track of the highest numbered
      // file descriptor.  We need it for the select() call.
      if (connfd > maxfd) {
	maxfd = connfd;
      }

      // Decrement the number of processed file desc left
      // to handle.
      if (--nready <= 0) {
	continue; // no more readable descriptors
      }
    }

    // Check file desc to see if there is ready data.
    for (i = 0 ; i <= MAX_CLIENTS ; i++) {
      sockfd = client[i];
      if (sockfd < 0) {
	// not a spot in use, keep looking
	continue;
      }

      // This checks to see if this file desc is one of the
      // ready ones.
      if (FD_ISSET(sockfd, &rset)) {
	printf("  server: received communication from client\n");

	// Make sure this is nice and clean.
	//	memset(buf, 0, sizeof(buf));
	// Received data on this file desc, read and echo it.
	if ((nbytes = read(sockfd, buf, sizeof(buf))) == 0) {
	  // The connection was closed by the client, because
	  // we received a zero bytes on the the read.
	  // Close the socket fd, remove it from the list to check,
	  // and set its spot to available.
	  close(sockfd);
	  FD_CLR(sockfd, &allset);
	  client[i] = NOT_IN_USE;

	  printf("    server: client closed connection\n");
	  nclients --;
	}
	else {
	  printf("    server: echo data back to client <%s>\n", buf);
	  // Echo back to the client the received data.  Of course
	  // we SHOULD check the return value from the write().
	  write(sockfd, buf, nbytes);
	}

	if (--nready <= 0) {
	  // All the changed file desc have been checked.
	  // can exit the for loop.
	  break;
	}
      }
    }
  }
}
