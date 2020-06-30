#include "gfx_local.h"

static Gfx_debug_shape *
alloc_shape (Gfx_debug *self, uint32_t type, uint32_t colour)
{
	Gfx_debug_shape *sph = NULL;
	if (self->nshapes < DEBUG_MAX_SHAPES)
	{
		sph = self->shapes + self->nshapes++;
		memset (sph, 0, sizeof (*sph));
		sph->col = colour;
		sph->type = type;
	}
	return sph;
}
void
gfx_dbg_point (Gfx_debug *self, uint32_t colour, float *origin)
{
	Gfx_debug_shape *sh = alloc_shape (self, GFD_POINT, colour);
	if (NULL == sh)
	{
		return;
	}
	for (uint32_t i = 0; i < 3; i++) sh->pt.xyz[i] = origin[i];
}
void
gfx_dbg_line (Gfx_debug *self, uint32_t colour, float *a, float *b)
{
	Gfx_debug_shape *sh = alloc_shape (self, GFD_LINE, colour);
	if (NULL == sh)
	{
		return;
	}
	for (uint32_t i = 0; i < 3; i++)
	{
		sh->ln.a[i] = a[i];
		sh->ln.b[i] = b[i];
	}
}
void
gfx_dbg_sphere (Gfx_debug *self, uint32_t colour, float *origin, float radius)
{
	Gfx_debug_shape *sh = alloc_shape (self, GFD_SPHERE, colour);
	if (NULL == sh)
	{
		return;
	}
	for (uint32_t i = 0; i < 3; i++) sh->sph.xyz[i] = origin[i];
	sh->sph.radius = radius;
}
void
gfx_dbg_capsule (
	Gfx_debug *self,
	uint32_t colour,
	float *head,
	float *tail,
	float radius
){
	Gfx_debug_shape *sh = alloc_shape (self, GFD_CAPSULE, colour);
	if (NULL == sh)
	{
		return;
	}
	for (uint32_t i = 0; i < 3; i++)
	{
		sh->cap.head[i] = head[i];
		sh->cap.tail[i] = tail[i];
	}
	sh->cap.radius = radius;
}
void
gfx_dbg_draw (Gfx_debug *self, M4x4 world)
{
	for (uint32_t i = 0; i < self->nshapes; i++)
	{
		Gfx_debug_shape *sh = self->shapes + i;
		M4x4 tform, local;
		/*Plug in the vertex attributes*/
		const float rp = 1.0/255.0;
		float r = rp*((sh->col>>24)&0xff);
		float g = rp*((sh->col>>16)&0xff);
		float b = rp*((sh->col>>8)&0xff);
		float i = rp*(sh->col&0xff);
		glColor4f (i*r, i*g, i*b, 1.0);
		/*Issue appropriate draw commands*/
		switch (sh->type)
		{
		case GFD_POINT:
#ifndef USING_KOS
			glVertexPointer (3, GL_FLOAT, sizeof (sh->pt.xyz), sh->pt.xyz);
			glPointSize (8);
			glDrawArrays (GL_POINTS, 0, 1);
#endif
			break;
		case GFD_LINE:
			glVertexPointer (3, GL_FLOAT, sizeof (sh->ln.a), sh->ln.a);
			glDrawArrays (GL_LINES, 0, 2);
			break;
		case GFD_SPHERE:
			/*Build local space matrix for the sphere*/
			vec_set (local[0], sh->sph.radius, 0, 0, 0);
			vec_set (local[1], 0, sh->sph.radius, 0, 0);
			vec_set (local[2], 0, 0, sh->sph.radius, 0);
			vec_set (local[3], 0, 0, 0, 1);
			m4x4_displace (local,
				sh->sph.xyz[0],
				sh->sph.xyz[1],
				sh->sph.xyz[2]
			);
			/*Transform it into worldspace and draw*/
			transform_into_system (tform, world, local);
			load_modelview (tform);
			glVertexPointer (3, GL_FLOAT, sizeof (self->sph[0]), self->sph);
			glDrawElements (
				GL_LINES,
				2*3*DEBUG_SPHERE_POINTS,
				GL_UNSIGNED_BYTE,
				self->sph_ndx
			);
			load_modelview (world);
			break;
		case GFD_CAPSULE:
			/*Not implemented*/
			break;
		default:
			assert (0 && "undefined debug shape");
		}
	}
	/*Clear the shapes from this frame*/
	self->nshapes = 0;
}
void
gfx_dbg_init (Gfx_debug *self)
{
	memset (self, 0, sizeof (*self));
	{/*Generate a unit sphere*/
		const float inc = 360.0/DEBUG_SPHERE_POINTS;
		uint32_t n = 0;
		uint32_t k = 0;
		for (uint32_t i = 0; i < 3; i++)
		{
			float ang = 0;
			for (uint32_t j = 0; j < DEBUG_SPHERE_POINTS; j++)
			{
				float *v = self->sph[k];
				static uint32_t ui[] = {1, 2, 0};
				static uint32_t vi[] = {2, 0, 1};
				float s, c;
				/*Generate point on the circle of this axis*/
				sincosf (DEG2RAD(ang), &s, &c);
				v[ui[i]] = c;
				v[vi[i]] = s;
				v[i] = 0;
				/*Add line to the vertices*/
				self->sph_ndx[n + 0] = k;
				self->sph_ndx[n + 1] = i*DEBUG_SPHERE_POINTS + (k + 1)%DEBUG_SPHERE_POINTS;
				/*Prepare for next point*/
				ang += inc;
				n += 2;
				k++;
			}
		}
	}
}


