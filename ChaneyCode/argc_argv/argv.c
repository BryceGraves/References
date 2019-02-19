/*
 * R Jesse Chaney
 */

// I have put a LOT of comments into this code. If you are not using
//   an editor that make comments stand out (such as by color), this
//   may be difficult to read. In any case, you will be wading through
//   a lot of comments to find a few lines of code. I hope the comments
//   are helpful.

// compile with
//     gcc -Wall -o argv argv.c

#include <stdio.h> // This is here so I can use printf().

// You can think of the command line allowing you to pass parameters
//   to the main() function in your code.

// There is actually nothing magic about the names argc and argv. You
//   could call them yin and yang or Coke and Pepsi. However, argc and
//   argv are the normal names and other folks reading your code would
//   find the use of other names a bit confusing.
int
main(
     int argc        // The number of command line arguments.
     // The meaning of argc is argument-count, abbreviated to argc.

     , char *argv[]  // An array of strings of the command line arguments.
     // The meaning of argv is argument-vector, abbreviated to argv.
     //   All the command line arguments get bundled up into the argv
     //   variable and passed to your main() function.
     //
     // You could also define the argv variable as char **argv. I
     //   prefer to use char *argv[] because to me it better shows what
     //   argv is really meant to be, an array/vector of strings.
     //   You cannot use char argv[][] though.
) 
{
  // This is a simple example of how C provides you with the
  //   command line arguments. An example of command line arguments
  //   is:
  //      argv argument1 argument2 argument3
  //
  // In the above example, argument1 argument2 argument3 are all
  //   called command line arguments for the program argv.
  //   Each of the command line arguments will be represented in the
  //   array argv.
  // Command line arguments are separated from each other on the command
  //   line by at least 1 space or tab (it could be more). If you want to
  //   pass an argument with a space in it, you need to put it inside
  //   single quotes ('') or double quotes ("").

  int i;

  // First, let's print the value of argc, the count of the number
  //   of command line arguments.
  printf("The value of argc is: %d\n", argc);

  // The initial value of argv is argv[0] (remember that arrays in
  //   C start at index 0). is the name of the program, which may 
  //   include the path to the program. A program run from the 
  //   command line will always have at least 1 command line 
  //   argument, the name of the program as argv[0].
  //   Occasionally, the name of the program is not available when
  //   the program is started and argv[0] is just an empty string. I've
  //   never experienced having argv[0] == NULL though.
  // Below I simply print the initial value of the argv variable,
  //   argv[0].
  printf("The value of argv[0] is: %s\n", argv[0]);

  // Now we can check to see if there are other values on the command
  //   line. If argc is 1, it means that there is nothing on the command
  //   line past the name of the program. I usually treat argv[0] as
  //   special, since it is the name of the program.
  for (i = 1; i < argc; i++) 
  {
    printf("\tThe value of argv[%d] is: %s\n", i, argv[i]);
  }
  // If you run this program as:
  //     ./argv argument1 argument2 argument3
  //   it will print
  //     The value is argc is: 4
  //     The value of argv[0] is: ./argv
  //     The value of argv[1] is: argument1
  //     The value of argv[2] is: argument2
  //     The value of argv[3] is: argument3
  // One thing to notice is that the path to the program is
  //   in front of the program name. In this case, the path
  //   is just ./
  // You can put as many or few arguments on the command line as you
  //   like with this example. I'm sure there is an upper limit
  //   somewhere for the number of command line arguments, but it
  //   is going to be pretty high.

  // It is important to recognize that argv is an array of _strings_.
  //   So, if you run the program as:
  //     ./argv 1 2 3
  //   it will print
  //     The value is argc is: 4
  //     The value of argv[0] is: ./argv
  //     The value of argv[1] is: 1
  //     The value of argv[2] is: 2
  //     The value of argv[3] is: 3
  // Though the values 1, 2, and 3 look like numbers, they are
  //   represented in argv as strings. If you want to turn them into
  //   numbers, you need to convert them, using strtol() or atoi()
  //   and store them in integer typed variables.

  // Another interesting note is that the last value of argv is
  //   actually NULL. Notice that this is the the NULL pointer,
  //   not an empty string. The NULL is in the argv
  //   array just past the last command line argument. So, the
  //   the loop from above could be rewritten as:
  //
  // for (i = 1; NULL != argv[i]; i++) {
  //   printf("\tThe value of argv[%d] is: %s\n", i, argv[i]);
  // }
  //
  // As it turns out, argv[argc] == NULL. Not that it is very useful,
  //   but it is true.

  // To keep the compiler warning messages at bay.
  return(0);
}
