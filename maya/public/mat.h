#pragma once

typedef Vec M4x4[4];

static inline void
m4x4_identity (Vec *dst)
{
	vec_set (dst[0], 1, 0, 0, 0);
	vec_set (dst[1], 0, 1, 0, 0);
	vec_set (dst[2], 0, 0, 1, 0);
	vec_set (dst[3], 0, 0, 0, 1);
}
static inline void
m4x4_copy (Vec *dst, Vec *src)
{
#if 0
	for (int i = 0; i < 4; i++) vec_copy (dst[i], src[i]);
#else
	/*NB: assumes linear layout*/
	memcpy (dst, src, sizeof (M4x4));
#endif
}
static inline float *
m4x4_as_pointer (Vec *src)
{
	return (float *)src;
}
static inline float *
m4x4_position (Vec *src)
{
	return (float *)src[3];
}
static inline void
m4x4_forward (float *dst, Vec *src)
{
	vec_set (dst, src[0][0], src[1][0], src[2][0], src[3][0]);
}
static inline void
m4x4_side (Vec dst, M4x4 src)
{
	vec_set (dst, src[0][1], src[1][1], src[2][1], src[3][1]);
}
static inline void
m4x4_up (Vec dst, M4x4 src)
{
	vec_set (dst, src[0][2], src[1][2], src[2][2], src[3][2]);
}
static inline void
m4x4_translate (Vec *dst, float x, float y, float z)
{
	for (int i = 0; i < 3; i++)
		dst[3][i] += x*dst[0][i] + y*dst[1][i] + z*dst[2][i];
}
static inline void
m4x4_displace (Vec *dst, float x, float y, float z)
{
	dst[3][0] = x;
	dst[3][1] = y;
	dst[3][2] = z;
}
static inline void
m4x4_transposed (Vec *dst, Vec *src)
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			dst[i][j] = src[j][i];
}
static inline void
m4x4_inversed (Vec *dst, Vec *src)
{
	m4x4_transposed (dst, src);
}
static inline void
m4x4_scaled (Vec *dst, float k, Vec *src)
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++) dst[i][j] = k*src[i][j];
		dst[i][3] = src[i][3];
	}
	vec_copy (dst[3], src[3]);
}
static inline void
m4x4_multiply (Vec *dst, Vec *a, Vec *b)
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			dst[i][j] = a[i][0]*b[0][j];
			for (int k = 1; k < 4; k++)
				dst[i][j] += a[i][k]*b[k][j];
		}
}
static inline void
m4x4_multiply_vector (float *dst, Vec *m, float *v)
{/*Technically a row vector by column matrix, but don't tell anyone*/
	for (int i = 0; i < 4; i++)
		dst[i] = m[0][i]*v[0] + m[1][i]*v[1] + m[2][i]*v[2] + m[3][i]*v[3];
}
static inline void
m4x4_inverse_multiply_vector (float *dst, Vec *m, float *v)
{/*As above, but exploits the transpose-as-inverse property of 
symmetric matrices to do the transform*/
	for (int i = 0; i < 4; i++)
		dst[i] = m[i][0]*v[0] + m[i][1]*v[1] + m[i][2]*v[2] + m[i][3]*v[3];
}
/*3x3 variants of the above*/
static inline void
m4x4_multiply_vector_as_3x3 (float *dst, Vec *m, float *v)
{
	for (int i = 0; i < 3; i++)
		dst[i] = m[0][i]*v[0] + m[1][i]*v[1] + m[2][i]*v[2];
	dst[3] = v[3];
}
static inline void
m4x4_inverse_multiply_vector_as_3x3 (float *dst, Vec *m, float *v)
{
	for (int i = 0; i < 3; i++)
		dst[i] = m[i][0]*v[0] + m[i][1]*v[1] + m[i][2]*v[2];
	dst[3] = v[3];
}
void m4x4_from_perspective (Vec *, float, float, float, float);
void m4x4_from_angles (Vec *, float *);
