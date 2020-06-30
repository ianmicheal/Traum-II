#include "traum.h"

struct _Assets _assets;
struct _Game_state _gs;

#define MAX_ENTITIES 64
static uint32_t _nents = 0;
static Gfx_entity _ents[MAX_ENTITIES];


static uint32_t _buttons_old = 0;
static uint32_t _buttons = 0;
static Vec _wish;

static Gfx_camera cam;

static int
init (void)
{	/*Load static assets*/
	_assets.clouds = gfx_image ("pic0209.kmg");
	_assets.alt_clouds = gfx_image ("pic0206.kmg");
	_assets.stars = gfx_image ("nightsky.kmg");
	_assets.sonne = gfx_image ("SGLOWx256.kmg");
	_assets.moon = gfx_image ("monglowx128.kmg");
	_assets.level = gfx_model ("level1.level");
	_assets.feuer = gfx_image_animated ("feuer.txt");
	_assets.grass = gfx_image ("grass_wet.kmg");
	_assets.bonfire = gfx_image_animated ("bonfire.txt");
	/*Set up camera*/
	vec_set (cam.position, 0, 0, 0, 0);
	vec_set (cam.rotation, 82, 0, 0, 0);
	/*Initialise subsystems*/
	sky_init ();
	{/*Spawn entities*/
		FILE *fp = sys_fopen ("level1.ents");
		uint32_t i, n, x;
		if (NULL == fp)
		{
			printf ("Could not find entity file\n");
			return 0;
		}
		/*Clear state*/
		_nents = 0;
		/*These should be sanity checked*/
		fread (&x, sizeof (x), 1, fp);
		fread (&x, sizeof (x), 1, fp);
		/*Read and spawn each entity*/
		fread (&n, sizeof (n), 1, fp);
		for (i = 0; i < n; i++)
		{
			Gfx_entity *e;
			char name[64];
			Vec pos = {0, 0, 0 ,0};
			/*Ensure we don't overflow*/
			if (MAX_ENTITIES <= _nents)
			{
				printf ("Too many entities!\n");
				break;
			}
			/*Read entity name*/
			fread (&x, sizeof (x), 1, fp);
			fread (name, 1, x, fp);
			name[x] = '\0';
			printf ("%s\n", name);
			/*Read position*/
			fread (pos, sizeof (float), 3, fp);
			/*Player start has no visual*/
			if (!strcmp ("player_start", name))
			{
				vec_copy (cam.position, pos);
				continue;
			}
			/*Allocate the entity*/
			if (!strcmp ("particle_grass", name))
			{
				e = _ents + _nents;
				gfx_scene_node_init (e);
				vec_copy (e->position, pos);
				e->position[2] += 1;
				e->texture = _assets.grass;
				e->state.caps = SC_BLEND|SC_CULLING|SC_TEXTURE|SC_FOG;
				e->flags = EF_BILLBOARD_YAW|EF_SORTED;
				e->sort = 2;
				e->type = ET_SPRITE;
				_nents++;
			}
			else if (!strcmp ("particle_torch", name))
			{
				e = _ents + _nents;
				gfx_scene_node_init (e);
				vec_copy (e->position, pos);
				e->position[2] += 0.66;
				e->texture = _assets.feuer;
				e->state.caps = SC_BLEND|SC_CULLING|SC_TEXTURE;
				e->state.blend.src = SB_SRC_ALPHA;
				e->state.blend.dst = SB_DST_ALPHA;
				e->flags = EF_BILLBOARD_YAW|EF_SORTED;
				e->sort = 1;
				e->type = ET_SPRITE;
				_nents++;
			
				e = _ents + _nents;
				gfx_scene_node_init (e);
				vec_copy (e->position, pos);
				e->position[2] += 1;
				vec_set (e->scale, 5, 5, 5, 1);
				e->colour[0] = 0xff;
				e->colour[1] = 0x80;
				e->colour[2] = 0x3f;
				e->colour[3] = 0x6f;
				e->texture = _assets.sonne;
				e->state.caps = SC_BLEND|SC_CULLING|SC_TEXTURE;
				e->state.blend.src = SB_SRC_ALPHA;
				e->state.blend.dst = SB_DST_ALPHA;
				e->flags = EF_BILLBOARD|EF_SORTED|EF_SCALED;
				e->sort = 0;
				e->type = ET_SPRITE;
				_nents++;
			}
			else if (!strcmp ("particle_bonfire", name))
			{
				e = _ents + _nents;
				gfx_scene_node_init (e);
				vec_copy (e->position, pos);
				e->position[2] += 0.66;
				vec_set (e->scale, 2, 2, 2, 1);
				e->texture = _assets.bonfire;
				e->state.caps = SC_BLEND|SC_CULLING|SC_TEXTURE;
				e->state.blend.src = SB_SRC_ALPHA;
				e->state.blend.dst = SB_DST_ALPHA;
				e->flags = EF_BILLBOARD_YAW|EF_SORTED|EF_SCALED;
				e->sort = 1;
				e->type = ET_SPRITE;
				_nents++;
			
				e = _ents + _nents;
				gfx_scene_node_init (e);
				vec_copy (e->position, pos);
				e->position[2] += 1;
				vec_set (e->scale, 8, 8, 8, 1);
				e->colour[0] = 0xff;
				e->colour[1] = 0xa0;
				e->colour[2] = 0x5f;
				e->colour[3] = 0x6f;
				e->texture = _assets.sonne;
				e->state.caps = SC_BLEND|SC_CULLING|SC_TEXTURE;
				e->state.blend.src = SB_SRC_ALPHA;
				e->state.blend.dst = SB_DST_ALPHA;
				e->flags = EF_BILLBOARD|EF_SORTED|EF_SCALED;
				e->sort = 0;
				e->type = ET_SPRITE;
				_nents++;				
			}
		}
		fclose (fp);
	}
	
	return 0;
}
static void
frame (float dt)
{	/*Bump the game timer*/
	_gs.time += dt;
	
	{/*Move*/
		Input inp;
		
		engine_input (&inp);
		
		_buttons_old = _buttons;
		_buttons = inp.buttons;
	
		uint32_t pressed = _buttons;
		if (pressed&KEY_LEFT) cam.rotation[0] += 60*dt;
		if (pressed&KEY_RIGHT) cam.rotation[0] -= 60*dt;
		cam.rotation[0] -= inp.lx;
		cam.rotation[1] += inp.ly;
		
		_wish[0] = 0;
		if (pressed&KEY_UP) _wish[0] += 40;
		if (pressed&KEY_DOWN) _wish[0] -= 40;
		_wish[1] = 0;
		if (pressed&KEY_A) _wish[1] += 40;
		if (pressed&KEY_B) _wish[1] -= 40;	

		M4x4 mat;
		Vec forward, side;
		m4x4_from_angles (mat, cam.rotation);
		m4x4_forward (forward, mat);
		m4x4_side (side, mat);
		vec_mult_add (cam.position, cam.position, _wish[0]*dt, forward);
		vec_mult_add (cam.position, cam.position, _wish[1]*dt, side);
	}
	
	/*Draw the sky*/
	sky_draw (1000*_gs.time);	

	/*Add world to scene*/
	static Gfx_entity e;
	gfx_scene_node_init (&e);
	e.type = ET_MODEL;
	e.model = _assets.level;
	gfx_scene_node_insert (SG_WORLD, &e);
	
	/*Insert spawned entities into the graph*/
	for (uint32_t i = 0; i < _nents; i++)
	{
		Gfx_entity *e = _ents + i;
		if (e->colour[1] == 0x80)
		{
			float u = 0.15*random_unit ();
			vec_set (e->scale, 5 + u, 5 + u, 5 + u, 1);
		}
		if (e->colour[1] == 0xa0)
		{
			float u = 0.15*random_unit ();
			vec_set (e->scale, 8 + u, 8 + u, 8 + u, 1);
		}
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
int
main (int argc, char **argv)
{
	Game game;
	memset (&game, 0, sizeof (game));
	game.title = "Traum";
	game.init = init;
	game.frame = frame;
	game.shutdown = shutdown;
	if (engine_run (&game, argc, argv))
	{
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
