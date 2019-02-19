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
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>  
#include <errno.h>

#include "tennisMatch_Common.h"

#ifndef _TENNISMATCH_POSIX_H
# define _TENNISMATCH_POSIX_H

// It would be good to make these unique per user, using the uid in the
// names would work nicely.
# define PLAYER_A_QUEUE_NAME "/playerA_queue_POSIX"
# define PLAYER_B_QUEUE_NAME "/playerB_queue_POSIX"
# define SCORER_QUEUE_NAME "/scorer_queue_POSIX"

extern void 
serve_ball(
	char *name
	, int player_id
	, mqd_t score_q
	, mqd_t to_queue
	, int serve_probalility_1
	, int serve_probalility_2
);
extern void 
return_ball(
	char *name
	, int player_id
	, mqd_t score_q
	, mqd_t to_queue
	, int return_probalility
	, char *stroke
);
extern void 
scorer(
	mqd_t score_q
);
extern void 
play(
	char *name
	, mqd_t score_q
	, mqd_t to_queue
	, mqd_t from_queue
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
	, mqd_t score_q
	, mqd_t from_queue
	, mqd_t to_queue
	, int return_probalility
	, int serve_probalility_1
	, int serve_probalility_2
	, char *stroke_name
);
extern void 
serve_in(
	char *name
	, mqd_t to_queue
);
extern void 
serve_out(
	char *name
	, int player_id
	, mqd_t score_q
);
extern void size_queues( void );

#endif // _TENNISMATCH_POSIX_H
