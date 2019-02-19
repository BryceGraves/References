#include <stdio.h>  // This is here so I can use printf().
#include <stdlib.h> // For putenv().
#include <string.h> // For strncmp() and strchr().

int
main(
     int argc        // The number of command line arguments.
     , char *argv[]  // An array of strings of the command line arguments
     , char *envp[]  // An array of strings for the environment for the process.
) {
  int i;
  int eq_loc;
  char *eq;
  char *new_env;

  printf("=======================\n");

  for (i = 0; NULL != envp[i]; i++) {
    printf("\tThe value of envp[%d] is: %s\n", i, envp[i]);
  }

  putenv("ENVIRONMENT_TEST=test_value");
  putenv("HOME=test_value");
  for (i = 0; NULL != envp[i]; i++) {
    eq = strchr(envp[i], '=');
    if (NULL != eq) {
      eq_loc = eq - envp[i];
      if (strncmp(envp[i], "ENVIRONMENT_TEST", eq_loc) == 0) {
	// Spoiler alert: you won't find it.
	printf("Found new environment variable in envp[%d] is: %s\n"
	       , i, envp[i]);
      }
      if (strncmp(envp[i], "HOME", eq_loc) == 0) {
	// Spoiler alert: it won't be changed.
	printf("Found HOME in envp[%d] is: %s\n"
	       , i, envp[i]);
      }
    }
  }

  new_env = getenv("ENVIRONMENT_TEST");
  if (new_env != NULL) {
    printf("Found new environment variable from getenv(): %s\n", new_env);
  }
  new_env = getenv("HOME");
  if (new_env != NULL) {
    printf("Found the value of HOME from getenv(): %s\n", new_env);
  }

  // To keep the compiler warning messages at bay.
  return(0);
}
