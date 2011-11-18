#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "types.h"
#include "list.h"

/* Usage: ./pathfind < mapfile.map */

int ROWS, COLS;
enum DIRECTION { N,E,S,W };
static struct square *freelist;

struct square {
	struct list_head node;
	struct list_head adj;
	struct square *parent;
	char x;
	char y;
	int h;
	int g;
};

int loc(int x, int y)
{
	return (x*COLS+y);
}

void write_img(int *map, const char *name)
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

void neighbors(int *map, struct square *parent, struct list_head *adj)
{
	int row, col, offset;
	enum DIRECTION d;

	for (d=0; d<4; d++) {
		row = parent->x;
		col = parent->y;
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
		offset = loc(row,col);
		if (map[offset] == '.') {
			struct square *square;
			square = &freelist[offset];
			square->x = row;
			square->y = col;
			square->g = parent->g + 1;
			square->parent = parent;
			list_add(&square->adj, adj);
		} else if (map[offset] != '%') {
			struct square *square;
			square = &freelist[offset];
			list_add(&square->adj, adj);
		}
	}
	return;
}

void print_map(int *map)
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

int * import_map() {
	int *map = 0;
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
				map = malloc(ROWS*COLS*sizeof(int));
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

/* Find F value for a given square and target */
void heuristic(struct square *current, struct square *goal) {
	char diff;
	// Calculate H, manhattan distance to the goal
	diff = abs(current->x - goal->x);
	current->h = min(diff, ROWS-diff);
	diff = abs(current->y - goal->y);
	current->h += min(diff, COLS-diff);
}

void astar_init(int rows, int cols)
{
	ROWS = rows;
	COLS = cols;
	freelist = malloc(ROWS*COLS*sizeof(struct square));
}

void astar(int *map, struct square *start, struct square *target)
{
	LIST_HEAD(open);
	LIST_HEAD(neigh);
	struct square *square, *lowest, *f, *n;

	start->parent = NULL;
	heuristic(start, target);
	start->g = 0;
	list_add(&start->node, &open);
	while (!list_empty(&open)) {
		lowest = 0;
		list_for_each_entry(square, &open, node) {
			// Find lowest F value
			if ((!lowest) || (lowest->h+lowest->g > square->h+square->g))
				lowest = square;
		}

		if ((lowest->x == target->x) && (lowest->y == target->y)) {
			// Zip backwards through the tree and set the square
			// to some value to indicate our chosen path
			do {
				map[loc(lowest->x,lowest->y)] = '0';
				lowest = lowest->parent;
			} while (lowest->parent != NULL);
			return;
		}
		map[loc(lowest->x,lowest->y)] = '1';

		// Add all valid neighbor moves onto the open list
		neighbors(map, lowest, &neigh);
		if (!list_empty(&neigh)) {
			list_for_each_entry_safe(f, n, &neigh, node) {
				// Neighbor already on open list or not
				if (map[loc(f->x,f->y)] != '.') {
					/* If current path to neighbor is
					 * shorter, reset neighbor parent
					 * and G score. */
					if (lowest->g+1 < f->g) {
						f->parent = lowest;
						f->g = lowest->g+1;
					}
					list_del_init(&f->adj);
				} else {
					heuristic(f, target);
					list_move(&f->adj, &open);
					map[loc(f->x,f->y)] = '2';
				}
			}
		}
		// Move current square from open to freelist
		list_del_init(&lowest->node);
	}
	printf("oops, open list is empty!\n");
}

int main()
{
	int *map;
	struct square start, finish;
	struct timespec a,b;

	map = import_map();
	if (!map)
		return -1;

	astar_init(ROWS, COLS);
	start.x = 10;
	start.y = 20;
	finish.x = 17;
	finish.y = 54;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &a);
	astar(map, &start, &finish);
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &b);
	write_img(map, "astar.ppm");
	//print_map(map);
	printf("run time: %.2fms\n", (float)(b.tv_nsec-a.tv_nsec)/1000000);

	return 0;
}
