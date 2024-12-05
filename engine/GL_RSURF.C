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





void R_DecalRemoveAll( int textureIndex )
{
	// TODO: Implement
}