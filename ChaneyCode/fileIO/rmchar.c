/*
 * R Jesse Chaney
 */

// I make use of the C feature test macro _GNU_SOURCE so that I
// can use the strdupa call.
#define _GNU_SOURCE

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <libgen.h>
#include <ctype.h>

// You should always put the system include files BEFORE your
// local include files. System include files are placed 
// within <> and your local include files are within "".
#include "rmchar.h"

// Prototypes are good to have.
static int remove_characters(char *, char *, char *, size_t *, unsigned);
static void show_help(char *, unsigned);
static char *my_to_upper(char *);
static char *create_remove_character_string(char *, unsigned, unsigned);

static void
show_help(
    char *prog
    , unsigned is_version_only
) {

    if (! is_version_only) {
        fprintf(stdout, "Program: %s\n", basename(prog));
        fprintf(stdout, "  options: " CMD_OPTIONS "\n\n");
        fprintf(stdout, "    -v   remove all the vowels from the input: %s\n"
                , VOWELS);
        fprintf(stdout, "    -c   remove all the consonants from the input: %s\n"
                , CONSONANTS SOMETIMES_Y);
        fprintf(stdout, "    -y   treat the character y as a vowel (default is consonant)\n");
        fprintf(stdout, "    -w   remove all the white space from the input\n");
        fprintf(stdout, "    -d   remove all the digits from the input: [0-9]\n");
        fprintf(stdout, "    -p   remove all the punctuation from the input: %s\n"
                , PUNCTUATION);
        fprintf(stdout, "    -s   remove all the special characters from the input: %s\n"
                , SPECIAL_CHARS);
        fprintf(stdout, "    -A   set the -v -c -w -d -p -s switches\n");
        fprintf(stdout, "    -h   show the help text and exit\n");
        fprintf(stdout, "    -V   show version and exit\n");
        fprintf(stdout
                , "    -b # specify the size of the buffer for reading input [%d-%d]\n"
                , BUFFER_SIZE_MIN, BUFFER_SIZE_MAX);
        fprintf(stdout, "    -D # show debug information while running [%d-%d]\n"
                , DEBUG_MIN, DEBUG_MAX);
    }
    fprintf(stdout
            // When I use RCS for revision control, I could put a $Id: rmchar.c,v 1.2 2019/01/22 15:11:42 chaneyr Exp chaneyr $ in here
            // and have RCS keep it current for me. The newer revision control
            // systems don't have that capability (and seem to ACTIVELY loathe it).
            , "Version 1.0\n");

    return;
}

// This should not be confused with the toupper() function that
// is part of clib. This modifies the string that is passed as
// a parameter. In C++, I'd be able to overload the function 
// definition based on the different argument types. Thankfully,
// we are not using C++.
static char *
my_to_upper(
    char *str
) {
    unsigned i;

    // I'm assuming that str is indeed a string (NULL terminated).
    for (i = 0; i < strlen(str); i++) {
        str[i] = toupper(str[i]);
    }

    return(str);
}

static int
remove_characters(
    char *letters_to_remove
    , char *read_buffer
    , char *write_buffer
    , size_t *nbytes
    , unsigned debug
) {
    unsigned read_index;
    unsigned write_index;

    FUNCTION_ENTRY(debug, 3);
    if (debug > 1) {
        fprintf(stderr, "DEBUG: removing characters \"%s\"\n", letters_to_remove);
    }
    for (read_index = 0, write_index = 0; read_index < *nbytes; read_index++) {
        // The strchr() function tries to locate the character (the second
        // parameter), in the string (the first parameter). The the char
        // is not found, a NULL is returned. In our case, when a NULL is
        // returned, we know that we want to store that character in the
        // output buffer.
        if (NULL == strchr(letters_to_remove, read_buffer[read_index])) {
            write_buffer[write_index] = read_buffer[read_index];
            write_index++;
        }
    }

    if (debug > 1) {
        fprintf(stderr, "DEBUG: removed %u characters\n"
                , (unsigned) ((*nbytes) - write_index));
    }
    *nbytes = write_index;
    // This gets the read buffer ready for the next step in the
    // process. Put into the read_buffer whatever is left after
    // the characters have been removed, which is just the
    // write_buffer.
    memcpy(read_buffer, write_buffer, write_index);
    FUNCTION_EXIT(debug, 3);

    return(write_index);
}

static char *
create_remove_character_string(
    char *letters
    , unsigned flag_y
    , unsigned debug
) {
    char *tmp = NULL;
    char *upper = NULL;

    // I should do a better job of estimating the size of
    // the buffer I want to allocate.
    //   (strlen(letters) * 2) + 2 + 1
    // should be right.
    tmp = (char *) calloc(100, sizeof(char));

    FUNCTION_ENTRY(debug, 3);
    // I assume that the string represented by letters is
    // all lower case.
    strcpy(tmp, letters);
    if (flag_y) {
        // If the -y flag is on the command line, add the y 
        // into the list of vowels. The y defaults to being
        // a consonant.
        if (debug > 0) {
            fprintf(stderr, "DEBUG: adding y into the remove character set\n");
        }
        // Concatenate the string "y" to the end.
        strcat(tmp, SOMETIMES_Y);
    }
    // I would not want to use strdupa with a large amount
    // of memory to allocate, but in this case it is only
    // a few characters. The nice thing about strdupa is
    // that I don't have to free it up when I'm done and
    // it will not fragment the heap.
    upper = my_to_upper(strdupa(tmp));

    // Concatenate the all upper case string with the all
    // lower case string.
    strcat(tmp, upper);
    // At this point, we have a string containing all the
    // letters (including y, if specified on the command
    // line) in a string, both lower and upper case.
    if (debug > 0) {
        fprintf(stderr, "DEBUG: remove character set: \"%s\"\n", tmp);
    }
    FUNCTION_EXIT(debug, 3);
    return(tmp);
}

int
main(
    int argc
    , char *argv[]
) {
    int opt;
    size_t buf_size = BUFFER_SIZE_DEFAULT;
    char *vowels = NULL;
    char *consonants = NULL;
    char *read_buffer = NULL;
    char *write_buffer = NULL;
    flags_t flags; // A new type we define in the rmchar.h file.

    memset((void *) &flags, 0, sizeof(flags_t));
    // In part, this code is about how to use the getopt() function.
    // Once you get accustomed to it, getopt is a really handy
    // function and it takes care of a lot of the cases you really
    // don't want to have to worry about.
    while ((opt = getopt(argc, argv, CMD_OPTIONS)) != -1) {
        switch (opt) {
        case 'v': // Remove vowels
            flags.flag_v = TRUE;
            if (flags.debug_value > 0) {
                fprintf(stderr, "DEBUG: remove vowels option\n");
            }
            break;
        case 'y': // Make y a vowel, defaults to a consonant
            flags.flag_y = TRUE;
            if (flags.debug_value > 0) {
                fprintf(stderr, "DEBUG: sometimes y vowel option\n");
            }
            break;
        case 'c': // Remove consonants
            flags.flag_c = TRUE;
            if (flags.debug_value > 0) {
                fprintf(stderr, "DEBUG: remove consonants option\n");
            }
            break;
        case 'w': // Remove white space
            flags.flag_w = TRUE;
            if (flags.debug_value > 0) {
                fprintf(stderr, "DEBUG: remove white space option\n");
            }
            break;
        case 'd': // Remove digits
            flags.flag_d = TRUE;
            if (flags.debug_value > 0) {
                fprintf(stderr, "DEBUG: remove digits option\n");
            }
            break;
        case 'p': // Remove punctuation
            flags.flag_p = TRUE;
            if (flags.debug_value > 0) {
                fprintf(stderr, "DEBUG: remove punctuation option\n");
            }
            break;
        case 's': // Remove special characters
            flags.flag_s = TRUE;
            if (flags.debug_value > 0) {
                fprintf(stderr, "DEBUG: remove special characters option\n");
            }
            break;
        case 'A': // enable LOTS of switches
            flags.flag_v = TRUE;
            flags.flag_c = TRUE;
            flags.flag_w = TRUE;
            flags.flag_d = TRUE;
            flags.flag_p = TRUE;
            flags.flag_s = TRUE;
            if (flags.debug_value > 0) {
                fprintf(stderr, "DEBUG: the -A option selected\n");
            }
            break;
        case 'h': // show help
            flags.flag_h = TRUE;
            if (flags.debug_value > 0) {
                fprintf(stderr, "DEBUG: show help option\n");
            }
            break;
        case 'V': // show version
            flags.flag_V = TRUE;
            if (flags.debug_value > 0) {
                fprintf(stderr, "DEBUG: show version option\n");
            }
            break;
        case 'b':
            // This is an option that has a required argument. The
            // getopt() function will automatically put the (required)
            // argument in the variable optarg. The optarg variable
            // is just another pointer into one of the argv elements.
            // You need to convert this one into an integer (of type
            // size_t).
            // Of course, we __should__ do some range checking on this
            // value. Providing a negative value or a very large value
            // would be bad.
            // I created a new scope in which to test this so that I
            // did not have to declare the variable i at the top of the
            // function. Its scope (and extent) is only known within
            // this narrow region of code. The compiler (and optimizer)
            // knows that it will not (an cannot) be used outside this
            // scope. I can re-declare it in other scopes, as is done
            // for the option for D.
        {
            int i;

            // strtol() is better to use than atoi().
            i = strtol(optarg, NULL, 10);
            if ((i < BUFFER_SIZE_MIN) || (i > BUFFER_SIZE_MAX)) {
                fprintf(stderr, "Illegal value given for buffer size %d\n", i);
                fprintf(stderr, "  Must be between %d and %d\n"
                        , BUFFER_SIZE_MIN, BUFFER_SIZE_MAX);
                return(EXIT_FAILURE);
            }
            buf_size = (size_t) i;
        }
        break;
        case 'D':
            // We should be doing some range checking on this value. If
            // the provided value does not fit into 2 bits, it could be
            // bad.
        {
            int i;

            i =  strtol(optarg, NULL, 10);
            if ((i < DEBUG_MIN) || (i > DEBUG_MAX)) {
                fprintf(stderr, "Illegal value given for debug flag %d\n", i);
                return(EXIT_FAILURE);
            }
            if ((flags.debug_value > 0) && (flags.debug_value != i)) {
                fprintf(stderr, "  Resetting debug value old: %d new: %d\n"
                        , flags.debug_value, i);
            }
            flags.debug_value = i;
            if (flags.debug_value > 0) {
                fprintf(stderr, "Debugging information enabled %d\n", flags.debug_value);
            }
        }
        break;
        case '?':
            // This is an unknown option on the command line.
            fprintf(stderr, "*** Unknown option used, ignoring. ***\n");
            break;
        default:
            fprintf(stderr, "*** Oops, something strange happened <%s> ***\n", argv[0]);
            break;
        }
    }

    // When getopt() is through chewing on the command line (argv), optind
    // is the value of the "next" command line argument. If there are
    // still extra things on the command line, this will let you know
    // and print what they are.
    if (optind < argc) {
        unsigned i;

        fprintf(stderr, "** Chatter following known arguments ignored.\n");
        for(i = optind; i < argc; i++) {
            fprintf(stderr, "   ** Unknown command line option: %s\n", argv[i]);
        }
    }

    if (TRUE == flags.flag_h) {
        // Print the help text and exit.
        show_help(argv[0], FALSE);
        return(EXIT_SUCCESS);
    }

    if (TRUE == flags.flag_V) {
        // Print the version text and exit.
        show_help(argv[0], TRUE);
        return(EXIT_SUCCESS);
    }

    // We want to allocate the memory that we will be using to read
    // the input to the program. We also want to make sure that
    // the memory is all set to zeroes (cleared) before we use it.
    read_buffer = (char *) malloc(buf_size * sizeof(char));
    if (NULL == read_buffer) {
        perror("error allocating memory for read buffer");
        return(EXIT_FAILURE);
    }
    // We want to make sure the newly allocated memory is set
    // to all zeroes.
    memset((void *) read_buffer, 0, (buf_size * sizeof(char)));

    // We can accomplish the allocate and clear in 2 steps or 
    // a single step.
    write_buffer = (char *) calloc(buf_size, sizeof(char));
    if (NULL == write_buffer) {
        perror("error allocating memory for write buffer");
        return(EXIT_FAILURE);
    }
  
    if (TRUE == flags.flag_v) {
        vowels = create_remove_character_string(
            VOWELS
            , flags.flag_y
            , flags.debug_value);
    }
    if (TRUE == flags.flag_c) {
        consonants = create_remove_character_string(
            CONSONANTS
            , (! flags.flag_y)
            , flags.debug_value);
    }
    // I tend to like to represent forever loops as for(;;) instead
    // of using while(1).
    for ( ; ; ) {
        ssize_t num_bytes_read;
        ssize_t num_bytes_written;
        size_t num_bytes_to_write;

        // Need to make sure these buffers are all set to zeroes before
        // we load anything into them.
        memset(read_buffer, 0, buf_size);
        memset(write_buffer, 0, buf_size);
        num_bytes_to_write = 0;

        num_bytes_read = read(STDIN_FILENO, read_buffer, buf_size);
        if (num_bytes_read < 0) {
            perror("error while reading input");
            return(EXIT_FAILURE);
        }
        if (0 == num_bytes_read) {
            // This means we've come to the end of the input.
            if (flags.debug_value > 0) {
                fprintf(stderr, "DEBUG: end of input reached\n");
            }
            // Break out of the for loop.
            break;
        }
        num_bytes_to_write = (size_t) num_bytes_read;

        // This is a good way to write this sort of statement. Since a constant
        // cannot be a lvalue, if you mistakenly type "if (TRUE = flags.flag_v) {"
        // it will generate a syntax error, which can save you a LOT of grief
        // during debugging (or after deployment).
        if (TRUE == flags.flag_v) {
            // Call function to remove vowels from input buffer.
            remove_characters(vowels, read_buffer, write_buffer
                              , &num_bytes_to_write, flags.debug_value);
        }

        if (TRUE == flags.flag_c) {
            // Call function to remove consonants from input buffer.
            remove_characters(consonants, read_buffer, write_buffer
                              , &num_bytes_to_write, flags.debug_value);
        }
        if (TRUE == flags.flag_d) {
            // Call function to remove the numbers [0-9] from input buffer.
            remove_characters(DIGITS, read_buffer, write_buffer
                              , &num_bytes_to_write, flags.debug_value);
        }
        if (TRUE == flags.flag_w) {
            // Call function to remove the white-space from input buffer.
            remove_characters(WHITESPACE, read_buffer, write_buffer
                              , &num_bytes_to_write, flags.debug_value);
        }
        if (TRUE == flags.flag_p) {
            // Call function to remove the punctuation from input buffer.
            remove_characters(PUNCTUATION, read_buffer, write_buffer
                              , &num_bytes_to_write, flags.debug_value);
        }
        if (TRUE == flags.flag_s) {
            // Call function to remove the special characters from input buffer.
            remove_characters(SPECIAL_CHARS, read_buffer, write_buffer
                              , &num_bytes_to_write, flags.debug_value);
        }

        if (num_bytes_to_write > 0) {
            num_bytes_written = write(STDOUT_FILENO
                                      , write_buffer, num_bytes_to_write);

            // Check to see if there was an error when writing the bytes
            // to STDOUT.
            if (num_bytes_written < 0) {
                perror("error while writing output");
                return(EXIT_FAILURE);
            }

            // Check to make sure that the number of bytes that were written
            // to STDOUT is what we expected.
            if (num_bytes_written != num_bytes_to_write) {
                fprintf(stderr, "*** Number of bytes written not equal to request ***\n");
                // This might be cause for a exit.
            }
        }
    }
    // I should consider printing a newline here. If all the white
    // space was removed, the result can look a bit strange with
    // the shell prompt not starting on a new line.

    // The call to free() does not return a value, so we cannot
    // check for success or failure. If you get s seg fault, it
    // failed.
    free(read_buffer);
    free(write_buffer);

    return(EXIT_SUCCESS);
}
