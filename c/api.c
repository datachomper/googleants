#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "types.h"
#include "list.h"
#include "api.h"

extern void do_turn(struct Game *game);

static struct square *slist;

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

struct square * astar_neighbor(char *map, struct square *s, enum DIRECTION d)
{
	int row, col;

	row = s->offset / COLS;
	col = s->offset % COLS;

	switch(d) {
	case N:
		if (row == 0)
			row = ROWS-1;
		else
			row = row-1;
		break;
	case E:
		if (col == COLS-1)
			col = 0;
		else
			col = col+1;
		break;
	case S:
		if (row == ROWS-1)
			row = 0;
		else
			row = row+1;
		break;
	case W:
		if (col == 0)
			col = COLS-1;
		else
			col = col-1;
		break;
	}
	return &slist[loc(row,col)];
}

char min(char a, char b) {
	return (a < b) ? a : b;
}

// Calculates the manhattan distance from start to goal
int manhattan(struct square *start, struct square *goal) {
	int diff, dist;
	int x1,y1,x2,y2;

	x1 = start->offset / COLS;
	y1 = start->offset % COLS;
	x2 = goal->offset / COLS;
	y2 = goal->offset % COLS;

	diff = abs(x1-x2);
	dist = min(diff, ROWS-diff);
	diff = abs(y1-y2);
	dist += min(diff, COLS-diff);
	return dist;
}

void astar_init(int rows, int cols)
{
	int offset;

	ROWS = rows;
	COLS = cols;
	slist = malloc(ROWS*COLS*sizeof(struct square));
	for (offset=0; offset<ROWS*COLS; offset++) {
		slist[offset].offset = offset;
		slist[offset].list = FREE;
	}
}

struct square * get_best_f(struct list_head *openlist)
{
	struct square *s;
	struct square *lowest = NULL;
	int lowest_f;

	lowest = list_first_entry(openlist, struct square, astar);
	lowest_f = lowest->g+lowest->h;

	list_for_each_entry(s, openlist, astar) {
		if ((s->g + s->h) < lowest_f)
			lowest = s;
	}
	return lowest;
}

struct square * astar(char *map, struct square *start, struct square *target)
{
	struct square *s, *n;
	LIST_HEAD(openlist);
	int d;

	start->parent = NULL;
	start->h = manhattan(start, target);
	start->g = 0;
	start->list = OPEN;
	list_add(&start->astar, &openlist);

	while ((s = get_best_f(&openlist)) != NULL) {
		if (s == target) {
			// Zip backwards through the tree and set the square
			// to some value to indicate our chosen path
			do {
				map[s->offset] = '0';
				s = s->parent;
			} while (s->parent->parent != NULL);
			return s;
		}
		s->list = CLOSED;
		list_del_init(&s->astar);

		// Add all valid neighbor moves onto the open list
		for (d=0; d<4; d++) {
			n = astar_neighbor(map, s, d);	
			if (map[n->offset] == '%' || n->list == CLOSED)
				continue;
			if (n->list == OPEN) {
				if (s->g+1 < n->g) {
					n->parent = s;
					n->g = s->g + 1;
				}
			} else if (n->list == FREE) {
				list_add(&n->astar, &openlist);
				n->list = OPEN;
				n->h = manhattan(n, target);
				n->g = s->g + 1;
				n->parent = s;
			}
		}
	}
	printf("oops, open list is empty!\n");
	return NULL;
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

int time_remaining(struct Game *game)
{
	struct timespec curr;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &curr);
	return game->turntime - ((float)(curr.tv_nsec - game->timestamp)/1000000);
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
			struct timespec curr;
			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &curr);
			game->timestamp = curr.tv_nsec;
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
				fprintf(stderr, "Turn %d complete with %dms remaining\n",
					game->turn, time_remaining(game));
				fprintf(stdout, "go\n");
				fflush(NULL);
			}
		}
	}
	do_cleanup(game);
	return 0;
}
