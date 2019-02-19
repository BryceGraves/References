/*
 * R Jesse Chaney
 * Jesse@JesseChaney.net
 * Copyright 2014 2015
 */

#include "tennisMatch_PThreads.h"

static mqd_t scorer_queue;
static mqd_t playerA_queue;
static mqd_t playerB_queue;

static pthread_t playerA_childrenThread[2];
static pthread_t playerB_childrenThread[2];

static char player_a_queue_name[100];
static char player_b_queue_name[100];
static char scorer_queue_name[100];

extern int player_to_serve;

extern int playerA_gamescore;
extern int playerB_gamescore;
extern int playerA_setscore;
extern int playerB_setscore;
extern int playerA_matchscore;
extern int playerB_matchscore;
extern int volley_count;
extern int sleep_time_seconds;

void 
serve_in(
	char *name
	, mqd_t to_queue
) {
  int rc = 0;
  long msg;

  // serve is in
  printf("  player %s serving (quiet please...)\n", name);
  fflush(stdout);
  msg = (long) SERVED_BALL;
  rc = mq_send(to_queue, (char *) &msg, sizeof(msg), 1);
  if (rc == -1) {
    perror("error: serve_ball in");
    exit(EXIT_FAILURE);
  }
  sleep(sleep_time_seconds);
}

void 
serve_out(
	char *name
	, int player_id
	, mqd_t score_q
) {
  int rc = 0;
  long msg;

  printf("    player %s second serve is %s\n", name, out_long_net());
  fflush(stdout);
  msg = (long) player_id;
  rc = mq_send(score_q, (char *) &msg, sizeof(msg), 1);
  if (rc == -1) {
    perror("error: serve_ball out");
    exit(EXIT_FAILURE);
  }
  sleep(sleep_time_seconds);
}

void 
serve_ball(
	char *name
	, int player_id
	, mqd_t score_q
	, mqd_t to_queue
	, int serve_probalility_1
	, int serve_probalility_2
) {
  int prob = 0;

  prob = rnd(MAX_PROBABLIITY);
  if (prob > serve_probalility_1) {
    // serve is out
    printf("  player %s first serve is %s\n", name, out_long_net());
    fflush(stdout);

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

void 
return_ball(
	char *name
	, int player_id
	, mqd_t score_q
	, mqd_t to_queue
	, int return_probalility
	, char *stroke
) {
  int prob = 0;
  int rc;
  long msg;

  volley_count++;
  prob = rnd(MAX_PROBABLIITY);
  if (prob > return_probalility) {
    // serve is out
    printf("  player %s %s return is %s\n", name , stroke, out_long_net());
    fflush(stdout);
    msg = (long) player_id;
    rc = mq_send(score_q, (char *) &msg, sizeof(msg), 1);
    sleep(sleep_time_seconds);
  }
  else {
    // serve is in
    printf("  player %s %s return\n", name , stroke);
    fflush(stdout);
    msg = (long) RETURN_BALL;
    rc = mq_send(to_queue, (char *) &msg, sizeof(msg), 1);
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
  long msg = (long) START_PLAY;

  if (player == PLAYER_A) {
    printf("  player %s to serve\n", PLAYER_A_STR);
    fflush(stdout);
    rc = mq_send(playerA_queue, (char *) &msg, sizeof(msg), 1);
  }
  else {
    printf("  player %s to serve\n", PLAYER_B_STR);
    fflush(stdout);
    rc = mq_send(playerB_queue, (char *) &msg, sizeof(msg), 1);
  }
  if (rc == -1) {
    perror("error: start_play");
    exit(EXIT_FAILURE);
  }
}

void 
scorer(
       mqd_t score_q
) {
  int rc;
  long msg[10000];
  int exit_called = FALSE;
  int match_over = FALSE;

  player_to_serve = flip_coin();
  start_play(player_to_serve);

  playerA_gamescore = 0;
  playerB_gamescore = 0;
  playerA_setscore = 0;
  playerB_setscore = 0;
  playerA_matchscore = 0;
  playerB_matchscore = 0;

  printf("\n\nMATCH IS BEGINNING\n\n");
  fflush(stdout);
  fflush(stdout);
  sleep(sleep_time_seconds);

  for ( ; ; ) {
    rc = mq_receive(score_q, (char *) &msg, sizeof(msg), NULL);
    if (rc == -1) {
      perror("error: scorer");
      exit(EXIT_FAILURE);
    }
    switch(msg[0]) {
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
    case EXIT:
      printf("    scorer exit received\n");
      exit_called = TRUE;
      match_over = TRUE;
      break;
    default:
      break;
    }
    if (exit_called || match_over) {
      break;	// break from the for(;;)
    }

    start_play(player_to_serve);
  }
  //	kill(getpid(), SIGINT);
  raise(SIGINT);
}

void *
play(
	void *_player_info
) {
  player_t *player_info = (player_t *) _player_info;
  int rc;
  long msg[10000];
  int exit_called = FALSE;

  fflush(stdout);
  for ( ; ; ) {		// continue until signal stops play
    rc = mq_receive(player_info->from_queue, (char *) &msg, sizeof(msg), NULL);
    if (rc == -1) {
      perror("error: play mq_receive");
      exit(EXIT_FAILURE);
    }

    switch(msg[0]) {
    case START_PLAY:
      serve_ball(
		 player_info->name
		 , player_info->player_id
		 , player_info->score_q
		 , player_info->to_queue
		 , player_info->serve_probalility_1
		 , player_info->serve_probalility_2
		 );
      break;
    case SERVED_BALL:
      // this could be improved so that an ACE could be awarded.
    case RETURN_BALL:
      return_ball(
		  player_info->name
		  , player_info->player_id
		  , player_info->score_q
		  , player_info->to_queue
		  , player_info->return_probalility
		  , player_info->stroke_name
		  );
      break;
    case EXIT:
      printf("    %s %s exit received\n"
	     , player_info->name
	     , player_info->stroke_name);
      fflush(stdout);
      exit_called = TRUE;
      break;
    default:
      printf("    %s %s play unknown message type\n"
	     , player_info->name
	     , player_info->stroke_name);
      fflush(stdout);
      break;
    }
    if (exit_called) {
      break;	// break from the for(;;)
    }
  }

  return(_player_info);
}

pthread_t 
create_stroke(
	char *name
	, int player_id
	, mqd_t score_q
	, mqd_t from_queue
	, mqd_t to_queue
	, int return_probalility
	, int serve_probalility_1
	, int serve_probalility_2
	, char *stroke_name
) {
  pthread_t child = (pthread_t) 0;
  int ret = 0;
  player_t *player_info = (player_t *) malloc(sizeof(player_t));

  player_info->name = name;
  player_info->player_id = player_id;
  player_info->score_q = score_q;
  player_info->from_queue = from_queue;
  player_info->to_queue = to_queue;
  player_info->return_probalility = return_probalility;
  player_info->serve_probalility_1 = serve_probalility_1;
  player_info->serve_probalility_2 = serve_probalility_2;
  player_info->stroke_name = stroke_name;

  ret = pthread_create(
		&child
		, NULL
		, &play
		, (void *) player_info
	);
  if (ret != 0) {
    perror("error: create_stroke");
    exit(EXIT_FAILURE);
  }

  return(child);
}

void 
close_queues( 
	     void 
) {
  // should check for errors.
  mq_close(scorer_queue);
  mq_close(playerA_queue);
  mq_close(playerB_queue);
  printf("    queues closed %d\n", getpid());
  fflush(stdout);
}

void 
remove_queues( 
	      void 
) {
  // should check for errors.
  mq_unlink(scorer_queue_name);
  mq_unlink(player_a_queue_name);
  mq_unlink(player_b_queue_name);
  printf("    queues removed %d\n", getpid());
  fflush(stdout);
}

void 
connect_to_queues( 
		  void 
) {
  scorer_queue = mq_open(scorer_queue_name, O_RDWR);
  playerA_queue = mq_open(player_a_queue_name, O_RDWR);
  playerB_queue = mq_open(player_b_queue_name, O_RDWR);
  printf("  queues opened %d\n", getpid( /* no args */ ));
  fflush(stdout);
}

// This is interesting if you want to know the size of the queues, but you
// cannot change much.
void 
size_queues( 
	    void 
) {
  struct mq_attr attr;

  mq_getattr(scorer_queue, &attr);
  printf("scorer_queue: %ld %ld %ld\n", attr.mq_msgsize, attr.mq_maxmsg, attr.mq_curmsgs);
  mq_getattr(scorer_queue, &attr);

  mq_getattr(playerA_queue, &attr);
  printf("playerB_queue: %ld %ld %ld\n", attr.mq_msgsize, attr.mq_maxmsg, attr.mq_curmsgs);

  mq_getattr(playerA_queue, &attr);
  printf("playerB_queue: %ld %ld %ld\n", attr.mq_msgsize, attr.mq_maxmsg, attr.mq_curmsgs);
  fflush(stdout);
}

void 
create_queues(
	void
) {
  // should check for errors.
  int oflag = (O_RDWR | O_CREAT);
  mode_t mode = (S_IRUSR | S_IWUSR);
  struct mq_attr *attr = NULL;

  sprintf(scorer_queue_name, "%s__%d", SCORER_QUEUE_NAME, getuid());
  sprintf(player_a_queue_name, "%s__%d", PLAYER_A_QUEUE_NAME, getuid());
  sprintf(player_b_queue_name, "%s__%d", PLAYER_B_QUEUE_NAME, getuid());

  scorer_queue = mq_open(scorer_queue_name, oflag, mode, attr);
  playerA_queue = mq_open(player_a_queue_name, oflag, mode, attr);
  playerB_queue = mq_open(player_b_queue_name, oflag, mode, attr);

  //size_queues();
  printf("queues created %d\n", getpid());
  fflush(stdout);
}

void 
create_players( 
       void 
) {
  {
    // player A
    playerA_childrenThread[0] = create_stroke(
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
    playerA_childrenThread[1] = create_stroke(
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
  }
  {
    // player B
    playerB_childrenThread[0] = create_stroke(
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
    playerB_childrenThread[1] = create_stroke(
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
}

void 
stop_players_threads( 
     void 
) {
  long msg;

  printf("  scorer stop_players called\n");
  fflush(stdout);

  msg = (long) EXIT;
  mq_send(scorer_queue, (char *) &msg, sizeof(msg), 1);
  mq_send(playerA_queue, (char *) &msg, sizeof(msg), 1);
  mq_send(playerA_queue, (char *) &msg, sizeof(msg), 1);
  mq_send(playerB_queue, (char *) &msg, sizeof(msg), 1);
  mq_send(playerB_queue, (char *) &msg, sizeof(msg), 1);

  pthread_join(playerA_childrenThread[0], NULL);
  pthread_join(playerA_childrenThread[1], NULL);
  pthread_join(playerB_childrenThread[0], NULL);
  pthread_join(playerB_childrenThread[1], NULL);
}

void 
parent_sig_int(
       int sig
) {
  printf("somebody caught SIGINT\n");
  fflush(stdout);
  stop_players_threads();
}

int 
main (
	int argc
	, char **argv
	, char **envp
) {
  parse_argv(argc, argv);

  create_queues( /* no args */ );
  create_players( /* no args */ );
  //size_queues();

  // i should probably install a SIGCHLD handler too.  I'd have to
  // keep track of if the child is exiting on an error or as expected.
  signal(SIGINT, parent_sig_int);
  sleep(sleep_time_seconds);
  scorer(scorer_queue);

  stop_players_threads( /* no args */ );
  close_queues( /* no args */ );
  remove_queues( /* no args */ );

  return(EXIT_SUCCESS);
}
