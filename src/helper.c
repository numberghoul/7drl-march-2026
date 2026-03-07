#include <raylib.h>
#include "game.h"
#include "raymath.h"

int vec2_index(Vector2 v2, int width)
{
	return v2.y * width + v2.x;
}

int coord_index(int x, int y, int width)
{
	return y * width + x;
}

Vector2 index_vec2(int index, int width)
{
	return (Vector2) {index % width, index / width};
}

Vector2 dir_vec2(int dir)
{
	switch(dir)
	{
	case DIR_NORTH:
		return VEC2_UP;
	case DIR_EAST:
		return VEC2_RIGHT;
	case DIR_SOUTH:
		return VEC2_DOWN;
	case DIR_WEST:
		return VEC2_LEFT;
	default:
		return Vector2Zero();
	}
}

int opposite_dir(int dir)
{
	switch(dir)
	{
	case DIR_NORTH:
		return DIR_SOUTH;
	case DIR_EAST:
		return DIR_WEST;
	case DIR_SOUTH:
		return DIR_NORTH;
	case DIR_WEST:
		return DIR_EAST;
	default:
		return -1;
	}
}