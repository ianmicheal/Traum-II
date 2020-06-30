#include "local.h"

void
array_resize (void *ptr, size_t elem, uint32_t resv, uint32_t grain)
{
	ARRAY(void) *array = ptr;
	/*Ensure that the array has to grow*/
	if (array->len < array->cap)
	{	/*Ensure the length is greater than the reserved minimum*/
		if (array->len < resv)
		{
			return;
		}
		/*If half of the array is unused, then shrink by half*/
		const uint32_t half = array->cap>>1;
		const uint32_t unused = array->cap - array->len;
		if (unused < half)
		{
			return;
		}
		array->cap = half;
	}
	/*Grow the array capacity by the given granularity*/
	uint32_t nc = array->cap + grain;
	void *nd = realloc (array->data, elem*nc);
	if (NULL == nd)
	{
		kr_throw (KR_ERR_NOMEM);
	}
	array->data = nd;
	array->cap = nc;
}
void
array_init (void *ptr, size_t elem, uint32_t resv)
{
	ARRAY(void) *array = ptr;
	memset (array, 0, sizeof (*array));
	array->data = malloc (elem*resv);
	if (NULL == array->data)
	{
		kr_throw (KR_ERR_NOMEM);
	}
	array->cap = resv;
}
void
array_destroy (void *ptr)
{
	ARRAY(void) *array = ptr;
	free (array->data);
	array->data = NULL;
	array->len = 0;
	array->cap = 0;
}
