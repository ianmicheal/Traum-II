#include "local.h"

Kr_mesh *
kr_mesh_from_file (FILE *fp)
{
	Kr_mesh *mesh = (Kr_mesh *)cache_alloc (&_kr.meshes);
	memset (mesh, 0, sizeof (*mesh));
	/*Read the header*/
	fread (&mesh->nverts, sizeof (mesh->nverts), 1, fp);
	fread (&mesh->nindices, sizeof (mesh->nindices), 1, fp);
	fread (&mesh->nfaces, sizeof (mesh->nfaces), 1, fp);
	/*Read the vertices and faces*/
	mesh->verts = kr_alloc (sizeof (mesh->verts[0])*mesh->nverts, 16);
	fread (mesh->verts, sizeof (mesh->verts[0]), mesh->nverts, fp);
	mesh->indices = kr_alloc (sizeof (mesh->indices[0])*mesh->nindices, 16);
	fread (mesh->indices, sizeof (mesh->indices[0]), mesh->nindices, fp);
	mesh->faces = kr_alloc (sizeof (mesh->faces[0])*mesh->nfaces, 16);
	fread (mesh->faces, sizeof (mesh->faces[0]), mesh->nfaces, fp);
	return mesh;
}

void
kr_collider_active (Kr_collider *self, bool flag)
{
	bool state = (self->flags&KR_CF_ACTIVE) != 0;
	if (flag == state)
	{
		return;
	}
	if (flag) kr_activate (self);
	else kr_disable (self);
}
