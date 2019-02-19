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

#define LISTENQ 1
#define MAXLINE 4096

#define QUOTE(_NAME_) #_NAME_
#define STR(_NAME_) QUOTE(_NAME_)

#define SERV_PORT 10001
#define SERV_PORT_STR STR(SERV_PORT)

int 
main(
     int argc
     , char *argv[]
) {
  int listenfd;
  int sockfd;
  int n;
  char buf[MAXLINE];
  socklen_t clilen;
  struct sockaddr_in cliaddr;
  struct sockaddr_in servaddr;

  // Create a socket from the AF_INET family, that is a stream
  listenfd = socket(AF_INET, SOCK_STREAM, 0);

  // Performing a memset() is quite important when doing 
  //   socket communication.
  memset(&servaddr, 0, sizeof(servaddr));

  // An IPv4 address
  servaddr.sin_family = AF_INET;
  // Host-TO-Network-Long. Listen on any interface/IP of the system.
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
  // Host-TO-Network-Short, the default port from above.
  servaddr.sin_port = htons(SERV_PORT);

  if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0) {
    perror("Something is not working");
    exit(-1);
  }

  // Set up a passive socket, with a backlog of 1. Reject any connection
  // attempts after 1 is already waiting. The return value from this should
  // be checked for error.
  listen(listenfd, LISTENQ);

  printf("server listening on %s\n", SERV_PORT_STR);  

  // client length
  clilen = sizeof(cliaddr);
  // Accept connections on the listenfd.
  sockfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);
  for ( ; ; ) {
    // You REALLY want to memset to all zeroes before you get bytes from
    // the socket.
    memset(buf, 0, sizeof(buf));
    if ((n = read(sockfd, buf, sizeof(buf))) == 0) {
      printf("EOF found on client connection socket, exiting\n");
      // nothing was read, EOF
      close(sockfd);
      // break out of the loop all is done
      break;
    }
    else {
      fprintf(stdout, "message from client: <%s>\n", buf);
      // echo the line back to the client
      write(sockfd, buf, n);
    }
  }

  printf("Closing listen socket\n");
  close(listenfd);
  return(EXIT_SUCCESS);
}
