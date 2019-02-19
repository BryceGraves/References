/*
 * R Jesse Chaney
 */

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
// I need to check this on big and little endian systems, with
//   both 32 bit and 64 bit.
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

// I have put a LOT of comments into this code. If you are not using
//   an editor that make comments stand out (such as by color), this
//   may be difficult to read. In any case, you will be wading through
//   a lot of comments to find a few lines of code. I hope the comments
//   are helpful.

// compile with
//     gcc -Wall -o sizeof sizeof.c

#include <stdio.h>
#include <values.h>
#include <limits.h>

// This will just print a few bits of information about the system it
//   is compiled and run on.

int 
main(
     int argc
     , char *argv[]
) {
  printf("\n");
  // The __WORDSIZE macro is not 100% guaranteed to be correct.
  printf("This appears to be a %d bit system.\n", __WORDSIZE);

  {
    // There are other ways to accomplish this. It does not check for other
    //   things like middle endian, odd endian, or obtuse endian.
    // Most of the time, the compiler will take care the the endianess for
    //   you. However, in some network programming, endianess becomes
    //   very important.
    //
    // Inspired from:
    //   http://www.ibm.com/developerworks/aix/library/au-endianc/
    //   http://www.geeksforgeeks.org/little-and-big-endian-mystery/

    // This needs to be unsigned.
    unsigned int i = 1;
    // The variable c is a character pointer to the first byte of the integer
    //   value i.  We'll use this when we check for big/little endian.
    char *c = (char *) &i;

    if (0 != (*c)) {
      // If the contents of the MSB is non-zero, this is
      // a little endian architecture.
      printf("\tThis system is little endian.\n");
    }
    else {
      printf("\tThis system is big endian.\n");
    }
  }
  printf("\n");

  // The sizeof() operator returns the number of bytes a type (or struct)
  //   requires. The return type of the sizeof() operator is a size_t, so
  //   you need to cast it to print it (without warnings from the compiler).
  //
  // The <blaa>BITS values come from the values.h include file. They should
  //   let you know how many bits certain types require. Not all types have
  //   a <blaa>BITS macro in values.h though.
  printf("Size of basic C data types\n");
  printf("                              bytes   bits\n");
  printf("\tsizeof(char)\t\t%2d    %3d\n", (int) sizeof(char)
	 , (int) CHARBITS);
  printf("\tsizeof(short)\t\t%2d    %3d\n", (int) sizeof(short)
	 , (int) SHORTBITS);
  printf("\tsizeof(int)\t\t%2d    %3d\n", (int) sizeof(int)
	 , (int) INTBITS);
  printf("\tsizeof(unsigned)\t%2d    %3d\n", (int) sizeof(unsigned)
	 // This is not directly supported by a nice macro, so I'm
	 //   calculating it.
	 , (int) (sizeof(unsigned) * CHARBITS));
  printf("\tsizeof(long)\t\t%2d    %3d\n", (int) sizeof(long)
	 , (int) LONGBITS);
  printf("\tsizeof(long long)\t%2d    %3d\n", (int) sizeof(long long)
	 // This is not directly supported by a nice macro, sir I'm
	 //   calculating it.
	 , (int) (sizeof(long long) * CHARBITS));
  printf("\tsizeof(__int128_t)\t%2d    %3d (gcc special 128 bit integer)\n", (int) sizeof(__int128_t)
	 // This is not directly supported by a nice macro, so I'm
	 //   calculating it.
	 , (int) (sizeof(__int128_t) * CHARBITS));

  printf("\tsizeof(float)\t\t%2d    %3d\n", (int) sizeof(float)
	 , (int) FLOATBITS);
  printf("\tsizeof(double)\t\t%2d    %3d\n", (int) sizeof(double)
	 , (int) DOUBLEBITS);
  printf("\tsizeof(long double)\t%2d    %3d\n", (int) sizeof(long double)
	 // This is not directly supported by a nice macro, sci I'm
	 //   calculating it.
	 , (int) (sizeof(long double) * CHARBITS));
  printf("\tsizeof(void *)\t\t%2d    %3d (aka a pointer)\n"
	 , (int) sizeof(void *), (int) PTRBITS);

  if (argc < 2) {
    // If you put something on the command line after the program name
    //   (anything), then this chunk of code will be skipped. It is just
    //   a bit ugly when it prints and wraps around a couple times on
    //   the screen.
    printf("\n");
    printf("The lower/upper limit of some base C types (from values.h)\n");
    printf("\tMINSHORT\t\t%d\n", MINSHORT);
    printf("\tMININT\t\t\t%d\n", MININT);
    printf("\tMINLONG\t\t\t%ld\n", MINLONG);
    printf("\tMAXSHORT\t\t%d\n", MAXSHORT);
    printf("\tMAXINT\t\t\t%d\n", MAXINT);
    printf("\tMAXLONG\t\t\t%ld\n", MAXLONG);

    printf("\tULONG_MAX\t\t%lu\n", ULONG_MAX);
    printf("\tULLONG_MAX\t\t%llu\n", ULLONG_MAX);

    printf("\n");
    printf("\tMINFLOAT\t\t%f\n", MINFLOAT);    // Turns out, this is boring.
    printf("\tMINDOUBLE\t\t%lf\n", MINDOUBLE); // So is this.

    printf("\tMAXFLOAT\t\t%f\n", MAXFLOAT);
    // This is pretty darn long.  It will probably wrap a couple
    //   lines.
    printf("\tMAXDOUBLE\t\t%lf\n", MAXDOUBLE);
  }

  // I'm not using the EXIT_SUCCESS macro here because I'd need to have 
  //   another include file and I want to make this as simple as possible.
  return(0);
}
