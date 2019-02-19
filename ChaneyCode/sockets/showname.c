 /*
 * R Jesse Chaney
 */

// from:
//   http://cboard.cprogramming.com/linux-programming/94179-getnameinfo.html

// Show the hostname from a IPv4 address given on the command line.

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>

int 
main(
     int argc
     , char *argv[]
) {
  struct sockaddr_in sa;
  char hostname[NI_MAXHOST];
  char servname[NI_MAXHOST];
  int res;

  if (argc < 2) {
    fprintf(stderr, "Usage: showname IPv4_address [port_number]\n");
    return(EXIT_FAILURE);
  }

  sa.sin_family = AF_INET;  // Works for IPv4 addresses.

  // Convert from the presentstion name to the network name.
  inet_pton(AF_INET, argv[1], &sa.sin_addr);
  if (argc > 2) {
    sa.sin_port = htons(atoi(argv[2]));
  }

  memset(hostname, 0, sizeof(hostname));
  // Now fetch the hostname from the network address.
  res = getnameinfo((struct sockaddr*) &sa
		    , sizeof(sa)
		    , hostname
		    , sizeof(hostname)
		    , servname, sizeof(servname), 0);

  if (0 == res) {
    printf("IPv4 address: %s\n", argv[1]);
    printf("  Hostname: %s\n", hostname);
    if (argc > 2) {
      printf("  Service name: %s\n", servname);
    }
  }
  else {
    perror("Failed getnameinfo()");
    fprintf(stderr, "%s\n", gai_strerror(res));
    return(EXIT_FAILURE);
  }
 
  return(EXIT_SUCCESS);
}
