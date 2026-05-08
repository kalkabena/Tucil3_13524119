#ifndef GAME_SOLVER_H
#define GAME_SOLVER_H

#include "game_state.h"

typedef enum {
	LEFT,
	UP,
	DOWN,
	RIGHT,
	NONE
} direction;

typedef enum {
	MANHATTAN,
	PYTHAGOREAN,
	CHERBYSHEV
} heuristic_functions;

typedef struct tr* tree_address;
typedef struct tr {
	player_state player;
	tree_address child[4];
	tree_address parent;
	direction last_dir;
	direction solution;
	char is_leaf;
} tree;

tree* create_node(tree *par, player_state *pl, direction dir);

double get_heuristic(game_board *board, player_state *pl, heuristic_functions hf);

player_state try_sliding(game_board *board, player_state *pl, direction dir);

void expand(game_board *board, tree *t, heuristic_functions hf);

tree* find_lowest_cost_leaf(tree* t);

void set_solution_path(tree *t);

int Astar(game_board *board, tree *root, heuristic_functions hf);

void print_tree(tree *t, int depth);

void free_tree(tree *t);

int get_max_depth(tree *t, int starting_depth);

#endif