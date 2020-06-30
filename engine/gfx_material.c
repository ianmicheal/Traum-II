#include "gfx_local.h"
#include "public/lex.h"

void
gfx_material_parse (const char *script)
{
	Parser_state ps;
	parser_init (&ps, script);
	while (1)
	{
		Material mat;
		memset (&mat, 0, sizeof (mat));
		/*Check for EOF*/
		if (parser_check (&ps, ""))
		{
			break;
		}
		/*Expect an ident for the material name*/
		strcpy (mat.name, parser_expect_type (&ps, LEX_IDENT));
		parser_consume (&ps);
		/*Parse material body*/
		parser_expect (&ps, "{");
		do
		{
			if (parser_check (&ps, "map"))
			{
				strcpy (mat.map, parser_expect_type (&ps, LEX_STRING));
				parser_consume (&ps);
				continue;
			}
			if (parser_check (&ps, "surface"))
			{
				const char *x = parser_expect_type (&ps, LEX_IDENT);
				if (strcmp ("tile", x))
				{
					mat.surface = 1;
				}
				parser_consume (&ps);
				continue;
			}
			/*Parse animation body*/
			if (parser_check (&ps, "animated"))
			{
				Material_anim anim;
				memset (&anim, 0, sizeof (anim));
				parser_expect (&ps, "{");
				do
				{
					if (parser_check (&ps, "size"))
					{
						anim.w = atoi (
							parser_expect_type (&ps, LEX_NUMBER)
						);
						parser_consume (&ps);
						parser_expect (&ps, ",");
						anim.h = atoi (
							parser_expect_type (&ps, LEX_NUMBER)
						);
						parser_consume (&ps);
						continue;
					}
					if (parser_check (&ps, "frames"))
					{
						anim.nframes = atoi (
							parser_expect_type (&ps, LEX_NUMBER)
						);
						parser_consume (&ps);
						continue;
					}
					if (parser_check (&ps, "fps"))
					{
						anim.fps = atof (
							parser_expect_type (&ps, LEX_NUMBER)
						);
						parser_consume (&ps);
						continue;
					}
				}
				while (!parser_check (&ps, "}"));
				/*Finalise the animation*/
				if (anim.fps <= 0) anim.fps = 1.0;
				if (anim.nframes <= 0) anim.nframes = 0;
				anim.length = ((1000.0/anim.fps)/1000.0)*anim.nframes;
				/*Move the finished animated block to the heap*/
				Material_anim *a = malloc (sizeof (*a));
				memcpy (a, &anim, sizeof (*a));
				mat.anim = a;
				continue;
			}
		}
		while (!parser_check (&ps, "}"));
		/*Move the finished material to the heap*/
		Material *m = malloc (sizeof (*m));
		memcpy (m, &mat, sizeof (*m));
		dlist_insert_front (&m->chain, &_gfx.materials);
		printf ("compiled material %s\n", m->name);
	}
}
Material *
material_find (const char *name)
{
	Dlist *n = dlist_front (&_gfx.materials);
	while (n != dlist_end (&_gfx.materials))
	{
		Material *m = LIST_DATA(n, Material, chain);
		if (!strcmp (name, m->name))
		{
			return m;
		}
		n = n->next;
	}
	return NULL;
}
Gfx_handle
gfx_material_alloc (const char *name)
{
	Material *m = material_find (name);
	if (m)
	{
		m->image = gfx_image (m->map);
		m->refs++;
	}
	return m;
}
void
gfx_material_release (Gfx_handle self)
{
	Material *m = (Material *)self;
	if (m->image)
	{
		gfx_image_release (m->image);
		m->image = NULL;
	}
	m->refs--;
}

