 /*
 * R Jesse Chaney
 */

#pragma once

# define LISTENQ 1
# define MAXLINE 4096

# define QUOTE(_NAME_) #_NAME_
# define STR(_NAME_) QUOTE(_NAME_)

# define SERV_PORT 10501
# define SERV_PORT_STR STR(SERV_PORT)

union long_union {
  long num;
  char ch[sizeof(long)];
};

union int_union {
  int num;
  char ch[sizeof(int)];
};

union short_union {
  short num;
  char ch[sizeof(short)];
};
