//-------------------------------------------------------------------------
//  Definitions.
//-------------------------------------------------------------------------

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

#define MY_INI_ROW  4
#define OP_INI_ROW  2

typedef struct s_game_settings
{
    int     timebank;
    int     time_per_move;
    char    player_names[512];
    char    my_bot_name[512];
    int     field_height;
    int     field_width;
}   GAME_SETTINGS;

typedef struct s_game_state
{
    int     round;
    int     this_piece_position_row;
    int     this_piece_position_col;
    char    this_piece_type;
    char    next_piece_type;
}   GAME_STATE;

#define PARAM_COUNT 18

typedef struct s_list_param
{
    double	heigh;
    double	lines;
    double	holes;
    double	bumps;
    double	mult1;
    double	mult2;
    double	empty;
    double  shole;
    double  maxht;
    double  lorow;
    double  block;
    double  tslin;
    double  tspct;
    double  tsbon;
    double  tsrev;
    double  tspre;
    double  occup;
    double  avrow;
}	LIST_PARAM;

typedef union u_eval_param
{
    LIST_PARAM  param;
    double      item[PARAM_COUNT];
}   EVAL_PARAM;

typedef struct s_piece
{
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
#define MOVE_SKIP       4

#define T_SLOT_ROW      3
#define T_SLOT_COL      3
#define T_SPIN_NONE     10
#define T_SPIN_LEFT     11
#define T_SPIN_RIGHT    12
#define T_SPIN_INIT     13

#define T_SLOT_LEFT     "001000101"
#define T_SLOT_RIGHT    "100000101"
#define T_SLOT_INIT     "000000101"

#define SHAPE_TYPES     "IZLJOTS"

typedef struct s_move
{
    double	score;
    int     access_type;
    int		shape;
    int		row;
    int		col;
}	MOVE;

typedef struct s_move_list
{
    int		count;
    MOVE	moves[100];
}	MOVE_LIST;

typedef struct s_t_spin
{
    int     row;
    int     col;
    int     type;
    int     lines;
    double  percent_complete;
}   T_SPIN;

typedef struct s_player
{
    int			row_points;
    int			combo;
    int			skips;
    char		*field;
    EVAL_PARAM	eval;
    MOVE        move;
}   PLAYER;

void    action(char *action, char *time);
void    do_moves(PLAYER *player);
int     piece_fits_at(char *field, int row, int col, PIECE *piece);
void    place_piece(char *field, PIECE *piece, int row, int col);
void    remove_piece(char *field, PIECE *piece, int row, int col);
void    find_best_move(PLAYER *player);
void    insert_solid_lines(char *field);
void    remove_piece(char *field, PIECE *piece, int row, int col);
int     get_t_spin_data(char *field, T_SPIN *t_spin);
int     can_drop(char *field, int row, int col, PIECE *piece);
int     can_place(char *field, int target_row, int target_col, PIECE *piece);
void    rotate_right(PIECE *piece);
void    rotate_left(PIECE *piece);
void    print_piece(PIECE *piece);
double  evaluate(PLAYER *player, int prev_removed, int prev_row);
int     fill_lines_count(char *field);
int     block_count(PLAYER *player);
int     is_blocked(char *field, int row, int col);
int     count_block_holes(char *field, int row, int col);
int     get_t_spin_init(char *field, T_SPIN *t_spin);
int     is_full(char *field, int row);
int     t_slot_match(char *field, char *t_slot, int field_row, int field_col);
int     is_filled(char *field, int row, int from_col, int to_col);
int     t_slot_drop(char * field, int row, int col);
double  calc_t_spin_complete(char *field, int row1, int row2);
void    init_player(PLAYER *player);
void    init_piece(PIECE *piece, char type);
void    gen_moves(char *field, PIECE *piece, MOVE_LIST *move_list);
int     get_start_row(char *field, PIECE *piece);
int     get_access_type(char *field, int row, int col, PIECE *piece);
int     can_move(char *field, int row, int col, PIECE *piece);
void    init_player(PLAYER *player);
void    settings(char *type, char *value);
void    update(char *player, char *type, char *value);
void    update_player(PLAYER *player, char *type, char *value);
double  high_occupancy(char *field);
int     remove_full_lines(char *field);
int     is_end_game(void);
void    print_player(PLAYER *player);
double  get_new_points(PLAYER *player);
void    copy_row(char *field, int row_from, int row_to);
void    insert_garbage_lines(char *field, int opp_removed, int move_access_type);
void    fill_row(char *field, int row, char cell_value);
void    settings(char *type, char *value);
void    update(char *player, char *type, char *value);
void    update_player(PLAYER *player, char *type, char *value);
void    do_move_sequence(char *field, PIECE *piece, MOVE *move);
int     get_sequence_col(char *field, PIECE *piece, MOVE *move);
void    do_t_spin_move(PLAYER *player, char * turn);
void    do_drop_move(PLAYER *player);
void    pause(char *str);

#ifdef DEVELOP
void    tune(PLAYER *player);
double  simulate_game(PLAYER *player, int by_turn, int games, int display, int max_rounds);
double  play_game(int games, int display);
void    view(void);
unsigned int get_time(void);
#endif

extern GAME_SETTINGS   game_settings;
extern GAME_STATE      game_state;
extern PLAYER          my_player;
extern PLAYER          opp_player;

// END