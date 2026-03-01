#include "game.h"
#include "raylib.h"

ng_game init_game()
{
	ng_game newGame;

	newGame.renderer = init_renderer(1280, 720);

	return newGame;
}

int run_game(ng_game game)
{
	BeginDrawing();

	ClearBackground(MAGENTA);

	EndDrawing();

	if (WindowShouldClose())
		return 1; // clean exit (make #defines if time allowed)
	return 0;
}

void exit_game(int exitCode)
{
	CloseWindow();
}