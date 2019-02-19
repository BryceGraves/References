/*
 * R Jesse Chaney
 */

// I have put a LOT of comments into this code. If you are not using
//   an editor that make comments stand out (such as by color), this
//   may be difficult to read. In any case, you will be wading through
//   a lot of comments to find a few lines of code. I hope the comments
//   are helpful.

// compile with
//     gcc -Wall -o getopt_long getopt_long.c
//
// I recommend that you NOT name the executable program getopt. There is
//   a program called getopt already on your *nix system (probably in
//   /usr/bin). That getopt is used within shell script for processing
//   the command line.

#include <stdio.h>  // This is here so I can use printf().
#include <string.h> // for strcpy()
#include <stdlib.h> // for strtol() and strtod().
#include <getopt.h> // for getopt() with long options

// This code example is about the long option variant of getopt, getopt_long().
//   The getopt_long() call is a GNU extension to the POSIX standard getopt().
//   The short option call for getopt is covered in another code sample.
// Sometimes you just run out of single letter command line options for your
//   program or you want to use some longer named options that are a bit more
//   meaningful. Who wants to use the -Q command line option to mean "enable
//   logging"? That is when you want to pull out the next level in command line
//   processing, the long options. Use of long options gives you a lot more
//   capability, but it is also quite a bit more complex.
// Finding a good and complete example of how to use the many forms of 
//   getopt_long() was a lot more difficult than I thought it would be.
//   Hopefully I've given a good and (mostly) complete sample here.
// Simply put, using getopt_long() allows you to specify either or both short
//   form command line options or long form command line options. Short form
//   options begin with a single dash/hyphen character (-). Long form options
//   begin with double dashes/hyphens (--).
//   Example
//     ./getopt_long -s --long

int
main(
     int argc        // The number of command line arguments.
     , char *argv[]  // An array of strings of the command line arguments
) 
{
  int long_index = 0;
  int i_opt = -1;
  int opt; // The return value from each call to getopt_long().
  int o_opt = 0;
  double d_opt = 0.0;
  char s_opt[100] = "";
  // A call to getopt_long() requires a structure that provides quite a bit
  //   more detail about how the options to the program are parsed. The
  //   structure is defined in getopt.h as:
  //     struct option {
  //       const char  *name;
  //       int         has_arg;
  //       int         *flag;
  //       int         val;
  //     }
  //  The fields in the structure are defined as follows.
  //    name is simply the name of the option, in its long form. You should 
  //      not put a -- in front of the name.
  //    has_arg is a the value no_argument (0), required_argument (1), or 
  //      optional_argument (2). The optional_argument is not something that
  //      the short form of getopt() supports. The getopt_long() call does
  //      allow you to have an optional argument to a command line option.
  //      While the numeric values work, the symbolic constants are 
  //      considerably easier to read, and you should use them.
  //    flag allows you to determine how getopt_long() will return the
  //      the result.
  //      If flag is 0 (NULL), then getopt_long returns the val field
  //        (the next parameter), which can be used as a case in the switch
  //        statement.
  //      If flag is non-zero, then getopt_long() returns 0 and flag is a 
  //        pointer to a variable which is set to the val.
  //    val is the value to return or load into the variable pointed to by
  //      the flag parameter.
  int int_val = 0;
  int char_val = 'a';
  struct option long_options[] = {
    // For the most part, these behave like their counterparts in the short
    //   option call of getopt(). You can also identify them with the long
    //   form of the command line.
    // Examples of how these can be used are:
    //     ./getopt_long -o --option -oo
    //       The above command will process the -o option 4 times.
    //     ./getopt_long -s str1 --string str2 --string=str3 -sstr4
    //       Note that you cannot use -s=str5. The equal sign between the
    //         the option and the argument can only be used with the long
    //         option form.
    //     ./getopt_long -i 2 --integer 3 --integer=4 -i5
    {"option",  no_argument,       NULL, 'o'},
    {"string",  required_argument, NULL, 's'},
    {"integer", required_argument, NULL, 'i'},
    {"double",  required_argument, NULL, 'd'},
    //                                    ^
    //                                 These all show up in the optstring
    //                                 parameter for getopt_long().

    // The long options listed below do not show up in the optstring
    //   parameter for getopt_long() (as indicated by the val field being
    //   0.
    // Examples of how these can be used are:
    //     ./getopt_long --no-arg --with-arg 17
    //     ./getopt_long --no-arg --with-arg=18
    //
    {"no-arg",    no_argument      , NULL, 0},
    {"with-arg",  required_argument, NULL, 0},

    // Don't put anything in the val field for this one. You'd need to
    //   include it in the optstring parameter for getopt_long, but with or
    //   without the colon? Truth is, don't put anything in the val field
    //   and don't put it in the optstring parameter to getopt_long().
    // This is another nice way to have a debug flag as a command line
    //   option. If the option is present on the command line without
    //   an argument, it can take on a default value of one. Or, you can
    //   specify a value greater than one.
    // When you specify a command line option was having an optional argument
    //   you must specify its argument using the equal (=) sign syntax when you
    //   do want to have an argument.
    // Examples of how these can be used are:
    //     ./getopt_long --debug-arg --debug-arg=4
    //
    // The command below DOES NOT work. You need to have the equal sign between
    //   the option and its argument when the argument is optional.
    //     ./getopt_long --debug-arg 3
    //
    {"debug-arg",  optional_argument, NULL, 0}, 

    // If you use this form (with a pointer in the flag field), you 
    //   cannot make the structure a static variable. The compiler
    //   will (should) warn that the flag field is not a constant.
    // One thing that is kind of neat about this form is that you don't
    //  need to process it through the switch statement at all. The value
    //  of val will be pushed into the flag variable without you needing
    //  to do anything. In the switch statement below, I just print out
    //  when the switch is seen and the value of int_val each time.
    // An example of how this can be used is:
    //     ./getopt_long --set-val --unset-val --set-val
    //
    {"set-val",  no_argument, &int_val, 1},
    {"unset-val",  no_argument, &int_val, 0},

    // An example of how this can be used is:
    //     ./getopt_long --set-char-a --set-char-b --set-char-a
    //
    {"set-char-a",  no_argument, &char_val, 'a'},
    {"set-char-b",  no_argument, &char_val, 'b'},

    {NULL, 0, NULL, 0} // This terminates the structure
  };

  // You don't want to have anything in the optstring that is not part
  //   of the long option definition. For example, if I put a "u" in the
  //   optstring and don't account for it in one of the long options (as
  //   is done with the -o and -i options), odd things may occur.
  while ((opt = getopt_long(argc, argv, "os:i:d:"
			    , long_options, &long_index)) != -1) 
  {
    switch (opt) 
    {
    case 0: // A long option without a short representation.
      // All the long option command line options that do not have a short
      //   option representation will fall into this case of the switch
      //   statement. You'll need to break down what to do based on the
      //   index into the long options structure or the name field.
      printf("\tLong command line option \"%s\"", long_options[long_index].name);
      if (NULL != optarg) 
      {
	printf(" with argument %s", optarg);
      }
      else
      {
	printf(" with no argument");
      }
      if ((strcmp(long_options[long_index].name, "set-val") == 0)
	  || (strcmp(long_options[long_index].name, "unset-val") == 0)) 
      {
	// Notice that I do not need to make any assignment to the int_val
	//   variable for it value to be updated.
	printf(" the int_val is %d", int_val);
      }
      if ((strcmp(long_options[long_index].name, "set-char-a") == 0)
	  || (strcmp(long_options[long_index].name, "set-char-b") == 0)) 
      {
	// This is a little dangerous.
	// Notice that I do not need to make any assignment to the char_val
	//   variable for it value to be updated.
	printf(" the char_val is %c", (char) char_val);
      }
      printf("\n");
      break;
    case 'o':
      // This case gets called when the command line contains either or both
      //   an -o or an --option command line option.
      o_opt++; // Increment the variable each time the -o/--option option 
               //   is seen.
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
      // This will show up if you try and use something with a - in front
      //   of it that is not listed in the optstring.
      printf("something strange has happened\n");
      break;
    }
  }

  ///////////////////////////////////////////////////////////////////////////
  // Just with the use of getopt() (with short options), the remainder of
  //   the command line past the command line options can be processed at
  //   this point. I went into detail about this in the getopt_short.c code
  //   example.
  // However, the getopt_long() call is a bit more versatile when it comes
  //   to recognizing that something on the command line is not a command
  //   option. For example, the following command will actually pull out
  //   only the command line options that are listed in the command line
  //   options call to getopt_long() and will leave everything else for
  //   this section of the code. A call to getopt_long() will do some
  //   rearranging of the command line.
  //     ./getopt_long not-an-option --no-arg na --with-arg=18 2 3 --set-val 17
  //   The result of the above command is:
  //    	Long command line option "no-arg" with no argument
  //    	Long command line option "with-arg" with argument 18
  //    	Long command line option "set-val" with no argument the int_val is 1
  //
  //    This is what remains on the command line:
  //    	not-an-option
  //    	na
  //    	2
  //    	3
  //    	17
  //
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

  // To keep the compiler warning messages at bay.
  return(0);
}
