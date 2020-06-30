#pragma once

typedef struct _TM_bone
{
	float origin[3];
	uint32_t parent;
}TM_bone;
typedef struct _TM_point
{
	float xyz[3];
	float weight;
}TM_point;
typedef struct _TM_vertex
{
	float normal[4];
	float uv[2];
	uint16_t start, count;
	uint16_t bones[2];
}TM_vertex;
typedef struct _TM_mesh
{
	uint32_t material;
	uint32_t npoints;
	uint32_t nverts;
	uint32_t nindices;
}TM_mesh;
typedef struct _TM_model
{
	uint32_t magick;
	uint32_t version;
	
	uint32_t animset;
	uint32_t nbones;
	float distal;
	
	uint32_t nmaterials;
	uint32_t nmeshes;
	uint32_t resv;
}TM_model;

typedef struct _TM_anim_key
{
	unsigned yaw:10;
	unsigned pitch:10;
	unsigned roll:10;
	unsigned pad:2;
}TM_anim_key;
typedef struct _TM_anim
{
	uint32_t magick;
	uint32_t verison;
	
	uint32_t animset;
	uint32_t nbones;
	uint32_t nframes;
	float framerate;
}TM_anim;

