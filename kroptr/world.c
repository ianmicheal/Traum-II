#if 0
typedef struct _World_object
{
	Trace_model *mdl;
	Vec head, tail;
	float radius;
	
	Vec origin;
	Vec angles;
}World_object;

typedef struct _World
{
	uint32_t nobj;
	World_object *objs;
}World;

typedef struct _Pair
{
	World_object *a, *b;
	M4x4 ta, tb;
}Pair;

#define MAX_CONTACTS 2
typedef struct _Contact
{
	Vec xyz;
	float depth;
}Contact;
typedef struct _Manifold
{
	Vec normal;
	uint32_t npoints;
	Contact points[MAX_CONTACTS];
}Manifold;

#define MAX_MANIFOLDS	256
static Manifold _manifolds[MAX_MANIFOLDS];
static uint32_t _nmanifolds;

static Manifold *
manifold_alloc (void)
{
	Manifold *m = NULL;
	if (MAX_MANIFOLDS <= _nmanifolds)
	{
		assert (0 && "exceeded manifold limit");
		return NULL;
	}
	m = &_manifolds[_nmanifolds];
	_nmanifolds++;
	return m;
}

/*
**Overlap tests
*/
static int
point_point_internal (Vec a, float ra, Vec b, float rb)
{
	Vec delta;
	float rad;
	float diff;
	float rej;
	/*Compute distance between the points*/
	vec_subtract (delta, a, b);
	diff = vec_dot3 (delta, delta);
	/*Ensure spheres overlap*/
	rad = ra + rb;
	if (rad*rad < diff)
	{
		return 0;
	}
	/*Ensure spheres are inside each other
	NB: There are a few different ways to handle this, but simply ignoring
	the collision is always valid*/
	rej = fabs (r0 - r1);
	if (diff < rej*rej)
	{
		return 0;
	}
	{/*Generate contacts*/
		Manifold *m0 = manifold_alloc ();
		Manifold *m1 = manifold_alloc ();
		Vec tmp;
		float m = sqrt (diff);
		float d = m - rad;
		float t = ra + 0.5*d;
		
		vec_normalised (m0->normal, delta);
		vec_mult_add (m0->points[0].xyz, a, t, m0->normal);
		m0->points[0].depth = d;
		m0->npoints = 1;
		
		vec_negated (m1->normal, m0->normal);
		m1->points[0] = m0->points[0];
		m1->npoints = 1;
	}
	return 1;
}
/*project p onto ab to find the closest point, clamping it either end point*/
static void
point_capsule_internal (float *d, float *p, float *a, float *b)
{
	Vec ab, ap;
	Vec pb;
	vec_subtract (ab, b, a);
	vec_subtract (ap, p, a);
	vec_subtract (pb, b, p);
	/*is the point outside a?*/
	float d1 = vec_length2 (ab);
	float d2 = vec_length2 (ap);
	if (d2 <= d1)
	{
		vec_copy (d, a);
		return;
	}
	/*is it outside of b?*/
	float d3 = vec_length2 (pb);
	if (d2 >= d3)
	{
		vec_copy (d, b);
		return;
	}
	/*well, the closest point is on the segment*/
	vec_mult_add (d, a, d2/(d1 - d3), ab);
}
/*
To find the closest point on a triangle from p we must examine 3 features:
	the voronoi regions at each point of the triangle
	each of the three segments that form the triangle
	the face itself
	
	if the projection of p into the plane of the triangle is inside any of the
	voronoi regions, then the closest point is the point that governs that
	region
	
	if the projection is outside any segment then the closest point is on that
	segment
	
	else the closest point is on the face itself
*/
static void
point_tri_internal (float *d, float *p, float *a, float *b, float *c)
{
	Vec ab, ac, ap;
	Vec bp, cp;
	/*gather some terms*/
	vec_subtract (ab, b, a);
	vec_subtract (ac, c, a);
	/*is p in the voronoi region of a?*/
	vec_subtract (ap, p, a);
	float d1 = vec_dot3 (ab, ap);
	float d2 = vec_dot3 (ac, ap);
	if (d1 <= 0 && d2 <= 0)
	{
		vec_copy (d, a);
		return;
	}
	/*is p in the voronoi region of b?*/
	vec_subtract (bp, p, b);
	float d3 = vec_dot3 (ab, bp);
	float d4 = vec_dot3 (ac, bp);
	if (d3 >= 0 && d4 >= 0)
	{
		vec_copy (d, b);
		return;
	}
	/*is p outside the segment ab?*/
	float vc = d1*d4 - d3*d2;
	if (vc <= 0 && d1 >= 0 && d3 <= 0)
	{
		vec_mult_add (d, a, d1/(d1 - d3), ab);
		return;
	}
	/*is p outside the voronoi region of c?*/
	vec_subtract (cp, p, c);
	float d5 = vec_dot3 (ab, cp);
	float d6 = vec_dot3 (ac, cp);
	if (d6 >= 0 && d5 <= d6)
	{
		vec_copy (d, c);
		return;
	}
	/*is p outside the segment of ac?*/
	float vb = d5*d2 - d1*d6;
	if (vb <= 0 && d2 >= 0 && d6 <= 0)
	{
		vec_mult_add (d, a, d2/(d2 - d6), ac);
		return;
	}
	/*is p outside the segment of bc?*/
	float va = d3*d6 - d5*d4;
	float t1 = d4 - d3;
	float t2 = d5 - d6;
	if (va <= 0 && t1 >= 0 && t2 >= 0)
	{
		Vec bc;
		vec_subtract (bc, c, b);
		vec_mult_add (d, b, t1/(t1 + t2), bc);
		return;
	}
	/*well, okay, so the closest point is on the face*/
	float de = 1.0/(va + vb + vc);
	float v = vb*de;
	float w = vc*de;
	for (uint32_t i = 0; i < 3; i++)
	{
		d[i] = a[i] + v*ab[i] + w*ac[i];
	}
}

/*
**Pair solvers
*/
static void
point_point (Pair *p)
{
	World_object *s = p->a;
	World_object *o = p->b;
	Vec a, b;
	/*Transform into world space*/
	vec_transform (a, p->ta, s->head);
	vec_transform (b, p->tb, o->head);
	/*Test for collision*/
	(void)point_point_internal (
		a, p->ta, s->radius,
		b, p->tb, o->radius,
		NULL
	);
}
static void
point_capsule (Pair *p)
{
}
static void
point_mesh (Pair *p)
{
	Vec a, b;
	Vec delta;
	/*Transform into mesh space. it's quicker this way*/
	vec_subtract (delta, p->ta[3], p->tb[3]);
	m4x4_inverse_multiply_vector_as_3x3 (a, p->tb, delta);
	/*Test the relevant faces*/
	Trace_model *mdl = p->b->mdl;
	for (uint32_t i = 0; i < mdl->nfeats; i++)
	{
		Vec pt, wp;
		Trace_feature *f = mdl->feats + i;
		float *v0 = mdl->verts + 3*f->indices[0];
		float *v1 = mdl->verts + 3*f->indices[1];
		float *v2 = mdl->verts + 3*f->indices[2];
		/*Find the closest point between to the triangle and test it*/
		closest_point_tri (pt, a, v0, v1, v2);
		(void)point_point_internal (a, p->a->radius, wp, 0, p->tb); 
	}
}
#endif
