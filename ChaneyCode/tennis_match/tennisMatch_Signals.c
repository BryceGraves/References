/*
 * R Jesse Chaney
 * Jesse@JesseChaney.net
 * Copyright 2014 2015
 */

#include "tennisMatch_Signals.h"

static pid_t scorer_pid = 0;
static char *name;
static int return_probalility_forehand;
static int return_probalility_backhand;
static int serve_probalility_1;
static int serve_probalility_2;
static int player_id;

static pid_t playerA_child;
static pid_t playerB_child;

extern int player_to_serve;

extern int playerA_gamescore;
extern int playerB_gamescore;
extern int playerA_setscore;
extern int playerB_setscore;
extern int playerA_matchscore;
extern int playerB_matchscore;
extern int volley_count;
extern int sleep_time_seconds;

// The serve was in.
void 
serve_in( 
	 void 
) {
  // serve is in
  printf("  player %s serving (quiet please...)\n", name);
  fflush(stdout);

  if (player_id == PLAYER_A) {
    kill(scorer_pid, RETURN_VOLLEY);
  }
  else {
    kill(scorer_pid, RETURN_VOLLEY);
  }
}

// the serve was out.
void 
serve_out( 
	  void 
) {
  printf("    player %s second serve is %s\n", name, out_long_net());
  fflush(stdout);

  if (player_id == PLAYER_A) {
    kill(scorer_pid, POINT_PLAYERA);
  }
  else {
    kill(scorer_pid, POINT_PLAYERB);
  }

  sleep(sleep_time_seconds);
}

// serve the ball.
void 
serve_ball( 
	   void 
) {
  int prob = 0;

  prob = rnd(MAX_PROBABLIITY);
  if (prob > serve_probalility_1) {
    // serve is out
    // if the first serve is out, the player gets a second try.
    prob = rnd(MAX_PROBABLIITY);
    if (prob > serve_probalility_2) {
      serve_out();
    }
    else {
      printf("    player %s second serve is in\n", name);
      serve_in();
    }
  }
  else {
    serve_in();
  }
}

// received a volley, return it.
void 
return_ball( 
	    void 
) {
  int prob = 0;
  int rb = 0;
  char *stroke;

  volley_count++;
  prob = rnd(MAX_PROBABLIITY);

  if (rnd(MAX_PROBABLIITY) < HIT_TO_BACKHAND_PROBABLILTY) {
    stroke = STROKE_BACKHAND_STR;
    rb = return_probalility_backhand;
  }
  else {
    stroke = STROKE_FOREHAND_STR;
    rb = return_probalility_forehand;
  }
  prob = rnd(MAX_PROBABLIITY);

  if (prob > rb) {
    // return is out
    printf("  player %s %s return is %s\n", name , stroke, out_long_net());
    fflush(stdout);
    // send a signal to the scorer that the ball was out and the volley over.
    sleep(sleep_time_seconds);
    if (player_id == PLAYER_A) {
      kill(scorer_pid, POINT_PLAYERA);
    }
    else {
      kill(scorer_pid, POINT_PLAYERB);
    }
  }
  else {
    // return is in
    printf("  player %s %s return\n", name , stroke);
    fflush(stdout);
    // send a message to the other player.
    sleep(sleep_time_seconds);
    if (player_id == PLAYER_A) {
      kill(scorer_pid, VOLLEY_PLAYERA);
    }
    else {
      kill(scorer_pid, VOLLEY_PLAYERB);
    }
  }
}

void 
start_play(
	   int player
) {
  if (player == PLAYER_A) {
    printf("  player %s to serve\n", PLAYER_A_STR);
    fflush(stdout);
    kill(playerA_child, SERVE_BALL);
  }
  else {
    printf("  player %s to serve\n", PLAYER_B_STR);
    fflush(stdout);
    kill(playerB_child, SERVE_BALL);
  }
}

int 
scorer(
       int player
) {
  int match_over = FALSE;

  {
    switch(player) {
      // based on from whom the message came, award points.
    case PLAYER_A:
      printf("POINT player %s\n", PLAYER_B_STR);
      fflush(stdout);

      playerB_gamescore++;
      volley_count = 0;
      match_over = eval_score(PLAYER_B_STR, &playerB_gamescore
			      , &playerB_setscore, &playerB_matchscore);

      break;
    case PLAYER_B:
      printf("POINT player %s\n", PLAYER_A_STR);
      fflush(stdout);

      playerA_gamescore++;
      volley_count = 0;
      match_over = eval_score(PLAYER_A_STR, &playerA_gamescore
			      , &playerA_setscore, &playerA_matchscore);

      break;
    default:
      break;
    }
  }
  return(match_over);
}

void 
play( 
     void 
) {
  for ( ; ; ) {		// continue until signal stops play
    sleep(1);	// the call to sleep will always be interrupted by the signals.
  }
}

pid_t 
create_player(
	char *n
	, int p
	, int rf
	, int rb
	, int s_1
	, int s_2
) {
  pid_t child = (pid_t) 0;

  child = fork();
  if (child == 0) {
    // this is the child process
    // the child process does not exec(), but just calls a function.
    signal(MATCH_OVER, player_sig_int);
    signal(RETURN_VOLLEY, player_sig_usr1_return_ball);
    signal(SERVE_BALL, player_sig_usr2_serve_ball);

    name = strdup(n);
    return_probalility_forehand = rf;
    return_probalility_backhand = rb;
    serve_probalility_1 = s_1;
    serve_probalility_2 = s_2;
    player_id = p;

    play();
  }
  else {
    // this is the parent process
    // the parent process just returns
  }

  return(child);
}

void 
create_players( 
	       void 
) {
  playerA_child = create_player(
		PLAYER_A_STR
		, PLAYER_A
		, PLAYER_A_FOREHAND_PROBABLIITY
		, PLAYER_A_BACKHAND_PROBABLIITY
		, PLAYER_A_SERVE_PROBABLIITY_1
		, PLAYER_A_SERVE_PROBABLIITY_2
	);

  playerB_child = create_player(
		PLAYER_B_STR
		, PLAYER_B
		, PLAYER_B_FOREHAND_PROBABLIITY
		, PLAYER_B_BACKHAND_PROBABLIITY
		, PLAYER_B_SERVE_PROBABLIITY_1
		, PLAYER_B_SERVE_PROBABLIITY_2
	);
}

void 
close_queues( 
	     void 
) {
  // empty
  // so i can reuse existing code.
}

void 
stop_players_signals(
	void
) {
  int status;

  kill(playerA_child, MATCH_OVER);
  kill(playerB_child, MATCH_OVER);

  wait(&status);
  wait(&status);
}

void 
player_sig_usr1_return_ball(
    int sig
) {
  // a volley from the other player
  return_ball();
}

void 
player_sig_usr2_serve_ball(
   int sig
) {
  // a request to serve
  serve_ball();
}

void 
scorer_sig_int(
       int sig
) {
  stop_players_signals();
  exit(EXIT_SUCCESS);
}

void 
scorer_sig_usr1_volleyA(
	int sig
) {
  // a volley from the player 1
  kill(playerB_child, RETURN_VOLLEY);
}

void 
scorer_sig_usr2_volleyB(
	int sig
) {
  // a volley from the player 2
  kill(playerA_child, RETURN_VOLLEY);
}

void 
scorer_sig_rtmin1_pointB(
	 int sig
) {
  int match_over = 0;

  // an out from the player 1, point for player 2
  match_over = scorer(PLAYER_A);
  if (match_over) {
    kill(getpid(), MATCH_OVER);
  }
  else {
    start_play(player_to_serve == 0 ? PLAYER_A : PLAYER_B);
  }
}

void 
scorer_sig_rtmin2_pointA(
	 int sig
) {
  int match_over = 0;

  // an out from the player 2, point for player 1
  match_over = scorer(PLAYER_B);
  if (match_over) {
    kill(getpid(), MATCH_OVER);
  }
  else {
    start_play(player_to_serve == 0 ? PLAYER_A : PLAYER_B);
  }
}

int 
main (
	int argc
	, char **argv
	, char **envp
) {
  parse_argv(argc, argv);

  // set up all the signals before any players are craeted
  signal(MATCH_OVER, scorer_sig_int);

  // these signals handle volleys from player to player
  signal(VOLLEY_PLAYERA, scorer_sig_usr1_volleyA);
  signal(VOLLEY_PLAYERB, scorer_sig_usr2_volleyB);

  // these signals handle points
  signal(POINT_PLAYERA, scorer_sig_rtmin1_pointB);
  signal(POINT_PLAYERB, scorer_sig_rtmin2_pointA);

  scorer_pid = getpid();
  create_players();

  //sleep(sleep_time_seconds);
  player_to_serve = flip_coin();

  playerA_gamescore = 0;
  playerB_gamescore = 0;
  playerA_setscore = 0;
  playerB_setscore = 0;
  playerA_matchscore = 0;
  playerB_matchscore = 0;

  printf("\n\nMATCH IS BEGINNING\n\n");
  fflush(stdout);
  sleep(sleep_time_seconds);
  start_play(player_to_serve == 0 ? PLAYER_A : PLAYER_B);

  //while (!exit_condition)
  for ( ; ; ) {
    sleep(1);	// the call to sleep will always be interrupted by the signals.
  }

  return(EXIT_SUCCESS);
}
