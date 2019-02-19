 /*
 * R Jesse Chaney
 */

// Inspired from:
// http://beej.us/guide/bgipc/output/html/multipage/unixsock.html

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>


#define SOCK_PATH "echo_socket"

static void simple_handler(int);
static void onexit_function( void );

static void
simple_handler (
    int sig
) {
  exit(EXIT_SUCCESS);
}

static void
onexit_function(
     void
) {
  char str[100];

  strcpy(str, SOCK_PATH);
  unlink(str);
}

int 
main(
	 int argc
	 , char *argv[]
) {
  int sfd1;
  int sfd2;
  socklen_t t;
  int len;
  struct sockaddr_un local;
  struct sockaddr_un remote;

  atexit(onexit_function);
  signal(SIGINT, simple_handler);

  // Notice that this is an AF_UNIX socket, not AF_INET.
  if ((sfd1 = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // Make sure it is nice and clean.
  memset(&local, 0, sizeof(local));

  local.sun_family = AF_UNIX;
  strcpy(local.sun_path, SOCK_PATH);

  // Make sure the socket file does not already exist. It would
  // be better to check the return value.
  unlink(local.sun_path);

  len = strlen(local.sun_path) + sizeof(local.sun_family);

  // Bind the socket path. Notice that there are no port numbers.
  if (bind(sfd1, (struct sockaddr *) &local, len) == -1) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  // If I want to limit access to the file.
  // fchmod(s, S_IRUSR | S_IWUSR);
  // chmod(local.sun_path, S_IRUSR | S_IWUSR);

  // Begin to allow client connections on the socket. Limit the backlog
  // to allow only 2 connections before client connections could be
  // denied.
  if (listen(sfd1, 2) == -1) {
    perror("listen failed");
    exit(EXIT_FAILURE);
  }

  for( ; ; ) {
    int done = 0;
    int n;
    char str[100];

    printf("Waiting for a client connection...\n");
    memset(&remote, 0, sizeof(remote));
    t = sizeof(remote);
    if ((sfd2 = accept(sfd1, (struct sockaddr *) &remote, &t)) == -1) {
      perror("accept");
      exit(EXIT_FAILURE);
    }

    printf("A new client connection.\n");

    // Allow the client and server to exchange text.
    // The way this is written, the server will allow multiple
    // clients to connect, but will interact with only one at
    // a time.
    do {
      n = read(sfd2, str, sizeof(str));
      if (n <= 0) {
	if (n < 0) {
	  perror("read error");
	}
	// Got 0 bytes back from the read on the socket, it must 
	// the end of commnications.
	done = 1;
	printf("Client connection closed\n");
      }

      if (!done) {
	// Print the text received from the client to the screen.
	printf("Received from client >>%s<<\n", str);

	// Send the text back to the client.
	if (write(sfd2, str, n) < 0) {
	  perror("write error");
	  done = 1;
	}
      }
    } while (!done);

    // Close the socket when done.
    close(sfd2);
  }

  return(EXIT_SUCCESS);
}


