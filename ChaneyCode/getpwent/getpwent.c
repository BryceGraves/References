 /*
 * R Jesse Chaney
 */

// Inspired from:
//    http://www-personal.engin.umich.edu/~jgotts/underground/hack-faq.html

#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>
#include <errno.h>

int
main(
     int argc
     , char *argv[]
)
{
  struct passwd *p;

  errno = 0;
  while((p = getpwent()) != NULL) {
    printf("%s:%s:%d:%d:%s:%s:%s\n"
	   , p->pw_name
	   , p->pw_passwd // This is probably not very useful.
	   , p->pw_uid
	   , p->pw_gid
	   , p->pw_gecos
	   , p->pw_dir
	   , p->pw_shell);
  }
  if (errno != 0) {
    perror("failed call to getpwent");
  }
  endpwent();

  return(0);
}
