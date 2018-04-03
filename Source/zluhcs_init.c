//-------------------------------------------------------------------------
//  Initializations.
//-------------------------------------------------------------------------

#include "zluhcs_defs.h"

void init_player(PLAYER *player)
{
    memset(player, 0, sizeof(PLAYER));
    player->eval.param.heigh = -1.465027;
    player->eval.param.lines = -2.000000;
    player->eval.param.holes = -2.563045;
    player->eval.param.bumps = -0.600000;
    player->eval.param.mult1 = 1.820896;
    player->eval.param.mult2 = 0.850000;
    player->eval.param.empty = -0.037140;
    player->eval.param.shole = 1.160000;
    player->eval.param.maxht = -0.096389;
    player->eval.param.lorow = 0.079104;
    player->eval.param.block = -0.116000;
    player->eval.param.tslin = 0.400105;
    player->eval.param.tspct = 7.125092;
    player->eval.param.tsbon = 11.949120;
    player->eval.param.tsrev = 3.564946;
    player->eval.param.tspre = 3.912365;
    player->eval.param.occup = 1.327680;
    player->eval.param.avrow = 11.101529;
}

void init_piece(PIECE *piece, char type)
{
    assert(piece != NULL);
    assert(strchr(SHAPE_TYPES, type) != NULL);

    static char *I_SHAPE[4] = { "0000111100000000", "0010001000100010", "0000000011110000", "0100010001000100" };
    static char *Z_SHAPE[4] = { "110011000", "001011010", "000110011", "010110100" };
    static char *L_SHAPE[4] = { "001111000", "010010011", "000111100", "110010010" };
    static char *J_SHAPE[4] = { "100111000", "011010010", "000111001", "010010110" };
    static char *O_SHAPE[1] = { "1111" };
    static char *T_SHAPE[4] = { "010111000", "010011010", "000111010", "010110010" };
    static char *S_SHAPE[4] = { "011110000", "010011001", "000011110", "100110010" };

    memset(piece, 0, sizeof(PIECE));

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

// END