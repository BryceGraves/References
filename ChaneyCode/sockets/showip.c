 /*
 * R Jesse Chaney
 */

// from:
//   http://www.beej.us/guide/bgnet/output/html/multipage/syscalls.html#getaddrinfo

// Show IP addresses (IPv4 and IPv6) for a host given on the command line.

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
  struct addrinfo hints;
  struct addrinfo *addr_list;
  struct addrinfo *p;
  int status;
  int i;
  char ipstr[INET6_ADDRSTRLEN];

  if (argc < 2) {
    fprintf(stderr,"Usage: showip hostname\n");
    return(EXIT_FAILURE);
  }

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;      // AF_INET or AF_INET6 to force version
  hints.ai_socktype = SOCK_STREAM;

  for (i = 1; i < argc; i++) {
    // Take the hostname give on the command line and turn it into
    // an ip address, IPv4 or IPv6 (or both).
    if ((status = getaddrinfo(argv[i], NULL, &hints, &addr_list)) != 0) {
      // An error.
      fprintf(stderr, "getaddrinfo: %s  %s\n", gai_strerror(status), argv[i]);
      //return(2);
      break;
    }

    printf("IP addresses for %s:\n", argv[i]);

    // Loop through the list of returned addresses.
    for(p = addr_list; p != NULL; p = p->ai_next) {
      void *addr;
      char *ipver;
      
      // Get the pointer to the address itself,
      //   different fields in IPv4 and IPv6:
      if (p->ai_family == AF_INET) { // IPv4
	// Notice that after we determine the address family, we have to
	// cast to a new type and that the types are different between
	// IPv4 and IPv6.
	struct sockaddr_in *ipv4 = (struct sockaddr_in *) p->ai_addr;
	
	addr = &(ipv4->sin_addr);
	ipver = "IPv4";
      }
      else { // IPv6
	// We are assuming that AF_INET and AF_INET6 will be the
	// only return types.
	struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *) p->ai_addr;
	
	addr = &(ipv6->sin6_addr);
	ipver = "IPv6";
      }
      
      // convert the IP to a string and print it:
      inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
      printf("\t%s: %s\n", ipver, ipstr);
    }
    printf("\n");

    // You must call this to free up the memory allocated to
    //   the list from the call to getaddrinfo().
    freeaddrinfo(addr_list);
  }

  return(EXIT_SUCCESS);
}
