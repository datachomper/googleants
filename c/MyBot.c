#include "ants.h"

enum STATE {GO, READY};

void move(int index, char dir, struct game_state* Game) {
    fprintf(stdout, "O %i %i %c\n", Game->my_ants[index].row, Game->my_ants[index].col, dir);
}

// just a function that returns the string on a given line for i/o
// you don't need to worry about this

char *get_line(char *text) {
    char *tmp_ptr = text;
    int len = 0;

    while (*tmp_ptr != '\n') {
        ++tmp_ptr;
        ++len;
    }

    char *return_str = malloc(len + 1);
    memset(return_str, 0, len + 1);

    int i = 0;
    for (; i < len; ++i) {
        return_str[i] = text[i];
    }

    return return_str;
}

// main, communicates with tournament engine

int main(int argc, char *argv[]) {
	enum STATE action;
    struct game_info Info;
    struct game_state Game;
    Info.map = 0;

    Game.my_ants = 0;
    Game.enemy_ants = 0;
    Game.food = 0;
    Game.dead_ants = 0;
    Game.obsmap = 0;
    Game.foodmap = 0;
    Game.antsmap = 0;

    while (1) {
        int initial_buffer = 100000;

        char *data = malloc(initial_buffer);
        memset(data, 0, initial_buffer);

        *data = '\n';

        char *ins_data = data + 1;

        int i = 0;

        while (1) {
            ++i;

            if (i >= initial_buffer) {
                initial_buffer *= 2;
                data = realloc(data, initial_buffer);
                ins_data = data + i;
                memset(ins_data, 0, initial_buffer - i);
            }

            *ins_data = getchar();

            if (*ins_data == '\n') {
                char *backup = ins_data;

                while (*(backup - 1) != '\n') {
                    --backup;
                }

                char *test_cmd = get_line(backup);

                if (strcmp(test_cmd, "go") == 0) {
                    action = GO; 
                    free(test_cmd);
                    break;
                }
                else if (strcmp(test_cmd, "ready") == 0) {
                    action = READY;
                    free(test_cmd);
                    break;
                }
                free(test_cmd);
            }
            
            ++ins_data;
        }

        if (action == GO) {
            char *skip_line = data + 1;
            while (*++skip_line != '\n');
            ++skip_line;

            _init_map(skip_line, &Info);
            _init_game(&Info, &Game);
            do_turn(&Game, &Info);
            fprintf(stdout, "go\n");
            fflush(NULL);
        }
        else if (action == READY) {
            _init_ants(data + 1, &Info);
			ROWS = Info.rows;
			COLS = Info.cols;

            Game.my_ant_index = -1;

            fprintf(stdout, "go\n");
            fflush(NULL);
        }

        free(data);
    }
}
