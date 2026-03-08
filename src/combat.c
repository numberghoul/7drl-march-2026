#include "game.h"
#include <math.h>

int get_stat_mod(int stat)
{
	return (stat - 10) / 2;
}

ng_stats generate_stats(int str, int agi, int wis, int xpvalue)
{
	ng_stats stats;

	stats.xp = xpvalue;

	if (str > STAT_MAX) stats.strength = STAT_MAX;
	else stats.strength = str;

	if (agi > STAT_MAX) stats.agility = STAT_MAX;
	else stats.agility = agi;

	if (wis > STAT_MAX) stats.wisdom = STAT_MAX;
	else stats.wisdom = wis;

	stats.level = 1 + floor((double)stats.xp / 100);

	stats.hp = STAT_BASE_HP + floor((((double)str / STAT_MAX) * (stats.level) * 10));

	int strMod = get_stat_mod(str);
	int agiMod = get_stat_mod(agi);
	stats.ac = STAT_BASE_AC + agiMod + (int)floor(strMod * 0.5f);

	stats.speed = STAT_BASE_SPEED * (1.0f + ((float)agi / STAT_MAX));
	
	stats.xpRate = 1 + ((float)wis / STAT_MAX);

	return stats;
}