#pragma once
#include <stdint.h>
#include <string.h>
#include <math.h>
/*Not defined in C99, but not sure about C11*/
#undef M_PI
#define M_PI 3.14159265359
#undef HALF_PI
#define HALF_PI 1.570796326795
/*min/max macros*/
#undef MIN
#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#undef MAX
#define MAX(x,y) (((x) > (y)) ? (x) : (y))
/*Degrees <-> Radians*/
#define DEG2RAD(x) ((M_PI/180)*(x))
#define RAD2DEG(x) ((180/M_PI)*(x))
#define DEG2RAD_HALF(x) ((M_PI/360)*(x))
#define RAD2DEG_HALF(x) ((360/M_PI)*(x))
/*Extra types*/
#ifndef MATH_HALF_FLT
#define MATH_HALF_FLT
typedef unsigned short half;
#endif
/*Random numbers*/
extern uint32_t maya_seed;
uint32_t xorshift32 (void);
static inline float
random_unit (void)
{
	return 2*((xorshift32 ()&0xffff)/65535.0 - 0.5);
}
/*Utilities*/
#define CLAMP(x, lo, hi) (((x) < (lo)) ? (lo) : (((hi) < (x)) ? (hi) : (x)))
half tofloat16 (float x);
float tofloat32 (half x);
uint32_t ceillog2 (uint32_t x);
void sincosf (float angle, float *s, float *c);
float smoothstep (float a, float b, float c);
/*Pull in all the headers*/
#include "vec.h"
#include "mat.h"
