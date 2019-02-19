/*
 * R Jesse Chaney
 */

// I have put a LOT of comments into this code. If you are not using
//   an editor that make comments stand out (such as by color), this
//   may be difficult to read. In any case, you will be wading through
//   a lot of comments to find a few lines of code. I hope the comments
//   are helpful.

// compile with
//     gcc -Wall -o read_file read_file.c

#include <fcntl.h>     // file control
#include <unistd.h>    // POSIX header
#include <stdlib.h>    // This is where exit() is declared.
#include <stdio.h>     // We want to use printf() and friends.
#include <errno.h>     // So we can use the errno value.

int 
main(
    int argc
    , char *argv[]
) {
    int flag = O_RDONLY; // We are going to open the file as
    //   read-only. There are a lot of other flags 
    //   available. Check `man 2 open`.
    int iFd = -1;  // This is the file descriptor we are going
    //   to open and read.

    // The open() system call has quite a number of parameters.
    // You should spend some time looking at the man page, man 2 open.
    // One of things you'll notice is that it can be called with either 2 
    //   or 3 parameters. In the case below, we are only using 2 parameters.
    //   The value of the flag parameter indicates that we are opening the 
    //   file as read-only.
    iFd = open("numbers_10.txt", flag);

    // When successful, a call to open will return a value >= 0 as a new file 
    //   descriptor. When a call to open() fails, it returns a value of -1. 
    //   Look at the "RETURN VALUE" portion of the man page. If you want to 
    //   try out what happens when the open() call cannot open the file, change 
    //   the name of the file to something that does not exist.
    if (-1 == iFd) {
        // Something bad happened. Could not open the file.
        // When a system call (or most other calls) fails, it will set a global 
        //   integer called errno.  If that happens, a call to perror() will print
        //   a meaningful message about what happened. Calling perror() has an 
        //   advantage over just printing errno in that you'll get some sort of 
        //   message about the error, not just the error number. However, if you
        //   want to have your own custom message, you can do that too.
        fprintf(stderr, "Could not open file, errno = %d\n", errno);
        perror("cannot open file");
        exit(EXIT_FAILURE);
    }

    // I could have a while loop like below, but I like to have a bit more 
    //   control over what happens with my return value.  I'll call break when 
    //   I reach the end of the file.
    //while((num_read = read(iFd, buf, sizeof(buf))) != 0) {
    for ( ; ; ) {
        char buf[5]; // We want this to be kind of small for this example.
        int num_read;
        int num_written;

        // A call to the read() function returns the number of bytes read from
        //   the file (when successful) or a -1 if an error occured.
        // The read() call takes 3 parameters: a file descriptor, a buffer into
        //   which bytes from the file are copied, and the maximum number of bytes
        //   to read from the file. It is possible that fewer than the maximum
        //   number of bytes will be read.
        // You need to keep in mind that this is reading bytes from the file, 
        //   which may not correspond to a line from the file. The number of 
        //   bytes read could be larger or smaller than a line from the file.
        num_read = read(iFd, buf, sizeof(buf));
        if (-1 == num_read) {
            // When the return value from read() is less than 0 there was some sort
            //   of problem reading the file.
            perror("Problem reading file");
            exit(EXIT_FAILURE);
        }
        if (0 == num_read) {
            // When a call to the read() function returns the value 0, you've 
            //   reached the end of the file. In this case, it means we need to 
            //   break out of the for() loop.
            // All bytes have been read, ie EOF.
            break;
        }

        // Just as a call to read() returns the number of bytes read from the 
        //   file, a call to write() returns the number of bytes written to
        //   the file.
        //
        // It is possible that the number of bytes you request be written to 
        //   the file is fewer than the number actually written. It is your 
        //   responsibility to notice and do the right thing if it happens.
        num_written = write(STDOUT_FILENO, buf, num_read);
        if (-1 == num_written) {
            // When the return value from write is less than 0 there was some 
            //   sort of problem writing to the file.
            perror("Problem writing to file");
            exit(EXIT_FAILURE);
        }
        // The line below can be interesting, but it will be intermingled with 
        //   the text output of the file (unless you redirect stderr or stdout
        //   to a file).
        //fprintf(stderr, "read %d bytes  written %d bytes\n"
        // , num_read, num_written);
    }

    // You should always close a file you open to clean up the kernel 
    //   structures. Yes, I know the kernel will clean up for you when you exit,
    //   but it is good to get into the habit of cleaning up for yourself.
    if (0 != close(iFd)) {
        // Something bad happened.
        // It is rare that a call to close() will cause and error, but you should 
        //   get into the habit of checking the return values from system calls. 
        //   In this case, were are about to exit the process anyway, but it is 
        //   nice to know if the call to close() failed and why.
        perror("Close failed");
        exit(EXIT_FAILURE);
    }

    return(EXIT_SUCCESS);
}
