#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "api.h"

extern void do_turn(struct Game *game);

char direction(short d)
{
	if (d == 0)
		return 'N';
	else if (d == 1)
		return 'E';
	else if (d == 2)
		return 'S';
	else
		return 'W';
}

int loc(int x, int y)
{
	return (x*COLS+y);
}

int neighbor(int row, int col, enum DIRECTION dir)
{
	switch(dir) {
	case N:
		if (row == 0)
			return loc(ROWS-1, col);
		else
			return loc(row-1, col);
	case E:
		if (col == COLS-1)
			return loc(row, 0);
		else
			return loc(row, col+1);
	case S:
		if (row == ROWS-1)
			return loc(0, col);
		else
			return loc(row+1, col);
	case W:
		if (col == 0)
			return loc(row, COLS-1);
		else
			return loc(row, col-1);
	}
	return 0;
}

void do_setup(struct Game *game)
{
	#define MAPSIZ (game->rows * game->cols * sizeof(int))
	int i;
	
	ROWS = game->rows;
	COLS = game->cols;

	game->watermap = malloc(MAPSIZ);
	game->foodmap = malloc(MAPSIZ);
	game->antmap = malloc(MAPSIZ);
	game->hillmap = malloc(MAPSIZ);

	for(i=0; i<ROWS*COLS; i++) {
		game->watermap[i] = 0;
		game->foodmap[i] = 0;
		game->antmap[i] = 0;
		game->hillmap[i] = 0;
	}
}

void order(int row, int col, enum DIRECTION dir)
{
	fprintf(stdout, "o %d %d %c\n", row, col, direction(dir));
}

void do_preturn(struct Game *game)
{
	int i;
	for(i=0; i<ROWS*COLS; i++) {
		game->foodmap[i] = 0;
		game->antmap[i] = 0;
		game->hillmap[i] = 0;
	}
}

void do_cleanup(struct Game *game)
{
	fflush(NULL);
	free(game->watermap);
	free(game->foodmap);
	free(game->antmap);
	free(game->hillmap);
	free(game);
}

int main()
{
	struct Game *game = malloc(sizeof(struct Game));
	char buf[BUFSIZ];
	char *index;
	char *arg[4];
	int arg_num;

	while (fgets(buf, sizeof(buf), stdin) != NULL) {
		index = buf;
		arg_num = 0;
		arg[arg_num] = index;
		for (;(*index != '\n'); index++) {
			if (*index == ' ') {
				*index = '\0';
				arg[arg_num+1] = index+1;
				arg_num++;
			}
		}
		*index = '\0';

		if (!strcmp(arg[0], "turn")) {
			game->turn = atoi(arg[1]);
			fprintf(stderr, "Got turn %d\n", game->turn);
			if (game->turn) {
				game->state = INGAME;
				do_preturn(game);
			} else {
				game->state = SETUP;
			}
		} else if (!strcmp(arg[0], "end")) {
			break;
		}

		if (game->state == SETUP) {
			if (!strcmp(arg[0], "loadtime")) {
				game->loadtime = atoi(arg[1]);
			} else if (!strcmp(arg[0], "turntime")) {
				game->turntime = atoi(arg[1]);
			} else if (!strcmp(arg[0], "rows")) {
				game->rows = atoi(arg[1]);
			} else if (!strcmp(arg[0], "cols")) {
				game->cols = atoi(arg[1]);
			} else if (!strcmp(arg[0], "turns")) {
				game->turns = atoi(arg[1]);
			} else if (!strcmp(arg[0], "viewradius2")) {
				game->viewradius2 = atoi(arg[1]);
			} else if (!strcmp(arg[0], "attackradius2")) {
				game->attackradius2 = atoi(arg[1]);
			} else if (!strcmp(arg[0], "spawnradius2")) {
				game->spawnradius2 = atoi(arg[1]);
			} else if (!strcmp(arg[0], "player_seed")) {
				game->player_seed = atoi(arg[1]);
			} else if (!strcmp(arg[0], "ready")) {
				do_setup(game);
				fprintf(stdout, "go\n");
				fflush(NULL);
			}
		} else {
			int row,col,owner;
			switch (*arg[0]) {
				case 'w':
					row = atoi(arg[1]);
					col = atoi(arg[2]);
					game->watermap[loc(row,col)] = 1;
					break;
				case 'f':
					row = atoi(arg[1]);
					col = atoi(arg[2]);
					game->foodmap[loc(row,col)] = 1;
					break;
				case 'h':
					row = atoi(arg[1]);
					col = atoi(arg[2]);
					owner = atoi(arg[3]);
					game->hillmap[loc(row,col)] = owner+1;
					break;
				case 'a':
					row = atoi(arg[1]);
					col = atoi(arg[2]);
					owner = atoi(arg[3]);
					game->antmap[loc(row,col)] = owner+1;
					break;
				case 'd':
					// Dead ants, meh
					break;
			}
			if (!strcmp(arg[0], "go")) {
				do_turn(game);
				fprintf(stdout, "go\n");
				fflush(NULL);
			}
		}
	}
	do_cleanup(game);
	return 0;
}
