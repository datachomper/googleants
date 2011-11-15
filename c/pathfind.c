#include <stdio.h>
#include <stdlib.h>

/* Usage: ./pathfind < mapfile.map */

int ROWS, COLS;
enum DIRECTION { N,E,S,W };

int loc(int x, int y)
{
	return (x*COLS+y);
}

int neighbor(int row, int col, enum DIRECTION dir)
{
	switch(dir) {
	case N:
		if (row == 0)
			return loc(ROWS-1, col);
		else
			return loc(row-1, col);
	case E:
		if (col == COLS-1)
			return loc(row, 0);
		else
			return loc(row, col+1);
	case S:
		if (row == ROWS-1)
			return loc(0, col);
		else
			return loc(row+1, col);
	case W:
		if (col == 0)
			return loc(row, COLS-1);
		else
			return loc(row, col-1);
	}
	return 0;
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

}

int main()
{
	char *map;

	map = import_map();
	if (!map)
		return -1;
	print_map(map);

	return 0;
}
