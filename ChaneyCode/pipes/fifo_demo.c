/*
 * R Jesse Chaney
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <libgen.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <linux/limits.h>
#include <stdlib.h>

# define FIFO_FILENAME    "TalkerFifo__"
# define FIFO_PERMISSIONS  S_IRUSR | S_IWUSR

# define FIFO_NAME(_BUF_) sprintf((_BUF_),"%s/%s",getenv("HOME"),FIFO_FILENAME);

# define PROMPT    ">>> "

#ifndef NULL
# define NULL 0
#endif // NULL
#ifndef FALSE
# define FALSE 0
#endif // FALSE
#ifndef TRUE
# define TRUE 1
#endif // TRUE


static int isClient = TRUE;
// I set this up as a global name so that can get to it from the
// at exit function.
static char fifoName[PATH_MAX];

extern void sigint_handler(int);
extern void remove_pipes(void);
extern void talker( void );
extern void listener( void );

void 
sigint_handler( int sig )
{
    fprintf(stderr, "signal hanler called: %d\n", (int) getpid());
    exit(EXIT_SUCCESS);
}

void
remove_pipes( void )
{
    fprintf(stderr, "  exit hanlder called: %d\n", getpid());
    unlink(fifoName);
}

void
talker( void ) {
    int fifoFd = -1;
    //ssize_t result = -1;
    char buffer[200];
    char *r;

    // Create the name of the FIFO using the macro.
    FIFO_NAME(fifoName);
    // Create the FIFO
    mkfifo(fifoName, FIFO_PERMISSIONS);
    // Open the FIFO.  This call to open() will block until the
    // read side is opened.
    fifoFd = open(fifoName, O_WRONLY);
    for ( ; ; ) {
        fputs(PROMPT, stdout);
        // Read a line of input text from the user and send it to the
        // other process.
        r = fgets(buffer, sizeof(buffer), stdin);
        if (NULL == r) break;  // EOF

        // Send the text to the other process using the FIFO.
        write(fifoFd, buffer, strlen(buffer));
    }
    close(fifoFd);
}

void
listener( void ) {
    int fifoFd;
    ssize_t result;
    char buffer[200];

    // Create the name of the FIFO.
    FIFO_NAME(fifoName);
    // Open the FIFO.  The FIFO will have been created already by the
    // talker process.
    fifoFd = open(fifoName, O_RDONLY);
    for ( ; ; ) {
        fputs("+++ ", stdout);
        // Read the FIFO for input.
        result = read(fifoFd, buffer, sizeof(buffer));
        if (0 == result) break;  // EOF

        // Make sure the buffer is a NULL terminiated string.
        buffer[result] = 0;
        // Just print the input.
        fputs(buffer, stdout);
    }
}

int
main(int argc, char *argv[]) 
{
    int opt;

    umask(0);
    if (argc < 2) {
        printf("Must give command line option -t or -l\n");
        exit(EXIT_FAILURE);
    }
    while ((opt = getopt(argc, argv, "lt")) != -1) {
        switch (opt) {
        case 'l':
            isClient = TRUE;
            break;
        case 't':
            isClient = FALSE;
            break;
        default:
            fprintf(stderr
                    , "*** opt:%c ptarg: <%s> optind: %d opterr: %d optopt: %d ***\n"
                    , opt, optarg, optind, opterr, optopt);
            break;
        }
    }

    if (isClient == FALSE) {
        // If this is a "server" establish an exit handler for the process and
        // a signal handler.
        (void) atexit(remove_pipes);
        (void) signal(SIGINT, sigint_handler);
        talker();
    }
    else {
        listener();
    }

    return(EXIT_SUCCESS);
}
