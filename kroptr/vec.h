#pragma once

#include <math.h>

/*Generic macro implementations*/
#define KR_VEC_COPY(n, dst, src)\
	do{ memcpy (dst, src, n*sizeof (dst[0])); }while(0)
	
#define KR_VEC_NEGATED(n, dst, src)\
	do{ for (int i = 0; i < n; i++) (dst)[i] = -(src)[i]; }while(0)

#define KR_VEC_ADD(n, d, a, b)\
	do{ for (int i = 0; i < n; i++) (d)[i] = (a)[i] + (b)[i]; }while(0)

#define KR_VEC_SUBTRACT(n, d, a, b)\
	do{ for (int i = 0; i < n; i++) (d)[i] = (a)[i] - (b)[i]; }while(0)

#define KR_VEC_SCALE(n, d, k, b)\
	do{ for (int i = 0; i < n; i++) (d)[i] = k*(b)[i]; }while(0)

#define KR_VEC_MULTIPLY(n, d, a, b)\
	do{ for (int i = 0; i < n; i++) (d)[i] = (a)[i]*(b)[i]; }while(0)

#define KR_VEC_MULTIPLY_ADD(n, d, a, k, b)\
	do{ for (int i = 0; i < n; i++) (d)[i] = (a)[i] + (k)*(b)[i]; }while(0)
	
#define KR_VEC_DOT_INTERNAL(n, a, b) do{\
	float sum = a[0]*b[0];\
	for (int i = 1; i < n; i++) sum += a[i]*b[i];\
	return sum; }while (0)

/*Enable ISA specfic routines*/
#ifdef SUPERH
#undef KR_VEC_MULTIPLY_ADD
#define KR_VEC_MULTIPLY_ADD(n, d, a, k, b) do{\
	for (int i = 0; i < n; i++) {\
		asm  ("fmac %0, %1, %2\n"\
			: "+f" ((d)[i])\
			: "f" ((k)), "f" ((b)[i]), "f" ((a)[i]));\
	} }while(0)

#endif

static inline float
kr_vec_dot3 (const float *a, const float *b)
{
	KR_VEC_DOT_INTERNAL(3, a, b);
}
static inline float
kr_vec_dot (const float *a, const float *b)
{
	KR_VEC_DOT_INTERNAL(4, a, b);
}

static inline void
kr_vec_normalised (float *dst, const float *src)
{
	float m = kr_vec_dot3 (src, src);
	if (m <= 1e-4)
	{
		KR_VEC_COPY(3, dst, src);
		return;
	}
	float rp= 1.0/sqrtf (m);
	for (uint32_t i = 0; i< 3; i++)
	{
		dst[i] = rp*src[i];
	}
}

static inline void
kr_vec_cross (float *v, const float *a, const float *b)
{
	v[0] = 0;
	v[1] = 0;
	v[2] = 0;
}

static inline int
sign (float a, float b)
{
	const uint32_t x = *((uint32_t *)&a)&0x80000000;
	const uint32_t y = *((uint32_t *)&b)&0x80000000;
	return x == y;
}

