#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>

// from http://beej.us/guide/bgipc/output/html/multipage/unixsock.html

int main(
	 int argc
	 , char *argv[]
) {
  int sock_pair[2]; /* the pair of socket descriptors */
  char buf; /* for data exchange between processes */
  pid_t cpid;
  int status;

  if (socketpair(AF_UNIX, SOCK_STREAM, 0, sock_pair) == -1) {
    perror("socketpair");
    exit(1);
  }

  if ((cpid = fork()) != 0) {  // child
    int i;

    for (i = 'a'; i <= 'z'; i++) {
      read(sock_pair[0], &buf, sizeof(buf));
      printf("  child: read '%c'\n", buf);

      buf = toupper(buf);  // upper case the character

      write(sock_pair[0], &buf, sizeof(buf));
      printf("  child: sent '%c'\n", buf);
    }

    for (i = 'z'; i >= 'a'; i--) {
      read(sock_pair[1], &buf, sizeof(buf));
      printf("    child: read '%c'\n", buf);

      buf = toupper(buf);  // upper case the character

      write(sock_pair[1], &buf, sizeof(buf));
      printf("    child: sent '%c'\n", buf);
    }
  } 
  else { // parent
    int i;

    for (i = 'a'; i <= 'z'; i++) {
      buf = i;
      write(sock_pair[1], &buf, sizeof(buf));
      printf("parent: sent '%c'\n", i);

      read(sock_pair[1], &buf, sizeof(buf));
      printf("parent: read '%c'\n", buf);
    }

    // Use the other side of the socket pair to go the other direction.
    for (i = 'z'; i >= 'a'; i--) {
      buf = i;
      write(sock_pair[0], &buf, sizeof(buf));
      printf("  parent: sent '%c'\n", i);

      read(sock_pair[0], &buf, sizeof(buf));
      printf("  parent: read '%c'\n", buf);
    }

    wait(&status); // wait for child to terminate
  }

  return 0;
}
