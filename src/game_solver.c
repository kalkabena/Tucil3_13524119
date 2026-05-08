#include "game_solver.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

tree* create_node(tree *par, player_state *pl, direction dir) {
	tree *t = (tree*)malloc(sizeof(tree));
	if (t == NULL) {
		return NULL;
	}
	t->parent = par;
	t->player = *pl;
	t->last_dir = dir;
	for (int i = 0; i < 4; i++) {
		t->child[i] = NULL;
	}
	t->solution = NONE;
	t->is_leaf = 1;
	return t;
}

double get_heuristic(game_board *board, player_state *pl, heuristic_functions hf) {
	switch (hf) {
		case MANHATTAN:
			double mh = board->rows + board->cols;
			for (int i = 0; i < board->rows; i++) {
				for (int j = 0; j < board->cols; j++) {
					if ((pl->next_goal < 10 && board->goals[i][j] == pl->next_goal) || (pl->next_goal == 10 && board->tiles[i][j] == GOAL)) {
						int temp = abs(i - pl->row) + abs(j - pl->col);
						if (mh > temp) mh = temp;
					}
				}
			}
			return mh;
			break;
		case PYTHAGOREAN:
			double py = board->rows + board->cols;
			for (int i = 0; i < board->rows; i++) {
				for (int j = 0; j < board->cols; j++) {
					if ((pl->next_goal < 10 && board->goals[i][j] == pl->next_goal) || (pl->next_goal == 10 && board->tiles[i][j] == GOAL)) {
						double temp = sqrt((i - pl->row) * (i - pl->row) + (j - pl->col) * (j - pl->col));
						if (py > temp) py = temp;
					}
				}
			}
			return py;
			break;
		case CHERBYSHEV:
			double ch = board->rows + board->cols;
			for (int i = 0; i < board->rows; i++) {
				for (int j = 0; j < board->cols; j++) {
					if ((pl->next_goal < 10 && board->goals[i][j] == pl->next_goal) || (pl->next_goal == 10 && board->tiles[i][j] == GOAL)) {
						int temp = abs(i - pl->row);
						if (abs(j - pl->col) > temp) temp = abs(j - pl->col);
						if (ch > temp) ch = temp;
					}
				}
			}
			return ch;
			break;
	}
}

player_state try_sliding(game_board *board, player_state *pl, direction dir) {
	puts("k");
	player_state p = *pl;
	signed char horz = 0;
	signed char vert = 0;
	switch (dir) {
		case LEFT:
			horz = -1;
			vert = 0;
			break;
		case RIGHT:
			horz = 1;
			vert = 0;
			break;
		case DOWN:
			horz = 0;
			vert = 1;
			break;
		case UP:
			horz = 0;
			vert = -1;
			break;
	}
	
	if (p.row + vert < 0 || p.row + vert >= board->rows ||
		p.col + horz < 0 || p.col + horz >= board->cols ||
		board->tiles[p.row + vert][p.col + horz] == LAVA ||
		board->goals[p.row + vert][p.col + horz] > p.next_goal) {
		p.row = -1;
		p.col = -1;
		return p;
	}
	
	while (board->tiles[p.row + vert][p.col + horz] != OBSTACLE) {
		if (p.row + vert < 0 || p.row + vert >= board->rows ||
		p.col + horz < 0 || p.col + horz >= board->cols ||
		board->tiles[p.row + vert][p.col + horz] == LAVA ||
		board->goals[p.row + vert][p.col + horz] > p.next_goal) {
			p.row = -1;
			p.col = -1;
			break;
		}
		if (board->goals[p.row + vert][p.col + horz] == p.next_goal) p.next_goal++;
		if (p.next_goal > board->max_goal) p.next_goal = 10;
		p.row += vert;
		p.col += horz;
		p.cost += board->costs[p.row][p.col];
		if (p.row + vert < 0 || p.row + vert >= board->rows ||
		p.col + horz < 0 || p.col + horz >= board->cols ||
		board->tiles[p.row + vert][p.col + horz] == LAVA ||
		board->goals[p.row + vert][p.col + horz] > p.next_goal) {
			p.row = -1;
			p.col = -1;
			break;
		}
	}
	if (pl->row == p.row && pl->col == p.col) {
		p.row = -1;
		p.col = -1;
	}
	puts("j");
	return p;
}

void expand(game_board *board, tree *t, heuristic_functions hf) {
	puts("d");
	t->is_leaf = 0;
	char flag = 0;
	for (int i = 0; i < 4; i++) {
		player_state pl = try_sliding(board, &(t->player), i);
		//if (t->parent != NULL && pl.row == t->parent->player.row && pl.col == t->parent->player.col) continue;
		if (pl.row != -1 && pl.col != -1) {
			flag = 1;
			t->child[i] = create_node(t, &pl, i);
			t->child[i]->player.heuristic = get_heuristic(board, &pl, hf);
			t->child[i]->player.total = t->child[i]->player.heuristic + t->child[i]->player.cost;
		}
	}
	if (flag == 0) t->is_leaf = 2;
}

tree* find_lowest_cost_leaf(tree* t) {
	if (t->is_leaf == 1) {
		return t;
	} else {
		tree *out = NULL;
		int total = INT_MAX;
		signed char best_goal = -1;
		//printf("%d %d %d %lf %lf %d |%d\n", t->player.row, t->player.col, t->player.cost, t->player.heuristic, t->player.total, t->player.next_goal, t->is_leaf);
		for (int i = 0; i < 4; i++) {
			if (t->child[i] == NULL) continue;
			//printf("|%d %d %d %lf %lf %d |%d\n", t->child[i]->player.row, t->child[i]->player.col, t->child[i]->player.cost, t->child[i]->player.heuristic, t->child[i]->player.total, t->child[i]->player.next_goal, t->child[i]->is_leaf);
			if (t->child[i]->is_leaf == 1) {
				if (t->child[i]->player.next_goal > best_goal) {
					best_goal = t->child[i]->player.next_goal;
					total = t->child[i]->player.total;
					out = t->child[i];
				} else if (t->child[i]->player.total < total && t->child[i]->player.next_goal == best_goal) {
					total = t->child[i]->player.total;
					out = t->child[i];
				}
			} else if (t->child[i]->is_leaf == 0) {
				tree *temp = find_lowest_cost_leaf(t->child[i]);
				if (temp != NULL) {
					if (temp->player.next_goal > best_goal) {
						best_goal = temp->player.next_goal;
						total = temp->player.total;
						out = temp;
					} else if (temp->player.total < total && temp->player.next_goal == best_goal) {
						total = temp->player.total;
						out = temp;
					}
				}
			}
		}
		return out;
	}
}

void set_solution_path(tree *t) {
	if (t->parent != NULL) {
		t->parent->solution = t->last_dir;
		set_solution_path(t->parent);
	}
}

int Astar(game_board *board, tree *root, heuristic_functions hf) {
	int i = 0;
	while (i < 9999999) {
		tree *cur_working_node = find_lowest_cost_leaf(root);
		if (cur_working_node == NULL) {
			i = -1;
			break;
		}
		printf("%d %d %d %lf %lf %d\n===\n", cur_working_node->player.row, cur_working_node->player.col, cur_working_node->player.cost, cur_working_node->player.heuristic, cur_working_node->player.total, cur_working_node->player.next_goal);
		if (cur_working_node->player.heuristic <= 0.000001) {
			set_solution_path(cur_working_node);
			break;
		}
		
		expand(board, cur_working_node, hf);
		print_tree(root, 0);
		i++;
	}
	if (i >= 9999999) i = -1;
	return i;
}

void print_tree(tree *t, int depth) {
	printf("%*s%d %d %d %lf %lf %d |%d\n", depth, "", t->player.row, t->player.col, t->player.cost, t->player.heuristic, t->player.total, t->player.next_goal, t->is_leaf);
	for (int i = 0; i < 4; i++) {
		if (t->child[i] != NULL) print_tree(t->child[i], depth + 1);
	}
}

void free_tree(tree *t) {
	for (int i = 0; i < 4; i++) {
		if (t->child[i] != NULL) {
			free_tree(t->child[i]);
			t->child[i] = NULL;
		}
	}
	free(t);
}

int get_max_depth(tree *t, int starting_depth) {
	int m = starting_depth;
	for (int i = 0; i < 4; i++) {
		if (t->child[i] != NULL) {
			int tt = get_max_depth(t->child[i], starting_depth + 1);
			if (tt > m) m = tt;
		}
	}
	return m;
}