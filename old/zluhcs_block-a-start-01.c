//-------------------------------------------------------------------------
// TheAiGames's block bot by Alcides Schulz (zluchs)
// - test allow spin at find-best-move
//		- tspin eval
//		- tspin search
// - opening 
//      - 4 lines search
// - end game - a*
// - skip
// - eval 3rd piece drop
//-------------------------------------------------------------------------

#define VERSION 6607

#define DEVELOP

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <math.h>

#define CELL_EMPTY  '0'
#define CELL_SHAPE  '1'
#define CELL_BLOCK  '2'
#define CELL_SOLID  '3'

#define CELL_VALID	"0123"

#define MY_MAXVAL	1000000

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

#define PARAM_COUNT 24

static char *PARAM_LABEL[PARAM_COUNT] = {
	"heigh", "heigh",
	"lines", "lines",
	"holes", "holes",
	"bumps", "bumps",
	"mult1", "mult1",
	"mult2", "mult2",
	"empty", "empty",
	"shole", "shole",
	"maxht", "maxht",
	"lorow", "lorow",
	"block", "block",
	"tslin", "tspct" 
};

typedef struct s_list_param {
	double	heigh[2];
	double	lines[2];
	double	holes[2];
	double	bumps[2];
	double	mult1[2];
	double	mult2[2];
	double	empty[2];
	double  shole[2];
	double  maxht[2];
	double  lorow[2];
	double  block[2];
	double  tslin;
	double  tspct;
}	LIST_PARAM;

typedef union u_eval_param {
	LIST_PARAM  param;
	double      item[PARAM_COUNT];
}   EVAL_PARAM;

typedef struct s_piece {
	char    type;
	char    **shapes;
	int     max_shapes;
	int     current_shape;
	int     size;
	int     row;
	int     col;
}   PIECE;

#define MOVE_NONE       0
#define MOVE_DROP       1
#define MOVE_T_SPIN     2
#define MOVE_SEQUENCE   3

#define PATH_MAX	100
#define PATH_NONE	0
#define PATH_TURNR	1
#define PATH_TURNL	2
#define PATH_DOWN	3
#define PATH_LEFT	4
#define PATH_RIGHT	5
#define PATH_DROP	6
#define PATH_MOVES	7

#define ROTATE_NONE	0
#define ROTATE_1R	1
#define ROTATE_2R	2
#define ROTATE_3R	3
#define ROTATE_1L	4
#define ROTATE_2L	5
#define ROTATE_3L	6
#define ROTATE_SIZE	7

char	ROTATIONS[ROTATE_SIZE] = { ROTATE_NONE, ROTATE_1R, ROTATE_2R, ROTATE_3R, ROTATE_1L, ROTATE_2L, ROTATE_3L };

typedef struct s_move {
	double	score;
	int     access_type;
	int		shape;
	int		row;
	int		col;
	char	path[PATH_MAX];
}	MOVE;

typedef struct s_move_list {
	int		count;
	MOVE	moves[100];
}	MOVE_LIST;

typedef struct s_t_spin {
	int     row;
	int     col;
	int     type;
	int     lines;
	double  percent_complete;
}   T_SPIN;

typedef struct s_player {
	int			row_points;
	int			combo;
	int			skips;
	char		*field;
	EVAL_PARAM	eval;
	MOVE        move;
	int         t_spin_allow;
	MOVE_LIST	top;
}   PLAYER;

char *SHAPE_TYPES = "IZLJOTS";

char *I_SHAPE[4] = { "0000111100000000", "0010001000100010", "0000000011110000", "0100010001000100" };
char *Z_SHAPE[4] = { "110011000", "001011010", "000110011", "010110100" };
char *L_SHAPE[4] = { "001111000", "010010011", "000111100", "110010010" };
char *J_SHAPE[4] = { "100111000", "011010010", "000111001", "010010110" };
char *O_SHAPE[1] = { "1111" };
char *T_SHAPE[4] = { "010111000", "010011010", "000111010", "010110010" };
char *S_SHAPE[4] = { "011110000", "010011001", "000011110", "100110010" };

char    *T_SLOT_LEFT = "001000101";
char    *T_SLOT_RIGHT = "100000101";
int     T_SLOT_ROW = 3;
int     T_SLOT_COL = 3;
int     T_SPIN_NONE = 10;
int     T_SPIN_LEFT = 11;
int     T_SPIN_RIGHT = 12;

GAME_SETTINGS   game_settings;
GAME_STATE      game_state;
PLAYER          my_player;
PLAYER          opp_player;

void settings(char *type, char *value);
void update(char *player, char *type, char *value);
void update_player(PLAYER *player, char *type, char *value);
void action(char *action, char *time);
void do_moves(PLAYER *player, int time, int is_batch);
int  piece_fits_at(char *field, int row, int col, PIECE *piece);
void place_piece(char *field, PIECE *piece, int row, int col, char cell_shape);
void remove_piece(char *field, PIECE *piece, int row, int col);
void find_best_move(PLAYER *player);
void insert_solid_lines(char *field);
void insert_garbage_lines(char *field, int opp_removed);
void remove_piece(char *field, PIECE *piece, int row, int col);
int get_t_spin_data(char *field, T_SPIN *t_spin);
int can_drop(char *field, int row, int col, PIECE *piece);
int can_place(char *field, int target_row, int target_col, PIECE *piece);

void init_piece(PIECE *piece, char type);
void rotate_right(PIECE *piece);
void rotate_left(PIECE *piece);
void print_piece(PIECE *piece);
int remove_full_lines(char *field);
double evaluate(PLAYER *player, int prev_removed, int prev_row);
void print_player(PLAYER *player);
void print_best_moves(PLAYER *player);
void init_player(PLAYER *player);

#ifdef DEVELOP
void tune(PLAYER *player);
double simulate_game(PLAYER *player, int by_turn, int games, int display);
double play_game(int games, int display);
void test(void);
void view(void);
void pause(char *str);
#endif

int main(void) {
	char    line[16384];
	char    part[3][1024];

	memset(&game_settings, 0, sizeof(GAME_SETTINGS));
	memset(&game_state, 0, sizeof(GAME_STATE));
	init_player(&my_player);
	init_player(&opp_player);

	fprintf(stderr, "v%d\n", VERSION);

#ifdef DEVELOP
	settings("timebank", "10000");
	settings("time_per_move", "500");
	settings("player_names", "player1,player2");
	settings("your_bot", "player1");
	settings("field_width", "10");
	settings("field_height", "20");
#endif

	while (fgets(line, 16384, stdin) != NULL) {
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
			simulate_game(&my_player, 1, 1, 1);
		if (!strncmp(line, "game", 4))
			simulate_game(&my_player, 0, 1, 0);
		if (!strncmp(line, "tune", 4))
			tune(&my_player);
		if (!strncmp(line, "eval", 4))
			evaluate(&my_player, 0, 0);
		if (!strncmp(line, "ppx", 3))
			print_player(&my_player);
		if (!strncmp(line, "pbm", 3))
			print_best_moves(&my_player);
		if (!strncmp(line, "view", 4))
			view();
#endif
	}

	return 0;
}

void init_player(PLAYER *player) {
	memset(player, 0, sizeof(PLAYER));

    player->eval.param.heigh[0] = -1.950000;	player->eval.param.heigh[1] = -0.850000;
	player->eval.param.lines[0] = -2.000000;	player->eval.param.lines[1] =  2.000000; 
	//player->eval.param.lines[0] = -2.000000;	player->eval.param.lines[1] =  2.500000; small improve
	//player->eval.param.lines[0] = -2.000000;	player->eval.param.lines[1] =  2.750000; no improve
	player->eval.param.holes[0] = -1.500000;	player->eval.param.holes[1] = -0.950000;
	player->eval.param.bumps[0] = -0.600000;	player->eval.param.bumps[1] = -0.050000;
	//player->eval.param.bumps[0] = -0.700000;	player->eval.param.bumps[1] = -0.150000; bad
	player->eval.param.mult1[0] =  1.100000; 	player->eval.param.mult1[1] =  1.750000;
	player->eval.param.mult2[0] =  0.850000; 	player->eval.param.mult2[1] =  0.350000;
    //player->eval.param.empty[0] = -0.550000;	player->eval.param.empty[1] = -1.000000; bad
	//player->eval.param.empty[0] = -0.050000;	player->eval.param.empty[1] = -1.500000; not good
	//player->eval.param.empty[0] = -0.050000;	player->eval.param.empty[1] = -0.500000;
	player->eval.param.empty[0] = -0.050000;	player->eval.param.empty[1] = -0.550000; //TEST
    player->eval.param.shole[0] =  1.000000; 	player->eval.param.shole[1] =  1.000000;
	//player->eval.param.maxht[0] = 0.000000; 	player->eval.param.maxht[1] = -0.100000;
	player->eval.param.maxht[0] =  0.000000; 	player->eval.param.maxht[1] = -0.150000; //TEST
	player->eval.param.lorow[0] =  0.100000; 	player->eval.param.lorow[1] =  0.100000;
	//player->eval.param.block[0] = -0.100000;	player->eval.param.block[1] = -0.100000;
	player->eval.param.block[0] = -0.100000;	player->eval.param.block[1] = -0.150000; //TEST
	//player->eval.param.block[0] = -0.200000;	player->eval.param.block[1] = -0.200000; // bad
	player->eval.param.tslin    =  1.000000;   	player->eval.param.tspct    = 10.000000;
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

void insert_garbage_lines(char *field, int opp_removed) {
	int		row;
	int		up_row;
	int     lines;
	int     holes;
	int     i;
	int     c;

	assert(field != NULL);

	if (opp_removed < 2)
		return;

	if (opp_removed == 2)
		lines = 1;
	if (opp_removed == 3)
		lines = 2;
	if (opp_removed == 4)
		lines = 3;

	for (row = game_settings.field_height - 1; row >= 0; row--) {
		if (field[row * game_settings.field_width] == CELL_SOLID)
			continue;
		for (i = 0; i < lines; i++) {
			for (up_row = 1; up_row <= row; up_row++) {
				copy_row(field, up_row, up_row - 1);
			}
			fill_row(field, row, CELL_BLOCK);
			holes = (rand() % 2) + 1;
			while (holes--) {
				c = rand() % game_settings.field_width;
				field[row * game_settings.field_width + c] = CELL_EMPTY;
			}
		}
		break;
	}
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
}

void action(char *type, char *time) {
	assert(type != NULL);
	assert(time != NULL);

	if (!strcmp(type, "moves"))
		do_moves(&my_player, atoi(time), 0);
	else
		fprintf(stderr, "action: unknown type: [%s]\n", type);
}

int get_sequence_col(char *field, PIECE *piece, MOVE *move) {
	int     seq_col;

	for (seq_col = move->col - 1; seq_col >= -1; seq_col--) {
		if (!can_place(field, move->row, seq_col, piece))
			break;
		if (can_drop(field, move->row, seq_col, piece))
			return seq_col;
	}
	for (seq_col = move->col + 1; seq_col < game_settings.field_width; seq_col++) {
		if (!can_place(field, move->row, seq_col, piece))
			break;
		if (can_drop(field, move->row, seq_col, piece))
			return seq_col;
	}
	return 0;
}

void do_move_sequence(char *field, PIECE *piece, MOVE *move) {
	int     seq_col;
	int     i;

	piece->current_shape = move->shape;
	seq_col = get_sequence_col(field, piece, move);

	for (i = 0; i < move->shape; i++)
		fprintf(stdout, "TURNRIGHT,");

	if (seq_col < game_state.this_piece_position_col) {
		for (i = seq_col; i < game_state.this_piece_position_col; i++) {
			fprintf(stdout, "LEFT,");
		}
	}
	else {
		for (i = game_state.this_piece_position_col; i < seq_col; i++) {
			fprintf(stdout, "RIGHT,");
		}
	}

	for (i = game_state.this_piece_position_row; i < move->row; i++) {
		fprintf(stdout, "DOWN,");
	}

	if (seq_col < move->col) {
		for (i = seq_col; i < move->col; i++) {
			fprintf(stdout, "RIGHT,");
		}
	}
	else {
		for (i = move->col; i < seq_col; i++) {
			fprintf(stdout, "LEFT,");
		}
	}
}

void do_moves(PLAYER *player, int time, int is_batch) {
	PIECE   this_piece;
	PIECE   next_piece;
	int     i;

	init_piece(&this_piece, game_state.this_piece_type);
	init_piece(&next_piece, game_state.next_piece_type);

	remove_piece(player->field, &this_piece, game_state.this_piece_position_row, game_state.this_piece_position_col);

	find_best_move(player);

	if (player->move.access_type == MOVE_SEQUENCE) {
		if (!is_batch)
			do_move_sequence(player->field, &this_piece, &player->move);
	}
	else
		if (player->move.shape == T_SPIN_LEFT) {

			assert(game_state.this_piece_type == 'T');

			if (!is_batch) {
				fprintf(stdout, "TURNLEFT,");
				if (player->move.col < game_state.this_piece_position_col) {
					for (i = player->move.col; i < game_state.this_piece_position_col; i++) {
						fprintf(stdout, "LEFT,");
					}
				}
				else {
					for (i = game_state.this_piece_position_col; i < player->move.col; i++) {
						fprintf(stdout, "RIGHT,");
					}
				}
				for (i = game_state.this_piece_position_row; i < player->move.row; i++) {
					fprintf(stdout, "DOWN,");
				}
				fprintf(stdout, "TURNLEFT,");
			}

		}
		else

			if (player->move.shape == T_SPIN_RIGHT) {

				assert(game_state.this_piece_type == 'T');

				if (!is_batch) {
					fprintf(stdout, "TURNRIGHT,");
					if (player->move.col < game_state.this_piece_position_col) {
						for (i = player->move.col; i < game_state.this_piece_position_col; i++) {
							fprintf(stdout, "LEFT,");
						}
					}
					else {
						for (i = game_state.this_piece_position_col; i < player->move.col; i++) {
							fprintf(stdout, "RIGHT,");
						}
					}
					for (i = game_state.this_piece_position_row; i < player->move.row; i++) {
						fprintf(stdout, "DOWN,");
					}
					fprintf(stdout, "TURNRIGHT,");
				}
			}

			else {

				for (i = 0; i < player->move.shape; i++)
					if (!is_batch) fprintf(stdout, "TURNRIGHT,");

				if (player->move.col < game_state.this_piece_position_col) {
					for (i = player->move.col; i < game_state.this_piece_position_col; i++) {
						if (!is_batch) fprintf(stdout, "LEFT,");
					}
				}
				else {
					for (i = game_state.this_piece_position_col; i < player->move.col; i++) {
						if (!is_batch) fprintf(stdout, "RIGHT,");
					}
				}
			}

			if (!is_batch) fprintf(stdout, "DROP\n");

#ifdef DEVELOP
			if (player->move.shape == T_SPIN_LEFT || player->move.shape == T_SPIN_RIGHT)
				this_piece.current_shape = 2;
			else
				this_piece.current_shape = player->move.shape;
			if (player->move.row >= 0)
				place_piece(player->field, &this_piece, player->move.row, player->move.col, CELL_SHAPE);
			//printf("4 line enabled: %d  eval: %f\n", player->four_line_enabled, four_line_eval(player->field));
#endif
}

void place_piece(char *field, PIECE *piece, int row, int col, char cell_shape) {
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
			field[bk_row * game_settings.field_width + bk_col] = cell_shape;
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

int is_filled(char *field, int row, int from_col, int to_col) {
	int     c;

	assert(field != NULL);
	assert(row >= 0 && row < game_settings.field_height);
	assert(from_col >= 0 && from_col <= game_settings.field_width);
	assert(to_col >= 0 && to_col <= game_settings.field_width);

	for (c = from_col; c < to_col; c++) {
		if (field[row * game_settings.field_width + c] == CELL_EMPTY)
			return 0;
	}

	return 1;
}

int t_slot_match(char *field, char *t_slot, int field_row, int field_col) {
	int     slot_row;
	int     slot_col;
	char    slot_cell;
	char    field_cell;

	assert(field != NULL);
	assert(field_row >= 0 && field_row < game_settings.field_height);
	assert(field_col >= 0 && field_col < game_settings.field_width);

	for (slot_row = 0; slot_row < T_SLOT_ROW; slot_row++) {
		for (slot_col = 0; slot_col < T_SLOT_COL; slot_col++) {
			slot_cell = t_slot[slot_row * T_SLOT_COL + slot_col];
			if (slot_cell == '?')
				continue;
			field_cell = field[(field_row + slot_row) * game_settings.field_width + (field_col + slot_col)];
			if (field_cell == CELL_SOLID)
				return 0;
			if (slot_cell == CELL_EMPTY && field_cell == CELL_EMPTY)
				continue;
			if (slot_cell != CELL_EMPTY && field_cell != CELL_EMPTY)
				continue;
			return 0;
		}
	}

	return 1;
}

int t_slot_drop(char * field, int row, int col) {
	int     r;
	int     c;

	assert(field != NULL);
	assert(row >= 0 && row < game_settings.field_height);
	assert(col >= 0 && col < game_settings.field_width);

	for (r = row - 1; r >= 0; r--) {
		for (c = col; c < col + 2; c++) {
			if (field[r * game_settings.field_width + c] != CELL_EMPTY) {
				if (!is_filled(field, r, 0, game_settings.field_width))
					return 0;
			}
		}
	}
	return 1;
}

//        XXX
// XXXXX   XX 7
// XXXXXX XXX 9
double calc_t_spin_complete(char *field, int row1, int row2) {
	int     col;
	double  blocks = 0;

	for (col = 0; col < game_settings.field_width; col++) {
		if (field[row1 * game_settings.field_width + col] != CELL_EMPTY)
			blocks++;
		if (field[row2 * game_settings.field_width + col] != CELL_EMPTY)
			blocks++;
	}

	return blocks / 16;
}

int get_t_spin_data(char *field, T_SPIN *t_spin) {
	int     field_row;
	int     field_col;
	int     row1_filled;
	int     row2_filled;

	assert(field != NULL);
	assert(t_spin != NULL);

	t_spin->row = -1;
	t_spin->col = -1;
	t_spin->type = T_SPIN_NONE;
	t_spin->lines = 0;

	for (field_row = 0; field_row < game_settings.field_height - T_SLOT_ROW; field_row++) {
		for (field_col = 0; field_col < game_settings.field_width - T_SLOT_COL; field_col++) {
			if (t_slot_match(field, T_SLOT_LEFT, field_row, field_col)) {
				row1_filled = is_filled(field, field_row + 1, 0, field_col) && is_filled(field, field_row + 1, field_col + T_SLOT_COL, game_settings.field_width);
				row2_filled = is_filled(field, field_row + 2, 0, field_col) && is_filled(field, field_row + 2, field_col + T_SLOT_COL, game_settings.field_width);
				if (/*row1_filled ||*/ row2_filled) {
					if (t_slot_drop(field, field_row, field_col)) {
						t_spin->row = field_row;
						t_spin->col = field_col;
						t_spin->type = T_SPIN_LEFT;
						t_spin->lines = (row1_filled && row2_filled) ? 2 : 1;
						t_spin->percent_complete = calc_t_spin_complete(field, field_row + 1, field_row + 2);
						return 1;
					}
				}
			}
			if (t_slot_match(field, T_SLOT_RIGHT, field_row, field_col)) {
				row1_filled = is_filled(field, field_row + 1, 0, field_col) && is_filled(field, field_row + 1, field_col + T_SLOT_COL, game_settings.field_width);
				row2_filled = is_filled(field, field_row + 2, 0, field_col) && is_filled(field, field_row + 2, field_col + T_SLOT_COL, game_settings.field_width);
				if (/*row1_filled ||*/ row2_filled) {
					if (t_slot_drop(field, field_row, field_col)) {
						t_spin->row = field_row;
						t_spin->col = field_col;
						t_spin->type = T_SPIN_RIGHT;
						t_spin->lines = (row1_filled && row2_filled) ? 2 : 1;
						t_spin->percent_complete = calc_t_spin_complete(field, field_row + 1, field_row + 2);
						return 1;
					}
				}
			}
		}
	}

	return 0;
}

int can_drop(char *field, int row, int col, PIECE *piece) {
	int     prow;
	int     pcol;

	int		i;

	assert(field != NULL);
	assert(row >= -1 && row < game_settings.field_height);
	assert(col >= -2 && col < game_settings.field_width);
	assert(piece != NULL);
	assert(strchr(SHAPE_TYPES, piece->type) != NULL);

	for (prow = 0; prow < piece->size; prow++) {
		for (pcol = 0; pcol < piece->size; pcol++) {
			if (piece->shapes[piece->current_shape][prow * piece->size + pcol] == CELL_EMPTY)
				continue;
			for (i = row + prow; i > 0; i--) {
				if (i < -1 || i >= game_settings.field_height)
					return 0;
				if (col + pcol < 0 || col + pcol >= game_settings.field_width)
					return 0;
				if (field[i * game_settings.field_width + (col + pcol)] != CELL_EMPTY)
					return 0;
			}
		}
	}
	return 1;
}

int can_place(char *field, int target_row, int target_col, PIECE *piece) {
	int     piece_row;
	int     piece_col;
	int		block_row;
	int		block_col;

	assert(field != NULL);
	assert(target_row >= -1 && target_row < game_settings.field_height);
	assert(target_col >= -2 && target_col < game_settings.field_width);
	assert(piece != NULL);
	assert(strchr(SHAPE_TYPES, piece->type) != NULL);

	for (piece_row = 0; piece_row < piece->size; piece_row++) {
		for (piece_col = 0; piece_col < piece->size; piece_col++) {
			if (piece->shapes[piece->current_shape][piece_row * piece->size + piece_col] == CELL_EMPTY)
				continue;
			block_row = target_row + piece_row;
			block_col = target_col + piece_col;
			if (block_row < 0)
				continue;
			if (block_row >= game_settings.field_height)
				return 0;
			if (block_col < 0 || block_col >= game_settings.field_width)
				return 0;
			if (field[block_row * game_settings.field_width + block_col] != CELL_EMPTY)
				return 0;
		}
	}

	return 1;
}

int can_move(char *field, int row, int col, PIECE *piece) {
	int		i;

	assert(field != NULL);
	assert(row >= 0 && row < game_settings.field_height);
	assert(col >= -2 && col < game_settings.field_width);
	assert(piece != NULL);
	assert(strchr(SHAPE_TYPES, piece->type) != NULL);

	for (i = col - 1; i >= -1; i--) {
		if (!can_place(field, row, i, piece))
			break;
		if (can_drop(field, row, i, piece))
			return 1;
	}
	for (i = col + 1; i < game_settings.field_width; i++) {
		if (!can_place(field, row, i, piece))
			break;
		if (can_drop(field, row, i, piece))
			return 1;
	}

	return 0;
}

int get_access_type(char *field, int row, int col, PIECE *piece) {
	if (can_drop(field, row, col, piece))
		return MOVE_DROP;
	if (can_move(field, row, col, piece))
		return MOVE_SEQUENCE;
	return MOVE_NONE;
}

int piece_fits_at(char *field, int row, int col, PIECE *piece) {
	int     pc_row;
	int     pc_col;
	int		bk_row;
	int		bk_col;
	int		empty = 0;

	assert(field != NULL);
	assert(row >= -1 && row < game_settings.field_height);
	assert(col >= -2 && col < game_settings.field_width);
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

int search_fit(char *field, int row, int col, PIECE *piece) {
	int     pc_row;
	int     pc_col;
	int		bk_row;
	int		bk_col;

	assert(field != NULL);
	assert(row >= -1 && row < game_settings.field_height);
	assert(col >= -3 && col < game_settings.field_width);
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
		}
	}

	return 1;
}

int try_right(char *field, PIECE *piece, int count) {
	int		i;

	for (i = 0; i < count; i++) {
		rotate_right(piece);
		if (!search_fit(field, piece->row, piece->col, piece))
			return 0;
	}

	return 1;
}

int try_left(char *field, PIECE *piece, int count) {
	int		i;

	for (i = 0; i < count; i++) {
		rotate_left(piece);
		if (!search_fit(field, piece->row, piece->col, piece))
			return 0;
	}

	return 1;
}

char	*closed_list = NULL;

//int search_path(char *field, PIECE *goal, PIECE *current, char *path, int pos) {
//	int		i;
//	int		rotation;
//	PIECE	piece;
//	int		saved_pos;
//
//	if (closed_list[current->row * game_settings.field_width + current->col * 4 + current->current_shape] == 1)
//		return 0;
//
//	if (!search_fit(field, current->row, current->col, current)) {
//		closed_list[current->row * game_settings.field_width + current->col * 4 + current->current_shape] = 1;
//		return 0;
//	}
//
//	place_piece(field, current, current->row, current->col);
//	//place_piece(field, goal, goal->row, goal->col);
//	print_player(&my_player);
//	pause("search_path");
//	remove_piece(field, current, current->row, current->col);
//	//remove_piece(field, goal, goal->row, goal->col);
//
//	if (current->current_shape == goal->current_shape) {
//		if (current->row == goal->row && current->col == goal->col) {
//			path[pos++] = PATH_DROP;
//			path[pos] = PATH_END;
//			return 1;
//		}
//	}
//
//	saved_pos = pos;
//	for (i = 0; i < ROTATE_SIZE; i++) {
//		memcpy(&piece, current, sizeof(PIECE));
//		pos = saved_pos;
//
//		rotation = ROTATIONS[i];
//
//		switch (rotation)
//		{
//		case ROTATE_NONE: 
//			break;
//		case ROTATE_1R:
//			if (!try_right(field, &piece, 1))
//				continue;
//			path[pos++] = PATH_TURNR;
//			break;
//		case ROTATE_2R:
//			if (!try_right(field, &piece, 2))
//				continue;
//			path[pos++] = PATH_TURNR;
//			path[pos++] = PATH_TURNR;
//			break;
//		case ROTATE_3R:
//			if (!try_right(field, &piece, 3))
//				continue;
//			path[pos++] = PATH_TURNR;
//			path[pos++] = PATH_TURNR;
//			path[pos++] = PATH_TURNR;
//			break;
//		case ROTATE_1L:
//			if (!try_left(field, &piece, 1))
//				continue;
//			path[pos++] = PATH_TURNL;
//			break;
//		case ROTATE_2L:
//			if (!try_left(field, &piece, 2))
//				continue;
//			path[pos++] = PATH_TURNL;
//			path[pos++] = PATH_TURNL;
//			break;
//		case ROTATE_3L:
//			if (!try_left(field, &piece, 3))
//				continue;
//			path[pos++] = PATH_TURNL;
//			path[pos++] = PATH_TURNL;
//			path[pos++] = PATH_TURNL;
//			break;
//		}
//
//		piece.row++;
//		path[pos++] = PATH_DOWN;
//		if (search_path(field, goal, &piece, path, pos)) {
//			return 1;
//		}
//		piece.row--;
//		pos--;
//
//		piece.col++;
//		path[pos++] = PATH_RIGHT;
//		if (search_path(field, goal, &piece, path, pos)) {
//			return 1;
//		}
//		piece.col--;
//		pos--;
//
//		piece.col--;
//		path[pos++] = PATH_LEFT;
//		if (search_path(field, goal, &piece, path, pos)) {
//			return 1;
//		}
//
//	}
//
//	closed_list[current->row * game_settings.field_width + current->col * 4 + current->current_shape] = 1;
//
//	return 0;
//}

typedef struct {
	int		deleted;
	int		rotate;	// ROTATE_RIGHT, ROTATE_LEFT
	int		path;	// DOWN, RIGHT, LEFT
	int		type;
	int		shape;
	int		row;
	int		col;
	int		f_score;
	int		g_score;
}	NODE;

#define NODE_LIST_SIZE	1024

typedef struct {
	NODE	nodes[NODE_LIST_SIZE];
	int		count;
}	NODE_LIST;

#define COME_FROM_SIZE	1024

typedef struct {
	NODE	current[COME_FROM_SIZE];
	NODE	from[COME_FROM_SIZE];
	int		count;
}	COME_FROM;

#define A_STAR_INFINITY		1000000

int a_star_add_node(NODE_LIST *list) {
	int		i;

	for (i = 0; i < list->count; i++) {
		if (list->nodes[i].deleted)
			return i;
	}

	if (list->count == NODE_LIST_SIZE) {
		fprintf(stderr, "a_star_add_node list size overflow > %d", NODE_LIST_SIZE);
		return -1;
	}
	
	list->count++;
	return list->count - 1;
}

void a_star_remove_node(NODE_LIST *list, int index) {
	list->nodes[index].deleted = 1;
}

int a_star_lowest_f_score(NODE_LIST *list) {
	int		small = A_STAR_INFINITY;
	int		index = 0;
	int		i;

	for (i = 0; i < list->count; i++) {
		if (!list->nodes[i].deleted && list->nodes[i].f_score < small) {
			small = list->nodes[i].f_score;
			index = i;
		}
	}
	
	return index;
}

int a_star_dist(int r1, int c1, int r2, int c2) {
	int		d1 = r1 - r2;
	int		d2 = c1 - c2;

	if (d1 < 0)
		d1 = -d1;
	if (d2 < 0)
		d2 = -d2;

	if (d1 - d2 >= 0)
		return d1 - d2;
	else
		return d2 - d1;
}

void a_star_find_neighbors(char *field, NODE_LIST *list, NODE *current, PIECE *goal) {
	int		i;
	int		j;
	NODE	neighbor;
	PIECE	piece;

	list->count = 0;
	init_piece(&piece, current->type);

	for (i = 0; i < ROTATE_SIZE; i++) {
		piece.row = current->row;
		piece.col = current->col;
		piece.current_shape = current->shape;

		switch (ROTATIONS[i])
		{
		case ROTATE_NONE:
			break;
		case ROTATE_1R:
			if (!try_right(field, &piece, 1))
				continue;
			break;
		case ROTATE_2R:
			if (!try_right(field, &piece, 2))
				continue;
			break;
		case ROTATE_3R:
			if (!try_right(field, &piece, 3))
				continue;
			break;
		case ROTATE_1L:
			if (!try_left(field, &piece, 1))
				continue;
			break;
		case ROTATE_2L:
			if (!try_left(field, &piece, 2))
				continue;
			break;
		case ROTATE_3L:
			if (!try_left(field, &piece, 3))
				continue;
			break;
		}
		
		neighbor.deleted = 0;
		neighbor.type = piece.type;
		neighbor.shape = piece.current_shape;
		neighbor.rotate = ROTATIONS[i];
		neighbor.g_score = 0;
		
		if (search_fit(field, piece.row + 1, piece.col, &piece)) {
			neighbor.row = piece.row + 1;
			neighbor.col = piece.col;
			neighbor.path = PATH_DOWN;
			neighbor.f_score = a_star_dist(neighbor.row, neighbor.col, goal->row, goal->col) + ROTATIONS[i] + neighbor.path;
			j = a_star_add_node(list);
			memcpy(&list->nodes[j], &neighbor, sizeof(NODE));
		}
		if (search_fit(field, piece.row, piece.col + 1, &piece)) {
			neighbor.row = piece.row;
			neighbor.col = piece.col + 1;
			neighbor.path = PATH_RIGHT;
			neighbor.f_score = a_star_dist(neighbor.row, neighbor.col, goal->row, goal->col) + ROTATIONS[i] + neighbor.path;
			j = a_star_add_node(list);
			memcpy(&list->nodes[j], &neighbor, sizeof(NODE));
		}
		if (search_fit(field, piece.row, piece.col - 1, &piece)) {
			neighbor.row = piece.row;
			neighbor.col = piece.col - 1;
			neighbor.path = PATH_LEFT;
			neighbor.f_score = a_star_dist(neighbor.row, neighbor.col, goal->row, goal->col) + ROTATIONS[i] + neighbor.path;
			j = a_star_add_node(list);
			memcpy(&list->nodes[j], &neighbor, sizeof(NODE));
		}
	}
}

int a_star_count(NODE_LIST *list) {
	int		i;
	int		c = 0;

	for (i = 0; i < list->count; i++)
		if (!list->nodes[i].deleted)
			c++;

	return c;
}

int a_star_first(NODE_LIST *list) {
	int		i;

	for (i = 0; i < list->count; i++)
		if (!list->nodes[i].deleted)
			return i;

	return -1;
}

int a_star_equals(NODE *node1, NODE *node2) {
	if (node1->shape == node2->shape && node1->rotate == node2->rotate) {
		if (node1->row == node2->row && node1->col == node2->col) {
			if (node1->path == node2->path) {
				return 1;
			}
		}
	}
	return 0;
}

int a_star_contains(NODE_LIST *list, NODE *node) {
	int		i;

	for (i = 0; i < list->count; i++) {
		if (list->nodes[i].deleted)
			continue;
		if (a_star_equals(&list->nodes[i], node)) {
			return 1;
		}
	}

	return 0;
}

int a_star_come_from_add(COME_FROM *come_from, NODE *current, NODE *from) {
	int		i;

	for (i = 0; i < come_from->count; i++) {
		if (a_star_equals(&come_from->current[i], current)) {
			memcpy(&come_from->from[i], from, sizeof(NODE));
			return 1;
		}
	}

	if (come_from->count == COME_FROM_SIZE) {
		fprintf(stderr, "a_star_come_from overflow: size > %d\n", COME_FROM_SIZE);
		return 0;
	}

	memcpy(&come_from->current[come_from->count], current, sizeof(NODE));
	memcpy(&come_from->from[come_from->count], from, sizeof(NODE));
	come_from->count++;

	return 1;
}

int	a_star_come_from_contains_key(COME_FROM *come_from, NODE *node) {
	int		i;

	for (i = 0; i < come_from->count; i++) {
		if (a_star_equals(&come_from->current[i], node)) {
			return 1;
		}
	}
	return 0;
}

int a_star_come_from_get(COME_FROM *come_from, NODE *node, NODE *from) {
	int		i;

	for (i = 0; i < come_from->count; i++) {
		if (a_star_equals(&come_from->current[i], node)) {
			memcpy(from, &come_from->from[i], sizeof(NODE));
			return 1;
		}
	}

	return 0;
}

//function reconstruct_path(Came_From,current)
//    total_path := [current]
//    while current in Came_From.Keys:
//        current := Came_From[current]
//        total_path.append(current)
//    return total_path
void a_star_reconstruct_path(COME_FROM *come_from, NODE *current, char *path) {
	char	reversed[PATH_MAX];
	int		i = 0;
	int		j = 0;
	NODE	temp;
	NODE	from;

	reversed[i++] = current->path;
	switch (current->rotate)
	{
	case ROTATE_1R:
		reversed[i++] = PATH_TURNR;
		break;
	case ROTATE_2R:
		reversed[i++] = PATH_TURNR;
		reversed[i++] = PATH_TURNR;
		break;
	case ROTATE_3R:
		reversed[i++] = PATH_TURNR;
		reversed[i++] = PATH_TURNR;
		reversed[i++] = PATH_TURNR;
		break;
	case ROTATE_1L:
		reversed[i++] = PATH_TURNL;
		break;
	case ROTATE_2L:
		reversed[i++] = PATH_TURNL;
		reversed[i++] = PATH_TURNL;
		break;
	case ROTATE_3L:
		reversed[i++] = PATH_TURNL;
		reversed[i++] = PATH_TURNL;
		reversed[i++] = PATH_TURNL;
		break;
	}
	memcpy(&temp, current, sizeof(NODE));
	while (a_star_come_from_contains_key(come_from, &temp)) {
		a_star_come_from_get(come_from, &temp, &from);
		if (from.path == PATH_NONE)
			break;
		reversed[i++] = from.path;
		switch (from.rotate)
		{
		case ROTATE_1R:
			reversed[i++] = PATH_TURNR;
			break;
		case ROTATE_2R:
			reversed[i++] = PATH_TURNR;
			reversed[i++] = PATH_TURNR;
			break;
		case ROTATE_3R:
			reversed[i++] = PATH_TURNR;
			reversed[i++] = PATH_TURNR;
			reversed[i++] = PATH_TURNR;
			break;
		case ROTATE_1L:
			reversed[i++] = PATH_TURNL;
			break;
		case ROTATE_2L:
			reversed[i++] = PATH_TURNL;
			reversed[i++] = PATH_TURNL;
			break;
		case ROTATE_3L:
			reversed[i++] = PATH_TURNL;
			reversed[i++] = PATH_TURNL;
			reversed[i++] = PATH_TURNL;
			break;
		}
		memcpy(&temp, &from, sizeof(NODE));
	}

	while (--i >= 0) {
		path[j++] = reversed[i];
	}
	path[j] = PATH_NONE;
}

int a_star(char *field, PIECE *goal, PIECE *start, char *path) {
	NODE_LIST	closed_nodes;
	NODE_LIST	open_nodes;
	NODE_LIST	neighbor_list;
	COME_FROM	come_from;
	int			i;
	int			j;
	int			k;
	NODE		current;
	NODE		neighbor;
	int			g_score;

	closed_nodes.count = 0;
	open_nodes.count = 0;
	come_from.count = 0;

	i = a_star_add_node(&open_nodes);
	if (i == -1)
		return 0;
	open_nodes.nodes[i].deleted = 0;
	open_nodes.nodes[i].shape = start->current_shape;
	open_nodes.nodes[i].type = start->type;
	open_nodes.nodes[i].row = start->row;
	open_nodes.nodes[i].col = start->col;
	open_nodes.nodes[i].rotate = ROTATE_NONE;
	open_nodes.nodes[i].path = PATH_NONE;
	open_nodes.nodes[i].f_score = a_star_dist(goal->row, goal->col, start->row, start->col);
	open_nodes.nodes[i].g_score = 0;

	while (a_star_count(&open_nodes) != 0) {
		i = a_star_lowest_f_score(&open_nodes);
		memcpy(&current, &open_nodes.nodes[i], sizeof(NODE));

		if (current.shape == goal->current_shape) {
			if (current.row == goal->row && current.col == goal->col) {
				a_star_reconstruct_path(&come_from, &current, path);
				return 1;
			}
		}

		a_star_remove_node(&open_nodes, i);
		i = a_star_add_node(&closed_nodes);
		if (i == -1)
			return 0;
		memcpy(&closed_nodes.nodes[i], &current, sizeof(NODE));

		a_star_find_neighbors(field, &neighbor_list, &current, goal);

		while (a_star_count(&neighbor_list) != 0) {
			j = a_star_first(&neighbor_list);
			if (j == -1) {
				fprintf(stderr, "a_star no neighbors found for node (%d, %d)\n", current.row, current.col);
				continue;
			}
			memcpy(&neighbor, &neighbor_list.nodes[j], sizeof(NODE));
			a_star_remove_node(&neighbor_list, j);

			if (a_star_contains(&closed_nodes, &neighbor))
				continue;

			g_score = current.g_score + a_star_dist(current.row, current.col, neighbor.row, neighbor.col);

			if (!a_star_contains(&open_nodes, &neighbor)) {
				k = a_star_add_node(&open_nodes);
				if (k == -1)
					return 0;
				memcpy(&open_nodes.nodes[k], &neighbor, sizeof(NODE));
			}
			else
				if (g_score >= neighbor.g_score)
					continue;
			if (!a_star_come_from_add(&come_from, &neighbor, &current))
				return 0;
			neighbor.g_score = g_score;
			neighbor.f_score = g_score + a_star_dist(neighbor.row, neighbor.row, goal->row, goal->col);
		}
	}
	
	return 0;
}


//function A*(start,goal)
//    ClosedSet := {}    	  // The set of nodes already evaluated.
//    OpenSet := {start}    // The set of tentative nodes to be evaluated, initially containing the start node
//    Came_From := the empty map    // The map of navigated nodes.
//
//    g_score := map with default value of Infinity
//    g_score[start] := 0    // Cost from start along best known path.
//    // Estimated total cost from start to goal through y.
//    f_score := map with default value of Infinity
//    f_score[start] := heuristic_cost_estimate(start, goal)
//
//    while OpenSet is not empty
//        current := the node in OpenSet having the lowest f_score[] value
//        if current = goal
//            return reconstruct_path(Came_From, goal)
//
//        OpenSet.Remove(current)
//        ClosedSet.Add(current)
//        for each neighbor of current
//            if neighbor in ClosedSet
//                continue		// Ignore the neighbor which is already evaluated.
//            tentative_g_score := g_score[current] + dist_between(current,neighbor) // length of this path.
//            if neighbor not in OpenSet	// Discover a new node
//                OpenSet.Add(neighbor)
//            else if tentative_g_score >= g_score[neighbor]
//                continue		// This is not a better path.
//
//            // This path is the best until now. Record it!
//            Came_From[neighbor] := current
//            g_score[neighbor] := tentative_g_score
//            f_score[neighbor] := g_score[neighbor] + heuristic_cost_estimate(neighbor, goal)
//
//    return failure
//


void debug_path(char *path) {
	int		i;
	PIECE	p;
	char	move[100];

	init_piece(&p, game_state.this_piece_type);
	p.row = game_state.this_piece_position_row;
	p.col = game_state.this_piece_position_col;

	for (i = 0; path[i] != PATH_NONE; i++) {
		switch (path[i])
		{
		case PATH_DOWN:
			strcpy(move, "DOWN,");
			p.row++;
			break;
		case PATH_TURNL:
			strcpy(move, "TURNLEFT,");
			rotate_left(&p);
			break;
		case PATH_TURNR:
			strcpy(move, "TURNRIGHT,");
			rotate_right(&p);
			break;
		case PATH_RIGHT:
			strcpy(move, "RIGHT,");
			p.col++;
			break;
		case PATH_LEFT:
			strcpy(move, "LEFT,");
			p.col--;
			break;
		}
#ifdef  DEVELOP
		place_piece(my_player.field, &p, p.row, p.col, CELL_SHAPE);
		print_player(&my_player);
		printf("%s", move);
		pause("debug_path");
		remove_piece(my_player.field, &p, p.row, p.col);
#endif //  DEVELOP
	}
#ifdef  DEVELOP
	place_piece(my_player.field, &p, p.row, p.col, CELL_SHAPE);
	print_player(&my_player);
	remove_piece(my_player.field, &p, p.row, p.col);
	printf("DROP\n");
	pause("debug_path_END");
#endif
}

int find_path(char *field, PIECE *piece, char *path, int to_row, int to_col, int to_shape) {
	PIECE	goal;
	PIECE	current;
	int		ROUND_TEST = 999;

	if (game_state.round >= ROUND_TEST) {
		//print_player(&my_player);
		//printf("find_path.goal: %c %d %d %d\n", piece->type, to_row, to_col, to_shape);
		//pause("find_path_start");
	}

	memcpy(&goal, piece, sizeof(PIECE));
	goal.row = to_row;
	goal.col = to_col;
	goal.current_shape = to_shape;

	memcpy(&current, piece, sizeof(PIECE));
	current.row = game_state.this_piece_position_row;
	current.col = game_state.this_piece_position_col;
	current.current_shape = 0;

	path[0] = PATH_NONE;

	a_star(field, &goal, &current, path);
	if (game_state.round >= ROUND_TEST)
		debug_path(path);

	if (strlen(path) == 0)
		return 0;
	else
		return 1;
}

void gen_moves(char *field, PIECE *piece, MOVE_LIST *move_list) {
	int		i;
	int		row;
	int		col;
	int		access_type;
#ifdef DEVELOP
	char	path[PATH_MAX];
#endif

	move_list->count = 0;

	for (i = 0; i < piece->max_shapes; i++) {
		piece->current_shape = i;
		for (row = 0; row < game_settings.field_height; row++) {
			for (col = -1; col < game_settings.field_width; col++) {
				if (piece_fits_at(field, row, col, piece)) {
					access_type = get_access_type(field, row, col, piece);
					if (access_type == MOVE_NONE) {
#ifdef DEVELOP
						place_piece(field, piece, row, col, piece->type);
						print_player(&my_player);
						remove_piece(field, piece, row, col);
						pause("gen_moves.no moves");


						//find_path(field, piece, path, row, col, i);
						//if (strlen(path)) {
						//	place_piece(field, piece, row, col);
						//	print_player(&my_player);
						//	remove_piece(field, piece, row, col);
						//	printf("no move: %c (%d,%d)->(%d,%d)\n", piece->type, piece->row, piece->col, row, col);
						//	debug_path(path);
						//	pause("gen_moves");
						//}
#endif
						continue;
					}
					memset(&move_list->moves[move_list->count], 0, sizeof(MOVE));
					move_list->moves[move_list->count].access_type = access_type;
					move_list->moves[move_list->count].shape = piece->current_shape;
					move_list->moves[move_list->count].score = -MY_MAXVAL;
					move_list->moves[move_list->count].row = row;
					move_list->moves[move_list->count].col = col;
					move_list->count++;
					//place_piece(field, piece, row, col);
					//print_player(&my_player);
					//pause("gen_move");
					//remove_piece(field, piece, row, col);
				}
			}
		}
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
			while (down_row < row + pc_row + 2 && down_row < game_settings.field_height) {
				if (field[down_row * game_settings.field_width + down_col] == CELL_EMPTY)
					holes += down_row;
				down_row++;
			}
		}
	}

	return holes;
}

int fill_lines_count(char *field) {
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

int is_full(char *field, int row) {
	int		col;

	for (col = 0; col < game_settings.field_width; col++) {
		if (field[row * game_settings.field_width + col] == CELL_EMPTY)
			return 0;
	}

	return 1;
}

int is_holes_blocked(char *field, int row, int exc_col) {
	int     col;

	for (col = 0; col < game_settings.field_width; col++) {
		if (col == exc_col)
			continue;
		if (field[row * game_settings.field_width + col] == CELL_EMPTY) {
			if (field[(row - 1) * game_settings.field_width + col] != CELL_EMPTY) {
				return 1;
			}
		}
	}
	return 0;
}

int block_count(PLAYER *player) {
	int     i;
	int     c = 0;

	for (i = 0; i < game_settings.field_height * game_settings.field_width; i++) {
		if (player->field[i] == CELL_SOLID)
			break;
		if (player->field[i] != CELL_EMPTY)
			c++;
	}

	return c;
}

double high_occupancy(char *field) {
	double	penalty[10] = { 1.0, 0.9, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1, 0.05 };
	int		row;
	int		col;
	double	val = 0;

	for (row = 0; row < 10; row++) {
		for (col = 0; col < game_settings.field_width; col++) {
			if (field[row * game_settings.field_width + col] != CELL_EMPTY)
				val -= penalty[row];
		}
	}
	return val;
}

int is_blocked(char *field, int row, int col) {
    while(--row >= 0) {
        if (field[row * game_settings.field_width + col] == CELL_EMPTY)
            continue;
        if (!is_full(field, row))
            return 1;
    }
    return 0;
}

int count_block_holes(char *field, int row, int col) {
	int     count = 0;

	while (--row >= 0) {
		if (is_full(field, row))
			continue;
		if (field[row * game_settings.field_width + col] == CELL_EMPTY)
			continue;
		count++;
	}

	return count;
}

int almost_full_count(char *field) {
    int     row;
    int     col;
    int     block_count;
    int     lines_count = 0;

    for (row = 0; row < game_settings.field_height; row++) {
        if (is_full(field, row))
            continue;
        block_count = 0;
        for (col = 0; col < game_settings.field_width; col++) {
            if (field[row * game_settings.field_width + col] != CELL_EMPTY)
                block_count++;
        }
        if (block_count >= game_settings.field_width - 2)
            lines_count++;
    }
    return lines_count;
}

double evaluate(PLAYER *player, int prev_removed, int prev_row) {
	int		field_row;
	int		field_col;
	double	score[2] = { 0, 0 };
	double	final_score;
	int		phase;
	int		game_phase;
	int		holes_row;
	int		this_height;
	int		col_heights[10];
	int		row_first_solid;
	int		max_heigth;
	int		sum_heights;
	int		avg_heights;
	int		empty_holes;
	double	side_holes;
	int		blocked_holes;

	int		heights = 0;
	int		lines = 0;
	int		holes = 0;
	int		bumps = 0;
	//T_SPIN  t_spin;

	assert(player != NULL);
	assert(player->field != NULL);
	assert(prev_removed >= 0 && prev_removed <= 4);
	assert(game_settings.field_width == 10);

	row_first_solid = game_settings.field_height;
	for (field_row = 0; field_row < game_settings.field_height; field_row++) {
		if (player->field[field_row * game_settings.field_width] == CELL_SOLID) {
			row_first_solid = field_row;
			break;
		}
	}

	side_holes = 0;
	for (field_row = 0; field_row < game_settings.field_height; field_row++) {
		if (player->field[field_row * game_settings.field_width + 0] == CELL_EMPTY) {
			side_holes -= 0.4;
		}
		if (player->field[field_row * game_settings.field_width + game_settings.field_width - 1] == CELL_EMPTY) {
			side_holes -= 0.4;
		}
		for (field_col = 1; field_col < game_settings.field_width - 1; field_col++) {
			if (player->field[field_row * game_settings.field_width + field_col] == CELL_EMPTY) {
				side_holes += 0.1;
			}
		}
	}

	blocked_holes = 0;
	for (field_col = 0; field_col < game_settings.field_width; field_col++) {
		col_heights[field_col] = 0;
		for (field_row = 0; field_row < game_settings.field_height; field_row++) {

			if (player->field[field_row * game_settings.field_width + field_col] == CELL_EMPTY)
				continue;

			for (holes_row = field_row + 1; holes_row < game_settings.field_height; holes_row++) {
				if (player->field[holes_row * game_settings.field_width + field_col] == CELL_EMPTY) {
                    if (is_blocked(player->field, holes_row, field_col)) {
                        holes++;
						blocked_holes += count_block_holes(player->field, holes_row, field_col);

                    }
                }
			}

			this_height = game_settings.field_height - field_row;
			col_heights[field_col] = this_height;
			heights += this_height;
			break;
		}
	}

	max_heigth = 0;
	sum_heights = 0;
	for (field_col = 0; field_col < game_settings.field_width; field_col++) {
		sum_heights += col_heights[field_col];
		if (col_heights[field_col] > max_heigth)
			max_heigth = col_heights[field_col];
	}
	avg_heights = sum_heights / game_settings.field_width;
	empty_holes = 0;
	for (field_col = 0; field_col < game_settings.field_width; field_col++) {
		if (col_heights[field_col] == 0)
			empty_holes += avg_heights;
	}

	for (field_col = 0; field_col < game_settings.field_width; field_col++) {
		if (field_col > 0)
			bumps += abs(col_heights[field_col] - col_heights[field_col - 1]);
	}

	lines = fill_lines_count(player->field);
	if (lines * game_settings.field_width == block_count(player))
		return MY_MAXVAL;

	for (phase = 0; phase < 2; phase++) {

		score[phase] += player->eval.param.heigh[phase] * heights;
		score[phase] += player->eval.param.lines[phase] * lines;
		score[phase] += player->eval.param.holes[phase] * holes;
		score[phase] += player->eval.param.bumps[phase] * bumps;
		score[phase] += player->eval.param.shole[phase] * side_holes;
		score[phase] += player->eval.param.empty[phase] * empty_holes;
		score[phase] += player->eval.param.maxht[phase] * max_heigth;
		score[phase] += player->eval.param.lorow[phase] * prev_row;
		score[phase] += player->eval.param.block[phase] * blocked_holes;

		if (max_heigth < 5 && row_first_solid > 18 && (prev_removed < 2 && lines - prev_removed < 2)) {
			score[phase] += -(lines * 10);
		}
		else
		{
			if (prev_removed > 1) {
				score[phase] += prev_removed * player->eval.param.mult1[phase];
			}
			if (lines - prev_removed > 1 && prev_removed == 0) {
				score[phase] += (lines - prev_removed) * player->eval.param.mult2[phase];
			}
		}
	}
    score[0] += (player->eval.param.lines[1] - player->eval.param.lines[0]) / (game_settings.field_height * 2) * (almost_full_count(player->field) - 4); // - 4 good

	game_phase = row_first_solid;
	final_score = ((score[0] * game_phase) + (score[1] * (game_settings.field_height - game_phase))) / game_settings.field_height;

	final_score += high_occupancy(player->field);

	return final_score;
}

int get_bottom_row(char *field) {
	int		row;
	
	if (field[(game_settings.field_height - 1) * game_settings.field_width] != CELL_SOLID)
		return game_settings.field_height - 1;

	for (row = game_settings.field_height - 1; row >= 0; row--) {
		if (field[row * game_settings.field_width] == CELL_SOLID)
			continue;
		return row;
	}

	return 0;
}

int get_highest_column(char *field, int bottom_row) {
	int		row;
	int		col;

	for (row = 0; row < bottom_row; row++) {
		for (col = 0; col < game_settings.field_width; col++) {
			if (field[row * game_settings.field_width + col] != CELL_EMPTY)
				return bottom_row - row;
		}
	}
	return 0;
}

void update_top_move(PLAYER *player) {
	int		i;
	int		low_index;
	double	low_score;

	if (player->top.count < 50) {
		memcpy(&player->top.moves[player->top.count], &player->move, sizeof(MOVE));
		player->top.count++;
		return;
	}
	low_score = -MY_MAXVAL;
	low_index = 0;
	for (i = 0; i < player->top.count; i++) {
		if (player->top.moves[i].score < low_score) {
			low_score = player->top.moves[i].score;
			low_index = i;
		}
	}
	memcpy(&player->top.moves[low_index], &player->move, sizeof(MOVE));
}

void find_best_move(PLAYER *player) {
	double		score;
	int			i, j;
	PIECE		this_piece;
	PIECE		next_piece;
	MOVE_LIST	this_moves;
	MOVE_LIST	next_moves;
	int			filled_lines = 0;
	T_SPIN      t_spin;
	//char		path[PATH_MAX];

	assert(player != NULL);
	assert(player->field != NULL);

	player->move.row = -1;
	player->move.col = -1;
	player->move.access_type = MOVE_NONE;
	player->move.shape = -1;
	player->move.score = -MY_MAXVAL;
	player->top.count = 0;

	if (game_state.this_piece_type == 'T') {
		player->t_spin_allow = 0;
		if (get_t_spin_data(player->field, &t_spin)) {
			player->move.access_type = MOVE_T_SPIN;
			player->move.shape = t_spin.type;
			player->move.row = t_spin.row;
			player->move.col = t_spin.col;
			update_top_move(player);
			return;
		}
	}

	init_piece(&this_piece, game_state.this_piece_type);
	init_piece(&next_piece, game_state.next_piece_type);

	gen_moves(player->field, &this_piece, &this_moves);

	for (i = 0; i < this_moves.count; i++) {

		this_piece.current_shape = this_moves.moves[i].shape;

		place_piece(player->field, &this_piece, this_moves.moves[i].row, this_moves.moves[i].col, CELL_SHAPE);

		filled_lines = fill_lines_count(player->field);

		if (game_state.next_piece_type == 'T') {

			if (get_t_spin_data(player->field, &t_spin)) {

				next_piece.current_shape = 2;
				place_piece(player->field, &next_piece, t_spin.row, t_spin.col, CELL_SHAPE);

				score = evaluate(player, filled_lines, this_moves.moves[i].row)
					+ t_spin.lines * player->eval.param.tslin
					+ t_spin.percent_complete * player->eval.param.tspct;

				if (score > player->move.score) {
					player->move.access_type = this_moves.moves[i].access_type;
					player->move.score = score;
					player->move.row = this_moves.moves[i].row;
					player->move.col = this_moves.moves[i].col;
					player->move.shape = this_moves.moves[i].shape;
					player->t_spin_allow = 1;
					update_top_move(player);
				}

				//print_player(&my_player);
				//printf("t_slot: (%d, %d) type: %d lines: %d\n", t_spin.row, t_spin.col, t_spin.type, t_spin.lines);
				//pause("tspin next");

				remove_piece(player->field, &next_piece, t_spin.row, t_spin.col);
			}
		}

		gen_moves(player->field, &next_piece, &next_moves);

		for (j = 0; j < next_moves.count; j++) {

			next_piece.current_shape = next_moves.moves[j].shape;

			place_piece(player->field, &next_piece, next_moves.moves[j].row, next_moves.moves[j].col, CELL_SHAPE);

			score = evaluate(player, filled_lines, this_moves.moves[i].row);

            if (score > player->move.score) {
				player->move.score = score;
				player->move.access_type = this_moves.moves[i].access_type;
				player->move.row = this_moves.moves[i].row;
				player->move.col = this_moves.moves[i].col;
				player->move.shape = this_moves.moves[i].shape;
				player->t_spin_allow = 0;
				update_top_move(player);
			}

			remove_piece(player->field, &next_piece, next_moves.moves[j].row, next_moves.moves[j].col);
		}

		remove_piece(player->field, &this_piece, this_moves.moves[i].row, this_moves.moves[i].col);
	}

	this_piece.row = game_state.this_piece_position_row;
	this_piece.col = game_state.this_piece_position_col;
	this_piece.current_shape = 0;

	//find_path(player->field, &this_piece, path, player->move.row, player->move.col, player->move.shape);
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
						switch (value[i]) {
						case ',': col++; break;
						case ';': row++; col = 0; break;
						default: player->field[row * game_settings.field_width + col] = value[i]; break;
						}
					}
				}
				else
					fprintf(stderr, "update_player: unknown type: [%s]\n", type);
}

void init_piece(PIECE *piece, char type)
{
	assert(piece != NULL);

	memset(piece, 0, sizeof(PIECE));
	piece->row = game_state.this_piece_position_row;
	piece->col = game_state.this_piece_position_col;
	if (type == 'O')
		piece->col++;

	switch (type) {
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

void pause(char *str) {
	char	line[100];

	if (str != NULL) {
		fprintf(stdout, "%s: ", str);
		fflush(stdout);
	}
	fgets(line, 100, stdin);
}

int is_init_full(char *field) {
	int		i;

	for (i = 3; i <= 4; i++)
		if (field[i] != CELL_EMPTY)
			return 1;

	return 0;
}

int is_end_game(void) {
	if (is_init_full(my_player.field) || is_init_full(opp_player.field))
		return 1;
	else
		return 0;
}

char field_cell_string(char cell) {
	switch (cell) {
	case CELL_EMPTY: return ' ';
	case CELL_SHAPE: return 176;
	case CELL_BLOCK: return 219;
	case CELL_SOLID: return 'X';
	default: return cell;
	}
}

void print_best_moves(PLAYER *player) {
	int		i;
	PIECE	piece;
	
	init_piece(&piece, game_state.this_piece_type);
	piece.current_shape = player->move.shape;
	remove_piece(player->field, &piece, player->move.row, player->move.col);

	for (i = 0; i < player->top.count; i++) {
		piece.current_shape = player->top.moves[i].shape;
		place_piece(player->field, &piece, player->top.moves[i].row, player->top.moves[i].col, CELL_SHAPE);
		print_player(player);
		printf("move: %d score: %f\n", i, player->top.moves[i].score);
		pause("best_move");
		remove_piece(player->field, &piece, player->top.moves[i].row, player->top.moves[i].col);
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

double simulate_game(PLAYER *player, int by_turn, int games, int display) {
	int		npi;
	double	total_games = 0;
	double	total_rounds = 0;
	int		this_removed;
	int		prev_removed;
	double	multiple_lines = 0;
	double	total_points = 0;
	double	total_lines = 0;
	double	results;
	int     removed;

	for (total_games = 0; total_games < games; total_games++) {

		srand((unsigned int)total_games);

		if (display) printf("game: %3.0f...", total_games + 1);
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

		prev_removed = 0;
		while (!is_end_game()) {
			if (display && games == 1)
				printf("game: %.0f round: %3d this_piece: %c next_piece: %c\n", total_games + 1, game_state.round, game_state.this_piece_type, game_state.next_piece_type);

			if (game_state.this_piece_type == 'O')
				game_state.this_piece_position_col = 4;
			else
				game_state.this_piece_position_col = 3;
			game_state.this_piece_position_row = -1;

			do_moves(player, 0, (games > 1));
			if (games == 1) {
				print_player(player);
			}
			if (by_turn) {
				pause("pause");
			}

			this_removed = remove_full_lines(player->field);
			total_lines += this_removed;

			if (this_removed == 1) {
				if (player->move.access_type == MOVE_T_SPIN)
					total_points += 5;
				else
					total_points += 1;
			}
			if (this_removed == 2) {
				total_points += 3;
				if (player->move.access_type == MOVE_T_SPIN)
					total_points += 7;
			}
			if (this_removed == 3)
				total_points += 6;
			if (this_removed == 3)
				total_points += 10;

			prev_removed = this_removed;

			if (game_state.round % 10 == 0) {
				removed = rand() % 5;
				insert_garbage_lines(player->field, removed);
			}

			insert_solid_lines(player->field);

			//printf("round: %d\n", game_state.round);
			//print_player(&my_player);
			//pause("xx");

			npi = rand() % 7;
			assert(npi >= 0 && npi < 7);

			game_state.this_piece_type = game_state.next_piece_type;
			game_state.next_piece_type = SHAPE_TYPES[npi];
			game_state.round++;
		}

		total_rounds += game_state.round;

		if (display)
			printf(" rounds: %3d  avg rounds: %3.2f avg xlines: %3.2f avg points: %3.2f avg lines: %3.2f\n",
				game_state.round,
				total_rounds / (total_games + 1),
				multiple_lines / (total_games + 1),
				total_points / (total_games + 1),
				total_lines / (total_games + 1));

		//print_player(&my_player);
		//pause("play");

	}

	//results = (double)total_rounds / (double)total_games
	//		+ (double)multiple_lines / (double)total_games
	//		+ (double)total_points / (double)total_games
	//		+ (double)lines / (double)total_games;

	results = total_points / total_games;
	//results = (double)lines / (double)total_games;
	//results = (double)total_rounds / (double)total_games;

	if (display)
		printf("games played: %.0f total rounds: %.0f avg rounds: %.2f avg xlines: %.2f avg points: %.2f avg lines: %.2f results: %.4f\n",
			total_games,
			total_rounds,
			total_rounds / total_games,
			multiple_lines / total_games,
			total_points / total_games,
			total_lines / total_games,
			results);


	return results;
}

double play_game(int games, int display) {
	int		npi;
	double	total_games = 0;
	int		my_removed;
	int		opp_removed;
	double	results;
	double	total_wins = 0;
	double	total_loss = 0;
	double	total_draw = 0;

	settings("timebank", "10000");
	settings("time_per_move", "500");
	settings("player_names", "player1,player2");
	settings("your_bot", "player2");
	settings("field_width", "10");
	settings("field_height", "20");

	for (total_games = 0; total_games < games; total_games++) {

		srand((unsigned int)total_games);

		if (display) printf("game: %3.0f...", total_games + 1);
		fflush(stdout);

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

		while (!is_end_game()) {
			if (display && games == 1)
				printf("game: %.0f round: %3d this_piece: %c next_piece: %c\n", total_games + 1, game_state.round, game_state.this_piece_type, game_state.next_piece_type);

			if (game_state.this_piece_type == 'O')
				game_state.this_piece_position_col = 4;
			else
				game_state.this_piece_position_col = 3;
			game_state.this_piece_position_row = -1;

			do_moves(&my_player, 0, (games > 1));
			do_moves(&opp_player, 0, (games > 1));

			game_state.round++;

			my_removed = remove_full_lines(my_player.field);
			opp_removed = remove_full_lines(opp_player.field);

			insert_garbage_lines(my_player.field, opp_removed);
			insert_garbage_lines(opp_player.field, my_removed);

			insert_solid_lines(my_player.field);
			insert_solid_lines(opp_player.field);

			npi = rand() % 7;
			assert(npi >= 0 && npi < 7);

			game_state.this_piece_type = game_state.next_piece_type;
			game_state.next_piece_type = SHAPE_TYPES[npi];
		}

		if (is_init_full(my_player.field) && is_init_full(opp_player.field)) {
			total_draw++;
			if (display) printf(" 1/2 ");
		}
		else {
			if (is_init_full(my_player.field)) {
				total_loss++;
				if (display) printf(" 0-1 ");
			}
			else {
				total_wins++;
				if (display) printf(" 1-0 ");
			}
		}
		if (display) {
			if (total_wins + total_loss)
				printf("wins: %.0f loss: %.0f draw: %.0f results: %f\n", total_wins, total_loss, total_draw,
					(total_wins - total_loss) / sqrt(total_wins + total_loss));
			else
				printf("\n");
		}
		if ((int)total_games % 10 == 0 && total_wins + total_loss > 0)
			printf("%.0f wins: %.0f loss: %.0f draw: %.0f results: %f\r", total_games, total_wins, total_loss, total_draw, (total_wins - total_loss) / sqrt(total_wins + total_loss));
	}

	//(Wins - Losses)/sqrt(Wins + Losses)
	results = (total_wins - total_loss) / sqrt(total_wins + total_loss);
	printf("%.0f wins: %.0f loss: %.0f draw: %.0f results: %f\n", total_games, total_wins, total_loss, total_draw, results);

	if (display)
		printf("\ngames played: %.0f wins: %.0f loss: %.0f results: %f\n", total_games, total_wins, total_loss, results);

	return results;
}

void view(void) {
	FILE    *f = fopen("c:/users/alcides/documents/block/positions.txt", "r");
	char    line[4096];
	int     result;
	int     round;
	char    field[1024];

	settings("timebank", "10000");
	settings("time_per_move", "500");
	settings("player_names", "player1,player2");
	settings("your_bot", "player2");
	settings("field_width", "10");
	settings("field_height", "20");

	while (fgets(line, 4096, f) != NULL) {

		sscanf(line, "%d %d %s", &result, &round, field);

		update("player2", "row_points", "0");
		update("player2", "combo", "0");
		update("player2", "skips", "0");
		update("player2", "field", field);

		print_player(&my_player);
		printf("round: %d  result: %d\n", round, result);
		pause("view");
	}

	fclose(f);
}

/*
void minimize_k() {
double e, k;
for (k = 1; k < 2; k += 0.05) {
e = calc_e(k);
printf("with k = %f, e = %f\n", k, e);
}
}

double calc_e(double k) {
addition = 0;
n = 0;
for each position {
s = quiesce(position);
Sigmoid = 1.0 / (1.0 + pow(10.0, (-k) * (double)s / 400.0));
addition += (game_result - Sigmoid) * (game_result - Sigmoid);
n++;
}
return 1.0 / n * addition;
}
*/

double calc_e(double k) {
	int		result;
	char	round[10];
	char	field[1024];
	char	line[2048];
	double	add = 0;
	double	n = 0;
	double	s = 0;
	double	sigmoid;

	FILE	*ff = fopen("positions.txt", "r");

	settings("timebank", "10000");
	settings("time_per_move", "500");
	settings("player_names", "player1,player2");
	settings("your_bot", "player1");
	settings("field_width", "10");
	settings("field_height", "20");

	while (fgets(line, 2048, ff) != NULL) {
		sscanf(line, "%d %s %s", &result, &round, field);
		//printf("[%d] [%s] [%s]\n", result, round, field);
		update("game", "round", round);
		update("player1", "field", field);
		//print_player(&my_player);
		//printf("evaluate: %f\n", evaluate(my_player.field, 0, 0));

		s = evaluate(&my_player, 0, 0);
		sigmoid = 1.0 / (1.0 + pow(10.0, (-k) * (double)s / 400.0));
		add += (result - sigmoid) * (result - sigmoid);
		n++;

		//printf("%d %f %f %f\n", result, s, sigmoid, add);

		//pause("calc_e");
	}

	fclose(ff);

	return 1.0 / n + add;
}


double get_points(PLAYER *player, double param[PARAM_COUNT]) {
	int     i;

	for (i = 0; i < PARAM_COUNT; i++)
		player->eval.item[i] = param[i];
	return simulate_game(player, 0, 1000, 0);
}

void print_tune_header(int iteration) {
	int     x;

	printf("%3d ", iteration);
	for (x = 0; x < PARAM_COUNT; x++) {
		printf("%6s ", PARAM_LABEL[x]);
	}
	printf("\n");
	printf("    ");
	for (x = 0; x < PARAM_COUNT; x++) {
		printf("------ ");
	}
	printf("\n");
}

void optimize(PLAYER *player, double initial_values[PARAM_COUNT]) {
	double	best_points;
	double	new_points;
	double	best_values[PARAM_COUNT];
	double	new_values[PARAM_COUNT];
	int		improved = 1;
	int		pi;
	int		x;
	int     START_PARAM = 0;
	int     END_PARAM = PARAM_COUNT;
	double  adjust;
	int     iteration = 0;

	best_points = get_points(player, initial_values);

	for (x = 0; x < PARAM_COUNT; x++) best_values[x] = initial_values[x];

	print_tune_header(0);

	printf(" i: ");
	for (x = 0; x < PARAM_COUNT; x++) {
		if (best_values[x] >= 0) printf(" ");
		printf("%02.3f ", best_values[x]);
	}
	printf(": %f\n\n", best_points);

	while (improved) {
		improved = 0;
		iteration++;

		print_tune_header(iteration);

		for (pi = START_PARAM; pi < END_PARAM; pi++) {
			for (x = 0; x < PARAM_COUNT; x++) new_values[x] = best_values[x];
			adjust = new_values[pi] * 0.10;
			new_values[pi] += adjust;
			new_points = get_points(player, new_values);
			if (new_points > best_points) {
				best_points = new_points;
				for (x = 0; x < PARAM_COUNT; x++) best_values[x] = new_values[x];
				improved = 1;
			}
			else {
				adjust *= 2; // 10%
				new_values[pi] -= adjust;
				new_points = get_points(player, new_values);
				if (new_points > best_points) {
					best_points = new_points;
					for (x = 0; x < PARAM_COUNT; x++) best_values[x] = new_values[x];
					improved = 1;
				}
			}

			printf("%2d: ", pi);
			for (x = 0; x < PARAM_COUNT; x++) {
				if (best_values[x] >= 0) printf(" ");
				printf("%2.3f ", best_values[x]);
			}
			printf(": %f\n", best_points);
		}

	}

	for (x = 0; x < PARAM_COUNT; x++) {
		printf("player->eval.item[%d] = %3.6f;\n", x, best_values[x]);
	}
}

void tune(PLAYER *player) {
	int     i;
	double  values[PARAM_COUNT];

	for (i = 0; i < PARAM_COUNT; i++) {
		values[i] = player->eval.item[i];
	}
	printf("tuning...\n");
	optimize(player, values);
	printf("done.\n");

	//printf("tune points: %d\n", play(0, 100, 0));

}

void test(void) {
	char	p[100];
	int     i;
	PIECE   pc;

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
	update("player2", "field", "0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0");

	update("player1", "row_points", "0");
	update("player1", "combo", "0");
	update("player1", "skips", "0");
	update("player1", "field", "0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0");

	while (1) {
		printf("\nthis piece: ");
		fflush(stdout);
		fgets(p, 100, stdin);
		if (p == NULL)
			break;
		game_state.this_piece_type = toupper(p[0]);
		printf("\nnext piece: ");
		fflush(stdout);
		fgets(p, 100, stdin);
		if (p == NULL)
			break;
		game_state.next_piece_type = toupper(p[0]);

		if (game_state.this_piece_type == 'S' && game_state.next_piece_type == 'J') {
			for (i = 0; i < 10; i++) {
				init_piece(&pc, 'S');
				printf("(18, %d) fits: %d access: %d drop: %d move: %d\n", i,
					piece_fits_at(my_player.field, 18, i, &pc),
					get_access_type(my_player.field, 18, i, &pc),
					can_drop(my_player.field, 18, i, &pc),
					can_move(my_player.field, 18, i, &pc)
					);
			}
		}

		do_moves(&my_player, 0, 0);

		//     for (i = 0; i < game_settings.field_height * game_settings.field_width; i++)
		//if (my_player.field[i] != CELL_EMPTY)
		//	my_player.field[i] = CELL_BLOCK;
		print_player(&my_player);
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

//function A*(start,goal)
//    ClosedSet := {}    	  // The set of nodes already evaluated.
//    OpenSet := {start}    // The set of tentative nodes to be evaluated, initially containing the start node
//    Came_From := the empty map    // The map of navigated nodes.
//
//    g_score := map with default value of Infinity
//    g_score[start] := 0    // Cost from start along best known path.
//    // Estimated total cost from start to goal through y.
//    f_score := map with default value of Infinity
//    f_score[start] := heuristic_cost_estimate(start, goal)
//
//    while OpenSet is not empty
//        current := the node in OpenSet having the lowest f_score[] value
//        if current = goal
//            return reconstruct_path(Came_From, goal)
//
//        OpenSet.Remove(current)
//        ClosedSet.Add(current)
//        for each neighbor of current
//            if neighbor in ClosedSet
//                continue		// Ignore the neighbor which is already evaluated.
//            tentative_g_score := g_score[current] + dist_between(current,neighbor) // length of this path.
//            if neighbor not in OpenSet	// Discover a new node
//                OpenSet.Add(neighbor)
//            else if tentative_g_score >= g_score[neighbor]
//                continue		// This is not a better path.
//
//            // This path is the best until now. Record it!
//            Came_From[neighbor] := current
//            g_score[neighbor] := tentative_g_score
//            f_score[neighbor] := g_score[neighbor] + heuristic_cost_estimate(neighbor, goal)
//
//    return failure
//
//function reconstruct_path(Came_From,current)
//    total_path := [current]
//    while current in Came_From.Keys:
//        current := Came_From[current]
//        total_path.append(current)
//    return total_path

// END
