#include "local.h"

/*Realistically there will be relatively few objects on the dreamcast*/
#define KR_ARRAY_MIN				32
#define KR_ARRAY_GRANULARITY		8

#define KR_PAIR_MIN					256
#define KR_PAIR_GRANULARITY			256

struct _Kroptr _kr;

static int
filter (Kr_collider *a, Kr_collider *b)
{	/*Ensure that the objects may interact*/
	uint32_t x = a->group&b->filter;
	uint32_t y = b->group&a->filter;
	if (0 == (x&y))
	{
		return -1;
	}
	/*Let the user do finer filtering if they asked*/
	if (_kr.cb.broad != NULL)
	{
		return _kr.cb.broad (a, b);
	}
	return 0;
}

static void
pair_add (Kr_collider *a, Kr_collider *b)
{	/*Order it so the lowest goes first for consistency*/
	if (b < a)
	{
		Kr_collider *swap = a;
		a = b;
		b = swap;
	}
	/*Ensure that the pair does not already exist*/
	for (uint32_t i = 0; i < _kr.pairs.len; i++)
	{
		Kr_pair *p = _kr.pairs.data + i;
		if (p->a != a || p->b != b)
		{
			continue;
		}
		/*Already exists*/
		return;
	}
	/*Apeend the new pair into the list*/
	ARRAY_RESIZE(&_kr.pairs, KR_PAIR_MIN, KR_PAIR_GRANULARITY);
	Kr_pair *p = &_kr.pairs.data[_kr.pairs.len++];
	p->a = a;
	p->b = b;
}
void
kr_step (float dt)
{	/*Mark a new cycle so everything may be reevaluated*/
	ARRAY_LENGTH(&_kr.pairs) = 0;
	_kr.cycle++;
	/*First do a broadphase pass over all objects, checking to see if they
	-may- overlap. Collision pairs are generated for each tentative
	interaction, then resolved later in the narrowphase*/
	for (uint32_t i = 0; i < ARRAY_LENGTH(&_kr.active); i++)
	{
		Kr_collider *self = ARRAY_DATA(&_kr.active)[i];
		for (uint32_t j = 0; j < ARRAY_LENGTH(&_kr.root); j++)
		{
			Kr_collider *other = ARRAY_DATA(&_kr.root)[i];
			/*Do not tests against our self*/
			if (self == other)
			{
				continue;
			}
			/*Check to see if we're recognising this interaction*/
			if (filter (self, other))
			{
				continue;
			}
			/*Ensure AABBs overlap*/
			for (uint32_t k = 0; k < 3; k++)
			{
				float delta = fabs (other->centre[k] - self->centre[k]);
				float minkowski = other->extent[k] + self->extent[k];
				if (delta > minkowski) goto Next;
			}
			pair_add (self, other);
		Next:
			continue;
		}
	}
	/*Iterate over the pairs and test to see if the objects actually collide*/
	for (uint32_t i = 0; i < ARRAY_LENGTH(&_kr.pairs); i++)
	{
		Kr_pair *p = _kr.pairs.data + i;
		//kr_solve (p->a, p->b);
	}
}
void
kr_run (float dt, float fps, uint32_t limit)
{
	assert (1 <= fps && "fps must be greater than 1.0");
	assert (limit != 0 && "limit must be non zero");
	float time = _kr.residual + dt;
	float quantum = 1.0f/fps;
	while (quantum <= time)
	{
		if (!limit)
		{
			time = 0;
			break;
		}
		kr_step (quantum);
		time -= quantum;
		limit--;
	}
	_kr.residual = time;
}

const char *
kr_error (int error)
{
	switch (error)
	{
	case KR_ERR_NOMEM: return "Failed to allocate memory";
	default: break;
	}
	return "Unknown error";
}
void
kr_panic (Kr_panic panic)
{
	_kr.panic = panic;
}
void
kr_callbacks (Kr_callbacks *cb)
{
	_kr.cb = *cb;
}
void
kr_init (void)
{
	memset (&_kr, 0, sizeof (_kr));
	ARRAY_INIT(&_kr.active, KR_ARRAY_MIN);
	ARRAY_INIT(&_kr.root, KR_ARRAY_MIN);
	ARRAY_INIT(&_kr.pairs, KR_PAIR_MIN);
	
	CACHE_INIT_RESERVE(&_kr.manifolds, Kr_manifold, KR_PAGE_SIZE, 64);
	CACHE_INIT_RESERVE(&_kr.colliders, Kr_collider, KR_PAGE_SIZE, 64);
	CACHE_INIT_RESERVE(&_kr.spheres, Kr_sphere, KR_PAGE_SIZE, 32);
	CACHE_INIT_RESERVE(&_kr.capsules, Kr_capsule, KR_PAGE_SIZE, 32);
	CACHE_INIT_RESERVE(&_kr.lozenges, Kr_lozenge, KR_PAGE_SIZE, 16);
	CACHE_INIT_RESERVE(&_kr.meshes, Kr_mesh, KR_PAGE_SIZE, 16);
}
void
kr_shutdown (void)
{
	ARRAY_DESTROY(&_kr.active);
	ARRAY_DESTROY(&_kr.root);
	ARRAY_DESTROY(&_kr.pairs);
	
	cache_destroy (&_kr.meshes);
	cache_destroy (&_kr.capsules);
	cache_destroy (&_kr.spheres);
	cache_destroy (&_kr.lozenges);
	cache_destroy (&_kr.colliders);
	cache_destroy (&_kr.manifolds);
}

/*
**Internal API
*/
void
kr_throw (int error)
{
	if (_kr.panic) _kr.panic (error);
	abort ();
}

void
kr_activate (Kr_collider *col)
{	/*Ensure the active list has a free slot*/
	ARRAY_RESIZE(&_kr.active, KR_ARRAY_MIN, KR_ARRAY_GRANULARITY);
	/*Append collider to the end of the active list*/
	Kr_collider **data = ARRAY_DATA(&_kr.active);
	data[ARRAY_LENGTH(&_kr.active)++] = col;
	/*Mark object as active*/
	col->flags |= KR_CF_ACTIVE;
}
void
kr_disable (Kr_collider *col)
{	/*Should always be a power of 2*/
	const uint32_t ITEMS_PER_CACHE_LINE = KR_CACHE_LINE/sizeof (void *);
	Kr_collider **list = ARRAY_DATA(&_kr.active);
	uint32_t len = ARRAY_LENGTH(&_kr.active);
	uint32_t i = 0;
	/*Align len to a multiple of the cache size and search*/
	uint32_t aligned = len&~(ITEMS_PER_CACHE_LINE - 1);
	while (i < aligned)
	{	/*Prefetch the next cache line while we work*/
		PREFETCH(list + i + ITEMS_PER_CACHE_LINE);
		for (uint32_t j = 0; j < ITEMS_PER_CACHE_LINE; j++)
		{
			if (list[i] != col) continue;
			else goto Done;
			i++;
		}
	}
	/*Search the remainder*/
	while (i < len)
	{
		if (list[i] != col) continue;
		else goto Done;
		i++;
	}
	/*Not found*/
	return;
Done:
	/*Rotate-replace the element*/
	list[i] = list[--ARRAY_LENGTH(&_kr.active)];
	/*Mark object as inactive*/
	col->flags &= ~KR_CF_ACTIVE;
}
