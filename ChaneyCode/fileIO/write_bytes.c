/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2015.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Supplementary program for Chapter 13 */

/* write_bytes.c

   Write bytes to a file. (A simple program for file I/O benchmarking.)

   Usage: write_bytes file num-bytes buf-size

   Writes 'num-bytes' bytes to 'file', using a buffer size of 'buf-size'
   for each write().

   If compiled with -DUSE_O_SYNC, open the file with the O_SYNC flag,
   so that all data and metadata changes are flushed to the disk.

   If compiled with -DUSE_FDATASYNC, perform an fdatasync() after each write,
   so that data--and possibly metadata--changes are flushed to the disk.

   If compiled with -DUSE_FSYNC, perform an fsync() after each write, so that
   data and metadata are flushed to the disk.
*/
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#ifndef min
# define min(_A,_B) (((_A) < (_B)) ? (_A) : (_B))
#endif // min

int
main(int argc, char *argv[])
{
    size_t bufSize;
    size_t numBytes;
    size_t thisWrite;
    size_t totWritten;
    char *buf;
    int fd, openFlags;
    int syncFlag = 0;

    if ((argc < 4) || (0 == strcmp(argv[1], "--help"))) {
      //usageErr("%s file num-bytes buf-size\n", argv[0]);
      printf("%s file-to-create tot-bytes-to-write bytes-per-write sync-flag\n", argv[0]);
      printf("  sync-flag:\n");
      printf("      0 = no forced sync\n");
      printf("      1 = open file as O_SYNC\n");
      printf("      2 = fsync() after each write\n");
      printf("      3 = fdatasync() after each write\n");
      printf("      4 = sync() after each write (this will be very slow)\n");
      exit(1);
    }

    //numBytes = getLong(argv[2], GN_GT_0, "num-bytes");
    //bufSize = getLong(argv[3], GN_GT_0, "buf-size");
    {
      char *endptr;

      numBytes = strtol(argv[2], &endptr, 10);
      bufSize = strtol(argv[3], &endptr, 10);

      if (argc > 4) {
	syncFlag = strtol(argv[4], &endptr, 10);
      }
      printf("sync flag = %d\n", syncFlag);
    }

    buf = malloc(bufSize);
    if (buf == NULL) { 
      exit(2);
    }

    openFlags = O_CREAT | O_WRONLY;

    if (1 == syncFlag) {
      openFlags |= O_SYNC;
    }

    fd = open(argv[1], openFlags, S_IRUSR | S_IWUSR);
    if (fd == -1) {
      //errExit("open");
      exit(3);
    }

    for (totWritten = 0; totWritten < numBytes; totWritten += thisWrite) {
        thisWrite = min(bufSize, numBytes - totWritten);

        if (write(fd, buf, thisWrite) != thisWrite) {
	  //fatal("partial/failed write");
	  exit(4);
	}

	if (2 == syncFlag) {
	  if (fsync(fd)) {
	    //errExit("fsync");
	    exit(5);
	  }
	}
	if (3 == syncFlag) {
	  if (fdatasync(fd)) {
	    //errExit("fdatasync");
	    exit(6);
	  }
	}
	if (4 == syncFlag) {
	  sync();
	}
    }

    close(fd);
    //if (close(fd) == -1)
    //    errExit("close");

    exit(EXIT_SUCCESS);
}
