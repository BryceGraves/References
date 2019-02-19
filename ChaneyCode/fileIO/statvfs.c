#include <unistd.h>    // POSIX header
#include <stdlib.h>    // This is where exit() is declared.
#include <stdio.h>     // We want to use printf() and friends.
#include <sys/statvfs.h>

#define FS "/home2"

void show_vfs_info(char *path);

int 
main(
     int argc
     , char *argv[]
) 
{
  if (argc > 1) {
    int i;

    for (i = 1; i < argc; i++) {
      show_vfs_info(argv[i]);
    }
  }
  else {
    show_vfs_info(FS);
  }

  return(0);
}

void show_vfs_info(char *path)
{
  struct statvfs fs;

  if (statvfs(path, &fs) == 0) {
    printf("Info for %s file system\n", path);
    printf("\tFile-system block size (in bytes) \t\t\t\t%lu\n", fs.f_bsize);
    printf("\tFundamental file-system block size (in bytes) \t\t\t%lu\n", fs.f_frsize);

    printf("\tTotal number of blocks in file system (in units of 'f_frsize') \t%lu\n", fs.f_blocks);
    printf("\tTotal number of free blocks \t\t\t\t\t%lu\n", fs.f_bfree);

    printf("\tNumber of free blocks available to unprivileged process \t%lu\n", fs.f_bavail);
    printf("\tTotal number of i-nodes \t\t\t\t\t%lu\n", fs.f_files);

    printf("\tTotal number of free i-nodes \t\t\t\t\t%lu\n", fs.f_ffree);

    printf("\tNumber of i-nodes available to unprivileged process \t\t%lu\n", fs.f_favail);

    printf("\tFile-system ID \t\t\t\t\t\t\t%lu\n", fs.f_fsid);
    printf("\tMount flags \t\t\t\t\t\t\t%lu\n", fs.f_flag);
    printf("\tMaximum length of filenames on this file system \t\t%lu\n", fs.f_namemax);
  }
  else {
    printf("*** Cannot get metadata for %s ***\n", path);
  }
}
