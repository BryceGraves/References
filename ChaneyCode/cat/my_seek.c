#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef MAX_BUFFER_LEN
# define MAX_BUFFER_LEN 1024
#endif // MAX_BUFFER_LEN

int main(int argc, char *argv[])
{
  char buffer[MAX_BUFFER_LEN];
  ssize_t bytes_read;
  char * file_name;
  int fd;
  long seek_to;
  long num_bytes;
  
  if (argc < 4) {
    fprintf(stderr, "Not enough arguments\n");
    return(1);
  }
  file_name = argv[1];
  seek_to = strtol(argv[2], NULL, 10);
  num_bytes = strtol(argv[3], NULL, 10);

  fd = open(file_name, O_RDONLY);
  if (fd >= 0) {
    int oset = lseek(fd, seek_to, SEEK_SET);
    if (oset < 0) {
      fprintf(stderr, "lseek failed\n");
      perror("lseek failed");
    }
    bytes_read = read(fd, buffer, num_bytes);
    if (bytes_read > 0) {
      write(STDOUT_FILENO, buffer, bytes_read);
    }
  }
  else {
    fprintf(stderr, "could not open file to read: %s\n", file_name);
  }

  return(0);
}
