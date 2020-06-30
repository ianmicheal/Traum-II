#pragma once

enum AEV
{
	AEV_NOP = 0,
	AEV_LSTEP,
	AEV_RSTEP,
	MAX_AEV
};
typedef struct _Anim_event
{
	uint32_t frame;
	uint32_t type;
}Anim_event;
typedef struct _Anim_header
{
	uint32_t magick;
	uint32_t version;
	float origin[3];
	float ext[3];
	float radius;
	uint32_t njoints;
	uint32_t nframes;
	uint32_t nevents;
	/*
	uint32_t frames[];
	uint8_t offsets[];
	Anim_event events[];
	*/
}Anim_header;

typedef struct _Vertex
{
	float xyz[3];
	float uv[2];
	float bias;
	uint16_t bones[2];
	uint8_t normal[4];
}Vertex;
typedef struct _Mesh
{
	uint32_t slot;
	uint32_t nverts;
	uint32_t ntris;
	/*
	Vertex verts[];
	uint16_t tris[];
	*/
}Mesh;
typedef struct _Bone
{
	float rotation[3];
	float position[3];
}Bone;
typedef struct _Model_header
{
	uint32_t magick;
	uint32_t version;
	uint32_t nbones;
	uint32_t nmaterials;
	uint32_t nmeshes;
	/*
	Bone bones[];
	String materials[];
	Mesh meshes[];
	*/
}Model_header;

typedef struct _Anim
{
	float origin[3];
	float ext[3];
	float radius;
	uint32_t njoints;
	uint32_t nframes;
	uint32_t *frames;
	uint8_t *offsets;
	uint32_t nevents;
	Anim_event *events;
}Anim;

