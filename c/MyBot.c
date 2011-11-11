#include "api.h"

void do_turn(struct Game *game)
{
	int r,c;
	for (r=0; r<ROWS; r++) {
		for (c=0; c<COLS; c++) {
			if (game->antmap[loc(r,c)] == 1) {
					order(r, c, N);
			}
		}
	}
}
