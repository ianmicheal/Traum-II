#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <maya/list.h>
#include <maya/math.h>

typedef void *Gfx_handle;

/*Statal object*/
enum SC
{
	SC_NONE				= 0,
	SC_BLEND			= (1<<0),
	SC_DEPTH			= (1<<1),
	SC_CULLING			= (1<<2),
	SC_TEXTURE			= (1<<3),
	SC_FOG				= (1<<4),
	SC_COLOUR			= (1<<16),
	SC_TEXCOORD			= (1<<17),
};
enum SB
{
	SB_ONE = 0,
	SB_ZERO,
	SB_SRC_COLOUR,
	SB_SRC_ALPHA,
	SB_ONE_MINUS_SRC_ALPHA,
	SB_DST_COLOUR,
	SB_DST_ALPHA,
	SB_ONE_MINUS_DST_ALPHA	
};
typedef struct _Gfx_statal
{
	uint32_t caps;
	union
	{
		uint32_t blend_mode;
		struct
		{
			uint16_t src;
			uint16_t dst;
		}blend;
	};
}Gfx_statal;

/*Scene node*/
enum ET
{
	ET_ERROR = 0,
	ET_SPRITE,
	ET_MODEL,
	ET_USER,
	ET_MAX
};
enum EF
{
	EF_NONE				= 0,
	EF_SCALED			= (1<<0),
	EF_BILLBOARD		= (1<<2),
	EF_BILLBOARD_YAW	= (1<<3),
	EF_SORTED			= (1<<4)
};
enum VB
{
	VB_NONE		= 0,
	VB_XYZ		= (1<<0),
	VB_UV		= (1<<1),
	VB_COLOUR	= (1<<2),
	VB_UV_XYZ	= (1<<3)
};
enum GFX
{
	GFX_NONE = -1,
	GFX_TRIANGLES,
	GFX_TRIFAN,
	GFX_TRISTRIP
};
typedef struct _Gfx_user_index
{
	uint32_t mode;
	uint32_t length;
	uint8_t *data;
}Gfx_user_index;
typedef struct _Gfx_user
{
	size_t size;
	uint32_t desc;
	uint32_t mode;
	uint32_t nverts;
	uint32_t nindices;
	Gfx_user_index *indices;
	void *verts;
	int (*render) (struct _Gfx_user *);
}Gfx_user;
typedef struct _Gfx_entity
{
	Dlist chain;
	uint32_t type;
	uint32_t flags;
	uint32_t sort;
	Vec position;
	Vec rotation;
	Vec scale;
	uint8_t colour[4];
	Gfx_statal state;
	/*Texture info*/
	Gfx_handle texture;
	struct
	{
		bool enabled;
		float scroll[2];
		float scale[2];
		float angle;
	}tm;
	/*Model data*/
	union
	{
		Gfx_handle model;
		Gfx_user user;
	};
	int32_t submodel;
}Gfx_entity;

/*Camera*/
typedef struct _Gfx_camera
{
	Vec position;
	Vec rotation;
	float aspect;
	float fovx;
	float fovy;
}Gfx_camera;

void gfx_material_parse (const char *script);
Gfx_handle gfx_material_alloc (const char *name);
void gfx_material_release (Gfx_handle self);

Gfx_handle gfx_model (const char *file);
void gfx_model_acquire (Gfx_handle model);
void gfx_model_release (Gfx_handle model);

enum SG
{
	SG_CAMERA = 0,
	SG_LOCAL,
	SG_WORLD,
	SG_MAX
};
void gfx_scene_node_init (Gfx_entity *);
void gfx_scene_node_insert (uint32_t list, Gfx_entity *entity);
void gfx_scene_fog_colour (float r, float g, float b);

void gfx_dbg_addpoint (uint32_t col, float *origin);
void gfx_dbg_addline (uint32_t col, float *a, float *b);
void gfx_dbg_addsphere (uint32_t col, float *origin, float radius);
void gfx_dbg_addcapsule (uint32_t col, float *a, float *b, float radius);

void gfx_draw (Gfx_camera *, float);
void gfx_init (void);
void gfx_shutdown (void);
