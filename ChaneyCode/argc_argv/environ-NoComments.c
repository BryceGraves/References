#include <stdio.h>  // This is here so I can use printf().
#include <stdlib.h> // For putenv().
#include <string.h> // For strncmp() and strchr().
#include <unistd.h> // The POSIX stuff.

// This is defined in the unistd.h include file and is POSIX happy.
extern char **environ;

int
main(
     int argc        // The number of command line arguments.
     , char *argv[]  // An array of strings of the command line arguments
) {
  int i;
  int eq_loc;
  char *eq;
  char *new_env;

  printf("=======================\n");

  for (i = 0; NULL != environ[i]; i++) {
    printf("\tThe value of environ[%d] is: %s\n", i, environ[i]);
  }

  putenv("ENVIRONMENT_TEST=test_value");
  putenv("HOME=test_value");
  for (i = 0; NULL != environ[i]; i++) {
    eq = strchr(environ[i], '=');
    if (NULL != eq) {
      eq_loc = eq - environ[i];
      if (strncmp(environ[i], "ENVIRONMENT_TEST", eq_loc) == 0) {
	printf("Found new environment variable in environ[%d] is: %s\n"
	       , i, environ[i]);
      }
      if (strncmp(environ[i], "HOME", eq_loc) == 0) {
	printf("Found new value for HOME in environ[%d] is: %s\n"
	       , i, environ[i]);
      }
    }
  }

  new_env = getenv("ENVIRONMENT_TEST");
  if (new_env != NULL) {
    printf("Found new environment variable from getenv(): %s\n", new_env);
  }
  new_env = getenv("HOME");
  if (new_env != NULL) {
    printf("Found the new value of HOME from getenv(): %s\n", new_env);
  }

  return(0);
}
