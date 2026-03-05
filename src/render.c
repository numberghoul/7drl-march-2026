#include "game.h"
#include "raylib.h"
#include "raymath.h"
#include <stdalign.h>

void print_char(unsigned char c, int x, int y, int size, ng_bmpfont font, Color color)
{
	Rectangle src = {((int)c % font.columns) * font.charWidth,
						((int)c / font.columns) * font.charWidth,
						font.charWidth, font.charWidth};

	Rectangle dest = {x * size, y * size, size, size};

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
		print_char(text[i], x + i, y, size, font, color);
		i++;
	}
}
void print_string_vec2(const char* text, Vector2 pos, int size, ng_bmpfont font, Color color)
{
	print_string(text, pos.x, pos.y, size, font, color);
}

void print_box(int x, int y, int width, int height, ng_bmpfont font, Color color)
{
	print_char((char)BOX_TOPLEFT, x, y, font.charWidth, font, color); // top left
	print_char((char)BOX_TOPRIGHT, x + width - 1, y, font.charWidth, font, color); // top right
	print_char((char)BOX_BOTLEFT, x, y + height - 1, font.charWidth, font, color); // bottom left
	print_char((char)BOX_BOTRIGHT, x + width - 1, y + height - 1, font.charWidth, font, color); // bottom right

	for (int i = 0; i < width - 2; i++)
	{
		print_char((char)BOX_HORIZONTAL, x + i + 1, y, font.charWidth, font, color);
		print_char((char)BOX_HORIZONTAL, x + i + 1, y + height - 1, font.charWidth, font, color);
	}

	for (int j = 0; j < height - 2; j++)
	{
		print_char((char)BOX_VERTICAL, x, y + j + 1, font.charWidth, font, color);
		print_char((char)BOX_VERTICAL, x + width - 1, y + j + 1, font.charWidth, font, color);
	}
}

void print_box_rec(Rectangle rec, ng_bmpfont font, Color color)
{
	print_box(rec.x, rec.y, rec.width, rec.height, font, color);
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

ng_tileset load_tileset(const char* path, int tileWidth)
{
	ng_tileset newSet;

	newSet.tileWidth = tileWidth;

	newSet.tileAtlas = LoadTexture(path);

	newSet.columns = newSet.tileAtlas.width / newSet.tileWidth;
	newSet.rows = newSet.tileAtlas.width / newSet.tileWidth;
	newSet.numTiles = newSet.columns * newSet.rows;

	return newSet;
}

int unload_tileset(ng_tileset tileset)
{
	UnloadTexture(tileset.tileAtlas);

	return 0;
}

void draw_tile(int id, int x, int y, ng_tileset tileset, Color color)
{
	Rectangle src = {(id % tileset.columns) * tileset.tileWidth,
						(id / tileset.columns) * tileset.tileWidth,
						tileset.tileWidth, tileset.tileWidth};

	Rectangle dest = {x * tileset.tileWidth, y * tileset.tileWidth, tileset.tileWidth, tileset.tileWidth};

	DrawTexturePro(tileset.tileAtlas, src, dest, Vector2Zero(), 0.0f, color);
}

void draw_tile_vec2(int id, Vector2 pos, ng_tileset tileset, Color color)
{
	draw_tile(id, pos.x, pos.y, tileset, color);
}

void draw_room(ng_room room, ng_tileset tileset)
{
	for (int i = 0; i < MAX_TILES; i++)
	{
		int x = i % ROOM_WIDTH;
		int y = i / ROOM_WIDTH;

		draw_tile(room.tiles[i], x, y, tileset, WHITE);
	}
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