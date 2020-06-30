#include "public/math.h"

void
m4x4_from_perspective (Vec *dst, float h, float v, float znear, float zfar)
{
	float x = 1.0/tan (M_PI*h/360.0);
	float y = 2.0/tan (M_PI*v/360.0);
	vec_set (dst[0], x, 0, 0, 0);
	vec_set (dst[1], 0, y, 0, 0);
	vec_set (dst[2], 0, 0,-1,-1);
	vec_set (dst[3], 0, 0,-1, 0);
}
void
m4x4_from_angles (Vec *dst, float *angles)
{
	float cy, cp, cr;
	float sy, sp, sr;
	float a, b, c, d;
	
	sincosf (DEG2RAD(angles[0]), &sy, &cy);
	sincosf (DEG2RAD(angles[1]), &sp, &cp);
	sincosf (DEG2RAD(angles[2]), &sr, &cr);
	
	a = sy*cr;
	b = sy*sr;
	c = cy*cr;
	d = cy*sr;

	vec_set (dst[0], cy*cp,-a + d*sp, b + c*sp, 0);
	vec_set (dst[1], sy*cp, c + b*sp,-d + a*sp, 0);
	vec_set (dst[2],   -sp,	   cp*sr,    cp*cr, 0);
	vec_set (dst[3], 0, 0, 0, 1);
}
