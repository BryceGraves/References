#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    int iFd;
    ssize_t bytes_read;
    size_t bytes_to_read = (size_t) 41;
    char buf[256];
    int i = 1;

    if (argc < 2) {
        printf("Must give file name on command line\n");
        exit(1);
    }
    if (argc > 2) {
        bytes_to_read = (size_t) atoi(argv[2]);
    }
    iFd = open(argv[1], O_RDONLY);
    if (iFd > 0) {
        //bytes_read = read(iFd, buf, 8);
        while ((bytes_read = read(iFd, buf, bytes_to_read)) > 0) {
            printf("%4d: ", i++);
            if (bytes_read != bytes_to_read) {
                printf("ERROR: Mismatch on bytes read for extract %s %3d %3d\n"
                       , argv[1], (int) bytes_read, (int) bytes_to_read);
            }
            else {
                printf("  %3d  %3d\n", (int) bytes_read, (int) bytes_to_read);
            }
        }
        close(iFd);
    }
    else {
        printf("failed to open file >>%s<<\n", argv[1]);
    }
    return(iFd > 0);
}
