//-------------------------------------------------------------------------
// TheAiGames's block start bot by Alcides Schulz (zluchs).
// alcides_schulz@hotmail.com
// Drop piece at lowest available position in the field.
// Implement your logic at do_moves() method.
//-------------------------------------------------------------------------

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define CELL_EMPTY  '0'
#define CELL_SHAPE  '1'
#define CELL_BLOCK  '2'
#define CELL_SOLID  '3'

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
 
void settings(char *type, char *value);
void update(char *player, char *type, char *value);
void update_player(PLAYER *player, char *type, char *value);
void action(char *action, char *time);
void do_moves(int time);
void find_lowest_row(char *field, PIECE *piece, int *lowest_row, int *lowest_col);
void set_piece(char *field, PIECE *piece, int row, int col);

void init_piece(PIECE *piece, char type);
void rotate_right(PIECE *piece);
void rotate_left(PIECE *piece);
void print_piece(PIECE *piece);

void print_player(struct s_player *player);

int main(void) {
    char    line[16384];
    char    part[3][1024];

    memset(&game_settings, 0, sizeof(GAME_SETTINGS));
    memset(&game_state, 0, sizeof(GAME_STATE));
    memset(&my_player, 0, sizeof(PLAYER));
    memset(&opp_player, 0, sizeof(PLAYER));

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
    }

    return 0;
}

void action(char *type, char *time) {
    assert(type != NULL);
    assert(time != NULL);
    
    if (!strcmp(type, "moves"))
        do_moves(atoi(time));
    else
        fprintf(stderr, "action: unknown type: [%s]\n", type);
}

void do_moves(int time) {
    PIECE   this_piece;
    PIECE   next_piece;
    int     lowest_row;
    int     lowest_col;
    int     i;
    
    init_piece(&this_piece, game_state.this_piece_type);
    init_piece(&next_piece, game_state.next_piece_type);

    find_lowest_row(my_player.field, &this_piece, &lowest_row, &lowest_col);
	
    if (lowest_col < game_state.this_piece_position_col) {
		for (i = lowest_col; i < game_state.this_piece_position_col; i++) {
            fprintf(stdout, "LEFT,");
		}
    }
    else {
		for (i = game_state.this_piece_position_col; i < lowest_col; i++) {
            fprintf(stdout, "RIGHT,");
		}
    }
    
    fprintf(stdout, "DROP\n");
}

void set_piece(char *field, PIECE *piece, int row, int col) {
    int     prow;
    int     pcol;
    
    for (prow = 0; prow < piece->size; prow++) {
        for (pcol = 0; pcol < piece->size; pcol++) {
            if (piece->shapes[piece->current_shape][prow * piece->size + pcol] == CELL_EMPTY)
                continue;
            field[(row + prow) * game_settings.field_width + (col + pcol)] = CELL_SHAPE;
        }
    }
}

int can_drop(char *field, int row, int col, PIECE *piece) {
    int     prow;
    int     pcol;

	int		i;

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
    int     prow;
    int     pcol;
    
    for (prow = 0; prow < piece->size; prow++) {
        for (pcol = 0; pcol < piece->size; pcol++) {
            if (piece->shapes[piece->current_shape][prow * piece->size + pcol] == CELL_EMPTY)
                continue;
            if (row + prow >= game_settings.field_height)
                return 0;
            if (col + pcol >= game_settings.field_width)
                return 0;
            if (field[(row + prow) * game_settings.field_width + (col + pcol)] != CELL_EMPTY)
                return 0;
        }
    }

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
    case CELL_SHAPE: return '*';
    case CELL_BLOCK: return '+';
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
        for (col = 0; col < game_settings.field_width; col++)
            fprintf(stderr, "+---");
        fprintf(stderr, "+\n");

        for (col = 0; col < game_settings.field_width; col++) {
            if (col == 0)
                fprintf(stderr, "%2d |", row);
            fprintf(stderr, " %c |", field_cell_string(player->field[row * game_settings.field_width + col]));
        }

        fprintf(stderr, " %2d\n", row);
    }

    fprintf(stderr, "   ");
    for (col = 0; col < game_settings.field_width; col++)
        fprintf(stderr, "+---");
    fprintf(stderr, "+\n");
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

// END 
