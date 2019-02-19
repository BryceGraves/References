#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#ifndef MAX_BUFFER_LEN
# define MAX_BUFFER_LEN 1024
#endif // MAX_BUFFER_LEN

int main(int argc, char *argv[])
{
  char buffer[MAX_BUFFER_LEN];
  ssize_t bytes_read;

  while ((bytes_read = read(STDIN_FILENO, buffer, MAX_BUFFER_LEN)) > 0) {
    write(STDOUT_FILENO, buffer, bytes_read);
  }

  return(0);
}
