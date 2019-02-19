/*
 * R Jesse Chaney
 */

// Compile with
//    gcc -o user_procs user_procs.c

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

#define PROC_DIR "/proc"
#define BUF_SIZE 256
#define CMD_OPTIONS "hTZS"

# ifndef FALSE
#  define FALSE 0
# endif // FALSE
# ifndef TRUE
#  define TRUE 1
# endif // TRUE

# define NUMBERS "0123456789"

static int showThreads = FALSE;
static int onlyZombies = FALSE;
static int sumOnly = FALSE;
static int showHelp = FALSE;

int
main(
     int argc
     , char *argv[]
     , char *envp[]
) {
  int opt;
  int process_count = 0;
  int zombie_count = 0;
  struct dirent* file;
  struct stat file_stat;
  FILE *status_file = NULL;
  char stat_file_name[BUF_SIZE];
  char dir_name[BUF_SIZE];
  char line[BUF_SIZE];
  char proc[BUF_SIZE];
  char *str;
  char *ch;
  DIR *directory = NULL;

  // Chomp on the command line.
  while ((opt = getopt(argc, argv, CMD_OPTIONS)) != -1) {
    switch (opt) {
      // It might be nice to be able to use another user's
      // uid from the command line. You could use -u to
      // indicate the user's logname and -U for the UID.
    case 'h':
      showHelp = TRUE;
      break;
    case 'S':
      sumOnly = TRUE;
      break;
    case 'T':
      // This does not quite work right when combined
      // with -Z. Oh well...
      showThreads = TRUE;
      break;
    case 'Z':
      onlyZombies = TRUE;
      break;
    case '?':
      printf("*** Unrecognized command line option '%s -%c' ***\n"
	     , argv[0], optopt);
      showHelp = TRUE;
      break;
    default :
      printf("*** Something strange happened '%s -%c' ***\n"
	     , argv[0], optopt);
      showHelp = TRUE;
      break;
    }
  }

  if (showHelp) {
    printf("%s: command line options: %s\n", argv[0], CMD_OPTIONS);
    printf("  -h:  show help and exit\n");
    printf("  -S:  show summary counts only\n");
    printf("  -T:  show thread count\n");
    printf("  -Z:  show only zombie processes\n");
    printf("\n$Id: user_procs.c,v 1.9 2015/02/27 04:47:59 chaneyr Exp rjchaney $\n");

    exit(EXIT_SUCCESS);
  }

  // Open the /proc directory for reading.
  printf("Showing processes for user: %-10s  uid: %-8d\n"
	 , getenv("LOGNAME"), getuid());
  directory = opendir(PROC_DIR);
  if (NULL != directory) {
    while((file = readdir(directory))) {
      sprintf(dir_name, "%s/%s", PROC_DIR, file->d_name);
      // If the name of the directory is 'self' it is this
      // process. The process will get picked up by pid
      // later on, so I skip this entry.
      //      if (strcmp(file->d_name, "self") == 0) {
      if (strchr(NUMBERS, file->d_name[0]) == NULL) {
	//fprintf(stderr, "  >> Skipping %s\n", file->d_name);
	continue;
      }

      if (0 == lstat(dir_name, &file_stat)) {
	// If the directory is owned by me, it is one of
	// my processes.
	if (file_stat.st_uid == getuid()) {
	  process_count ++;
	  sprintf(stat_file_name, "%s/%s", dir_name, "status");

	  // Open the 'status' file in the directory and snoop
	  // through it some.
	  status_file = fopen(stat_file_name, "r");

	  if (NULL != status_file) {
	    // The first line in the 'status' file conatins
	    // the name of the command.
	    fgets(line, BUF_SIZE, status_file);
	    line[strlen(line) - 1] = 0;
	    str = strrchr(line, '\t') + 1;
	    sprintf(proc, "Process id: %8s   Command: %s\n", file->d_name, str);
	    if (!sumOnly) {
	      if (!onlyZombies) {
		printf(proc);
	      }
	    }

	    // The second line in the 'status' file contains the
	    // run state of the process. It the run state is
	    // a zombie, make sure to let the user know.
	    fgets(line, BUF_SIZE, status_file);

	    // It is kind of risky to assume that the value is
	    // this very string, but it is okay for a demo.
	    if (NULL != strstr(line, "zombie")) {
	      if (!sumOnly) {
		if (onlyZombies) {
		  printf(proc);
		}
		// Found a zombie!!!
		printf("   ****  THIS PROCESS IS A ZOMBIE  ****\n");
	      }
	      // Just skip forward 3 lines in the file.
	      fgets(line, BUF_SIZE, status_file);
	      fgets(line, BUF_SIZE, status_file);
	      fgets(line, BUF_SIZE, status_file);
	      // Find the parent id of the zombie process.
	      line[strlen(line) - 1] = 0;
	      str = strrchr(line, '\t') + 1;
	      if (!sumOnly) {
		printf("   ****  You need to kill its parent process %s  ****\n"
		       , str);
	      }
	      zombie_count ++;
	    }

	    if ((!sumOnly) && (TRUE == showThreads)) {
	      // Hunt through the 'stauts' file to find out
	      // how many threads this process is running.
	      for ( ; ; ) {
		ch = fgets(line, BUF_SIZE, status_file);
		if (NULL == ch) {
		  break;
		}
		// Again, kind of risky to look for this 
		// string, but ...
		if (NULL != strstr(line, "Threads:")) {
		  line[strlen(line) - 1] = 0;
		  str = strrchr(line, '\t') + 1;
		  printf("  Thread count: %s\n", str);
		  break;
		}
	      }
	    }
	  }
	  else {
	    // This is probably because the process has come
	    // and gone by the time we get to reading it.
	    //printf("could not open >%s<\n", stat_file_name);
	  }
	}
      }
      else {
	perror("**stat failed");
	printf("**stat failed %s\n", dir_name);
      }
    }
  }
  else {
    perror("could not open /proc");
    printf("ERROR: could not open /proc directory");
    exit(EXIT_FAILURE);
  }

  printf("\nYou have %d processes running\n", process_count);
  if (zombie_count > 0) {
    // Is that a bell hear?
    printf(" **  You have %d zombie processes. **\a\n", zombie_count);
  }

  return(EXIT_SUCCESS);
}
