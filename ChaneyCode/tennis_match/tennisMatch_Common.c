/*
 * R Jesse Chaney
 * Jesse@JesseChaney.net
 * Copyright 2014 2015
 */

// I could probably pull out some of these include files.
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>

#include "tennisMatch_Common.h"

// It would be a good idea to create a struct to hold all of these.
int player_to_serve = -1;
int playerA_gamescore = 0;
int playerB_gamescore = 0;
int playerA_setscore = 0;
int playerB_setscore = 0;
int playerA_matchscore = 0;
int playerB_matchscore = 0;

// this could be a single 2 dim array.  Or, even better, dynamically allocated.
pid_t playerA_children[2];
pid_t playerB_children[2];

// In order to make use of this, it either needs to be part of the payload in
// in the messages between players, or placed in shared memory.
int volley_count = 0;

int sleep_time_seconds =  SLEEP_TIME_SECONDS;

int 
rnd(
	int max
) {
  // generate a random number >= 0 and < max.
  int r = random();
  int ret = r % max;

  return (ret);
}

int 
flip_coin(
	void
) {
  return (rnd(2));
}

void 
stop_players(
	void
) {
  int status;

  printf("  scorer stop_players called\n");
  fflush(stdout);
  kill(playerA_children[0], SIGINT);
  kill(playerA_children[1], SIGINT);
  kill(playerB_children[0], SIGINT);
  kill(playerB_children[1], SIGINT);

  wait(&status);
  wait(&status);
  wait(&status);
  wait(&status);
}

void 
player_sig_int(
		    int sig
) {
  printf("  player caught SIGINT: %d\n", getpid());
  fflush(stdout);
  close_queues();
  exit(EXIT_SUCCESS);
}

// just return a random string for the ball being out/long/net.
char *
out_long_net(
	void
) {
  int p = rnd(3);

  if (p == 0) {
    return("out");
  }
  if (p == 1) {
    return("long");
  }
  return("in the net");
}

// I've never understood this progression of: love, 15, 30, 40, game
char *
tennis_game_points(
			 int pts
) {
  if (pts == 0) {
    return("love");
  }
  if (pts == 1) {
    return(" 15 ");
  }
  if (pts == 2)	{
    return(" 30 ");
  }
  return(" 40 ");
}

int 
eval_score(
	char *player
	, int *gamescore
	, int *setscore
	, int *matchscore
) {
  int game_over = FALSE;

  if ((*gamescore) >= POINTS_TO_WIN_GAME) {
    printf("    GAME to %s\n", player);
    fflush(stdout);
    (*gamescore) = 0;
    (*setscore)++;
    if ((*setscore) >= POINTS_TO_WIN_SET) {
      printf("    SET  to %s\n", player);
      printf("    SCORE games :   %d     %d\n", playerA_setscore, playerB_setscore);
      fflush(stdout);
      (*setscore) = 0;
      (*matchscore)++;
      // switch servers
      player_to_serve = (player_to_serve + 1) % 2;
      if ((*matchscore) >= POINTS_TO_WIN_MATCH) {
	printf("    SCORE sets  :   %d     %d\n", playerA_matchscore, playerB_matchscore);
	printf("    GAME-SET-MATCH to %s\n\n", player);
	fflush(stdout);

	game_over = TRUE;
      }
    }
    playerA_gamescore = 0;
    playerB_gamescore = 0;
  }
  else {
    printf("                   A     B\n");
    printf("    SCORE points: %s  %s\n"
	   , tennis_game_points(playerA_gamescore)
	   , tennis_game_points(playerB_gamescore)
	   );
    printf("    SCORE games :   %d     %d\n", playerA_setscore, playerB_setscore);
    printf("    SCORE sets  :   %d     %d\n", playerA_matchscore, playerB_matchscore);
    fflush(stdout);
  }

  return(game_over);
}

void 
parse_argv(
	int argc
	, char **argv
) {
  char c;
  int random_initalized = FALSE;

  while ((c = getopt (argc, argv, "rRs:")) != -1) {
    switch (c) {
    case 'r':
      srandom(255);
      random_initalized = TRUE;
      break;
    case 'R':
      srandom((unsigned int) time(NULL));
      random_initalized = TRUE;
      break;
    case 's':
      // should REALLY check to make sure this a reasonable int.
      sleep_time_seconds = atoi(optarg);
      printf("setting sleep time to: %d\n", sleep_time_seconds);
      fflush(stdout);
      break;
    case '?':
      break;
    default:
      break;
    }
  }

  if (! random_initalized) {
    srandom((unsigned int) time(NULL));
  }
}
