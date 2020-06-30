#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef DREAMCAST
#	define KR_CACHE_LINE		32
#	define KR_PAGE_SIZE			2048
#else
#	define KR_CACHE_LINE		64
#	define KR_PAGE_SIZE			4096
#endif

#define KR_EPSILON				0.01 /*1 cm*/
#define KR_HALF_EPSILON			(KR_EPSILON/2)

/*Colliders*/
#define KR_RAY		-1
#define KR_SPHERE	0
#define KR_CAPSULE	1
#define KR_LOZENGE	2
#define KR_MESH		3

/*Vector types*/
typedef float Kr_vec3[3];
typedef float Kr_vec4[4];
typedef Kr_vec4 Kr_tform[3];

typedef struct _Kr_collider Kr_collider;
typedef void (*Kr_motor) (Kr_collider *self);

void kr_collider_active (Kr_collider *self, bool flag);

/*Global state*/
#define KR_ERR_NOMEM			0xdeadd00d
#define KR_ERR_BADTEST			0xbaadbaad

typedef void (*Kr_panic) (int error);
typedef int (*Kr_phase) (Kr_collider *a, Kr_collider *b);
typedef struct _Kr_callbacks
{
	Kr_phase broad;
	Kr_phase narrow;
}Kr_callbacks;

typedef struct _Kr_query
{
	Kr_vec3 origin;
	Kr_vec3 direction;
	float distance;
	uint32_t filter;
	Kr_collider *ignore;
}Kr_query;

typedef struct _Kr_result
{
	float fraction;
	Kr_collider *hit;
	uint32_t flags;
	Kr_vec3 normal;
	Kr_vec3 point;
}Kr_result;

int kr_query (Kr_query *query, Kr_result *res);

void kr_step (float dt);
void kr_run (float dt, float fps, uint32_t limit);

const char *kr_error (int error);
void kr_panic (Kr_panic panic);
void kr_callbacks (Kr_callbacks *cb);
void kr_init (void);
void kr_shutdown (void);
