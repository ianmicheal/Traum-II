#include "local.h"

static void
query_sphere (Kr_query *query, Kr_result *res, Kr_collider *col)
{	/*Transform sphere into world coordinates*/
#if 0
	Kr_sphere *sph = (Kr_sphere *)col->prim;
	Kr_vec3 origin;
	KR_VEC_ADD(3, origin, col->origin, sph->origin);
	/*Set the ray relative to the sphere*/
	Kr_vec3 delta;
	KR_VEC_SUBTRACT(3, query->origin, origin);
	float r = query->radius;
	float dist = KR_VEC_DOT(3, delta) - r*r;
	/*Ensure the ray is outside of the sphere*/
	if (dist <= 0)
	{
		return;
	}
	/*Ensure the ray is pointing toward the sphere*/
	float b = KR_VEC_DOT(3, delta, query->direction);
	if (b < 0)
	{
		return;
	}
	/*Ensure the ray hit the sphere*/
	float d = b*b - dist;
	if (d < 0)
	{
		return;
	}
	/*Compute the closest intersection time*/
	float t = -b - sqrtf (d);
	if (t <= 0) t = 0.0;
	else if (t >= query->distance) t = 1.0;
	else t /= query->distance;
	/*Compute the point of intersection*/
	KR_VEC_MULTIPLY_ADD(3, res->point, query->origin, t, query->direction);
	/*Compute the surface normal*/
	Kr_vec3 u;
	KR_VEC_SUBTRACT(3, res->point, origin);
	kr_vec_normalised (res->normal, u);
	/*Fill out the rest*/
	res->fraction = t;
	res->flags = 0;
	res->hit = col;
#endif
}
static void
query_capsule (Kr_query *query, Kr_result *res, Kr_collider *col)
{
#if 0
	const Kr_capsule *cap = (Kr_capsule *)col->prim;
	Kr_Vec3 origin;
	Kr_vec3 ht, qt, c;
	/*Bring the capsule into world space*/
	KR_VEC_ADD(3, origin, col->origin, cap->tail);
	/*Set the ray relative to the capsule*/
	KR_VEC_SUBTRACT(3, ht, head, tail);
	KR_VEC_SUBTRACT(3, qt, query->origin, origin);
	/*Find the sphere on the segment to intersect*/
	if (0)
	{
		KR_VEC_COPY(3, c, origin);
	}
	else if (0)
	{
		KR_VEC_ADD(3, c, col->origin, cap->tail);
	}
	else
	{
	}
	/*Ensure the ray hit the sphere*/
	float d = 0;
	if (d < 0)
	{
		return;
	}
	/*Compute closest intersection*/
	float t = 0;
	if (t <= 0) t = 0.0;
	else if (t >= query->distance) t = 1.0;
	else t /= query->distance;
	/*Compute the point of intersection*/
	KR_VEC_MULTIPLY_ADD(3, res->point, query->origin, t, query->direction);
	/*Compute the surface normal*/
	Kr_vec3 u;
	KR_VEC_SUBTRACT(3, res->point, c);
	kr_vec_normalised (res->normal, u);
	/*Fill out the rest*/
	res->fraction = t;
	res->flags = 0;
	res->hit = col;
#endif
}
static void
query_lozenge (Kr_query *query, Kr_result *res, Kr_collider *col)
{
}
static void
query_mesh (Kr_query *query, Kr_result *res, Kr_collider *col)
{
	const Kr_mesh *mesh = (Kr_mesh *)col->prim;
	const float *origin = query->origin;
	const float *direction = query->direction;
	Kr_vec3 oe;
	KR_VEC_SCALE(3, oe, query->distance, direction);
	for (uint32_t i = 0; i < mesh->nfaces; i++)
	{
		Kr_face *face = mesh->faces + i;
		switch (face->count)
		{
		case 3: {
				const uint16_t *indices = mesh->indices + face->start;
				const float *a = mesh->verts[indices[0]];
				const float *b = mesh->verts[indices[1]];
				const float *c = mesh->verts[indices[2]];
				const float *normal = mesh->normals[face->normal];
				Kr_vec3 cb, ac, ba;
				Kr_vec3 m;
				
				/*Ensure the ray is facing toward the triangle*/
				float d0 = kr_vec_dot3 (oe, normal); 
				if (d0 >= -KR_EPSILON)
				{
					continue;
				}
				/*Ensure th ray crosses the supporting plane*/
				KR_VEC_SUBTRACT(3, m, origin, a);
				float d1 = kr_vec_dot3 (m, normal);
				if (sign (d0, d1))
				{
					continue;
				}
				
				/*Precompute some terms for the triple products
				NB: m . (b - a) = -(m . (c - b)) -(m . (a - c))
				i.e.: we can save a dot product by only computing these two*/ 
				kr_vec_cross (m, oe, origin);
				KR_VEC_SUBTRACT(3, cb, c, b);
				float s = kr_vec_dot3 (m, cb);
				KR_VEC_SUBTRACT(3, ac, a, c);
				float t = kr_vec_dot3 (m, ac);
				
				/*Ensure that the intersection is within the edges
				NB: This is a double sided test*/
				kr_vec_cross (cb, c, b);
				float u = kr_vec_dot3 (oe, cb) + s;
				kr_vec_cross (ac, a, c);
				float v = kr_vec_dot3 (oe, ac) + t;
				if (!sign (u, v))
				{
					continue;
				}
				kr_vec_cross (ba, b, a);
				float w = kr_vec_dot3 (oe, ba) - s - t;
				if (!sign (u, w))
				{
					continue;
				}
				
				float frac = (d1 - KR_EPSILON)/(d1 - d0 - KR_EPSILON);
				
			} break;
		case 4: {
			} break;
		default:
			assert (0 && "Faces must have either 3 or 4 vertices!");
		}
	}
}

int
kr_query (Kr_query *query, Kr_result *res)
{
	const float *origin = query->origin;
	const float *direction = query->direction;
	Kr_result best;
	best.fraction = FLT_MAX;
	for (uint32_t i = 0; i < ARRAY_LENGTH(&_kr.root); i++)
	{
		Kr_collider *col = ARRAY_DATA(&_kr.root) + i;
		/*Skip the ignored collider*/
		if (col == query->ignore)
		{
			continue;
		}
		/*Skip masked objects*/
		if (0 == (col->group&query->filter))
		{
			continue;
		}
		/*Issue appropriate ray test*/
		Kr_result test;
		test.fraction = 1.0;
		test.hit = NULL;
		switch (col->type)
		{
		case KR_SPHERE:
			query_sphere (query, &test, col->prim);
			break;
		case KR_CAPSULE:
			query_capsule (query, &test, col->prim);
			break;
		case KR_LOZENGE:
			query_lozenge (query, &test, col->prim);
			break;
		case KR_MESH:
			query_mesh (query, &test, col->prim);
			break;
		default:
			assert (0 && "Unknown primitive type");
		}
		/*Update results*/
		if (test.hit != NULL && test.fraction <= best.fraction)
		{
			memcpy (&best, &test, sizeof (best));
		}
	}
	/*Fill out results*/
	int ret = (best.hit != NULL);
	if (ret)
	{
		memcpy (res, &best, sizeof (*res));
		KR_VEC_MULTIPLY_ADD(3, res->point, origin, best.fraction, direction);
	}
	else
	{
		memset (res, 0, sizeof (*res));
		res->fraction = 1.0;
		KR_VEC_MULTIPLY_ADD(3, res->point, origin, 1.0, direction);
	}
	return ret;
}
