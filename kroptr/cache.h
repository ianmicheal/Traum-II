#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct _Cache_blk
{
	struct _Cache_blk *next;
	uint32_t head;
}Cache_blk;
typedef struct _Cache
{
	size_t *free;
	Cache_blk *blocks;
	Cache_blk *empty;
	uint32_t elem;
	uint32_t size;
	uint32_t cap;
}Cache;

void *cache_alloc (Cache *cache);
void cache_release (Cache *cache, void *ptr);
void cache_clear (Cache *cache);
void cache_reserve (Cache *cache, uint32_t reserved);
void cache_init (Cache *cache, uint32_t elem, uint32_t size);
void cache_destroy (Cache *cache);

#define CACHE_INIT(cache, type, size)\
	cache_init (cache, sizeof (type), size)
	
#define CACHE_INIT_RESERVE(cache, type, size, reserved) do {\
		cache_init (cache, sizeof (type), size);\
		cache_reserve (cache, reserved);\
	} while (0)
