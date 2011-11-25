#include <time.h>
#include "types.h"
#include "list.h"

enum STATE {SETUP, INGAME, END};
enum DIRECTION { N,E,S,W };
enum LISTS {FREE, CLOSED, OPEN};
enum GOALTYPE {HOLD, FOOD, EXPLORE};

int ROWS, COLS;

struct loc {
	int x;
	int y;
};

struct ant {
	struct list_head node;
	struct loc loc;
};

struct food {
	struct list_head node;
	struct loc loc;
};

struct hill {
	struct list_head node;
	struct loc loc;
	int owner;
};

struct square {
	struct list_head astar;
	struct square *parent;
	struct loc loc;
	int g;
	int h;
	enum LISTS list;
};

struct goal {
	struct list_head node;
	struct loc loc;
	enum GOALTYPE type;
};

struct Game {
	enum STATE state;
	int turn;
	int loadtime;
	int turntime;
	int rows;
	int cols;
	int turns;
	int viewradius2;
	int attackradius2;
	int spawnradius2;
	int player_seed;

	/* bitmap indexes */
	int *watermap;
	int *foodmap;
	int *antmap;
	int *hillmap;
	int *viewmap;
	int *obsmap;

	/* spatial pointer indexes */
	struct ant **ant_i;
	struct square **food_i;

	/* lists */
	struct list_head ant_l;
	struct list_head food_l;
	struct list_head goals;

	/* free lists */
	struct list_head freeants;
	struct list_head freefood;
	struct list_head freegoals;

	long timestamp;
};

int loc(int x, int y);
char direction(short d);
int neighbor(int row, int col, enum DIRECTION dir);
void order(int row, int col, enum DIRECTION dir);
int time_remaining(struct Game *game);
char min(char a, char b);
int astar(int *map, struct loc *a, struct loc *b, struct loc *next);
int loc2offset(struct loc *a);
