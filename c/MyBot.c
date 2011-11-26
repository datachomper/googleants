#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "api.h"
#include "list.h"
#include "list_sort.h"

void write_img(int *map, const char *name)
{
	FILE *fp;
	int r,c,val;
	int x,y;
	#define ZOOM 5

	fp = fopen(name, "w");
	fprintf(stderr, "rows %d cols %d\n", ROWS, COLS);
	fprintf(fp, "P3\n%d %d\n255\n", COLS*ZOOM, ROWS*ZOOM);
	for (r=0; r<ROWS; r++) {
		for (x=0; x<ZOOM; x++) {
			for (c=0; c<COLS; c++) {
				val = map[loc(r,c)];
				for (y=0; y<ZOOM; y++) {
					if (val == 0)
						fprintf(fp, "%03d %03d %03d ",0,0,0);
					else if ((val>0) && val<255)
						fprintf(fp, "%03d %03d %03d ",100,100,100);
					else
						fprintf(fp, "%03d %03d %03d ",255,255,255);
				}
			}
			fprintf(fp, "\n");
		}
	}
	fclose(fp);
}

int loc_dist(struct loc *a, struct loc *b) {
	int diff, dist;

	diff = abs(a->x - b->x);
	dist = min(diff, ROWS-diff);
	diff = abs(a->y - b->y);
	dist += min(diff, COLS-diff);
	return dist;
}

void loc_neighbor(struct loc *s, struct loc *n, enum DIRECTION d)
{
	int row, col;
	row = s->x;
	col = s->y;

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
	n->x = row;
	n->y = col;
}

enum DIRECTION loc_dir(struct loc *from, struct loc *to) {
	int d;
	struct loc tmp;

	for (d=0; d<4; d++) {
		loc_neighbor(from, &tmp, d);
		if (!memcmp(&tmp, to, sizeof(tmp)))
			return d;
	}
	return 0;
}

void order_loc(struct loc *ant, struct loc *to) {
	order(ant->x, ant->y, loc_dir(ant,to));
}

static struct ant *find_nearest_idle_ant(struct Game *game, struct loc *loc)
{
	struct ant *ant;
	struct ant *closest = NULL;
	int lowest = -1;

	list_for_each_entry(ant, &game->ant_l, node) {
		int dist = loc_dist(loc, &ant->loc);
		if ((lowest == -1) || dist < lowest) {
			lowest = dist;
			closest = ant;
		}
	}
	return closest;
}

int cmp_food(void *pvt, struct list_head *a, struct list_head *b) {
	struct Game *game = pvt;
	struct food *food_a = container_of(a, struct food, node);
	struct food *food_b = container_of(b, struct food, node);
	struct ant *ant_a = find_nearest_idle_ant(game, &food_a->loc);
	struct ant *ant_b = find_nearest_idle_ant(game, &food_b->loc);
	
	if (!ant_a || !ant_b)
		return 0;

	return (loc_dist(&food_a->loc, &ant_a->loc) - 
		    loc_dist(&food_b->loc, &ant_b->loc));
}

void do_turn(struct Game *game)
{
	struct food *f;
	struct goal *g, *g2;
	int x,y;
	struct ant *a, *a2;

	/* Generate obstacle map from water + antmap */
	for (x=0; x<ROWS*COLS; x++) {
			game->obsmap[x] = (game->watermap[x] || game->antmap[x]) ? 1 : 0;
	}

	/* Slowly create a BFS map to flow ants away from the hill */
	for (x=0; x<ROWS; x++) {
			for (y=0; y<COLS; y++) {
				struct loc a = {x,y};
				int d;
				int *aval = &game->flowaway[loc(x,y)];

				if (*aval == 0 || !game->viewmap[loc(x,y)])
						continue;

				for (d=0; d<4; d++) {
					struct loc b;
					int nval;
					loc_neighbor(&a, &b, d);
					nval = game->flowaway[loc2offset(&b)];
					if (nval == 0)
							continue;
					if (nval < *aval)
							*aval = nval+1;
				}
			}
	}

	if (game->turn < 50) {
			char *mapname = malloc(BUFSIZ);
			sprintf(mapname, "maps/turn%d.pnm", game->turn);
			write_img(game->flowaway, mapname);
	}

	list_for_each_entry(f, &game->food_l, node) {
		if (list_empty(&game->freegoals)) {
			g = malloc(sizeof(*g));
			list_add_tail(&g->node, &game->goals);
		} else {
			g = list_first_entry(&game->freegoals, struct goal, node);
			list_move(&g->node, &game->goals);
		}
		memcpy(&g->loc, &f->loc, sizeof(g->loc));
		g->type = FOOD;
	}

	list_sort(game, &game->goals, cmp_food);

	list_for_each_entry_safe(g, g2, &game->goals, node) {
		struct ant *a = NULL;
		struct loc next;

		if (list_empty(&game->ant_l))
				break;

		a = find_nearest_idle_ant(game, &g->loc);
		if (!a) {
			fprintf(stderr, "No ant for goal (%d,%d)\n", a->loc.x, a->loc.y);
			continue;
		}

		if (!memcmp(&a->loc, &g->loc, sizeof(a->loc))) {
			fprintf(stderr, "Ant holding on (%d,%d)\n", a->loc.x, a->loc.y);
			continue;
		}

		if (!astar(game->obsmap, &a->loc, &g->loc, &next)) {
			fprintf(stderr, "move a(%d,%d) to g(%d,%d)\n",
				a->loc.x, a->loc.y,
				next.x, next.y);
			order_loc(&a->loc, &next);
			game->obsmap[loc2offset(&a->loc)] = 0;
			game->obsmap[loc2offset(&next)] = 1;
			list_move(&a->node, &game->freeants);
			list_move(&g->node, &game->freegoals);
		} else {
			fprintf(stderr, "No route for a(%d,%d) to g(%d,%d)\n",
				a->loc.x, a->loc.y,
				g->loc.x, g->loc.y);
		}
	}

	/* Leftover ants after goals are assigned */
	list_for_each_entry_safe(a, a2, &game->ant_l, node) {
		int d, nval;
		int highest = -1;
		struct loc n, highest_n;
		for (d=0; d<4; d++) {
			loc_neighbor(&a->loc, &n, d);
			if (game->obsmap[loc2offset(&n)])
					continue;
			nval = game->flowaway[loc2offset(&n)];
			if (highest == -1 || nval > highest) {
					highest = nval;
					highest_n = n;
			} else if (nval == highest) {
				if (rand() > RAND_MAX/2) {
					highest = nval;
					highest_n = n;
				}
			}
		}
		order_loc(&a->loc, &highest_n);
		game->obsmap[loc2offset(&a->loc)] = 0;
		game->obsmap[loc2offset(&highest_n)] = 1;
		list_move(&a->node, &game->freeants);
	}
}
