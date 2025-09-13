// r_surf.c: surface-related refresh code

#include "quakedef.h"
#include "decal.h"

drawsurf_t	r_drawsurf;

int				lightleft, sourcesstep, blocksize, sourcetstep;
int				lightdelta, lightdeltastep;
int				lightright, lightleftstep, lightrightstep, blockdivshift;
unsigned		blockdivmask;
void* prowdestbase;
unsigned char* pbasesource;
int				surfrowbytes;	// used by ASM files
int				r_stepback;
int				r_lightwidth;
int             r_deltav, r_numhblocks, r_numvblocks;

unsigned char* r_sourcemax;
colorVec* r_lightptr;
word* r_palette;

double lut_8byte_aligner;
byte r_lut[65536];
word red_64klut[65536];
word lut_realigner1[256];
word green_64klut[65536];
word lut_realigner2[256];
word blue_64klut[65536];

//-----------------------------------------------------------------------------
//
// Decal system
//
//-----------------------------------------------------------------------------

// UNDONE: Compress this???
static decal_t			gDecalPool[MAX_DECALS];

void R_DrawInitLut( void );

/*
===============
R_TextureAnimation

Returns the proper texture for a given time and base texture
===============
*/
texture_t* R_TextureAnimation( texture_t* base )
{
	// TODO: Implement
	return NULL;
}

/*
===============
R_DrawSurface
===============
*/
void R_DrawSurface( void )
{
	// TODO: Implement
	
	R_DrawInitLut();

	// TODO: Implement
}

/*
===============
R_DrawInitLut

===============
*/
void R_DrawInitLut( void )
{
	int i, j, k;

	if (r_lut[0xFFFF])
		return; // already initialized

	for (i = 0; i < 256; i++)
	{
		for (j = 0; j < 256; j++)
		{
			k = i + j * 256;
			r_lut[k] = min((i * j) / 192, 255);

			if (is15bit)
			{
				red_64klut[k] = (r_lut[k] << 7) & 0x7C00;
				green_64klut[k] = (r_lut[k] << 2) & 0x03E0;
			}
			else
			{
				red_64klut[k] = (r_lut[k] << 8) & 0xF800;
				green_64klut[k] = (r_lut[k] << 3) & 0x07E0;
			}

			blue_64klut[k] = (r_lut[k] >> 3) & 0x001F;
		}
	}
}

//-----------------------------------------------------------------------------
//
// Decal system
//
//-----------------------------------------------------------------------------


// Init the decal pool
void R_DecalInit( void )
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