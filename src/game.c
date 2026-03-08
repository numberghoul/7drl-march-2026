#include "game.h"
#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int update(ng_game* game, float dt)
{
	if(IsKeyPressed(KEY_M))
	{
		if(game->state == STATE_PLAY)
			game->state = STATE_MAP;
		else
			game->state = STATE_PLAY;
	}

	ng_collision playerCol;
	bool checkMove = false;
	int moveDir;

	switch (game->state)
	{
	case STATE_MENU:
		break;
	case STATE_PLAY:
	
	if(IsKeyPressed(KEY_UP))
	{
		moveDir = DIR_NORTH;
		checkMove = true;
	}
	if(IsKeyPressed(KEY_RIGHT))
	{
		moveDir = DIR_EAST;
		checkMove = true;
	}
	if(IsKeyPressed(KEY_DOWN))
	{
		moveDir = DIR_SOUTH;
		checkMove = true;
	}
	if(IsKeyPressed(KEY_LEFT))
	{
		moveDir = DIR_WEST;
		checkMove = true;
	}

	if (checkMove)
	{
		playerCol = actor_collides(game->actors, &game->actors[ACTOR_PLAYER], 
									&game->dungeon.levels[game->current_floor].rooms[game->current_room], 
									moveDir, 1.0f);
		if(!playerCol.hit)
		{
			move_actor(&game->actors[ACTOR_PLAYER], moveDir, 1.0f);
		}
		else
		{
			if (playerCol.isActor)
			{

			}
			else
			{
				switch(playerCol.id)
				{
				case TILE_DOORV_TL:
				case TILE_DOORV_TR:
					move_actor(&game->actors[ACTOR_PLAYER], DIR_SOUTH, LEVEL_HEIGHT - 2);
					game->current_room = game->dungeon.levels[game->current_floor].rooms[game->current_room].doors[DIR_NORTH].connectedRoom;
					game->dungeon.levels[game->current_floor].rooms[game->current_room].visible = true;
					break;
				case TILE_DOORV_BL:
				case TILE_DOORV_BR:
					move_actor(&game->actors[ACTOR_PLAYER], DIR_NORTH, LEVEL_HEIGHT - 2);
					game->current_room = game->dungeon.levels[game->current_floor].rooms[game->current_room].doors[DIR_SOUTH].connectedRoom;
					game->dungeon.levels[game->current_floor].rooms[game->current_room].visible = true;
					break;
				case TILE_DOORH_LC:
					move_actor(&game->actors[ACTOR_PLAYER], DIR_EAST, LEVEL_WIDTH - 3);
					game->current_room = game->dungeon.levels[game->current_floor].rooms[game->current_room].doors[DIR_WEST].connectedRoom;
					game->dungeon.levels[game->current_floor].rooms[game->current_room].visible = true;
					break;
				case TILE_DOORH_RC:
					move_actor(&game->actors[ACTOR_PLAYER], DIR_WEST, LEVEL_WIDTH - 3);
					game->current_room = game->dungeon.levels[game->current_floor].rooms[game->current_room].doors[DIR_EAST].connectedRoom;
					game->dungeon.levels[game->current_floor].rooms[game->current_room].visible = true;
					break;
				case TILE_STAIRS_DOWN:
					if (game->current_floor < game->dungeon.numLevels)
					{
						game->current_floor++;
						game->current_room = game->dungeon.levels[game->current_floor].startRoom;
						game->actors[ACTOR_PLAYER].position = (Vector2) {START_X, START_Y};
					}
					break;
				case TILE_STAIRS_UP:
					if (game->current_floor > 0)
					{
						game->current_floor--;
						game->current_room = game->dungeon.levels[game->current_floor].endRoom;
						game->actors[ACTOR_PLAYER].position = (Vector2) {START_X - 2, START_Y};
					}
					break;
				default:
					break;
				}
			}
		}
	}

		break;
	case STATE_MAP:
		break;
	default:
		printf("invalid game state");
		return INVALID_STATE;
		break;
	}

	return 0;
}

int render(ng_game* game)
{
	ClearBackground(BLACK);

	if (game->state != STATE_MENU)
	{
		print_box(12 * 2, 0, 8, 18, game->mainFont, ORANGE);
		print_string(game->actors[ACTOR_PLAYER].name, 12 * 2 + 1, 1, 8, game->mainFont, WHITE);
		print_string(TextFormat("HP %d", game->actors[ACTOR_PLAYER].stats.hp), 12 * 2 + 1, 2, 8, game->mainFont, WHITE);
		print_string(TextFormat("AC %d", game->actors[ACTOR_PLAYER].stats.ac), 12 * 2 + 1, 3, 8, game->mainFont, WHITE);

		print_string(TextFormat("STR %d", game->actors[ACTOR_PLAYER].stats.strength), 12 * 2 + 1, 5, 8, game->mainFont, WHITE);
		print_string(TextFormat("AGI %d", game->actors[ACTOR_PLAYER].stats.agility), 12 * 2 + 1, 6, 8, game->mainFont, WHITE);
		print_string(TextFormat("WIS %d", game->actors[ACTOR_PLAYER].stats.wisdom), 12 * 2 + 1, 7, 8, game->mainFont, WHITE);

		print_string(TextFormat("[M]Map", game->actors[ACTOR_PLAYER].stats.wisdom), 12 * 2 + 1, 9, 8, game->mainFont, WHITE);

		print_string(TextFormat("LVL %d", game->actors[ACTOR_PLAYER].stats.level), 12 * 2 + 1, 15, 8, game->mainFont, WHITE);
		print_string(TextFormat("XP %d", game->actors[ACTOR_PLAYER].stats.xp), 12 * 2 + 1, 16, 8, game->mainFont, WHITE);
	}

	switch (game->state)
	{
	case STATE_MENU:
		break;
	case STATE_PLAY:
		draw_room(game->dungeon.levels[game->current_floor].rooms[game->current_room], game->tileset);

		draw_tile(game->actors[ACTOR_PLAYER].spriteId,
				 game->actors[ACTOR_PLAYER].position.x, game->actors[ACTOR_PLAYER].position.y, 
				 false, false, game->spriteSheet, WHITE);
		break;
	case STATE_MAP:
		draw_map(game->dungeon.levels[game->current_floor], game->tileset);
		print_string(TextFormat("FLOOR %d", game->current_floor + 1), 0, 17, 8, game->mainFont, WHITE);
		break;
	default:
		printf("invalid game state");
		return INVALID_STATE;
		break;
	}

	return 0;
}

ng_game* init_game(const char* title, int virtualWidth, int virtualHeight)
{
	ng_game* newGame = malloc(sizeof(ng_game));

	newGame->renderer = init_renderer(title, 1280, 720, virtualWidth, virtualHeight);

	newGame->mainFont = load_bmpfont("assets/font_8px.png", 8);

	newGame->tileset = load_tileset("assets/dungeon_16px.png", 16);

	newGame->spriteSheet = load_tileset("assets/actors_16px.png", 16);

	newGame->state = STATE_PLAY;

	newGame->actors[ACTOR_PLAYER].id = ACTOR_PLAYER;
	newGame->actors[ACTOR_PLAYER].spriteId = 0;
	newGame->actors[ACTOR_PLAYER].position = (Vector2){START_X, START_Y};
	newGame->actors[ACTOR_PLAYER].isPlayer = true;
	newGame->actors[ACTOR_PLAYER].stats = generate_stats(1, 1, 1, 0);
	name_actor(&newGame->actors[ACTOR_PLAYER], "NAME");

	generate_dungeon(&newGame->dungeon, MAX_LEVELS); // will likely prompt at start
	newGame->current_floor = 0;
	newGame->current_room = newGame->dungeon.levels[newGame->current_floor].startRoom;

	return newGame;
}

int run_game(ng_game* game)
{
	int exitCode = 0;

	exitCode = update(game, GetFrameTime());
	if (exitCode != 0)
		return exitCode;

	BeginDrawing();

	ClearBackground(MAGENTA);

	BeginTextureMode(game->renderer.frameTexture);

	exitCode = render(game);
	if (exitCode != 0)
		return exitCode;

	EndTextureMode();

	DrawTexturePro(game->renderer.frameTexture.texture, 
					(Rectangle){0.0f, 
								0.0f, 
								game->renderer.virtualWidth, 
								-game->renderer.virtualHeight}, 
					(Rectangle){0.0f, 
								0.0f, 
								game->renderer.winWidth, 
								game->renderer.winHeight}, 
					Vector2Zero(), 
					0.0f, 
					WHITE);

	EndDrawing();

	if (WindowShouldClose())
		exitCode = 1; // clean exit (make #defines if time allowed)

	return exitCode;
}

void exit_game(ng_game* game, int exitCode)
{
	unload_bmpfont(game->mainFont);
	CloseWindow();

	free(game);

	printf("Exited with code %d", exitCode);
}