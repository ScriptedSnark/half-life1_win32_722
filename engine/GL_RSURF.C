#include "quakedef.h"



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
	// TODO: Implement
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