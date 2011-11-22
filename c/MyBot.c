#include <stdlib.h>
#include <stdio.h>
#include "api.h"
#include "list.h"

int loc_dist(struct loc *a, struct loc *b) {
	int diff, dist;

	diff = abs(a->x - b->x);
	dist = min(diff, ROWS-diff);
	diff = abs(a->y - b->y);
	dist += min(diff, COLS-diff);
	return dist;
}

void do_turn(struct Game *game)
{
	int r,c;
	struct food *f;
	struct ant *a, *b;
	for (r=0; r<ROWS; r++) {
		for (c=0; c<COLS; c++) {
			if (game->antmap[loc(r,c)] == 1) {
					order(r, c, N);
			}
		}
	}

	list_for_each_entry_safe(a, b, &game->ant_l, node) {
		int lowest = -1;
		struct food *closest = NULL;
		list_for_each_entry(f, &game->food_l, node) {
			int dist = loc_dist(&f->loc, &a->loc);
			if ((lowest == -1) || dist < lowest) {
				lowest = dist;
				closest = f;
			}
		}
		if (closest != NULL) {
			fprintf(stderr, "assign a(%d,%d) to f(%d,%d)\n",
				a->loc.x, a->loc.y,
				closest->loc.x, closest->loc.y);
			order(a->loc.x, a->loc.y, astar_dir(a->loc, f->loc));
			list_move(&a->node, &game->freefood);
			list_move(&closest->node, &game->freeants);
		}
	}
}
