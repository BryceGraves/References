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
  //  int i;
  int sockfd;
  //size_t nbytes;
  struct sockaddr_in servaddr;
  char sendline[MAXLINE];
  char recvline[MAXLINE];
  char ip_addr[50] = "10.101.0.47"; // If you don't give an ip on the cmd line.

  if (argc > 1) {
    //printf("Must give ip addess\n");
    //exit(1);
    strcpy(ip_addr, argv[1]);
  }

  // You might want to allow the port to be passed on the command line.

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  memset(&servaddr, 0, sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  // Host-TO-Network-Short for the port number.
  servaddr.sin_port = htons(SERV_PORT);
  inet_pton(AF_INET, ip_addr, &servaddr.sin_addr.s_addr);

  if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0) {
    perror("could not connect");
    exit(EXIT_FAILURE);
  }

  fputs("ready >> ", stdout);
  while (fgets(sendline, sizeof(sendline), stdin) != NULL) {
    // read from the user.

    // Get rid of that PESKY newline character
    sendline[strlen(sendline) - 1] = 0;

    // make sure that the receive buffer is nice and clean
    memset(recvline, 0, sizeof(recvline));

    // send the line to the server
    write(sockfd, sendline, strlen(sendline));

    memset(recvline, 0, sizeof(recvline));
    // read back from the server
    if (read(sockfd, recvline, sizeof(recvline)) == 0) {
      perror("socket is closed");
      break;
    }
    
    fprintf(stdout, "back from server: <%s>\n", recvline);
    fputs("ready >> ", stdout);
  }

  close(sockfd);
  return(EXIT_SUCCESS);
}
