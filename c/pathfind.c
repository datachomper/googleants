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
	LIST_HEAD(adj);
	char row, col;
	enum DIRECTION d;

	for (d=0; d<4; d++) {
		row = parent->row;
		col = parent->col;
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
			struct *square = malloc(sizeof(struct square));
			square->x = row;
			square->y = col;
			&square->parent = parent;
			list_add(&square->node, &adj);
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

void astar(char *map, int startx, int starty, int endx, int endy)
{
	LIST_HEAD(open);
	LIST_HEAD(closed);
	struct square *start = malloc(sizeof(struct square));
	struct square *square;
	int d;

	start->x = startx;
	start->y = starty;
	start->parent = NULL;
	list_add(&start->node, &open);
	while (!list_empty(&open)) {
		list_for_each_entry(square, &open, node) {
			// Find lowest F value
		}
		// Add all valid neighbor moves onto the open list
		neighbors = neighbors(map, square);
		list_splice(&neighbors, &open);
		// Move current square from open to closed
		list_move(&square->node, &closed);
	}
}

int main()
{
	char *map;

//	map = import_map();
//	if (!map)
//		return -1;
//	print_map(map);
	astar(map, 12, 13, 44, 55);

	return 0;
}
