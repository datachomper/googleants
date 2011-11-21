#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "types.h"
#include "list.h"

/* Usage: ./pathfind < mapfile.map */

int ROWS, COLS;
enum DIRECTION {N,E,S,W};
enum LISTS {FREE, CLOSED, OPEN};
static struct square *slist;

struct square {
	struct list_head node;
	struct square *parent;
	int offset;
	int g;
	int h;
	enum LISTS list;
};

int loc(int x, int y)
{
	return (x*COLS+y);
}

void write_img(char *map, const char *name)
{
	FILE *fp;
	int r,c,val;

	fp = fopen(name, "w");
	fprintf(fp, "P3\n%d %d\n255\n", COLS, ROWS);
	for (r=0; r<ROWS; r++) {
		for (c=0; c<COLS; c++) {
			val = map[loc(r,c)];
			if (val == '%')
				fprintf(fp, "%03d %03d %03d ",0,0,0);
			else if (val == '0')
				fprintf(fp, "%03d %03d %03d ",0,0,255);
			else if (val == '1')
				fprintf(fp, "%03d %03d %03d ",255,0,0);
			else if (val == '2')
				fprintf(fp, "%03d %03d %03d ",100,100,100);
			else
				fprintf(fp, "%03d %03d %03d ",255,255,255);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
}

struct square * neighbor(char *map, struct square *s, enum DIRECTION d)
{
	int row, col;

	row = s->offset / COLS;
	col = s->offset % COLS;

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
	return &slist[loc(row,col)];
}

void print_map(char *map)
{
	int r,c;
	for(r=0; r<ROWS; r++) {
		printf("%3d ", r);
		for(c=0; c<COLS; c++) {
			printf("%c", map[loc(r,c)]);
		}
		printf("\n");
	}
}

char * import_map() {
	char *map = 0;
	char buf[BUFSIZ];
	char *index;
	int offset = 0;

	while (fgets(buf, sizeof(buf), stdin) != NULL) {
		index = buf;
		if (*index == 'r') {
			index += 5;
			ROWS = atoi(index);
		} else if (*index == 'c') {
			index += 5;
			COLS = atoi(index);
		} else if (*index == 'p') {
			// Ignore
		} else if (*index == 'm') {
			if (!map) {
				map = malloc(ROWS*COLS*sizeof(char));
			}
			index += 2;
			while (*index != '\n') {
				map[offset] = *index;
				index++;
				offset++;
			}
		} else {
			// Ignore
		}
	}
	return map;
}

char min(char a, char b) {
	return (a < b) ? a : b;
}

// Calculates the manhattan distance from start to goal
int manhattan(struct square *start, struct square *goal) {
	int diff, dist;
	int x1,y1,x2,y2;

	x1 = start->offset / COLS;
	y1 = start->offset % COLS;
	x2 = goal->offset / COLS;
	y2 = goal->offset % COLS;

	diff = abs(x1-x2);
	dist = min(diff, ROWS-diff);
	diff = abs(y1-y2);
	dist += min(diff, COLS-diff);
	return dist;
}

void astar_init(int rows, int cols)
{
	int offset;

	ROWS = rows;
	COLS = cols;
	slist = malloc(ROWS*COLS*sizeof(struct square));
	for (offset=0; offset<ROWS*COLS; offset++) {
		slist[offset].offset = offset;
		slist[offset].list = FREE;
	}
}

struct square * get_best_f(struct list_head *openlist)
{
	struct square *s;
	struct square *lowest = NULL;
	int lowest_f;

	lowest = list_first_entry(openlist, struct square, node);
	lowest_f = lowest->g+lowest->h;

	list_for_each_entry(s, openlist, node) {
		if ((s->g + s->h) < lowest_f)
			lowest = s;
	}
	return lowest;
}

struct square * astar(char *map, struct square *start, struct square *target)
{
	struct square *s, *n;
	LIST_HEAD(openlist);
	int d;

	start->parent = NULL;
	start->h = manhattan(start, target);
	start->g = 0;
	start->list = OPEN;
	list_add(&start->node, &openlist);

	while ((s = get_best_f(&openlist)) != NULL) {
		if (s == target) {
			// Zip backwards through the tree and set the square
			// to some value to indicate our chosen path
			do {
				map[s->offset] = '0';
				s = s->parent;
			} while (s->parent->parent != NULL);
			return s;
		}
		s->list = CLOSED;
		list_del_init(&s->node);

		// Add all valid neighbor moves onto the open list
		for (d=0; d<4; d++) {
			n = neighbor(map, s, d);	
			if (map[n->offset] == '%' || n->list == CLOSED)
				continue;
			if (n->list == OPEN) {
				if (s->g+1 < n->g) {
					n->parent = s;
					n->g = s->g + 1;
				}
			} else if (n->list == FREE) {
				list_add(&n->node, &openlist);
				n->list = OPEN;
				n->h = manhattan(n, target);
				n->g = s->g + 1;
				n->parent = s;
			}
		}
	}
	printf("oops, open list is empty!\n");
	return NULL;
}

int main()
{
	char *map;
	struct square *start, *finish;
	struct square *next = NULL;
	struct timespec a,b;

	map = import_map();
	if (!map)
		return -1;

	astar_init(ROWS, COLS);
	start = &slist[loc(4,4)];
	finish = &slist[loc(39,50)];

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &a);
	next = astar(map, start, finish);
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &b);

	write_img(map, "astar.ppm");
	//print_map(map);
	if (next)
		printf("Moving to (%d,%d)\n", next->offset/COLS, next->offset%COLS);

	printf("run time: %.2fms\n", (float)(b.tv_nsec-a.tv_nsec)/1000000);

	return 0;
}
