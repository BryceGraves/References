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

// This will be the backlog argument. The maximum length
// to which the queue of pending connections for sockfd
// may grow.
#define LISTENQ 1024      // 2nd argument to listen()

#define MAXLINE 4096      // max text line length

#define NOT_IN_USE -1

#define QUOTE(_NAME_) #_NAME_
#define STR(_NAME_) QUOTE(_NAME_)

// Define some port number that can be used for client-servers
#define SERV_PORT 10001    // default port
#define SERV_PORT_STR STR(SERV_PORT)  // Also make is a string.

// FS_SETSIZE maximum number of file descriptors that a
// fd_set object can hold information about.
//   from http://www.delorie.com/gnu/docs/glibc/libc_248.html
// This probably going to be 1k.
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
  int client[MAX_CLIENTS];  // file desc
  int nclients;
  // An fd_set is a set of file descriptors (sockets in this 
  //   case) to "monitor" for activity. In general, the
  //   file descriptors could be from files, directory events, 
  //   pipes, fifos, message queues, ... The universality
  //   of UNIX file i/o really shows up here.
  fd_set rset;
  fd_set allset;
  char buf[MAXLINE];
  ssize_t nbytes;
  socklen_t clilen;
  struct sockaddr_in cliaddr;
  struct sockaddr_in servaddr;
  //struct timeval timeout; // If we want a timeout on the select()

  // Create the initial socket. This is the socket over
  //   which new client connections will arrive.
  listenfd = socket(AF_INET, SOCK_STREAM, 0);

  // Make sure everything is nice and clean.
  memset(&servaddr, 0, sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(SERV_PORT);

  // Bind it to a port.
  if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0) {
    perror("cannot bind address");
    exit(EXIT_FAILURE);
  }

  // Make it passive.
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
  // to a "not used" state. We know that all file descriptors
  // are a non-negative value, so using -1 indicates it is 
  // unused.
  for (i = 0 ; i < MAX_CLIENTS ; i++) {
    client[i] = NOT_IN_USE; // A value of -1 indicates available entry
  }

  // Four macros are provided to manipulate the sets.
  //   FD_ZERO() clears a set. 
  //   FD_SET() and FD_CLR() respectively add and remove a 
  //     given file descriptor from a set. 
  //   FD_ISSET() tests to see if a file descriptor is part 
  //     of the set; this is useful after select() returns. 
  // Clean out the set of all file descriptors.
  FD_ZERO(&allset);

  // Add the listen file desc into the list of all descriptors.
  FD_SET(listenfd, &allset);
  //  timeout.tv_sec = 5;  // 5 seconds
  for ( ; ; ) {
    rset = allset;

    fprintf(stdout, "sever: calling select with %d clients\n>> ", nclients);
    fflush(stdout);

    // select() allows a program to monitor multiple file 
    //   descriptors, waiting until one or more of the 
    //   file descriptors become "ready" for some class 
    //   of I/O operation.
    // int select(int nfds
    //            , fd_set *readfds
    //            , fd_set *writefds
    //            , fd_set *exceptfds
    //            , struct timeval *timeout);

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

    // We could also have a timeout on the select() call.
    //timeout.tv_sec = 5;  // 5 seconds
    //nready = select(maxfd + 1, &rset, NULL, NULL, &timeout);

    printf("\n");

    //    if (nready == 0) {
    //      printf("  server: timeout occured without new i/o\n");
    //      continue;
    //    }

    printf("  server: select found %d ready fds\n", nready);

    if (nready == -1) {
      perror("select returned failure");
      exit(EXIT_FAILURE);
    }

    // Check the listenfd first. If it is ready, then it is
    // a new client connection request.
    if (FD_ISSET(listenfd, &rset)) {
      // A new client connection... Woooo  Whoooo!!!
      nclients++;
      clilen = sizeof(cliaddr);

      // Get the new connection file descriptor.
      connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);
      printf("  server: a new client connection request  %d\n", connfd);
      // Find a place to save the new file descriptor in
      // our set of file descriptors.
      for (i = 0 ; i < MAX_CLIENTS ; i++) {
	if (client[i] == NOT_IN_USE) {
	  client[i] = connfd; // save descriptor
	  break;              // no need to keep looking
	}
      }

      if (i == MAX_CLIENTS) {
	printf("!!! TOO MANY CLIENTS  !!!!\n");
	// This might not be the best way to handle this condition,
	// but it is a good demonstration.
	//close(connfd);
	exit(EXIT_FAILURE);
      }

      // This is the fd we just got from accept().
      FD_SET(connfd, &allset); // add new descriptor to the all set

      // Make sure that we keep track of the highest numbered
      // file descriptor. We need it for the select() call.
      if (connfd > maxfd) {
	maxfd = connfd;
      }

      // Decrement the number of processed file desc left
      // to handle.
      if (--nready <= 0) {
	continue; // no more readable descriptors
      }
    }

    // Check the other file descriptors to see if there is ready data.
    for (i = 0 ; i <= MAX_CLIENTS ; i++) {
      sockfd = client[i];
      if (sockfd < 0) {
	// not a spot in use, keep looking
	continue;
      }

      // This checks to see if this file desc is one of the
      // ready ones.
      if (FD_ISSET(sockfd, &rset)) {
	printf("  server: received communication from client %d\n", sockfd);

	memset(buf, 0, sizeof(buf));
	// Make sure this is nice and clean.
	//	memset(buf, 0, sizeof(buf));
	// Received data on this file desc, read and echo it.
	if ((nbytes = read(sockfd, buf, sizeof(buf))) == 0) {
	  // The connection was closed by the client, because
	  // we received a zero bytes on the the read.
	  // Close the socket fd, remove it from the list to check,
	  // and set its spot to available.
	  close(sockfd);
	  FD_CLR(sockfd, &allset); // Remove this fd from the set.
	  client[i] = NOT_IN_USE;  // Allow that spot to be reused.

	  printf("    server: client closed connection\n");
	  nclients --;
	}
	else {
	  printf("    server: echo data back to client <%s>\n", buf);
	  // Echo back to the client the received data. Of course
	  // we SHOULD check the return value from the write().
	  write(sockfd, buf, nbytes);
	}

	if (--nready <= 0) {
	  // All the changed file desc have been checked.
	  // can exit the for loop.
	  // This just makes it a bit more efficient. There
	  // could be a lot of idle file descriptors.
	  break;
	}
      }
    }
  }
}
