/*
 * R Jesse Chaney
 */

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

static char *song_lines[] = {
  "Whether you're a brother or whether you're a mother"
  , "You're stayin' alive, stayin' alive"
  , "Feel the city breakin' and everybody shakin'"
  , "And we're stayin' alive, stayin' alive"
  , "Ah, ha, ha, ha, stayin' alive, stayin' alive"
  , "Ah, ha, ha, ha, stayin' alive"
  , NULL
};

void stayin_alive(
    int sig
    , siginfo_t *siginfo
    , void *ucontext
);

int main(int argc, char *argv[]) 
{
  struct sigaction stayin_alive_handler;
  struct sigaction old_handler;

  stayin_alive_handler.sa_sigaction = stayin_alive;
  sigfillset(&stayin_alive_handler.sa_mask);
  stayin_alive_handler.sa_flags = SA_SIGINFO;
  stayin_alive_handler.sa_flags |= SA_RESTART;

  sigaction(SIGINT, &stayin_alive_handler, &old_handler);
  sigaction(SIGTERM, &stayin_alive_handler, &old_handler);

  while (pause())
    ;

  return(EXIT_SUCCESS);
}

void stayin_alive(
    int sig
    , siginfo_t *siginfo
    , void *ucontext
) {
  static int song_line = 0;

  fprintf(stderr, "  %s\n", song_lines[song_line++]);

  if (song_lines[song_line] == NULL) {
    fprintf(stderr, "\n");
    song_line = 0;
  }
}
