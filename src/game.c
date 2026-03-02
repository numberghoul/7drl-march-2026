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

	DrawTexture(game.mainFont.fontAtlas, 0, 0, WHITE);

	return 0;
}

ng_game init_game(const char* title, int virtualWidth, int virtualHeight)
{
	ng_game newGame;

	newGame.renderer = init_renderer(title, 1280, 720, virtualWidth, virtualHeight);

	newGame.mainFont = load_bmpfont("assets/font_8px.png", 8);

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