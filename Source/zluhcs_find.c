//-------------------------------------------------------------------------
//  Find move.
//-------------------------------------------------------------------------

#include "zluhcs_defs.h"

void find_best_move(PLAYER *player) 
{
    double		score;
    PIECE		this_piece;
    PIECE		next_piece;
    MOVE_LIST	this_moves;
    MOVE_LIST	next_moves;
    int			filled_lines = 0;
    T_SPIN      t_spin;

    assert(player != NULL);
    assert(player->field != NULL);

    player->move.row = -1;
    player->move.col = -1;
    player->move.access_type = MOVE_NONE;
    player->move.shape = -1;
    player->move.score = -MY_MAXVAL;

    if (game_state.this_piece_type == 'T') {
        if (get_t_spin_data(player->field, &t_spin)) {
            player->move.access_type = MOVE_T_SPIN;
            player->move.shape = t_spin.type;
            player->move.row = t_spin.row;
            player->move.col = t_spin.col;
            return;
        }
    }

    init_piece(&this_piece, game_state.this_piece_type);
    init_piece(&next_piece, game_state.next_piece_type);

    gen_moves(player->field, &this_piece, &this_moves);

    for (int i = 0; i < this_moves.count; i++) {

        this_piece.current_shape = this_moves.moves[i].shape;

        place_piece(player->field, &this_piece, this_moves.moves[i].row, this_moves.moves[i].col);

        filled_lines = fill_lines_count(player->field);

        if (filled_lines * game_settings.field_width == block_count(player)) {
            player->move.score = MY_MAXVAL;
            player->move.access_type = this_moves.moves[i].access_type;
            player->move.row = this_moves.moves[i].row;
            player->move.col = this_moves.moves[i].col;
            player->move.shape = this_moves.moves[i].shape;
            remove_piece(player->field, &this_piece, this_moves.moves[i].row, this_moves.moves[i].col);
            return;
        }

        if (game_state.next_piece_type == 'T') {
            if (get_t_spin_data(player->field, &t_spin)) {
                next_piece.current_shape = 2;
                place_piece(player->field, &next_piece, t_spin.row, t_spin.col);
                score = evaluate(player, filled_lines, this_moves.moves[i].row)
                    + t_spin.lines * player->eval.param.tslin
                    + t_spin.percent_complete * player->eval.param.tspct;
                if (score > player->move.score) {
                    player->move.access_type = this_moves.moves[i].access_type;
                    player->move.score = score;
                    player->move.row = this_moves.moves[i].row;
                    player->move.col = this_moves.moves[i].col;
                    player->move.shape = this_moves.moves[i].shape;
                }
                remove_piece(player->field, &next_piece, t_spin.row, t_spin.col);
            }
        }

        gen_moves(player->field, &next_piece, &next_moves);

        for (int j = 0; j < next_moves.count; j++) {

            next_piece.current_shape = next_moves.moves[j].shape;

            place_piece(player->field, &next_piece, next_moves.moves[j].row, next_moves.moves[j].col);

            score = evaluate(player, filled_lines, this_moves.moves[i].row);

            if (score > player->move.score) {
                player->move.score = score;
                player->move.access_type = this_moves.moves[i].access_type;
                player->move.row = this_moves.moves[i].row;
                player->move.col = this_moves.moves[i].col;
                player->move.shape = this_moves.moves[i].shape;
            }

            remove_piece(player->field, &next_piece, next_moves.moves[j].row, next_moves.moves[j].col);
        }

        remove_piece(player->field, &this_piece, this_moves.moves[i].row, this_moves.moves[i].col);
    }

    if (player->skips) {
        player->move.score += 10.0;
        gen_moves(player->field, &next_piece, &next_moves);
        for (int j = 0; j < next_moves.count; j++) {
            next_piece.current_shape = next_moves.moves[j].shape;
            place_piece(player->field, &next_piece, next_moves.moves[j].row, next_moves.moves[j].col);
            filled_lines = fill_lines_count(player->field);
            score = evaluate(player, filled_lines, next_moves.moves[j].row);
            if (score > player->move.score) {
                player->move.score = score;
                player->move.access_type = MOVE_SKIP;
                player->move.row = 0;
                player->move.col = 0;
                player->move.shape = 0;
            }
            remove_piece(player->field, &next_piece, next_moves.moves[j].row, next_moves.moves[j].col);
        }
    }
}

int can_move(char *field, int row, int col, PIECE *piece)
{
    assert(field != NULL);
    assert(row >= 0 && row < game_settings.field_height);
    assert(col >= -2 && col < game_settings.field_width);
    assert(piece != NULL);
    assert(strchr(SHAPE_TYPES, piece->type) != NULL);

    for (int i = col - 1; i >= -1; i--) {
        if (!can_place(field, row, i, piece)) break;
        if (can_drop(field, row, i, piece)) return 1;
    }
    for (int i = col + 1; i < game_settings.field_width; i++) {
        if (!can_place(field, row, i, piece)) break;
        if (can_drop(field, row, i, piece)) return 1;
    }

    return 0;
}

void gen_moves(char *field, PIECE *piece, MOVE_LIST *move_list) {
    assert(field != NULL);
    assert(piece != NULL);
    assert(move_list != NULL);

    int start_row = get_start_row(field, piece);
    
    move_list->count = 0;

    for (int i = 0; i < piece->max_shapes; i++) {
        piece->current_shape = i;
        for (int row = start_row; row < game_settings.field_height; row++) {
            for (int col = -2; col < game_settings.field_width; col++) {
                if (piece_fits_at(field, row, col, piece)) {
                    int access_type = get_access_type(field, row, col, piece);
                    if (access_type == MOVE_NONE) continue;
                    memset(&move_list->moves[move_list->count], 0, sizeof(MOVE));
                    move_list->moves[move_list->count].access_type = access_type;
                    move_list->moves[move_list->count].shape = piece->current_shape;
                    move_list->moves[move_list->count].score = -MY_MAXVAL;
                    move_list->moves[move_list->count].row = row;
                    move_list->moves[move_list->count].col = col;
                    move_list->count++;
                }
            }
        }
    }
}

int get_start_row(char *field, PIECE *piece)
{
    assert(field != NULL);
    assert(piece != NULL);

    for (int row = 0; row < game_settings.field_height; row++) {
        for (int col = 0; col < game_settings.field_width; col++) {
            if (field[row * game_settings.field_width + col] != CELL_EMPTY) {
                return row - piece->size >= 0 ? row - piece->size : 0;
            }
        }
    }

    return 0;
}

int get_access_type(char *field, int row, int col, PIECE *piece)
{
    assert(field != NULL);
    assert(piece != NULL);

    if (can_drop(field, row, col, piece)) return MOVE_DROP;
    if (can_move(field, row, col, piece)) return MOVE_SEQUENCE;
    return MOVE_NONE;
}

// END
