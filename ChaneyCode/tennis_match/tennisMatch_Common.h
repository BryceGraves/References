/*
 * R Jesse Chaney
 * Jesse@JesseChaney.net
 * Copyright 2014 2015
 */

#ifndef _TENNISMATCH_COMMON_H
# define _TENNISMATCH_COMMON_H

# ifndef FALSE
#  define FALSE 0
# endif // FALSE
# ifndef TRUE
#  define TRUE 1
# endif // TRUE

# define START_PLAY 	10
# define SERVED_BALL 	20
# define RETURN_BALL 	30
# define MISSED_SHOT 	40
# define EXIT		50

# define PLAYER_A 11
# define PLAYER_A_FOREHAND_PROBABLIITY 80
# define PLAYER_A_BACKHAND_PROBABLIITY 80
# define PLAYER_A_SERVE_PROBABLIITY_1 75
# define PLAYER_A_SERVE_PROBABLIITY_2 85
# define PLAYER_A_STR "A"

# define PLAYER_B 12
# define PLAYER_B_FOREHAND_PROBABLIITY 95
# define PLAYER_B_BACKHAND_PROBABLIITY 70
# define PLAYER_B_SERVE_PROBABLIITY_1 80
# define PLAYER_B_SERVE_PROBABLIITY_2 90
# define PLAYER_B_STR "B"

# define MAX_PROBABLIITY 100

# define POINTS_TO_WIN_GAME 4
# define POINTS_TO_WIN_SET 3
# define POINTS_TO_WIN_MATCH 3

# define SLEEP_TIME_SECONDS 1

# define STROKE_FOREHAND_STR "forehand"
# define STROKE_BACKHAND_STR "backhand"

extern void stop_players( void );
extern void create_keys( void );
extern void create_queues( void );
extern void remove_queues( void );
extern void connect_to_queues( void );
extern void create_players( void );
extern void close_queues( void );
extern int flip_coin( void );
extern char *out_long_net( void );

extern void player_sig_int( int sig );
extern void parent_sig_int( int sig );
extern void start_play( int player_to_serve );
extern int rnd ( int max );
extern char *tennis_game_points( int pts );

extern int 
eval_score(
	char *player
	, int *gamescore
	, int *setscore
	, int *matchscore
);
extern int 
main (
	int argc
	, char **argv
	, char **envp
);
extern void 
parse_argv(
	int argc
	, char **argv
);

#endif // _TENNISMATCH_COMMON_H
