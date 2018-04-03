//-------------------------------------------------------------------------
//  Evaluation.
//-------------------------------------------------------------------------

#include "zluhcs_defs.h"

double evaluate(PLAYER *player, int prev_removed, int prev_row)
{

    assert(player != NULL);
    assert(player->field != NULL);
    assert(prev_removed >= 0 && prev_removed <= 4);
    assert(game_settings.field_width == 10);

    int row_first_solid = game_settings.field_height;
    for (int field_row = 0; field_row < game_settings.field_height; field_row++) {
        if (player->field[field_row * game_settings.field_width] == CELL_SOLID) {
            row_first_solid = field_row;
            break;
        }
    }

    double side_holes = 0;
    for (int field_row = 0; field_row < game_settings.field_height; field_row++) {
        if (player->field[field_row * game_settings.field_width + 0] == CELL_EMPTY) {
            side_holes -= 0.4;
        }
        if (player->field[field_row * game_settings.field_width + game_settings.field_width - 1] == CELL_EMPTY) {
            side_holes -= 0.4;
        }
        for (int field_col = 1; field_col < game_settings.field_width - 1; field_col++) {
            if (player->field[field_row * game_settings.field_width + field_col] == CELL_EMPTY) {
                side_holes += 0.1;
            }
        }
    }

    int blocked_holes = 0;
    int col_heights[10];
    int	heights = 0;
    int	holes = 0;

    for (int field_col = 0; field_col < game_settings.field_width; field_col++) {
        col_heights[field_col] = 0;
        for (int field_row = 0; field_row < game_settings.field_height; field_row++) {
            if (player->field[field_row * game_settings.field_width + field_col] == CELL_EMPTY) {
                continue;
            }
            for (int holes_row = field_row + 1; holes_row < game_settings.field_height; holes_row++) {
                if (player->field[holes_row * game_settings.field_width + field_col] == CELL_EMPTY) {
                    if (is_blocked(player->field, holes_row, field_col)) {
                        holes++;
                        blocked_holes += count_block_holes(player->field, holes_row, field_col);
                    }
                }
            }
            int this_height = game_settings.field_height - field_row;
            col_heights[field_col] = this_height;
            heights += this_height;
            break;
        }
    }

    int max_heigth = 0;
    int sum_heights = 0;
    for (int field_col = 0; field_col < game_settings.field_width; field_col++) {
        sum_heights += col_heights[field_col];
        if (col_heights[field_col] > max_heigth) {
            max_heigth = col_heights[field_col];
        }
    }
    int avg_heights = sum_heights / game_settings.field_width;
    int empty_holes = 0;
    for (int field_col = 0; field_col < game_settings.field_width; field_col++) {
        if (col_heights[field_col] == 0) empty_holes += avg_heights;
    }

    int bumps = 0;
    for (int field_col = 0; field_col < game_settings.field_width; field_col++) {
        if (field_col > 0) bumps += abs(col_heights[field_col] - col_heights[field_col - 1]);
    }

    int lines = fill_lines_count(player->field);
    if (lines * game_settings.field_width == block_count(player)) return MY_MAXVAL;

    double final_score = 0;

    final_score += player->eval.param.heigh * heights;
    final_score += player->eval.param.lines * lines;
    final_score += player->eval.param.holes * holes;
    final_score += player->eval.param.bumps * bumps;
    final_score += player->eval.param.shole * side_holes;
    final_score += player->eval.param.empty * empty_holes;
    final_score += player->eval.param.maxht * max_heigth;
    final_score += player->eval.param.lorow * prev_row;
    final_score += player->eval.param.block * blocked_holes;
    if (prev_removed > 1) {
        final_score += prev_removed * player->eval.param.mult1;
    }
    if (lines - prev_removed > 1 && prev_removed == 0) {
        final_score += (lines - prev_removed) * player->eval.param.mult2;
    }

    final_score += high_occupancy(player->field) * player->eval.param.occup;

    if (avg_heights < player->eval.param.avrow) {
        T_SPIN  t_spin;
        if (get_t_spin_data(player->field, &t_spin)) {
            if (t_spin.row >= 15 && t_spin.percent_complete == 1.0) {
                final_score += player->eval.param.tsbon * t_spin.percent_complete;
                final_score += (player->eval.param.tsrev * lines);
                final_score += (((holes > 0 ? holes - 1 : holes)) * player->eval.param.holes);

            }
            else {
                final_score += (player->eval.param.tspre * t_spin.percent_complete);
                final_score += (holes * player->eval.param.holes);
                final_score += (player->eval.param.tsrev * lines);
                final_score += (t_spin.row - 16);
            }
        }
        else {
            if (holes <= 0 && get_t_spin_init(player->field, &t_spin) && t_spin.row >= 15) {
                final_score += (player->eval.param.tspre * t_spin.percent_complete);
                final_score += (holes * player->eval.param.holes);
                final_score += (player->eval.param.tsrev * lines);
            }
        }
    }

    return final_score;
}

double high_occupancy(char *field)
{
    double	penalty[10] = { 1.0, 0.9, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1, 0.05 };
    double	value = 0;
    for (int row = 0; row < 10; row++) {
        for (int col = 0; col < game_settings.field_width; col++) {
            if (field[row * game_settings.field_width + col] != CELL_EMPTY)
                value -= penalty[row];
        }
    }
    return value;
}

// END

