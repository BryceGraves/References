 /*
 * R Jesse Chaney
 */

#include <stdio.h>
#include <sys/types.h>
#include <grp.h>

int
main(
     int argc
     , char *argv[]
) {
  struct group *g;
  int i;

  while((g = getgrent()) != NULL) {
    printf("%s:%s:%d:"
	   , g->gr_name
	   , g->gr_passwd
	   , g->gr_gid
	   );
    for (i = 0; NULL != g->gr_mem[i]; i++) {
      printf("%s%s"
	     , (i == 0) ? "" : ","
	     , g->gr_mem[i]);
    }
    printf("\n");
  }
  endgrent();

  return(0);
}
