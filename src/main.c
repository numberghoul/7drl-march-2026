#include "game.h"

int main(void)
{
	ng_game game = init_game();
	int exitCode;

	while(exitCode == 0)
	{
		exitCode = run_game(game);
	}

	exit_game(exitCode);

	return exitCode;
}