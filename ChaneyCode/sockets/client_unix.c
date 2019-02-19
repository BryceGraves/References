 /*
 * R Jesse Chaney
 */

// from:
// http://beej.us/guide/bgipc/output/html/multipage/unixsock.html

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCK_PATH "echo_socket"

int main(void)
{
  int s;
  int t;
  int len;
  struct sockaddr_un remote;
  char str[101];

  if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(1);
  }

  printf("Trying to connect...\n");

  memset(&remote, 0, sizeof(remote));
  remote.sun_family = AF_UNIX;
  strcpy(remote.sun_path, SOCK_PATH);
  len = strlen(remote.sun_path) + sizeof(remote.sun_family);
  if (connect(s, (struct sockaddr *)&remote, len) == -1) {
    perror("could not connect");
    exit(EXIT_FAILURE);
  }

  printf("Connected to server.\n");

  for ( ; ; ) {

    printf("--> ");
    fgets(str, 100, stdin);
    if (feof(stdin)) {
      // End of input from stdin, exiting.
      break;
    }
    if (write(s, str, strlen(str)) == -1) {
      perror("write error");
      exit(EXIT_FAILURE);
    }

    if ((t = read(s, str, 100)) > 0) {
      // Make sure the retuned buffer is NULL terminated.
      str[t] = 0;
      printf("back from server> >%s<\n", str);
    } else {
      if (t < 0) {
	perror("read error");
      } else {
	printf("Server closed connection\n");
      }
      exit(EXIT_FAILURE);
    }
  }

  close(s);

  return(EXIT_SUCCESS);
}
