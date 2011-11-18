#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "list.h"

/* Usage: ./pathfind < mapfile.map */

int ROWS, COLS;
enum DIRECTION { N,E,S,W };

struct square {
	struct list_head node;
	struct square *parent;
	char x;
	char y;
	int f;
};

int loc(int x, int y)
{
	return (x*COLS+y);
}

void neighbors(int *map, struct square *parent, struct list_head *adj)
{
	char row, col;
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
		if (map[loc(row,col)] == '.') {
			struct square *square = malloc(sizeof(struct square));
			square->x = row;
			square->y = col;
			square->parent = parent;
			list_add(&square->node, adj);
			printf("row:%d col:%d offset:%d map:%c\n", row, col, loc(row,col), map[loc(row,col)]);
			map[loc(row,col)] = '2';
		} else {
			printf("threw away x:%d y:%d\n", row, col);
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
	int *map;
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
			if (!map)
				map = malloc(sizeof(ROWS*COLS*sizeof(int)));
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

/* Calculates the manhattan distance from start to goal
 * Stores it in start->f */
void fu(struct square *start, struct square *goal) {
	char diff;
	diff = abs(start->x - goal->x);
	start->f = min(diff, ROWS-diff);
	diff = abs(start->y - goal->y);
	start->f += min(diff, COLS-diff);
}

void astar(int *map, struct square *start, struct square *target)
{
	LIST_HEAD(open);
	LIST_HEAD(closed);
	LIST_HEAD(neigh);
	struct square *square, *lowest, *f, *t;

	printf("start x%d y%d\n", start->x, start->y);
	start->parent = NULL;
	fu(start, target);
	list_add(&start->node, &open);
	while (!list_empty(&open)) {
		lowest = 0;
		list_for_each_entry(square, &open, node) {
			// Find lowest F value
			printf("iter x%d y%d\n", square->x, square->y);
			if ((!lowest) || (lowest->f > square->f))
				lowest = square;
		}
		printf("Evaluating x%d y%d\n", lowest->x, lowest->y);

		if ((lowest->x == target->x) && (lowest->y == target->y)) {
			// Zip backwards through the tree and set the square
			// to some value to indicate our chosen path
//			do {
//				map[loc(lowest->x,lowest->y)] = '0';
//				lowest = lowest->parent;
//			} while (lowest->parent != NULL);
			return;
		}
		map[loc(lowest->x,lowest->y)] = '1';

		// Add all valid neighbor moves onto the open list
		neighbors(map, lowest, &neigh);
		if (!list_empty(&neigh)) {
			list_for_each_entry(f, &neigh, node) {
				fu(f, target);
//				list_add_tail(&f->node, &open);
				printf("adj x%d y%d f%d\n", f->x, f->y, f->f);
			}
			list_splice_init(&f->node, &open);
			list_for_each_entry(f, &neigh, node) {
				list_del_init(&f->node);
				free(f);
			}
		}
		// Move current square from open to closed
		list_move(&lowest->node, &closed);
		list_for_each_entry(t, &open, node) {
			printf("new open x%d y%d f%d\n", t->x, t->y, t->f);
		}
	}
	printf("oops, open list is empty!\n");
}

int main()
{
	int *map;
	struct square start, finish;

	map = import_map();
	if (!map)
		return -1;

	start.x = 4;
	start.y = 4;
	finish.x = 34;
	finish.y = 0;
	astar(map, &start, &finish);
	print_map(map);

	return 0;
}
