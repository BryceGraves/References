#include <stdio.h>  // This is here so I can use printf().
#include <string.h> // for strcpy()
#include <stdlib.h> // for strtol() and strtod().
#include <unistd.h> // for getopt(), the short options.

int
main(
     int argc        // The number of command line arguments.
     , char *argv[]  // An array of strings of the command line arguments
) {
  int opt; // The return value from each call to getopt().
  int i_opt = -1;
  int o_opt = 0;
  double d_opt = 0.0;
  char s_opt[100] = "";

  while ((opt = getopt(argc, argv, "os:i:d:")) != -1) {
    switch (opt) {
    case 'o':
      o_opt++; // Increment the variable each time the -o option is seen.
      printf("The -o option has been seen: %d\n", o_opt);
      break;
    case 's':
      strcpy(s_opt, optarg);
      printf("The -s option has been seen with argument %s\n", s_opt);
      break;
    case 'i':
      i_opt = (int) strtol(optarg, NULL, 10);
      printf("The -i option has been seen with argument %d\n", i_opt);
      break;
    case 'd':
      d_opt = strtod(optarg, NULL);
      printf("The -d option has been seen with argument %f\n", d_opt);
      break;
    default:
      printf("something strange has happened\n");
      break;
    }
  }

  if (optind < argc) {
    int j;

    fprintf(stderr, "\nThis is what remains on the command line:\n");
    for(j = optind; j < argc; j++) {
      printf("\t%s\n", argv[j]);
    }
  }
  return(0);
}
