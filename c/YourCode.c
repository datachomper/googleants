#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ants.h"

enum DIRECTION { n,e,s,w };

int loc(int x, int y)
{
	return (x*COLS+y);
}

void print_map(int *map)
{
	int r,c;
	for(r=0; r<ROWS; r++) {
		for(c=0; c<COLS; c++) {
			fprintf(stderr, "%3d ", map[loc(r,c)]);
		}
		printf("\n");
	}
}

int neighbor(int row, int col, enum DIRECTION dir)
{
	int x,y;

	switch(dir) {
	case n:
		if (row == 0)
			return loc(ROWS-1, col);
		else
			return loc(row-1, col);
	case e:
		if (col == COLS-1)
			return loc(row, 0);
		else
			return loc(row, col+1);
	case s:
		if (row == ROWS-1)
			return loc(0, col);
		else
			return loc(row+1, col);
	case w:
		if (col == 0)
			return loc(row, COLS-1);
		else
			return loc(row, col-1);
	}
}

void diffuse_iter(int *map)
{
	int r,c,dir,iter,changed;
	iter = 0;
	changed = 1;
	while (changed) {
		iter++;
		changed = 0;
		for (r=0; r<ROWS; r++) {
			for (c=0; c<COLS; c++) {
				int lowest = map[loc(r,c)];
				for (dir=0; dir<4; dir++) {
					int childval = 0;
					int tmp;
					tmp = neighbor(r, c, dir);
					childval = map[tmp];
					if ((childval) && (lowest > childval+1))
						lowest = childval+1;
				}
				if (lowest < map[loc(r,c)]) {
					changed = 1;
					map[loc(r,c)] = lowest;
				}
			}
		}
	}
}

void do_turn(struct game_state *Game, struct game_info *Info) {
    int i, child, dir;
	int lowest, lowest_dir;
	print_map(Game->foodmap);
	diffuse_iter(Game->foodmap);
	print_map(Game->foodmap);
    for (i=0; i<Game->my_count; i++) {
		struct my_ant ant = Game->my_ants[i];
		lowest = 255;
		for (dir=0; dir<4; dir++) {
			child = Game->foodmap[neighbor(ant.row, ant.col, dir)];
			if (child && (lowest > child)) {
				lowest = child;	
				lowest_dir = dir;
			}
		}
	}

}
