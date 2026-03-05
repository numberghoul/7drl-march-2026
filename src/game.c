#include "game.h"
#include "raylib.h"
#include "raymath.h"
#include <stdio.h>

int update(ng_game game, float dt)
{
	return 0;
}

int render(ng_game game)
{
	ClearBackground(BLACK);

	//print_string("--------", 12*16, 0, game.mainFont.charWidth, game.mainFont, PURPLE);
	print_box(12 * 2, 0, 8, 18, game.mainFont, ORANGE);
	print_string("Test", 12 * 2 + 1, 1, 8, game.mainFont, WHITE);

	draw_room(game.testRoom, game.tileset);

	draw_tile(52, 5, 0, game.tileset, WHITE);
	draw_tile(53, 6, 0, game.tileset, WHITE);

	draw_tile(68, 5, 8, game.tileset, WHITE);
	draw_tile(69, 6, 8, game.tileset, WHITE);

	draw_tile(4, 0, 3, game.tileset, WHITE);
	draw_tile(20, 0, 4, game.tileset, WHITE);
	draw_tile(36, 0, 5, game.tileset, WHITE);

	draw_tile(5, 11, 3, game.tileset, WHITE);
	draw_tile(21, 11, 4, game.tileset, WHITE);
	draw_tile(37, 11, 5, game.tileset, WHITE);

	return 0;
}

ng_game init_game(const char* title, int virtualWidth, int virtualHeight)
{
	ng_game newGame;

	newGame.renderer = init_renderer(title, 1280, 720, virtualWidth, virtualHeight);

	newGame.mainFont = load_bmpfont("assets/font_8px.png", 8);

	newGame.tileset = load_tileset("assets/dungeon_16px.png", 16);

	newGame.testRoom = generate_room(0);

	return newGame;
}

int run_game(ng_game game)
{
	int exitCode = 0;

	exitCode = update(game, GetFrameTime());
	if (exitCode != 0)
		return exitCode;

	BeginDrawing();

	ClearBackground(MAGENTA);

	BeginTextureMode(game.renderer.frameTexture);

	exitCode = render(game);
	if (exitCode != 0)
		return exitCode;

	EndTextureMode();

	DrawTexturePro(game.renderer.frameTexture.texture, 
					(Rectangle){0.0f, 
								0.0f, 
								game.renderer.virtualWidth, 
								-game.renderer.virtualHeight}, 
					(Rectangle){0.0f, 
								0.0f, 
								game.renderer.winWidth, 
								game.renderer.winHeight}, 
					Vector2Zero(), 
					0.0f, 
					WHITE);

	EndDrawing();

	if (WindowShouldClose())
		exitCode = 1; // clean exit (make #defines if time allowed)

	return exitCode;
}

void exit_game(ng_game game, int exitCode)
{
	unload_bmpfont(game.mainFont);
	CloseWindow();

	printf("Exited with code %d", exitCode);
}