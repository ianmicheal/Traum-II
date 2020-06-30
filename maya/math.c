#include "public/math.h"

uint32_t maya_seed = 0x23191775;

uint32_t
xorshift32 (void)
{/*from marsaglia's xorshift RNGs*/
	uint32_t x = maya_seed;
	x ^= x<<13;
	x ^= x>>17;
	x ^= x<<5;
	return maya_seed = x;
}

half
tofloat16 (float x)
{
	int s, e, f;
	union _Coerce
	{
		float f;
		int i;
	}c;
	c.f = x;
	s = (c.i>>31)&0x1;
	e = (c.i>>23)&0xff;
	/*Preserve subnormal and error cases*/
	if (0 < e && e < 0xff) e = e - 127 + 15;
	else e &= 0x1f;
	f = (c.i>>13)&0x3ff;
	return (half)((s<<15)|(e<<10)|f);
}
float
tofloat32 (half x)
{
	union _Coerce
	{
		float f;
		int i;
	}c;
	int e = (x>>10)&0x1f;
	if (0 < e && e < 0x1f) e = e - 15 + 127;
	else e = (e<<27)>>27;
	int s = (x>>15)&0x1;
	int f = x&0x3ff;
	c.i = (s<<31)|(e<<23)|(f<<13);
	return c.f;
}
uint32_t
ceillog2 (uint32_t x)
{/*Returns the ceiling of log2x*/
	static unsigned char log2tbl[] =
	{
		0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
		7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
		8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
		8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
		8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
		8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8
	};
	register uint32_t y;
	if (--x < 256)
	{
		return log2tbl[x];
	}
	if ((y = (x>>8)) < 256)
	{
		return 8 + log2tbl[y];
	}
	if ((y = (x>>16)) < 256)
	{
		return 16 + log2tbl[y];
	}
	return 24 + log2tbl[x>>24];
}
void
sincosf (float angle, float *s, float *c)
{
	*s = sinf (angle);
	*c = cosf (angle);
}
float
smoothstep (float a, float b, float c)
{
	float x = (c - a)/(b - a);
	if (x <= 0) x = 0;
	else if (1 <= x) x = 1;
	return x*x*(3 - 2*x);
}
