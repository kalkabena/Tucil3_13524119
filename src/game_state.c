#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "game_state.h"
#include "str_utils.h"

char create_board(game_board *board, player_state *player) {
	char input[101];
	int cur_row = 0;
	player->row = -1;
	player->col = -1;
	player->next_goal = 10;
	player->heuristic = 1;
	player->cost = 0;
	player->total = 0;
	board->max_goal = -1;
	
	fputs("Enter board dimensions: ", stdout);
	
	while (fgets(input, sizeof(input), stdin)) {
		char *end;
		board->rows = strtoi(input, &end, 10);
		if (end == input + strlen(input) - 1) {
			fputs("Only one number detected. Try again: ", stdout);
			continue;
		}
		board->cols = strtoi(end, &end, 10);
		if (end == input)  {
			fputs("No numbers detected. Try again: ", stdout);
		} else if (end != input + strlen(input) - 1) {
			fputs("Invalid input. Try again: ", stdout);
		} else if (board->rows <= 0 || board->rows > MAX_ROWS || board->cols <= 0 || board->cols > MAX_COLS) {
			fputs("Inputs out of range. Try again: ", stdout);
		} else {
			break;
		}
	}
	
	board->tiles = (tile_type**)malloc(board->rows * sizeof(tile_type*));
	if (board->tiles == NULL) {
		return 0;
	}
	board->costs = (int**)malloc(board->rows * sizeof(int*));
	if (board->costs == NULL) {
		free_board(board);
		return 0;
	}
	board->goals = (signed char**)malloc(board->rows * sizeof(signed char*));
	if (board->goals == NULL) {
		free_board(board);
		return 0;
	}
	for (int i = 0; i < board->rows; i++) {
		board->tiles[i] = (tile_type*)malloc(board->cols * sizeof(tile_type));
		if (board->tiles[i] == NULL) {
			free_board(board);
			return 0;
		}
		board->costs[i] = (int*)malloc(board->cols * sizeof(int));
		if (board->costs[i] == NULL) {
			free_board(board);
			return 0;
		}
		board->goals[i] = (signed char*)malloc(board->cols * sizeof(signed char));
		if (board->goals[i] == NULL) {
			free_board(board);
			return 0;
		}
	}
	
	for (int i = 0; i < board->rows; i++) {
		for (int j = 0; j < board->cols; j++) {
			board->tiles[i][j] = OBSTACLE;
			board->costs[i][j] = 999999;
			board->goals[i][j] = -1;
		}
	}
	
	puts("Enter board state, allowed chars: *, X, L, Z, O, 0-9");
	
	while (cur_row < board->rows && fgets(input, sizeof(input), stdin)) {
		fputs("|", stdout);
		if (strlen(input) - 1 != board->cols) {
			puts("Input row doesn't match number of board columns");
			continue;
		}
		
		for (int i = 0; i < board->cols; i++) {
			switch (input[i]) {
				case '*':
					board->tiles[cur_row][i] = PATH;
					break;
				case 'x':
				case 'X':
					board->tiles[cur_row][i] = OBSTACLE;
					break;
				case 'l':
				case 'L':
					board->tiles[cur_row][i] = LAVA;
					break;
				case 'z':
				case 'Z':
					if (player->row == -1 && player->col == -1) {
						player->row = cur_row;
						player->col = i;
						board->tiles[cur_row][i] = PATH;
					} else {
						puts("Multiple player symbols detected.");
						i--;
					}
					break;
				case 'o':
				case 'O':
					board->tiles[cur_row][i] = GOAL;
					break;
				default:
					if (input[i] >= '0' && input[i] <= '9') {
						board->goals[cur_row][i] = input[i] - '0';
						board->tiles[cur_row][i] = PATH;
						if (player->next_goal > input[i] - '0') player->next_goal = input[i] - '0';
						if (board->max_goal < input[i] - '0') board->max_goal = input[i] - '0';
					} else {
						puts("Invalid character detected.");
						i--;
					}
					break;
			}
		}
		cur_row++;
	}
	
	cur_row = 0;
	
	puts("Enter board costs");
	
	while (cur_row < board->rows && fgets(input, sizeof(input), stdin)) {
		fputs("*", stdout);
		char *end = input;
		for (int i = 0; i < board->cols; i++) {
			char *end2;
			board->costs[cur_row][i] = strtoi(end, &end2, 10);
			if (end == end2)  {
				fputs("Invalid input.", stdout);
				cur_row--;
				break;
			}
			end = end2;
		}
		
		cur_row++;
	}
	return 1;
}

void free_board(game_board *board) {
	for (int i = 0; i < board->rows; i++) {
		if (board->tiles[i] != NULL) {
			free(board->tiles[i]);
			board->tiles[i] = NULL;
		}
		if (board->costs[i] != NULL) {
			free(board->costs[i]);
			board->costs[i] = NULL;
		}
		if (board->goals[i] != NULL) {
			free(board->goals[i]);
			board->goals[i] = NULL;
		}
	}
	if (board->tiles != NULL) {
		free(board->tiles);
		board->tiles = NULL;
	}
	if (board->tiles != NULL) {
		free(board->costs);
		board->tiles = NULL;
	}
	if (board->goals != NULL) {
		free(board->goals);
		board->goals = NULL;
	}
}

void print_board(game_board *board, player_state *player) {
	for (int i = 0; i < board->rows; i++) {
		for (int j = 0; j < board->cols; j++) {
			if (player->row == i && player->col == j) {
				fputs("Z", stdout);
				continue;
			}
			if (board->goals[i][j] >= 0) {
				printf("%d", board->goals[i][j]);
				continue;
			}
			switch (board->tiles[i][j]) {
				case PATH:
					fputs("*", stdout);
					break;
				case OBSTACLE:
					fputs("X", stdout);
					break;
				case LAVA:
					fputs("L", stdout);
					break;
				case GOAL:
					fputs("O", stdout);
					break;
			}
		}
		puts("");
	}
}

void print_board_to_file(game_board *board, player_state *player, FILE *fp) {
    for (int i = 0; i < board->rows; i++) {
        for (int j = 0; j < board->cols; j++) {
            if (player->row == i && player->col == j) {
                fputs("Z", fp);
                continue;
            }
            if (board->goals[i][j] >= 0) {
                fprintf(fp, "%d", board->goals[i][j]);
                continue;
            }
            switch (board->tiles[i][j]) {
                case PATH:
                    fputs("*", fp);
                    break;
                case OBSTACLE:
                    fputs("X", fp);
                    break;
                case LAVA:
                    fputs("L", fp);
                    break;
                case GOAL:
                    fputs("O", fp);
                    break;
            }
        }
        fputc('\n', fp);
    }
}