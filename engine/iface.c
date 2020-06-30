#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "maya/math.h"
#include "system/system.h"
#include "public/engine.h"
#include "public/gfx.h"

static Game *_game = NULL;
static Input _cont[MAX_CONTROLLERS];
static int _mouse_x, _mouse_y;


static int
init (int argc, char **argv)
{
	assert (_game != NULL);
	sys_video_gl_init (_game->title, 640, 480);
	gfx_init ();
	memset (_cont, 0, sizeof (_cont));
	_game->init (argc, argv);
	return 0;
}
static void
frame (uint32_t tick)
{
	float dt = 1e-3*tick;
	assert (_game != NULL);
	_game->frame (dt);
	_mouse_x = 0;
	_mouse_y = 0;
}
static void
shutdown (void)
{
	assert (_game != NULL);
	_game->shutdown ();
	gfx_shutdown ();
	sys_video_gl_shutdown ();
}
static void
event (int type, void *data)
{
	switch (type)
	{
	case SE_KEYDOWN: {
			size_t raw = (size_t)data;
			uint32_t cont = raw>>16;
			uint32_t button = raw&0xffff;
			_cont[cont].buttons |= button;
			break;
		}
	case SE_KEYUP: {
			size_t raw = (size_t)data;
			uint32_t cont = raw>>16;
			uint32_t button = raw&0xffff;
			_cont[cont].buttons &= ~button;
			break;
		}
	case SE_MOUSE: {
			Mouse_state s = *(Mouse_state *)data;
			_mouse_x = 0.1*s.x;
			_mouse_y = 0.1*s.y;
			break;
		}
	case SE_AXIS: {
			Axis_state *axis = (Axis_state *)data;
			switch (axis->axis)
			{
			case AXIS_L1: _cont[axis->cont].lt = axis->value; break;
			case AXIS_R1: _cont[axis->cont].rt = axis->value; break;
			case AXIS_LX: _cont[axis->cont].lx = axis->value; break;
			case AXIS_LY: _cont[axis->cont].ly = axis->value; break;
			case AXIS_RX: _cont[axis->cont].rx = axis->value; break;
			case AXIS_RY: _cont[axis->cont].ry = axis->value; break;
			default:
				break;
			}
			break;
		}
	default:
		break;
	}
}
void
engine_input (Input *res, uint32_t index)
{
	if (MAX_CONTROLLERS <= index)
	{
		assert (0 && "index out of range");
		return;
	}
	memcpy (res, &_cont[index], sizeof (*res));
}
void
engine_input_mouse (int *x, int *y)
{
	assert (x != NULL && y != NULL && "Gave NULL to engine_input_mouse");
	*x = _mouse_x;
	*y = _mouse_y;
}
int
engine_run (Game *game, int argc, char **argv)
{
	Sys_app app;
	/*Store the game in a global for the rest of the program to use*/
	_game = game;
	/*Spin up into the lower system loop*/
	memset (&app, 0, sizeof (app));
	app.init = init;
	app.event = event;
	app.frame = frame;
	app.shutdown = shutdown;
	if (sys_run (&app, argc, argv))
	{
		_game = NULL;
		return -1;
	}
	_game = NULL;
	return 0;
}
