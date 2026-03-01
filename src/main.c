#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "raylib.h"
#include "raymath.h"

int main(void)
{
	int screenWidth = 1280;
	int screenHeight = 720;

	InitWindow(screenWidth, screenHeight, "7drl-march2026");

	SetTargetFPS(60);

	while (!WindowShouldClose())    // Detect window close button or ESC key
	{
		BeginDrawing();

		ClearBackground(BLACK);

		EndDrawing();
	}

	CloseWindow();

	return 0;
}