/*
 * R Jesse Chaney
 */

// I have put a LOT of comments into this code. If you are not using
//   an editor that make comments stand out (such as by color), this
//   may be difficult to read. In any case, you will be wading through
//   a lot of comments to find a few lines of code. I hope the comments
//   are helpful.

// compile with
//     gcc -Wall -o simple_seek simple_seek.c

#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

// In this example, we kind of use BLOCKSIZE to represent the number of 
//   characters on a line in the file. Don't forget to count the new-line 
//   character on each line. The default example uses a file with a 2 digit 
//   number on each line (with a new-line character), so we set the BLOCKSIZE
//   to 3. If you want to step backwards in the file 1 byte at a time, set
//   BLOCKSIZE to 1. The output will juxtapose the digits in the the input
//   file.
#define BLOCKSIZE 3

int 
main(
     int argc
     , char *argv[]
) {
  char *input_name;
  ssize_t tot_read = 0;
  int in_fd;            // The file descriptor for the input file.
  off_t loc;
  struct stat sb;       // We stat the file to get its size.

  if (argc < 2) {
    fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  input_name = argv[1];

  // Open the input file, as read-only.
  in_fd = open(input_name, O_RDONLY);
  if (-1 == in_fd) {
    perror("Cannot open input file.");
    exit(1);
  }

  // Find out how many bytes are in the input file. Do a fstat() on the file
  //   to find its size.
  if (-1 == fstat(in_fd, &sb)) {
    perror("fstat failed");
    exit(3);
  }

  /* loc = lseek(in_fd, -10, SEEK_SET); */
  /* if (((off_t) -1) == loc) { */
  /*   perror("lseek SEEK_SET -10 failed"); */
  /* } */
  /* exit(17); */

  // Seek into the file. We've set the whence parameter to SEEK_CUR.
  loc = lseek(in_fd, (3 * BLOCKSIZE), SEEK_CUR);
  if (((off_t) -1) == loc) {
    perror("lseek 1 failed");
    exit(4);
  }

  fprintf(stderr, "stat file size: %d  seek bytes: %d\n"
	  , (int) sb.st_size
	  , (int) loc);

  for ( ; ; ) {
    ssize_t num_read;
    ssize_t num_written;
    char buf[BLOCKSIZE];

    // Read BLOCKSIZE number of bytes from the input file.
    num_read = read(in_fd, buf, BLOCKSIZE);
    if (((size_t) -1) == num_read) {
      perror("read failed");
      exit(6);
    }
    
    if (((size_t) 0) == num_read) {
      // We've reached the end of the file. Break out of the loop.
      break;
    }

    // Now that we've read some bytes from the input file, it is time to 
    //   write bytes to the output file.
    num_written = write(STDOUT_FILENO, buf, num_read);
    if (((size_t) -1) == num_written) {
      perror("write failed");
      exit(7);
    }
  }

  // seek back to the beginning of the file.
  lseek(in_fd, 0, SEEK_SET);
  for ( ; ; ) {
    ssize_t num_read;
    ssize_t num_written;
    char buf[BLOCKSIZE];
    
    num_read = read(in_fd, buf, BLOCKSIZE);
    if (((size_t) -1) == num_read) {
      perror("read failed");
      exit(7);
    }
    if (((size_t) 0) == num_read) {
      // We've reached the end of the file. Break out of the loop.
      break;
    }

    tot_read += num_read;
    num_written = write(STDOUT_FILENO, buf, num_read);
    if (((size_t) -1) == num_written) {
      perror("write failed");
      exit(7);
    }
    // Once we've read the number of bytes to get us back to where
    // we jumped into the file with the first lseek(), we can break.
    if (tot_read >= loc) {
      break;
    }
  }
  
  close(in_fd);
  //fprintf(stderr, "Success\n");
  return(EXIT_SUCCESS);
}
