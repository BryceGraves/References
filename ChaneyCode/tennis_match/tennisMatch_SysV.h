/*
 * R Jesse Chaney
 * Jesse@JesseChaney.net
 * Copyright 2014 2015
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <getopt.h>

#include "tennisMatch_Common.h"

#ifndef _TENNISMATCH_SYSV_H
# define _TENNISMATCH_SYSV_H

# define QUEUE_PERMS 0644

typedef struct tennismsgbuf {
  long mtype;
  long msg;
} mess_t;


extern void scorer( int score_q );
extern void 
serve_ball(
	char *name
	, int player_id
	, int score_q
	, int to_queue
	, int serve_probalility_1
	, int serve_probalility_2
);
extern void 
return_ball(
	char *name
	, int player_id
	, int score_q
	, int to_queue
	, int return_probalility
	, char *stroke
);
extern void 
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
);
extern pid_t 
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
);
extern void 
serve_in(
	char *name
	, int to_queue
);
extern void 
serve_out(
	char *name
	, int player_id
	, int score_q
);

#endif // _TENNISMATCH_SYSV_H
