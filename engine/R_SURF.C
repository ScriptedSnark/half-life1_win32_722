#include "quakedef.h"

double lut_8byte_aligner;
byte r_lut[65536];
word red_64klut[65536];
word lut_realigner1[256];
word green_64klut[65536];
word lut_realigner2[256];
word blue_64klut[65536];


/*
===============
R_DrawInitLut

===============
*/
void R_DrawInitLut( void )
{
	// TODO: Implement
}




void R_DecalRemoveAll( int textureIndex )
{
	// TODO: Implement
}

int DecalListCreate( DECALLIST* pList )
{
	// TODO: Implement
	return 0;
}

// Shoots a decal onto the surface of the BSP.  position is the center of the decal in world coords
// This is called from cl_parse.c, cl_tent.c
void R_DecalShoot( int textureIndex, int entity, int modelIndex, vec_t* position, int flags )
{
	// TODO: Implement
}

void R_CustomDecalShoot( texture_t* ptexture, int playernum, int entity, int modelIndex, vec_t* position, int flags )
{
	// TODO: Implement
}