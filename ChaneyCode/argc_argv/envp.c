/*
 * R Jesse Chaney
 */

// I have put a LOT of comments into this code. If you are not using
//   an editor that make comments stand out (such as by color), this
//   may be difficult to read. In any case, you will be wading through
//   a lot of comments to find a few lines of code. I hope the comments
//   are helpful.

// compile with
//     gcc -Wall -o envp envp.c

#include <stdio.h>  // This is here so I can use printf().
#include <stdlib.h> // For putenv().
#include <string.h> // For strncmp() and strchr().

// Environment variables are sometimes called shell variables. You
//   typically set them in the dot files in your home directory, such 
//   as your .login or .bashrc file.
int
main(
     int argc        // The number of command line arguments.
     , char *argv[]  // An array of strings of the command line arguments

     // Though the envp array is commonly supported on many Unix (and Unix
     //   like) environments, it is not part of the POSIX (SUSv3) standard.
     //   In addition, as we'll see below, the environment in envp is
     //   static, while the actual full environment (accessed through
     //   getenv() and putenv()) need not be static.
     , char *envp[]  // An array of strings for the environment for
                     // the process.
     // The name envp can be read as environment-pointers.
     // The envp variable is rarely used, but it is another possible
     //   parameter to the main() function. It is an array of strings
     //   that contain all the environment variables with which the
     //   process was started.
) 
{
  int i;
  int eq_loc;
  char *eq;
  char *new_env;

  // I'm going to leave out the discussion of argc and argv. If you
  //   want to review those comments of that code, you should download
  //   and look at the argv.c code. This is going to focus on the 
  //   envp variable.
  printf("The value of argc is: %d\n", argc);
  printf("The value of argv[0] is: %s\n", argv[0]);
  for (i = 1; i < argc; i++) 
  {
    printf("\tThe value of argv[%d] is: %s\n", i, argv[i]);
  }
  printf("=======================\n");

  // Now that we've gotten argc and argv out of the way, let's get to
  //   envp.
  // Let's start with the fact that we do not know how many strings
  //   may be in the envp array. There is not an argc like variable
  //   containing that value. We have to loop through the envp array
  //   until we find the NULL at the end. This should look a lot like
  //   the second example (commented out) in the argv.c
  //   code example.
  // Also note that the envp array ends with a NULL pointer, not a
  //  empty string (a NULL string).
  for (i = 0; NULL != envp[i]; i++) 
  {
    printf("\tThe value of envp[%d] is: %s\n", i, envp[i]);
    // When you run this, you might be surprised to see how many
    //   environment variables you have. You might also start to
    //   wonder what they are all for. I'll leave that to you.
    // An individual environment variable looks like this:
    //     SHELL=/bin/bash
    //   This tells me that there is an environment variable called
    //   SHELL which has a value of /bin/bash
    // You can test out this little program by creating a new
    //   environment variable, running the program, and looking for
    //   your new variable. If you are using bash, try this:
    //     export NEW_VARIABLE=new_value
    //     ./envp | grep NEW_VARIABLE
    //   You should see something like this:
    //     The value of envp[37] is: NEW_VARIABLE=new_value
    //   You can then get rid of new variable by doing this (in bash):
    //     unset NEW_VARIABLE
    //     ./envp | grep NEW_VARIABLE
    //   You should no longer see NEW_VARIABLE in the list of
    //   environment variables.
    // I don't recall that I've ever looked through the values in
    //   envp variable. I have called getenv() to fetch an environment
    //   variable value to use, such as "HOME".
  }

  // Now that we've looked at how you can get to the values in the
  //   envp variable, we can check to see if you can make changes to
  //   the environment at run time and have them show up in the envp
  //   variable.
  // The environment array that get passed as envp is static. If 
  //   your process adds a new environment variable, it will not 
  //   show up in the envp variable. If the value of one of the 
  //   environment variables is changed, it will not be reflected in
  //   envp variable.
  //
  // The first call to putenv() will create a new environment variable
  //   for this process.
  // The second call to putenv() will alter the value of the HOME
  //   environment variable, for this process only.
  putenv("ENVIRONMENT_TEST=test_value");
  putenv("HOME=test_value"); // This will change for this process only.
  for (i = 0; NULL != envp[i]; i++) 
  {
    eq = strchr(envp[i], '=');
    if (NULL != eq) 
    {
      eq_loc = eq - envp[i]; // Finding the position in envp[i] where
                             //   the = character occurs. This is some
                             //   tricky C pointer arithmetic. Now,
                             //   eq_loc is the position in envp[i]
                             //   where the = occurs.
      // Now check to see if the environment variable is the new one
      //   we just created above with the putenv() call.
      if (strncmp(envp[i], "ENVIRONMENT_TEST", eq_loc) == 0) 
      {
	// Spoiler alert: you won't find it.
	printf("Found new environment variable in envp[%d] is: %s\n"
	       , i, envp[i]);
      }
      if (strncmp(envp[i], "HOME", eq_loc) == 0) 
      {
	// Spoiler alert: it won't be changed.
	printf("Found HOME in envp[%d] is: %s\n"
	       , i, envp[i]);
      }
    }
  }

  // But, this will find the new environment variable. A call to
  //   getenv() will search the entire environment, not just the 
  //   one passed in the envp variable.
  new_env = getenv("ENVIRONMENT_TEST");
  if (new_env != NULL) 
  {
    printf("Found new environment variable from getenv(): %s\n", new_env);
  }
  // You can also alter the value of an environment variable with a
  //   call to putenv() and fetch the altered value with getenv().
  new_env = getenv("HOME");
  if (new_env != NULL) 
  {
    printf("Found the value of HOME from getenv(): %s\n", new_env);
  }

  // To keep the compiler warning messages at bay.
  return(0);
}
