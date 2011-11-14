#include <stdio.h>
#include <stdlib.h>

/* Usage: ./pathfind < mapfile.map */

int ROWS, COLS;

int loc(int x, int y)
{
	return (x*COLS+y);
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

int main()
{
	char *map;

	map = import_map();
	if (!map)
		return -1;
	print_map(map);

	return 0;
}
