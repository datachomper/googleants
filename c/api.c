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

int loc(int x, int y) {
	return (x*COLS+y);
}

int loc2offset(struct loc *a) {
	return loc(a->x,a->y);
}

char min(char a, char b) {
	return (a < b) ? a : b;
}

// Calculates the manhattan distance from start to goal
int manhattan(struct loc *start, struct loc *goal) {
	int diff, dist;
	int x1,y1,x2,y2;

	x1 = start->x;
	y1 = start->y;
	x2 = goal->x;
	y2 = goal->y;

	diff = abs(x1-x2);
	dist = min(diff, ROWS-diff);
	diff = abs(y1-y2);
	dist += min(diff, COLS-diff);
	return dist;
}

int manhattan2(struct loc *start, struct loc *goal) {
	int diff, a, b;
	int x1,y1,x2,y2;

	x1 = start->x;
	y1 = start->y;
	x2 = goal->x;
	y2 = goal->y;

	diff = abs(x1-x2);
	a = min(diff, ROWS-diff);
	diff = abs(y1-y2);
	b = min(diff, COLS-diff);
	return a*a+b*b;
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

int astar(int *map, struct loc *a, struct loc *b, struct loc *next)
{
	struct square *s, *n;
	struct square *start, *target;
	LIST_HEAD(openlist);
	int d, offset;

	for (offset=0; offset<ROWS*COLS; offset++) {
		slist[offset].list = FREE;
	}

	start = &slist[loc2offset(a)];
	target = &slist[loc2offset(b)];

	start->parent = NULL;
	start->h = manhattan(&start->loc, &target->loc);
	start->g = 0;
	start->list = OPEN;
	list_add(&start->astar, &openlist);

	while ((s = get_best_f(&openlist)) != NULL) {
		if (s == target) {
			// Zip backwards through the tree and set the square
			// to some value to indicate our chosen path
			do {
				s = s->parent;
			} while (s->parent->parent != NULL);
			memcpy(next, &s->loc, sizeof(*next));
			return 0;
		}
		s->list = CLOSED;
		list_del_init(&s->astar);

		// Add all valid neighbor moves onto the open list
		for (d=0; d<4; d++) {
			n = &slist[neighbor(s->loc.x, s->loc.y, d)];
			if (map[loc2offset(&n->loc)] == 1 || n->list == CLOSED)
				continue;
			if (n->list == OPEN) {
				if (s->g+1 < n->g) {
					n->parent = s;
					n->g = s->g + 1;
				}
			} else if (n->list == FREE) {
				list_add(&n->astar, &openlist);
				n->list = OPEN;
				n->h = manhattan(&n->loc, &target->loc);
				n->g = s->g + 1;
				n->parent = s;
			}
		}
	}
	printf("oops, open list is empty!\n");
	return -1;
}

int time_remaining(struct Game *game)
{
	struct timespec curr;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &curr);
	return game->turntime - ((float)(curr.tv_nsec - game->timestamp)/1000000);
}

void do_setup(struct Game *game)
{
	int offset;
	#define MAPSIZ (game->rows * game->cols * sizeof(int))

	ROWS = game->rows;
	COLS = game->cols;

	/* slist is used for astar */
	slist = malloc(ROWS*COLS*sizeof(*slist));
	for (offset=0; offset<ROWS*COLS; offset++) {
		slist[offset].loc.x = offset/COLS;
		slist[offset].loc.y = offset%COLS;
		slist[offset].list = FREE;
	}

	game->watermap = malloc(MAPSIZ);
	game->foodmap = malloc(MAPSIZ);
	game->antmap = malloc(MAPSIZ);
	game->hillmap = malloc(MAPSIZ);
	game->viewmap = malloc(MAPSIZ);

	game->ant_i = malloc(sizeof(*game->ant_i)*ROWS*COLS);
	game->food_i = malloc(sizeof(*game->food_i)*ROWS*COLS);

	memset(game->watermap, 0, MAPSIZ);
	memset(game->foodmap, 0, MAPSIZ);
	memset(game->antmap, 0, MAPSIZ);
	memset(game->hillmap, 0, MAPSIZ);
	memset(game->viewmap, 0, MAPSIZ);

	memset(game->ant_i, 0, sizeof(*game->ant_i)*ROWS*COLS);
	memset(game->food_i, 0, sizeof(*game->food_i)*ROWS*COLS);

	INIT_LIST_HEAD(&game->ant_l);
	INIT_LIST_HEAD(&game->food_l);
	INIT_LIST_HEAD(&game->goals);
	INIT_LIST_HEAD(&game->freeants);
	INIT_LIST_HEAD(&game->freefood);
	INIT_LIST_HEAD(&game->freegoals);
}

void order(int row, int col, enum DIRECTION dir)
{
	fprintf(stdout, "o %d %d %c\n", row, col, direction(dir));
}

void do_preturn(struct Game *game)
{
	memset(game->foodmap, 0, sizeof(*game->foodmap));
	memset(game->antmap, 0, sizeof(*game->antmap));
	memset(game->hillmap, 0, sizeof(*game->hillmap));
	memset(game->viewmap, 0, sizeof(*game->viewmap));

	/* Cleanup the food and ants list, rebuild from scratch */
	list_splice_init(&game->food_l, &game->freefood);
	list_splice_init(&game->ant_l, &game->freeants);
	list_splice_init(&game->goals, &game->freegoals);
}

void do_cleanup(struct Game *game)
{
	fflush(NULL);
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

					struct food *f;
					if (list_empty(&game->freefood)) {
						f = malloc(sizeof(*f));
						list_add_tail(&f->node, &game->food_l);
					} else {
						f = list_first_entry(&game->freefood, struct food, node);
						list_move(&f->node, &game->food_l);	
					}
					f->loc.x = row;
					f->loc.y = col;
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

					int x,y;
					struct loc a = {row,col};
					for (x=0;x<ROWS;x++) {
						for (y=0;y<COLS;y++) {
							if (!game->viewmap[loc(x,y)]) {
								struct loc b = {x,y};
								int dist2 = manhattan2(&a,&b);
								if (dist2 <= game->viewradius2)
									game->viewmap[loc(x,y)] = 1;
							}
						}
					}

					if (!owner) {
						struct ant *a;
						if (list_empty(&game->freeants)) {
							a = malloc(sizeof(*a));
							list_add_tail(&a->node, &game->ant_l);
						} else {
							a = list_first_entry(&game->freeants, struct ant, node);
							list_move(&a->node, &game->ant_l);
						}
						a->loc.x = row;
						a->loc.y = col;
						game->ant_i[loc(row,col)] = a;
					}
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
