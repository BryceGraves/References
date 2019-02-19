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

# define FIFO_FILENAME    "ChatFifo__"
# define FIFO_PERMISSIONS  S_IRUSR | S_IWUSR

# define FIFO_NAME(_BUF_,_suffix) sprintf((_BUF_),"%s/%s_%s",getenv("HOME"),FIFO_FILENAME,_suffix);

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


static int isWriter = TRUE;
// I set this up as a global name so that can get to it from the
// atexit() function.
static char fifoNameW[PATH_MAX] = { '\0' };
static char fifoNameR[PATH_MAX] = { '\0' };

extern void sigint_handler(int);
extern void remove_pipes(void);
extern void writer( void );
extern void reader( void );

int
main(int argc, char *argv[])
{
    int opt;

    umask(0);
    if (argc < 2) {
        printf("Must give command line option -w or -r\n");
        exit(EXIT_FAILURE);
    }
    while ((opt = getopt(argc, argv, "wr")) != -1) {
        switch (opt) {
        case 'w':
            isWriter = TRUE;
            break;
        case 'r':
            isWriter = FALSE;
            break;
        default:
            fprintf(stderr
                    , "*** opt:%c ptarg: <%s> optind: %d opterr: %d optopt: %d ***\n"
                    , opt, optarg, optind, opterr, optopt);
            break;
        }
    }

    if (isWriter == TRUE) {
        // If this is a "writer" establish an exit handler for the process and
        // a signal handler.
        (void) atexit(remove_pipes);
        (void) signal(SIGINT, sigint_handler);
        writer();
    }
    else {
        reader();
    }

    return(EXIT_SUCCESS);
}

void 
sigint_handler( int sig )
{
    fprintf(stderr, "++  signal hanler called: %d\n", (int) getpid());
    exit(EXIT_SUCCESS);
}

void
remove_pipes( void )
{
    fprintf(stderr, "++  exit hanlder called: %d\n", getpid());
    unlink(fifoNameW);
    unlink(fifoNameR);
}

void
writer( void ) {
    int fifoFdW;
    int fifoFdR;
    ssize_t result;
    char buffer[200];
    char *r;

    // Create the name of the FIFO using the macro.
    FIFO_NAME(fifoNameW,"w");
    FIFO_NAME(fifoNameR,"r");
    // Create the FIFO
    mkfifo(fifoNameW, FIFO_PERMISSIONS);
    mkfifo(fifoNameR, FIFO_PERMISSIONS);
    // Open the FIFO. This call to open() will block until the
    // read side is opened.
    fifoFdW = open(fifoNameW, O_WRONLY);
    fifoFdR = open(fifoNameR, O_RDONLY);
    for ( ; ; ) {
        fputs(PROMPT, stdout);
        // Read a line of input text from the user and send it to the
        // other process.
        r = fgets(buffer, sizeof(buffer), stdin);
        if (NULL == r) {
            break;  // EOF
        }

        // Send the text to the other process using the FIFO.
        result = write(fifoFdW, buffer, strlen(buffer));
        memset(buffer, 0, sizeof(buffer));
        fputs("listening... \n", stdout);
        result = read(fifoFdR, buffer, sizeof(buffer));
        if (0 == result) {
            break;
        }
        fputs("reply>>", stdout);
        fputs(buffer, stdout);
    }
    close(fifoFdW);
    close(fifoFdR);
}

void
reader( void ) {
    int fifoFdW;
    int fifoFdR;
    ssize_t result;
    char buffer[200];
    int count = 0;

    // Create the name of the FIFO.
    FIFO_NAME(fifoNameW,"w");
    FIFO_NAME(fifoNameR,"r");

    for (count = 0; (count < 100) && (access(fifoNameW, FIFO_PERMISSIONS) == 0)
             && (access(fifoNameR, FIFO_PERMISSIONS) == 0); count++)
    {
        sleep(1);
    }

    // Open the FIFO. The FIFO will have been created already by the
    // writer process.
    fifoFdW = open(fifoNameW, O_RDONLY);
    fifoFdR = open(fifoNameR, O_WRONLY);

    for ( ; ; ) 
    {
        char *r;
        fputs("listening... \n", stdout);
        // Read the FIFO for input.
        result = read(fifoFdW, buffer, sizeof(buffer));
        if (0 == result) {
            break;  // EOF
        }

        // Make sure the buffer is a NULL terminiated string.
        buffer[result] = 0;
        // Just print the input.
        fputs(buffer, stdout);
        fputs(PROMPT, stdout);
        r = fgets(buffer, sizeof(buffer), stdin);
        if (NULL == r) {
            break;
        }
        result = write(fifoFdR, buffer, strlen(buffer));
    }
}
