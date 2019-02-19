#include <stdio.h>

#ifndef MAX_LINE_LEN
# define MAX_LINE_LEN 1024
#endif // MAX_LINE_LEN

int main(int argc, char *argv[])
{
  char line[MAX_LINE_LEN];
  char *line_ptr;

  while ((line_ptr = fgets(line, MAX_LINE_LEN, stdin)) != NULL) {
    fputs(line, stdout);
  }

  return(0);
}
