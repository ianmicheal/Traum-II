#include "local.h"

typedef void (*Solver) (Kr_collider *a, Kr_collider *b);

static inline int
narrow_filter (Kr_collider *a, Kr_collider *b)
{
	if (_kr.cb.narrow) return _kr.cb.narrow (a, b);
	return 0;
}
static void
invalid (Kr_collider *a, Kr_collider *b)
{
	kr_throw (KR_ERR_BADTEST);
}
/*
**Sphere tests
*/
static void
sphere_sphere (Kr_collider *a, Kr_collider *b)
{
#if 0
	const Kr_sphere *sa = a->prim;
	const Kr_sphere *sb = b->prim;
	const float ra = sa->radius;
	const float rb = sb->radius;
	Kr_tform cat;
	Kr_vec4 p, q;
	/*Make everything relative to a*/
	tform_multiply_vector (p, a->tform, sa->origin);
	tform_multiply (cat, a->tform, b->tform);
	tform_multiply_vector (q, cat, sb->origin);
	/*Ensure the spheres overlap*/
	Kr_vec4 delta;
	KR_VEC_SUBTRACT(3, delta, q, p);
	float dist = kr_vec_dot3 (delta, delta);
	float sum = ra + rb;
	if (sum*sum < dist)
	{
		return;
	}
	/*Intrude here to allow the user to reject this interaction*/
	if (narrow_filter (a, b))
	{
		return;
	}
	/*If the distance is less than the minkowski difference, then there's no
	right way to handle this intersection. We could use the up vector if we 
	want absolutely no penetration, but ignoring it is just as valid.*/
	float diff = ra - rb;
	if (dist < diff*diff)
	{
		return;
	}
	{/*Generate contact manifolds*/
		Kr_manifold *m0 = manifold_alloc ();
		Kr_manifold *m1 = manifold_alloc ();
		Kr_Vec3 tmp;
		float m = sqrtf (dist);
		float d = m - ra;
		float t = ra + 0.5*d;
		
		vec_normalised (m0->normal, delta);
		vec_mult_add (m0->points[0].xyz, a, t, m0->normal);
		m0->points[0].depth = d;
		m0->npoints = 1;
		
		vec_negated (m1->normal, m0->normal);
		m1->points[0] = m0->points[0];
		m1->npoints = 1;
	}
#endif
}
static void
sphere_capsule (Kr_collider *a, Kr_collider *b)
{
}
static void
sphere_lozenge (Kr_collider *a, Kr_collider *b)
{
}
static void
sphere_mesh (Kr_collider *a, Kr_collider *b)
{
}
/*
**Capsule tests
*/
static void
capsule_capsule (Kr_collider *a, Kr_collider *b)
{
}
static void
capsule_lozenge (Kr_collider *a, Kr_collider *b)
{
}
static void
capsule_mesh (Kr_collider *a, Kr_collider *b)
{
}
/*
**Lozenge tests
*/
static void
lozenge_lozenge (Kr_collider *a, Kr_collider *b)
{
}
static void
lozenge_mesh (Kr_collider *a, Kr_collider *b)
{
}

void
kr_solve (Kr_collider *a, Kr_collider *b)
{
	static Solver solvers[] = {
		sphere_sphere, sphere_capsule,  sphere_lozenge,  sphere_mesh,
		invalid,       capsule_capsule, capsule_lozenge, capsule_mesh,
		invalid,       invalid,         lozenge_lozenge, lozenge_mesh,
		invalid,       invalid,         invalid,         invalid
	};
	/*Flip the colliders so lower order shapes come first*/
	if (b->type < a->type)
	{
		Kr_collider *swap = a;
		a = b;
		b = swap;
	}
	/*Select the appropriate solver and invoke it*/
	uint32_t id = 4*a->type + b->type;
	Solver solver = solvers[id];
	solver (a, b);
}
