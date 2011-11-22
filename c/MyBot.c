#include <stdio.h>
#include "api.h"
#include "list.h"

struct ant * find_nearest_ant(struct Game *game, int offset) {
	int lowest;
	/* No ants? just return :O( */
	if (list_empty(&game->ant_l))
			return NULL;
}

void do_turn(struct Game *game)
{
	int r,c;
	struct ant *a, *z, *f;
	for (r=0; r<ROWS; r++) {
		for (c=0; c<COLS; c++) {
			if (game->antmap[loc(r,c)] == 1) {
					order(r, c, N);
			}
		}
	}

	list_for_each_entry_safe(f, z, &game->food_l, node) {
		struct ant *nearest = NULL;
		// Radiative search from target to nearest ant
		nearest = find_nearest_ant(game, loc(f->row,f->col));
		// Pathfind
		// Order ant in that direction
	}

	list_for_each_entry_safe(a, z, &game->ant_l, node) {
		fprintf(stderr, "Got ant at %d,%d\n", a->row, a->col);
		list_move(&a->node, &game->freeants);
	}
}
