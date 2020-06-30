#include "gfx_local.h"
#include "public/engine.h"

static void
level_destroy (Level *lvl)
{
	for (uint32_t i = 0; i < lvl->nmeshes; i++)
	{
		Level_mesh *mesh = lvl->meshes + i;
		for (uint32_t j = 0; j < mesh->nbatches; j++)
		{
			Level_batch *batch = mesh->batches + j;
			gfx_material_release (batch->mat);
			free (batch->verts);
			batch->verts = NULL;
		}
		free (mesh->batches);
		mesh->batches = NULL;
	}
	free (lvl->meshes);
	lvl->meshes = NULL;
}
static bool
level_loader (Gfx_model *mdl, FILE *fp)
{
	Level lvl;
	Level_header hdr;
	uint32_t num;
	/*Read out the full header*/
	fseek (fp, 0, SEEK_SET);
	fread (&hdr, 1, sizeof (hdr), fp);
	if (LEVEL_VERSION != hdr.version)
	{
		printf ("Bad level version\n");
		return false;
	}
	fseek (fp, hdr.meshes, SEEK_SET);
	/*Zero out the work area*/
	memset (&lvl, 0, sizeof (lvl));
	fread (&num, 1, sizeof (num), fp);
	/*Read out each mesh*/
	lvl.meshes = malloc (num*sizeof (lvl.meshes[0]));
	lvl.nmeshes = num;
	for (uint32_t i = 0; i < num; i++)
	{
		Level_mesh *mesh = lvl.meshes + i;
		uint32_t nbatches = 0;
		/*Read each batch*/
		fread (&nbatches, 1, sizeof (nbatches), fp);
		mesh->batches = malloc (nbatches*sizeof (mesh->batches[0]));
		mesh->nbatches = nbatches;
		for (uint32_t j = 0; j < nbatches; j++)
		{
			Level_batch *batch = mesh->batches + j;
			char name[32];
			uint32_t len = 0;
			/*Grab the material name
			Need to ensure name does not exceed bounds*/
			fread (&len, 1, sizeof (len), fp);
			fread (name, 1, len, fp);
			name[len] = '\0';
			/*Load the material for this batch*/
			batch->mat = (Material *)gfx_material_alloc (name);
			if (!batch->mat)
			{
				printf ("Failed to load material %s\n", name);
			}
			/*Read in the vertices*/
			fread (&len, 1, sizeof (len), fp);
			batch->verts = malloc (len*sizeof (batch->verts[0]));
			batch->nverts = len;
			fread (batch->verts, len, sizeof (batch->verts[0]), fp);
		}
	}
	/*Store everything on the model and return*/
	mdl->type = MT_LEVEL;
	mdl->lvl = lvl;
	return true;
}
Gfx_handle
gfx_model (const char *file)
{
	static struct _Loader
	{
		bool (*func) (Gfx_model *, FILE *);
		uint32_t magick;
	} loaders[] = {
		{level_loader, LEVEL_MAGICK},
		{NULL, 0}
	};
	Gfx_model *mdl;
	FILE *fp;
	/*Create model record*/
	mdl = malloc (sizeof (*mdl));
	if (!mdl)
	{/*Should be fatal*/
		return NULL;
	}
	memset (mdl, 0, sizeof (*mdl));
	dlist_insert_front (&mdl->chain, &_gfx.models);
	mdl->refs = 0;
	mdl->type = MT_ERROR;
	/*Try to open a stream to the file*/
	fp = sys_fopen (file);
	if (NULL != fp)
	{/*Now try to figure out which type it is via the magic key*/
		uint32_t x, i;
		fread (&x, 1, sizeof (x), fp);
		for (i = 0; loaders[i].func != NULL; i++)
		{
			if (x != loaders[i].magick)
			{/*No dice, try the next one*/
				continue;
			}
			/*Found it!*/
			loaders[i].func (mdl, fp);
			break;
		}
	}
	fclose (fp);
	return mdl;
}
void
gfx_model_acquire (Gfx_handle model)
{
	((Gfx_model *)model)->refs++;
}
void
gfx_model_release (Gfx_handle model)
{
	Gfx_model *self = (Gfx_model *)model;
	if (self->refs != 0)
	{
		self->refs--;
		return;
	}
	model_free (model);
}
void
model_free (Gfx_model *model)
{
	dlist_remove (&model->chain);
	switch (model->type)
	{
	case MT_LEVEL: {
		Level *lvl = &model->lvl;
		level_destroy (lvl);
		}
	default:
		break;
	}
	free (model);
}

