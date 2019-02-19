#include <stdio.h> // This is here so I can use printf().

int
main(
     int argc        // The number of command line arguments.
     , char *argv[]  // An array of strings of the command line arguments.
) {
  int i;

  printf("The value of argc is: %d\n", argc);
  printf("The value of argv[0] is: %s\n", argv[0]);

  for (i = 1; i < argc; i++) {
    printf("\tThe value of argv[%d] is: %s\n", i, argv[i]);
  }

  for (i = 1; NULL != argv[i]; i++) {
    printf("\tThe value of argv[%d] is: %s\n", i, argv[i]);
  }

  return(0);
}
