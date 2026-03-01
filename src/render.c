#include "game.h"

ng_renderer init_renderer(int width, int height)
{
	ng_renderer r;

	r.winWidth = width;
	r.winHeight = height;
	r.fullscreen = false;

	//init window
	InitWindow(r.winWidth, r.winHeight, "7drl-march2026");
	SetTargetFPS(60);

	return r;
}