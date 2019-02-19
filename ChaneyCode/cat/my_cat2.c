#include <stdio.h>

#ifndef MAX_LINE_LEN
# define MAX_LINE_LEN 1024
#endif // MAX_LINE_LEN

int main(int argc, char *argv[])
{
  char line[MAX_LINE_LEN];
  char *line_ptr;

  if (argc <= 1) {
    // If no files are listed on the command line.
    while ((line_ptr = fgets(line, MAX_LINE_LEN, stdin)) != NULL) {
      fputs(line, stdout);
    }
  }
  else {
    int i;
    FILE *ip;

    // Loop through the  files listed on the command line.
    for (i = 1; i < argc; i++) {
      ip = fopen(argv[i], "r");
      if (ip != NULL) {
	while ((line_ptr = fgets(line, MAX_LINE_LEN, ip)) != NULL) {
	  fputs(line, stdout);
	}
	fclose(ip);
      }
    }
  }

  return(0);
}
