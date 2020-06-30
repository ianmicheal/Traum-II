#include "public/math.h"

Vec vec_forward = {0, 0, 1, 0};
Vec vec_side = {1, 0, 0, 0};
Vec vec_up = {0, 1, 0, 0};

int
vec_normalised (float *dst, float *src)
{
	float m, rp;
	m = vec_dot (src, src);
	if (m <= 1e-4)
	{
		return -1;
	}
	rp = 1.0/sqrt (m);
	for (int i = 0; i < 4; i++) dst[i] = rp*src[i];
	return 0;
}
