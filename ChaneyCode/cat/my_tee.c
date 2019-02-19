#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

//#ifndef MAX_BUFFER_LEN
//# define MAX_BUFFER_LEN 1024
//#endif // MAX_BUFFER_LEN
//#ifndef MAX_TEE_FILES
//# define MAX_TEE_FILES 10
//#endif // MAX_TEE_FILES

int main(int argc, char *argv[])
{
  char buffer[MAX_BUFFER_LEN];
  ssize_t bytes_read;
  int tee_fd[MAX_TEE_FILES];
  int i;

  for (i = 1; i < argc; i++) {
    int fd = open(argv[i], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP);
    if (fd >= 0) {
      tee_fd[i] = fd;
    }
    else {
      // Could not open the file for output.
      tee_fd[i] = -1;
      fprintf(stderr, "Could not open file for output %s\n", argv[i]);
    }
  }

  while ((bytes_read = read(STDIN_FILENO, buffer, MAX_BUFFER_LEN)) > 0) {
    write(STDOUT_FILENO, buffer, bytes_read);
    for (i = 1; i < argc; i++) {
      if (tee_fd[i] >= 0) {
	write(tee_fd[i], buffer, bytes_read);
      }
    }
  }
  for (i = 1; i < argc; i++) {
    if (tee_fd[i] >= 0) {
      close(tee_fd[i]);
    }
  }

  return(0);
}
