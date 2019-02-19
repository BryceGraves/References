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

#define SOCK_PATH "fd_socket"

int recv_fd(int socket);

int recv_fd(int socket)
{
  int sent_fd;
  //int available_ancillary_element_buffer_space;
  struct msghdr socket_message;
  struct iovec io_vector[1];
  struct cmsghdr *control_message = NULL;
  char message_buffer[1];
  char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];

  /* start clean */
  memset(&socket_message, 0, sizeof(struct msghdr));
  memset(ancillary_element_buffer, 0, CMSG_SPACE(sizeof(int)));

  /* setup a place to fill in message contents */
  io_vector[0].iov_base = message_buffer;
  io_vector[0].iov_len = 1;
  socket_message.msg_iov = io_vector;
  socket_message.msg_iovlen = 1;

  /* provide space for the ancillary data */
  socket_message.msg_control = ancillary_element_buffer;
  socket_message.msg_controllen = CMSG_SPACE(sizeof(int));

  if(recvmsg(socket, &socket_message, MSG_CMSG_CLOEXEC) < 0) {
    return -1;
  }

  if(message_buffer[0] != 'F') {
    /* this did not originate from the above function */
    return -1;
  }

  if((socket_message.msg_flags & MSG_CTRUNC) == MSG_CTRUNC) {
    /* we did not provide enough space for the ancillary element array */
    return -1;
  }

  /* iterate ancillary elements */
  for(control_message = CMSG_FIRSTHDR(&socket_message);
      control_message != NULL;
      control_message = CMSG_NXTHDR(&socket_message, control_message)) {
    if((control_message->cmsg_level == SOL_SOCKET) &&
       (control_message->cmsg_type == SCM_RIGHTS) ) {
      sent_fd = *((int *) CMSG_DATA(control_message));
      return sent_fd;
    }
  }

  return -1;
}

int main(void)
{
  int s;
  int fd;
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

  fd = recv_fd(s);

  memset(str, 0, sizeof(str));
  len = read(fd, str, 100);
  if (len < 0) {
    perror("read from fd failed");
  }
  write(STDOUT_FILENO, str, len);

  close(s);

  return(EXIT_SUCCESS);
}
