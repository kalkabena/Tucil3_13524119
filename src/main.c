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
	char obj_name[100];
	
	int fch = -1;
	do {
		fputs("0 = Manual input, 1 = File input: ", stdout);
		scanf("%d", &fch);
		while (getchar() != '\n');
	} while (fch < 0 || fch > 1);
	
	if (fch == 0) {
		while (create_board(&board, &player) == 0) {};
	} else {
		FILE *inptr = NULL;
		while (inptr == NULL) {
			fputs("Write absolute path to file (or q to quit): ", stdout);
			scanf("%s", obj_name);
			if ((obj_name[0] == 'q' || obj_name[0] == 'Q') && obj_name[1] == '\0') {
				free_board(&board);
				exit(EXIT_SUCCESS);
			}
			inptr = fopen(obj_name, "r");
			if (inptr == NULL) puts("File not found.");
		}
		while (read_board_from_file(&board, &player, inptr) == 0) {};
	}
	
	tree *root = create_node(NULL, &player, NONE);
	heuristic_functions hf = MANHATTAN;
	int hch = -1;
	do {
		fputs("Input heuristic function[0-2]: ", stdout);
		scanf("%d", &hch);
		while (getchar() != '\n');
	} while (hch < 0 || hch > 2);
	switch (hch) {
		case 0:
			hf = MANHATTAN;
			break;
		case 1:
			hf = CHERBYSHEV;
			break;
		case 2:
			hf = PYTHAGOREAN;
			break;
	}
	clock_t start_time = clock();
	int n_of_its = Astar(&board, root, hf);
	if (n_of_its == -1) {
		puts("Solution not found");
		exit(EXIT_SUCCESS);
	}
	clock_t end_time = clock();
	
	tree *tnode = root;
	
	int tctr = 0;
	
	while (tnode->is_leaf == 0) {
		char cc;
		switch (tnode->last_dir) {
			case LEFT:
				cc = 'L';
				break;
			case RIGHT:
				cc = 'R';
				break;
			case UP:
				cc = 'U';
				break;
			case DOWN:
				cc = 'D';
				break;
		}
		printf( "Step %d, Input:%c\n", tctr, cc);
		print_board(&board, &(tnode->player));
		puts("");
		tctr++;
		tnode = tnode->child[tnode->solution];
	}
	
	char ccc = 0;
	switch (tnode->last_dir) {
		case LEFT:
			ccc = 'L';
			break;
		case RIGHT:
			ccc = 'R';
			break;
		case UP:
			ccc = 'U';
			break;
		case DOWN:
			ccc = 'D';
			break;
	}
	printf("Step %d, Input:%c\n", tctr, ccc);
	print_board(&board, &(tnode->player));
	puts("");
	
	fputs("Solution: ", stdout);
	tnode = root;
	while (tnode->is_leaf == 0) {
		switch (tnode->last_dir) {
			case LEFT:
				fputs("L", stdout);
				break;
			case RIGHT:
				fputs("R", stdout);
				break;
			case UP:
				fputs("U", stdout);
				break;
			case DOWN:
				fputs("D", stdout);
				break;
		}
		tnode = tnode->child[tnode->solution];
	}
	switch (tnode->last_dir) {
		case LEFT:
			fputs("L", stdout);
			break;
		case RIGHT:
			fputs("R", stdout);
			break;
		case UP:
			fputs("U", stdout);
			break;
		case DOWN:
			fputs("D", stdout);
			break;
	}
	printf("\nJumlah iterasi: %d\n", n_of_its);
	printf("Cost: %d\n", tnode->player.cost);
	printf("Elapsed time: %.2lf ms\n\n", (double)(end_time - start_time) / CLOCKS_PER_SEC * 1000);
	fputs("Playback? (y/n): ", stdout);
	int yn;
	char flag = 0;
	yn = getchar();
	if (yn == 'y' || yn == 'Y') flag = 1;
	
	tree *cur_node = root;
	
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
	
	FILE *outptr;
	
	outptr = fopen("test/output.txt", "w");
	fprintf(outptr, "\xEF\xBB\xBF");
	
	tree *tempnode = root;
	
	int tempctr = 0;
	
	while (tempnode->is_leaf == 0) {
		char cc;
		switch (tempnode->last_dir) {
			case LEFT:
				cc = 'L';
				break;
			case RIGHT:
				cc = 'R';
				break;
			case UP:
				cc = 'U';
				break;
			case DOWN:
				cc = 'D';
				break;
		}
		fprintf(outptr, "Step %d, Input:%c\n", tempctr, cc);
		print_board_to_file(&board, &(tempnode->player), outptr);
		fputc('\n', outptr);
		tempctr++;
		tempnode = tempnode->child[tempnode->solution];
	}
	
	char cc = 0;
	switch (tempnode->last_dir) {
		case LEFT:
			cc = 'L';
			break;
		case RIGHT:
			cc = 'R';
			break;
		case UP:
			cc = 'U';
			break;
		case DOWN:
			cc = 'D';
			break;
	}
	fprintf(outptr, "Step %d, Input:%c\n", tempctr, cc);
	print_board_to_file(&board, &(tnode->player), outptr);
	fputc('\n', outptr);
	
	fclose(outptr);
	free_tree(root);
	
	free_board(&board);
	
	return 0;
}