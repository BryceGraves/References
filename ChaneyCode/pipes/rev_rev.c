/*
 * R Jesse Chaney
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef TRUE
# define TRUE 1
#endif // TRUE
#ifndef FALSE
# define FALSE 0
#endif // FALSE

int main(int argc, char *argv[]) 
{
    pid_t pid = -1;
    int pipes[2];

    pipe(pipes);
    {
        pid = fork();
        switch (pid) {
        case -1:
            perror("failed creating fork for pipes");
            return(EXIT_FAILURE);
            break;
        case 0:
            // Child process.
            if (dup2(pipes[STDIN_FILENO], STDIN_FILENO) < 0) {
                perror("rev 1 failed dup2");
                return(EXIT_FAILURE);
            }
            close(pipes[STDIN_FILENO]);
            close(pipes[STDOUT_FILENO]);

            //execlp("sed", "sed", "s/[^a-zA-Z]/ /g", (char *) NULL);
            execlp("rev", "rev", (char *) NULL);
            perror("child cannot execlp rev 1");
            fprintf(stderr, "*** %d: rev 1 failed ***\n"
                    , getpid());
            _exit(EXIT_FAILURE);
            break;
        default:
            // Parent process.
            if (dup2(pipes[STDOUT_FILENO], STDOUT_FILENO) < 0) {
                perror("rev 2 failed dup2");
                return(EXIT_FAILURE);
            }
            close(pipes[STDIN_FILENO]);
            close(pipes[STDOUT_FILENO]);

            //execlp("tr", "tr", "[A-Z]", "[a-z]", (char *) NULL);
            execlp("rev", "rev", (char *) NULL);
            perror("parent cannot execlp rev 2");
            fprintf(stderr, "*** %d: rev 2 child failed ***\n"
                    , getpid());
            exit(EXIT_FAILURE);
            break;
        }
    }

    return(EXIT_SUCCESS);
}
