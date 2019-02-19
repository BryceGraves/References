/*
 * R Jesse Chaney
 */

// The #pragma once is the "more modern" way to implement include
//   guards, but it is not universally supported. In addition, if
//   there are hard or symbolic links to an include file, the compiler
//   may not notice that the content was actually included more than
//   once (and would be very frustrating to debug). For extra protection,
//   I'm doing both.
// At present, this is a bit of a flame between a couple big parties
//   in the programming world.
#pragma once

#ifndef RMCHAR_H_
# define RMCHAR_H_

# define CMD_OPTIONS "vycwdpshVb:ulD:A"
# define BUFFER_SIZE_DEFAULT 1000
# define BUFFER_SIZE_MIN 10
# define BUFFER_SIZE_MAX 100000
# define DEBUG_MIN 0
# define DEBUG_MAX 3

# define VOWELS "aeiou"
# define SOMETIMES_Y "y"
# define CONSONANTS "bcdfghjklmnpqrstvwxz"
# define DIGITS "0123456789"
# define WHITESPACE " \t\n\r\f\v"
# define PUNCTUATION ".,?!:;"
# define SPECIAL_CHARS "`~@#$%^&*()-_+=|\\{}[]\"'<>/"

//# define MAX_DEBUG
# ifdef MAX_DEBUG
   // FUNCTION_ENTRY and FUNCTION_EXIT are cpp macros that take
   // arguments.  They are textual substitutions made by cpp 
   // during stage 1 of the compilation process. 
#  define FUNCTION_ENTRY(_DB, _TST) if (_DB >= _TST) {\
    fprintf(stderr, "DEBUG: entering: file: %s  function: %s  line: %d\n", \
	    __FILE__, __FUNCTION__, __LINE__);}
#  define FUNCTION_EXIT(_DB, _TST) if (_DB >= _TST) {\
    fprintf(stderr, "DEBUG: exiting: file: %s  function: %s  line: %d\n", \
	    __FILE__, __FUNCTION__, __LINE__);}
# else // MAX_DEBUG
#  define FUNCTION_ENTRY(_DB, _TST) 
#  define FUNCTION_EXIT(_DB, _TST) 
# endif // MAX_DEBUG

// I know these are defined __SOMEPLACE__ I just don't
// remember where.  So, just in case I've not included
// the right file, I have them here (protected).
# ifndef FALSE
#  define FALSE 0
# endif // FALSE
# ifndef TRUE
#  define TRUE 1
# endif // TRUE

// I'm going to create a structure and then also make it
// a new type.
typedef struct flags_s {
# ifdef SORTA_WASTEFUL
  // C does not have a type boolean.  So, we want the values
  // to take up as little space as possible.  We could create
  // a boolean type as a typedef (from an unsigned short
  // or char.), the for this case, the bit fields below
  // are better.
  unsigned short flag_v;
  unsigned short flag_y;
  unsigned short flag_c;
  unsigned short flag_w;
  unsigned short flag_d;
  unsigned short flag_p;
  unsigned short flag_s;
  unsigned short flag_t;
  unsigned short flag_h;
  unsigned short flag_V;
  unsigned short flag_u;
  unsigned short flag_l;
  unsigned short debug_value;
# else // SORTA_WASTEFUL
  // We can be more space efficient by using bit fields to
  // represent the boolean values for the flags.  The : 1
  // means that only a single bit is set aside for the
  // the value stored with that name.  The size of the
  // type/struct is rounded up to the next larger byte.
  unsigned int flag_v : 1;
  unsigned int flag_y : 1;
  unsigned int flag_c : 1;
  unsigned int flag_w : 1;
  unsigned int flag_d : 1;
  unsigned int flag_p : 1;
  unsigned int flag_s : 1;
  unsigned int flag_t : 1;
  unsigned int flag_h : 1;
  unsigned int flag_V : 1;
  unsigned int flag_u : 1;
  unsigned int flag_l : 1;
  unsigned int debug_value : 2;  // 2 bits is enough to store 0-3.
# endif // SORTA_WASTEFUL
} flags_t; // This is now a new type.

#endif // RMCHAR_H_
