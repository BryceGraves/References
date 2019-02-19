// Jesse Chaney

#include <fcntl.h>     // file control
#include <unistd.h>    // POSIX header
#include <stdlib.h>    // needed for the prototype for exit()
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "rw_struct.h"

int 
main(
     int argc       // The number of command line arguments.
     , char *argv[] // An array of strings of the command line arguments.
) {
  simple_struct_t simple_var;
  int iFd = -1;
  ssize_t bytes_read = -1;
  unsigned short count = 0;
  off_t total_bytes = 0;

  memset(&simple_var, 0, sizeof(simple_var));

  iFd = open(BIN_FILENAME, O_RDONLY);

  if (argc > 1) {
    count = strtol(argv[1], NULL, 10);
    if (count > NUM_TO_WRITE) {
      fprintf(stderr, "Binary file only has %d records.\n"
	      "  Cannot seek %d records into file\n", NUM_TO_WRITE, count);
      exit(1);
    }

    total_bytes = lseek(iFd, count * sizeof(simple_struct_t), SEEK_SET);

    bytes_read = read(iFd, &simple_var, sizeof(simple_var));

    printf("record %d  offset into file %u\n", count++, (unsigned int) total_bytes);
    printf("  the rec num     is %s\n", simple_var.record_num);
    printf("  the char        is '%c'\n", simple_var.single_char);
    printf("  the str         is \n%s\n", simple_var.multi_char);
    printf("  the int         is %d\n", simple_var.an_int);
    printf("  the long double is %.4Lf\n", simple_var.a_double);
    
  }
  else {
    if (iFd >= 0) {
      while ((bytes_read = read(iFd, &simple_var, sizeof(simple_var))) > 0) {

	// This is really cheating. I'm simply blasting a null char into the last
	// character of the ARRAY of char. I might be STOMPING on a very good
	// character. There are betters ways to manage this without stomping on
	// the last character so meanly.
	// I __SHOULD__ do the right thing and excise all the trailing spaces
	// from the field multi_char. But, in this case, I forgive myself for
	// being lazy.
	printf("record %d  offset into file %u\n", count++
	       , (unsigned int) total_bytes);
	printf("  the rec num is %s\n", simple_var.record_num);
	printf("  the char    is '%c'\n", simple_var.single_char);
	printf("  the str     is \n%s\n", simple_var.multi_char);
	printf("  the int     is %d\n", simple_var.an_int);
	total_bytes += bytes_read;
	memset(&simple_var, 0, sizeof(simple_var));
      }
    }
  }
  close(iFd);

  return(EXIT_SUCCESS);
}
