/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2015.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 4-1 */

/* copy.c

   Copy the file named argv[1] to a new file named in argv[2].
*/
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef BUF_SIZE
# define BUF_SIZE 2
    //1024
#endif

int
main(int argc, char *argv[])
{
    int inputFd, outputFd, openFlags;
    mode_t filePerms;
    ssize_t numRead;
    long buf_size = BUF_SIZE;

    if (argc < 3 || strcmp(argv[1], "--help") == 0) {
        printf("%s old-file new-file\n", argv[0]);
	exit(1);
    }
    if (argc > 3) {
      char *endptr;

      buf_size = strtol(argv[3], &endptr, 10);
    }

    /* Open input and output files */

    inputFd = open(argv[1], O_RDONLY);
    if (inputFd == -1) {
        perror("opening input file error");
	exit(2);
    }

    openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH;      /* rw-rw-rw- */
    outputFd = open(argv[2], openFlags, filePerms);
    if (outputFd == -1) {
        perror("opening output file error");
	exit(3);
    }

    /* Transfer data until we encounter end of input or an error */
    {
      char buf[buf_size];

      while ((numRead = read(inputFd, buf, buf_size)) > 0) {
	if (write(outputFd, buf, numRead) != numRead) {
	  perror("couldn't write whole buffer");
	  exit(4);
	}
      }
    }
    if (numRead == -1) {
        perror("read failed");
	exit(5);
    }

    close(inputFd);
    close(outputFd);

    exit(EXIT_SUCCESS);
}
