/*
 * R Jesse Chaney
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libgen.h>

int main(int argc, char *argv[])
{
    char *lines = NULL;
    char *file_name = NULL;

    {
        char c;

        while ((c = getopt(argc, argv, "hn:")) != -1) {
            switch (c) {
            case 'n':
                lines = strdup(optarg);
                break;
            case 'h':
                // be helpful in here.
                break;
            default:
                // oops
                break;
            }
        }
    }

    if (optind < argc) {
        file_name = strdup(argv[optind]);
    }
    else {
        fprintf(stderr, "did not give a file name\n");
        exit(EXIT_FAILURE);
    }

    {
        int pipes[2] = {-1, -1};
        pid_t pid = -1;

        pipe(pipes);
        pid = fork();

        switch (pid) {
        case -1:
            perror("fork failed");
            return(EXIT_FAILURE);
            break;
        case 0:
        {
            char **rhp_argv = NULL;
            char *rhp = NULL;

            // Child process.
            // This will right hand side process.
            // dup2 the input side of the pipe onto stdin for the
            // right hand process. This will automatically close
            // stdin, in this process.
            if (dup2(pipes[STDIN_FILENO], STDIN_FILENO) < 0) {
                perror("child process failed dup2");
                _exit(EXIT_FAILURE);
            }
            // Close both pipes.
            // We are done with the pipe for this process. Close
            // both sides.
            close(pipes[STDIN_FILENO]);
            close(pipes[STDOUT_FILENO]);

            // Take the name of the program, removing any directory
            // rubbish from in front of it.
            rhp = basename(argv[0]);
            // If the name of the program is ztail, make the right
            // process tail. Otherwise, the right hand process will
            // be head.
            if (strcmp(rhp, "ztail") == 0) {
                rhp = strdup("tail");
            }
            else {
                rhp = strdup("head");
            }

            // If a command line option was given to change the number
            // of lines printed, put that into the argv for the
            // exec-ed process.
            if (NULL != lines) {
                // I use calloc() because I know it will set the memory
                // to all zeroes before returning it. That means that
                // the last value in the new argv will be the NULL pointer.
                rhp_argv = (char **) calloc(4, sizeof(char *));
                rhp_argv[0] = rhp;
                rhp_argv[1] = strdup("-n");
                rhp_argv[2] = lines;
            }
            else {
                rhp_argv = (char **) calloc(2, sizeof(char *));
                rhp_argv[0] = rhp;
            }

            execvp(rhp_argv[0], rhp_argv);
            // Sad panda. :-( The exec failed.
            perror("child cannot exec program");
            fprintf(stderr, "*** %d: %s failed ***\n", getpid(), rhp);
            fflush(stderr);
            _exit(EXIT_FAILURE);
        }
        break;
        default:
        {
            // The left hand side of all of this is simpler. We always
            // know how many items will be in the new argv.
            char *lhp_argv[] = {
                "gunzip"
                , "-c"
                , file_name
                , (char *) NULL
            };
            // Parent process.
            if (dup2(pipes[STDOUT_FILENO], STDOUT_FILENO) < 0) {
                perror("zhead 2 failed dup2");
                return(EXIT_FAILURE);
            }
            close(pipes[STDIN_FILENO]);
            close(pipes[STDOUT_FILENO]);

            //lhp_argv[2] = file_name;

            execvp(lhp_argv[0], lhp_argv);
            //execlp("gunzip", "gunzip", "-c", file_name, (char *) NULL);
            perror("parent cannot exec gunzip");
            fprintf(stderr, "*** %d: gunzip parent failed ***\n", getpid());
            exit(EXIT_FAILURE);
        }
        break;
        }
    }

    return(EXIT_FAILURE);
}
