#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#ifndef MAX_BUFFER_LEN
# define MAX_BUFFER_LEN 1024
#endif // MAX_BUFFER_LEN

int main(int argc, char *argv[])
{
  char buffer[MAX_BUFFER_LEN];
  ssize_t bytes_read;

  if (argc <= 1) {
    // If no files are listed on the command buffer.
    while ((bytes_read = read(STDIN_FILENO, buffer, MAX_BUFFER_LEN)) > 0) {
      write(STDOUT_FILENO, buffer, bytes_read);
    }
  }
  else {
    int i;
    int fd;

    // Loop through the  files listed on the command line.
    for (i = 1; i < argc; i++) {
      fd = open(argv[i], O_RDONLY);
      if (fd > 0) {
	while ((bytes_read = read(fd, buffer, MAX_BUFFER_LEN)) > 0) {
	  write(STDOUT_FILENO, buffer, bytes_read);
	}
	close(fd);
      }
      else {
	fprintf(stderr, "Could not open file for input %s\n", argv[i]);
      }
    }
  }

  return(0);
}
