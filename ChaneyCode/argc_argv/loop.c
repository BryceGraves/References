/*
 * R Jesse Chaney
 */

// I have put a LOT of comments into this code. If you are not using
//   an editor that make comments stand out (such as by color), this
//   may be difficult to read. In any case, you will be wading through
//   a lot of comments to find a few lines of code. I hope the comments
//   are helpful.

// compile with
//     gcc -Wall -o loop loop.c

#include <stdio.h>  // This is here so I can use printf().
#include <stdio.h>  // This is here so I can use printf().
#include <stdlib.h> // for strtol()

// One of the constructs you'll see in these code samples is an infinite
//   loop, or seemingly infinite. I often use that construct when processing
//   an unknown amount of data. You basically have to do something like that
//   when you don't know how much data you need to process before the loop
//   reaches termination.
// My preferred code to implement an infinite loop is:
//     for( ; ; ) { ... }
//   I know there are other ways, specifically:
//     while( 1 ) { ... }
//   but I prefer the for loop. I've never looked at the assembler output to
//   see if the compiler generates the same code or if there is any difference
//   in run time. The difference should be very slight if any.
// It would be possible to create a macro like
//     #define FOREVER for(;;)
//   but that is ugly, abusive of cpp, and would bring you demerits.

int
main(
     int argc        // The number of command line arguments.
     , char *argv[]  // An array of strings of the command line arguments
) 
{
  long val;     // This is the integer value that the user types. Remember that
                //   the fgets() function returns only strings. We need to
                //   convert that string into an integer.
  long sum = 0; // This is the sum of all the numbers typed in.
  char buf[80]; // This is the buffer that is used to read the user input.
  char *ch;     // This is the return value from fgets() to know if we've
                //   come to the end of the input.

  // I'm simply creating an infinite loop that will read input from the
  //   user, convert each line of the user input into an integer, and
  //   keep a sum of the integer input from the user.
  // The loop will be terminated when the user indicates end of input by
  //   either entering a 0 or by typing a control-D (for end of input).
  // As I said in the beginning of the code sample, I like to use the
  //   for ( ; ; ) { ... } to represent an infinite loop.
  for ( ; ; ) 
  {
    // Read the user input (from the keyboard) into the buffer buf.
    ch = fgets(buf, sizeof(buf), stdin);

    // This is the first way that we can determine if we've reached the
    //   termination point of the input. If the user types a control-D, it
    //   means the end of input (or end of file). In that case, fgets() will
    //   return a NULL, which we check and break if seen.
    if (NULL == ch) 
    {
      // The call to break will terminate the nearest enclosing loop. In this
      //   case, we only have a single enclosing loop.
      break;
    }

    // Convert the string entered by the user into a base 10 integer. We are
    //   not doing very much error checking here. If the input given by the
    //   user is a string such as "bogus", the call to strtol() will return a 
    //   0, which will cause the process to exit.
    val = strtol(buf, NULL, 10);

    // If the user has entered a 0 as a value, then we are also going to
    //   exit the loop.
    if (0 == val) {
      // Break out of the loop.
      break;
    }

    // Add the value entered by the user into the running sum we are keeping.
    sum += val;
  }

  // Now that we've exited the loop, we just print the sum of the given
  //   values and exit.
  printf("the sum of the input numbers is %ld\n", sum);

  // To keep the compiler warning messages at bay.
  return(0);
}
