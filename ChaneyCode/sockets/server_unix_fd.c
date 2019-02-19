 /*
 * R Jesse Chaney
 */

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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define SOCK_PATH "fd_socket"

static void simple_handler(int);
static void onexit_function( void );
int send_fd(int socket, int fd_to_send);

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

// from http://www.thomasstover.com/uds.html
int send_fd(int socket, int fd_to_send)
{
  struct msghdr socket_message;
  struct iovec io_vector[1];
  struct cmsghdr *control_message = NULL;
  char message_buffer[1];
  // storage space needed for an ancillary element with a paylaod 
  //   of length is CMSG_SPACE(sizeof(length))
  char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];
  int available_ancillary_element_buffer_space;

  /* at least one vector of one byte must be sent */
  message_buffer[0] = 'F';
  io_vector[0].iov_base = message_buffer;
  io_vector[0].iov_len = 1;

  /* initialize socket message */
  memset(&socket_message, 0, sizeof(struct msghdr));
  socket_message.msg_iov = io_vector;
  socket_message.msg_iovlen = 1;

  /* provide space for the ancillary data */
  available_ancillary_element_buffer_space = CMSG_SPACE(sizeof(int));
  memset(ancillary_element_buffer, 0, available_ancillary_element_buffer_space);
  socket_message.msg_control = ancillary_element_buffer;
  socket_message.msg_controllen = available_ancillary_element_buffer_space;

  /* initialize a single ancillary data element for fd passing */
  control_message = CMSG_FIRSTHDR(&socket_message);
  control_message->cmsg_level = SOL_SOCKET;
  control_message->cmsg_type = SCM_RIGHTS;
  control_message->cmsg_len = CMSG_LEN(sizeof(int));
  *((int *) CMSG_DATA(control_message)) = fd_to_send;

  return sendmsg(socket, &socket_message, 0);
}

int 
main(
 int argc
 , char *argv[]
) {
  int sfd1;
  int sfd2;
  int fd = -1;
  socklen_t t;
  int len;
  struct sockaddr_un local;
  struct sockaddr_un remote;

  atexit(onexit_function);
  signal(SIGINT, simple_handler);

  // Notice that this is an AF_UNIX socket, not AF_INET.
  if ((sfd1 = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket could not be created");
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

  printf("Waiting for a client connection...\n");
  memset(&remote, 0, sizeof(remote));
  t = sizeof(remote);
  if ((sfd2 = accept(sfd1, (struct sockaddr *) &remote, &t)) == -1) {
    perror("accept");
    exit(EXIT_FAILURE);
  }

  printf("A new client connection.\n");

  // Have the server send an open file descriptor to the client.

  fd = open("words4.txt", O_RDONLY);
  if (fd < 0) {
    perror("error opening file");
    exit(EXIT_FAILURE);
  }
  printf(">>>%d<<<\n", fd);
  send_fd(sfd2, fd);

  // Close the socket when done.
  close(sfd2);

  return(EXIT_SUCCESS);
}


