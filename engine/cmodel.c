// cmodel.c -- model loading

#include "quakedef.h"
#include "CONSOLE.H"
#include "common.h"
#include "cmodel.h"

byte* gPAS;
byte* gPVS;
int gPVSRowBytes;

byte mod_novis[MAX_MAP_LEAFS / 8];

/*
===============================================================================

PVS / PAS

===============================================================================
*/


void Mod_Init( void )
{
#if !defined ( GLQUAKE )
	SW_Mod_Init();
#endif
	memset(mod_novis, 255, sizeof(mod_novis));
}


/*
===================
Mod_DecompressVis
===================
*/
byte* Mod_DecompressVis( byte* in, model_t* model )
{
	static byte decompressed[MAX_MAP_LEAFS / 8];
	int		row;

	row = (model->numleafs + 7) / 8;

	if (!in)
	{
		return mod_novis;
	}

	CM_DecompressPVS(in, decompressed, row);

	return decompressed;
}

byte* Mod_LeafPVS( mleaf_t* leaf, model_t* model )
{
	if (leaf == model->leafs)
		return mod_novis;

	if (gPVS)
	{
		int leafnum = leaf - model->leafs;
		return CM_LeafPVS(leafnum);
	}

	return Mod_DecompressVis(leaf->compressed_vis, model);
}

void CM_DecompressPVS( byte* in, byte* decompressed, int byteCount )
{
	int		c;
	byte*	out;

	if (!in)
	{
		// no vis info, so make all visible
		memcpy(decompressed, mod_novis, byteCount);
		return;
	}

	out = decompressed;
	while (out < decompressed + byteCount)
	{
		if (*in)
		{
			*out++ = *in++;
			continue;
		}

		c = in[1];
		in += 2;
		if (c)
		{
			memset(out, 0, c);
			out += c;
		}
	}
}


byte* CM_LeafPVS( int leafnum )
{
	if (!gPVS)
		return mod_novis;

	return gPVS + gPVSRowBytes * leafnum;
}


byte* CM_LeafPAS( int leafnum )
{
	if (!gPAS)
		return mod_novis;

	return gPAS + gPVSRowBytes * leafnum;
}


void CM_FreePAS( void )
{
	if (gPAS)
		free(gPAS);

	if (gPVS)
		free(gPVS);

	gPAS = NULL;
	gPVS = NULL;
}

/*
===================
CM_CalcPAS

Build a potential audible set.
===================
*/
void CM_CalcPAS( model_t* pModel )
{
	int		rowwords;
	int		actualRowBytes;
	int		i, j, k, l;
	int		index;
	int		num;
	int		bitbyte;
	unsigned int* dest, * src;
	byte* scan;
	int		count, vcount, acount;

	Con_DPrintf("Building PAS...\n");

	CM_FreePAS();

	// Calculate memory: matrix of each to each leaf visibility
	num = (pModel->numleafs + 7) / 8;
	count = pModel->numleafs;
	actualRowBytes = (num + 3) & 0xFFFFFFFC;	// 4-byte align
	rowwords = actualRowBytes / 4;
	gPVSRowBytes = actualRowBytes;

	// Alloc PVS
	gPVS = (byte*)calloc(gPVSRowBytes, count);

	// Decompress visibility data
	scan = gPVS;
	vcount = 0;
	for (i = 0; i < count; i++, scan += gPVSRowBytes)
	{
		CM_DecompressPVS(pModel->leafs[i].compressed_vis, scan, num);

		if (i == 0)
			continue;

		for (j = 0; j < count; j++)
		{
			if (scan[j >> 3] & (1 << (j & 7)))
			{
				vcount++;
			}
		}
	}

	// Alloc PAS
	gPAS = (byte*)calloc(gPVSRowBytes, count);

	// Build PAS
	acount = 0;
	scan = gPVS;
	dest = (unsigned int*)gPAS;
	for (i = 0; i < count; i++, scan += gPVSRowBytes, dest += rowwords)
	{
		memcpy(dest, scan, gPVSRowBytes);

		for (j = 0; j < gPVSRowBytes; j++)	// bytes
		{
			bitbyte = scan[j];
			if (bitbyte == 0)
				continue;

			for (k = 0; k < 8; k++)			// bits
			{
				if (!(bitbyte & (1 << k)))
					continue;

				index = j * 8 + k + 1;		// bit index
				if (index <= 0 || index >= count)
					continue;

				src = (unsigned int*)&gPVS[index * gPVSRowBytes];
				for (l = 0; l < rowwords; l++)
				{
					dest[l] |= src[l];
				}
			}
		}

		if (i == 0)
			continue;

		for (j = 0; j < count; j++)
		{
			if (((byte*)dest)[j >> 3] & (1 << (j & 7)))
				acount++;
		}
	}

	Con_DPrintf("Average leaves visible / audible / total: %i / %i / %i\n", vcount / count, acount / count, count);
}

/*
=============
CM_HeadnodeVisible

Returns true if any leaf under headnode has a cluster that
is potentially visible
=============
*/
qboolean CM_HeadnodeVisible( mnode_t* node, byte* visbits )
{
	int leafnum;
	mleaf_t* leaf;

	if (!node || node->contents == CONTENTS_SOLID)
		return FALSE;

	// add an efrag if the node is a leaf
	if (node->contents < 0)
	{
		leaf = (mleaf_t*)node;
		leafnum = (leaf - sv.worldmodel->leafs) - 1;

		if ((visbits[leafnum >> 3] & (1 << (leafnum & 7))) == 0)
			return FALSE;

		return TRUE;
	}

	if (CM_HeadnodeVisible(node->children[0], visbits))
		return TRUE;

	if (CM_HeadnodeVisible(node->children[1], visbits))
		return TRUE;

	return FALSE;
}