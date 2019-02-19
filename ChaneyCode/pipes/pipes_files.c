/*
 * R Jesse Chaney
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>

int 
main(int argc, char *argv[])
{
    pid_t cpid;
    int pipes[2];
    int ret;
    int i;
  
    if (argc < 2) {
        printf("Must give at least 1 command line arg\n");
        exit(EXIT_FAILURE);
    }

    ret = pipe(pipes);
    if (0 != ret) {
        perror("pipe creation failed");
        exit(EXIT_FAILURE);
    }

    cpid = fork();
    switch(cpid) {
    case -1:
        // fork failed
        perror("Fork failed");
        break;
    case 0:
        {
            FILE *output = NULL;

            // Child process
            close(pipes[STDIN_FILENO]);
            output = fdopen(pipes[STDOUT_FILENO], "w");
            for (i = 1 ; i < argc ; i++) {
                printf("Child process sending '%s'  %d\n", argv[i], getuid());
                fputs(argv[i], output);
                fputs("\n", output);

                //fprintf(output, "%s\n", argv[i]);
            }
        }
        break;
    default:
        {
            FILE *input = NULL;
            char rstr[100];
            char *r;

            ret = 0;
            // Parent process
            close(pipes[STDOUT_FILENO]);
            input = fdopen(pipes[STDIN_FILENO], "r");
            for ( ; ; ) {
                memset(rstr, 0, sizeof(rstr));
                //ret = read(pipes[STDIN_FILENO], rstr, sizeof(rstr));
                //if (0 == ret) break;
                r = fgets(rstr, sizeof(rstr), input);
                if (r == NULL) {
                    break;
                }
                printf("Parent procss received '%s' %d  %d\n", rstr, ret, getuid());
            }
        }
        break;
    }
    return EXIT_SUCCESS;
}
