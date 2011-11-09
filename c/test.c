#include <stdio.h>
#include <stdlib.h>

#define ROWS 10
#define COLS 10
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
			printf("%01d ", map[r][c]);
		}
		printf("\n");
	}
}

struct point neighbor(struct point *t, enum DIRECTION dir)
{
	int x;
	struct point *p;

	switch(dir) {
	case n:
		p.x = (t.x-1)%ROWS
		p.y = (t.y+0)%COLS
		return p;
	case e;
	case s;
	case w;
	}
}

void diffuse_iter(int **map, struct point *t)
{
	int r,c;
	for(r=0; r<ROWS; r++) {
		for(c=0; c<COLS; c++) {
		}
	}
}

int main(void)
{
	int **map = malloc(ROWS*sizeof(int *));
	int r,c;

	for(r=0; r<ROWS; r++)
		map[r] = malloc(COLS*sizeof(int));	

	for(r=0; r<ROWS; r++)
		for(c=0; c<COLS; c++)
			map[r][c] = 255;

	print_map(map);

	for(r=0; r<ROWS; r++)
		free(map[r]);
	free(map);
	return 0;
}
