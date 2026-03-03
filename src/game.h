#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <raylib.h>

// RENDER_C

typedef struct {
	int charWidth;
	int columns, rows;
	int numChars;

	Texture2D fontAtlas;
} ng_bmpfont;

typedef struct {
	int winWidth, winHeight;
	int virtualWidth, virtualHeight;
	bool fullscreen;

	RenderTexture2D frameTexture;
} ng_renderer;

void print_char(char c, int x, int y, int size, ng_bmpfont font, Color color);
void print_char_vec2(char c, Vector2 pos, int size, ng_bmpfont font, Color color);

void print_string(const char* text, int x, int y, int size, ng_bmpfont font, Color color);
void print_string_vec2(const char* text, Vector2 pos, int size, ng_bmpfont font, Color color);

ng_bmpfont load_bmpfont(const char* path, int charWidth);
int unload_bmpfont(ng_bmpfont font);

ng_renderer init_renderer(const char* title, int winWidth, int winHeight, int virtualWidth, int virtualHeight);

// GAME_C
typedef struct {
	ng_renderer renderer;
	ng_bmpfont mainFont;
} ng_game;

int update(ng_game game, float dt);
int render(ng_game game);

ng_game init_game(const char* title, int virtualWidth, int virtualHeight); 
int run_game(ng_game game);
void exit_game(ng_game game, int exitCode);

#endif