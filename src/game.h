#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <raylib.h>

#define MAX_LEVELS 256
#define LEVEL_WIDTH 12
#define LEVEL_HEIGHT 8
#define MAX_ROOMS LEVEL_WIDTH * LEVEL_HEIGHT
#define ROOM_WIDTH 12
#define ROOM_HEIGHT 9
#define MAX_TILES ROOM_WIDTH * ROOM_HEIGHT
#define PREFAB_COUNT 1

#define DIR_NORTH 0
#define DIR_EAST 1
#define DIR_SOUTH 2
#define DIR_WEST 3

#define VEC2_UP    (Vector2){0, -1}
#define VEC2_RIGHT (Vector2){1,  0}
#define VEC2_DOWN  (Vector2){0,  1}
#define VEC2_LEFT  (Vector2){-1, 0}

// Font IDs
#define BOX_HORIZONTAL 205
#define BOX_VERTICAL 186
#define BOX_TOPLEFT 201
#define BOX_TOPRIGHT 187
#define BOX_BOTLEFT 200
#define BOX_BOTRIGHT 188

// Tile IDs
#define MAP_ROOM 49
#define MAP_DOORV 50
#define MAP_DOORH 51
#define MAP_DOWN 16
#define MAP_UP 32

// HELPER_C
int vec2_index(Vector2 v2, int width);
int coord_index(int x, int y, int width);
Vector2 index_vec2(int index, int width);
Vector2 dir_vec2(int dir);
int opposite_dir(int dir);

// DUNGEON_C
typedef struct {
	bool active;
	int connectedRoom;
} ng_door;

typedef struct {
	int tiles[MAX_TILES];
	ng_door doors[4];

	bool active;
	bool visible;
	bool start;
	bool end;
} ng_room;

typedef struct {
	ng_room rooms[MAX_ROOMS];
	int id;
	int startRoom;
	int endRoom;
} ng_level;

typedef struct {
	ng_level levels[MAX_LEVELS];
	int numLevels;
} ng_dungeon;

ng_room generate_room(int prefabId, bool start, bool end);
ng_level generate_level(float difficulty); // between 0.0 and 1.0
void generate_dungeon(ng_dungeon* dungeon, int levels); // less than MAX_LEVELS

// RENDER_C

typedef struct {
	int charWidth;
	int columns, rows;
	int numChars;

	Texture2D fontAtlas;
} ng_bmpfont;

typedef struct {
	int tileWidth;
	int columns, rows;
	int numTiles;

	Texture2D tileAtlas;
} ng_tileset;

typedef struct {
	int winWidth, winHeight;
	int virtualWidth, virtualHeight;
	bool fullscreen;

	RenderTexture2D frameTexture;
} ng_renderer;

void print_char(unsigned char c, int x, int y, int size, ng_bmpfont font, Color color);
void print_char_vec2(char c, Vector2 pos, int size, ng_bmpfont font, Color color);

void print_string(const char* text, int x, int y, int size, ng_bmpfont font, Color color);
void print_string_vec2(const char* text, Vector2 pos, int size, ng_bmpfont font, Color color);

void print_box(int x, int y, int width, int height, ng_bmpfont font, Color color);
void print_box_rec(Rectangle rec, ng_bmpfont font, Color color);

ng_bmpfont load_bmpfont(const char* path, int charWidth);
int unload_bmpfont(ng_bmpfont font);

ng_tileset load_tileset(const char* path, int tileWidth);
int unload_tileset(ng_tileset tileset);
void draw_tile(int id, int x, int y, bool flipX, bool flipY, ng_tileset tileset, Color color); // x and y scaled to print in grid space, use raylib WHITE for no shading
void draw_tile_vec2(int id, Vector2 pos, bool flipX, bool flipY, ng_tileset tileset, Color color);

void draw_room(ng_room room, ng_tileset tileset);
void draw_map(ng_level level, ng_tileset tileset);

ng_renderer init_renderer(const char* title, int winWidth, int winHeight, int virtualWidth, int virtualHeight);

// GAME_C

typedef struct {
	ng_renderer renderer;
	ng_bmpfont mainFont;
	ng_tileset tileset;

	ng_dungeon dungeon;
	int current_floor;
} ng_game;

int update(ng_game* game, float dt);
int render(ng_game* game);

ng_game* init_game(const char* title, int virtualWidth, int virtualHeight); 
int run_game(ng_game* game);
void exit_game(ng_game* game, int exitCode);

#endif