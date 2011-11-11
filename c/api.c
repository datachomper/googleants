#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include "simclist.h"

enum STATE {SETUP, INGAME, END};

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
};

void do_setup(struct Game *game)
{
}

void do_turn(struct Game *game)
{
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
			if (game->turn)
				game->state = INGAME;
			else
				game->state = SETUP;
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
			}
		} else {
			switch (*arg[0]) {
				case 'w':
					fprintf(stderr, "Got water at %d,%d\n", atoi(arg[1]),atoi(arg[2]));
					break;
				case 'f':
					break;
				case 'h':
					break;
				case 'a':
					break;
				case 'd':
					break;
			}
			if (!strcmp(arg[0], "go")) {
				do_turn(game);
			}
		}
	}
	return 0;
}
