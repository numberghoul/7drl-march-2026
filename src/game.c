#include "game.h"
#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>

int update(ng_game* game, float dt)
{
	if(IsKeyPressed(KEY_UP) || IsKeyDown(KEY_PAGE_UP))
	{
		if(game->current_floor > 0)
			game->current_floor--;
	}
	if(IsKeyPressed(KEY_DOWN) || IsKeyDown(KEY_PAGE_DOWN))
	{
		if(game->current_floor < game->dungeon.numLevels - 1)
			game->current_floor++;
	}

	return 0;
}

int render(ng_game* game)
{
	ClearBackground(BLACK);

	//print_string("--------", 12*16, 0, game.mainFont.charWidth, game.mainFont, PURPLE);
	print_box(12 * 2, 0, 8, 18, game->mainFont, ORANGE);
	print_string(TextFormat("flr:%d", game->current_floor + 1), 12 * 2 + 1, 1, 8, game->mainFont, WHITE);

	draw_map(game->dungeon.levels[game->current_floor], game->tileset);

	return 0;
}

ng_game* init_game(const char* title, int virtualWidth, int virtualHeight)
{
	ng_game* newGame = malloc(sizeof(ng_game));

	newGame->renderer = init_renderer(title, 1280, 720, virtualWidth, virtualHeight);

	newGame->mainFont = load_bmpfont("assets/font_8px.png", 8);

	newGame->tileset = load_tileset("assets/dungeon_16px.png", 16);

	generate_dungeon(&newGame->dungeon, MAX_LEVELS); // will likely prompt at start
	newGame->current_floor = 0;

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