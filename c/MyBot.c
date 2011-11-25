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

	fp = fopen(name, "w");
	fprintf(fp, "P3\n%d %d\n255\n", COLS, ROWS);
	for (r=0; r<ROWS; r++) {
		for (c=0; c<COLS; c++) {
			val = map[loc(r,c)];
			if (val == 0)
				fprintf(fp, "%03d %03d %03d ",0,0,0);
			else
				fprintf(fp, "%03d %03d %03d ",255,255,255);
		}
		fprintf(fp, "\n");
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
	struct ant *a, *b;
	struct goal *g, *g2;

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

		if (!astar(game->watermap, &a->loc, &g->loc, &next)) {
			fprintf(stderr, "move a(%d,%d) to g(%d,%d)\n",
				a->loc.x, a->loc.y,
				next.x, next.y);
			order_loc(&a->loc, &next);
			list_move(&a->node, &game->freeants);
			list_move(&g->node, &game->freegoals);
		} else {
			fprintf(stderr, "No route for a(%d,%d) to g(%d,%d)\n",
				a->loc.x, a->loc.y,
				g->loc.x, g->loc.y);
		}
	}
	return;

	list_for_each_entry_safe(a, b, &game->ant_l, node) {
		int lowest = -1;
		struct food *closest = NULL;

		if (list_empty(&game->food_l))
			break;

		list_for_each_entry(f, &game->food_l, node) {
			int dist = loc_dist(&f->loc, &a->loc);
			if ((lowest == -1) || dist < lowest) {
				lowest = dist;
				closest = f;
			}
		}
		if (closest != NULL) {
			struct loc next;
			if (!astar(game->watermap, &a->loc, &closest->loc, &next)) {
				fprintf(stderr, "move a(%d,%d) to f(%d,%d)\n",
					a->loc.x, a->loc.y,
					next.x, next.y);
				order_loc(&a->loc, &next);
				list_move(&a->node, &game->freefood);
				list_move(&closest->node, &game->freeants);
			}
		}
	}
}
