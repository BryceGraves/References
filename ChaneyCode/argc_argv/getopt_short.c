/*
 * R Jesse Chaney
 */

// I have put a LOT of comments into this code. If you are not using
//   an editor that make comments stand out (such as by color), this
//   may be difficult to read. In any case, you will be wading through
//   a lot of comments to find a few lines of code. I hope the comments
//   are helpful.

// compile with
//     gcc -Wall -o getopt_short getopt_short.c
//
// I recommend that you NOT name the executable program getopt. There is
//   a program called getopt already on your *nix system (probably in
//   /usr/bin). That getopt is used within shell script for processing
//   the command line.

#include <stdio.h>  // This is here so I can use printf().
#include <string.h> // for strcpy()
#include <stdlib.h> // for strtol() and strtod().
#include <unistd.h> // for getopt(), the short options.

// This example is about the short option variant of getopt(). The short
//   option call is standard and part of POSIX. There is another call
//   called getopt_long() that is a GNU extension. It will be covered in
//   another code example.

int
main(
     int argc        // The number of command line arguments.
     , char *argv[]  // An array of strings of the command line arguments
) 
{
  int opt; // The return value from each call to getopt().
  int i_opt = -1;
  int o_opt = 0;
  double d_opt = 0.0;
  char s_opt[100] = "";

  // When you are going to use getopt(), you need to let it know what you
  //   are going to use as legal command line options and which ones
  //   may have a required argument to the option. If a command line
  //   option is going to have a required argument, it is followed with
  //   a colon (:) in the optstring. If you neglect to pass an argument
  //   with a command line option that is listed in the optstring as needing
  //   one, getopt() will generate a warning message such as:
  //     ./getopt_short: option requires an argument -- 'd'
  // If you pass a command line option that is not listed in the optstring,
  //   getopt() will issue a warning message, such as:
  //     ./getopt_short: invalid option -- 'z'
  // When you define the optstring, you do not put any - in with the characters
  //   for the options, just the option letter followed by an optional colon.
  // The optstring below should be read as:
  //   a command line option -o, that has no command line argument.
  //   a command line option -s that does have a required argument.
  //   a command line option -i that does have a required argument.
  //   a command line option -d that does have a required argument.
  while ((opt = getopt(argc, argv, "os:i:d:")) != -1) 
  {
    switch (opt) 
    {
    case 'o':
      // This is just a basic command line option that lets you know
      //   if it is on the command line. You can use it as an on/off 
      //   value or you can use as it I've done here to count how many 
      //   times it has occurred on the command line.
      //   If you start the program as:
      //     ./getopt_short -o -o -o
      //   or as
      //     ./getopt_short -ooo
      //   the program will count the number of times that the -o
      //   option was on the command line. Use like this comes in
      //   handy when you want to have a debug value that you can use
      //   with multiple levels of debug.
      // This demonstrates one of the great things about getopt(), it
      //   is already able to handle command lines that have the options
      //   next to each other.
      o_opt++; // Increment the variable each time the -o option is seen.
      printf("The -o option has been seen: %d\n", o_opt);
      break;
    case 's':
      // This option has a required argument. When getopt() has finds a
      //   option with an argument, the argument is put into the special
      //   variable optarg.
      // The line below copies the argument to the -s option into the
      //   the char variable s.
      strcpy(s_opt, optarg);
      printf("The -s option has been seen with argument %s\n", s_opt);
      break;
    case 'i':
      // This option also has a required argument. All arguments parsed with
      //   getopt() come in as strings. This argument is intended to be an 
      //   integer, so we must convert the optarg value from a string to an
      //   integer, I use the strtol() call (atoi() could be used).
      i_opt = (int) strtol(optarg, NULL, 10);
      printf("The -i option has been seen with argument %d\n", i_opt);
      break;
    case 'd':
      // This option has a required argument that will be converted from a 
      //   string into a double data type.
      d_opt = strtod(optarg, NULL);
      printf("The -d option has been seen with argument %f\n", d_opt);
      break;
    default:
      // This will show up if you try and use something with a - in front
      //   of it that is not listed in the optstring.
      printf("something strange has happened\n");
      break;
    }
  }

  ///////////////////////////////////////////////////////////////////////////
  // A this point, getopt() has processed all of the options on the command
  //   line, but that may not be everything that is on the command line. There
  //   could be additional items on the command line that are not used as 
  //   switches. An example of when something like this can happen is with
  //   the ls command:
  //     ls -l argv.c environ.c envp.c getopt_short.c
  //   Everything following the -l command line option are still contents of
  //   argv, but getopt() won't process them.
  // We can still process the remainder of the command line. The code below
  //   simply loops over anything remaining on the command line that getopt()
  //   has left behind.
  // The optind variable is the position in argv just past where getopt()
  //   stopped processing command line options. As it says in the man page
  //   for getopt():
  //     If there are no more option characters, getopt() returns -1. Then 
  //     optind is the index in argv of the first argv-element that is not 
  //     an option.
  // When optind (option index) is less than argc, it means that there are
  //   still come things on the command line. If optind is equal to argc, it
  //   means that getopt() processed everything on the command line and there
  //   is nothing left to process from it.
  if (optind < argc) 
  {
    int j;

    fprintf(stderr, "\nThis is what remains on the command line:\n");
    for(j = optind; j < argc; j++) 
    {
      // If you want to do something with these values on the command line
      //   you'll need to use something other than getopt().
      printf("\t%s\n", argv[j]);
    }
  }
  // If you run this program with the following command line:
  //     Has a space between option and argument
  //                                |    Does not have a space
  //                                V    V
  //     ./getopt_short -ooos junk -i 2 -d3.6 blaa1 blaa2 blaa3
  //                     ^                   ^^^^^^^^^^^^^^^^^
  //                     |                   getopt() does not process these.
  //                     the -o option is processed 3 times by getopt().
  //   You will see this output:
  //     The -o option has been seen: 1
  //     The -o option has been seen: 2
  //     The -o option has been seen: 3
  //     The -s option has been seen with argument junk
  //     The -i option has been seen with argument 2
  //     The -d option has been seen with argument 3.600000
  //
  //     This is what remains on the command line:
  //     	blaa1
  //     	blaa2
  //     	blaa3
  //
  // One of the things you can notice about getopt() is that it is able to
  //   handle multiple different ways that options and arguments can have
  //   spaces between them (or not).

  // If you pass an argument to to option (that requires an argument) which
  //   begins with a -, getopt() will still consider it a valid argument for
  //   the option. For example:
  //     ./getopt_short -s -g
  //   The argument for the -s option will have the value -g. Whatever follows
  //   the option is considered the argument.

  // What happens when you want to pass some items on the command line that
  //   start with a -, but that are not command line options. For example:
  //     ./getopt_short -o -z
  //   I want to pass the -z value on the command line, but do not want 
  //   getopt() to process it. The special way you can tell getopt() to
  //   stop processing the command line is with the -- characters:
  //     ./getopt_short -o -- -z
  //   When getopt() sees the -- on the command line, it stops processing
  //   and leaves the rest for you to process.
  //     ./getopt_short -o -- -z blaa1
  //   will show:
  //     The -o option has been seen: 1
  //
  //     This is what remains on the command line:
  //     	-z
  //    	blaa1

  // To keep the compiler warning messages at bay.
  return(0);
}
