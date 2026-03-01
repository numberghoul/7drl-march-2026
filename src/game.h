#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <raylib.h>

// RENDER_C

typedef struct {
	int size;
	Texture2D fontatlas;
} ng_bmpfont;

typedef struct {
	int winWidth, winHeight;
	bool fullscreen;
} ng_renderer;

ng_renderer init_renderer(int width, int height);

// GAME_C
typedef struct {
	ng_renderer renderer;
} ng_game;

ng_game init_game(); 
int run_game(ng_game game);
void exit_game(int exitCode);

#endif