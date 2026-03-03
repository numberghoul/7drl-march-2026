#include "game.h"
#include "raylib.h"
#include "raymath.h"
#include <stdalign.h>

void print_char(char c, int x, int y, int size, ng_bmpfont font, Color color)
{
	Rectangle src = {((int)c % font.columns) * font.charWidth,
						((int)c / font.columns) * font.charWidth,
						font.charWidth, font.charWidth};

	Rectangle dest = {x, y, size, size};

	DrawTexturePro(font.fontAtlas, src, dest, Vector2Zero(), 0.0f, color);
}

void print_char_vec2(char c, Vector2 pos, int size, ng_bmpfont font, Color color)
{
	print_char(c, pos.x, pos.y, size, font, color);
}

void print_string(const char* text, int x, int y, int size, ng_bmpfont font, Color color)
{
	int i = 0;
	while(text[i] != '\0')
	{
		print_char(text[i], x + (i * size), y, size, font, color);
		i++;
	}
}
void print_string_vec2(const char* text, Vector2 pos, int size, ng_bmpfont font, Color color)
{
	print_string(text, pos.x, pos.y, size, font, color);
}

ng_bmpfont load_bmpfont(const char* path, int charWidth)
{
	ng_bmpfont newFont;

	newFont.charWidth = charWidth;

	Image fontImg = LoadImage(path);
	ImageColorContrast(&fontImg, 100.0f);
	Image alphaMask = GenImageColor(fontImg.width, fontImg.height, WHITE);

	ImageAlphaMask(&alphaMask, fontImg);
	ImageColorBrightness(&alphaMask, 255);

	newFont.fontAtlas = LoadTextureFromImage(alphaMask);

	// assumes square font characters
	newFont.columns = fontImg.width / newFont.charWidth;
	newFont.rows = fontImg.height / newFont.charWidth;
	newFont.numChars = newFont.columns * newFont.rows;

	UnloadImage(alphaMask);
	UnloadImage(fontImg);

	return newFont;
}

int unload_bmpfont(ng_bmpfont font)
{
	UnloadTexture(font.fontAtlas);

	return 0;
}

ng_renderer init_renderer(const char* title, int winWidth, int winHeight, int virtualWidth, int virtualHeight)
{
	ng_renderer r;

	r.winWidth = winWidth;
	r.winHeight = winHeight;
	r.virtualWidth = virtualWidth;
	r.virtualHeight = virtualHeight;
	r.fullscreen = false;

	//init window
	InitWindow(r.winWidth, r.winHeight, "7drl-march2026");
	SetTargetFPS(60);

	r.frameTexture = LoadRenderTexture(r.virtualWidth, r.virtualHeight);

	return r;
}