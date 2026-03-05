#include "game.h"
#include "raylib.h"
#include "string.h"

ng_room generate_room(int prefabId)
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
            // clean the string, replace newlines with nothing
            // This turns "\n17" into "17"
            const char* cleanValue = TextReplace(values[i], "\n", "");
            cleanValue = TextReplace(cleanValue, "\r", ""); // Handle Windows line endings

            // convert and store
            if (tileCount < MAX_TILES && TextLength(cleanValue) > 0)
            {
                newRoom.tiles[tileCount] = TextToInteger(cleanValue);
                tileCount++;
            }
        }
        UnloadFileText(prefabData);
    }

    return newRoom;
}


ng_level generate_level(float difficulty)
{
	ng_level newLevel;

	return newLevel;
}

ng_dungeon generate_dungeon(int levels)
{
	ng_dungeon newDungeon;

	return newDungeon;
}