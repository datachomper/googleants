#include <stdio.h>
#include <stdlib.h>
#include "ants.h"

enum DIRECTION { n,e,s,w };

int loc(int x, int y)
{
	return (x*COLS+y);
}

char direction(short d)
{
	if (d == 0)
		return 'n';
	if (d == 1)
		return 'e';
	if (d == 2)
		return 's';
	if (d == 3)
		return 'w';
}

void write_img(int *map, const char *name)
{
	FILE *fp;
	int r,c,val,max;

	max = 1;
	for (r=0; r<ROWS; r++)
		for (c=0; c<COLS; c++)
			max = (map[loc(r,c)] > max) ? map[loc(r,c)] : max;

	fp = fopen(name, "w");
	fprintf(fp, "P3\n%d %d\n255\n", COLS, ROWS);
	for (r=0; r<ROWS; r++) {
		for (c=0; c<COLS; c++) {
			val = map[loc(r,c)];
			if (!val)
				fprintf(fp, "%03d %03d %03d ",0,0,0);
			else if (val == 1)
				fprintf(fp, "%03d %03d %03d ",255,0,0);
			else {
				val = 255*(val-1)/(max-1);
				fprintf(fp, "%03d %03d %03d ", val, 255, 255);
			}
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
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

int turn = 1;

void do_turn(struct game_state *Game, struct game_info *Info) {
	int i, child, dir;
	int lowest, lowest_dir;
	int orders[ROWS*COLS];

	fprintf(stderr, "Doing turn %d\n", Game->turn);

	diffuse_iter(Game->foodmap);
	if (turn) {
		write_img(Game->foodmap, "foodmap_turn1.pnm");
		turn = 0;
	}

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
		fprintf(stderr, "Moving ant %d %c val %d\n",i,direction(lowest_dir),lowest);
		move(i, direction(lowest_dir), Game);
		Game->foodmap[neighbor(ant.row, ant.col, lowest_dir)] = 0;
		Game->foodmap[loc(ant.row, ant.col)] = 255;
	}
}
