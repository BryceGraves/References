/*
 * R Jesse Chaney
 */

// I have put a LOT of comments into this code. If you are not using
//   an editor that make comments stand out (such as by color), this
//   may be difficult to read. In any case, you will be wading through
//   a lot of comments to find a few lines of code. I hope the comments
//   are helpful.

// compile with
//     gcc -Wall -o environ environ.c

#include <stdio.h>  // This is here so I can use printf().
#include <stdlib.h> // For putenv().
#include <string.h> // For strncmp() and strchr().
#include <unistd.h> // The POSIX stuff.

// This is defined in the unistd.h include file and is POSIX happy.
extern char **environ;

// Environment variables are sometimes called shell variables. You
//   typically set them in the dot files in your home directory, such 
//   as your .login or .bashrc file.
int
main(
     int argc        // The number of command line arguments.
     , char *argv[]  // An array of strings of the command line arguments
) 
{
  int i;
  int eq_loc;
  char *eq;
  char *new_env;

  // I'm going to leave out the discussion of argc and argv. If you
  //   want to review those comments of that code, you should download
  //   and look at the argv.c code. This is going to focus on the 
  //   environ variables.
  printf("The value of argc is: %d\n", argc);
  printf("The value of argv[0] is: %s\n", argv[0]);
  for (i = 1; i < argc; i++) 
  {
    printf("\tThe value of argv[%d] is: %s\n", i, argv[i]);
  }
  printf("=======================\n");

  // We gave a look at using a third parameter to the main() function
  //   that contained a list of all of the environment variables, but
  //   found it lacking, for a couple of reasons. First, it is not POSIX.
  //   Its use makes our code less portable. We like portable code. Second,
  //   It is static. Neither new environment variables nor changed environment
  //   variables are reflected in the envp variable. This makes us sad.
  //   There is option though, the environ variable (from the unistd.h include
  //   file). It is POSIX happy and reflects changes to the environment.
  for (i = 0; NULL != environ[i]; i++) 
  {
    printf("\tThe value of environ[%d] is: %s\n", i, environ[i]);
  }

  // Create a new environment variable.
  putenv("ENVIRONMENT_TEST=test_value");
  // Change an existing environment variable. This will change for this
  //   process only.
  putenv("HOME=test_value");
  for (i = 0; NULL != environ[i]; i++) 
  {
    eq = strchr(environ[i], '=');
    if (NULL != eq) 
    {
      eq_loc = eq - environ[i];
      if (strncmp(environ[i], "ENVIRONMENT_TEST", eq_loc) == 0) 
      {
	// This will find the new environment variable. If you look at the
	//   the index at which this is found, it will probably be appended
	//   to the end.
	printf("Found new environment variable in environ[%d] is: %s\n"
	       , i, environ[i]);
      }
      if (strncmp(environ[i], "HOME", eq_loc) == 0) 
      {
	// This will print the new value for the existing environment
	//   variable.
	printf("Found new value for HOME in environ[%d] is: %s\n"
	       , i, environ[i]);
      }
    }
  }

  // As before, these will also pickup the new environment variable and
  //   the changed value for the existing environment variable.
  new_env = getenv("ENVIRONMENT_TEST");
  if (new_env != NULL) 
  {
    printf("Found new environment variable from getenv(): %s\n", new_env);
  }
  new_env = getenv("HOME");
  if (new_env != NULL) 
  {
    printf("Found the new value of HOME from getenv(): %s\n", new_env);
  }

  // To keep the compiler warning messages at bay.
  return(0);
}
