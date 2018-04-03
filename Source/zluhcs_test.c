//-------------------------------------------------------------------------
//  Tests.
//-------------------------------------------------------------------------

#include "zluhcs_defs.h"

double simulate_game(PLAYER *player, int by_turn, int games, int display, int max_rounds)
{
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
    double  new_points = 0;
    PIECE   this_piece;

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
        while (!is_end_game() && game_state.round < max_rounds) {
            if (display && games == 1)
                printf("game: %.0f round: %3d this_piece: %c next_piece: %c\n", total_games + 1, game_state.round, game_state.this_piece_type, game_state.next_piece_type);

            if (game_state.this_piece_type == 'O')
                game_state.this_piece_position_col = 4;
            else
                game_state.this_piece_position_col = 3;
            game_state.this_piece_position_row = -1;

            init_piece(&this_piece, game_state.this_piece_type);

            remove_piece(player->field, &this_piece, game_state.this_piece_position_row, game_state.this_piece_position_col);

            find_best_move(player);

            if (games == 1) {
                print_player(player);
            }
            else {
                if (player->move.shape == T_SPIN_LEFT || player->move.shape == T_SPIN_RIGHT)
                    this_piece.current_shape = 2;
                else
                    this_piece.current_shape = player->move.shape;
                if (player->move.row >= 0)
                    place_piece(player->field, &this_piece, player->move.row, player->move.col);
            }

            if (by_turn)
            {
                pause("pause");
            }

            new_points += get_new_points(player);

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
                insert_garbage_lines(player->field, removed, MOVE_DROP);
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

    //results = (double)lines / (double)total_games;
    //results = (double)total_rounds / (double)total_games;

    //total_points = new_points;

    results = total_points / total_games;

    if (display)
        printf("games: %.0f rounds: %.0f avg rounds: %.2f avg xlines: %.2f avg points: %.2f avg lines: %.2f results: %.4f\n",
            total_games,
            total_rounds,
            total_rounds / total_games,
            multiple_lines / total_games,
            total_points / total_games,
            total_lines / total_games,
            results);

    return results;
}

void insert_garbage_lines(char *field, int opp_removed, int move_access_type)
{
    int		row;
    int		up_row;
    int     lines = 0;
    int     holes;
    int     i;
    int     c;

    assert(field != NULL);
    assert(opp_removed >= 0 && opp_removed <= 4);

    if (opp_removed == 0)
        return;
    if (opp_removed == 1) {
        if (move_access_type == MOVE_T_SPIN)
            lines = 1;
        else
            return;
    }
    if (opp_removed == 2) {
        lines = 1;
        if (move_access_type == MOVE_T_SPIN)
            lines = 3;
    }
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

void fill_row(char *field, int row, char cell_value)
{
    assert(field != NULL);
    assert(row >= 0 && row < game_settings.field_height);
    assert(strchr(CELL_VALID, cell_value) != NULL);

    memset(&field[row * game_settings.field_width], cell_value, game_settings.field_width);
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

char field_cell_string(char cell) 
{
    switch (cell) {
    case CELL_EMPTY: return ' ';
    case CELL_SHAPE: return 176;
    case CELL_BLOCK: return 219;
    case CELL_SOLID: return 'X';
    default: return '?';
    }
}

void print_player(PLAYER *player)
{
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

int is_init_full(char *field, int row) 
{
    int		i;

    for (i = 3; i <= 4; i++)
        if (field[row * game_settings.field_width + i] != CELL_EMPTY)
            return 1;

    return 0;
}

int is_end_game(void)
{
    if (is_init_full(my_player.field, MY_INI_ROW) || is_init_full(opp_player.field, OP_INI_ROW))
        return 1;
    else
        return 0;
}

double get_new_points(PLAYER *player)
{
    int		field_row;
    int		field_col;
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

    double  new_points = 0;

    //T_SPIN  t_spin;

    assert(player != NULL);
    assert(player->field != NULL);
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

    new_points = lines * 1000;

    new_points -= avg_heights;
    new_points -= holes;
    new_points -= bumps;
    new_points -= max_heigth;
    new_points -= blocked_holes * 10;
    new_points -= high_occupancy(player->field);

    return new_points;

}

int remove_full_lines(char *field)
{
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

void copy_row(char *field, int row_from, int row_to) 
{
    assert(field != NULL);
    assert(row_from >= 0 && row_from < game_settings.field_height);
    assert(row_to >= 0 && row_to < game_settings.field_height);
    assert(row_from != row_to);

    memcpy(&field[row_to * game_settings.field_width], &field[row_from * game_settings.field_width], game_settings.field_width);
}

//void test(void)
//{
    //char	p[100];
    //int     i;
    //PIECE   pc;

    //settings("timebank", "10000");
    //settings("time_per_move", "500");
    //settings("player_names", "player1,player2");
    //settings("your_bot", "player2");
    //settings("field_width", "10");
    //settings("field_height", "20");

    //update("game", "round", "1");
    //update("game", "this_piece_type", "I");
    //update("game", "next_piece_type", "J");
    //update("game", "this_piece_position", "3,-1");

    //update("player2", "row_points", "0");
    //update("player2", "combo", "0");
    //update("player2", "skips", "0");
    //update("player2", "field", "0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0");

    //update("player1", "row_points", "0");
    //update("player1", "combo", "0");
    //update("player1", "skips", "0");
    //update("player1", "field", "0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0");

    //while (1) {
    //    printf("\nthis piece: ");
    //    fflush(stdout);
    //    fgets(p, 100, stdin);
    //    if (p == NULL)
    //        break;
    //    game_state.this_piece_type = toupper(p[0]);
    //    printf("\nnext piece: ");
    //    fflush(stdout);
    //    fgets(p, 100, stdin);
    //    if (p == NULL)
    //        break;
    //    game_state.next_piece_type = toupper(p[0]);

    //    if (game_state.this_piece_type == 'S' && game_state.next_piece_type == 'J') {
    //        for (i = 0; i < 10; i++) {
    //            init_piece(&pc, 'S');
    //            printf("(18, %d) fits: %d access: %d drop: %d move: %d\n", i,
    //                piece_fits_at(my_player.field, 18, i, &pc),
    //                get_access_type(my_player.field, 18, i, &pc),
    //                can_drop(my_player.field, 18, i, &pc),
    //                can_move(my_player.field, 18, i, &pc)
    //            );
    //        }
    //    }

    //    do_moves(&my_player, 0, 0);

    //    //     for (i = 0; i < game_settings.field_height * game_settings.field_width; i++)
    //    //if (my_player.field[i] != CELL_EMPTY)
    //    //	my_player.field[i] = CELL_BLOCK;
    //    print_player(&my_player);
    //}
//}

void pause(char *str) 
{
    char	line[100];

    if (str != NULL) {
        fprintf(stdout, "%s: ", str);
        fflush(stdout);
    }
    fgets(line, 100, stdin);
}

#ifdef DEVELOP

//double play_game(int games, int display) {
//    int		next_piece_index;
//    double	total_games = 0;
//    int		my_removed;
//    int		opp_removed;
//    double	results;
//    double	total_wins = 0;
//    double	total_loss = 0;
//    double	total_draw = 0;
//    int     rand_num = 0;
//
//    settings("timebank", "10000");
//    settings("time_per_move", "500");
//    settings("player_names", "player1,player2");
//    settings("your_bot", "player2");
//    settings("field_width", "10");
//    settings("field_height", "20");
//
//    for (total_games = 1; total_games <= games; total_games++) {
//
//        rand_num++;
//        srand((unsigned int)rand_num);
//
//        if (display) printf("game: %3.0f...", total_games + 1);
//        fflush(stdout);
//
//        next_piece_index = rand() % 7;
//        assert(next_piece_index >= 0 && next_piece_index < 7);
//        game_state.this_piece_type = SHAPE_TYPES[next_piece_index];
//        next_piece_index = rand() % 7;
//        assert(next_piece_index >= 0 && next_piece_index < 7);
//        game_state.next_piece_type = SHAPE_TYPES[next_piece_index];
//
//        update("game", "round", "1");
//
//        update("player2", "row_points", "0");
//        update("player2", "combo", "0");
//        update("player2", "skips", "0");
//        update("player2", "field", "0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0");
//
//        update("player1", "row_points", "0");
//        update("player1", "combo", "0");
//        update("player1", "skips", "0");
//        update("player1", "field", "0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0");
//
//        while (!is_end_game()) {
//            if (display && games == 1)
//                printf("game: %.0f round: %3d this_piece: %c next_piece: %c\n", total_games + 1, game_state.round, game_state.this_piece_type, game_state.next_piece_type);
//
//            if (game_state.this_piece_type == 'O')
//                game_state.this_piece_position_col = 4;
//            else
//                game_state.this_piece_position_col = 3;
//            game_state.this_piece_position_row = -1;
//
//            do_moves(&my_player, 0, (games > 1));
//            do_moves(&opp_player, 0, (games > 1));
//
//            game_state.round++;
//
//            my_removed = remove_full_lines(my_player.field);
//            opp_removed = remove_full_lines(opp_player.field);
//
//            insert_garbage_lines(my_player.field, opp_removed, opp_player.move.access_type);
//            insert_garbage_lines(opp_player.field, my_removed, my_player.move.access_type);
//
//            insert_solid_lines(my_player.field);
//            insert_solid_lines(opp_player.field);
//
//            //if (my_removed == 4 || (my_removed == 2 && my_player.move.access_type == MOVE_T_SPIN))
//            //    my_player.skips++;
//            //if (opp_removed == 4 || (opp_removed == 2 && opp_player.move.access_type == MOVE_T_SPIN))
//            //    opp_player.skips++;
//            //if (my_player.move.access_type == MOVE_SKIP)
//            //    my_player.skips--;
//            //if (opp_player.move.access_type == MOVE_SKIP)
//            //    opp_player.skips--;
//
//            next_piece_index = rand() % 7;
//            assert(next_piece_index >= 0 && next_piece_index < 7);
//
//            game_state.this_piece_type = game_state.next_piece_type;
//            game_state.next_piece_type = SHAPE_TYPES[next_piece_index];
//        }
//
//        if (is_init_full(my_player.field, MY_INI_ROW) && is_init_full(opp_player.field, OP_INI_ROW)) {
//            total_draw++;
//            if (display) printf(" 1/2 ");
//        }
//        else {
//            if (is_init_full(my_player.field, MY_INI_ROW)) {
//                total_loss++;
//                if (display) printf(" 0-1 ");
//            }
//            else {
//                total_wins++;
//                if (display) printf(" 1-0 ");
//            }
//        }
//        if (display) {
//            if (total_wins + total_loss)
//                printf("wins: %.0f loss: %.0f draw: %.0f results: %f\n", total_wins, total_loss, total_draw,
//                (total_wins - total_loss) / sqrt(total_wins + total_loss));
//            else
//                printf("\n");
//        }
//        if ((int)total_games % 10 == 0 && total_wins + total_loss > 0)
//            printf("%.0f wins: %.0f loss: %.0f draw: %.0f results: %f\r", total_games, total_wins, total_loss, total_draw, (total_wins - total_loss) / sqrt(total_wins + total_loss));
//    }
//
//    //(Wins - Losses)/sqrt(Wins + Losses)
//    results = (total_wins - total_loss) / sqrt(total_wins + total_loss);
//    //printf("%.0f wins: %.0f loss: %.0f draw: %.0f results: %f\n", total_games, total_wins, total_loss, total_draw, results);
//
//    if (display)
//        printf("\ngames played: %.0f wins: %.0f loss: %.0f results: %f\n", total_games, total_wins, total_loss, results);
//
//    return results;
//}
//
//void view(void) {
//    FILE    *f = fopen("c:/users/alcides/documents/block/positions.txt", "r");
//    char    line[4096];
//    int     result;
//    int     round;
//    char    field[1024];
//
//    settings("timebank", "10000");
//    settings("time_per_move", "500");
//    settings("player_names", "player1,player2");
//    settings("your_bot", "player2");
//    settings("field_width", "10");
//    settings("field_height", "20");
//
//    while (fgets(line, 4096, f) != NULL) {
//
//        sscanf(line, "%d %d %s", &result, &round, field);
//
//        update("player2", "row_points", "0");
//        update("player2", "combo", "0");
//        update("player2", "skips", "0");
//        update("player2", "field", field);
//
//        print_player(&my_player);
//        printf("round: %d  result: %d\n", round, result);
//        pause("view");
//    }
//
//    fclose(f);
//}
//
//double get_points(PLAYER *player, double param[PARAM_COUNT]) {
//    int     i;
//
//    for (i = 0; i < PARAM_COUNT; i++)
//        player->eval.item[i] = param[i];
//    return play_game(1000, 0);
//}
//
//void print_tune_header(int iteration) {
//    int     x;
//
//    printf("%3d ", iteration);
//    for (x = 0; x < PARAM_COUNT; x++) {
//        printf("%6s ", PARAM_LABEL[x]);
//    }
//    printf("\n");
//    printf("    ");
//    for (x = 0; x < PARAM_COUNT; x++) {
//        printf("------ ");
//    }
//    printf("\n");
//}
//
//void optimize(PLAYER *player, double initial_values[PARAM_COUNT]) {
//    double	best_points;
//    double	new_points;
//    double	best_values[PARAM_COUNT];
//    double	new_values[PARAM_COUNT];
//    double  adjustment[PARAM_COUNT];
//    int		improved = 1;
//    int		pi;
//    int		x;
//    int     START_PARAM = 0;
//    int     END_PARAM = PARAM_COUNT;
//    double  adjust;
//    int     iteration = 0;
//
//    best_points = get_points(player, initial_values);
//
//    for (x = 0; x < PARAM_COUNT; x++) best_values[x] = initial_values[x];
//    for (x = 0; x < PARAM_COUNT; x++) adjustment[x] = 0.16384;
//
//    print_tune_header(0);
//
//    printf(" i: ");
//    for (x = 0; x < PARAM_COUNT; x++) {
//        if (best_values[x] >= 0 && best_values[x] < 10) printf(" ");
//        printf("%02.3f ", best_values[x]);
//    }
//    printf(": %f\n\n", best_points);
//
//    while (improved) {
//        improved = 0;
//        iteration++;
//
//        print_tune_header(iteration);
//
//        for (pi = START_PARAM; pi < END_PARAM; pi++) {
//            for (x = 0; x < PARAM_COUNT; x++) new_values[x] = best_values[x];
//            adjust = new_values[pi] * adjustment[pi];
//            new_values[pi] += adjust;
//            new_points = get_points(player, new_values);
//            if (new_points > best_points) {
//                best_points = new_points;
//                for (x = 0; x < PARAM_COUNT; x++) best_values[x] = new_values[x];
//                improved = 1;
//                adjustment[pi] /= 2;
//            }
//            else {
//                adjust *= 2;
//                new_values[pi] -= adjust;
//                new_points = get_points(player, new_values);
//                if (new_points > best_points) {
//                    best_points = new_points;
//                    for (x = 0; x < PARAM_COUNT; x++) best_values[x] = new_values[x];
//                    improved = 1;
//                    adjustment[pi] /= 2;
//                }
//                else
//                    if (adjustment[pi] * 2 < new_values[pi])
//                        adjustment[pi] *= 2;
//            }
//
//            printf("%2d: ", pi);
//            for (x = 0; x < PARAM_COUNT; x++) {
//                if (x <= pi) {
//                    if (best_values[x] >= 0 && best_values[x] < 10) printf(" ");
//                    printf("%2.3f ", best_values[x]);
//                }
//                else {
//                    printf("       ");
//                }
//            }
//            printf(": %f\n", best_points);
//        }
//
//        for (x = 0; x < PARAM_COUNT; x++) {
//            printf("player->eval.param.%s = %3.6f;\n", PARAM_LABEL[x], best_values[x]);
//        }
//    }
//
//}
//
//void tune(PLAYER *player) {
//    int     i;
//    double  values[PARAM_COUNT];
//
//    for (i = 0; i < PARAM_COUNT; i++) {
//        values[i] = player->eval.item[i];
//    }
//    printf("tuning...\n");
//    optimize(player, values);
//    printf("done.\n");
//}
//
//#include <windows.h>
//
//unsigned int get_time(void)
//{
//    FILETIME ft;
//    GetSystemTimeAsFileTime(&ft);
//    return (unsigned int)((((long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime) / 10000);
//}

#endif

// END
