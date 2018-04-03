//-------------------------------------------------------------------------
// TheAiGames's block bot by Alcides Schulz (zluchs)
//-------------------------------------------------------------------------

#undef DEVELOP

#define VERSION 36

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#define CELL_EMPTY  '0'
#define CELL_SHAPE  '1'
#define CELL_BLOCK  '2'
#define CELL_SOLID  '3'

#define CELL_VALID	"0123"

#define INFINITY	1000000

typedef struct s_game_settings {
    int     timebank;
    int     time_per_move;
    char    player_names[512];
    char    my_bot_name[512];
    int     field_height;
    int     field_width;
}   GAME_SETTINGS;

typedef struct s_game_state {
    int     round;
    char    this_piece_type;
    char    next_piece_type;
    int     this_piece_position_row;
    int     this_piece_position_col;
}   GAME_STATE;

typedef struct s_player {
    int     row_points;
    int     combo;
    int     skips;
    char    *field;
}   PLAYER;

typedef struct s_piece {
    char    type;
    char    **shapes;
    int     max_shapes;
    int     current_shape;
    int     size;
    int     row;
    int     col;
}   PIECE;

typedef struct s_move {
	double	score;
	int		shape;
	int		row;
	int		col;
}	MOVE;

typedef struct s_move_list {
	int		count;
	MOVE	moves[100];
}	MOVE_LIST;

GAME_SETTINGS   game_settings;
GAME_STATE      game_state;
PLAYER          my_player;
PLAYER          opp_player;

char *SHAPE_TYPES = "IZLJOTS";

char *I_SHAPE[4] = {"0000111100000000", "0010001000100010", "0000000011110000", "0100010001000100"};
char *Z_SHAPE[4] = {"110011000", "001011010", "000110011", "010110100"};
char *L_SHAPE[4] = {"001111000", "010010011", "000111100", "110010010"};
char *J_SHAPE[4] = {"100111000", "011010010", "000111001", "010010110"};
char *O_SHAPE[1] = {"1111"};
char *T_SHAPE[4] = {"010111000", "010011010", "000111010", "010110010"};
char *S_SHAPE[4] = {"011110000", "010011001", "000011110", "100110010"};


//////double	EV_HEIGH = -0.760066;
//////double	EV_LINES =  0.260666;
//////double	EV_HOLES = -0.956630;
//////double	EV_BUMPS = -0.284483;
//////double	EV_MULT1 =  1.000000;
//////double	EV_MULT2 =  0.375000;

//double EV_HEIGH[2] = {-0.3000, -2.3750};
//double EV_LINES[2] = {0.4250, 1.2000};
//double EV_HOLES[2] = {-1.1250, -1.5000};
//double EV_BUMPS[2] = {-0.3000, -0.9000};
//double EV_MULT1[2] = {0.6750, 0.4000};
//double EV_MULT2[2] = {0.4000, 0.2000};

//double EV_HEIGH[2] = {-0.7000, -1.4000};
//double EV_LINES[2] = { 0.4000,  0.8000};
//double EV_HOLES[2] = {-0.9000, -1.0000};
//double EV_BUMPS[2] = {-0.2000, -0.5000};
//double EV_MULT1[2] = { 0.6000,  0.0000};
//double EV_MULT2[2] = { 0.4000,  0.0000};

//double EV_HEIGH[2] = {-0.2000, -0.9000};
//double EV_LINES[2] = { 0.4000,  0.8000};
//double EV_HOLES[2] = {-0.9000, -1.5000};
//double EV_BUMPS[2] = {-0.2000, -0.5000};
//double EV_MULT1[2] = { 0.0000,  0.5000};
//double EV_MULT2[2] = { 2.0000,  0.0000};

//avg rounds: 213.38 avg xlines: 19.32 avg points: 71.01
//double EV_HEIGH[2] = {-0.4500, -1.1500};
//double EV_LINES[2] = {-0.1000, 0.5500};
//double EV_HOLES[2] = {-0.9000, -1.5000};
//double EV_BUMPS[2] = {-0.2000, -0.2500};
//double EV_MULT1[2] = {0.5000, 0.5000};
//double EV_MULT2[2] = {2.2500, 0.2500};
//double EV_LDIST[2] = {0.0000, 0.0000};

// avg rounds: 213.62 avg xlines: 16.67 avg points: 62.07 avg
//double EV_HEIGH[2] = {-0.4500, -1.1500};
//double EV_LINES[2] = {-0.1000, 0.5500};
//double EV_HOLES[2] = {-0.9000, -1.5000};
//double EV_BUMPS[2] = {-0.2000, -0.2500};
//double EV_MULT1[2] = {0.5000, 0.5000};
//double EV_MULT2[2] = {2.2500, 0.2500};
//double EV_LDIST[2] = {0.0000, -0.5000};

// avg rounds: 225.43 avg xlines: 18.29 avg points: 67.63 avg
//double EV_HEIGH[2] = {-0.450000, -1.150000};
//double EV_LINES[2] = {-0.100000, 0.425000};
//double EV_HOLES[2] = {-0.775000, -1.500000};
//double EV_BUMPS[2] = {-0.200000, -0.250000};
//double EV_MULT1[2] = {0.750000, 0.500000};
//double EV_MULT2[2] = {2.250000, 0.125000};
//double EV_LDIST[2] = {0.000000, -0.500000};

//avg rounds: 215.34 avg xlines: 24.35 avg points: 84.34
double EV_HEIGH[2] = {0.050000, -1.150000};
double EV_LINES[2] = {-0.100000, -0.075000};
double EV_HOLES[2] = {-0.775000, -1.000000};
double EV_BUMPS[2] = {-0.200000, -0.250000};
double EV_MULT1[2] = {1.250000, 1.000000};
double EV_MULT2[2] = {2.250000, 0.125000};
double EV_LDIST[2] = {0.000000, 0.000000};

void settings(char *type, char *value);
void update(char *player, char *type, char *value);
void update_player(PLAYER *player, char *type, char *value);
void action(char *action, char *time);
void do_moves(int time, int is_batch);
void find_lowest_row(char *field, PIECE *piece, int *lowest_row, int *lowest_col);
int  piece_fits_at(char *field, int row, int col, PIECE *piece);
void place_piece(char *field, PIECE *piece, int row, int col);
void remove_piece(char *field, PIECE *piece, int row, int col);
void find_best_drop(char *field, int *best_row, int *best_col, int *best_shape);
void insert_solid_lines(char *field);
void remove_piece(char *field, PIECE *piece, int row, int col);

void init_piece(PIECE *piece, char type);
void rotate_right(PIECE *piece);
void rotate_left(PIECE *piece);
void print_piece(PIECE *piece);
int remove_full_lines(char *field);
double evaluate(char *field, int prev_removed, int print);
void print_player(struct s_player *player);

#ifdef DEVELOP
void tune(void);
double play(int by_turn, int games, int display);
void test(void);
#endif

int main(void) {
    char    line[16384];
    char    part[3][1024];

    memset(&game_settings, 0, sizeof(GAME_SETTINGS));
    memset(&game_state, 0, sizeof(GAME_STATE));
    memset(&my_player, 0, sizeof(PLAYER));
    memset(&opp_player, 0, sizeof(PLAYER));
	srand(1);

#ifdef DEVELOP
    fprintf(stderr, "DEVELOP mode - version: %d\n", VERSION);
	settings("timebank", "10000");
	settings("time_per_move", "500");
	settings("player_names", "player1,player2");
	settings("your_bot", "player1");
	settings("field_width", "10");
	settings("field_height", "20");
#endif

    while(fgets(line, 16384, stdin) != NULL) {
        if (!strncmp(line, "settings ", 9)) {
            sscanf(&line[9], "%s %s", part[0], part[1]);
            settings(part[0], part[1]);
            continue;
        }
        if (!strncmp(line, "update ", 7)) {
            sscanf(&line[7], "%s %s %s", part[0], part[1], part[2]);
            update(part[0], part[1], part[2]);
            continue;
        }
        if (!strncmp(line, "action ", 7)) {
            sscanf(&line[7], "%s %s", part[0], part[1]);
            action(part[0], part[1]);
			fflush(stdout);
            continue;
        }
#ifdef DEVELOP
        if (!strncmp(line, "test", 4))
            test();
        if (!strncmp(line, "play", 4))
            play(1, 1, 1);
		if (!strncmp(line, "game", 4))
            play(0, 100, 1);
        if (!strncmp(line, "tune", 4))
            tune();
        if (!strncmp(line, "eval", 4))
            evaluate(my_player.field, 0, 1);
		if (!strncmp(line, "ppx", 3))
			print_player(&my_player);
#endif
    }

    return 0;
}

void pause(char *str) {
	char	line[100];

	if (str != NULL) {
		fprintf(stdout, "%s: ", str);
		fflush(stdout);
	}
	fgets(line, 100, stdin);
}

void copy_row(char *field, int row_from, int row_to) {
	assert(field != NULL);
	assert(row_from >= 0 && row_from < game_settings.field_height);
	assert(row_to >= 0 && row_to < game_settings.field_height);
	assert(row_from != row_to);

	memcpy(&field[row_to * game_settings.field_width], &field[row_from * game_settings.field_width], game_settings.field_width);
}

void fill_row(char *field, int row, char cell_value) {
	assert(field != NULL);
	assert(row >= 0 && row < game_settings.field_height);
	assert(strchr(CELL_VALID, cell_value) != NULL);

	memset(&field[row * game_settings.field_width], cell_value, game_settings.field_width);
}

int remove_full_lines(char *field) {
	int		row;
	int		col;
	int		full;
	int		row_copy;
	int		count = 0;

	assert(field != NULL);

	for (row = 0; row < game_settings.field_height; row++) {
		full = 0;
		for (col = 0; col < game_settings.field_width; col++) {
			if (field[row * game_settings.field_width + col] == CELL_BLOCK || 
				field[row * game_settings.field_width + col] == CELL_SHAPE)
				full++;
		}
		if (full == game_settings.field_width) {
			for (row_copy = row; row_copy > 0; row_copy--) {
				copy_row(field, row_copy - 1, row_copy);
			}
			fill_row(field, 0, CELL_EMPTY);
			count++;
		}
	}
	return count;
}

void insert_solid_lines(char *field) {
	int		row;
	int		up_row;

	assert(field != NULL);
	if (game_state.round == 0 || game_state.round % 15 != 0)
		return;
	for (row = game_settings.field_height - 1; row >= 0; row--) {
		if (field[row * game_settings.field_width] == CELL_SOLID)
			continue;
		for (up_row = 1; up_row <= row; up_row++) {
			copy_row(field, up_row, up_row - 1);
		}
		fill_row(field, row, CELL_SOLID);
		break;
	}
	//if (field == my_player.field) {
	//	print_player(&my_player);
	//	pause("insert_solid_lines");
	//}
}

void action(char *type, char *time) {
    assert(type != NULL);
    assert(time != NULL);
    
    if (!strcmp(type, "moves"))
        do_moves(atoi(time), 0);
    else
        fprintf(stderr, "action: unknown type: [%s]\n", type);
}

void do_moves(int time, int is_batch) {
    PIECE   this_piece;
    PIECE   next_piece;
    int     best_row;
    int     best_col;
    int     best_shape;
    int     i;
    
    init_piece(&this_piece, game_state.this_piece_type);
    init_piece(&next_piece, game_state.next_piece_type);

	remove_piece(my_player.field, &this_piece, game_state.this_piece_position_row, game_state.this_piece_position_col);

    //find_lowest_row(my_player.field, &this_piece, &lowest_row, &lowest_col);
    find_best_drop(my_player.field, &best_row, &best_col, &best_shape);

	//fprintf(stderr, "best: row: %d col: %d shape: %d\n", best_row, best_col, best_shape);
	
	for (i = 0; i < best_shape; i++)
		if (!is_batch) fprintf(stdout, "TURNRIGHT,");

    if (best_col < game_state.this_piece_position_col) {
		for (i = best_col; i < game_state.this_piece_position_col; i++) {
            if (!is_batch) fprintf(stdout, "LEFT,");
		}
    }
    else {
		for (i = game_state.this_piece_position_col; i < best_col; i++) {
            if (!is_batch) fprintf(stdout, "RIGHT,");
		}
    }
    
    if (!is_batch) fprintf(stdout, "DROP\n");

#ifdef DEVELOP
	this_piece.current_shape = best_shape;
	place_piece(my_player.field, &this_piece, best_row, best_col);
#endif
}

void place_piece(char *field, PIECE *piece, int row, int col) {
    int     pc_row;
    int     pc_col;
	int		bk_row;
	int		bk_col;
    
    for (pc_row = 0; pc_row < piece->size; pc_row++) {
        for (pc_col = 0; pc_col < piece->size; pc_col++) {
            if (piece->shapes[piece->current_shape][pc_row * piece->size + pc_col] == CELL_EMPTY)
                continue;
			bk_row = row + pc_row;
			bk_col = col + pc_col;
			if (bk_row < 0 || bk_row >= game_settings.field_height)
				continue;
			if (bk_col < 0 || bk_col >= game_settings.field_width)
				continue;
			field[bk_row * game_settings.field_width + bk_col] = CELL_SHAPE;
        }
    }
}

void remove_piece(char *field, PIECE *piece, int row, int col) {
    int     pc_row;
    int     pc_col;
	int		bk_row;
	int		bk_col;
    
    for (pc_row = 0; pc_row < piece->size; pc_row++) {
        for (pc_col = 0; pc_col < piece->size; pc_col++) {
            if (piece->shapes[piece->current_shape][pc_row * piece->size + pc_col] == CELL_EMPTY)
                continue;
			bk_row = row + pc_row;
			bk_col = col + pc_col;
			if (bk_row < 0 || bk_row >= game_settings.field_height)
				continue;
			if (bk_col < 0 || bk_col >= game_settings.field_width)
				continue;
			field[bk_row * game_settings.field_width + bk_col] = CELL_EMPTY;
        }
    }
}

int can_drop(char *field, int row, int col, PIECE *piece) {
    int     prow;
    int     pcol;

	int		i;

	assert(field != NULL);
	assert(row >= 0 && row < game_settings.field_height);
	assert(col >= -1 && col < game_settings.field_width);
	assert(piece != NULL);
	assert(strchr(SHAPE_TYPES, piece->type) != NULL);

	for (prow = 0; prow < piece->size; prow++) {
		for (pcol = 0; pcol < piece->size; pcol++) {
            if (piece->shapes[piece->current_shape][prow * piece->size + pcol] == CELL_EMPTY)
                continue;
			for (i = row + prow - 1; i > 0; i--) {
				if (field[i * game_settings.field_width + (col + pcol)] != CELL_EMPTY)
					return 0;
			}
		}
	}
	return 1;
}

int piece_fits_at(char *field, int row, int col, PIECE *piece) {
    int     pc_row;
    int     pc_col;
	int		bk_row;
	int		bk_col;
	int		empty = 0;

	assert(field != NULL);
	assert(row >= -1 && row < game_settings.field_height);
	assert(col >= -1 && col < game_settings.field_width);
	assert(piece != NULL);
	assert(strchr(SHAPE_TYPES, piece->type) != NULL);
    
    for (pc_row = 0; pc_row < piece->size; pc_row++) {

        for (pc_col = 0; pc_col < piece->size; pc_col++) {

            if (piece->shapes[piece->current_shape][pc_row * piece->size + pc_col] == CELL_EMPTY)
                continue;

			bk_row = row + pc_row;
			bk_col = col + pc_col;

			if (bk_row < 0)
				continue;
            if (bk_row >= game_settings.field_height)
                return 0;
            if (bk_col < 0 || bk_col >= game_settings.field_width)
                return 0;

            if (field[bk_row * game_settings.field_width + bk_col] != CELL_EMPTY)
                return 0;

			if (bk_row + 1 < game_settings.field_height && field[(bk_row + 1) * game_settings.field_width + bk_col] == CELL_EMPTY)
				empty++;
        }

    }

	if (empty == 4)
		return 0;

    return 1;
}

void find_lowest_row(char *field, PIECE *piece, int *lowest_row, int *lowest_col) {
    int     row;
    int     col;
	int		best_row = -1;
    
    assert(lowest_row != NULL);
    assert(lowest_col != NULL);
    
    *lowest_row = *lowest_col = 0;
    
    for (row = 0; row < game_settings.field_height; row++) {
        for (col = 0; col < game_settings.field_width; col++) {
			if (piece_fits_at(field, row, col, piece)) {
				if (can_drop(field, row, col, piece)) {
					if (row > best_row) {
						*lowest_row = row;
						*lowest_col = col;
						best_row = row;
					}
				}
            }
        }
    }
}

void gen_moves(PIECE *piece, MOVE_LIST *move_list) {
	int		i;

	move_list->count = 0;
	
	for (i = 0; i < piece->max_shapes; i++) {
		memset(&move_list->moves[move_list->count], 0, sizeof(MOVE));
		move_list->moves[move_list->count].shape = i;
		move_list->moves[move_list->count].score = -INFINITY;
		move_list->count++;
	}
}

int holes_count(char *field, PIECE *piece, int row, int col) {
	int		holes = 0;
    int     pc_row;
    int     pc_col;
	int		down_row;
	int		down_col;

	assert(field != NULL);
	assert(piece != NULL);
	assert(row >= 0 && row < game_settings.field_height);
	assert(col >= -1 && col < game_settings.field_width);
    
    for (pc_row = 0; pc_row < piece->size; pc_row++) {
        for (pc_col = 0; pc_col < piece->size; pc_col++) {
            if (piece->shapes[piece->current_shape][pc_row * piece->size + pc_col] == CELL_EMPTY)
                continue;
			down_row = row + pc_row + 1;
			down_col = col + pc_col;
			while(down_row < row + pc_row + 2 && down_row < game_settings.field_height) {
				if (field[down_row * game_settings.field_width + down_col] == CELL_EMPTY)
					holes += down_row;
				down_row++;
			}
        }
    }

	return holes;
}

//int evaluate(char *field, PIECE *piece, int pc_row, int pc_col) {
//	int		field_row;
//	int		field_col;
//	int		score = 0;
//	int		fill;
//
//	assert(field != NULL);
//	assert(piece != NULL);
//	assert(pc_row >= 0 && pc_row < game_settings.field_height);
//	assert(pc_col >= -1 && pc_col < game_settings.field_width);
//
//	for (field_row = 1; field_row < game_settings.field_height; field_row++) {
//		if (field[field_row * game_settings.field_width] == CELL_SOLID)
//			continue;
//		fill = 0;
//		for (field_col = 0; field_col < game_settings.field_width; field_col++) {
//			if (field[field_row * game_settings.field_width + field_col] == CELL_EMPTY)
//				continue;
//			fill++;
//		}
//		score += field_row * fill;
//		if (fill == game_settings.field_width)
//			score += 150;
//	}
//
//	score -= holes_count(field, piece, pc_row, pc_col);
//
//	return score;
//}


/*
a \times (Aggregate Height) + b \times (Complete Lines) + c \times (Holes) + d \times (Bumpiness)
where a, b, c, d are constant paramters.

We want to minimize aggregate height, holes and bumpiness, so we can expect a, c, d to be negative. Similarly, we want to maximize the number of complete lines, so we can expect B to be positive.

I used a Genetic Algorithm (GA) (explained in full detail below) to produce the following optimal set of parameters:

a = -0.510066
b = 0.760666
c = -0.35663
d = -0.184483
*/

int fill_lines(char *field) {
	int		lines = 0;
	int		field_row;
	int		field_col;
	int		fill;

	for (field_row = 0; field_row < game_settings.field_height; field_row++) {
		if (field[field_row * game_settings.field_width] == CELL_SOLID)
			continue;
		fill = 0;
		for (field_col = 0; field_col < game_settings.field_width; field_col++) {
			if (field[field_row * game_settings.field_width + field_col] == CELL_EMPTY)
				break;
			fill++;
		}
		if (fill == game_settings.field_width)
			lines += 1;
	}

	return lines;
}

double evaluate(char *field, int prev_removed, int print) {
	int		field_row;
	int		field_col;
	double	score[2] = {0, 0};
	double	final_score;
	int		phase;
	int		game_phase;
	int		holes_row;
	int		this_height;
	int		col_heights[10];
	int		row_first_solid;
	int		min_dist;
	int		max_dist;

	int		heights = 0;
	int		lines = 0;
	int		holes = 0;
	int		bumps = 0;

	assert(field != NULL);
	//assert(pc_row >= 0 && pc_row < game_settings.field_height);
	//assert(pc_col >= -1 && pc_col < game_settings.field_width);

	row_first_solid = 0;
	for (field_row = 0; field_row < game_settings.field_height; field_row++) {
		if (field[field_row * game_settings.field_width] == CELL_SOLID) {
			row_first_solid = field_row;
			break;
		}
	}

	for (field_col = 0; field_col < game_settings.field_width; field_col++) {
		col_heights[field_col] = 0;
		for (field_row = 0; field_row < game_settings.field_height; field_row++) {
			if (field[field_row * game_settings.field_width + field_col] == CELL_EMPTY)
				continue;
			for (holes_row = field_row + 1; holes_row < game_settings.field_height; holes_row++) {
				if (field[holes_row * game_settings.field_width + field_col] == CELL_EMPTY)
					holes += 1;
			}
			//if (row_first_solid != 0)
			//	this_height = row_first_solid - field_row;
			//else
			this_height = game_settings.field_height - field_row;
			col_heights[field_col] = this_height;
			heights += this_height;
			break;
		}
	}
	min_dist = game_settings.field_height;
	max_dist = 0;

	for (field_col = 0; field_col < 10; field_col++) {
		if (field_col > 0)
			bumps += abs(col_heights[field_col] - col_heights[field_col - 1]);
		if (col_heights[field_col] < min_dist)
			min_dist = col_heights[field_col];
		if (col_heights[field_col] > max_dist)
			max_dist = col_heights[field_col];
	}

	lines = fill_lines(field);

	for (phase = 0; phase < 2; phase++) {
		score[phase] = (EV_HEIGH[phase] * heights) 
					 + (EV_LINES[phase] * lines) 
					 + (EV_HOLES[phase] * holes) 
					 + (EV_BUMPS[phase] * bumps);
		if (prev_removed > 1) {
			score[phase] += prev_removed * EV_MULT1[phase];
		}
		if (lines - prev_removed > 1) {
			score[phase] += (lines - prev_removed) * EV_MULT2[phase];
		}
		score[phase] += (max_dist - min_dist) * EV_LDIST[phase];
	}
    
	//score = ((opening * (24 - eval_values.phase)) + (endgame * eval_values.phase)) / 24;
	game_phase = row_first_solid ? row_first_solid : game_settings.field_height;
	final_score = ((score[0] * (game_settings.field_height - game_phase)) + (score[1] * game_phase)) / game_settings.field_height;

	if (print) {
		print_player(&my_player);
		printf("heigths: %d lines: %d holes: %d bumps: %d prev: %d max_dist: %d min_dist: %d max-min: %d\n", heights, lines, holes, bumps, prev_removed, max_dist, min_dist, max_dist - min_dist);
		printf("game_phase: %d score[0]: %f score[1]: %f final: %f\n", game_phase, score[0], score[1], final_score);
		pause("eval");
	}

	return final_score;
}

void find_best_drop(char *field, int *best_row, int *best_col, int *best_shape) {
    int			row;
    int			col;
    int			row_np;
    int			col_np;
	double		score;
	int			i, j;
	PIECE		this_piece;
	PIECE		next_piece;
	MOVE_LIST	this_moves;
	MOVE_LIST	next_moves;
	double		best_score;
	int			best_move;
	int			this_removed = 0;
	//char		line[500];
	//static char *backup = NULL;
    
	assert(field != NULL);
    assert(best_row != NULL);
    assert(best_col != NULL);
	assert(best_shape != NULL);

	//if (backup == NULL) {
	//	backup = (char *)malloc(game_settings.field_height * game_settings.field_width);
	//	if (backup == NULL)
	//		fprintf(stderr, "find_best_drop: no memory for 'backup'.");
	//}
    
	init_piece(&this_piece, game_state.this_piece_type);
    gen_moves(&this_piece, &this_moves);
	init_piece(&next_piece, game_state.next_piece_type);
	gen_moves(&next_piece, &next_moves);

	best_score = -INFINITY;

	//memcpy(backup, my_player.field, game_settings.field_height * game_settings.field_width);

	for (i = 0; i < this_moves.count; i++) {
		this_piece.current_shape = this_moves.moves[i].shape;
		for (row = 0; row < game_settings.field_height; row++) {
			for (col = -1; col < game_settings.field_width; col++) {
				if (piece_fits_at(field, row, col, &this_piece)) {
					if (can_drop(field, row, col, &this_piece)) {
						
						//memcpy(my_player.field, backup, game_settings.field_height * game_settings.field_width);
						place_piece(field, &this_piece, row, col);
						this_removed = fill_lines(field);
						//memcpy(my_player.field, backup, game_settings.field_height * game_settings.field_width);
						//place_piece(field, &this_piece, row, col);


						for (j = 0; j < next_moves.count; j++) {
							next_piece.current_shape = next_moves.moves[j].shape;
							for (row_np = 0; row_np < game_settings.field_height; row_np++) {
								for (col_np = -1; col_np < game_settings.field_width; col_np++) {
									if (piece_fits_at(field, row_np, col_np, &next_piece)) {
										if (can_drop(field, row_np, col_np, &next_piece)) {
											place_piece(field, &next_piece, row_np, col_np);
											score = evaluate(field, this_removed, 0);
											if (score > best_score)
												best_score = score;
											if (score > this_moves.moves[i].score) { 
												this_moves.moves[i].score = score;
												this_moves.moves[i].row = row;
												this_moves.moves[i].col = col;

												//print_player(&my_player);
												//printf("score: %f\n", score);
												//pause("p");

											}
											remove_piece(field, &next_piece, row_np, col_np);
										}
									}
								}
							}
						}



						remove_piece(field, &this_piece, row, col);
					}
				}
			}
		}
	}
	best_score = -INFINITY;
	best_move = 0;
	for (i = 0; i < this_moves.count; i++) {
		if (this_moves.moves[i].score > best_score) {
			best_move = i;
			best_score = this_moves.moves[i].score;
		}
	}

	//printf("best_score: %f  best_shape: %d  best_row: %d  best_col: %d\n", 
	//	best_score, 
	//	this_moves.moves[best_move].shape,
	//	this_moves.moves[best_move].row,
	//	this_moves.moves[best_move].col);

    *best_row = this_moves.moves[best_move].row;
	*best_col = this_moves.moves[best_move].col;
	*best_shape = this_moves.moves[best_move].shape;
}

void settings(char *type, char *value) {

    assert(type != NULL);
    assert(value != NULL);
    
    if (!strcmp(type, "timebank"))
        game_settings.timebank = atoi(value);
    else
    if (!strcmp(type, "time_per_move"))
        game_settings.time_per_move = atoi(value);
    else
    if (!strcmp(type, "player_names"))
        strcpy(game_settings.player_names, value);
    else
    if (!strcmp(type, "your_bot"))
        strcpy(game_settings.my_bot_name, value);
    else
    if (!strcmp(type, "field_height"))
        game_settings.field_height = atoi(value);
    else
    if (!strcmp(type, "field_width"))
        game_settings.field_width = atoi(value);
    else
        fprintf(stderr, "settings: unknown type: [%s]\n", type);
}

void update(char *player, char *type, char *value) {

    assert(player != NULL);
    assert(type != NULL);
    assert(value != NULL);

    if (!strcmp(player, "game")) {
        if (!strcmp(type, "round"))
            game_state.round = atoi(value);
        else
        if (!strcmp(type, "this_piece_type"))
            game_state.this_piece_type = value[0];
        else
        if (!strcmp(type, "next_piece_type"))
            game_state.next_piece_type = value[0];
        else
        if (!strcmp(type, "this_piece_position"))
            sscanf(value, "%d,%d", &game_state.this_piece_position_col, &game_state.this_piece_position_row);
        else
            fprintf(stderr, "update: unknown game type: [%s]\n", type);
        return;
    }

    if (!strcmp(player, game_settings.my_bot_name))
        update_player(&my_player, type, value);
    else
        update_player(&opp_player, type, value);
}

void update_player(PLAYER *player, char *type, char *value) {
    int     row;
    int     col;
    int     i;

    assert(player != NULL);
    assert(type != NULL);
    assert(value != NULL);
    
    if (!strcmp(type, "row_points"))
        player->row_points = atoi(value);
    else
    if (!strcmp(type, "combo"))
        player->combo = atoi(value);
    else
    if (!strcmp(type, "skips"))
        player->skips = atoi(value);
    else
    if (!strcmp(type, "field")) {
        if (player->field == NULL) {
            player->field = (char *)malloc(game_settings.field_height * game_settings.field_width);
            if (player->field == NULL) {
                fprintf(stderr, "update_player: malloc failed for player->field.\n");
                return;
            }
        }
        for (row = col = i = 0; value[i]; i++) {
            switch(value[i]) {
            case ',': col++; break;
            case ';': row++; col = 0; break;
            default: player->field[row * game_settings.field_width + col] = value[i]; break;
            }
       }
    }
    else
        fprintf(stderr, "update_player: unknown type: [%s]\n", type);
}

char field_cell_string(char cell) {
    switch(cell) {
    case CELL_EMPTY: return ' ';
    case CELL_SHAPE: return 176;
    case CELL_BLOCK: return 219;
    case CELL_SOLID: return 'X';
    default: return '?';
    }
}

void print_player(PLAYER *player) {
    int     row;
    int     col;

    assert(player != NULL);

    fprintf(stderr, "row_points: %d combo: %d skips: %d\n", player->row_points, player->combo, player->skips);

    for (row = 0; row < game_settings.field_height; row++) {
        fprintf(stderr, "   ");
		for (col = 0; col < game_settings.field_width; col++) {
			if (row == 0) {
				if (col == 0)
					fprintf(stderr, "%c", 218);
				else
					fprintf(stderr, "%c", 194);
			}
			else {
				if (col == 0)
					fprintf(stderr, "%c", 195);
				else
					fprintf(stderr, "%c", 197);
			}
			fprintf(stderr, "%c%c%c", 196, 196, 196);
		}
		if (row == 0)
			fprintf(stderr, "%c\n", 191);
		else
			fprintf(stderr, "%c\n", 180); 

        for (col = 0; col < game_settings.field_width; col++) {
            if (col == 0)
                fprintf(stderr, "%2d %c", row, 179);
            fprintf(stderr, " %c %c", field_cell_string(player->field[row * game_settings.field_width + col]), 179);
        }

        fprintf(stderr, " %2d\n", row);
    }

    fprintf(stderr, "   ");
	for (col = 0; col < game_settings.field_width; col++) {
		if (col == 0)
			fprintf(stderr, "%c", 192);
		else
			fprintf(stderr, "%c", 193);
        fprintf(stderr, "%c%c%c", 196, 196, 196);
	}
    fprintf(stderr, "%c\n", 217);
}

void init_piece(PIECE *piece, char type)
{
    assert(piece != NULL);

    memset(piece, 0, sizeof(PIECE));

    switch(type) {
    case 'I':
        piece->type = 'I';
        piece->size = 4;
        piece->current_shape = 0;
        piece->shapes = I_SHAPE;
        piece->max_shapes = 4;
        break;
    case 'Z':
        piece->type = 'Z';
        piece->size = 3;
        piece->current_shape = 0;
        piece->shapes = Z_SHAPE;
        piece->max_shapes = 4;
        break;
    case 'L':
        piece->type = 'L';
        piece->size = 3;
        piece->current_shape = 0;
        piece->shapes = L_SHAPE;
        piece->max_shapes = 4;
        break;
    case 'J':
        piece->type = 'J';
        piece->size = 3;
        piece->current_shape = 0;
        piece->shapes = J_SHAPE;
        piece->max_shapes = 4;
        break;
    case 'O':
        piece->type = 'O';
        piece->size = 2;
        piece->current_shape = 0;
        piece->shapes = O_SHAPE;
        piece->max_shapes = 1;
        break;
    case 'T':
        piece->type = 'T';
        piece->size = 3;
        piece->current_shape = 0;
        piece->shapes = T_SHAPE;
        piece->max_shapes = 4;
        break;        
    case 'S':
        piece->type = 'S';
        piece->size = 3;
        piece->current_shape = 0;
        piece->shapes = S_SHAPE;
        piece->max_shapes = 4;
        break; 
    default:
        fprintf(stderr, "init_piece: unknown type: [%c]\n", type);
    }
}

void rotate_right(PIECE *piece) {
    assert(piece != NULL);
	if (piece->current_shape == piece->max_shapes - 1)
		piece->current_shape = 0;
	else
		piece->current_shape++;
}

void rotate_left(PIECE *piece) {
    assert(piece != NULL);
	if (piece->current_shape == 0)
		piece->current_shape = piece->max_shapes - 1;
	else
		piece->current_shape--;
}

void print_piece(PIECE *piece) {
    int     row;
    int     col;

    assert(piece != NULL);

    for (row = 0; row < piece->size; row++) {
        for (col = 0; col < piece->size; col++)
            fprintf(stderr, "+-");
        fprintf(stderr, "+\n");
        fprintf(stderr, "|");

        for (col = 0; col < piece->size; col++)
            fprintf(stderr, "%c|", (piece->shapes[piece->current_shape][row * piece->size + col] == '1' ? CELL_BLOCK : CELL_EMPTY));
        fprintf(stderr, "\n");
    }

    for (col = 0; col < piece->size; col++)
        fprintf(stderr, "+-");
    fprintf(stderr, "+\n\n");
}

#ifdef DEVELOP

#define PARAM_COUNT 14

int is_end_game(void) {
	int		i;
	for (i = 3; i < 4; i++)
		if (my_player.field[i] != CELL_EMPTY)
			return 1;
	return 0;
}

double play(int by_turn, int games, int display) {
    int		npi;
	int		i;
	char	resp[1000];
	int		total_games = 0;
	int		total_rounds = 0;
	int		my_removed;
	int		multiple_lines = 0;
	int		total_points = 0;
	int		lines = 0;
	double	results;

	for (total_games = 0; total_games < games; total_games++) {

		srand(total_games);

		if (display) printf("game: %3d...", total_games + 1);
		fflush(stdout);

		settings("timebank", "10000");
		settings("time_per_move", "500");
		settings("player_names", "player1,player2");
		settings("your_bot", "player2");
		settings("field_width", "10");
		settings("field_height", "20");

		npi = rand() % 7;
		assert(npi >= 0 && npi < 7);
		game_state.this_piece_type = SHAPE_TYPES[npi];
		npi = rand() % 7;
		assert(npi >= 0 && npi < 7);
		game_state.next_piece_type = SHAPE_TYPES[npi];

		update("game", "round", "1");

		update("player2", "row_points", "0");
		update("player2", "combo", "0");
		update("player2", "skips", "0");
		update("player2", "field", "0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0");

		update("player1", "row_points", "0");
		update("player1", "combo", "0");
		update("player1", "skips", "0");
		update("player1", "field", "0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0");

		while(game_state.round < 1000) {
			if (display && games == 1)
				printf("game: %d round: %d this_piece: %c next_piece: %c\n", total_games + 1, game_state.round, game_state.this_piece_type, game_state.next_piece_type);

			if (game_state.this_piece_type == 'O')
				game_state.this_piece_position_col = 4;
			else
				game_state.this_piece_position_col = 3;
			game_state.this_piece_position_row = -1;

			if (is_end_game()) {
				if (display && games == 1)
					printf("GAMEOVER\n");
				break;
			}

			do_moves(0, (games > 1));
			if (games == 1)
				print_player(&my_player);

			if (by_turn) {
				gets(resp);
				if (resp[0] == 'q')
					break;
			}

			for (i = 0; i < game_settings.field_height * game_settings.field_width; i++)
				if (my_player.field[i] == CELL_SHAPE)
					my_player.field[i] = CELL_BLOCK;

			//if (game_state.round % 15 == 0) {
			//	printf("remove/insert\n");
			//	print_player(&my_player);
			//}
			game_state.round++;
			my_removed = remove_full_lines(my_player.field);

			lines += my_removed;

			if (my_removed > 1)
				multiple_lines++;

			if (my_removed == 2)
				total_points += 3;
			if (my_removed == 3)
				total_points += 6;
			if (my_removed == 4)
				total_points += 10;

			remove_full_lines(opp_player.field);


			//if (game_state.round % 15 == 0) {
			//	printf("before insert\n");
			//	print_player(&my_player);
			//}
			insert_solid_lines(my_player.field);
			insert_solid_lines(opp_player.field);
			//if (game_state.round % 15 == 0) {
			//	printf("after insert\n");
			//	print_player(&my_player);
			//}

			//printf("after remove_lines\n");

			//print_player(&my_player);

			npi = rand() % 7;
			assert(npi >= 0 && npi < 7);

			game_state.this_piece_type = game_state.next_piece_type;
			game_state.next_piece_type = SHAPE_TYPES[npi];
		}

		total_rounds += game_state.round;
		if (display) 
			printf(" rounds: %d  avg rounds: %.2f avg xlines: %.2f avg points: %.2f lines: %d\n", 
				game_state.round, 
				(double)total_rounds / (double)(total_games + 1),
				(double)multiple_lines / (double)(total_games + 1),
				(double)total_points / (double)(total_games + 1),
				lines);

		//print_player(&my_player);
		//pause("play");

	}

	//results = (double)total_rounds / (double)total_games
	//		+ (double)multiple_lines / (double)total_games
	//		+ (double)total_points / (double)total_games
	//		+ (double)lines / (double)total_games;

	results = (double)total_points / (double)total_games;

	if (display) 
		printf("games played: %d total rounds: %d avg rounds: %.2f avg xlines: %.2f avg points: %.2f avg lines: %.2f results: %f\n", 
			total_games, 
			total_rounds, 
			(double)total_rounds / (double)total_games,
			(double)multiple_lines / (double)total_games,
			(double)total_points / (double)total_games,
			(double)lines / (double)total_games,
			results);


	return results;
}

double get_points(double params[PARAM_COUNT]) {

    EV_HEIGH[0] = params[0];
	EV_HEIGH[1] = params[1];
	EV_LINES[0] = params[2];
	EV_LINES[1] = params[3];
	EV_HOLES[0] = params[4];
	EV_HOLES[1] = params[5];
	EV_BUMPS[0] = params[6];
	EV_BUMPS[1] = params[7];
	EV_MULT1[0] = params[8];
	EV_MULT1[1] = params[9];
	EV_MULT2[0] = params[10];
	EV_MULT2[1] = params[11];
	EV_LDIST[0] = params[12];
	EV_LDIST[1] = params[13];

	return play(0, 100, 0);
}

void optimize(double initial_guess[PARAM_COUNT]) {
	double	best_points;
	double	new_points;
	double	best_values[PARAM_COUNT];
	double	new_values[PARAM_COUNT];
	int		improved = 1;
	int		pi;
	int		x;

	best_points = get_points(initial_guess);

	for (x = 0; x < PARAM_COUNT; x++) best_values[x] = initial_guess[x];

	printf(" i: values: ");
	for (x = 0; x < PARAM_COUNT; x++) {
		printf("%2.3f ", best_values[x]);
	}
	printf("points: %f\n\n", best_points);

	while(improved) {
		improved = 0;

		for (pi = 0; pi < PARAM_COUNT; pi++) {
			for (x = 0; x < PARAM_COUNT; x++) new_values[x] = best_values[x];
			new_values[pi] += 0.500;
			new_points = get_points(new_values);
			if (new_points > best_points) {
				best_points = new_points;
				for (x = 0; x < PARAM_COUNT; x++) best_values[x] = new_values[x];
				improved = 1;
			}
			else {
				new_values[pi] -= 1.000;
				new_points = get_points(new_values);
				if (new_points > best_points) {
					best_points = new_points;
					for (x = 0; x < PARAM_COUNT; x++) best_values[x] = new_values[x];
					improved = 1;
				}
			}

			printf("%2d: values: ", pi);
			for (x = 0; x < PARAM_COUNT; x++) {
				printf("%2.3f ", best_values[x]);
			}
			printf("points: %f\n", best_points);
		}

	}

	
	printf("double EV_HEIGH[2] = {%3.6f, %3.6f};\n", best_values[0], best_values[1]);
	printf("double EV_LINES[2] = {%3.6f, %3.6f};\n", best_values[2], best_values[3]);
	printf("double EV_HOLES[2] = {%3.6f, %3.6f};\n", best_values[4], best_values[5]);
	printf("double EV_BUMPS[2] = {%3.6f, %3.6f};\n", best_values[6], best_values[7]);
	printf("double EV_MULT1[2] = {%3.6f, %3.6f};\n", best_values[8], best_values[9]);
	printf("double EV_MULT2[2] = {%3.6f, %3.6f};\n", best_values[10], best_values[11]);
	printf("double EV_LDIST[2] = {%3.6f, %3.6f};\n", best_values[12], best_values[13]);
}

void tune(void) {
	double	params[PARAM_COUNT] = {
		EV_HEIGH[0], EV_HEIGH[1],
		EV_LINES[0], EV_LINES[1],
		EV_HOLES[0], EV_HOLES[1],
		EV_BUMPS[0], EV_BUMPS[1], 
		EV_MULT1[0], EV_MULT1[1],
		EV_MULT2[0], EV_MULT2[1],
		EV_LDIST[0], EV_LDIST[1],
	};

	printf("tuning...\n");
	optimize(params);
	printf("done.\n");

	//printf("tune points: %d\n", play(0, 100, 0));

}

void test(void) {
    char	p[100];
	int		i;

	settings("timebank", "10000");
	settings("time_per_move", "500");
	settings("player_names", "player1,player2");
	settings("your_bot", "player2");
	settings("field_width", "10");
	settings("field_height", "20");

	update("game", "round", "1");
	update("game", "this_piece_type", "I");
	update("game", "next_piece_type", "J");
	update("game", "this_piece_position", "3,-1");

	update("player2", "row_points", "0");
	update("player2", "combo", "0");
	update("player2", "skips", "0");
	update("player2", "field", "0,0,0,1,1,1,1,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0");

	update("player1", "row_points", "0");
	update("player1", "combo", "0");
	update("player1", "skips", "0");
	update("player1", "field", "0,0,0,1,1,1,1,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0");

	while(1) {
		printf("\npiece: ");
		fflush(stdout);
		gets(p);
		if (p == NULL)
			break;
		game_state.this_piece_type = toupper(p[0]);
		do_moves(0, 0);
		for (i = 0; i < game_settings.field_height * game_settings.field_width; i++)
			if (my_player.field[i] != CELL_EMPTY)
				my_player.field[i] = CELL_BLOCK;
	}
}

#endif

//vector<int> localOptimize(const vector<int>& initialGuess) {
//   const int nParams = initialGuess.size();
//   double bestE = E(initialGuess);
//   vector<int> bestParValues = initialGuess;
//   bool improved = true;
//   while ( improved ) {
//      improved = false;
//      for (int pi = 0; pi < nParams; pi++) {
//         vector<int> newParValues = bestParValues;
//         newParValues[pi] += 1;
//         double newE = E(newParValues);
//         if (newE < bestE) {
//            bestE = newE;
//            bestParValues = newParValues;
//            improved = true;
//         } else {
//            newParValues[pi] -= 2;
//            newE = E(newParValues);
//            if (newE < bestE) {
//               bestE = newE;
//               bestParValues = newParValues;
//               improved = true;
//            }
//         }
//      }
//   }
//   return bestParValues;
//}

// END
