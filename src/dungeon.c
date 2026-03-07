#include "game.h"
#include <raylib.h>
#include <raymath.h>
#include <string.h>
#include <stdio.h>

ng_room generate_room(int prefabId, bool start, bool end)
{
    ng_room newRoom = { 0 }; 
    char* prefabData = LoadFileText(TextFormat("assets/rooms/%d.csv", prefabId));
    int tileCount = 0;

    if (prefabData != NULL)
    {
        int count = 0;
        // split by comma
        const char **values = TextSplit(prefabData, ',', &count);

        for (int i = 0; i < count; i++)
        {
            // convert and store
            if (tileCount < MAX_TILES)
            {
                newRoom.tiles[tileCount] = TextToInteger(values[i]);
                tileCount++;
            }
        }
        UnloadFileText(prefabData);
    }

    for (int i = 0; i < 4; i++)
    {
    	newRoom.doors[i].connectedRoom = -1;
    	newRoom.doors[i].active = false;
    }

    newRoom.active = true;
    newRoom.visible = false;
    newRoom.start = start;
    newRoom.end = end;
    return newRoom;
}


ng_level generate_level(float difficulty)
{
	ng_level newLevel = {0};

	int numRooms = difficulty * (GetRandomValue(5, MAX_ROOMS));
	if (numRooms < 5) numRooms = 5;
	if (numRooms > MAX_ROOMS) numRooms = MAX_ROOMS;

	int roomsCreated[MAX_ROOMS];
	for (int i = 0; i < MAX_ROOMS; i++)
	{
		roomsCreated[i] = -1;
	}

	int startRoom = GetRandomValue(0, MAX_ROOMS - 1);
	int currentRoom = startRoom;

	roomsCreated[0] = currentRoom;
	newLevel.rooms[currentRoom] = generate_room(0, true, false);
	newLevel.rooms[currentRoom].visible = true;
	Vector2 position = index_vec2(currentRoom, LEVEL_WIDTH);

	int count = 1;
	int timeout = 0;
	int furthestRoom = startRoom;

	while (count < numRooms)
	{
		int checkDir = GetRandomValue(0, 3);
		Vector2 checkPosition;

		switch(checkDir)
		{
		case DIR_NORTH:
			checkPosition = Vector2Add(position, VEC2_UP);
			break;
		case DIR_EAST:
			checkPosition = Vector2Add(position, VEC2_RIGHT);
			break;
		case DIR_SOUTH:
			checkPosition = Vector2Add(position, VEC2_DOWN);
			break;
		case DIR_WEST:
			checkPosition = Vector2Add(position, VEC2_LEFT);
			break;
		default:
			printf("Incorrect direction passed!");
			break;
		}

		if (checkPosition.x >= 0 && checkPosition.x < LEVEL_WIDTH && 
			checkPosition.y >= 0 && checkPosition.y < LEVEL_HEIGHT)
		{
			int checkRoom = vec2_index(checkPosition, LEVEL_WIDTH);
			if(newLevel.rooms[checkRoom].active == false)
			{
				newLevel.rooms[currentRoom].doors[checkDir].active = true;
				newLevel.rooms[currentRoom].doors[checkDir].connectedRoom = checkRoom;

				newLevel.rooms[checkRoom] = generate_room(GetRandomValue(0, PREFAB_COUNT - 1), false, false);
				newLevel.rooms[checkRoom].doors[opposite_dir(checkDir)].active = true;
				newLevel.rooms[checkRoom].doors[opposite_dir(checkDir)].connectedRoom = currentRoom;

				currentRoom = checkRoom;
				position = checkPosition;
				roomsCreated[count] = currentRoom;
				count++;
				furthestRoom = currentRoom;
				timeout = 0;
				continue;
			}
		}

		timeout++;

		if (timeout > 30)
		{
			currentRoom = roomsCreated[GetRandomValue(0, count - 1)];
			position = index_vec2(currentRoom, LEVEL_WIDTH);

			timeout = 0;
		}
	}

    if (furthestRoom == startRoom && count > 1) {
        furthestRoom = roomsCreated[count - 1];
    }

	newLevel.rooms[furthestRoom].end = true;

	newLevel.startRoom = startRoom;
	newLevel.endRoom = furthestRoom;

	// add shortcuts
	int numShortcuts = (int)(difficulty * 5);
	for (int i = 0; i < numShortcuts; i++)
	{
		int randId = roomsCreated[GetRandomValue(0, count - 1)];
		Vector2 pos = index_vec2(randId, LEVEL_WIDTH);

		int dir = GetRandomValue(0, 3);
		Vector2 neighborPos = Vector2Add(pos, dir_vec2(dir));

		if (neighborPos.x >= 0 && neighborPos.x < LEVEL_WIDTH && 
			neighborPos.y >= 0 && neighborPos.y < LEVEL_HEIGHT)
		{
			int neighborId = vec2_index(neighborPos, LEVEL_WIDTH);

			if (newLevel.rooms[neighborId].active && 
            	!newLevel.rooms[randId].doors[dir].active)
        	{
            	newLevel.rooms[randId].doors[dir].active = true;
            	newLevel.rooms[randId].doors[dir].connectedRoom = neighborId;

        	    newLevel.rooms[neighborId].doors[opposite_dir(dir)].active = true;
           		newLevel.rooms[neighborId].doors[opposite_dir(dir)].connectedRoom = randId;
        	}
		}
	}

	return newLevel;
}

void generate_dungeon(ng_dungeon* dungeon, int levels)
{
	if (levels > MAX_LEVELS) levels = MAX_LEVELS;
	dungeon->numLevels = levels;

	for (int i = 0; i < levels; i++)
	{
		float difficulty = 1.0f;

		if (levels > 1)
		{
			difficulty = (float)i / (float)(levels - 1);
		}

		dungeon->levels[i] = generate_level(difficulty);
		dungeon->levels[i].id = i;
	}
}