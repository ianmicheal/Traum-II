#include <SDL2/SDL.h>
#include "public/gl.h"
#include "public/glext.h"
#include "public/system.h"
#include "public/gl1bind.h"

static SDL_Window *_window = NULL;
static SDL_GLContext _glc = NULL;

void
sys_video_gl_swap (void)
{
	SDL_GL_SwapWindow (_window);
}
void
sys_video_gl_enable (void)
{
	SDL_GL_MakeCurrent (_window, _glc);
}
void
sys_video_gl_disable (void)
{
	SDL_GL_MakeCurrent (_window, NULL);
}
int
sys_video_gl_init (const char *title, int width, int height)
{
	SDL_GL_SetAttribute (SDL_GL_DEPTH_SIZE, 24);
    _window = SDL_CreateWindow (
		title,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width,
		height,
		SDL_WINDOW_OPENGL
    );
    if (NULL == _window) 
	{
		return -1;
	}
	_glc = SDL_GL_CreateContext (_window);
	if (NULL == _glc)
	{
		sys_video_gl_shutdown ();
		return -2;
	}
	if (gl1bind_load ())
	{
		sys_video_gl_shutdown ();
		return -3;
	}
	sys_video_gl_enable ();
	/*Enable adaptive vsync*/
	SDL_GL_SetSwapInterval (-1);
	return 0;
}
void
sys_video_gl_shutdown (void)
{
	if (_glc)
	{
		SDL_GL_MakeCurrent (_window, NULL);
		SDL_GL_DeleteContext (_glc);
		_glc = NULL;
	}
	if (_window)
	{
		SDL_DestroyWindow (_window);
		_window = NULL;
	}
}
static inline uint32_t
sym2key (uint32_t key)
{
	uint32_t btn = MAX_KEY;
	switch (key)
	{
	case SDLK_w: btn = KEY_Y; break;
	case SDLK_a: btn = KEY_X; break;
	case SDLK_s: btn = KEY_A; break;
	case SDLK_d: btn = KEY_B; break;
	case SDLK_UP: btn = KEY_UP; break;
	case SDLK_LEFT: btn = KEY_LEFT; break;
	case SDLK_DOWN: btn = KEY_DOWN; break;
	case SDLK_RIGHT: btn = KEY_RIGHT; break;
	case SDLK_SPACE: btn = KEY_START; break;
	default: break;
	}
	return btn;
}

FILE *
sys_fopen (const char *file)
{
	return fopen (file, "rb");
}

/*General interface*/
int
sys_run (Sys_app *app, int argc, char **argv)
{/*SDL should be brought online first, naturally.
In a nicer world each subsystem would be initialised in separate*/
	if (SDL_Init (SDL_INIT_EVERYTHING))
	{
		return -1;
	}
	/*Now let the user do their set up*/
	if (app->init (argc, argv))
	{
		return -2;
	}
	/*The Main Loop (tm)*/
	while (1) 
	{
		static Uint32 last = 0;
		Uint32 now, tick;
		SDL_Event ev;
		while (SDL_PollEvent (&ev)) 
		{/*Events are ignored if the user did not supply a callback here*/
			if (!app->event)
			{
				continue;
			}
			switch (ev.type)
			{
			case SDL_KEYDOWN: {
					size_t btn = sym2key (ev.key.keysym.sym);
					if (btn != MAX_KEY)
					{
						app->event (SE_KEYDOWN, (void *)btn);
					}
				} break;
			case SDL_KEYUP: {
					size_t btn = sym2key (ev.key.keysym.sym);
					if (btn != MAX_KEY)
					{
						app->event (SE_KEYUP, (void *)btn);
					}
				} break;
			case SDL_QUIT:
				goto Shutdown;
			default:
				break;
			}
		}
		{/*Sample mouse*/
			Uint32 buttons;
			Mouse_state state;
			buttons = SDL_GetMouseState (&state.x, &state.y);
			if (buttons&SDL_BUTTON(1))
			{
				SDL_ShowCursor (SDL_DISABLE);
				SDL_CaptureMouse (1);
				SDL_WarpMouseInWindow (_window, 320, 240);
				state.x -= 320; state.y -= 240;
				app->event (SE_MOUSE, &state);
			}
			else
			{
				SDL_ShowCursor (SDL_ENABLE);
				SDL_CaptureMouse (0);
			}
		}
		now = SDL_GetTicks ();
		tick = now - last;
		last = now;
		/*Let the user do their per frame stuff*/
		app->frame ((uint32_t)tick);
	}
Shutdown:
	app->shutdown ();
	SDL_Quit ();
	return 0;
}
