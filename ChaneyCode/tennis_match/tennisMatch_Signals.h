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

#ifndef _TENNISMATCH_SIGNALS_H
# define _TENNISMATCH_SIGNALS_H

# define HIT_TO_BACKHAND_PROBABLILTY 67

# define RETURN_VOLLEY	SIGUSR1
# define SERVE_BALL		SIGUSR2
# define POINT_PLAYERA	(SIGRTMIN + 1)
# define POINT_PLAYERB	(SIGRTMIN + 2)

# define VOLLEY_PLAYERA SIGUSR1
# define VOLLEY_PLAYERB SIGUSR2

# define MATCH_OVER 	SIGINT

extern void serve_ball( void );
extern void return_ball( void );
extern int scorer(int player);
extern void play( void );
extern void serve_in( void );
extern void serve_out( void );

extern void player_sig_usr1_return_ball(int sig);
extern void player_sig_usr2_serve_ball(int sig);
extern void scorer_sig_int(int sig);
extern void scorer_sig_usr1_volleyA(int sig);
extern void scorer_sig_usr2_volleyB(int sig);
extern void scorer_sig_rtmin2_pointA(int sig);
extern void scorer_sig_rtmin1_pointB(int sig);

extern void stop_players_signals( void );
extern pid_t 
create_player(
	char *n
	, int p
	, int rf
	, int rb
	, int s_1
	, int s_2
);

#endif // _TENNISMATCH_SIGNALS_H
