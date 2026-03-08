#include "game.h"
#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int update(ng_game* game, float dt)
{
	if(IsKeyPressed(KEY_M) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_LEFT))
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
		game->moveTimer += dt;
		game->animTimer += dt;

		if(game->attackWait)
			game->attackCooldown += dt;

		if(game->attacking)
			game->attackTimer += dt;
		else
		{
			if(IsKeyDown(KEY_Z) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
			{
				if (!game->attackWait)
				{
					game->attacking = true;
					ng_collision hit = actor_collides(game->actors, &game->actors[ACTOR_PLAYER],
					 &game->dungeon.levels[game->current_floor].rooms[game->current_room], game->actors[ACTOR_PLAYER].dir, 1.0f);

					if(hit.hit && hit.isActor)
					{
						game->actors[hit.id].alive = false;
					}
				}
			}
		}

		if(game->attackTimer >= game->attackDuration)
		{
			game->attacking = false;
			game->attackTimer = 0.0f;
			game->attackWait = true;
		}

		if(game->attackCooldown >= game->attackDuration * 2)
		{
			game->attackCooldown = 0.0f;
			game->attackWait = false;
		}

		if (game->animTimer >= game->animInterval)
		{
			game->animShift = !game->animShift;
			game->animTimer = 0.0f;
		}

		if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT) ||
			IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN) ||
			IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) 
		{
    		if (game->moveTimer >= game->moveBuffer && !game->attacking) 
    		{
        		if (IsKeyDown(KEY_UP) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP))
        		{
        		    moveDir = DIR_NORTH;
        		}
        		else if (IsKeyDown(KEY_DOWN) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN))  
        		{
        			moveDir = DIR_SOUTH;
        		}
        		else if (IsKeyDown(KEY_LEFT) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT))  
        		{
        			moveDir = DIR_WEST;
        		}
        		else if (IsKeyDown(KEY_RIGHT) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) 
        		{
        			moveDir = DIR_EAST;
        		}
		
        		checkMove = true;
        		game->moveTimer = 0.0f;
        		game->actors[ACTOR_PLAYER].dir = moveDir;
        		game->actors[ACTOR_PLAYER].walkStep = !game->actors[ACTOR_PLAYER].walkStep;
		    }
		} 
		else 
		{
		    game->moveTimer = game->moveBuffer; 
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
					if(!game->actors[playerCol.id].alive)
						move_actor(&game->actors[ACTOR_PLAYER], moveDir, 1.0f);
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

		print_string(TextFormat("[%c]Mov", (char)game->promptPad), 12 * 2 + 1, 9, 8, game->mainFont, WHITE);
		print_string(TextFormat("[%c]Atk", (char)game->promptA), 12 * 2 + 1, 10, 8, game->mainFont, WHITE);
		print_string(TextFormat("[%c]Map", (char)game->promptSel), 12 * 2 + 1, 11, 8, game->mainFont, WHITE);
		//print_string(TextFormat("%d", game->attacking), 12 * 2 + 1, 12, 8, game->mainFont, WHITE);

		print_string(TextFormat("LVL %d", game->actors[ACTOR_PLAYER].stats.level), 12 * 2 + 1, 15, 8, game->mainFont, WHITE);
		print_string(TextFormat("XP %d", game->actors[ACTOR_PLAYER].stats.xp), 12 * 2 + 1, 16, 8, game->mainFont, WHITE);
	}

	switch (game->state)
	{
	case STATE_MENU:
		break;
	case STATE_PLAY:
		draw_room(game->dungeon.levels[game->current_floor].rooms[game->current_room], game->tileset, game->animShift);

		int actorCount = 0;
		int actorId = game->dungeon.levels[game->current_floor].rooms[game->current_room].actors[actorCount];

		while(actorId != -1)
		{
			Vector2 actorPos = game->actors[actorId].position;
			if(game->actors[actorId].alive)
				draw_tile(game->actors[actorId].spriteId, actorPos.x, actorPos.y, false, false, game->spriteSheet, WHITE);

			actorCount++;
			actorId = game->dungeon.levels[game->current_floor].rooms[game->current_room].actors[actorCount];
		}

		int playerSprite = game->actors[ACTOR_PLAYER].spriteId;
		int playerDir = game->actors[ACTOR_PLAYER].dir;
		bool playerFlip = game->actors[ACTOR_PLAYER].walkStep;
		Vector2 playerPos = game->actors[ACTOR_PLAYER].position;

		if (playerDir == DIR_NORTH)
		{
			playerSprite += PLAYER_FRAME_N;
		}
		else if (playerDir == DIR_WEST)
		{
			if(playerFlip)
				playerSprite += PLAYER_FRAME_W2;
			else
				playerSprite += PLAYER_FRAME_W1;

			playerFlip = false; // don't actually flip the sprite, facing left
		}
		else if (playerDir == DIR_EAST)
		{
			if(playerFlip)
				playerSprite += PLAYER_FRAME_W2;
			else
				playerSprite += PLAYER_FRAME_W1;

			playerFlip = true; // facing right, always flip
		}

		draw_tile(playerSprite,
				 game->actors[ACTOR_PLAYER].position.x, game->actors[ACTOR_PLAYER].position.y, 
				 playerFlip, false, game->playerSheet, WHITE);

		if(game->attacking)
		{
			Vector2 swordPos;
			bool flipX = false;
			bool flipY = false;
			int swordId = PLAYER_SWORD;
			int offset = 1;
			switch(playerDir)
			{
			case DIR_NORTH:
				swordPos = Vector2Add(playerPos, VEC2_UP);
				flipY = true;
				swordId += 2;
				offset = 16;
				break;
			case DIR_SOUTH:
				swordPos = Vector2Add(playerPos, VEC2_DOWN);
				swordId += 2;
				offset = 16;
				break;
			case DIR_EAST:
				swordPos = Vector2Add(playerPos, VEC2_RIGHT);
				break;
			case DIR_WEST:
				swordPos = Vector2Add(playerPos, VEC2_LEFT);
				flipX = true;
				break;
			default:
				break;
			}
			draw_tile(swordId, playerPos.x, playerPos.y, flipX, flipY, game->playerSheet, WHITE);
			draw_tile(swordId + offset, swordPos.x, swordPos.y, flipX, flipY, game->playerSheet, WHITE);
		}
		break;
	case STATE_MAP:
		draw_map(game->current_room, game->dungeon.levels[game->current_floor], game->tileset);
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

	newGame->playerSheet = load_tileset("assets/players_16px.png", 16);

	newGame->state = STATE_PLAY;
	newGame->moveTimer = 0.0f;
	newGame->moveBuffer = 0.15f;

	newGame->animTimer = 0.0f;
	newGame->animInterval = 0.5f;
	newGame->animShift = false;

	newGame->attackTimer = 0.0f;
	newGame->attackCooldown = 0.0f;
	newGame->attackDuration = .15f;
	newGame->attacking = false;
	newGame->attackWait = false;

	newGame->actors[ACTOR_PLAYER].id = ACTOR_PLAYER;
	newGame->actors[ACTOR_PLAYER].spriteId = 0;
	newGame->actors[ACTOR_PLAYER].position = (Vector2){START_X, START_Y};
	newGame->actors[ACTOR_PLAYER].isPlayer = true;
	newGame->actors[ACTOR_PLAYER].dir = DIR_SOUTH;
	newGame->actors[ACTOR_PLAYER].walkStep = false;
	newGame->actors[ACTOR_PLAYER].stats = generate_stats(1, 1, 1, 0);
	name_actor(&newGame->actors[ACTOR_PLAYER], "NAME");

	newGame->nextActorId = 1;

	generate_dungeon(&newGame->dungeon, MAX_LEVELS); // will likely prompt at start
	populate_rooms(&newGame->dungeon, newGame->actors, &newGame->nextActorId);
	newGame->current_floor = 0;
	newGame->current_room = newGame->dungeon.levels[newGame->current_floor].startRoom;

	if (IsGamepadAvailable(0))
	{
		newGame->promptA = BTN_A;
		newGame->promptB = BTN_B;
		newGame->promptSel = BTN_SEL;
		newGame->promptStart = BTN_START;
		newGame->promptPad = BTN_DPAD;
	}
	else
	{
		newGame->promptA = BTN_PRI;
		newGame->promptB = BTN_SEC;
		newGame->promptSel = BTN_MAP;
		newGame->promptStart = BTN_PAUSE;
		newGame->promptPad = BTN_ARROWS;
	}

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
	unload_tileset(game->tileset);
	unload_tileset(game->spriteSheet);
	unload_tileset(game->playerSheet);
	CloseWindow();

	free(game);

	printf("Exited with code %d", exitCode);
}