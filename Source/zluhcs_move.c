//-------------------------------------------------------------------------
//  Execute move.
//-------------------------------------------------------------------------

#include "zluhcs_defs.h"

void action(char *type, char *time) 
{
    assert(type != NULL);
    assert(time != NULL);

    if (!strcmp(type, "move")) {
        do_moves(&my_player);
        return;
    }

    fprintf(stderr, "action: unknown type: [%s] [%s]\n", type, time);
}

void do_moves(PLAYER *player)
{
    PIECE this_piece;
    init_piece(&this_piece, game_state.this_piece_type);
    remove_piece(player->field, &this_piece, game_state.this_piece_position_row, game_state.this_piece_position_col);

    find_best_move(player);

    if (player->move.access_type == MOVE_SEQUENCE) {
        do_move_sequence(player->field, &this_piece, &player->move);
        return;
    }

    if (player->move.access_type == MOVE_SKIP) {
        fprintf(stdout, "SKIP,DROP\n");
        return;
    }

    if (player->move.shape == T_SPIN_LEFT) {
        do_t_spin_move(player, "TURNLEFT");
        return;
    }

    if (player->move.shape == T_SPIN_RIGHT) {
        do_t_spin_move(player, "TURNRIGHT");
        return;
    }

    do_drop_move(player);
}

void do_move_sequence(char *field, PIECE *piece, MOVE *move)
{
    piece->current_shape = move->shape;

    int seq_col = get_sequence_col(field, piece, move);

    for (int i = 0; i < move->shape; i++) fprintf(stdout, "TURNRIGHT,");

    if (seq_col < game_state.this_piece_position_col) {
        for (int i = seq_col; i < game_state.this_piece_position_col; i++) {
            fprintf(stdout, "LEFT,");
        }
    }
    else {
        for (int i = game_state.this_piece_position_col; i < seq_col; i++) {
            fprintf(stdout, "RIGHT,");
        }
    }

    for (int i = game_state.this_piece_position_row; i < move->row; i++) {
        fprintf(stdout, "DOWN,");
    }

    if (seq_col < move->col) {
        for (int i = seq_col; i < move->col; i++) {
            fprintf(stdout, "RIGHT,");
        }
    }
    else {
        for (int i = move->col; i < seq_col; i++) {
            fprintf(stdout, "LEFT,");
        }
    }

    fprintf(stdout, "DROP\n");
}

int get_sequence_col(char *field, PIECE *piece, MOVE *move)
{
    for (int seq_col = move->col - 1; seq_col >= -1; seq_col--) {
        if (!can_place(field, move->row, seq_col, piece)) break;
        if (can_drop(field, move->row, seq_col, piece)) return seq_col;
    }
    for (int seq_col = move->col + 1; seq_col < game_settings.field_width; seq_col++) {
        if (!can_place(field, move->row, seq_col, piece)) break;
        if (can_drop(field, move->row, seq_col, piece)) return seq_col;
    }
    return 0;
}

void do_t_spin_move(PLAYER *player, char *turn)
{
    assert(game_state.this_piece_type == 'T');
    assert(player->move.shape == T_SPIN_LEFT || player->move.shape == T_SPIN_RIGHT);

    fprintf(stdout, "%s,", turn);

    if (player->move.col < game_state.this_piece_position_col) {
        for (int i = player->move.col; i < game_state.this_piece_position_col; i++) {
            fprintf(stdout, "LEFT,");
        }
    }
    else {
        for (int i = game_state.this_piece_position_col; i < player->move.col; i++) {
            fprintf(stdout, "RIGHT,");
        }
    }

    for (int i = game_state.this_piece_position_row; i < player->move.row; i++) {
        fprintf(stdout, "DOWN,");
    }

    fprintf(stdout, "%s,DROP\n", turn);
}

void do_drop_move(PLAYER *player)
{
    assert(player != NULL);

    for (int i = 0; i < player->move.shape; i++)
        fprintf(stdout, "TURNRIGHT,");

    if (player->move.col < game_state.this_piece_position_col) {
        for (int i = player->move.col; i < game_state.this_piece_position_col; i++) {
            fprintf(stdout, "LEFT,");
        }
    }
    else {
        for (int i = game_state.this_piece_position_col; i < player->move.col; i++) {
            fprintf(stdout, "RIGHT,");
        }
    }

    fprintf(stdout, "DROP\n");
}

void place_piece(char *field, PIECE *piece, int row, int col)
{
    for (int pc_row = 0; pc_row < piece->size; pc_row++) {
        for (int pc_col = 0; pc_col < piece->size; pc_col++) {
            if (piece->shapes[piece->current_shape][pc_row * piece->size + pc_col] == CELL_EMPTY) {
                continue;
            }
            int bk_row = row + pc_row;
            int bk_col = col + pc_col;
            if (bk_row < 0 || bk_row >= game_settings.field_height) continue;
            if (bk_col < 0 || bk_col >= game_settings.field_width)  continue;
            field[bk_row * game_settings.field_width + bk_col] = CELL_SHAPE;
        }
    }
}

// END
