 /*
 * R Jesse Chaney
 */

// From
// http://www.xinotes.net/notes/note/1810/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#define BUF_SIZE 1024
#define PORT 10011

int
main(
     int argc
     , char *argv[]
) {
    struct sockaddr_in server;
    socklen_t len = sizeof(struct sockaddr_in);
    char buf[BUF_SIZE];
    //char request[] = "Are we there yet?";
    ssize_t nbytes;
    int serv_soc;
    struct timeval tv;

    if (argc < 2) {
	fprintf(stderr, "Usage: %s <host_ip>\n", argv[0]);
	return(1);
    }

    /* initialize socket */
    if ((serv_soc = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
	perror("socket");
	return(1);
    }
    // Configure the port to timeout after 5 seconds.
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    if (setsockopt(serv_soc, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
      perror("Error on setsockopt");
    }


    /* initialize server addr */
    memset((char *) &server, 0, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    inet_pton(AF_INET, argv[1], &server.sin_addr.s_addr);

    fputs("ready >> ", stdout);
    while (fgets(buf, sizeof(buf), stdin) != NULL) {
      buf[strlen(buf) - 1] = 0;
      if (sendto(serv_soc, buf, strlen(buf ), 0
		 , (struct sockaddr *) &server, len) == -1) {
	perror("sendto() error");
	break;
      }
      printf("Sent request\n");
      // Notice that at this point, the "client" is doing a recvfrom
      // exactly as the "server" has done.
      if ((nbytes = recvfrom(serv_soc, buf, BUF_SIZE, 0
			     , (struct sockaddr *) &server, &len)) == -1) {
	if (errno == EAGAIN || errno == EWOULDBLOCK) {
	  printf("timeout\n");
	}
	else {
	  perror("recvfrom() error");
	  break;
	}
      }
      buf[nbytes] = 0;
      printf("  Received reply from %s:%d: %s\n"
	     , inet_ntoa(server.sin_addr)
	     , ntohs(server.sin_port)
	     , buf);
    }

    close(serv_soc);
    return(0);
}
