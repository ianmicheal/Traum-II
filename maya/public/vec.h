#pragma once

typedef float Vec[4];

extern Vec vec_forward;
extern Vec vec_side;
extern Vec vec_up;

static inline void
vec_set (float *dst, float x, float y, float z, float w)
{
	dst[0] = x;
	dst[1] = y;
	dst[2] = z;
	dst[3] = w;
}
static inline void
vec_copy (float *dst, float *src)
{
	for (int i = 0; i < 4; i++) dst[i] = src[i];
}
static inline void
vec_negated (float *dst, float *src)
{
	for (int i = 0; i < 4; i++) dst[i] =-src[i];
}
static inline void
vec_add (float *dst, float *a, float *b)
{
	for (int i = 0; i < 4; i++) dst[i] = a[i] + b[i];
}
static inline void
vec_subtract (float *dst, float *a, float *b)
{
	for (int i = 0; i < 4; i++) dst[i] = a[i] - b[i];
}
static inline void
vec_scaled (float *dst, float k, float *src)
{
	for (int i = 0; i < 4; i++) dst[i] = k*src[i];
}
static inline void
vec_multiply (float *dst, float *a, float *b)
{
	for (int i = 0; i < 4; i++) dst[i] = a[i]*b[i];
}
static inline void
vec_mult_add (float *dst, float *a, float k, float *b)
{
	for (int i = 0; i < 4; i++) dst[i] = a[i] + k*b[i];
}
static inline void
vec_lerp (float *dst, float s, float *a, float t, float *b)
{
	for (int i = 0; i < 4; i++) dst[i] = s*a[i] + t*b[i];
}
static inline float
vec_dot3 (float *a, float *b)
{
	return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}
static inline float
vec_dot (float *a, float *b)
{
	return vec_dot3 (a, b) + a[3]*b[3];
}
static inline float
vec_length (float *src)
{
	return sqrt (vec_dot (src, src));
}
int vec_normalised (float *dst, float *src);
