#include <kos.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "system/system.h"

KOS_INIT_FLAGS(INIT_DEFAULT | INIT_MALLOCSTATS);

/*These are unneeded on the dreamcast*/
int sys_video_gl_init (const char *title, int width, int height) {return 0;}
void sys_video_gl_enable (void) {}
void sys_video_gl_disable (void) {}
void sys_video_gl_shutdown (void) {}

void
sys_video_gl_swap (void)
{
	glutSwapBuffers ();
}

static struct _Cont_state
{
	uint32_t buttons;
	bool connected;
}_cont_state[MAX_CONTROLLERS];

/*Implements the standard soft reset*/
#define QUIT_MASK	(CONT_A|CONT_B|CONT_X|CONT_Y|CONT_START)
static bool _reset;
static void
quit_cb (uint8 addr, uint32 btns)
{
	if ((QUIT_MASK&btns) == btns)
	{
		_reset = true;
	}
}

FILE *
sys_fopen (const char *file)
{
	static char *pref[] = {"/pc", "/rom", "/rd", "/vmu", NULL};
	char path[256];
	FILE *fp = NULL;
	/*Try each location to open the file
	TODO: check for truncation*/
	for (uint32_t i = 0; pref[i] != NULL; i++)
	{
		snprintf (path, sizeof (path), "%s/%s", pref[i], file);
		fp = fopen (path, "rb");
		if (fp)
		{
			return fp;
		}
	}
	/*Could not find the file*/
	return NULL;
}

int
sys_run (Sys_app *app, int argc, char **argv)
{
	uint64_t last = 0;
	glKosInit ();
	last = timer_ms_gettime64 ();
Entry:
	/*Let the user do their set up*/
	if (app->init (argc, argv))
	{
		return -2;
	}
	memset (_cont_state, 0, sizeof (_cont_state));
	_reset = false;
	/*The Main Loop (tm)*/
    while (1)
	{	/*Poll each controller for input*/
		for (uint32_t i = 0; i < MAX_CONTROLLERS; i++)
		{	
			struct _Cont_state *cs = &_cont_state[i];
			maple_device_t *cont = maple_enum_type (i, MAPLE_FUNC_CONTROLLER);
			/*Issue gamepad plug/unplug events*/
			bool status = (cont != NULL);
			if (cs->connected != status)
			{
				uint32_t info = (i<<16)|status;
				app->event (SE_GAMEPAD, (void *)info);
				if (status)
				{/*Install the soft reset callback*/
					memset (cs, 0, sizeof (*cs));
					cont_btn_callback (
						maple_addr (cont->port, cont->unit),
						QUIT_MASK,
						quit_cb
					);
				}
				cs->connected = status;
			}
			/*Ensure the controller is plugged in*/
			if (!cont)
			{
				continue;
			}
			/*Sample the controller's state*/
			cont_state_t *state = (cont_state_t *)maple_dev_status (cont);
			if (!state)
			{
				continue;
			}
			/*Evaluate which buttons were pressed and released this frame,
			then issue appropriate events for each one*/
			uint32_t old = cs->buttons;
			uint32_t btn = state->buttons;
			uint32_t chg = old^btn;
			uint32_t pressed = chg&btn;
			uint32_t released = chg&old;
			for (uint32_t j = 0; j < MAX_KEY; j++)
			{
				uint32_t b = 1<<j;
				uint32_t key = (i<<16)|b;
				if (pressed&b) app->event (SE_KEYDOWN, (void *)key);
				else if (released&b) app->event (SE_KEYUP, (void *)key);
			}
			cs->buttons = btn;
			/*Send axial events*/
			int collocation[MAX_AXIS] = {
				state->ltrig, state->rtrig,
				state->joyx, state->joyy,
				state->joy2x, state->joy2y
			};
			static float scalars[MAX_AXIS] = {
				1.0/255.0, 1.0/255.0,
				1.0/127.0, 1.0/127.0,
				1.0/127.0, 1.0/127.0
			};
			for (uint32_t j = 0; j < MAX_AXIS; j++)
			{
				Axis_state axis = {i, j, collocation[j]};
				app->event (SE_AXIS, (void *)&axis);
			}
		}
		/*Handle the soft reset*/
		if (_reset)
		{
			app->shutdown ();
			goto Entry;
		}
		/*Let the user do their frame stuff*/
		uint64_t now = timer_ms_gettime64 ();
		app->frame ((uint32_t)(now - last));
		last = now;
    }
	return 0;
}
