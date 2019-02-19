 /*
 * R Jesse Chaney
 */

// Inspired from
// http://www.xinotes.net/notes/note/1810/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>

#define BUF_SIZE 2048
#define PORT 10011

int 
main(
     int argc
     , char* argv[]
) {
  char buf[BUF_SIZE];
  char reply[BUF_SIZE] = {0};
  struct sockaddr_in servaddr;
  struct sockaddr_in cliaddr;
  socklen_t addrlen = sizeof(struct sockaddr_in);
  ssize_t nbytes;
  int socketfd;
  FILE *fp;

  /* initialize socket */
  if ((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    perror("socket creation error");
    return(EXIT_FAILURE);
  }

  memset(&servaddr, 0, sizeof(struct sockaddr_in));

  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(PORT);
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(socketfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) == -1) {
    perror("bind failur");
    return(EXIT_FAILURE);
  }

  printf("Listening for connections on port %d\n", PORT);
  // No call to listen, accept, or select, just a recvfrom().
  while ((nbytes = recvfrom(socketfd, buf, BUF_SIZE, 0
			    , (struct sockaddr *) &cliaddr, &addrlen)) != -1) {
    buf[nbytes] = 0;
    printf("Received message from %s:%d >%s<\n"
	   , inet_ntoa(cliaddr.sin_addr)
	   , ntohs(cliaddr.sin_port)
	   , buf);

    memset(reply, 0, sizeof(reply));
    fp = popen("fortune", "r");
    while (fgets(buf, sizeof(buf), fp) != NULL) {
      strcat(reply, buf);      
    }
    pclose(fp);

    sendto(socketfd, reply, strlen(reply), 0, (struct sockaddr *) &cliaddr, addrlen);
    printf("  Sent random fortune: %s\n", reply);
  }
  
  close(socketfd);
  return 0;
}
