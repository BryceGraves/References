/*
 * R Jesse Chaney
 */

// Inspired From
// http://ntrg.cs.tcd.ie/undergrad/4ba2/multicast/antony/example.html
//   See also http://www.tenouk.com/Module41c.html
/*
 * sender.c -- multicasts "hello, world!" to a multicast group once a second
 *
 * Antony Courtney,	25/11/94
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#define PORT 10502
#define MCAST_GROUP "225.0.0.37"

int
main(
     int argc
     , char *argv[]
) {
  struct sockaddr_in addr;
  //struct ip_mreq mreq;
  int fd;
  //int cnt;
  char message[100];

  /* create what looks like an ordinary UDP socket */
  if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket");
    return(1);
  }

  /* set up destination address */
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(MCAST_GROUP);
  addr.sin_port = htons(PORT);

  /* now just sendto() our destination! */
  for ( ; ; ) {
    time_t t = time(NULL);
    sprintf(message, "The time is now %s", ctime(&t));
    message[strlen(message) - 1] = 0;
    printf("Sending message >%s<\n", message);
    if (sendto(fd, message, sizeof(message), 0
	       , (struct sockaddr *) &addr, sizeof(addr)) < 0) {
      perror("sendto");
      return(EXIT_FAILURE);
    }
    sleep(2);
  }
}
