#include "gfx_local.h"
#include "kmg.h"

typedef struct _Vertex
{
	Vec xyz;
	float uv[2];
	uint8_t clr[4];
}Vertex;

static Vertex _sprite[] = {
	{{0,-1, 1}, {0, 0}, {0xff, 0xff, 0xff, 0xff}},
	{{0, 1, 1}, {1, 0}, {0xff, 0xff, 0xff, 0xff}},
	{{0, 1,-1}, {1, 1}, {0xff, 0xff, 0xff, 0xff}},
	{{0,-1,-1}, {0, 1}, {0xff, 0xff, 0xff, 0xff}}
};

static Vec _fog_colour = {1, 1, 1, 1};
struct _Gfx_state _gfx;

void
load_modelview (M4x4 matrix)
{
	static M4x4 conv =
	{/*conversion to Z forward -> X forward space*/
		{ 0, 0,-1, 0},
		{-1, 0, 0, 0},
		{ 0, 1, 0, 0},
		{ 0, 0, 0, 1}
	};
	M4x4 mm;
	m4x4_multiply (mm, matrix, conv);
	glLoadMatrixf (m4x4_as_pointer (mm));
}
static inline void
transform_from_node (Vec *dst, Gfx_entity *e)
{/*Scale is factored in later with other matrix effects*/
	m4x4_from_angles (dst, e->rotation);
	m4x4_displace (dst, e->position[0], e->position[1], e->position[2]);
}
static void
apply_statal (Gfx_statal *to)
{/*Emits the minimal amount of state changes needed to transition*/
	assert (to != NULL && "NULL 'to' statal");
	uint32_t changed = _gfx.curs.caps^to->caps;
	if (changed)
	{/*Something changed, so do the whole song and dance to an optimal state*/
		uint32_t disabled = changed&_gfx.curs.caps;
		uint32_t enabled = changed&to->caps;
		if (disabled)
		{
			if (disabled&SC_BLEND) glDisable (GL_BLEND);
			if (disabled&SC_DEPTH) glDisable (GL_DEPTH_TEST);
			if (disabled&SC_CULLING) glDisable (GL_CULL_FACE);
			if (disabled&SC_FOG) glDisable (GL_FOG);
		}
		if (enabled)
		{
			if (enabled&SC_BLEND) glEnable (GL_BLEND);
			if (enabled&SC_DEPTH) glEnable (GL_DEPTH_TEST);
			if (enabled&SC_CULLING) glEnable (GL_CULL_FACE);
			if (enabled&SC_FOG) glEnable (GL_FOG);
		}
		/*Update the capabilities*/
		_gfx.curs.caps = to->caps;
	}
	if (_gfx.curs.blend_mode != to->blend_mode)
	{/*Blend needs updating it seems*/
		static GLuint modes[] = {
			GL_ONE,
			GL_ZERO,
			GL_SRC_COLOR,
			GL_SRC_ALPHA,
			GL_ONE_MINUS_SRC_ALPHA,
			GL_DST_COLOR,
			GL_DST_ALPHA,
			GL_ONE_MINUS_DST_ALPHA
		};
		glBlendFunc (modes[to->blend.src], modes[to->blend.dst]);
		_gfx.curs.blend_mode = to->blend_mode;
	}
}
static void
bind_texture (Gfx_image *tex)
{	/*Just clear the texture*/
	if (NULL == tex)
	{
		glBindTexture (GL_TEXTURE_2D, 0);
		_gfx.bound = 0;
		return;
	}
	/*Only update the texture if it's needed*/
	GLuint h = tex->handle;
	if (_gfx.bound != h)
	{
		glBindTexture (GL_TEXTURE_2D, h);
		_gfx.bound = h;
	}
	/*Animated textures need a texture matrix
	Is this a good idea? or should the animation matrix be loaded separate?
	seems like a gotcha*/
	glMatrixMode (GL_TEXTURE);
	if (tex->format&IF_ANIMATED)
	{
		M4x4 tm;
		
		float mod = fmod (_gfx.time, tex->ani.length)/tex->ani.length; 
		float frame = tex->ani.nframes*mod;
		float y = floor (frame*tex->ani.w);
		float x = floor (frame - y/tex->ani.w);
		
		vec_set (tm[0], tex->ani.w, 0, 0, 0);
		vec_set (tm[1], 0, tex->ani.h, 0, 0);
		vec_set (tm[2], 0, 0, 1, 0);
		vec_set (tm[3], x*tex->ani.w, y*tex->ani.h, 0, 1);
		glLoadMatrixf (m4x4_as_pointer (tm));
	}
	else glLoadIdentity ();
	glMatrixMode (GL_MODELVIEW);
}
static void
draw_model (Gfx_model *mdl, int32_t submodel)
{
	switch (mdl->type)
	{
	case MT_LEVEL: {
			Level *lvl = &mdl->lvl;
			/*Ensure the submodel is a valid index*/
			if (!(0 <= submodel && submodel < lvl->nmeshes))
			{
				return;
			}
			/*Enable relevant state and draw the requested submodel*/
			Level_mesh *mesh = lvl->meshes + submodel;
#ifndef USING_KOS
			glEnableClientState (GL_TEXTURE_COORD_ARRAY);
			glEnable (GL_TEXTURE_COORD_ARRAY);
#endif
			for (uint32_t i = 0; i < mesh->nbatches; i++)
			{
				Level_batch *batch = mesh->batches + i;
				bind_texture (batch->mat->image);
				glVertexPointer (
					3, GL_FLOAT,
					sizeof (batch->verts[0]),
					batch->verts[0].xyz
				);	
				glTexCoordPointer (
					2, GL_FLOAT,
					sizeof (batch->verts[0]),
					batch->verts[0].uv
				);
				glDrawArrays (GL_TRIANGLES, 0, batch->nverts);
			}
#ifndef USING_KOS
			/*Return to default state*/
			glDisable (GL_TEXTURE_COORD_ARRAY);
			glDisableClientState (GL_TEXTURE_COORD_ARRAY);
#endif
		} break;
	default:
		assert (NULL && "Unknown model type!");
		break;
	}
}
void
draw_node (Gfx_entity *e, Vec *space)
{
	M4x4 tform;
	/*Transform into the given coordinate space if supplied,
	else just leave everything alone in its local space*/
	if (space != NULL)
	{
		M4x4 tmp;
		transform_from_node (tmp, e);
		transform_into_system (tform, space, tmp);
	}
	else transform_from_node (tform, e);
	/*Apply any billboard effect*/
	if (e->flags&EF_BILLBOARD)
	{/*NB: Potential optimisation:
		Could just transform position into world space and use 
		the identity for rotation. Would need a refactor*/
		tform[0][0] = 1.0;
		tform[1][0] = 0.0;
		tform[2][0] = 0.0;
		
		tform[0][1] = 0.0;
		tform[1][1] = 1.0;
		tform[2][1] = 0.0;
		
		tform[0][2] = 0.0;
		tform[1][2] = 0.0;
		tform[2][2] = 1.0;
	}
	else if (e->flags&EF_BILLBOARD_YAW)
	{
		tform[1][0] = 0.0;
		tform[0][1] = 0.0;
		tform[1][1] = 1.0;
		tform[2][1] = 0.0;
		tform[1][2] = 0.0;
	}
	/*Apply any scaling*/
	if (e->flags&EF_SCALED)
	{
		M4x4 tmp, scale;
		vec_set (scale[0], e->scale[0], 0, 0, 0);
		vec_set (scale[1], 0, e->scale[1], 0, 0);
		vec_set (scale[2], 0, 0, e->scale[2], 0);
		vec_set (scale[3], 0, 0, 0, 1);
		m4x4_multiply (tmp, scale, tform);
		load_modelview (tmp);
	}
	else load_modelview (tform);
	/*Update graphics state*/
	apply_statal (&e->state);
	{/*Apply colour*/
		const float rp = 1.0/65535.0;
		float r = rp*(e->colour[3]*e->colour[0]);
		float g = rp*(e->colour[3]*e->colour[1]);
		float b = rp*(e->colour[3]*e->colour[2]);
		glColor4f (r, g, b, 1.0);
	}
	/*Draw the model*/
	switch (e->type)
	{
	case ET_SPRITE: {
		Gfx_image *tex = (Gfx_image *)e->texture;
#ifndef USING_KOS
		/*Enable UV coordinates and set up buffers*/
		glEnableClientState (GL_TEXTURE_COORD_ARRAY);
		glEnable (GL_TEXTURE_COORD_ARRAY);
#endif
		glVertexPointer (3, GL_FLOAT, sizeof (Vertex), _sprite[0].xyz);	
		glTexCoordPointer (2, GL_FLOAT, sizeof (Vertex), _sprite[0].uv);
		/*Assign texture and draw*/
		bind_texture (tex);
		glDrawArrays (GL_TRIANGLE_FAN, 0, 4);
		/*Return to default state*/
#ifndef USING_KOS
		glDisable (GL_TEXTURE_COORD_ARRAY);
		glDisableClientState (GL_TEXTURE_COORD_ARRAY);
#endif
		break;
	}
	case ET_MODEL:
		draw_model ((Gfx_model *)e->model, e->submodel);
		break;
	case ET_USER: {
		static GLuint modes[] = {
			GL_TRIANGLES,
			GL_TRIANGLE_FAN,
			GL_TRIANGLE_STRIP
		};
		/*Let the user modify the model before drawing*/
		Gfx_user *u = (Gfx_user *)&e->user;
		if (u->render)
		{
			if (u->render (u))
			{/*User requested a pass!*/
				break;
			}
		}
		/*Set everything up then draw it*/
		uint8_t *verts = u->verts;
		size_t size = u->size;
		size_t ofs = 0;
		if (u->desc&VB_XYZ)
		{
			glVertexPointer(3, GL_FLOAT, size, verts);
			if (u->desc&VB_UV_XYZ)
			{
#ifndef USING_KOS
				glEnableClientState (GL_TEXTURE_COORD_ARRAY);
				glEnable (GL_TEXTURE_COORD_ARRAY);
#endif
				glTexCoordPointer (2, GL_FLOAT, size, verts);
			}
			ofs += sizeof (Vec);
		}
		if (u->desc&VB_UV)
		{
#ifndef USING_KOS
			glEnableClientState (GL_TEXTURE_COORD_ARRAY);
			glEnable (GL_TEXTURE_COORD_ARRAY);
#endif
			glTexCoordPointer (2, GL_FLOAT, size, verts + ofs);
			ofs += 2*sizeof (float);
		}
		if (u->desc&VB_COLOUR)
		{
#ifndef USING_KOS
			glEnableClientState (GL_COLOR_ARRAY);
			glEnable (GL_COLOR_ARRAY);
#endif
			glColorPointer (3, GL_UNSIGNED_BYTE, size, verts + ofs);
			ofs += 4;
		}
		/*Apply texture matrix*/
		bind_texture (e->texture);
		if (e->tm.enabled)
		{
			M4x4 tm;
			float s, c;
			/*Craft the texture matrix*/
			sincosf (DEG2RAD(e->tm.angle), &s, &c);
			vec_set (tm[0], c*e->tm.scale[0],-s, 0, 0);
			vec_set (tm[1], s, c*e->tm.scale[1], 0, 0);
			vec_set (tm[2], 0, 0, 1, 0);
			vec_set (tm[3], e->tm.scroll[0], e->tm.scroll[1], 0, 1);
			/*Upload it*/
			glMatrixMode (GL_TEXTURE);
			glLoadMatrixf (m4x4_as_pointer (tm));
			glMatrixMode (GL_MODELVIEW);
		}
		/*Draw the model*/
		if (u->indices)
		{
			for (uint32_t i = 0; i < u->nindices; i++)
			{
				Gfx_user_index *ndx = u->indices + i;
				glDrawElements (
					modes[ndx->mode],
					ndx->length,
					GL_UNSIGNED_BYTE,
					ndx->data
				);
			}
		}
		else glDrawArrays (modes[u->mode], 0, u->nverts);
		/*Return to default state*/
		if (e->tm.enabled)
		{
			glMatrixMode (GL_TEXTURE);
			glLoadIdentity ();
			glMatrixMode (GL_MODELVIEW);
		}
#ifndef USING_KOS
		if (u->desc&(VB_UV|VB_UV_XYZ))
		{
			glDisable (GL_TEXTURE_COORD_ARRAY);
			glDisableClientState (GL_TEXTURE_COORD_ARRAY);
		}
		if (u->desc&VB_COLOUR)
		{
			glDisable (GL_COLOR_ARRAY);
			glDisableClientState (GL_COLOR_ARRAY);
		}
#endif
		break;
	}
	default:
		assert (NULL && "Unknown entity type!");
		break;
	}
}
void
draw_graph (Dlist *graph, Vec *space)
{
	Dlist *node = dlist_front (graph);
	while (node != dlist_end (graph))
	{
		Dlist *next = node->next;
		Gfx_entity *e = LIST_DATA(node, Gfx_entity, chain);
		if (e->flags&EF_SORTED)
		{/*Defer drawing until later*/
			uint32_t lv = e->sort;
			if (MAX_SORT <= lv)
			{
				lv = MAX_SORT - 1;
			}
			/*Indices closer to the head require less iteration*/
			lv = MAX_SORT - lv - 1;
			if (lv < _gfx.active_sort)
			{/*Mark this level as having work*/
				_gfx.active_sort = lv;
			}
			dlist_insert_front (node, _gfx.sorted + lv);
		}
		else draw_node (e, space);
		node = next;
	}
	/*Draw sorted nodes*/
	glDepthMask (GL_FALSE);
	for (uint32_t i = _gfx.active_sort; i < MAX_SORT; i++)
	{
		Dlist *head = _gfx.sorted + i;
		/*Ensure there's something to draw*/
		if (dlist_empty (head))
		{
			continue;
		}
		/*Draw everything on this level*/
		node = dlist_front (head);
		while (node != dlist_end (head))
		{
			draw_node (LIST_DATA(node, Gfx_entity, chain), space);
			node = node->next;
		}
		/*Reset the list*/
		dlist_init (head);
	}
	glDepthMask (GL_TRUE);
	/*Avoid (potential) redundant work next frame*/
	_gfx.active_sort = MAX_SORT;
}
void
gfx_draw (Gfx_camera *camera, float dt)
{
	M4x4 pm, mm;
	/*Bump timer*/
	_gfx.time += dt;
	/*Set up the matrices*/
	glMatrixMode (GL_PROJECTION);
	glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glColor4f (1, 1, 1, 1);
	m4x4_from_perspective (pm, camera->fovx, camera->fovy, 1, 2000);
	glLoadMatrixf (m4x4_as_pointer (pm));
	glMatrixMode (GL_MODELVIEW);
	m4x4_from_angles (mm, camera->rotation);
	/*Draw local space*/
	draw_graph (&_gfx.sg.local, mm);
	/*Now world space*/
	glClear (GL_DEPTH_BUFFER_BIT);
	m4x4_translate (
		mm,
		-camera->position[0],
		-camera->position[1],
		-camera->position[2]
	);
	draw_graph (&_gfx.sg.world, mm);
	/*Draw debugging shapes*/
	gfx_dbg_draw (&_gfx.dbg, mm);
	/*Camera space always gets drawn on top of everything else*/
	glDisable (GL_DEPTH_TEST);
	draw_graph (&_gfx.sg.camera, NULL);
	glEnable (GL_DEPTH_TEST);
	/*Reset the graphs*/
	dlist_init (&_gfx.sg.camera);
	dlist_init (&_gfx.sg.local);
	dlist_init (&_gfx.sg.world);
}

void
gfx_scene_node_init (Gfx_entity *entity)
{
	memset (entity, 0, sizeof (*entity));
	for (uint32_t i = 0; i < 4; i++) entity->colour[i] = 0xff;
	entity->state = _gfx.defs;
}
void
gfx_scene_node_insert (uint32_t list, Gfx_entity *entity)
{
	switch (list)
	{
	case SG_WORLD:
		dlist_insert_front (&entity->chain, &_gfx.sg.world);
		break;
	case SG_LOCAL:
		dlist_insert_front (&entity->chain, &_gfx.sg.local);
		break;
	case SG_CAMERA:
		dlist_insert_front (&entity->chain, &_gfx.sg.camera);
		break;
	default:
		assert (0 && "Invalid scenegraph!");
		break;
	}
}
void
gfx_scene_fog_colour (float r, float g, float b)
{
	vec_set (_fog_colour, r, g, b, 1.0);
}
void
gfx_dbg_addpoint (uint32_t col, float *origin)
{
	gfx_dbg_point (&_gfx.dbg, col, origin);
}
void
gfx_dbg_addline (uint32_t col, float *a, float *b)
{
	gfx_dbg_line (&_gfx.dbg, col, a, b);
}
void
gfx_dbg_addsphere (uint32_t col, float *origin, float radius)
{
	gfx_dbg_sphere (&_gfx.dbg, col, origin, radius);
}
void
gfx_dbg_addcapsule (uint32_t col, float *a, float *b, float radius)
{
	gfx_dbg_capsule (&_gfx.dbg, col, a, b, radius);
}
void
gfx_init (void)
{	/*Initialise gfx state*/
	memset (&_gfx, 0, sizeof (_gfx));
	dlist_init (&_gfx.sg.camera);
	dlist_init (&_gfx.sg.local);
	dlist_init (&_gfx.sg.world);
	dlist_init (&_gfx.images);
	dlist_init (&_gfx.models);
	dlist_init (&_gfx.materials);
	for (uint32_t i = 0; i < MAX_SORT; i++) dlist_init (_gfx.sorted + i);
	_gfx.active_sort = MAX_SORT;
	/*Set up default GL state*/
	glClearColor (1.0, 0.5, 0.25, 1.0);
	glEnableClientState (GL_VERTEX_ARRAY);
	glEnable (GL_DEPTH_TEST);
#ifndef USING_KOS
	/*KGL does not use these...*/
	glEnable (GL_VERTEX_ARRAY);
	glEnable (GL_TEXTURE_2D);
#endif
	glDepthFunc (GL_LEQUAL);
	{/*Compose default state*/
		Gfx_statal s;
		s.caps = SC_BLEND|SC_CULLING|SC_TEXTURE;
		s.blend.src = SB_SRC_ALPHA;
		s.blend.dst = SB_ONE_MINUS_SRC_ALPHA;
		/*Make the default one current*/
		_gfx.defs = s;
		apply_statal (&s);
	}
	gfx_dbg_init (&_gfx.dbg);
}
void
gfx_shutdown (void)
{
	{/*Free models*/
		Dlist *n = dlist_front (&_gfx.models);
		while (n != dlist_end (&_gfx.models))
		{
			Dlist *next = n->next;
			model_free (LIST_DATA(n, Gfx_model, chain));
			n = next;
		}
	}
	{/*Free images*/
		Dlist *n = dlist_front (&_gfx.images);
		while (n != dlist_end (&_gfx.images))
		{
			Dlist *next = n->next;
			image_free (LIST_DATA(n, Gfx_image, chain));
			n = next;
		}
	}
	glDisable (GL_BLEND);
	glDisable (GL_CULL_FACE);
	glDisable (GL_TEXTURE_2D);
#ifndef USING_KOS
	glDisable (GL_VERTEX_ARRAY);
	glDisableClientState (GL_VERTEX_ARRAY);
#endif
}

