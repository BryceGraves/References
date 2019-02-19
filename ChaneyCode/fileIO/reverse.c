/*
 * R Jesse Chaney
 */

// I have put a LOT of comments into this code. If you are not using
//   an editor that make comments stand out (such as by color), this
//   may be difficult to read. In any case, you will be wading through
//   a lot of comments to find a few lines of code. I hope the comments
//   are helpful.

// compile with
//     gcc -Wall -o reverse reverse.c

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
  char *output_name;

  int in_fd;            // The file descriptor for the input file.
  int out_fd;           // The file descriptor for the output file.
  off_t file_size;
  off_t bytes_copied;
  off_t file_offset;
  struct stat sb;       // We stat the file to get its size.

  // Make sure the command line contains 2 file names.  The first file
  //   name is the name of the input file and the second file name is
  //   the name of the output file.
  if (argc < 3) {
    fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
    fprintf(stderr, "  Try: %s numbers_10.txt rev_numbers.txt\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  // Take the 2 files given on the command line and give them names that
  //   are a bit easier than argv[1] and argv[2].
  input_name = argv[1];
  output_name = argv[2];

  // Open the input file, as read-only.
  in_fd = open(input_name, O_RDONLY);
  if (-1 == in_fd) {
    perror("Cannot open input file.");
    exit(1);
  }

  // Open the output file, as write-only. If it does not exist, create it.
  //   If it does exist, truncate it to zero bytes.
  // Set the file's mode to read and write for user (owner) only. All the other
  //   mode bits are disabled.
  out_fd = open(output_name
		, (O_WRONLY | O_CREAT | O_TRUNC)
		, (S_IRUSR | S_IWUSR));
  if (-1 == out_fd) {
    perror("Cannot open output file.");
    exit(2);
  }

  // Find out how many bytes are in the input file. Do a fstat() on the file
  //   to find its size.
  if (-1 == fstat(in_fd, &sb)) {
    perror("fstat failed");
    exit(3);
  }

  // Seek to the end of the file. We've set the whence parameter to SEEK_END.
  //   This will move the file pointer to just past the last byte in the file.
  file_size = lseek(in_fd, 0, SEEK_END);
  if (((off_t) -1) == file_size) {
    perror("lseek 1 failed");
    exit(4);
  }

  // In this case, the result of SEEK_END is also how many bytes are in the 
  //   file. However, that will not always be the case. In this case, we could
  //   have skipped calling the fstat() on the file.
  fprintf(stderr, "stat file size: %d  seek bytes: %d  block size: %d\n"
	  , (int) sb.st_size
	  , (int) file_size
	  , BLOCKSIZE);

  // Seek __backwards__ BLOCKSIZE bytes in the input file.
  // We need to get to the beginning of where we want to start to read in the
  //   file. We use the whence parameter to seek the end of the file, then 
  //   backup BLOCKSIZE bytes.
  file_offset = lseek(in_fd, (BLOCKSIZE * -1), SEEK_END);
  if (((off_t) -1) == file_offset) {
    perror("lseek 2 failed");
    exit(5);
  }

  bytes_copied = 0;
  // Now loop through the file, in reverse. There is a check on the number of
  //   bytes written that will be our clue as to when to break out of the loop.
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
    // Now that we've read some bytes from the input file, it is time to 
    //   write bytes to the output file.
    num_written = write(out_fd, buf, BLOCKSIZE);
    if (((size_t) -1) == num_written) {
      perror("write failed");
      exit(7);
    }

    // If the number of bytes read is not equal to the number of bytes 
    //   written, we have a problem.
    if (num_read != num_written) {
      perror("Bytes written not equal to bytes read.");
      // Since there was a problem writing the output file, delete it.
      unlink(output_name);  // This is how you delete a file.
      exit(8);
    }

    // We need to keep track of how many total bytes we've written.
    bytes_copied += num_written;
    
    // Check to see if we've read backwards all the way back to the the
    //   beginning of the file. You don't want to try to seek backwards further
    //   than the beginning of the file. It will cause an error.
    if (bytes_copied >= file_size) {
      // If we've copied out all the bytes of the file, break out of the loop.
      fprintf(stderr, "Back to the beginning of the file: %d\n"
	      , (int) bytes_copied);
      // We not break out of the for() loop.
      break;
    }
    // Seek __backwards__ in the input file. Note that we multiply the
    //   BLOCKSIZE by -2. Use of a negative offset will move us backwards
    //   (towards the beginning of the file).
    file_offset = lseek(in_fd, (BLOCKSIZE * -2), SEEK_CUR);
    if (((off_t) -1) == file_offset) {
      // Something bad happened.
      perror("lseek 3 failed");
      exit(9);
    }
  }

  fprintf(stderr, "Success\n");
  return(EXIT_SUCCESS);
}
