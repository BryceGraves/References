/*
 * R Jesse Chaney
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "socket_htonl.h"

int 
main(
     int argc
     , char *argv[]
) {
  int listenfd;
  int sockfd;
  int n;
  union int_union from;
  union int_union back;
  socklen_t clilen;
  struct sockaddr_in cliaddr;
  struct sockaddr_in servaddr;
  
  listenfd = socket(AF_INET, SOCK_STREAM, 0);

  memset(&servaddr, 0, sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
  servaddr.sin_port = htons(SERV_PORT);

  if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0) {
    perror("Something is not working");
    exit(-1);
  }

  // set up a passive socket, with a back log of LISTENQ
  listen(listenfd, LISTENQ);

  // client length
  clilen = sizeof(cliaddr);
  printf("server listening on %s\n", SERV_PORT_STR);  

  // accept connections on the listenfd.
  sockfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);
  for ( ; ; ) {
    memset(from.ch, 0, sizeof(from));
    if ((n = read(sockfd, from.ch, sizeof(from))) == 0) {
      // nothing was read, EOF
      close(sockfd);
      // break out of the loop all is done
      break;
    }
    fprintf(stdout, "  from bytes  0x%02hhx  ox%02hhx  ox%02hhx  ox%02hhx\n"
	    , from.ch[0], from.ch[1], from.ch[2], from.ch[3]);
    if (argc > 1) {
      // Switch the data form network order to host order.
      from.num = ntohl(from.num);
      fprintf(stdout, "  htonl bytes 0x%02hhx  ox%02hhx  ox%02hhx  ox%02hhx\n"
	      , from.ch[0], from.ch[1], from.ch[2], from.ch[3]);
    }
    fprintf(stdout, "  from value: %d\n", from.num);
    from.num ++;
    fprintf(stdout, "  back value: %d\n", from.num);

    fprintf(stdout, "  back bytes  0x%02hhx  ox%02hhx  ox%02hhx  ox%02hhx\n"
	    , from.ch[0], from.ch[1], from.ch[2], from.ch[3]);
    if (argc > 1) {
      // Switch it from host order to network order.
      back.num = htonl(from.num);
      fprintf(stdout, "  htonl bytes 0x%02hhx  ox%02hhx  ox%02hhx  ox%02hhx\n"
	      , from.ch[0], from.ch[1], from.ch[2], from.ch[3]);
    }
    else {
      back.num = from.num;
    }
    write(sockfd, back.ch, sizeof(back));
  }

  return(EXIT_SUCCESS);
}
