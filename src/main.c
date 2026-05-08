#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game_state.h"
#include "game_solver.h"

static void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    // Assume POSIX (Linux, macOS, etc.)
    system("clear");
#endif
}

int main() {
	game_board board;
	player_state player;
	
	if (create_board(&board, &player) != 0) {
		tree *root = create_node(NULL, &player, NONE);
		clock_t start_time = clock();
		Astar(&board, root, MANHATTAN);
		clock_t end_time = clock();
		//print_tree(root, 0);
		
		tree *cur_node = root;
		char flag = 1;
		int cur_depth = 0;
		while (flag == 1) {
			int in;
			clearScreen();
			print_board(&board, &(cur_node->player));
			fputs("Input: ", stdout);
			in = getchar();
			switch (in) {
				case ',':
					if (cur_node->parent != NULL) {
						cur_node = cur_node->parent;
						cur_depth--;
					}
					break;
				case '.':
					if (cur_node->is_leaf == 0) {
						cur_node = cur_node->child[cur_node->solution];
						cur_depth++;
					}
					break;
				case 'q':
					flag = 0;
					break;
				case 'e':
					int step = -1;
					do {
						printf("Input step: ");
						scanf("%d", &step);
						while (getchar() != '\n');
				    } while (step < 0 || step > get_max_depth(root, 0));
					while (cur_depth > step) {
						cur_node = cur_node->parent;
						cur_depth--;
					}
					while (cur_depth < step) {
						cur_node = cur_node->child[cur_node->solution];
						cur_depth++;
					}
			}
		}
		printf("Elapsed time: %.2lf ms\n\n", (double)(end_time - start_time) / CLOCKS_PER_SEC * 1000);
		
		free_tree(root);
	}
	
	
	free_board(&board);
	
	return 0;
}