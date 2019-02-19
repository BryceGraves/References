// Jesse Chaney

#include <fcntl.h>     // file control
#include <unistd.h>    // POSIX header
#include <stdlib.h>    // needed for the prototype for exit()
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include "rw_struct.h"

int 
main(
    int argc       // The number of command line arguments.
    , char *argv[] // An array of strings of the command line arguments.
)
{
    simple_struct_t simple_var;
    int oFd = -1;
    int i = -1;

    //  memset(&simple_var, 0, sizeof(simple_var));

    //simple_var.single_char = 'a';
    //strcpy(simple_var.multi_char, "this is a simple string");
    //simple_var.an_int = 17;

    //umask(0);
    oFd = open(BIN_FILENAME, O_WRONLY | O_CREAT | O_TRUNC
              , S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);

    srand(time(NULL));
    if (oFd >= 0) {
        for (i = 0; i < NUM_TO_WRITE; i++) {
            // The value given as the -n argument to fortune should be at least
            // one less than CHAR_LEN. Either that or do a better job of managing
            // the ARRAY of char.
            FILE *fortune = popen("fortune -n " FORTUNE_LEN " -s", "r");
            char buffer[CHAR_LEN + 1];

            if (fortune != NULL) {
                memset(&simple_var, '\0', sizeof(simple_var));
                // This is a cheating way to make the C libraries 'think' this is
                // a null terminated string, which has zero characters in it. It
                // allows me to to the following strcat() calls with out worry that
                // it will go searching through the ARRAY of char (with all spaces)
                // hunting for the first null char to being the concatination.
                simple_var.multi_char[0] = '\0';
                while (fgets(buffer, CHAR_LEN, fortune) != NULL) {
                    strcat(simple_var.multi_char, buffer);
                    fprintf(stdout, "%s", buffer);
                }
                fprintf(stdout, "\n");
                pclose(fortune);

                // This is pretty risky. I'm assuming that the length of the string
                // is less than than the maximum length of the ARRAY of char. If the
                // string is exactly CHAR_LEN long (unlikely since I specify the max
                // length of a fortune to less than that), then I could be putting the
                // space into the bytes occupied by the an_int field. That makes it all
                // the better that I assign on top of the an_int field after I blast
                // the space into this location.
                // By the way, I put the space in there to eliminate the null character
                // terminator of the string i just created. the multi_char field is an
                // ARRAY of char, not a string.
                simple_var.multi_char[strlen(simple_var.multi_char)] = ' ';

                simple_var.single_char = (char) ('A' + i);
                simple_var.an_int = rand() % 100;
                simple_var.a_double = (long double) ((rand() % 10000) / 10.0);

                sprintf(simple_var.record_num, "%d", i);
                write(oFd, &simple_var, sizeof(simple_var));
            }
            else {
                perror("could not popen() fortune command");
                exit(EXIT_FAILURE);
            }
        }

        printf("wrote %d records into binary file\n", NUM_TO_WRITE);
        close(oFd);
    }
    else {
        perror("could not open() " BIN_FILENAME " for for o/p");
        exit(EXIT_FAILURE);
    }

    return(EXIT_SUCCESS);
}
