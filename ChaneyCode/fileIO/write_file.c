/*
 * R Jesse Chaney
 */

// I have put a LOT of comments into this code. If you are not using
//   an editor that make comments stand out (such as by color), this
//   may be difficult to read. In any case, you will be wading through
//   a lot of comments to find a few lines of code. I hope the comments
//   are helpful.

// compile with
//     gcc -Wall -o write_file write_file.c

#include <fcntl.h>     // file control
#include <unistd.h>    // POSIX header
#include <stdlib.h>    // needed for the prototype for exit()
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

int 
main(
    int argc       // The number of command line arguments.
    , char *argv[] // An array of strings of the command line arguments.
) {
    char text[] = 
        "THIS is just some long text to write to the file.\n"
        "This is a second line of text to write to the file.\n\n"
        ;
    ssize_t num_written = 0;
    // Check the 'man 2 open' for details on what the
    //   values for flags and mode mean. While you are looking
    //   at what the flags mean, take a gander at section below
    //   the O_CREAT flag that describes the symbolic constants
    //   are provided for mode.
    // In this case, we want to open the file as write-only, create
    //   the file if it does not exist, and if it does exist, truncate
    //   its size to 0 bytes.
    int flags = (
        O_WRONLY 
        | O_CREAT 
        //| O_EXCL
        //O_RDWR
        //| O_APPEND
        //| O_CREAT 
        //| O_TRUNC
        );
    // The mode (also called permissions) of a new file can be set using
    //   the symbolic values as listed in the man page for open (man 2).
    //   In this case, we set the file as readable by the owner (user),
    //   writable by the owner, readable by the group owner, writable by
    //   the group owner, and readable by other.
    // Remember that the permissions on a file are split up by 3 different
    //   catigories, user (aka owner), group, and other. The user and the
    //   the group are ideitified by the user id (uid) and group id (gid)
    //   associated with the file.
    mode_t mode = (
        S_IRUSR 
        | S_IWUSR 
        | S_IRGRP 
        | S_IWGRP 
        | S_IROTH
        );

    int oFd = -1; // The file descriptor we will use.

    // The umask is an interesting thing. You should look at the
    //   man page for it. It is 'man 2 umask'. It will probably take
    //   you a bit to wrap your head around what the umask actually means.
    // Keep in mind that it masks OUT bits that you set on the permissions
    //   of a file when created. If you call umask(0), you will disable 
    //   the setting for this process only (and any processes it creates).
    // The call to umask(0) says "you need to actually obey my request
    //   when I create a file". The alternative is to explicitly call
    //   chmod() or fchmod() on the file after you create it.
    // Normally, your umask is set in one of your .files in your home 
    //   directory (like .login).
    // If you want to change your umask permanently, find it in
    //   the .file and edit it.
    // If you want to know what your umask currently is, simply type
    //   `umask` at a terminal prompt. It might be a bit easier to type
    //   `umask -S` to show the symbolic meaning of your current umask.
    //   If your current umask is 0007, then all the permissions (read,
    //   write, and execute) will be automatically disabled on any new
    //   file you create.
    umask(0);

    // Open the file to which we will write some bytes. Check the flags and
    //   mode values above.
    oFd = open("TestFile.txt", flags, mode);
    if (-1 == oFd) {
        // Something bad happened.
        // A call to open() will return a value greater than or equal to zero
        //   on success and a value less then zero on failure or error.
        // You can save yourself a LOT of time by checking the return 
        //   value from system calls and calling perror() if it is an error
        //   and print the value.
        // You could simply print the value of oFd and errno, then
        //   look those values up in an error listing of codes, but a call
        //   to perror() will do most of that for you.
        perror("Cannot open file");
        // I set up the exit value so that I can have a unique exit
        //   value for each error that can occur in the code. Of course,
        //   this is pretty simple code.
        exit(1);
    }

    // Now we are ready to write into the newly opened file. The write()
    //   call takes a file descriptor (returned by open), a buffer of
    //   bytes to write, and the number of bytes to write. In this case,
    //   I'm using a call to strlen() to tell me how many bytes I want to
    //   write to the file descriptor.
    num_written = write(oFd, text, strlen(text));
    if (-1 == num_written) {
        // Something bad happend.
        // A call to write() will return the number of bytes actually written
        //   to the file descriptor. If the number of bytes written is less than
        //   zero, then an error occured.
        perror("Write failed.");
        exit(2);
    }
    if (strlen(text) != num_written) {
        // The write was incomplete for some reason!!!
        // It is possible for a call to write() to not write all the bytes from
        //   the buffer to the file descriptor. The OS does not consider this 
        //   an error. It is up to you to catch and handle it.
        fprintf(stderr, "Incomplete write. Wrote %d of %d.\n"
                , (int) num_written, (int) strlen(text));
        // This might be a good place to exit, but since that
        //   is what we are about to do anyway, I don't.
    }

    if (0 != close(oFd)) {
        // Something bad happened.
        // It is rare that a call to close() will cause and error, but you should 
        //   get into the habit of checking the return values from system calls. 
        //   In this case, were are about to exit the process anyway, but it is 
        //   nice to know if the call to close() failed and why.
        perror("Close failed");
        exit(3);
    }

    fprintf(stderr, "Success!!! We wrote to a file.\n");
    return(EXIT_SUCCESS);
}
