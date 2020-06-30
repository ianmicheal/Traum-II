#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <system/system.h>
#include <engine/engine.h>
#include <engine/gfx.h>

/*Game state*/
extern struct _Game_state
{
	float time;
}_gs;

void game_changelevel (const char *level);
