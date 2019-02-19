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
  union int_union from;
  union int_union back;
  struct sockaddr_in servaddr;
  char line[MAXLINE];
  char ip_addr[50];
  int sockfd;

  if (argc < 2) {
    fprintf(stderr, "Must give ip addess\n");
    exit(1);
  }
  else {
    strcpy(ip_addr, argv[1]);
  }

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  memset(&servaddr, 0, sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(SERV_PORT);
  inet_pton(AF_INET, ip_addr, &servaddr.sin_addr.s_addr);

  if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0) {
    perror("could not connect");
    exit(EXIT_FAILURE);
  }

  fputs("ready >> ", stdout);
  while (fgets(line, sizeof(line), stdin) != NULL) {
    // read an integer from the user.

    // get rid of that pesky newline character
    line[strlen(line) - 1] = 0;

    memset(from.ch, 0, sizeof(from));
    from.num = atol(line);
    fprintf(stdout, "  from bytes  0x%02hhx  ox%02hhx  ox%02hhx  ox%02hhx\n"
	    , from.ch[0], from.ch[1], from.ch[2], from.ch[3]);
    if (argc > 2) {
      from.num = htonl(atol(line));
      fprintf(stdout, "  htonl bytes 0x%02hhx  ox%02hhx  ox%02hhx  ox%02hhx\n"
	      , from.ch[0], from.ch[1], from.ch[2], from.ch[3]);
    }

    // make sure that the receive buffer is nice and clean
    //memset(recvline, 0, sizeof(recvline));
    memset(back.ch, 0, sizeof(back));

    // send the line to the server
    write(sockfd, from.ch, sizeof(from));

    // read back from the server
    if (read(sockfd, back.ch, sizeof(back)) == 0) {
      perror("socket is closed");
      exit(EXIT_FAILURE);
    }
    fprintf(stdout, "  back bytes  ox%02hhx  ox%02hhx  0x%02hhx  ox%02hhx\n"
	    , back.ch[0], back.ch[1], back.ch[2], back.ch[3]);
    if (argc > 2) {
      back.num = ntohl(back.num);
      fprintf(stdout, "  htonl bytes ox%02hhx  ox%02hhx  0x%02hhx  ox%02hhx\n"
	      , back.ch[0], back.ch[1], back.ch[2], back.ch[3]);
    }
    fprintf(stdout, "  value from server: <%d>\n", back.num);
    fputs("ready >> ", stdout);
  }

  return(EXIT_SUCCESS);
}
