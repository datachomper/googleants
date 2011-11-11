enum STATE {SETUP, INGAME, END};
enum DIRECTION { N,E,S,W };

int ROWS, COLS;

int loc(int x, int y);
char direction(short d);
int neighbor(int row, int col, enum DIRECTION dir);
void order(int row, int col, enum DIRECTION dir);

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

	int *watermap;
	int *foodmap;
	int *antmap;
	int *hillmap;
};
