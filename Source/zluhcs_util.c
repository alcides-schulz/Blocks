//-------------------------------------------------------------------------
//  Find best move.
//-------------------------------------------------------------------------

#include "zluhcs_defs.h"

int fill_lines_count(char *field)
{
    int lines = 0;
    for (int field_row = 0; field_row < game_settings.field_height; field_row++) {
        if (field[field_row * game_settings.field_width] == CELL_SOLID) continue;
        int fill = 0;
        for (int field_col = 0; field_col < game_settings.field_width; field_col++) {
            if (field[field_row * game_settings.field_width + field_col] == CELL_EMPTY) break;
            fill++;
        }
        if (fill == game_settings.field_width) lines += 1;
    }
    return lines;
}

int block_count(PLAYER *player)
{
    int count = 0;
    for (int i = 0; i < game_settings.field_height * game_settings.field_width; i++) {
        if (player->field[i] == CELL_SOLID) break;
        if (player->field[i] != CELL_EMPTY) count++;
    }
    return count;
}

int is_blocked(char *field, int row, int col)
{
    while (--row >= 0) {
        if (field[row * game_settings.field_width + col] == CELL_EMPTY) continue;
        if (!is_full(field, row)) return 1;
    }
    return 0;
}

int count_block_holes(char *field, int row, int col)
{
    int count = 0;
    while (--row >= 0) {
        if (is_full(field, row)) continue;
        if (field[row * game_settings.field_width + col] == CELL_EMPTY) continue;
        count++;
    }
    return count;
}

int get_t_spin_init(char *field, T_SPIN *t_spin)
{
    assert(field != NULL);
    assert(t_spin != NULL);

    t_spin->row = -1;
    t_spin->col = -1;
    t_spin->type = T_SPIN_NONE;
    t_spin->lines = 0;

    for (int field_row = 0; field_row <= game_settings.field_height - T_SLOT_ROW; field_row++) {
        for (int field_col = 0; field_col <= game_settings.field_width - T_SLOT_COL; field_col++) {
            if (t_slot_match(field, T_SLOT_INIT, field_row, field_col)) {

                int row1_filled = is_filled(field, field_row + 1, 0, field_col) && is_filled(field, field_row + 1, field_col + T_SLOT_COL, game_settings.field_width);
                int row2_filled = is_filled(field, field_row + 2, 0, field_col) && is_filled(field, field_row + 2, field_col + T_SLOT_COL, game_settings.field_width);

                if (row2_filled && (t_slot_drop(field, field_row, field_col) || t_slot_drop(field, field_row, field_col + 1))) {
                    t_spin->row = field_row;
                    t_spin->col = field_col;
                    t_spin->type = T_SPIN_INIT;
                    t_spin->lines = row1_filled + row2_filled;
                    t_spin->percent_complete = calc_t_spin_complete(field, field_row + 1, field_row + 2);
                }
            }
        }
    }

    return (t_spin->type != T_SPIN_NONE ? 1 : 0);
}

int is_full(char *field, int row)
{
    for (int col = 0; col < game_settings.field_width; col++) {
        if (field[row * game_settings.field_width + col] == CELL_EMPTY) return 0;
    }
    return 1;
}

int t_slot_match(char *field, char *t_slot, int field_row, int field_col)
{
    assert(field != NULL);
    assert(field_row >= 0 && field_row < game_settings.field_height);
    assert(field_col >= 0 && field_col < game_settings.field_width);

    for (int slot_row = 0; slot_row < T_SLOT_ROW; slot_row++) {
        for (int slot_col = 0; slot_col < T_SLOT_COL; slot_col++) {
            if (field_row + slot_row < 0 || field_row + slot_row >= game_settings.field_height) return 0;
            if (field_col + slot_col < 0 || field_col + slot_col >= game_settings.field_width)  return 0;
            char slot_cell = t_slot[slot_row * T_SLOT_COL + slot_col];
            if (slot_cell == '?') continue;
            char field_cell = field[(field_row + slot_row) * game_settings.field_width + (field_col + slot_col)];
            if (field_cell == CELL_SOLID) return 0;
            if (slot_cell == CELL_EMPTY && field_cell == CELL_EMPTY) continue;
            if (slot_cell != CELL_EMPTY && field_cell != CELL_EMPTY) continue;
            return 0;
        }
    }

    return 1;
}

int is_filled(char *field, int row, int from_col, int to_col)
{
    assert(field != NULL);
    assert(row >= 0 && row < game_settings.field_height);
    assert(from_col >= 0 && from_col <= game_settings.field_width);
    assert(to_col >= 0 && to_col <= game_settings.field_width);

    for (int col = from_col; col < to_col; col++) {
        if (field[row * game_settings.field_width + col] == CELL_EMPTY) return 0;
    }

    return 1;
}

int t_slot_drop(char * field, int row, int col)
{
    assert(field != NULL);
    assert(row >= 0 && row < game_settings.field_height);
    assert(col >= 0 && col < game_settings.field_width);

    for (int r = row - 1; r >= 0; r--) {
        for (int c = col; c < col + 2; c++) {
            if (field[r * game_settings.field_width + c] != CELL_EMPTY) {
                if (!is_filled(field, r, 0, game_settings.field_width)) return 0;
            }
        }
    }
    return 1;
}

//        XXX
// XXXXX   XX 7
// XXXXXX XXX 9
double calc_t_spin_complete(char *field, int row1, int row2)
{
    double blocks = 0;

    for (int col = 0; col < game_settings.field_width; col++) {
        if (field[row1 * game_settings.field_width + col] != CELL_EMPTY) blocks++;
        if (field[row2 * game_settings.field_width + col] != CELL_EMPTY) blocks++;
    }

    return blocks / 16;
}

void remove_piece(char *field, PIECE *piece, int row, int col)
{
    for (int pc_row = 0; pc_row < piece->size; pc_row++) {
        for (int pc_col = 0; pc_col < piece->size; pc_col++) {
            if (piece->shapes[piece->current_shape][pc_row * piece->size + pc_col] == CELL_EMPTY) continue;
            int bk_row = row + pc_row;
            int bk_col = col + pc_col;
            if (bk_row < 0 || bk_row >= game_settings.field_height) continue;
            if (bk_col < 0 || bk_col >= game_settings.field_width)  continue;
            field[bk_row * game_settings.field_width + bk_col] = CELL_EMPTY;
        }
    }
}

int get_t_spin_data(char *field, T_SPIN *t_spin)
{
    assert(field != NULL);
    assert(t_spin != NULL);

    t_spin->row = -1;
    t_spin->col = -1;
    t_spin->type = T_SPIN_NONE;
    t_spin->lines = 0;
    t_spin->percent_complete = -1;

    for (int field_row = 0; field_row <= game_settings.field_height - T_SLOT_ROW; field_row++) {
        for (int field_col = 0; field_col <= game_settings.field_width - T_SLOT_COL; field_col++) {
            if (t_slot_match(field, T_SLOT_LEFT, field_row, field_col)) {
                int row1_filled = is_filled(field, field_row + 1, 0, field_col) && is_filled(field, field_row + 1, field_col + T_SLOT_COL, game_settings.field_width);
                int row2_filled = is_filled(field, field_row + 2, 0, field_col) && is_filled(field, field_row + 2, field_col + T_SLOT_COL, game_settings.field_width);
                double percent = calc_t_spin_complete(field, field_row + 1, field_row + 2);
                if (row2_filled && percent >= t_spin->percent_complete) {
                    if (t_slot_drop(field, field_row, field_col)) {
                        t_spin->row = field_row;
                        t_spin->col = field_col;
                        t_spin->type = T_SPIN_LEFT;
                        t_spin->lines = (row1_filled && row2_filled) ? 2 : 1;
                        t_spin->percent_complete = percent;
                    }
                }
            }
            if (t_slot_match(field, T_SLOT_RIGHT, field_row, field_col)) {
                int row1_filled = is_filled(field, field_row + 1, 0, field_col) && is_filled(field, field_row + 1, field_col + T_SLOT_COL, game_settings.field_width);
                int row2_filled = is_filled(field, field_row + 2, 0, field_col) && is_filled(field, field_row + 2, field_col + T_SLOT_COL, game_settings.field_width);
                double percent = calc_t_spin_complete(field, field_row + 1, field_row + 2);
                if (row2_filled && percent >= t_spin->percent_complete) {
                    if (t_slot_drop(field, field_row, field_col + 1)) {
                        t_spin->row = field_row;
                        t_spin->col = field_col;
                        t_spin->type = T_SPIN_RIGHT;
                        t_spin->lines = (row1_filled && row2_filled) ? 2 : 1;
                        t_spin->percent_complete = percent;
                    }
                }
            }
        }
    }

    if (t_spin->type != T_SPIN_NONE) return 1;

    return 0;
}

int can_drop(char *field, int row, int col, PIECE *piece) {
    assert(field != NULL);
    assert(row >= -1 && row < game_settings.field_height);
    assert(col >= -2 && col < game_settings.field_width);
    assert(piece != NULL);
    assert(strchr(SHAPE_TYPES, piece->type) != NULL);

    for (int prow = 0; prow < piece->size; prow++) {
        for (int pcol = 0; pcol < piece->size; pcol++) {
            if (piece->shapes[piece->current_shape][prow * piece->size + pcol] == CELL_EMPTY) continue;
            for (int i = row + prow; i > 0; i--) {
                if (i < -1 || i >= game_settings.field_height) return 0;
                if (col + pcol < 0 || col + pcol >= game_settings.field_width) return 0;
                if (field[i * game_settings.field_width + (col + pcol)] != CELL_EMPTY) return 0;
            }
        }
    }
    return 1;
}

int can_place(char *field, int target_row, int target_col, PIECE *piece) {
    assert(field != NULL);
    assert(target_row >= -1 && target_row < game_settings.field_height);
    assert(target_col >= -2 && target_col < game_settings.field_width);
    assert(piece != NULL);
    assert(strchr(SHAPE_TYPES, piece->type) != NULL);

    for (int piece_row = 0; piece_row < piece->size; piece_row++) {
        for (int piece_col = 0; piece_col < piece->size; piece_col++) {
            if (piece->shapes[piece->current_shape][piece_row * piece->size + piece_col] == CELL_EMPTY) continue;
            int block_row = target_row + piece_row;
            int block_col = target_col + piece_col;
            if (block_row < 0) continue;
            if (block_row >= game_settings.field_height) return 0;
            if (block_col < 0 || block_col >= game_settings.field_width) return 0;
            if (field[block_row * game_settings.field_width + block_col] != CELL_EMPTY) return 0;
        }
    }

    return 1;
}

int piece_fits_at(char *field, int row, int col, PIECE *piece)
{
    assert(field != NULL);
    assert(row >= -1 && row < game_settings.field_height);
    assert(col >= -2 && col < game_settings.field_width);
    assert(piece != NULL);
    assert(strchr(SHAPE_TYPES, piece->type) != NULL);

    int	empty = 0;

    for (int pc_row = 0; pc_row < piece->size; pc_row++) {
        for (int pc_col = 0; pc_col < piece->size; pc_col++) {
            if (piece->shapes[piece->current_shape][pc_row * piece->size + pc_col] == CELL_EMPTY) continue;

            int bk_row = row + pc_row;
            int bk_col = col + pc_col;

            if (bk_row < 0) continue;
            if (bk_row >= game_settings.field_height) return 0;
            if (bk_col < 0 || bk_col >= game_settings.field_width) return 0;
            if (field[bk_row * game_settings.field_width + bk_col] != CELL_EMPTY) return 0;
            if (bk_row + 1 < game_settings.field_height && field[(bk_row + 1) * game_settings.field_width + bk_col] == CELL_EMPTY) empty++;
        }

    }

    if (empty == 4) return 0;

    return 1;
}

int holes_count(char *field, PIECE *piece, int row, int col)
{
    assert(field != NULL);
    assert(piece != NULL);
    assert(row >= 0 && row < game_settings.field_height);
    assert(col >= -1 && col < game_settings.field_width);

    int holes = 0;

    for (int pc_row = 0; pc_row < piece->size; pc_row++) {
        for (int pc_col = 0; pc_col < piece->size; pc_col++) {
            if (piece->shapes[piece->current_shape][pc_row * piece->size + pc_col] == CELL_EMPTY) continue;
            int down_row = row + pc_row + 1;
            int down_col = col + pc_col;
            while (down_row < row + pc_row + 2 && down_row < game_settings.field_height) {
                if (field[down_row * game_settings.field_width + down_col] == CELL_EMPTY) holes += down_row;
                down_row++;
            }
        }
    }

    return holes;
}

int is_holes_blocked(char *field, int row, int exc_col)
{
    for (int col = 0; col < game_settings.field_width; col++) {
        if (col == exc_col) continue;
        if (field[row * game_settings.field_width + col] == CELL_EMPTY) {
            if (field[(row - 1) * game_settings.field_width + col] != CELL_EMPTY) {
                return 1;
            }
        }
    }
    return 0;
}

int almost_full_count(char *field)
{
    int lines_count = 0;

    for (int row = 0; row < game_settings.field_height; row++) {
        if (is_full(field, row)) continue;
        int block_count = 0;
        for (int col = 0; col < game_settings.field_width; col++) {
            if (field[row * game_settings.field_width + col] != CELL_EMPTY) block_count++;
        }
        if (block_count >= game_settings.field_width - 2) lines_count++;
    }

    return lines_count;
}

int get_bottom_row(char *field)
{
    if (field[(game_settings.field_height - 1) * game_settings.field_width] != CELL_SOLID) {
        return game_settings.field_height - 1;
    }

    for (int row = game_settings.field_height - 1; row >= 0; row--) {
        if (field[row * game_settings.field_width] == CELL_SOLID) continue;
        return row;
    }

    return 0;
}

int get_highest_column(char *field, int bottom_row)
{
    for (int row = 0; row < bottom_row; row++) {
        for (int col = 0; col < game_settings.field_width; col++) {
            if (field[row * game_settings.field_width + col] != CELL_EMPTY) return bottom_row - row;
        }
    }
    return 0;
}

void rotate_right(PIECE *piece)
{
    assert(piece != NULL);
    if (piece->current_shape == piece->max_shapes - 1)
        piece->current_shape = 0;
    else
        piece->current_shape++;
}

void rotate_left(PIECE *piece)
{
    assert(piece != NULL);
    if (piece->current_shape == 0)
        piece->current_shape = piece->max_shapes - 1;
    else
        piece->current_shape--;
}

void print_piece(PIECE *piece)
{
    assert(piece != NULL);

    for (int row = 0; row < piece->size; row++) {
        for (int col = 0; col < piece->size; col++) fprintf(stderr, "+-");
        fprintf(stderr, "+\n");
        fprintf(stderr, "|");

        for (int col = 0; col < piece->size; col++) {
            fprintf(stderr, "%c|", (piece->shapes[piece->current_shape][row * piece->size + col] == '1' ? CELL_BLOCK : CELL_EMPTY));
        }
        fprintf(stderr, "\n");
    }

    for (int col = 0; col < piece->size; col++) fprintf(stderr, "+-");

    fprintf(stderr, "+\n\n");
}

// END
