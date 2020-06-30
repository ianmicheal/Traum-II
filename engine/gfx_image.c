#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <maya/list.h>
#include <maya/math.h>
#include "gfx_local.h"
#include "kmg.h"

#ifndef USING_KOS
static uint32_t
morton (uint32_t x, uint32_t y)
{/*Interleaves the bits of x and y to form a morton number
there are a few ways to do this, but this one can run entirely
in the cpu without messing up the cache*/
	x = (x|(x<<8))&0x00FF00FF;
	y = (y|(y<<8))&0x00FF00FF;
	x = (x|(x<<4))&0x0F0F0F0F;
	y = (y|(y<<4))&0x0F0F0F0F;
	x = (x|(x<<2))&0x33333333;
	y = (y|(y<<2))&0x33333333;	
	x = (x|(x<<1))&0x55555555;
	y = (y|(y<<1))&0x55555555;
	return x|(y<<1);
}
#endif

Gfx_handle
gfx_image (const char *file)
{
	uint16_t codes[4*256];
	KMG_header h;
	Gfx_image *image = NULL;
	uint8_t *tmp = NULL;
	uint8_t *d = NULL;
	FILE *fp = NULL;
	/*Create a new record*/
	image = malloc (sizeof (*image));
	if (NULL == image)
	{/*Should be fatal*/
		return NULL;
	}
	memset (image, 0, sizeof (*image));
	dlist_insert_front (&image->chain, &_gfx.images);
	image->format = IF_ERROR;
	image->refs = 1;
	/*Try to open a stream to the file*/
	fp = sys_fopen (file);
	if (!fp)
	{
		goto Error;
	}
	/*Verify the header*/
	fread (&h, sizeof (h), 1, fp);
	if (h.magick != KMG_MAGICK)
	{
		goto Error;
	}
	if (h.version != KMG_VERSION)
	{
		goto Error;
	}
	if (h.platform != KMG_PLAT_DC)
	{
		goto Error;
	}
	/*Textures must be twiddled and quantitised*/
	if (!(h.format&(KMG_DCFMT_VQ|KMG_DCFMT_TWIDDLED)))
	{
		goto Error;
	}
	/*Ensure the image is a supported format*/
	if (!(h.format&(KMG_DCFMT_RGB565|KMG_DCFMT_ARGB4444|KMG_DCFMT_ARGB1555)))
	{
		goto Error;
	}
	image->width = h.width;
	image->height = h.height;
	image->format = 0;
	/*Set the format flags*/
	if (h.format&(KMG_DCFMT_ARGB4444|KMG_DCFMT_ARGB1555))
	{
		image->format |= IF_ALPHA;
	}
	/*Define the texture for opengl*/
	glGenTextures (1, &image->handle);
	glBindTexture (GL_TEXTURE_2D, image->handle);
	glTexParameteri (
		GL_TEXTURE_2D,
		GL_TEXTURE_MIN_FILTER,
		GL_LINEAR
	);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
#ifndef USING_KOS
	/*Read the codebook. If there are mipmaps then an weedy little zero byte
	follows, for some reason. I don't make the rules here.*/
	fread (codes, sizeof (codes), 1, fp);
	/*Create stratch buffers*/
	tmp = malloc (4*h.width*h.height);
	if (NULL == tmp)
	{
		goto Error;
	}
	d = malloc (h.size);
	if (NULL == d)
	{
		goto Error;
	}
	/*Load each mipmap*/
	fread (d, h.size, 1, fp); 
	/*Convert into RGBA from what ever ungodly format it is in*/
#define UNPACK565(ndx, colour) do {\
		uint32_t c = colour;\
		tmp[ndx++] = (uint8_t)(c>>11)/31.0*255.0;\
		tmp[ndx++] = (uint8_t)((c>>5)&0x3f)/63.0*255.0;\
		tmp[ndx++] = (uint8_t)(c&0x1f)/31.0*255.0;\
		tmp[ndx++] = 0xff;\
	} while (0)
#define UNPACK4444(ndx, colour) do {\
		uint32_t c = colour;\
		tmp[ndx++] = (uint8_t)((c>>8)&0x1f)/15.0*255.0;\
		tmp[ndx++] = (uint8_t)((c>>4)&0x1f)/15.0*255.0;\
		tmp[ndx++] = (uint8_t)(c&0x1f)/15.0*255.0;\
		tmp[ndx++] = (uint8_t)(c>>12)/15.0*255.0;\
	} while (0)
#define UNPACK1555(ndx, colour) do {\
		uint32_t c = colour;\
		tmp[ndx++] = (uint8_t)((c>>10)&0x1f)/31.0*255.0;\
		tmp[ndx++] = (uint8_t)((c>>5)&0x1f)/31.0*255.0;\
		tmp[ndx++] = (uint8_t)(c&0x1f)/31.0*255.0;\
		tmp[ndx++] = (uint8_t)(c>>15)*255;\
	} while (0)
	uint32_t i, j;
	for (i = 0; i < h.height/2; i++)
	{
		for (j = 0; j < h.width/2; j++)
		{
			uint32_t curve = morton (i, j); /*inverse morton order*/
			uint32_t index = d[curve];
			uint32_t p, q;
			/*Restore tile*/
			p = 8*(i*h.width + j);
			q = 4*(2*i + 1)*h.width + 8*j;
			if (h.format&KMG_DCFMT_RGB565)
			{
				UNPACK565(p, codes[4*index + 0]);
				UNPACK565(p, codes[4*index + 2]);
				UNPACK565(q, codes[4*index + 1]);
				UNPACK565(q, codes[4*index + 3]);
			}
			else if (h.format&KMG_DCFMT_ARGB4444)
			{
				UNPACK4444(p, codes[4*index + 0]);
				UNPACK4444(p, codes[4*index + 2]);
				UNPACK4444(q, codes[4*index + 1]);
				UNPACK4444(q, codes[4*index + 3]);				
			}
			else if (h.format&KMG_DCFMT_ARGB1555)
			{
				UNPACK1555(p, codes[4*index + 0]);
				UNPACK1555(p, codes[4*index + 2]);
				UNPACK1555(q, codes[4*index + 1]);
				UNPACK1555(q, codes[4*index + 3]);
			}
		}
	}
#undef UNPACK565
#undef UNPACK4444
#undef UNPACK1555
	/*Upload the image data*/
	glTexImage2D (
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		h.width,
		h.height,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		tmp
	);
	/*Release working resources and return the image*/
	free (d);
	free (tmp);
	fclose (fp);
	return image;
Error: /*Oops!*/
	if (d) free (d);
	if (tmp) free (tmp);
	fclose (fp);
	return image;
#else
	tmp = d = malloc (h.size + sizeof (codes));
	if (NULL == d)
	{
		goto Error;
	}								 
	/*Load the data*/
	fread (tmp, sizeof (codes), 1, fp);
	tmp += sizeof (codes);
	fread (tmp, h.size, 1, fp);
	/*Set the proper format*/
	GLenum fmt = GL_UNSIGNED_SHORT_5_6_5_VQ_TWID;
	if (h.format&KMG_DCFMT_ARGB4444)
		fmt = GL_UNSIGNED_SHORT_4_4_4_4_VQ_TWID;
	else if (h.format&KMG_DCFMT_ARGB1555)
		fmt = GL_UNSIGNED_SHORT_1_5_5_5_VQ_TWID;
	/*Upload the texture to the PVR*/
	glCompressedTexImage2D (
		GL_TEXTURE_2D, 1, fmt, h.width, h.height, 0, h.size, d
	);
Error:
	if (d) free (d);
	return image;
#endif
}
void
gfx_image_acquire (Gfx_handle image)
{
	((Gfx_image *)image)->refs++;
}
void
gfx_image_release (Gfx_handle image)
{
	Gfx_image *self = (Gfx_image *)image;
	assert (self != NULL);
	if (self->refs != 0)
	{
		self->refs--;
		return;
	}
	image_free (image);
}
void
image_free (Gfx_image *image)
{
	glDeleteTextures (1, &image->handle);
	dlist_remove (&image->chain);
	free (image);
}
