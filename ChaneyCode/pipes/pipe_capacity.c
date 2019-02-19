/*
 * R Jesse Chaney
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

int 
main(int argc, char *argv[]) 
{
    pid_t cpid;
    int pipes[2];
    int ret;
    int cnt = 1;

    // Create a pair of pipes that will be used to send data from
    // process to process. It is important that the call to
    // pipe() occur before the fork(). That means that both
    // processes have access to the pipes.
    ret = pipe(pipes);
    if (0 != ret) {
        perror("pipe creation failed");
        exit(EXIT_FAILURE);
    }

    // Create a new child process.  The child process will also
    // receive the same pipes as the parent process.
    cpid = fork();
    switch(cpid) {
    case -1:
        // fork failed
        perror("Fork failed");
        break;
    case 0:
        {
            FILE *f = NULL;
            char line[100];
            char *res;

            // Child process.
            // Close the unused "read" file of the pipes.
            close(pipes[STDIN_FILENO]);

            f = fopen("numbers_10000.txt", "r");

            // Send all the lines from the file, 1 at a time.
            for ( ;  ; ) {
                res = fgets(line, sizeof(line), f);
                if (NULL == res) {
                    break;
                }

                // Write the bytes from the file to the "write" side of the pipe.
                ret = write(pipes[STDOUT_FILENO], line, strlen(line));
                fprintf(stderr, "    Sending process line num: %d  bytes: %d\n", cnt, ret);
                cnt ++;
            }
            close(pipes[STDOUT_FILENO]);
            fclose(f);
        }
        break;
    default:
        {
            char rstr[500];

            // Parent process
            // Close the unused "write" side of the pipes.
            close(pipes[STDOUT_FILENO]);

            // sleep for a few seconds to allow the pipe to
            // fill up.
            sleep(5);

            for ( ; ; ) {
                // Read bytes from the "read" side of the pipe.
                ret = read(pipes[STDIN_FILENO], rstr, sizeof(rstr));
                if (ret == 0) {
                    break;  // EOF
                }
                if (ret < 0) {
                    perror("read on pipe failed");
                    break;
                }

                fprintf(stderr, "Reading procss received line: %d  bytes: %d\n", cnt, ret);
                cnt ++;
            }
            close(pipes[STDIN_FILENO]);
        }
        break;
    }
    return(EXIT_SUCCESS);
}
