#include "quakedef.h"
#include "cmodel.h"



/*
=============================================================

	BRUSH MODELS

=============================================================
*/

qboolean mtexenabled = FALSE;

void GL_SelectTexture( GLenum target );

void GL_DisableMultitexture( void )
{
	if (mtexenabled)
	{
		qglDisable(GL_TEXTURE_2D);
		GL_SelectTexture(TEXTURE0_SGIS);
		mtexenabled = FALSE;
	}
}

void GL_EnableMultitexture( void )
{
	if (gl_mtexable)
	{
		GL_SelectTexture(TEXTURE1_SGIS);
		qglEnable(GL_TEXTURE_2D);
		mtexenabled = TRUE;
	}
}

// TODO: Implement

/*
=============
R_DrawWorld
=============
*/
void R_DrawWorld( void )
{
	// TODO: Implement
}

/*
===============
R_MarkLeaves
===============
*/
void R_MarkLeaves( void )
{
	byte* vis;
	mnode_t* node;
	int		i;
	byte	solid[4096];

	if (r_oldviewleaf == r_viewleaf && !r_novis.value)
		return;

	if (mirror)
		return;

	r_visframecount++;
	r_oldviewleaf = r_viewleaf;

	if (r_novis.value)
	{
		vis = solid;
		memset(solid, 0xff, (cl.worldmodel->numleafs + 7) >> 3);
	}
	else
		vis = Mod_LeafPVS(r_viewleaf, cl.worldmodel);

	for (i = 0; i < cl.worldmodel->numleafs; i++)
	{
		if (vis[i >> 3] & (1 << (i & 7)))
		{
			node = (mnode_t*)&cl.worldmodel->leafs[i + 1];
			do
			{
				if (node->visframe == r_visframecount)
					break;
				node->visframe = r_visframecount;
				node = node->parent;
			} while (node);
		}
	}
}

// TODO: Implement

/*
==================
GL_BuildLightmaps

Builds the lightmap texture
with all the surfaces from all brush models
==================
*/
void GL_BuildLightmaps( void )
{
	// TODO: Implement
}

// TODO: Implement


// Init the decal pool
void R_DecalInit( void )
{
	// TODO: Implement
}






void R_DecalRemoveAll( int textureIndex )
{
	// TODO: Implement
}