//-------------------------------------------------------------------------
// TheAiGames's block bot by Alcides Schulz (zluchs)
//-------------------------------------------------------------------------

//games played: 100 total rounds: 8507 avg rounds: 85.07 avg xlines: 0.00 avg points: 66.46 avg lines: 34.37 results: 66.4600

#define VERSION 5

#include "zluhcs_defs.h"

GAME_SETTINGS   game_settings;
GAME_STATE      game_state;
PLAYER          my_player;
PLAYER          opp_player;

int main(void)
{
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
		if (!strncmp(line, "play", 4))
			simulate_game(&my_player, 0, 100, 1, 100);
#endif
	}

	return 0;
}

void settings(char *type, char *value)
{
	assert(type != NULL);
	assert(value != NULL);

    if (!strcmp(type, "timebank")) {
        game_settings.timebank = atoi(value);
        return;
    }
    if (!strcmp(type, "time_per_move")) { 
        game_settings.time_per_move = atoi(value); 
        return; 
    }
    if (!strcmp(type, "player_names")) {
        strcpy(game_settings.player_names, value);
        return;
    }
    if (!strcmp(type, "your_bot")) {
        strcpy(game_settings.my_bot_name, value);
        return;
    }
    if (!strcmp(type, "field_height")) {
        game_settings.field_height = atoi(value);
        return;
    }
    if (!strcmp(type, "field_width")) {
        game_settings.field_width = atoi(value);
        return;
    }
    fprintf(stderr, "settings: unknown type: [%s]\n", type);
}

void update(char *player, char *type, char *value)
{
	assert(player != NULL);
	assert(type != NULL);
	assert(value != NULL);

	if (!strcmp(player, "game")) {
        if (!strcmp(type, "round")) {
            game_state.round = atoi(value);
            return;
        }
        if (!strcmp(type, "this_piece_type")) {
            game_state.this_piece_type = value[0];
            return;
        }
        if (!strcmp(type, "next_piece_type")) {
            game_state.next_piece_type = value[0];
            return;
        }
        if (!strcmp(type, "this_piece_position")) {
            sscanf(value, "%d,%d", &game_state.this_piece_position_col, &game_state.this_piece_position_row);
            return;
        }
        fprintf(stderr, "update: unknown game type: [%s]\n", type);
		return;
	}

	if (!strcmp(player, game_settings.my_bot_name))
		update_player(&my_player, type, value);
	else
		update_player(&opp_player, type, value);
}

void update_player(PLAYER *player, char *type, char *value) {
    assert(player != NULL);
    assert(type != NULL);
    assert(value != NULL);

    if (!strcmp(type, "row_points")) {
        player->row_points = atoi(value);
        return;
    }
    if (!strcmp(type, "combo")) {
        player->combo = atoi(value);
        return;
    }
    if (!strcmp(type, "skips")) {
        player->skips = atoi(value);
        return;
    }
    if (!strcmp(type, "field")) {
        if (player->field == NULL) {
            player->field = (char *)malloc(game_settings.field_height * game_settings.field_width);
            if (player->field == NULL) {
                fprintf(stderr, "update_player: malloc failed for player->field.\n");
                return;
            }
        }
        int row, col, i;
        for (row = col = i = 0; value[i]; i++) {
            switch (value[i]) {
            case ',': col++; break;
            case ';': row++; col = 0; break;
            default: player->field[row * game_settings.field_width + col] = value[i]; break;
            }
        }
        return;
    }
    fprintf(stderr, "update_player: unknown type: [%s]\n", type);
}

// END
