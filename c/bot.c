#include "api.h"
#include <stdio.h>
#include <stdlib.h>

void print_map(int *map)
{
	int r,c;
	for(r=0; r<ROWS; r++) {
		for(c=0; c<COLS; c++) {
			fprintf(stderr, "%3d ", map[loc(r,c)]);
		}
		fprintf(stderr, "\n");
	}
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

void do_turn(struct Game *game)
{
	#define NO_MOVE -1
	int *map;
	int *viewmap;
	int i,r,c;
	int dir, child, lowest, lowest_dir;

	// Start from a base of watermap
	map = malloc(ROWS*COLS*sizeof(int));

	// Generate a shitty viewmap that needs to be fixed later
	viewmap = malloc(ROWS*COLS*sizeof(int));
	for (i=0; i<ROWS*COLS; i++) {
		viewmap[i] = (game->antmap[i] == 1) ? 255-7 : 255;
		map[i] = game->watermap[i];
	}
	diffuse_iter(viewmap);

	for (i=0; i<ROWS*COLS; i++) {
			// Elevate base map to 255 everywhere except for walls
			map[i] = (map[i]) ? 0 : 255;

			// Add ant positions as unpassable
			if (game->antmap[i] == 1)
				map[i] = 0;

			// Make our own anthill unpassable
			// Make enemy anthills desireable
			if (game->hillmap[i] == 1)
				map[i] = 0;
			else if (game->hillmap[i] > 1)
				map[i] = 1;

			// Make food desireable
			if (game->foodmap[i])
				map[i] = 1;

			// Assign all unviewable locations as desireable
			if ((map[i] == 255) && (viewmap[i] == 255))
				map[i] = 5;
	}

	diffuse_iter(map);

	if (game->turn == 3)
		print_map(map);

	for (r=0; r<ROWS; r++) {
		for (c=0; c<COLS; c++) {
			if (game->antmap[loc(r,c)] == 1) {
				lowest = 255;
				lowest_dir = NO_MOVE;
				for (dir=0; dir<4; dir++) {
					child = map[neighbor(r, c, dir)];
					if (child && (lowest > child)) {
						lowest = child;	
						lowest_dir = dir;
					}
				}
				if (lowest_dir != -1)
					order(r, c, lowest_dir);
					map[loc(r,c)] = 255;
					map[neighbor(r ,c, lowest_dir)] = 0;
			}
		}
	}
}
