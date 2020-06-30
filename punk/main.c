#include "game.h"

struct _Game_state _gs;

#define MAX_ENTITIES 4096
static uint32_t _nents = 0;
static Gfx_entity _ents[MAX_ENTITIES];


static uint32_t _buttons_old = 0;
static uint32_t _buttons = 0;
static Vec _wish;

static Gfx_camera cam;

static Gfx_handle _level;

static Gfx_entity *
entity_create (void)
{
	if (_nents >= MAX_ENTITIES)
	{/*Should be fatal*/
		return NULL;
	}
	Gfx_entity *e = _ents + _nents++;
	gfx_scene_node_init (e);
	return e;
}

#define VERSION "Traum 2.0"

static int
init (int argc, char **argv)
{
	printf ("%s\n", VERSION);
	printf ("COMPILED: %s %s\n", __DATE__, __TIME__);
	/*Load the materials*/
	FILE *fp = sys_fopen ("materials.txt");
	fseek (fp, 0, SEEK_END);
	size_t len = ftell (fp);
	fseek (fp, 0, SEEK_SET);
	char *script = malloc (len + 1);
	fread (script, 1, len, fp);
	script[len] = '\0';
	fclose (fp);
	gfx_material_parse (script);
	free (script);
	/*Zero out the current level*/
	_level = NULL;
	/*Parse arguements*/
	for (int i = 0; i < argc; i++)
	{
		if (!strcmp ("level", argv[i]))
		{
			game_changelevel (argv[++i]);
			continue;
		}
	}
	if (!_level)
	{
		game_changelevel ("traum.level");
	}
	/*Set up camera*/
	vec_set (cam.position, 0, 0, 0, 0);
	vec_set (cam.rotation, 0, 0, 0, 0);
	return 0;
}
void
axial2dir (int x, int y, int deadzone, float *dx, float *dy, float *scale)
{
#define MAX_RANGE 127
	float xsensitivity = 1.0;
	float ysensitivity = 1.0;
	float X = (float)x;
	float Y = (float)y;
	float dz = (float)deadzone;
	
	X *= xsensitivity;
	if (X >= MAX_RANGE) X = MAX_RANGE;
	else if (X <=-MAX_RANGE) X =-MAX_RANGE;
	
	Y *= ysensitivity;
	if (Y >= MAX_RANGE) Y = MAX_RANGE;
	else if (Y <=-MAX_RANGE) Y =-MAX_RANGE;
	
	float m = sqrt (X*X + Y*Y);
	if (dz < m)
	{
		if (MAX_RANGE < m)
		{
			m = MAX_RANGE;
		}
		*scale = (m - dz)/(MAX_RANGE - dz);
		*dx = X/m;
		*dy = Y/m;
	}
	else *dx = *dy = *scale = 0;
#undef MAX_RANGE
}
static void
frame (float dt)
{	/*Bump the game timer*/
	_gs.time += dt;
	{/*Move*/
		Input inp;
		static float speed = 120;
		
		engine_input (&inp, 0);
		
		_buttons_old = _buttons;
		_buttons = inp.buttons;
	
		uint32_t pressed = _buttons;
		
#ifdef USING_KOS
		//printf ("%i [%.2f %.2f] (%.2f %.2f)\n", inp.buttons, inp.lt, inp.rt, inp.lx, inp.ly);
		_wish[0] = _wish[1] = 0;
		if (0 < inp.lt)
		{
			float x, y, scale;
			axial2dir (inp.lx, inp.ly, 31, &x, &y, &scale); 
			_wish[0] = -20*y*scale;
			_wish[1] = -20*x*scale;
		}
		else if (0 < inp.rt)
		{
			cam.rotation[0] -= 0.02*inp.lx;
			cam.rotation[1] -= 0.02*inp.ly;
		}
		else
		{
			float x, y, scale;
			axial2dir (inp.lx, inp.ly, 31, &x, &y, &scale); 
			_wish[0] = -20*y*scale;
			cam.rotation[0] -= 2*scale*x;
		}
#else	
		int mx, my;
		if (pressed&KEY_LEFT) cam.rotation[0] += speed*dt;
		if (pressed&KEY_RIGHT) cam.rotation[0] -= speed*dt;
		
		engine_input_mouse (&mx, &my);
		cam.rotation[0] -= mx;
		cam.rotation[1] += my;
		
		_wish[0] = 0;
		if (pressed&KEY_Y) _wish[0] += speed;
		if (pressed&KEY_A) _wish[0] -= speed;
		_wish[1] = 0;
		if (pressed&KEY_X) _wish[1] += speed;
		if (pressed&KEY_B) _wish[1] -= speed;
#endif		
		if (pressed&KEY_UP) speed += 20;
		if (pressed&KEY_DOWN) speed -= 20;

		

		M4x4 mat;
		Vec forward, side;
		m4x4_from_angles (mat, cam.rotation);
		m4x4_forward (forward, mat);
		m4x4_side (side, mat);
		vec_mult_add (cam.position, cam.position, _wish[0]*dt, forward);
		vec_mult_add (cam.position, cam.position, _wish[1]*dt, side);
	}
	
	Vec p1 = {12, 0, 0, 0};
	gfx_dbg_addpoint (0xffff00ff, p1);
	Vec p2 = {16, 0, 0, 0};
	Vec p3 = {20, 0, 0, 0};
	gfx_dbg_addline (0xff00ffff, p2, p3);
	Vec p4 = {25, 0, 0, 0};
	gfx_dbg_addsphere (0x00ff00ff, p4, 4);
	
	/*Draw scene*/
	for (uint32_t i = 0; i < _nents; i++)
	{
		Gfx_entity *e = _ents + i;
		gfx_scene_node_insert (SG_WORLD, e);
	}
	cam.aspect = 4.0/3.0;
	cam.fovy = 90;
	cam.fovx = cam.fovy/cam.aspect;
	gfx_draw (&cam, dt);
	sys_video_gl_swap ();
}
static void
shutdown (void)
{
}
static void
load_entities (FILE *fp)
{
	
}
void
game_changelevel (const char *level)
{	/*Load the geometry*/
	Gfx_handle l = gfx_model (level);
	if (!l)
	{
		printf ("%s cannot be found\n", level);
		return;
	}
	printf ("loading map %s...\n", level);
	/*Release old map and acquire reference to the new one*/
	if (_level) gfx_model_release (_level);
	gfx_model_acquire (l);
	_level = l;
	_nents = 0;
	/*This is kind of silly, I guess*/
	FILE *fp = sys_fopen (level);
	if (!fp)
	{
		return;
	}
	Level_header lvl;
	uint32_t num;
	fread (&lvl, 1, sizeof (lvl), fp);
	/*Create scene graph*/
	fseek (fp, lvl.scene, SEEK_SET);
	fread (&num, 1, sizeof (num), fp);
	for (uint32_t i = 0; i < num; i++)
	{
		int32_t id;
		float t[3];
		float r[3];
		float s[3];
		/*Read out node data*/
		fread (&id, 1, sizeof (id), fp);
		fread (&t[0], 1, 12, fp);
		fread (&r[0], 1, 12, fp);
		fread (&s[0], 1, 12, fp);
		/*Create a new entity for it*/
		Gfx_entity *e = entity_create ();
		vec_set (e->position, t[0], t[1], t[2], 0);
		vec_set (e->rotation, r[0], r[1], r[2], 0);
		vec_set (e->scale, s[0], s[1], s[2], 1);
		e->type = ET_MODEL;
		e->flags = EF_SCALED;
		e->model = _level;
		e->submodel = id;
	}
	/*Load entities*/
	fseek (fp, lvl.entities, SEEK_SET);
	load_entities (fp);
	fclose (fp);
}

int
main (int argc, char **argv)
{
	Game game;
	memset (&game, 0, sizeof (game));
	game.title = "Distopia";
	game.init = init;
	game.frame = frame;
	game.shutdown = shutdown;
	if (engine_run (&game, argc, argv))
	{
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
