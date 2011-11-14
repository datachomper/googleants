#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROWS 200
#define COLS 200

enum DIRECTION { n,e,s,w };

struct point {
	int x;
	int y;
};

int loc(int x, int y)
{
	return (x*COLS+y);
}

void print_map(int **map)
{
	int r,c;
	for(r=0; r<ROWS; r++) {
		for(c=0; c<COLS; c++) {
			printf("%3d ", map[r][c]);
		}
		printf("\n");
	}
}

void print_map2(int *map)
{
	int r,c;
	for(r=0; r<ROWS; r++) {
		for(c=0; c<COLS; c++) {
			printf("%3d ", map[loc(r,c)]);
		}
		printf("\n");
	}
}

struct point neighbor(int row, int col, enum DIRECTION dir)
{
	int x;
	struct point p = {0,0};

	switch(dir) {
	case n:
		p.x = (row == 0) ? ROWS-1 : row-1;
		p.y = col;
		return p;
	case e:
		p.x = row;
		p.y = (col == COLS-1) ? 0 : col+1;
		return p;
	case s:
		p.x = (row == ROWS-1) ? 0 : row+1;
		p.y = col;
		return p;
	case w:
		p.x = row;
		p.y = (col == 0) ? COLS-1 : col-1;
		return p;
	}
}

int neighbor2(int row, int col, enum DIRECTION dir)
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

void diffuse_iter(int **map)
{
	int r,c,dir,iter,changed;
	struct point tmp;
	iter = 0;
	changed = 1;
	while (changed) {
		iter++;
		changed = 0;
		for (r=0; r<ROWS; r++) {
			for (c=0; c<COLS; c++) {
				int lowest = map[r][c];
				for (dir=0; dir<4; dir++) {
					int childval = 0;
					tmp = neighbor(r, c, dir);
					childval = map[tmp.x][tmp.y];
					if ((childval != 0) && (lowest > childval+1))
						lowest = childval+1;
				}
				if (lowest < map[r][c]) {
					changed = 1;
					map[r][c] = lowest;
				}
			}
		}
	}
}

void diffuse_iter2(int *map)
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
					tmp = neighbor2(r, c, dir);
					childval = map[tmp];
					if ((childval != 0) && (lowest > childval+1))
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

int main(void)
{
	int **map = malloc(ROWS*sizeof(int *));
	int *map2 = malloc(ROWS*COLS*sizeof(int));
	int r,c,d,iter;
	struct point p;
	struct point neigh;
	struct timespec start, finish;

	for(r=0; r<ROWS; r++)
		map[r] = malloc(COLS*sizeof(int));	

	/* Benchmarking */
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
	for(r=0; r<ROWS; r++)
		for(c=0; c<COLS; c++)
			map[r][c] = 255;

	map[0][0] = 1;
	diffuse_iter(map);
	if (!clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &finish))
		printf("%.2fms per diffusion\n",(float)(finish.tv_nsec-start.tv_nsec)/1000000);
//	print_map(map);


	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
	for(r=0; r<ROWS; r++)
		for(c=0; c<COLS; c++)
			map2[loc(r,c)] = 255;

	map2[loc(0,0)] = 1;
	diffuse_iter2(map2);
//	print_map2(map2);
	if (!clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &finish))
		printf("%.2fms per diffusion2\n",(float)(finish.tv_nsec-start.tv_nsec)/1000000);
#if 0

	/* Visualize */
	for(r=0; r<ROWS; r++)
		for(c=0; c<COLS; c++)
			map[r][c] = 255;
	map[10][10] = 255-7;
	diffuse_iter(map);
	print_map(map);

	/* Visualize 2 */
	for(r=0; r<ROWS; r++)
		for(c=0; c<COLS; c++)
			map2[loc(r,c)] = 255;
	map2[loc(10,10)] = 255-7;
	diffuse_iter2(map2);
	for(r=0; r<ROWS; r++)
		for(c=0; c<COLS; c++)
			if (map2[loc(r,c)] == 255)
				map2[loc(r,c)] = 1;
			else
				map2[loc(r,c)] = 255;
	diffuse_iter2(map2);
	printf("\n");
	print_map2(map2);
#endif




	for(r=0; r<ROWS; r++)
		free(map[r]);
	free(map);
	free(map2);
	return 0;
}
