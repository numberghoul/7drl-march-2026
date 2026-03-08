#include "game.h"
#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

void populate_rooms(ng_dungeon* dungeon, ng_actor* actors, int* nextId)
{
	for (int l = 0; l < dungeon->numLevels; l++)
	{
		for (int r = 0; r < MAX_ROOMS; r++)
		{
			if(dungeon->levels[l].rooms[r].active || (!dungeon->levels[l].rooms[r].start && l == 0))
			{
				int monsterMax = dungeon->levels[l].rooms[r].difficulty * MAX_ACTORS_ROOM;
    			if (monsterMax < 1) monsterMax = 1;
			
    			int monsterCount = GetRandomValue(0, monsterMax);
			
    			for (int i = 0; i < monsterCount; i++)
    			{
    				dungeon->levels[l].rooms[r].actors[i] = *nextId;

    				int monId = GetRandomValue(0, NUM_MONSTERS - 1);

    				int testTile;
    				Vector2 spawnPoint;
    				int timeout = 0;
    				bool found = false;

    				do
    				{
    					int testTileId = GetRandomValue(0, MAX_TILES - 1);
    					spawnPoint = index_vec2(testTileId, ROOM_WIDTH);
    					testTile = dungeon->levels[l].rooms[r].tiles[testTileId];
    					if(testTile == TILE_FLOOR)
    					{
    						found = true;
    					}

    					timeout++;
    				} while(!found && timeout < 32);
    				if(found)
    					spawn_monster(actors, nextId, monId, spawnPoint.x, spawnPoint.y);
    			}
			}
		}
	}
}

ng_room generate_room(int prefabId, bool start, bool end, float difficulty)
{
    ng_room newRoom = { 0 }; 
    char* prefabData = LoadFileText(TextFormat("assets/rooms/%d.csv", prefabId));
    int tileCount = 0;

    if (prefabData != NULL)
    {
    	for (int i = 0; prefabData[i] != '\0'; i++) {
            if (prefabData[i] == '\n' || prefabData[i] == '\r') {
                prefabData[i] = ',';
            }
        }

        int count = 0;
        // split by comma
        const char **values = TextSplit(prefabData, ',', &count);

        for (int i = 0; i < count; i++)
        {
            // Only process if the string isn't empty (handles trailing commas/newlines)
            if (values[i][0] != '\0' && tileCount < MAX_TILES)
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

    for (int i = 0; i < MAX_ACTORS_ROOM; i++)
    {
    	newRoom.actors[i] = -1;
    }

    newRoom.difficulty = difficulty;

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
	newLevel.rooms[currentRoom] = generate_room(ROOM_START, true, false, difficulty);
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

				newLevel.rooms[checkRoom] = generate_room(GetRandomValue(0, PREFAB_COUNT - 1), false, false, difficulty);
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

    ng_room endCopy = newLevel.rooms[furthestRoom];
    newLevel.rooms[furthestRoom] = generate_room(ROOM_END, false, true, difficulty);

    for (int i = 0; i < 4; i++)
    	newLevel.rooms[furthestRoom].doors[i] = endCopy.doors[i];

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

	// update tilemaps for doors
	for(int i = 0; i < MAX_ROOMS; i++)
	{
		if(newLevel.rooms[i].active)
		{
			if (newLevel.rooms[i].doors[DIR_NORTH].active)
			{
				newLevel.rooms[i].tiles[coord_index(5, 0, LEVEL_WIDTH)] = TILE_DOORV_TL;
				newLevel.rooms[i].tiles[coord_index(6, 0, LEVEL_WIDTH)] = TILE_DOORV_TR;
			}
			if (newLevel.rooms[i].doors[DIR_SOUTH].active)
			{
				newLevel.rooms[i].tiles[coord_index(5, 8, LEVEL_WIDTH)] = TILE_DOORV_BL;
				newLevel.rooms[i].tiles[coord_index(6, 8, LEVEL_WIDTH)] = TILE_DOORV_BR;
			}
			if (newLevel.rooms[i].doors[DIR_WEST].active)
			{
				newLevel.rooms[i].tiles[coord_index(0, 3, LEVEL_WIDTH)] = TILE_DOORH_LT;
				newLevel.rooms[i].tiles[coord_index(0, 4, LEVEL_WIDTH)] = TILE_DOORH_LC;
				newLevel.rooms[i].tiles[coord_index(0, 5, LEVEL_WIDTH)] = TILE_DOORH_LB;
			}
			if (newLevel.rooms[i].doors[DIR_EAST].active)
			{
				newLevel.rooms[i].tiles[coord_index(11, 3, LEVEL_WIDTH)] = TILE_DOORH_RT;
				newLevel.rooms[i].tiles[coord_index(11, 4, LEVEL_WIDTH)] = TILE_DOORH_RC;
				newLevel.rooms[i].tiles[coord_index(11, 5, LEVEL_WIDTH)] = TILE_DOORH_RB;
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

	// Regenerate floor 0 start room to have no stairs
	ng_room startCopy = dungeon->levels[0].rooms[dungeon->levels[0].startRoom];
	dungeon->levels[0].rooms[dungeon->levels[0].startRoom] = generate_room(0, true, false, 0.0f);
	dungeon->levels[0].rooms[dungeon->levels[0].startRoom].visible = true;

	for (int i = 0; i < 4; i++)
	{
    	dungeon->levels[0].rooms[dungeon->levels[0].startRoom].doors[i] = startCopy.doors[i];
	}

    if (dungeon->levels[0].rooms[dungeon->levels[0].startRoom].doors[DIR_NORTH].active)
	{
		dungeon->levels[0].rooms[dungeon->levels[0].startRoom].tiles[coord_index(5, 0, LEVEL_WIDTH)] = TILE_DOORV_TL;
		dungeon->levels[0].rooms[dungeon->levels[0].startRoom].tiles[coord_index(6, 0, LEVEL_WIDTH)] = TILE_DOORV_TR;
	}
	if (dungeon->levels[0].rooms[dungeon->levels[0].startRoom].doors[DIR_SOUTH].active)
	{
		dungeon->levels[0].rooms[dungeon->levels[0].startRoom].tiles[coord_index(5, 8, LEVEL_WIDTH)] = TILE_DOORV_BL;
		dungeon->levels[0].rooms[dungeon->levels[0].startRoom].tiles[coord_index(6, 8, LEVEL_WIDTH)] = TILE_DOORV_BR;
	}
	if (dungeon->levels[0].rooms[dungeon->levels[0].startRoom].doors[DIR_WEST].active)
	{
		dungeon->levels[0].rooms[dungeon->levels[0].startRoom].tiles[coord_index(0, 3, LEVEL_WIDTH)] = TILE_DOORH_LT;
		dungeon->levels[0].rooms[dungeon->levels[0].startRoom].tiles[coord_index(0, 4, LEVEL_WIDTH)] = TILE_DOORH_LC;
		dungeon->levels[0].rooms[dungeon->levels[0].startRoom].tiles[coord_index(0, 5, LEVEL_WIDTH)] = TILE_DOORH_LB;
	}
	if (dungeon->levels[0].rooms[dungeon->levels[0].startRoom].doors[DIR_EAST].active)
	{
		dungeon->levels[0].rooms[dungeon->levels[0].startRoom].tiles[coord_index(11, 3, LEVEL_WIDTH)] = TILE_DOORH_RT;
		dungeon->levels[0].rooms[dungeon->levels[0].startRoom].tiles[coord_index(11, 4, LEVEL_WIDTH)] = TILE_DOORH_RC;
		dungeon->levels[0].rooms[dungeon->levels[0].startRoom].tiles[coord_index(11, 5, LEVEL_WIDTH)] = TILE_DOORH_RB;
	}
}