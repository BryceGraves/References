/*
 * R Jesse Chaney
 * Jesse@JesseChaney.net
 * Copyright 2014 2015
 */

#include "tennisMatch_SysV.h"

// These are keys and queues.  I declare them static so that they cannot be
// see outside this module.
static key_t scorer_key = (key_t) 0;
static key_t playerA_key = (key_t) 0;
static key_t playerB_key = (key_t) 0;
static int scorer_queue = (int) 0;
static int playerA_queue = (int) 0;
static int playerB_queue = (int) 0;

// These are all defined in the tessnisMatch_Common.c module.  I extern them
// to make the compiler happy.
extern pid_t playerA_children[2];
extern pid_t playerB_children[2];

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
	char *name
	, int to_queue
) {
  mess_t msg;
  int rc = 0;

  // serve is in
  printf("  player %s serving (quiet please...)\n", name);
  fflush(stdout);
  // send a message to the other player.
  msg.mtype = (long) SERVED_BALL;
  msg.msg = (long) SERVED_BALL;
  rc = msgsnd(to_queue, &msg, sizeof(mess_t) - sizeof(long), MSG_NOERROR);
  if (rc == -1) {
    perror("error: serve_ball in");
    exit(EXIT_FAILURE);
  }
  sleep(sleep_time_seconds);
}

// the serve was out.
void 
serve_out(
	char *name
	, int player_id
	, int score_q
) {
  mess_t msg;
  int rc = 0;

  printf("    player %s second serve is %s\n", name, out_long_net());
  fflush(stdout);
  // send a message to the scorer that the ball was out and the volley over.
  msg.mtype = (long) player_id;
  msg.msg = (long) player_id;
  rc = msgsnd(score_q, &msg, sizeof(mess_t) - sizeof(long), MSG_NOERROR);
  if (rc == -1) {
    perror("error: serve_ball out");
    exit(EXIT_FAILURE);
  }
  sleep(sleep_time_seconds);
}

// serve the ball.
void 
serve_ball(
	char *name
	, int player_id
	, int score_q
	, int to_queue
	, int serve_probalility_1
	, int serve_probalility_2
) {
  int prob = 0;

  prob = rnd(MAX_PROBABLIITY);
  if (prob > serve_probalility_1) {
    // serve is out
    printf("  player %s first serve is %s\n", name, out_long_net());
    fflush(stdout);

    // if the first serve is out, the player gets a second try.
    prob = rnd(MAX_PROBABLIITY);
    if (prob > serve_probalility_2) {
      serve_out(name, player_id, score_q);
    }
    else {
      printf("    player %s second serve is in\n", name);
      serve_in(name, to_queue);
    }
  }
  else {
    serve_in(name, to_queue);
  }
}

// received a volley, return it.
void 
return_ball(
	char *name
	, int player_id
	, int score_q
	, int to_queue
	, int return_probalility
	, char *stroke	// forehand or backhand
) {
  int prob = 0;
  int rc;
  mess_t msg;

  volley_count++;
  prob = rnd(MAX_PROBABLIITY);
  if (prob > return_probalility) {
    // return is out
    printf("  player %s %s return is %s\n", name , stroke, out_long_net());
    fflush(stdout);
    // send a message to the scorer that the ball was out and the volley over.
    msg.mtype = (long) player_id;
    msg.msg = (long) player_id;
    rc = msgsnd(score_q, &msg, sizeof(mess_t) - sizeof(long), MSG_NOERROR);
    sleep(sleep_time_seconds);
  }
  else {
    // return is in
    printf("  player %s %s return\n", name , stroke);
    fflush(stdout);
    // send a message to the other player.
    msg.mtype = (long) RETURN_BALL;
    msg.msg = (long) RETURN_BALL;
    rc = msgsnd(to_queue, &msg, sizeof(mess_t) - sizeof(long), MSG_NOERROR);
    sleep(sleep_time_seconds);
  }
  if (rc == -1) {
    perror("error: return_ball");
    exit(EXIT_FAILURE);
  }
}

void 
start_play(
	   int player
) {
  int rc;
  mess_t msg;

  msg.mtype = (long) START_PLAY;
  msg.msg = (long) START_PLAY;
  // send a message to the correct player that a serve is required.
  if (player == PLAYER_A) {
    printf("  player %s to serve\n", PLAYER_A_STR);
    fflush(stdout);
    rc = msgsnd(playerA_queue, &msg, sizeof(mess_t) - sizeof(long), MSG_NOERROR);
  }
  else {
    printf("  player %s to serve\n", PLAYER_B_STR);
    fflush(stdout);
    rc = msgsnd(playerB_queue, &msg, sizeof(mess_t) - sizeof(long), MSG_NOERROR);
  }
  if (rc == -1) {
    perror("error: start_play");
    exit(EXIT_FAILURE);
  }
}

void 
scorer(
       int score_q
) {
  int rc;
  mess_t msg;
  int match_over = FALSE;

  msg.mtype = (long) START_PLAY;
  msg.msg = (long) START_PLAY;
  player_to_serve = flip_coin();
  start_play(player_to_serve);

  // Initialize all scores to zero.
  playerA_gamescore = 0;
  playerB_gamescore = 0;
  playerA_setscore = 0;
  playerB_setscore = 0;
  playerA_matchscore = 0;
  playerB_matchscore = 0;

  printf("\n\nMATCH IS BEGINNING\n\n");
  fflush(stdout);
  sleep(sleep_time_seconds);

  for ( ; ; ) {
    // wait for messages from the players than a volley has
    // ended (ball out, net, or long).
    rc = msgrcv(score_q, &msg, (sizeof(mess_t) - sizeof(long)), 0, MSG_NOERROR);
    if (rc == -1) {
      perror("error: scorer");
      exit(EXIT_FAILURE);
    }
    switch(msg.mtype) {
      // based on from whom the message came, award points.
    case PLAYER_A:
      printf("POINT player %s\n", PLAYER_B_STR);
      fflush(stdout);

      playerB_gamescore++;
      volley_count = 0;
      match_over = eval_score(
			      PLAYER_B_STR
			      , &playerB_gamescore
			      , &playerB_setscore
			      , &playerB_matchscore
			      );
      break;
    case PLAYER_B:
      printf("POINT player %s\n", PLAYER_A_STR);
      fflush(stdout);

      playerA_gamescore++;
      volley_count = 0;
      match_over = eval_score(
			      PLAYER_A_STR
			      , &playerA_gamescore
			      , &playerA_setscore
			      , &playerA_matchscore
			      );
      break;
    default:
      break;
    }
    if (match_over) {
      break;
    }

    // start a new game.
    start_play(player_to_serve);
  }
  kill(getpid(), SIGINT);
}

void 
play(
     char *name
     , int score_q
     , int to_queue
     , int from_queue
     , int return_probalility
     , int serve_probalility_1
     , int serve_probalility_2
     , int player_id
     , char *stroke
) {
  int rc;
  mess_t msg;

  for ( ; ; ) {		// continue until signal stops play
    // players wait on messages from the other player or the scorer
    rc = msgrcv(from_queue, &msg, (sizeof(mess_t) - sizeof(long)), 0, MSG_NOERROR);
    if (rc == -1) {
      perror("error: play");
      exit(EXIT_FAILURE);
    }

    switch(msg.mtype) {
    case START_PLAY:
      serve_ball(
		 name
		 , player_id
		 , score_q
		 , to_queue
		 , serve_probalility_1
		 , serve_probalility_2
		 );
      break;
    case SERVED_BALL:
      // this could be improved so that an ACE could be awarded.
    case RETURN_BALL:
      return_ball(
		  name
		  , player_id
		  , score_q
		  , to_queue
		  , return_probalility
		  , stroke
		  );
      break;
    default:
      printf("%ld: %s %s msgrcv unknown message type", msg.mtype, name, stroke);
      fflush(stdout);
      break;
    }
  }
}

pid_t 
create_stroke(
	char *name
	, int player_id
	, int score_q
	, int from_queue
	, int to_queue
	, int return_probalility
	, int serve_probalility_1
	, int serve_probalility_2
	, char *stroke_name
) {
  pid_t child = (pid_t) 0;
  sig_t sigret = 0;

  child = fork();
  if (child == 0) {
    // this is the child process
    // the child process does not exec(), but just calls a function.
    sigret = signal(SIGINT, player_sig_int);
    if (sigret == SIG_ERR) {
      printf("*** ERROR installing signal handler\n");
      exit(EXIT_FAILURE);
    }
    create_keys( /* no args */ );
    connect_to_queues( /* no args */ );
    play(
	 name
	 , score_q
	 , to_queue
	 , from_queue
	 , return_probalility
	 , serve_probalility_1
	 , serve_probalility_2
	 , player_id
	 , stroke_name
	 );
  }
  else {
    // this is the parent process
    // the parent process just returns
  }

  return(child);
}

void 
create_keys( 
    void 
) {
  // should check for errors.
  // Make use of the the __FILE__ macro to identify the current file.
  scorer_key = ftok(__FILE__, 's');
  playerA_key = ftok(__FILE__, 'a');
  playerB_key = ftok(__FILE__, 'b');
}

void 
close_queues( 
     void 
) {
  // empty
}

void 
remove_queues( 
      void 
) {
  // should check for errors.
  msgctl(scorer_queue, IPC_RMID, NULL);
  msgctl(playerA_queue, IPC_RMID, NULL);
  msgctl(playerB_queue, IPC_RMID, NULL);
  printf("    queues removed %d\n", getpid());
  fflush(stdout);
}

void 
connect_to_queues( 
	  void 
) {
  // should check for errors.
  scorer_queue = msgget(scorer_key, QUEUE_PERMS);
  playerA_queue = msgget(playerA_key, QUEUE_PERMS);
  playerB_queue = msgget(playerB_key, QUEUE_PERMS);
  printf("    queues connected %d\n", getpid());
  fflush(stdout);
}

void 
create_queues( 
	      void 
) {
  // should check for errors.
  scorer_queue = msgget(scorer_key, QUEUE_PERMS | IPC_CREAT);
  playerA_queue = msgget(playerA_key, QUEUE_PERMS | IPC_CREAT);
  playerB_queue = msgget(playerB_key, QUEUE_PERMS | IPC_CREAT);
  printf("queues created %d\n", getpid());
  fflush(stdout);
}

void 
create_players( 
	       void 
) {
  playerA_children[0] = create_stroke(
		PLAYER_A_STR
		, PLAYER_A
		, scorer_queue
		, playerA_queue
		, playerB_queue
		, PLAYER_A_FOREHAND_PROBABLIITY
		, PLAYER_A_SERVE_PROBABLIITY_1
		, PLAYER_A_SERVE_PROBABLIITY_2
		, STROKE_FOREHAND_STR
	);
  playerA_children[1] = create_stroke(
		PLAYER_A_STR
		, PLAYER_A
		, scorer_queue
		, playerA_queue
		, playerB_queue
		, PLAYER_A_BACKHAND_PROBABLIITY
		, PLAYER_A_SERVE_PROBABLIITY_1
		, PLAYER_A_SERVE_PROBABLIITY_2
		, STROKE_BACKHAND_STR
	);

  playerB_children[0] = create_stroke(
		PLAYER_B_STR
		, PLAYER_B
		, scorer_queue
		, playerB_queue
		, playerA_queue
		, PLAYER_B_FOREHAND_PROBABLIITY
		, PLAYER_B_SERVE_PROBABLIITY_1
		, PLAYER_B_SERVE_PROBABLIITY_2
		, STROKE_FOREHAND_STR
	);
  playerB_children[1] = create_stroke(
		PLAYER_B_STR
		, PLAYER_B
		, scorer_queue
		, playerB_queue
		, playerA_queue
		, PLAYER_B_BACKHAND_PROBABLIITY
		, PLAYER_B_SERVE_PROBABLIITY_1
		, PLAYER_B_SERVE_PROBABLIITY_2
		, STROKE_BACKHAND_STR
	);
}

void 
parent_sig_int(
       int sig
) {
  printf("parent caught SIGINT\n");
  fflush(stdout);
  stop_players( /* no args */ );
  close_queues( /* no args */ );
  remove_queues( /* no args */ );
  exit(EXIT_SUCCESS);
}

int 
main (
	int argc
	, char **argv
	, char **envp
) {
  parse_argv(argc, argv);

  create_keys( /* no args */ );
  create_queues( /* no args */ );
  create_players( /* no args */ );

  signal(SIGINT, parent_sig_int);
  sleep(sleep_time_seconds);
  scorer(scorer_queue);

  stop_players( /* no args */ );
  remove_queues( /* no args */ );

  return(EXIT_SUCCESS);
}
