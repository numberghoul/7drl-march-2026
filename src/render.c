#include "game.h"
#include "raylib.h"
#include <stdalign.h>

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