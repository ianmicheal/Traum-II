#include "local.h"

#define CACHE_ALIGN(x, y) (((x)+((y)-1))&(~((y)-1)))

static Cache_blk *
add_block (Cache *cache)
{
	Cache_blk *blk = malloc (cache->size);
	memset (blk, 0, sizeof (*blk));
	return blk;
}
void *
cache_alloc (Cache *cache)
{	/*Try to reuse a free element*/
	void *ptr = cache->free;
	if (ptr != NULL)
	{
		cache->free = (size_t *)(*(size_t *)ptr);
		return ptr;
	}
	/*Ensure there are unused elements on the block*/
	Cache_blk *curr = cache->blocks;
	if (!curr || cache->cap <= curr->head)
	{
		if (cache->empty)
		{
			curr = cache->empty;
			cache->empty = curr->next;
		}
		else curr = add_block (cache);
		curr->next = cache->blocks;
		cache->blocks = curr;
	}
	{/*Initialise the new item*/
		char *base = (char *)curr + sizeof (*curr);
		ptr = base + curr->head*cache->elem;
		curr->head++;
	}
	return ptr;
}
void
cache_release (Cache *cache, void *ptr)
{	/*Store the next free slot in this item's place*/
	*(size_t *)ptr = (size_t)cache->free;
	/*Mark this item slot as free*/
	cache->free = ptr;
}
void
cache_clear (Cache *cache)
{
	Cache_blk *blk = cache->blocks;
	while (blk != NULL)
	{
		Cache_blk *next = blk->next;
		/*Reset the top elment to effectively clear the block,
		then add it to the empty list*/
		blk->head = 0;
		blk->next = cache->empty;
		cache->empty = blk;
		
		blk = next;
	}
	/*Clear the free and active lists to enable reuse*/
	cache->free = NULL;
	cache->blocks = NULL;
}
static void
free_list (Cache_blk *head)
{
	while (head != NULL)
	{
		Cache_blk *next = head->next;
		free (head);
		head = next;
	}
}
void
cache_reserve (Cache *cache, uint32_t reserved)
{
	Cache_blk *blk = cache->empty;
	Cache_blk **tail = &cache->empty;
	uint32_t count = 0;
	while (blk != NULL)
	{
		if (count < reserved)
		{
			count += cache->cap;
			*tail = blk->next;
			blk = blk->next;
		}
		else break;	
	}
	/*Free the rest of the blocks, and stub out the tail*/
	free_list (*tail);
	*tail = NULL;
}
void
cache_init (Cache *cache, uint32_t elem, uint32_t size)
{
	memset (cache, 0, sizeof (*cache));
	cache->elem = CACHE_ALIGN(elem, sizeof (void *));
	cache->size = size;
	cache->cap = cache->size/cache->elem;
}
void
cache_destroy (Cache *cache)
{
	free_list (cache->blocks);
	free_list (cache->empty);
}
