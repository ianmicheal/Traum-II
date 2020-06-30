#pragma once
#ifndef USING_KOS
#	include <system/gl.h>
#	include <system/glext.h>
#	include <system/gl1bind.h>
#else
#	include <GL/gl.h>
#	include <GL/glext.h>
#endif

#include "public/gfx.h"
#include <system/system.h>

/*Debugging*/
#define DEBUG_MAX_SHAPES	256
#define DEBUG_SPHERE_POINTS	16

#define GFD_POINT	0
#define GFD_LINE	1
#define GFD_SPHERE	2
#define GFD_CAPSULE	3
typedef struct _Gfx_debug_shape
{
	uint32_t col;
	uint32_t type;
	union
	{
		struct _Point
		{
			float xyz[3];
		}pt;
		struct _Line
		{
			float a[3];
			float b[3];
		}ln;
		struct _Sphere
		{
			float xyz[3];
			float radius;
		}sph;
		struct _Capsule
		{
			float head[3];
			float tail[3];
			float radius;
		}cap;
	};
}Gfx_debug_shape;

typedef struct _Gfx_debug
{
	uint32_t nshapes;
	Gfx_debug_shape shapes[DEBUG_MAX_SHAPES];
	uint8_t sph_ndx[2*3*DEBUG_SPHERE_POINTS];
	Vec sph[3*DEBUG_SPHERE_POINTS];
}Gfx_debug;

void gfx_dbg_point (Gfx_debug *self, uint32_t colour, float *origin);
void gfx_dbg_line (Gfx_debug *self, uint32_t colour, float *a, float *b);
void gfx_dbg_sphere (
	Gfx_debug *self,
	uint32_t colour,
	float *origin,
	float radius);
void gfx_dbg_capsule (
	Gfx_debug *self,
	uint32_t colour,
	float *head,
	float *tail,
	float radius);
void gfx_dbg_draw (Gfx_debug *self, M4x4 world);
void gfx_dbg_init (Gfx_debug *self);

/*Images*/
#define IF_ERROR		0x80000000
#define IF_ALPHA		0x01
#define IF_ANIMATED		0x02
typedef struct _Gfx_image
{
	Dlist chain;
	GLuint handle;
	uint32_t width;
	uint32_t height;
	uint32_t format;
	uint32_t refs;
	struct _Animation
	{
		float w, h;
		float fps;
		float length;
		int nframes;
	}ani;
}Gfx_image;

/*Materials*/
typedef struct _Material_anim
{
	float length;
	float w, h;
	float fps;
	int nframes;
}Material_anim;
typedef struct _Material
{
	Dlist chain;
	char name[32];
	char map[32];
	int surface;
	uint32_t refs;
	Material_anim *anim;
	Gfx_image *image;
}Material;

void material_load (Material *self);
void material_unload (Material *self);
Material *material_find (const char *name);

/*Levels*/
#define LEVEL_MAGICK (('R'<<24)|('3'<<16)|('W'<<8)|('S'))
#define LEVEL_VERSION 0x20200317
typedef struct _Level_vertex
{
	float xyz[3];
	float uv[2];
}Level_vertex;
typedef struct _Level_batch
{
	Material *mat;
	uint32_t nverts;
	Level_vertex *verts;
}Level_batch;
typedef struct _Level_mesh
{
	uint32_t nbatches;
	Level_batch *batches;
}Level_mesh;
typedef struct _Level
{
	uint32_t nmeshes;
	Level_mesh *meshes;
}Level;

/*Generic model object*/
enum MT
{
	MT_ERROR = 0,
	MT_LEVEL,
	MT_MAX
};
typedef struct _Gfx_model
{
	Dlist chain;
	uint32_t refs;
	uint32_t type;
	union
	{
		Level lvl;
	};
}Gfx_model;

/*Global graphics state*/
#define MAX_SORT 16
extern struct _Gfx_state
{
	struct _Scene_graphs
	{
		Dlist camera;
		Dlist local;
		Dlist world;
	}sg;
	GLuint bound;
	Gfx_statal defs;
	Gfx_statal curs;
	uint32_t active_sort;
	Dlist sorted[MAX_SORT];
	Dlist images;
	Dlist models;
	Dlist materials;
	float time;
	
	Gfx_debug dbg;
}_gfx;

Gfx_handle gfx_image (const char *file);
void gfx_image_acquire (Gfx_handle image);
void gfx_image_release (Gfx_handle image);

void image_free (Gfx_image *image);
void model_free (Gfx_model *model);

void load_modelview (M4x4 matrix);

static inline void
transform_into_system (M4x4 dst, M4x4 system, M4x4 local)
{
	m4x4_multiply (dst, local, system);
}
