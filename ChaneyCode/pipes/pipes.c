/*
 * R Jesse Chaney
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

int main(int argc, char *argv[])
{
    pid_t cpid;
    int pipes[2];
    int ret;
    int i;
    char rstr[100];

    // When you are using pipes to connect processes using stdin and
    // stdout, it is important that you send your diagnostic messages
    // to stderr, to make sure that the diagnostic messages are not
    // getting stuffed into the stdout pipe.
    if (argc < 2) {
        printf("Must give at least 1 command line arg\n");
        exit(EXIT_FAILURE);
    }

    // Create a pair of pipes that will be used to send data from
    // process to process. It is important that the call to
    // pipe() occur before the fork(). That means that both
    // processes have access to the pipes.
    ret = pipe(pipes);
    if (0 != ret) {
        perror("pipe creation failed");
        exit(EXIT_FAILURE);
    }

    // Create a new child process. The child process will also
    // receive the same pipes as the parent process.
    cpid = fork();
    switch(cpid) {
    case -1:
        // fork failed
        perror("Fork failed");
        break;
    case 0:
        // Child process.
        // Close the unused "read" file of the pipes.
        close(pipes[STDIN_FILENO]);

        // Send all the words on the command, 1 at a time.
        for (i = 1 ; i < argc ; i++) {
            fprintf(stderr, "    Child process sending '%s'  %d\n", argv[i], getuid());
            // Write the bytes from argv to the "write" side of the pipe.
            ret = write(pipes[STDOUT_FILENO], argv[i], strlen(argv[i]));
        }
        close(pipes[STDOUT_FILENO]);
        break;
    default:
        // Parent process
        // Close the unused "write" side of the pipes.
        close(pipes[STDOUT_FILENO]);

        for ( ; ; ) {
            // Read bytes from the "read" side of the pipe.
            ret = read(pipes[STDIN_FILENO], rstr, sizeof(rstr));
            if (ret == 0) {
                break;  // EOF
            }

            // Remember, the buffer that you read from the pipe may
            // not be NULL terminated.
            rstr[ret] = 0;
            fprintf(stderr, "Parent procss received '%s' pid: %d\n", rstr, getuid());
        }
        close(pipes[STDIN_FILENO]);
        break;
    }
    return(EXIT_SUCCESS);
}
