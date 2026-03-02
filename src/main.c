#include "game.h"

int main(void)
{
	ng_game game = init_game("7drl-march2026", 256, 144);
	int exitCode = 0;

	while(exitCode == 0)
	{
		exitCode = run_game(game);
	}

	exit_game(exitCode);

	return exitCode;
}