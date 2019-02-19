/*
 * R Jesse Chaney
 */

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) 
{
    char *cmd = "ls -l *.c";
    char buf[BUFSIZ];
    FILE *pFile;

    if ((pFile = popen(cmd, "r")) != NULL) {
        while (fgets(buf, BUFSIZ, pFile) != NULL) {
            (void) printf("%s", buf);
        }
        (void) pclose(pFile);
    }
    return(EXIT_SUCCESS);
}
