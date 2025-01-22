// cl_tent.c -- client side temporary entities

#include "quakedef.h"
#include "cl_tent.h"

// TODO: Implement

/*
=================
CL_InitTEnts

Initialize TE system
=================
*/
void CL_InitTEnts( void )
{
	// TODO: Implement
}

int ModelFrameCount( model_t* model )
{
	int count;

	count = 1;

	if (model)
	{
		if (model->type == mod_sprite)
		{
			msprite_t* psprite;

			psprite = (msprite_t*)model->cache.data;
			count = psprite->numframes;
		}
		else if (model->type == mod_studio)
		{
//			count = R_StudioBodyVariations(model); TODO: Implement
		}

		if (count < 1)
			count = 1;
	}

	return count;
}

// TODO: Implement

/*
=================
CL_TempEntInit

Initialize temp entities
=================
*/
void CL_TempEntInit( void )
{
	// TODO: Implement
}

// TODO: Implement

/*
=============================================================

  SPRITE MODELS

=============================================================
*/

/*
================
R_GetSpriteFrame
================
*/
mspriteframe_t* R_GetSpriteFrame( msprite_t* pSprite, int frame )
{
	mspriteframe_t* pspriteframe;

	// Invalid frame
	if ((frame >= pSprite->numframes) || (frame < 0))
	{
		Con_Printf("Sprite: no such frame %d\n", frame);
		frame = 0;
	}

	if (pSprite->frames[frame].type == SPR_SINGLE)
	{
		pspriteframe = pSprite->frames[frame].frameptr;
	}
	else
	{
		pspriteframe = NULL;
	}

	return pspriteframe;
}