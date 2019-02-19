/*
 * R Jesse Chaney
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>


#ifndef TRUE
# define TRUE 1
#endif // TRUE
#ifndef FALSE
# define FALSE 0
#endif // FALSE

int 
main(int argc, char *argv[]) 
{
    char c;
    pid_t pid = -1;
    // Default values if nothing is given on the command line.
    char first_letter[10] = "a";
    char last_letter[10] = "g$";
    int pipes[2];
    short verbose = FALSE;

    if (argc < 2) {
        printf("Must specify first and last character strings with -f str and -l str\n");
        exit(EXIT_FAILURE);
    }
    while ((c = getopt(argc, argv, "vf:l:")) != -1) {
        switch (c) {
        case 'f':
            // Create the pattern to match for the initial characters for the
            // words.
            sprintf(first_letter, "%s", optarg);
            break;
        case 'l':
            // Create the search pattern to look for the end characters in
            // input line.
            sprintf(last_letter, "%s$", optarg);
            break;
        default:
            break;
        }
    }
    if (verbose) {
        fprintf(stderr, "first letter(s): '%s'  last_letter(s) pattern: '%s'\n"
                , first_letter, last_letter);
    }

    // Create the pipe.
    pipe(pipes);
    {
        pid = fork();
        switch (pid) {
        case -1:
            perror("failed creating fork for pipes");
            return(EXIT_FAILURE);
            break;
        case 0:
            if (verbose) {
                fprintf(stderr, "  *** look child process created: %d\n", getpid());
            }
            // dup the "output" side of the pipes into the standard
            // file descriptor. The dup2() call will automatically
            // close the stdout file descriptor before copying the
            // new fd on top of it.
            dup2(pipes[STDOUT_FILENO], STDOUT_FILENO);

            // Close the unused file descriptors.
            close(pipes[STDIN_FILENO]);
            close(pipes[STDOUT_FILENO]);

            // The child process will execlp() the look command.
            execlp("look", "look", first_letter, (char *) NULL);
            perror("child cannot execlp look");
            fprintf(stderr, "******* %d: look failed *******\n", getpid());
            exit(EXIT_FAILURE);
            break;
        default:
            // The parent process simply carries on.
            break;
        }
    }
    {
        pid = fork();
        switch (pid) {
        case -1:
            perror("failed creating fork for pipes_2");
            return(EXIT_FAILURE);
            break;
        case 0:
            if (dup2(pipes[STDIN_FILENO], STDIN_FILENO) < 0) {
                perror("child failed dup2");
                return(EXIT_FAILURE);
            }
            if (verbose) {
                fprintf(stderr, "  *** grep child process created: %d\n", getpid());
            }
            close(pipes[STDIN_FILENO]);
            close(pipes[STDOUT_FILENO]);

            execlp("grep", "grep", last_letter, (char *) NULL);
            perror("child cannot execlp grep");
            fprintf(stderr, "******* %d: grep child failed *******\n", getpid());
            _exit(EXIT_FAILURE);
            break;
        default:
            close(pipes[STDIN_FILENO]);
            close(pipes[STDOUT_FILENO]);

            break;
        }
    }

    pid = wait(NULL);
    pid = wait(NULL);
    return(EXIT_SUCCESS);
}
