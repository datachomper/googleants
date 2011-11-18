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

struct list_head *neighbors(char *map, struct square *parent)
{
	struct list_head *adj;
	char row, col;
	enum DIRECTION d;
	INIT_LIST_HEAD(adj);

	for (d=0; d<4; d++) {
		row = parent->x;
		col = parent->x;
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
		if (map[loc(row,col)] != '%') {
			struct square *square = malloc(sizeof(struct square));
			square->x = row;
			square->y = col;
			square->parent = parent;
			list_add(&square->node, adj);
		}
	}
	return adj;
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
	char *map;
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
				map = malloc(sizeof(ROWS*COLS*sizeof(char)));
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

void astar(char *map, struct square *start, struct square *target)
{
	LIST_HEAD(open);
	LIST_HEAD(closed);
	struct square *square, *lowest, *f;
	struct list_head *neigh;

	start->parent = NULL;
	fu(start, target);
	list_add(&start->node, &open);
	while (!list_empty(&open)) {
		list_for_each_entry(square, &open, node) {
			// Find lowest F value
			if ((!lowest) || (lowest->f > square->f))
				lowest = square;
		}

		if ((lowest->x == target->x) && (lowest->y == target->y)) {
			// Zip backwards through the tree and set the square
			// to some value to indicate our chosen path
			do {
				map[loc(lowest->x, lowest->y)] = 0;
				lowest = lowest->parent;
			} while (lowest->parent != NULL);
			return;
		}

		// Add all valid neighbor moves onto the open list
		neigh = neighbors(map, lowest);
		if (!list_empty(neigh)) {
			list_for_each_entry(f, neigh, node) {
				fu(f, target);
			}
			list_splice(neigh, &open);
		}
		// Move current square from open to closed
		list_move(&lowest->node, &closed);
	}
}

int main()
{
	char *map;
	struct square start, finish;

	map = import_map();
	if (!map)
		return -1;

	start.x = 4;
	start.y = 0;
	finish.x = 4;
	finish.y = 4;
	astar(map, &start, &finish);
	print_map(map);

	return 0;
}
