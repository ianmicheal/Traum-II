#pragma once 

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*Units are given in elements*/
void array_resize (void *ptr, size_t elem, uint32_t resv, uint32_t grain);
void array_init (void *ptr, size_t elem, uint32_t resv);
void array_destroy (void *ptr);

/*Defines a new array structure*/
#define ARRAY(type)\
	struct { type *data; uint32_t len, cap; }
	
/*Returns the data of an array*/
#define ARRAY_DATA(array) ((array)->data)

/*Returns the length of an array*/
#define ARRAY_LENGTH(array) ((array)->len)

/*Returns the capacity of an array*/
#define ARRAY_CAPACITY(array) ((array)->cap)

/*Initialises an array*/
#define ARRAY_INIT(array, reserved)\
	array_init ((array), sizeof ((array)->data[0]), reserved)

/*Destroys an array, freeing its heaped resources*/
#define ARRAY_DESTROY(array)\
	array_destroy ((array))

/*Shrinks or grows the capacity of an array as needed*/
#define ARRAY_RESIZE(array, reserved, granularity)\
	array_resize ((array), sizeof ((array)->data[0]), reserved, granularity)
