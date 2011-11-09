#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROWS 200
#define COLS 200
#define UNSET -1

enum DIRECTION { n,e,s,w };

struct point {
	int x;
	int y;
};

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

struct point neighbor(int row, int col, enum DIRECTION dir)
{
	int x;
	struct point p = {0,0};

	switch(dir) {
	case n:
		p.x = (ROWS+row-1)%ROWS;
		p.y = (COLS+col+0)%COLS;
		return p;
	case e:
		p.x = (ROWS+row+0)%ROWS;
		p.y = (COLS+col+1)%COLS;
		return p;
	case s:
		p.x = (ROWS+row+1)%ROWS;
		p.y = (COLS+col+0)%COLS;
		return p;
	case w:
		p.x = (ROWS+row+0)%ROWS;
		p.y = (COLS+col-1)%COLS;
		return p;
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

int main(void)
{
	int **map = malloc(ROWS*sizeof(int *));
	int r,c,d,iter;
	struct point p;
	struct point neigh;
	clock_t start, end;
	double elapsed;

	for(r=0; r<ROWS; r++)
		map[r] = malloc(COLS*sizeof(int));	

	start = clock();
	for(iter=0; iter < 100; iter++) {
		for(r=0; r<ROWS; r++)
			for(c=0; c<COLS; c++)
				map[r][c] = 255;

//		print_map(map);

		map[0][0] = 1;
		map[1][1] = 0;
		map[1][2] = 0;
		map[1][3] = 0;
		map[2][1] = 0;
		map[3][1] = 0;
		diffuse_iter(map);
//		print_map(map);
	}
	end = clock();
	elapsed = ((double)(end-start))/CLOCKS_PER_SEC;
	printf("%fs per diffusion\n", elapsed/100);





	for(r=0; r<ROWS; r++)
		free(map[r]);
	free(map);
	return 0;
}
