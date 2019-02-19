 /*
 * R Jesse Chaney
 */

// From
// http://ntrg.cs.tcd.ie/undergrad/4ba2/multicast/antony/example.html
//   See also http://www.tenouk.com/Module41c.html
/*
 * listener.c -- joins a multicast group and echoes all data it receives from
 *		the group to its stdout...
 *
 * Antony Courtney,	25/11/94
 * Modified by: Frédéric Bastien (25/03/04)
 * to compile without warning and work correctly
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define PORT 10502
#define MCAST_GROUP "225.0.0.37"
#define MSGBUFSIZE 256

int
main(
     int argc
     , char *argv[]
) {
  struct sockaddr_in addr;
  struct ip_mreq mreq;
  int fd;
  int nbytes;
  socklen_t addrlen;
  char message[MSGBUFSIZE];
  unsigned yes = 1;

  /* create what looks like an ordinary UDP socket */
  if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket");
    return(1);
  }

  /* allow multiple sockets to use the same PORT number */
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
    perror("Reusing ADDR failed");
    return(1);
  }

  /* set up destination address */
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY); /* N.B.: differs from sender */
  addr.sin_port = htons(PORT);
     
  /* bind to receive address */
  if (bind(fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
    perror("bind");
    return(1);
  }
     
  /* use setsockopt() to request that the kernel join a multicast group */
  mreq.imr_multiaddr.s_addr = inet_addr(MCAST_GROUP);
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);
  //mreq.imr_interface.s_addr = inet_addr(argv[1]);
  //inet_pton(AF_INET, argv[1], &mreq.imr_interface.s_addr);

  if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
    perror("setsockopt");
    return(1);
  }

  /* now just enter a read-print loop */
  for ( ; ; ) {
    addrlen = sizeof(addr);
    if ((nbytes = recvfrom(fd, message, MSGBUFSIZE, 0,
			 (struct sockaddr *) &addr, &addrlen)) < 0) {
      perror("recvfrom");
      return(EXIT_FAILURE);
    }
    message[nbytes] = 0;
    printf("Received message: >%s<\n", message);
  }
}
