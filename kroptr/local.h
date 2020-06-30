#pragma once

#include <assert.h>
#include <float.h>
#include <stdio.h>
#include "kroptr.h"
#include "array.h"
#include "cache.h"
#include "vec.h"

#define PREFETCH(address) do { int __x = *((int *)(address)); (void)__x; } while (0)

/*Generic collider type*/
typedef struct _Kr_sphere
{
	Kr_vec3 origin;
	float radius;
}Kr_sphere;
typedef struct _Kr_capsule
{
	Kr_vec3 head;
	Kr_vec3 tail;
	float radius;
}Kr_capsule;
typedef struct _Kr_lozenge
{
	Kr_tform tform;
	Kr_vec3 hext;
	float radius;
}Kr_lozenge;

typedef struct _Kr_face
{
	uint16_t flags;
	uint16_t normal;
	uint16_t start;
	uint16_t count;
}Kr_face;
typedef struct _Kr_mesh
{
	uint32_t nverts;
	uint32_t nnormals;
	uint32_t nindices;
	uint32_t nfaces;
	Kr_vec3 *verts;
	Kr_vec3 *normals;
	uint16_t *indices;
	Kr_face *faces;
}Kr_mesh;

Kr_mesh *kr_mesh_from_file (FILE *fp);

#define MAX_CONTACTS 2
typedef struct _Kr_manifold
{
	Kr_vec3 normal;
	uint32_t npoints;
	Kr_vec4 points[MAX_CONTACTS];
	struct _Kr_manifold *next;
	Kr_collider *collider;
	uint32_t flags;
	uint32_t resv;
}Kr_manifold;

#define KR_CF_ACTIVE	0x1
typedef struct _Kr_collider
{	/*Broadphase info, packed into 32 byte cache line*/
	uint32_t cycle;
	uint16_t filter;
	uint16_t group;
	Kr_vec3 centre;
	Kr_vec3 extent;
	/*Narrowphase info in two 32 byte cache lines*/
	Kr_tform tform;
	uint32_t type;
	void *prim;
	void *user;
	Kr_manifold *manifolds;
	/*Physical properties*/
	Kr_motor motor;
	Kr_vec3 velocity;
	Kr_vec3 angular;
	uint32_t flags;
}Kr_collider;

typedef struct _Kr_pair
{
	Kr_collider *a;
	Kr_collider *b;
}Kr_pair;

extern struct _Kroptr
{
	float residual;
	Kr_callbacks cb;
	Kr_panic panic;
	uint32_t cycle;
	/*World state*/
	ARRAY(Kr_collider *) active;
	ARRAY(Kr_collider *) root;
	/*Step state*/
	ARRAY(Kr_pair) pairs;
	/*Object allocators*/
	Cache manifolds;
	Cache colliders;
	Cache spheres;
	Cache capsules;
	Cache lozenges;
	Cache meshes;
}_kr;

void kr_throw (int error);
void kr_activate (Kr_collider *col);
void kr_disable (Kr_collider *col);

/*
Memory management
*/
#define KR_ALIGN(x, y) (((x)+((y)-1))&(~((y)-1)))

static inline void *
kr_alloc (size_t size, uint32_t align)
{
	void *ptr = malloc (KR_ALIGN(size, align));
	if (!ptr)
	{
		assert (0 && "Failed to allocate memory!");
		kr_throw (KR_ERR_NOMEM);
	}
	return ptr;
}
static inline void
kr_free (void *ptr)
{
	free (ptr);
}
