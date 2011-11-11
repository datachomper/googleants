#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include "simclist.h"

enum STATE {SETUP, INGAME, END};

int ROWS, COLS;

int loc(int x, int y)
{
	return (x*COLS+y);
}

struct Game {
	enum STATE state;	
	int turn;
	int loadtime;
	int turntime;
	int rows;
	int cols;
	int turns;
	int viewradius2;
	int attackradius2;
	int spawnradius2;
	int player_seed;

	int *watermap;
	int *foodmap;
	int *antmap;
	int *hillmap;
};

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

	for(i=0; i<MAPSIZ; i++) {
		game->watermap[i] = 0;
		game->foodmap[i] = 0;
		game->antmap[i] = 0;
		game->hillmap[i] = 0;
	}
}

void do_preturn(struct Game *game)
{
	int i;
	for(i=0; i<MAPSIZ; i++) {
		game->foodmap[i] = 0;
		game->antmap[i] = 0;
		game->hillmap[i] = 0;
	}
}

void do_turn(struct Game *game)
{
	int r,c;
	for (r=0; r<ROWS; r++) {
		for (c=0; c<COLS; c++) {
			if (game->antmap[loc(r,c)] == 1) {
				fprintf(stderr, "o %d %d N\n", r, c);
				fprintf(stdout, "o %d %d N\n", r, c);
				fflush(NULL);
			}
		}
	}
}

void do_cleanup(struct Game *game)
{
	fflush(NULL);
	free(game);
}

int main()
{
	struct Game *game = malloc(sizeof(struct Game));
	char buf[BUFSIZ];
	char *index;
	char *arg[4];
	int arg_num;

	while (1) {
		fgets(buf, sizeof(buf), stdin);

		index = buf;
		arg_num = 0;
		arg[arg_num] = index;
		for (;*index != '\n'; index++) {
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
			do_cleanup(game);
			return 0;
		}

		if (game->state == SETUP) {
			if (!strcmp(arg[0], "loadtime")) {
				game->loadtime = atoi(arg[1]);
				fprintf(stderr, "Got loadtime %d\n", game->loadtime);
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
					fprintf(stderr, "Got water at %d,%d\n",row,col);
					game->watermap[loc(row,col)] = 1;
					break;
				case 'f':
					row = atoi(arg[1]);
					col = atoi(arg[2]);
					fprintf(stderr, "Got food at %d,%d\n",row,col);
					game->foodmap[loc(row,col)] = 1;
					break;
				case 'h':
					row = atoi(arg[1]);
					col = atoi(arg[2]);
					owner = atoi(arg[3]);
					fprintf(stderr, "Got hill at %d,%d,%d\n",row,col,owner);
					game->hillmap[loc(row,col)] = owner+1;
					break;
				case 'a':
					row = atoi(arg[1]);
					col = atoi(arg[2]);
					owner = atoi(arg[3]);
					fprintf(stderr, "Got ant at %d,%d,%d\n",row,col,owner);
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
	return 0;
}
