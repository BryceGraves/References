/*
 * R Jesse Chaney
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

volatile sig_atomic_t child_value = 1;

extern void say_goodbye1( void );
extern void say_goodbye2( void );
void say_goodbye3( int status, void *arg );
extern void handler_simple(int);

int 
main(
     int argc
     , char *argv[]
     , char *envp[]
) {
  pid_t cpid;
  pid_t mypid;
  int i;

  // Establish a signal handler that will catch and ignore
  // all child process exits. I'm using the old (lazy) way
  // to establish this signal handler.
  signal(SIGCHLD, SIG_IGN);
  // Establish a signal handler to catch all SIGINT signals.
  signal(SIGINT, handler_simple);

  // Set the group process id to be the group id of the parent
  // process.
  setpgid(getuid(), getpgrp());

  // Establish an exit handler for the process. Because I set
  // the exit handler here, all the child processes will have 
  // the same exit handler.
  atexit(say_goodbye1);
  atexit(say_goodbye2);

  on_exit(say_goodbye3, NULL);
  
  for (i = 0; i < 3; i++) {
    /*
    if ((cpid = fork()) == 0) {
      // child code
    }
    else if (cpid < 0) {
      // barf code
    }
    else {
      // parent code
    }
    */
    switch((cpid = fork())) {
      // Both parent and child processes continue executing here
    case -1:
      perror("Houston, we have a problem");
      // We had a failure to fork.
      exit(EXIT_FAILURE);
      break;
    case 0:
      mypid = getpid();
      (void) fprintf(stderr, "    CHILD: pid = %d child value: %d line: %d\n"
		     , mypid, child_value, __LINE__);
      // Set the process group id of the child to be the one created by
      // its parent process.
      setpgid(getpid(), getpgrp());
      break;
    default:
      // Parent specific code goes here.
      mypid = getpid();
      fprintf(stderr, "PARENT: pid = %d  child pid = %d\n"
	      , mypid, cpid);

      // Add the newly created child process into the process
      // group.
      //setpgid(cpid, getpgrp());
      child_value++; // This is not an atomic operation.
      break;
    }

    if (cpid == 0) {
      // The child breaks out of the loop.
      break;
    }
  }

  if (cpid == 0) {
    // Just have the child processes pause() until some signal
    // is received.
    pause();
    //sleep(MAX_INT);
    _exit(EXIT_SUCCESS);
  } 
  else {
    child_value = -1;
    sleep(5);
    fprintf(stderr, "PARENT: pid = %d %d\n\n", mypid, __LINE__);
    // Send a SIGINT signal to all members of the process group.
    //kill(0, SIGINT);
    killpg(getpgrp(), SIGINT);
  }

  exit(EXIT_SUCCESS);
  //exit(17);
}

void say_goodbye1( void ) 
{
  // If you have some code that you want to run to clean up before
  // the process exits, this is the place to put it.
  printf("    EXIT HANDLER 1: %d  child value: %d  function: %s  line: %d\n"
	 , (int) getpid(), child_value, __func__, __LINE__);
}

void say_goodbye2( void ) 
{
  // If you have some code that you want to run to clean up before
  // the process exits, this is the place to put it.
  printf("    EXIT HANDLER 2: %d  child value: %d  function: %s  line: %d\n"
	 , (int) getpid(), child_value, __func__, __LINE__);
}

void say_goodbye3( int status, void *arg ) 
{
  // If you have some code that you want to run to clean up before
  // the process exits, this is the place to put it.
  printf("    EXIT HANDLER 3: %d  child value: %d  function: %s  line: %d  exit status: %d\n"
	 , (int) getpid(), child_value, __func__, __LINE__, status);
}

void
handler_simple(
       int sig
) {
  printf("  SIGNAL HANDLER: %d  signal: %d  child value: %2d\n"
	 , (int) getpid(), sig, child_value);
  exit(EXIT_SUCCESS);
}
