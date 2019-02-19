/*
 * R Jesse Chaney
 */

// Adapted from
// http://www.opensourceforu.com/2011/04/getting-started-with-inotify/

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
// This uses the Linux specifc inotify() API. //////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <limits.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

// Max. number of events to process at one go
#define MAX_EVENTS 1024

// Assume that the length of the filename won't exceed 16 bytes
#define LEN_NAME 16

// size of one event
#define EVENT_SIZE  (sizeof(struct inotify_event))

// buffer to store the data of events
#define BUF_LEN     (MAX_EVENTS * (EVENT_SIZE + LEN_NAME))

// I make these (module) global so that I can close them in the
// signal handler.
static int fd = -1;
static int wd = -1;

static void sigint_handler(int sig);
static void remove_notify( void );

int 
main(int argc, char *argv[])
{
  int length;
  int event_index = 0;
  int event_count = 0;
  unsigned int total_events = 0;
  unsigned int create_events = 0;
  char *dir_to_monitor = NULL;
  char buffer[BUF_LEN] = {0};

  if (argc < 2) {
    // If you don't identify a directory on the command line,
    // assume you want to watch the current directory.
    dir_to_monitor = strdup(".");
    //strcpy(buffer, ".");
  }
  else {
    //strcpy(buffer, argv[1]);
    dir_to_monitor = strdup(argv[1]);
  }

  (void) atexit(remove_notify);
  (void) signal(SIGINT, sigint_handler);

  // Initialize inotify() system.  We have to do this before we can use
  // the inotify interface.
  fd = inotify_init();
  if (fd < 0) {
    perror("Couldn't initialize inotify");
    exit(EXIT_FAILURE);
  }
 
  /* add watch to starting directory */
  fprintf(stderr, "Adding inotify for '%s'\n", dir_to_monitor);

  // You can get a complete list of all the possible events and their
  // meaning by doing 'man 7 initify'.
  wd = inotify_add_watch(fd
			 , dir_to_monitor
//#define EVERYTHING
#ifdef EVERYTHING
			 , IN_ALL_EVENTS
#else // EVERYTHING
			 // get notice for all the followng events.
			 //   IN_CREATE: new files/directories
			 //   IN_MODIFY: modified files
			 //   IN_DELETE: deleted files/directories
			 //   IN_ATTRIB: meta-data changes files/directories
			 , IN_CREATE 
			  | IN_MODIFY 
			  | IN_DELETE 
			  | IN_ATTRIB
			  | IN_CLOSE_WRITE
			  | IN_CLOSE_NOWRITE
# define INCLUDE_MOVE
# ifdef INCLUDE_MOVE
			  | IN_MOVED_FROM
			  | IN_MOVED_TO
			 // File moved out of watched directory
			 // File moved into watched directory
# endif // INCLUDE_MOVE
#endif // EVERYTHING
			 );
  if (-1 == wd) {
    fprintf(stderr, "Couldn't add watch to %s\n", dir_to_monitor);
    perror("add watch failed");
    exit(EXIT_FAILURE);
  }
  else {
    fprintf(stderr, "Watching: '%s'\n", dir_to_monitor);
  }
 
  // Loop until a control-C is pressed.
  for ( ; ; ) {
    // Read from the file descriptor we created to watch the directory.
    length = read(fd, buffer, BUF_LEN);
    event_count = 0;
    total_events ++;
 
    if (length < 0) {
      perror("read error on the event notifier fd");
      exit(EXIT_FAILURE);
    }
    if (0 == length) {
      printf("zero length event list read\n");
      continue;
    }
    event_index = 0;
    while (event_index < length) {
      // Pull out an event from the list of returned events.
      struct inotify_event *event = (struct inotify_event *) &buffer[event_index];

      event_count ++;
      if (event->len > 0) {
	// Make sure we have an event with some content.

	if (event->mask & IN_CREATE) {
	  // A new file or directory was created.
	  if (event->mask & IN_ISDIR) {
	    printf( "  A new directory %s was created.\n", event->name);
	    printf( "    The contents of the directory are not being monitored.\n");
	    // If we wanted, we could add this new directory into our
	    // watch and be norified about changes to it as well.
	  }
	  else {
	    create_events++;
	    printf( "  A new file %s was created  %d.\n"
		    , event->name, create_events);
	    {
	      char *ext = strrchr(event->name, '.');
	      if (ext) {
		if (strcasecmp(ext, ".xml") == 0) {
		  printf("    Oh my! It is an xml file. Call the data loader...\a\n");
		}
	      }
	    }
	  }
	}
	else if (event->mask & IN_MODIFY) {
	  // The contents of a file was changed.
	  if (event->mask & IN_ISDIR) {
	    // I don't think this will even be called.  From what I read
	    // of the API, this is not generated for a directory.
	    printf( "  The directory %s was modified.\n", event->name);
	  }
	  else {
	    printf( "  The file %s was modified.\n", event->name);
	  }
	}
	else if (event->mask & IN_DELETE) {
	  // A file or directory was deleted (unlink-ed).
	  if (event->mask & IN_ISDIR) {
	    printf( "  The directory %s was deleted.\n", event->name);
	    // If we add a notofication about new directories being
	    // created in our watch list, we also need to remove
	    // them if deleted.
	  }
	  else {
	    printf( "  The file %s was deleted.\n", event->name);
	  }
	}
	else if (event->mask & IN_ATTRIB) {
	  // The meta-data for the file/directory was changed.
	  // The meta-data includes: permissions, timestamps, extended attributes...
	  if (event->mask & IN_ISDIR) {
	    printf( "  The directory %s had meta-data changed.\n", event->name);
	  }
	  else {
	    printf( "  The file %s had meta data changed.\n", event->name);
	  }
	}
	else if (event->mask & IN_MOVED_FROM) {
	  if (event->mask & IN_ISDIR) {
	    printf( "  The directory %s moved out, cookie %u.\n"
		    , event->name, event->cookie);
	  }
	  else {
	    printf( "  The file %s moved out, cookie %u.\n"
		    , event->name, event->cookie);
	  }
	}
	else if (event->mask & IN_MOVED_TO) {
	  if (event->mask & IN_ISDIR) {
	    printf( "  The directory %s was moved in, cookie %u.\n"
		    , event->name, event->cookie);
	  }
	  else {
	    printf( "  The file %s was moved in, cookie %u.\n"
		    , event->name, event->cookie);

	    {
	      char *ext = strrchr(event->name, '.');
	      if (ext) {
		if (strcasecmp(ext, ".xml") == 0) {
		  printf("    Ah haaa! Another xml file moved in. Enqueue the data file...\a\n");
		}
	      }
	    }

	  }
	}
	else if (event->mask & IN_CLOSE_WRITE) {
	  if (event->mask & IN_ISDIR) {
	    printf( "  The directory %s was closed.\n", event->name);
	  }
	  else {
	    printf( "  The file %s (opened for write) was closed.\n", event->name);
	  }
	}
	else if (event->mask & IN_CLOSE_NOWRITE) {
	  if (event->mask & IN_ISDIR) {
	    printf( "  The directory %s was closed.\n", event->name);
	  }
	  else {
	    printf( "  The file %s (opened for no-write) was closed.\n", event->name);
	  }
	}
	else {
	  // Hmmmmm...  We received a event that we are not managing
	  // correctly.
	  printf( "  Some other event occured on %s\n", event->name);
	}

	// Skip ahead to the next event in the list.
	event_index += EVENT_SIZE + event->len;
      }
      else {
	// I'm not sure why, but I need to have this in here.
	// When I use the TAB key for filename completion I get
	// an event generated that falls into this bucket.
	// This is an event with a length == 0.
	break;
      }
    }
    printf("done with event block  %d  %d\n", event_count, total_events);
  }
}

static void
sigint_handler(int sig)
{
  // Terminate program.
  // The atexit() function will handle closing stuff.
  exit(EXIT_SUCCESS);
}

static void
remove_notify( void )
{
  // Clean up.
  fprintf(stderr, "\nCleaning up the inotify structures\n");
  if ((fd > 0) && (wd > 0)) {
    inotify_rm_watch(fd, wd);
    close(fd);
  }
}
