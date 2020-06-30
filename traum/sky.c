#include "traum.h"

#define HOUR_LENGTH 5000
#define DAY_LENGTH (24*HOUR_LENGTH)

#define SKY_RINGS 6
#define SKY_SEGMENTS 8
#define SKY_NUM_VERTS (SKY_RINGS*SKY_SEGMENTS)

typedef struct _Sky_vertex
{
	Vec xyz;
	uint8_t clr[4];
}Sky_vertex;

static uint8_t _sky_zenith[] = {
	  5,   0,  30, 0x04,
	  5,   0,  30, 0x08,
	  5,   0,  30, 0x00,
	 60,  90, 120, 0x40,
	 60,  90, 120, 0x80,
	110, 180, 220, 0xc0,
	110, 180, 220, 0xff,
	 90, 160, 210, 0xff,
	 70, 140, 190, 0xff,
	 40, 130, 180, 0xff,
	  0,  90, 150, 0xff,
	 20, 110, 180, 0xff,
	 30, 120, 190, 0xff,
	 40, 130, 205, 0xff,
	 50, 140, 220, 0xff,
	 90, 140, 220, 0xff,
	110, 140, 220, 0xff,
	 90,  70, 140, 0xff,
	 90,  70, 140, 0xc0,
	 20,   0,  20, 0x80,
	 20,   0,  20, 0x40,
	  0,   0,  10, 0x10,
	  0,   0,  10, 0x08,
	  0,   0,  10, 0x04
};
static uint8_t _sky_horizon[] = {
	 10,  30,  70, 0x04,
	 10,  30,  70, 0x08,
	 10,  30,  70, 0x10,
	120,  85,  90, 0x40,
	120,  85,  90, 0x80,
	230, 140, 110, 0xc0,
	200, 150, 130, 0xff,
	170, 160, 150, 0xff,
	140, 160, 180, 0xff,
	110, 170, 200, 0xff,
	 80, 180, 220, 0xff,
	115, 180, 200, 0xff,
	150, 180, 180, 0xff,
	190, 180, 165, 0xff,
	220, 180, 150, 0xff,
	255, 180, 130, 0xff,
	255, 180, 130, 0xff,
	250, 140,  40, 0xff,
	250, 140,  40, 0xc0,
	150,  40,   0, 0x80,
	150,  40,   0, 0x40,
	  5,   0,  30, 0x10,
	  5,   0,  30, 0x08,
	  5,   0,  30, 0x04
};
static Sky_vertex _atmos[SKY_SEGMENTS + 2];
static uint8_t _atmos_indices[2][SKY_SEGMENTS + 2];
static Vec _atmos_uv;
static Gfx_user_index _atmos_user_indices[] =
{
	{GFX_TRIFAN, SKY_SEGMENTS + 2, _atmos_indices[0]},
	{GFX_TRIFAN, SKY_SEGMENTS + 2, _atmos_indices[1]}
};
static Gfx_user _atmos_mdl =
{
	sizeof (Sky_vertex),
	VB_XYZ|VB_COLOUR,
	GFX_NONE,
	SKY_SEGMENTS + 2,
	2,
	_atmos_user_indices,
	_atmos,
	NULL
};

static uint32_t _wind_time = 0;
static Vec _wind_stratos[2];
static Vec _wind_atmos[2];

static Vec _fog_colour;
static float _time;

static Gfx_entity atmos, sun, moon;
static Gfx_entity lo, clouds, stars;


/*Procedural skydome*/
static int
sky_render (Gfx_user *sky)
{
	Sky_vertex *verts = sky->verts;
	uint8_t alpha = 0x1f;
	if (&clouds.user == sky)
	{
		alpha = 0x7f;
	}
	else if (&stars.user == sky)
	{
		float scale = 1 - sin (DEG2RAD(180*_time));
		if (scale <= 0)
		{/*No need to draw*/
			return 1;
		}
		alpha = 0x3f*(scale*scale);
	}
	for (uint32_t i = 0; i < SKY_NUM_VERTS; i++)
	{
		uint32_t intensity = (i < sky->nverts - SKY_SEGMENTS)<<8;
		for (uint32_t j = 0; j < 3; j++)
		{
			verts[i].clr[j] = (alpha*intensity)>>8;
		}
	}
	return 0;
}
static Sky_vertex _sky[SKY_NUM_VERTS];
static uint8_t _sky_indices[SKY_RINGS][2*SKY_SEGMENTS + 2];
static Vec _sky_uv;
static Gfx_user_index _sky_user_indices[SKY_RINGS];
static Gfx_user _sky_mdl =
{
	sizeof (Sky_vertex),
	VB_XYZ|VB_UV_XYZ|VB_COLOUR,
	GFX_NONE,
	SKY_NUM_VERTS,
	SKY_RINGS/2 + 1,
	_sky_user_indices,
	_sky,
	sky_render,
};


static void
blend (uint8_t *dst, uint8_t *a, float t, uint8_t *b)
{	
	uint8_t bc0[4];
	uint8_t bc1[4];
	uint32_t ai = a[3] + 1;
	uint32_t bi = b[3] + 1;
	float s = 1.0 - t;
	/*Prescale the colours by their intensities
	the addition by one preserves the scale of the value in the division
	the subtraction makes the result fit within 8 bits*/
	for (int i = 0; i < 3; i++)
	{
		uint32_t x = (ai*(a[i] + 1))>>8;
		uint32_t y = (bi*(b[i] + 1))>>8;
		bc0[i] = (uint8_t)(x - (x != 0));
		bc1[i] = (uint8_t)(y - (y != 0));
	}
	/*Lerp between the two colours*/
	for (int i = 0; i < 3; i++)
	{
		dst[i] = (uint8_t)(s*bc0[i] + t*bc1[i]);
	}
}
static void
wind_direction (Vec dst, float t, Vec src[2])
{
	for (uint32_t i = 0; i < 3; i++)
	{
		dst[i] = src[0][i] + t*(src[1][i] - src[0][i]);
	}
	dst[3] = 0;
}
void
sky_draw (unsigned time)
{
	int index0, index1;
	int index2, index3;
	Vec wa, ws;
	float frac;
	int i;
	/*Figure out the index*/
	time %= DAY_LENGTH;
	_time = (float)time/DAY_LENGTH;
	index0 = time/HOUR_LENGTH;
	index1 = (index0 + 1)%24;
	index2 = (index0 + 12)%24;
	index3 = (index1 + 12)%24;
	index0 *= 4; index1 *= 4; index2 *= 4; index3 *= 4;
	frac = (time%HOUR_LENGTH)/(float)HOUR_LENGTH;
	/*Update wind periodically
	NB: this is probably not the best place for this, but it's okay for now*/
	if (time - _wind_time > HOUR_LENGTH)
	{
		float s, c, k;
		/*Compute new directions*/
		vec_copy (_wind_stratos[0], _wind_stratos[1]);
		k = _wind_stratos[0][2] + _wind_stratos[1][2];
		_wind_stratos[1][3] += 8*random_unit ();
		sincosf (DEG2RAD(_wind_stratos[1][3]), &s, &c);
		_wind_stratos[1][0] = c;
		_wind_stratos[1][1] = s;
		_wind_stratos[1][2] = CLAMP(
			0.33*(k + 0.5 + (xorshift32 ()&0x7f)/255.0),
			0.25, 1
		);
		vec_copy (_wind_atmos[0], _wind_atmos[1]);
		k = _wind_atmos[0][2] + _wind_atmos[1][2];
		_wind_atmos[1][3] += 8*random_unit ();
		sincosf (DEG2RAD(_wind_atmos[1][3]), &s, &c);
		_wind_atmos[1][0] = c;
		_wind_atmos[1][1] = s;
		_wind_atmos[1][2] = CLAMP(
			0.33*(k + 0.5 + (xorshift32 ()&0x7f)/255.0),
			0.25, 1
		);
		_wind_time = time;
	}
	{/*Interpolate wind parametrics for this moment*/
		Vec tmp;
		float lerp = (time%(HOUR_LENGTH))/(1.0*HOUR_LENGTH);
		wind_direction (wa, lerp, _wind_atmos);
		wind_direction (ws, lerp, _wind_stratos);
		vec_set (tmp, wa[2]*wa[0]/1e3, wa[2]*wa[1]/1e3, 0, 0);
		vec_add (_atmos_uv, tmp, _atmos_uv);
		vec_set (tmp, ws[2]*ws[0]/1e3, ws[2]*ws[1]/1e3, 0, 0);
		vec_add (_sky_uv, tmp, _sky_uv);
	}
	/*Blend together the zenith and nadir colours*/
	blend (_atmos[0].clr, _sky_zenith + index0, frac, _sky_zenith + index1);
	blend (_atmos[1].clr, _sky_zenith + index2, frac, _sky_zenith + index3);
	/*Now do the same for the horizon*/
	uint8_t *from = _sky_horizon + index0;
	uint8_t *to = _sky_horizon + index1;
	for (i = 2; i < SKY_SEGMENTS + 2; i++)
	{
		blend (_atmos[i].clr, from, frac, to);
	}
	/*Average nadir and horizon for fog*/
	for (i = 0; i < 3; i++)
	{
		_fog_colour[i] = (_atmos[1].clr[i] + _atmos[2].clr[i])/511.0;
	}
	gfx_scene_fog_colour (_fog_colour[0], _fog_colour[1], _fog_colour[2]);
	

	gfx_scene_node_init (&atmos);
	vec_set (atmos.scale, 100, 100, 100, 0);
	atmos.state.caps = SC_BLEND|SC_CULLING|SC_TEXTURE;
	atmos.flags = EF_SCALED|EF_SORTED;
	atmos.sort = 10;
	atmos.type = ET_USER;
	atmos.user = _atmos_mdl;
	gfx_scene_node_insert (SG_LOCAL, &atmos);	
	
	M4x4 tform, spin;
	Vec angles, offset, delta;
	vec_set (angles, 0, 360*((6*HOUR_LENGTH + time)/(float)DAY_LENGTH), 0, 0);
	m4x4_from_angles (tform, angles);
	
	gfx_scene_node_init (&moon);
	/*Have the moon orbit the planet. This just looks too cool*/
	const uint32_t period = 7*DAY_LENGTH;
	uint32_t timed = (uint32_t)(1000*_gs.time)%period;
	vec_set (angles, 360*(float)timed/period, 0, 0, 0);
	m4x4_from_angles (spin, angles);
	vec_set (delta,16, 0, 2, 0);
	m4x4_multiply_vector (offset, tform, delta);
	m4x4_multiply_vector (moon.position, spin, offset);
	
	moon.state.caps = SC_BLEND|SC_CULLING|SC_TEXTURE;
	moon.flags = EF_BILLBOARD|EF_SORTED;
	moon.sort = 9;
	moon.type = ET_SPRITE;
	moon.texture = _assets.moon;
	gfx_scene_node_insert (SG_LOCAL, &moon);
	
	gfx_scene_node_init (&sun);
	vec_set (offset,-5, 0, 0, 0);
	m4x4_multiply_vector (sun.position, tform, offset);
	sun.state.caps = SC_BLEND|SC_CULLING|SC_TEXTURE;
	sun.state.blend.src = SB_SRC_ALPHA;
	sun.state.blend.dst = SB_DST_ALPHA;
	sun.flags = EF_BILLBOARD|EF_SORTED;
	sun.sort = 8;
	sun.type = ET_SPRITE;
	sun.texture = _assets.sonne;
	gfx_scene_node_insert (SG_LOCAL, &sun);

	gfx_scene_node_init (&stars);
	vec_set (stars.position, 0, 0, 0, 0);
	vec_set (stars.scale, 10, 10, 1, 0);
	stars.state.caps = SC_BLEND|SC_CULLING|SC_TEXTURE;
	stars.state.blend.src = SB_SRC_ALPHA;
	stars.state.blend.dst = SB_DST_ALPHA;
	stars.flags = EF_SORTED|EF_SCALED;
	stars.tm.enabled = true;
	stars.tm.scale[0] = 4;
	stars.tm.scale[1] = 4;
	stars.sort = 7;
	stars.user = _sky_mdl;
	stars.type = ET_USER;
	stars.texture = _assets.stars;
	gfx_scene_node_insert (SG_LOCAL, &stars);
	
	gfx_scene_node_init (&lo);
	vec_set (lo.position, 0, 0, 0, 0);
	vec_set (lo.scale, 15, 15, 1, 0);
	lo.state.caps = SC_BLEND|SC_CULLING|SC_TEXTURE;
	lo.state.blend.src = SB_SRC_ALPHA;
	lo.state.blend.dst = SB_DST_ALPHA;
	lo.tm.enabled = true;
	lo.tm.scroll[0] = _sky_uv[0];
	lo.tm.scroll[1] = _sky_uv[1];
	lo.tm.scale[0] = 2;
	lo.tm.scale[1] = 2;
	lo.flags = EF_SORTED|EF_SCALED;
	lo.sort = 6;
	lo.user = _sky_mdl;
	lo.type = ET_USER;
	lo.texture = _assets.alt_clouds;
	gfx_scene_node_insert (SG_LOCAL, &lo);
	
	gfx_scene_node_init (&clouds);
	vec_set (clouds.position, 0, 0, 1.5, 0);
	vec_set (clouds.scale, 20, 20, 1, 0);
	clouds.state.caps = SC_BLEND|SC_CULLING|SC_TEXTURE;
	clouds.state.blend.src = SB_SRC_ALPHA;
	clouds.state.blend.dst = SB_DST_ALPHA;
	clouds.tm.enabled = true;
	clouds.tm.scroll[0] = _atmos_uv[0];
	clouds.tm.scroll[1] = _atmos_uv[1];
	clouds.tm.scale[0] = 1;
	clouds.tm.scale[1] = 1;
	clouds.tm.angle = 45;
	clouds.flags = EF_SORTED|EF_SCALED;
	clouds.sort = 5;
	clouds.user = _sky_mdl;
	clouds.type = ET_USER;
	clouds.texture = _assets.clouds;
	gfx_scene_node_insert (SG_LOCAL, &clouds);
}

void
sky_init (void)
{
	const float ra = 180.0/SKY_RINGS;
	const float rb = 360.0/SKY_SEGMENTS;
	{/*Generate atmospheric volume*/
		uint32_t i, j = 2;
		float ts, tc;
		/*Add the zenith and nadir points*/
		vec_set (_atmos[0].xyz, 0, 0, 0.5, 0);
		vec_set (_atmos[1].xyz, 0, 0,-0.5, 0);
		/*Generate horizon ring*/
		sincosf (DEG2RAD(ra), &ts, &tc);
		for (i = 0; i < SKY_SEGMENTS; i++)
		{
			float ps, pc;
			sincosf (DEG2RAD(rb*i), &ps, &pc);
			vec_set (_atmos[j++].xyz, ts*pc, ts*ps, 0, 0);
		}
		/*Generate the caps*/
		_atmos_indices[0][0] = 0;
		_atmos_indices[1][0] = 1;
		for (i = 0; i < SKY_SEGMENTS; i++)
		{
			_atmos_indices[0][i + 1] = SKY_SEGMENTS - i + 2;
			_atmos_indices[1][i + 1] = i + 2;
		}
		_atmos_indices[0][SKY_SEGMENTS + 1] = SKY_SEGMENTS + 1;
		_atmos_indices[1][SKY_SEGMENTS + 1] = 2;
	}
	{/*Generate skydome*/
		uint32_t i, j, k = 1;
		vec_set (_sky[0].xyz, 0, 0, 1, 0);
		for (i = 1; i <= SKY_RINGS/2; i++)
		{
			uint32_t start = k;
			float ts, tc;
			sincosf (DEG2RAD(ra*i), &ts, &tc);
			for (j = 0; j < SKY_SEGMENTS; j++)
			{
				float ps, pc;
				sincosf (DEG2RAD(rb*j), &ps, &pc);
				/*Compute vertex position*/
				vec_set (_sky[k].xyz, ts*pc, ts*ps, tc, 0);
				/*Compute strip indices*/
				_sky_indices[i][2*j + 0] = start + j;
				_sky_indices[i][2*j + 1] = SKY_SEGMENTS + start + j;
				k++;
			}
			_sky_mdl.nverts = k;
			
			/*Finish the strip*/
			_sky_indices[i][2*j + 0] = start;
			_sky_indices[i][2*j + 1] = SKY_SEGMENTS + start;
			
			_sky_user_indices[i].mode = GFX_TRISTRIP;
			_sky_user_indices[i].length = 2*SKY_SEGMENTS + 2;
			_sky_user_indices[i].data = _sky_indices[i];
		}
		/*Generate the cap*/
		_sky_indices[0][0] = 0;
		for (i = 0; i < SKY_SEGMENTS; i++) _sky_indices[0][i + 1] = i + 1;
		_sky_indices[0][SKY_SEGMENTS + 1] = 1;
		
		_sky_user_indices[0].mode = GFX_TRIFAN;
		_sky_user_indices[0].length = SKY_SEGMENTS + 2;
		_sky_user_indices[0].data = _sky_indices[0];
	}
	{/*Initialise the wind*/
		float s, c, th = 180*random_unit ();
		sincosf (DEG2RAD(th), &s, &c);
		for (int i = 0; i < 2; i++)
		{
			_wind_atmos[i][0] = c;
			_wind_stratos[i][0] = c;
			_wind_atmos[i][1] = s;
			_wind_stratos[i][1] = s;
			_wind_atmos[i][2] = 1;
			_wind_stratos[i][2] = 0.7;
			_wind_atmos[i][3] = th;
			_wind_stratos[i][3] = th;
		}
		vec_set (_sky_uv, 0, 0, 0, 0);
		vec_set (_atmos_uv, 0, 0, 0, 0);
	}
}

