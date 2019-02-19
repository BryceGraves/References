// Jesse Chaney

#ifndef _RW_STRUCT_H
# define _RW_STRUCT_H

# define STRINGIFY_(x) #x
# define STRINGIFY(x) STRINGIFY_(x)

# define NUM_TO_WRITE 15

// This is a crummy name for this constant. :-(
# define CHAR_LEN 100
# define FORTUNE_LEN STRINGIFY(CHAR_LEN)

#define BIN_FILENAME "WriteStruct.bin"

typedef struct simple_struct_s {
    char record_num[5];
    char single_char;

    // This is an ARRAY of char, not a string.
    char multi_char[CHAR_LEN + 1];

    // This is what makes the file really a binary file.
    int an_int;
    long double a_double;
} simple_struct_t;

#endif // _RW_STRUCT_H
