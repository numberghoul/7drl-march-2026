#include "game.h"
#include <raylib.h>
#include <raymath.h>
#include <math.h>

void name_actor(ng_actor* actor, char* name)
{
	for(int i = 0; i < MAX_MSG_WIDTH; i++)
	{
		if (i != MAX_MSG_WIDTH - 1)
			actor->name[i] = name[i];
		else
			actor->name[i] = '\0';
	}
}

void move_actor(ng_actor* actor, ng_dir dir, float speed)
{
	Vector2 v2Dir = dir_vec2(dir);
	Vector2 velocity = (Vector2){v2Dir.x * speed, v2Dir.y * speed};
	actor->position = Vector2Add(actor->position, velocity);
}

ng_collision actor_collides(ng_actor* actors, ng_actor* actor, ng_room* room, ng_dir dir, float speed)
{
	ng_collision col;
	col.hit = false;
	col.id = -1;
	col.isActor = false;

	Vector2 position = actor->position;
	Vector2 checkPos;

	Vector2 v2Dir = dir_vec2(dir);
	Vector2 velocity = (Vector2){v2Dir.x * speed, v2Dir.y * speed};
	checkPos = Vector2Add(position, velocity);

	int checkX = (int)floorf(checkPos.x);
	int checkY = (int)floorf(checkPos.y);

	if (room->tiles[coord_index(checkX, checkY, ROOM_WIDTH)] != TILE_FLOOR)
	{
		col.id = room->tiles[coord_index(checkX, checkY, ROOM_WIDTH)];
		col.isActor = false;
		col.hit = true;

		return col;
	}

	for(int i = 0; i < MAX_ACTORS_ROOM; i++)
	{
		int actorCheck = room->actors[i];
		if (actorCheck == -1 || &actors[actorCheck] == actor) continue;

		int actorX = actors[actorCheck].position.x;
		int actorY = actors[actorCheck].position.y;

		if (fabsf(checkPos.x - actors[actorCheck].position.x) < 0.5f && 
		fabsf(checkPos.y - actors[actorCheck].position.y) < 0.5f) 
		{
			col.id = actorCheck;
			col.isActor = true;
			col.hit = true;
			return col;
		}	
	}

	return col;
}