#pragma once
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

/*Static assets*/
extern struct _Assets
{
	Gfx_handle clouds;
	Gfx_handle alt_clouds;
	Gfx_handle stars;
	Gfx_handle sonne;
	Gfx_handle moon;
	Gfx_handle level;
	Gfx_handle feuer;
	Gfx_handle grass;
	Gfx_handle bonfire;
}_assets;

/*Sky and weather system*/
void sky_draw (unsigned time);
void sky_init (void);
