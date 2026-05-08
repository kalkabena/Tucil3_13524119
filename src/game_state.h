#ifndef GAME_STATE_H
#define GAME_STATE_H

#define MAX_ROWS 100
#define MAX_COLS 100

typedef enum {
	PATH,
	OBSTACLE,
	LAVA,
	GOAL
} tile_type;

typedef struct {
	tile_type **tiles;
	int **costs;
	signed char **goals;	
	int rows;
	int cols;
	signed char max_goal;
} game_board;

typedef struct {
	double heuristic;
	double total;
	int row;
	int col;
	int cost;
	signed char next_goal;
} player_state;

char create_board(game_board *board, player_state *player);

void free_board(game_board *board);

void print_board(game_board *board, player_state *player);

#endif