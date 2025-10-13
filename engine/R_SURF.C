// r_surf.c: surface-related refresh code

#include "quakedef.h"
#include "winquake.h"
#include "d_local.h"
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
colorVec 		blocklights[18 * 18];
texture_t* r_basetexture[18];
int             r_offset;
int             sourcevstep;
word* r_palette;

double lut_8byte_aligner;
byte r_lut[65536];
word red_64klut[65536];
word lut_realigner1[256];
word green_64klut[65536];
word lut_realigner2[256];
word blue_64klut[65536];

// UNDONE: Compress this???
static decal_t			gDecalPool[MAX_DECALS];
static int				gDecalCount;					// Pool index
static vec3_t			gDecalPos;

static model_t*			gDecalModel = NULL;
static texture_t* gDecalTexture = NULL;
static int				gDecalSize, gDecalIndex;
static int				gDecalFlags, gDecalEntity;
static const int        gBits[8] = { 0, 0, 1, 1, 2, 2, 2, 3 };

#define DECAL_DISTANCE			4

// Empirically determined constants for minimizing overalpping decals
#define MAX_OVERLAP_DECALS		6

void R_DrawInitLut( void );
void R_SetupTextureIndex( int u );
void R_DrawSurfaceBlock16Fullbright1( void );
void R_DrawSurfaceBlock16Fullbright2( void );
void R_DrawSurfaceBlock16Fullbright3( void );
void R_DrawSurfaceBlock16Fullbright4( void );
void R_DrawSurfaceBlock16Fullbright5( void );
void R_DrawSurfaceBlock16( void );
void R_DrawSurfaceBlock16MMX( void );
void R_DrawSurfaceBlock16Holes( void );

void R_DecalCacheSurface( unsigned char* psource, unsigned short* ppalette, unsigned char* pdest );
void R_DecalRect( decal_t* decals, vrect_t* decalrect, int surfmip );
void R_DrawDecals( decal_t* list, byte* decalbuffer, int s, int t );
void R_DrawScaledDecal( decal_t* list, byte* decalbuffer, int s, int t );
int R_DecalUnProject( decal_t* pdecal, vec_t* position );
void R_DecalCreate( msurface_t* psurface, int textureIndex, int scale, int x, int y );
void R_DecalShoot( int textureIndex, int entity, int modelIndex, vec_t* position, int flags );

unsigned short* R_DecalLightSurface( byte* psource, unsigned short* prowdest );
unsigned short* R_DecalFullbrightSurface( byte* psource, unsigned short* prowdest );

/*
===============
R_AddDynamicLights
===============
*/
void R_AddDynamicLights( void )
{
	msurface_t* surf;
	int			lnum;
	int			sd, td;
	float		dist, rad, minlight;
	vec3_t      local, impact;
	int			s, t;
	int			smax, tmax;
	mtexinfo_t* tex;

	surf = r_drawsurf.surf;
	smax = (surf->extents[0] >> 4) + 1;
	tmax = (surf->extents[1] >> 4) + 1;
	tex = surf->texinfo;

	for (lnum = 0; lnum < MAX_DLIGHTS; lnum++)
	{
		if (!(surf->dlightbits & (1 << lnum)))
			continue;		// not lit by this light

		VectorSubtract(cl_dlights[lnum].origin, currententity->origin, impact);

		rad = cl_dlights[lnum].radius;
		dist = DotProduct(impact, surf->plane->normal) -
			surf->plane->dist;
		rad -= fabs(dist);
		minlight = cl_dlights[lnum].minlight;
		if (rad < minlight)
			continue;
		minlight = rad - minlight;

		local[0] = DotProduct(impact, tex->vecs[0]) + tex->vecs[0][3];
		local[1] = DotProduct(impact, tex->vecs[1]) + tex->vecs[1][3];

		local[0] -= surf->texturemins[0];
		local[1] -= surf->texturemins[1];

		for (t = 0; t < tmax; t++)
		{
			td = local[1] - t * 16;
			if (td < 0)
				td = -td;
			for (s = 0; s < smax; s++)
			{
				sd = local[0] - s * 16;
				if (sd < 0)
					sd = -sd;
				if (sd > td)
					dist = sd + (td >> 1);
				else
					dist = td + (sd >> 1);
				if (dist < minlight)
				{
					unsigned delta;
					delta = (rad - dist) * 256;

					blocklights[t * smax + s].r += (delta * cl_dlights[lnum].color.r) >> 8;
					blocklights[t * smax + s].g += (delta * cl_dlights[lnum].color.g) >> 8;
					blocklights[t * smax + s].b += (delta * cl_dlights[lnum].color.b) >> 8;
				}
			}
		}
	}
}

/*
===============
R_BuildLightMap

Combine and scale multiple lightmaps into the 8.8 format in blocklights
===============
*/
void R_BuildLightMap( void )
{
	int			smax, tmax;
	int			i, size;
	color24* lightmap;
	unsigned	scale;
	int			maps;
	msurface_t* surf;

	surf = r_drawsurf.surf;

	smax = (surf->extents[0] >> 4) + 1;
	tmax = (surf->extents[1] >> 4) + 1;
	size = smax * tmax;
	lightmap = surf->samples;

	if (r_fullbright.value == 1.0 || !cl.worldmodel->lightdata)
	{
		for (i = 0; i < size; i++)
		{
			blocklights[i].r = 0xFFFF;
			blocklights[i].g = 0xFFFF;
			blocklights[i].b = 0xFFFF;
		}
		return;
	}

// clear to ambient
	for (i = 0; i < size; i++)
	{
		blocklights[i].r = r_refdef.ambientlight.r << 8;
		blocklights[i].g = r_refdef.ambientlight.g << 8;
		blocklights[i].b = r_refdef.ambientlight.b << 8;
	}

// add all the lightmaps
	if (lightmap)
	{
		if (r_lightmap.value == -1.0 && r_lightstyle.value == -1.0)
		{
			for (maps = 0; maps < MAXLIGHTMAPS && surf->styles[maps] != 255;
				maps++)
			{
				scale = r_drawsurf.lightadj[maps];	// 8.8 fraction	
				for (i = 0; i < size; i++)
				{
					blocklights[i].r += lightmap[i].r * scale;
					blocklights[i].g += lightmap[i].g * scale;
					blocklights[i].b += lightmap[i].b * scale;
				}
				lightmap += size;	// skip to next lightmap
			}
		}
		else
		{
			for (maps = 0; maps < MAXLIGHTMAPS && surf->styles[maps] != 255;
				maps++)
			{
				if (maps != r_lightmap.value)
				{
					if (surf->styles[maps] != r_lightstyle.value)
						continue;
				}

				for (i = 0; i < size; i++)
				{
					blocklights[i].r += lightmap[i].r * 264;
					blocklights[i].g += lightmap[i].g * 264;
					blocklights[i].b += lightmap[i].b * 264;
				}
				lightmap += size;	// skip to next lightmap
			}
		}
	}
	
// add all the dynamic lights
	if (surf->dlightframe == r_framecount)
		R_AddDynamicLights();

// bound, invert, and shift
	for (i = 0; i < size; i++)
	{
		if (blocklights[i].r > 0xFF00)
			blocklights[i].r = 0xFF00;
		else
			blocklights[i].r = (lightgammatable[blocklights[i].r >> VID_CBITS] & (0xFF00 >> VID_CBITS)) << VID_CBITS;

		if (blocklights[i].g > 0xFF00)
			blocklights[i].g = 0xFF00;
		else
			blocklights[i].g = (lightgammatable[blocklights[i].g >> VID_CBITS] & (0xFF00 >> VID_CBITS)) << VID_CBITS;

		if (blocklights[i].b > 0xFF00)
			blocklights[i].b = 0xFF00;
		else
			blocklights[i].b = (lightgammatable[blocklights[i].b >> VID_CBITS] & (0xFF00 >> VID_CBITS)) << VID_CBITS;
	}
}


/*
===============
R_TextureAnimation

Returns the proper texture for a given time and base texture
===============
*/
texture_t* R_TextureAnimation( texture_t* base )
{
	int     relative;
	int     count;

	if (currententity->frame)
	{
		if (base->alternate_anims)
			base = base->alternate_anims;
	}

	if (!base->anim_total)
		return base;

	if (base->name[0] != '-')
	{
		relative = (int)(cl.time * 10.0) % base->anim_total;

		count = 0;
		while (base->anim_min > relative || base->anim_max <= relative)
		{
			base = base->anim_next;
			if (!base)
				Sys_Error("R_TextureAnimation: broken cycle");
			if (++count > 100)
				Sys_Error("R_TextureAnimation: infinite cycle");
		}
	}

	return base;
}

/*
===============
R_DrawSurface
===============
*/
void R_DrawSurface( void )
{
	int				smax, tmax;
	int				u, v;
	decal_t* decals;
	int				soffset, basetoffset;
	int				horzblockstep;
	unsigned char* pcolumndest;
	void			(*pblockdrawer)(void);
	unsigned short*	(*pdecaldrawer)(byte * pBuffer, unsigned short* pDest);

// calculate the lightings
	R_BuildLightMap();

	surfrowbytes = r_drawsurf.rowbytes;

	blocksize = 16 >> r_drawsurf.surfmip;
	blockdivshift = 4 - r_drawsurf.surfmip;
	blockdivmask = (1 << blockdivshift) - 1;

	r_lightwidth = (r_drawsurf.surf->extents[0] >> 4) + 1;

	r_numhblocks = r_drawsurf.surfwidth >> blockdivshift;
	r_numvblocks = r_drawsurf.surfheight >> blockdivshift;

	horzblockstep = blocksize << 1;

	decals = r_drawsurf.surf->pdecals;

	if (currententity->rendermode == kRenderTransAlpha)
	{
		pblockdrawer = R_DrawSurfaceBlock16Holes;
		pdecaldrawer = NULL;
		decals = NULL;
	}
	else if (currententity->rendermode != kRenderNormal || (r_drawsurf.surf->flags & SURF_DRAWTILED))
	{
		pblockdrawer = R_DrawSurfaceBlock16Fullbright1;
		pdecaldrawer = R_DecalFullbrightSurface;
	}
	else
	{
		switch ((int)r_fullbright.value)
		{
		case 1:
			pblockdrawer = R_DrawSurfaceBlock16Fullbright1;
			pdecaldrawer = R_DecalFullbrightSurface;
			break;
		case 2:
			pblockdrawer = R_DrawSurfaceBlock16Fullbright2;
			pdecaldrawer = R_DecalFullbrightSurface;
			break;
		case 3:
			pblockdrawer = R_DrawSurfaceBlock16Fullbright3;
			pdecaldrawer = NULL;
			decals = NULL;
			break;
		case 4:
			//pblockdrawer = R_DrawSurfaceBlock16Fullbright4;
			//pdecaldrawer = NULL;
			//decals = NULL;
			//break;
		case 5:
			pblockdrawer = R_DrawSurfaceBlock16Fullbright5;
			pdecaldrawer = NULL;
			decals = NULL;
			break;
		default:
#if defined( _WIN32 ) && (_MSC_VER != 1020)
			if (r_mmx.value)
			{
				pblockdrawer = R_DrawSurfaceBlock16MMX;
			}
			else
#endif
			{
				pblockdrawer = R_DrawSurfaceBlock16;
			}
			pdecaldrawer = R_DecalLightSurface;
			break;
		}
	}

	smax = r_drawsurf.texture->width >> r_drawsurf.surfmip;
	tmax = r_drawsurf.texture->height >> r_drawsurf.surfmip;
	sourcetstep = smax;
	sourcevstep = smax * blocksize;
	r_stepback = smax * tmax;

	soffset = r_drawsurf.surf->texturemins[0];
	basetoffset = r_drawsurf.surf->texturemins[1];

// << 16 components are to guarantee positive values for %
	soffset = ((soffset >> r_drawsurf.surfmip) + (smax << 16)) % smax;
	basetoffset = (smax * (((tmax << 16) + (basetoffset >> r_drawsurf.surfmip)) % tmax));

	R_SetupTextureIndex(0);

	pcolumndest = r_drawsurf.surfdat;
	
	R_DrawInitLut();

	if (decals)
	{
		unsigned char* psource;
		unsigned short* prowdest;
		word* ppalette;
		vrect_t decalrect;
		int vmax;
		static byte decalbuffer[(16 * 16) * 4];

		// get the decal rect and adjust it
		R_DecalRect(decals, &decalrect, r_drawsurf.surfmip);

		decalrect.x >>= blockdivshift;
		decalrect.y >>= blockdivshift;
		decalrect.width = (blocksize + decalrect.width - 1) >> blockdivshift;
		decalrect.height = (blocksize + decalrect.height - 1) >> blockdivshift;

		vmax = r_numvblocks;

		for (u = 0; u < r_numhblocks; u++)
		{
			r_lightptr = &blocklights[u];

			r_offset = basetoffset + soffset;
			r_deltav = 0;

			prowdestbase = pcolumndest;
			prowdest = (unsigned short*)pcolumndest;

			if (u < decalrect.x || u > decalrect.width)
			{
				(*pblockdrawer)();
			}
			else
			{
				r_numvblocks = decalrect.y;
				if (decalrect.y)
				{
					(*pblockdrawer)();
					prowdest = (unsigned short*)&pcolumndest[blocksize * r_numvblocks * surfrowbytes];
				}

				for (v = decalrect.y; v < decalrect.height; v++)
				{
					psource = (byte*)r_basetexture[v] + r_basetexture[v]->offsets[r_drawsurf.surfmip] + r_offset;
					ppalette = (word*)((byte*)r_basetexture[v] + r_basetexture[v]->paloffset);

					// build decal palette (RGBA 16x16)
					R_DecalCacheSurface(psource, ppalette, decalbuffer);
					R_DrawDecals(decals, decalbuffer, u << blockdivshift, v << blockdivshift);
					prowdest = (*pdecaldrawer)(decalbuffer, prowdest);
				}

				r_numvblocks = vmax - decalrect.height;
				if (r_numvblocks > 0)
				{
					r_deltav = v;
					prowdestbase = prowdest;
					(*pblockdrawer)();
				}
			}

			r_numvblocks = vmax;
			soffset = soffset + blocksize;
			if (soffset >= smax)
			{
				soffset = 0;
				R_SetupTextureIndex(u + 1);
			}

			pcolumndest += horzblockstep;
		}
	}
	else
	{
		r_deltav = 0;

		for (u = 0; u < r_numhblocks; u++)
		{
			r_lightptr = &blocklights[u];

			prowdestbase = pcolumndest;

			r_offset = basetoffset + soffset;

			(*pblockdrawer)();

			soffset = soffset + blocksize;
			if (soffset >= smax)
			{
				soffset = 0;
				R_SetupTextureIndex(u + 1);
			}

			pcolumndest += horzblockstep;
		}
	}
}

// Pre-computed random table to add some noise to animated textures
static const int rtable[20][20] = {
	{ 1630, 2723, 2341, 227, 534, 916, 2865, 356, 1445, 2401, 780, 2919, 3136, 2817, 770, 496, 338, 2106, 2607, 2420 },
	{ 951, 2377, 3087, 2028, 595, 444, 3128, 1635, 2979, 3341, 1707, 1580, 2947, 299, 88, 433, 2364, 73, 774, 1361 },
	{ 418, 1919, 430, 3347, 2211, 1829, 1942, 118, 2595, 2530, 1669, 2043, 3326, 637, 2126, 1487, 2005, 1086, 13, 1734 },
	{ 2407, 1413, 3095, 2829, 2314, 1470, 536, 207, 604, 2233, 1398, 679, 1950, 1951, 603, 2686, 297, 2195, 9, 728 },
	{ 318, 2777, 2214, 2611, 3282, 1256, 1422, 3031, 3225, 404, 955, 641, 751, 2885, 1468, 2589, 2375, 522, 587, 2365 },
	{ 3257, 1240, 1531, 2298, 1876, 2893, 2132, 841, 260, 254, 3132, 2026, 929, 2756, 2739, 68, 3206, 2833, 1647, 2421 },
	{ 1494, 1831, 77, 2103, 522, 14, 3145, 39, 2828, 736, 473, 1874, 1225, 234, 775, 1842, 1396, 669, 2693, 2566 },
	{ 2225, 1424, 2026, 2315, 1669, 732, 1419, 2645, 2670, 1707, 3175, 1457, 154, 890, 237, 2528, 1942, 3124, 815, 3268 },
	{ 1730, 1330, 817, 1521, 590, 1553, 1987, 2254, 1385, 3176, 1134, 2284, 227, 2775, 1372, 367, 1569, 437, 2100, 3233 },
	{ 2373, 1126, 738, 2245, 316, 963, 2273, 860, 1459, 1242, 2176, 1097, 1080, 1208, 2491, 2052, 2610, 1964, 151, 1856 },
	{ 704, 2625, 275, 1074, 407, 2265, 2551, 330, 312, 811, 375, 246, 83, 1665, 1314, 808, 1811, 766, 1053, 1360 },
	{ 456, 2259, 1010, 3007, 3341, 2599, 3153, 2824, 1931, 2255, 468, 2647, 1674, 1027, 2662, 2393, 1558, 497, 2539, 2057 },
	{ 2029, 2030, 139, 233, 1600, 224, 1665, 2150, 2233, 702, 2921, 2574, 327, 2393, 156, 1266, 2614, 2393, 722, 2325 },
	{ 651, 3022, 1815, 1783, 1796, 3111, 842, 2068, 2717, 2888, 2587, 1272, 3041, 1050, 1565, 1783, 105, 2659, 773, 2396 },
	{ 2291, 2313, 814, 2385, 1011, 1730, 2685, 456, 2234, 117, 2423, 1648, 2175, 3245, 423, 933, 1210, 1221, 2182, 2446 },
	{ 2020, 1646, 1698, 437, 3240, 1092, 2303, 1054, 1377, 59, 285, 977, 874, 2432, 1089, 515, 2388, 2667, 2465, 467 },
	{ 856, 579, 776, 2459, 3097, 1458, 301, 1127, 1268, 3016, 1262, 2904, 2735, 978, 2111, 2782, 2594, 1633, 2724, 1305 },
	{ 2559, 3056, 1993, 1852, 708, 721, 1280, 2830, 1701, 2537, 678, 1481, 880, 3310, 2316, 2549, 732, 1452, 3106, 2287 },
	{ 3106, 729, 1324, 2862, 2996, 2823, 886, 867, 3074, 2968, 1399, 2778, 1744, 1767, 103, 705, 22, 341, 2328, 260 },
	{ 3336, 467, 2052, 1564, 2234, 2026, 1583, 1928, 9, 3084, 2093, 256, 1262, 1483, 974, 3311, 3295, 1658, 2164, 249 }
};

/*
===============
R_SetupTextureIndex

Sets up r_basetexture array for the current texture
===============
*/
void R_SetupTextureIndex( int u )
{
    int     v, i;
    texture_t* tm, * tm2;
    int		relative;
    int		count;
    int     tu, tv;

    tm = r_drawsurf.texture;

    if (tm->name[0] == '-' && tm->anim_total)
    {
        for (v = 0; v < r_numvblocks; v++)
        {
            tu = (int)((r_drawsurf.surf->texturemins[0] + ((tm->width << 16) + u * 16)) / tm->width) % 20;
            tv = (int)((r_drawsurf.surf->texturemins[1] + ((tm->height << 16) + v * 16)) / tm->height) % 20;

            relative = rtable[tu][tv] % tm->anim_total;

            count = 0;
			tm2 = tm;
			while (tm2->anim_min > relative || tm2->anim_max <= relative)
			{
				tm2 = tm2->anim_next;
				if (!tm2)
					Sys_Error("R_TextureAnimation: broken cycle");
				if (++count > 100)
					Sys_Error("R_TextureAnimation: infinite cycle");
			}

            r_basetexture[v] = tm2;
        }
        return;
    }

    if (r_basetexture[0] != r_drawsurf.texture)
	{
		for (i = 0; i < 18; i++)
		{
            r_basetexture[i] = r_drawsurf.texture;
		}
	}
}

// Clear the cached texture of the specified surface
// Used by decal code
void R_InvalidateSurface( msurface_t* surface )
{
	surfcache_t* cache;
	int i;

	for (i = 0; i < 4; i++)
	{
		cache = surface->cachespots[i];
		if (cache)
			cache->texture = NULL;
	}
}

unsigned short fpu_cw, my_cw = 0x037F;

/*
================
R_DrawSurfaceBlock16

Draw a surface block
================
*/
void R_DrawSurfaceBlock16( void )
{
	unsigned char* psource;
	unsigned short* ppalette;
	unsigned short* prowdest;
	unsigned short* prowloop;
	static double block_multiplier[5] = { 1.0, 0.5, 0.25, 0.125, 0.0625 };
	double clight, clightdelta, corners[2], cornerdeltas[2];
	int i, j, v;

	clightdelta = 0i64;
	clight = 0i64;
	corners[0] = 0i64;
	corners[1] = 0i64;
	cornerdeltas[0] = 0i64;
	cornerdeltas[1] = 0i64;

	prowdest = (unsigned short*)prowdestbase;

	// change the FPU state to round-to-nearest
	__asm fstcw fpu_cw
	__asm fldcw my_cw

	prowdest = (unsigned short*)prowdestbase;

	for (v = 0; v < r_numvblocks; v++)
	{
		corners[0] = (double)(((int64)(r_lightptr[0].r & 0xFFFF) << 32) | ((int64)(r_lightptr[0].g & 0xFFFF) << 16) | ((int64)(r_lightptr[0].b & 0xFFFF)));
		corners[1] = (double)(((int64)(r_lightptr[1].r & 0xFFFF) << 32) | ((int64)(r_lightptr[1].g & 0xFFFF) << 16) | ((int64)(r_lightptr[1].b & 0xFFFF)));

		r_lightptr = (colorVec*)((byte*)r_lightptr + (r_lightwidth << 4));

		cornerdeltas[0] = ((double)(((int64)(r_lightptr[0].r & 0xFFFF) << 32) | ((int64)(r_lightptr[0].g & 0xFFFF) << 16) | ((int64)(r_lightptr[0].b & 0xFFFF)))
			- corners[0]) * block_multiplier[blockdivshift];
		cornerdeltas[1] = ((double)(((int64)(r_lightptr[1].r & 0xFFFF) << 32) | ((int64)(r_lightptr[1].g & 0xFFFF) << 16) | ((int64)(r_lightptr[1].b & 0xFFFF)))
			- corners[1]) * block_multiplier[blockdivshift];

		ppalette = (unsigned short*)((byte*)r_basetexture[r_deltav + v] + r_basetexture[r_deltav + v]->paloffset);
		psource = (unsigned char*)((byte*)r_basetexture[r_deltav + v] + r_basetexture[r_deltav + v]->offsets[r_drawsurf.surfmip] + r_offset);

		for (i = 0; i < blocksize; i++)
		{
			clight = corners[0];
			prowloop = prowdest;

			clightdelta = (corners[1] - corners[0]) * block_multiplier[blockdivshift];

			for (j = 0; j < blocksize; j++)
			{
				unsigned short* pColor = &ppalette[psource[j] * 4];
				uint32 r = red_64klut[(((int64)clight >> 32) & 0xFF00) + pColor[2]];
				uint32 g = green_64klut[(((int64)clight >> 16) & 0xFF00) + pColor[1]];
				uint32 b = blue_64klut[(((int64)clight) & 0xFF00) + pColor[0]];

				prowloop[j] = r | g | b;

				clight += clightdelta;
			}

			corners[0] += cornerdeltas[0];
			corners[1] += cornerdeltas[1];

			prowdest += (surfrowbytes / 2);
			psource += sourcetstep;
		}

		r_offset += sourcevstep;
		if (r_offset >= r_stepback)
			r_offset -= r_stepback;
	}

	// restore the FPU state
	__asm fldcw fpu_cw
}

// ScriptedSnark: FIXME: MSVC++ 4.2 can't handle this yet.
#if defined( _WIN32 ) && (_MSC_VER != 1020)
#include <xmmintrin.h>

/*
================
R_DrawSurfaceBlock16MMX

Draw a surface block using the MMX instruction set
================
*/
void R_DrawSurfaceBlock16MMX( void )
{
    unsigned char* psource;
    unsigned short* prowdest;
    int				lightstep;
    int             v;
    byte* texture;

	static __m64 MMX_BLOCKMULTIPLIER = { 0x555555555555 };	// 0.33333 in 16.16 fixed point
	static __m64 MMX_REDBLUE1516 = { 0x00F800F800F800F8 };	// red and blue masks for both RGB555 and RGB565
	static __m64 MMX_GREEN15 = { 0x0000F8000000F800 };		// green mask for RGB555
	static __m64 MMX_GREEN16 = { 0x0000FC000000FC00 };		// green mask for RGB565
	static __m64 MMX_LIGHTMULTIPLIER15 = { 0x2000000820000008 };	// light multiplier for RGB555
	static __m64 MMX_LIGHTMULTIPLIER16 = { 0x2000000420000004 };	// light multiplier for RGB565
	static __m64 MMX_LIGHTLEFT = { 0 };						// left light values
	static __m64 MMX_LIGHTRIGHT = { 0 };					// right light values
    
	if (!gHasMMXTechnology)
		return;
	
	__asm
	{
        xor         eax, eax
		mov         ecx, prowdestbase
		mov         edx, r_lightwidth
		mov         prowdest, ecx                           ; 0xC = (word*)prowdestbase;
		mov         ecx, is15bit                            ; determine the color depth (RGB555 or RGB565)
		shl         edx, 4
		cmp         ecx, eax
		mov         lightstep, edx                          ; 0x14 = r_lightwidth << 2;
		jz          IS16BIT

		; 15-bit routine, calculate the lighting
		mov         esi, r_lightptr                         ; store the light pointer

		; store lightptr (left)
		movd        mm0, DWORD PTR[esi + 0]                 ; red (lightptr[0])
		movd        mm2, DWORD PTR[esi + 4]                 ; green (lightptr[1])
		movd        mm4, DWORD PTR[esi + 8]                 ; blue (lightptr[2])

		; store lightptr (right)
		movd        mm1, DWORD PTR[esi + 16]                ; red (lightptr[4])
		punpcklwd   mm4, mm2
		movd        mm3, DWORD PTR[esi + 20]                ; green (lightptr[5])
		punpckldq   mm4, mm0
		movd        mm5, DWORD PTR[esi + 24]                ; blue (lightptr[6])

		psrlw       mm4, 1
		pmulhw      mm4, MMX_BLOCKMULTIPLIER                ; apply the block multiplier
		punpcklwd   mm5, mm3
		punpckldq   mm5, mm1
		psrlw       mm5, 1
		pmulhw      mm5, MMX_BLOCKMULTIPLIER                ; apply the block multiplier

		mov         ecx, r_numvblocks
		mov         v, eax
		cmp         ecx, eax
		jle         END
		mov         ecx, r_offset

BITSCAN15:                                                  ; 15-bit scan of all vblocks
		mov         edx, r_deltav
		mov         eax, v
		add         edx, eax
		mov         eax, DWORD PTR r_basetexture[edx * 4]
		mov         edx, r_drawsurf.surfmip
		mov         edx, [eax + edx * 4 + 2Ch]
		add         edx, eax
		add         edx, ecx
		mov         ecx, [eax + 3Ch]
		add         ecx, eax
		mov         psource, edx
		mov         texture, ecx
		mov         esi, r_lightptr                         ; store the light pointer
		add         esi, lightstep

		movd        mm0, DWORD PTR[esi + 0]                 ; red left (lightptr[0])
		mov         r_lightptr, esi
		movd        mm2, DWORD PTR[esi + 4]                 ; green left (lightptr[1])
		mov         edx, blocksize
		movd        mm6, DWORD PTR[esi + 8]                 ; blue left (lightptr[2])

		movd        mm1, DWORD PTR[esi + 16]                ; red left (lightptr[4])
		punpcklwd   mm6, mm2
		movd        mm3, DWORD PTR[esi + 20]                ; green left (lightptr[5])
		punpckldq   mm6, mm0
		movd        mm7, DWORD PTR[esi + 24]                ; blue left (lightptr[6])

		psrlw       mm6, 1
		pmulhw      mm6, MMX_BLOCKMULTIPLIER                ; apply the block multiplier
		punpcklwd   mm7, mm3
		punpckldq   mm7, mm1
		mov         ebx, texture
		movd        mm1, blockdivshift
		psrlw       mm7, 1
		pmulhw      mm7, MMX_BLOCKMULTIPLIER                ; apply the block multiplier

		movq        MMX_LIGHTLEFT, mm6                      ; copy
		psubw       mm6, mm4
		movq        MMX_LIGHTRIGHT, mm7                           ; copy
		psubw       mm7, mm5
		psraw       mm6, mm1
		psraw       mm7, mm1
		mov         ecx, 0

BLOCKSIZEINIT15:                                            ; initialize data for 15-bit blocksize scan loop
		movd        mm1, blockdivshift
		movq        mm0, mm5                                ; copy
		psubw       mm0, mm4
		mov         esi, psource
		psraw       mm0, mm1
		mov         edi, prowdest
		movq        mm2, mm4                                ; copy
		push        ecx
		mov         ecx, 0

BLOCKSIZESCAN15:                                            ; start scanning
		movzx       eax, BYTE PTR[esi + ecx]
		shl         eax, 3
		add         eax, ebx
		movq        mm1, QWORD PTR[eax]                     ; copy
		psllw       mm1, 3
		pmulhw      mm1, mm2
		paddw       mm2, mm0
		movzx       eax, BYTE PTR[esi + ecx + 1]
		shl         eax, 3
		add         eax, ebx
		movq        mm3, QWORD PTR[eax]                     ; copy
		psllw       mm3, 3
		pmulhw      mm3, mm2
		paddw       mm2, mm0
		packuswb    mm1, mm3
		movq        mm3, mm1                                ; copy
		pand        mm1, MMX_REDBLUE1516
		pand        mm3, MMX_GREEN15
		pmaddwd     mm1, MMX_LIGHTMULTIPLIER15
		por         mm1, mm3
		pslld       mm1, 0Bh
		psrad       mm1, 10h
		packssdw    mm1, mm1
		movd        DWORD PTR[edi + ecx * 2], mm1
		add         ecx, 2
		cmp         ecx, edx
		jl          short BLOCKSIZESCAN15                   ; new blocksize iteration
		pop         ecx
		paddw       mm4, mm6
		paddw       mm5, mm7
		mov         eax, sourcetstep
		add         psource, eax
		mov         eax, surfrowbytes
		add         prowdest, eax
		inc         ecx
		cmp         ecx, edx
		jl          BLOCKSIZEINIT15
		movq        mm4, MMX_LIGHTLEFT                      ; copy
		movq        mm5, MMX_LIGHTRIGHT                     ; copy
		mov         ecx, r_offset
		mov         esi, sourcevstep
		mov         eax, r_stepback
		add         ecx, esi
		cmp         ecx, eax
		mov         r_offset, ecx
		jl          short BLOCKSIZESCAN15_NEXT
		sub         ecx, eax
		mov         r_offset, ecx

BLOCKSIZESCAN15_NEXT:                                       ; increment v
		mov         eax, v
		mov         edx, r_numvblocks
		inc         eax
		cmp         eax, edx
		mov         v, eax
		jl          BITSCAN15                               ; new iteration
		jmp         END                                     ; done
        
IS16BIT:
        ; 16-bit routine, calculate the lighting
		mov         esi, r_lightptr; store the light pointer

		; store lightptr (left)
		movd        mm0, DWORD PTR[esi + 0]                 ; red (lightptr[0])
		movd        mm2, DWORD PTR[esi + 4]                 ; green (lightptr[1])
		movd        mm4, DWORD PTR[esi + 8]                 ; blue (lightptr[2])

		; store lightptr (right)
		movd        mm1, DWORD PTR[esi + 16]                ; red (lightptr[4])
		punpcklwd   mm4, mm2
		movd        mm3, DWORD PTR[esi + 20]                ; green (lightptr[5])
		punpckldq   mm4, mm0
		movd        mm5, DWORD PTR[esi + 24]                ; blue (lightptr[6])

		psrlw       mm4, 1
		pmulhw      mm4, MMX_BLOCKMULTIPLIER                ; apply the block multiplier
		punpcklwd   mm5, mm3
		punpckldq   mm5, mm1
		psrlw       mm5, 1
		pmulhw      mm5, MMX_BLOCKMULTIPLIER                ; apply the block multiplier
		mov         ecx, r_numvblocks
		mov         v, eax
		cmp         ecx, eax
		jle         END

		mov         ecx, r_offset

BITSCAN16:                                                  ; 16-bit scan of all vblocks
		mov         edx, r_deltav
		mov         eax, v
		add         edx, eax
		mov         eax, DWORD PTR r_basetexture[edx * 4]
		mov         edx, r_drawsurf.surfmip
		mov         edx, [eax + edx * 4 + 2Ch]
		add         edx, eax
		add         edx, ecx
		mov         ecx, [eax + 3Ch]
		add         ecx, eax
		mov         psource, edx
		mov         texture, ecx
		mov         esi, r_lightptr                         ; store the light pointer
		add         esi, lightstep

		movd        mm0, DWORD PTR[esi + 0]                 ; red left (lightptr[0])
		mov         r_lightptr, esi
		movd        mm2, DWORD PTR[esi + 4]                 ; green left (lightptr[1])
		mov         edx, blocksize
		movd        mm6, DWORD PTR[esi + 8]                 ; blue left (lightptr[2])

		movd        mm1, DWORD PTR[esi + 16]                ; red left (lightptr[4])
		punpcklwd   mm6, mm2
		movd        mm3, DWORD PTR[esi + 20]                ; green left (lightptr[5])
		punpckldq   mm6, mm0
		movd        mm7, DWORD PTR[esi + 24]                ; blue left (lightptr[6])

		psrlw       mm6, 1
		pmulhw      mm6, MMX_BLOCKMULTIPLIER                ; apply the block multiplier
		punpcklwd   mm7, mm3
		punpckldq   mm7, mm1
		mov         ebx, texture
		movd        mm1, blockdivshift
		psrlw       mm7, 1
		pmulhw      mm7, MMX_BLOCKMULTIPLIER                ; apply the block multiplier

		movq        MMX_LIGHTLEFT, mm6                      ; copy
		psubw       mm6, mm4
		movq        MMX_LIGHTRIGHT, mm7                     ; copy
		psubw       mm7, mm5
		psraw       mm6, mm1
		psraw       mm7, mm1
		mov         ecx, 0

BLOCKSIZEINIT16:                                            ; initialize data for 16-bit blocksize scan loop
		movd        mm1, blockdivshift
		movq        mm0, mm5                                ; copy
		psubw       mm0, mm4
		mov         esi, psource
		psraw       mm0, mm1
		mov         edi, prowdest
		movq        mm2, mm4                                ; copy
		push        ecx
		mov         ecx, 0

BLOCKSIZESCAN16:                                            ; start scanning
		movzx       eax, BYTE PTR[esi + ecx]
		shl         eax, 3
		add         eax, ebx
		movq        mm1, QWORD PTR[eax]                     ; copy
		psllw       mm1, 3
		pmulhw      mm1, mm2
		paddw       mm2, mm0
		movzx       eax, BYTE PTR[esi + ecx + 1]
		shl         eax, 3
		add         eax, ebx
		movq        mm3, QWORD PTR[eax]                     ; copy
		psllw       mm3, 3
		pmulhw      mm3, mm2
		paddw       mm2, mm0
		packuswb    mm1, mm3
		movq        mm3, mm1                                ; copy
		pand        mm1, MMX_REDBLUE1516
		pand        mm3, MMX_GREEN16
		pmaddwd     mm1, MMX_LIGHTMULTIPLIER16
		por         mm1, mm3
        pslld       mm1, 11
        psrad       mm1, 16
		packssdw    mm1, mm1
		movd        DWORD PTR[edi + ecx * 2], mm1
		add         ecx, 2
		cmp         ecx, edx
		jl          short BLOCKSIZESCAN16                   ; new blocksize iteration
		pop         ecx
		paddw       mm4, mm6
		paddw       mm5, mm7
		mov         eax, sourcetstep
		add         psource, eax
		mov         eax, surfrowbytes
		add         prowdest, eax
		inc         ecx
		cmp         ecx, edx
		jl          BLOCKSIZEINIT16
		movq        mm4, MMX_LIGHTLEFT                      ; copy
		movq        mm5, MMX_LIGHTRIGHT                     ; copy
		mov         ecx, r_offset
		mov         esi, sourcevstep
		mov         eax, r_stepback
		add         ecx, esi
		cmp         ecx, eax
		mov         r_offset, ecx
		jl          short BLOCKSIZESCAN16_NEXT
		sub         ecx, eax
		mov         r_offset, ecx

BLOCKSIZESCAN16_NEXT:                                       ; increment v
		mov         eax, v
		mov         edx, r_numvblocks
		inc         eax
		cmp         eax, edx
		mov         v, eax
		jl          BITSCAN16                               ; new iteration

END:
		emms
	}
}
#endif

void R_DrawSurfaceBlock16Holes( void )
{
    colorVec        clightleft, clightright, clightleftstep, clightrightstep;
    colorVec        clight, clightstep;
    int				v, i, j;
    unsigned char   pix, * psource;
    unsigned short* prowdest, * ppalette, * pColor;
    uint32          r, g, b;
    texture_t** pTexture;

    prowdest = (unsigned short*)prowdestbase;

    for (v = 0; v < r_numvblocks; v++)
    {
        pTexture = &r_basetexture[r_deltav + v];

        ColorCopy(r_lightptr[0], clightleft);
        ColorCopy(r_lightptr[1], clightright);

        r_lightptr = (colorVec*)((byte*)r_lightptr + (r_lightwidth << 4));

        ColorSubtract(r_lightptr[0], clightleft, clightleftstep);
        ColorShift(clightleftstep, blockdivshift);
        ColorSubtract(r_lightptr[1], clightright, clightrightstep);
        ColorShift(clightrightstep, blockdivshift);

        ppalette = (unsigned short*)((byte*)(*pTexture) + (*pTexture)->paloffset);
        psource = (unsigned char*)((byte*)(*pTexture) + (*pTexture)->offsets[r_drawsurf.surfmip] + r_offset);

        for (i = 0; i < blocksize; i++)
        {
            ColorCopy(clightleft, clight);

            for (j = 0; j < blocksize; j++)
            {
                pix = psource[j];
                if (pix == TRANSPARENT_COLOR)
                {
                    prowdest[j] = 0x001F;
                }
                else
                {
                    pColor = &ppalette[pix * 4];
                    r = r_lut[(clight.r & 0xFF00) + pColor[2]];
                    g = r_lut[(clight.g & 0xFF00) + pColor[1]];
                    b = r_lut[(clight.b & 0xFF00) + pColor[0]];

                    if (is15bit)
                    {
                        prowdest[j] = PACKEDRGB555(r, g, b);
                    }
                    else
                    {
                        prowdest[j] = PACKEDRGB565(r, g, b);
                    }
                }

                ColorSubtract(clightright, clightleft, clightstep);
                ColorShift(clightstep, blockdivshift);
                ColorAdd(clight, clightstep, clight);
            }

            ColorAdd(clightleft, clightleftstep, clightleft);
            ColorAdd(clightright, clightrightstep, clightright);

			prowdest += (surfrowbytes / 2);
			psource += sourcetstep;
        }

        r_offset += sourcevstep;
        if (r_offset >= r_stepback)
            r_offset -= r_stepback;
    }
}


void R_DrawSurfaceBlock16Fullbright1( void )
{
	int             v, i, j;
	unsigned char   pix, * psource;
	unsigned short* prowdest, * ppalette, * pColor;
	uint32          r, g, b;
	texture_t** pTexture;

	prowdest = (unsigned short*)prowdestbase;

	for (v = 0; v < r_numvblocks; v++)
	{
		pTexture = &r_basetexture[r_deltav + v];

		ppalette = (unsigned short*)((byte*)(*pTexture) + (*pTexture)->paloffset);
		psource = (unsigned char*)((byte*)(*pTexture) + (*pTexture)->offsets[r_drawsurf.surfmip] + r_offset);

		for (i = 0; i < blocksize; i++)
		{
			for (j = 0; j < blocksize; j++)
			{
				pix = psource[j];
				pColor = &ppalette[pix * 4];
				r = pColor[2];
				g = pColor[1];
				b = pColor[0];

				if (is15bit)
				{
					prowdest[j] = PACKEDRGB555(r, g, b);
				}
				else
				{
					prowdest[j] = PACKEDRGB565(r, g, b);
				}
			}

			prowdest += (surfrowbytes / 2);
			psource += sourcetstep;
		}

		r_offset += sourcevstep;
		if (r_offset >= r_stepback)
			r_offset -= r_stepback;
	}
}


void R_DrawSurfaceBlock16Fullbright2( void )
{
    colorVec        clightleft, clightright, clightleftstep, clightrightstep;
    colorVec        clight, clightstep;
    int             v, i, j;
    unsigned short* prowdest;
    uint32          r, g, b;

    prowdest = (unsigned short*)prowdestbase;

    for (v = 0; v < r_numvblocks; v++)
    {
        ColorCopy(r_lightptr[0], clightleft);
        ColorCopy(r_lightptr[1], clightright);
        
        r_lightptr = (colorVec*)((byte*)r_lightptr + (r_lightwidth << 4));
        
        ColorSubtract(r_lightptr[0], clightleft, clightleftstep);
		ColorShift(clightleftstep, blockdivshift);
        ColorSubtract(r_lightptr[1], clightright, clightrightstep);
		ColorShift(clightrightstep, blockdivshift);

        for (i = 0; i < blocksize; i++)
        {
            ColorCopy(clightleft, clight);
            
            for (j = 0; j < blocksize; j++)
            {
                if (is15bit)
                {
					r = ((clight.r >> 1) & 0x7C00);
					g = ((clight.g >> 6) & 0x03E0);
					b = ((clight.b >> 11) & 0x001F);
                }
                else
                {
					r = (clight.r & 0xF800);
					g = ((clight.g >> 5) & 0x07E0);
					b = ((clight.b >> 11) & 0x001F);
                }

				prowdest[j] = r | g | b;

                ColorSubtract(clightright, clightleft, clightstep);
                ColorShift(clightstep, blockdivshift);
                ColorAdd(clight, clightstep, clight);
            }
            
            ColorAdd(clightleft, clightleftstep, clightleft);
            ColorAdd(clightright, clightrightstep, clightright);
            
            prowdest += (surfrowbytes / 2);
        }
        
        r_offset += sourcevstep;
        if (r_offset >= r_stepback)
            r_offset -= r_stepback;
    }
}


void R_DrawSurfaceBlock16Fullbright3( void )
{
	colorVec        clightleft, clightright, clightleftstep, clightrightstep;
	colorVec        clight, clightstep;
	int             v, i, j;
	unsigned short* prowdest;
	uint32          r, g, b;

	prowdest = (unsigned short*)prowdestbase;

	for (v = 0; v < r_numvblocks; v++)
	{
		ColorCopy(r_lightptr[0], clightleft);
		ColorCopy(r_lightptr[1], clightright);

		r_lightptr = (colorVec*)((byte*)r_lightptr + (r_lightwidth << 4));

		ColorSubtract(r_lightptr[0], clightleft, clightleftstep);
		ColorShift(clightleftstep, blockdivshift);
		ColorSubtract(r_lightptr[1], clightright, clightrightstep);
		ColorShift(clightrightstep, blockdivshift);

		for (i = 0; i < blocksize; i++)
		{
			ColorSubtract(clightright, clightleft, clightstep);
			ColorShift(clightstep, blockdivshift);
			ColorCopy(clightleft, clight);

			for (j = 0; j < blocksize; j++)
			{
				if (is15bit)
				{
					r = ((clight.r >> 1) & 0x7C00);
					g = ((clight.g >> 6) & 0x03E0);
					b = ((clight.b >> 11) & 0x001F);

					prowdest[j] = r | g | b;

					if (i == 0 || j == 0)
					{
						prowdest[j] =
							((prowdest[j] & 0x7C00) + ((255 * (1 - (prowdest[j] & 0x7C00))) >> r_drawsurf.surfmip >> 8)) & 0x7C00 |
							((prowdest[j] & 0x03E0) + ((255 * (1 - (prowdest[j] & 0x03E0))) >> r_drawsurf.surfmip >> 8)) & 0x03E0 |
							(prowdest[j] & 0x001F);
					}
				}
				else
				{
					r = (clight.r & 0xF800);
					g = ((clight.g >> 5) & 0x07E0);
					b = ((clight.b >> 11) & 0x001F);

					prowdest[j] = r | g | b;

					if (i == 0 || j == 0)
					{
						prowdest[j] =
							((prowdest[j] & 0xF800) + ((255 * (1 - (prowdest[j] & 0xF800))) >> r_drawsurf.surfmip >> 8)) & 0xF800 |
							((prowdest[j] & 0x07E0) + ((255 * (1 - (prowdest[j] & 0x07E0))) >> r_drawsurf.surfmip >> 8)) & 0x07E0 |
							(prowdest[j] & 0x001F);
					}
				}

				ColorAdd(clight, clightstep, clight);
			}

			ColorAdd(clightleft, clightleftstep, clightleft);
			ColorAdd(clightright, clightrightstep, clightright);

			prowdest += (surfrowbytes / 2);
		}

		r_offset += sourcevstep;
		if (r_offset >= r_stepback)
			r_offset -= r_stepback;
	}
}


void R_DrawSurfaceBlock16Fullbright4( void ) 
{
	colorVec        clightleft, clightright, clightleftstep, clightrightstep;
	colorVec        clight, clightstep;
	int             v, i, j;
	unsigned short* prowdest;
	uint32          r, g, b;

	prowdest = (unsigned short*)prowdestbase;

	for (v = 0; v < r_numvblocks; v++)
	{
		ColorCopy(r_lightptr[0], clightleft);
		ColorCopy(r_lightptr[1], clightright);

		r_lightptr = (colorVec*)((byte*)r_lightptr + (r_lightwidth << 4));

		ColorSubtract(r_lightptr[0], clightleft, clightleftstep);
		ColorShift(clightleftstep, blockdivshift);
		ColorSubtract(r_lightptr[1], clightright, clightrightstep);
		ColorShift(clightrightstep, blockdivshift);

		for (i = 0; i < blocksize; i++)
		{
			ColorSubtract(clightright, clightleft, clightstep);
			ColorShift(clightstep, blockdivshift);
			ColorCopy(clightleft, clight);

			for (j = 0; j < blocksize; j++)
			{
				r = r_lut[(clight.r & 0xFF00) + 255];
				g = r_lut[(clight.g & 0xFF00) + 255];
				b = r_lut[(clight.b & 0xFF00) + 255];

				if (is15bit)
				{
					prowdest[j] = PACKEDRGB555(r, g, b);

					if (i == 0 || j == 0)
					{
						prowdest[j] =
							((prowdest[j] & 0x7C00) + ((255 * (1 - (prowdest[j] & 0x7C00))) >> r_drawsurf.surfmip >> 8)) & 0x7C00 |
							((prowdest[j] & 0x03E0) + ((255 * (1 - (prowdest[j] & 0x03E0))) >> r_drawsurf.surfmip >> 8)) & 0x03E0 |
							(prowdest[j] & 0x001F);
					}
				}
				else
				{
					prowdest[j] = PACKEDRGB565(r, g, b);

					if (i == 0 || j == 0)
					{
						prowdest[j] =
							((prowdest[j] & 0xF800) + ((255 * (1 - (prowdest[j] & 0xF800))) >> r_drawsurf.surfmip >> 8)) & 0xF800 |
							((prowdest[j] & 0x07E0) + ((255 * (1 - (prowdest[j] & 0x07E0))) >> r_drawsurf.surfmip >> 8)) & 0x07E0 |
							(prowdest[j] & 0x001F);
					}
				}

				ColorAdd(clight, clightstep, clight);
			}

			ColorAdd(clightleft, clightleftstep, clightleft);
			ColorAdd(clightright, clightrightstep, clightright);

			prowdest += (surfrowbytes / 2);
		}

		r_offset += sourcevstep;
		if (r_offset >= r_stepback)
			r_offset -= r_stepback;
	}
}


void R_DrawSurfaceBlock16Fullbright5( void )
{
	colorVec        clightleft, clightright, clightleftstep, clightrightstep;
	colorVec        clight, clightstep;
	colorVec        mipcolors[MIPLEVELS] = { { 255, 0, 0, 0 }, { 255, 255, 0, 0 }, { 0, 255, 0, 0 }, { 0, 255, 255, 0 } };
	int             v, i, j;
	unsigned short* prowdest;
	uint32          r, g, b;

	prowdest = (unsigned short*)prowdestbase;

	for (v = 0; v < r_numvblocks; v++)
	{
		ColorCopy(r_lightptr[0], clightleft);
		ColorCopy(r_lightptr[1], clightright);

		r_lightptr = (colorVec*)((byte*)r_lightptr + (r_lightwidth << 4));

		ColorSubtract(r_lightptr[0], clightleft, clightleftstep);
		ColorShift(clightleftstep, blockdivshift);
		ColorSubtract(r_lightptr[1], clightright, clightrightstep);
		ColorShift(clightrightstep, blockdivshift);

		for (i = 0; i < blocksize; i++)
		{
			ColorSubtract(clightright, clightleft, clightstep);
			ColorShift(clightstep, blockdivshift);
			ColorCopy(clightleft, clight);

			for (j = 0; j < blocksize; j++)
			{
				r = r_lut[(clight.r & 0xFF00) + mipcolors[r_drawsurf.surfmip].r];
				g = r_lut[(clight.g & 0xFF00) + mipcolors[r_drawsurf.surfmip].g];
				b = r_lut[(clight.b & 0xFF00) + mipcolors[r_drawsurf.surfmip].b];

				if (is15bit)
				{
					prowdest[j] = PACKEDRGB555(r, g, b);

					if (i == 0 || j == 0)
					{
						prowdest[j] =
							((prowdest[j] & 0xFC00) + ((255 * (1 - (prowdest[j] & 0x7C00))) >> r_drawsurf.surfmip >> 8)) & 0x7C00 |
							((prowdest[j] & 0x03E0) + ((255 * (1 - (prowdest[j] & 0x03E0))) >> r_drawsurf.surfmip >> 8)) & 0x03E0 |
							((prowdest[j] & 0x001F) + ((255 * (31 - (prowdest[j] & 0x001F))) >> r_drawsurf.surfmip >> 8)) & 0x001F;
					}
				}
				else
				{
					prowdest[j] = PACKEDRGB565(r, g, b);

					if (i == 0 || j == 0)
					{
						prowdest[j] =
							((prowdest[j] & 0xF800) + ((255 * (1 - (prowdest[j] & 0xF800))) >> r_drawsurf.surfmip >> 8)) & 0xF800 |
							((prowdest[j] & 0x07E0) + ((255 * (1 - (prowdest[j] & 0x07E0))) >> r_drawsurf.surfmip >> 8)) & 0x07E0 |
							((prowdest[j] & 0x001F) + ((255 * (31 - (prowdest[j] & 0x001F))) >> r_drawsurf.surfmip >> 8)) & 0x001F;
					}
				}

				ColorAdd(clight, clightstep, clight);
			}

			ColorAdd(clightleft, clightleftstep, clightleft);
			ColorAdd(clightright, clightrightstep, clightright);

			prowdest += (surfrowbytes / 2);
		}

		r_offset += sourcevstep;
		if (r_offset >= r_stepback)
			r_offset -= r_stepback;
	}
}

// Initializes the lookup tables
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
	memset(gDecalPool, 0, sizeof(gDecalPool));
	gDecalCount = 0;
}

void R_DecalUnlink( decal_t* pdecal )
{
	decal_t* tmp;

	if (pdecal->psurface)
	{
		tmp = pdecal->psurface->pdecals;
		if (tmp == pdecal)
		{
			pdecal->psurface->pdecals = pdecal->pnext;
		}
		else
		{
			if (!tmp)
				Sys_Error("Bad decal list");

			while (tmp->pnext)
			{
				if (tmp->pnext == pdecal)
				{
					tmp->pnext = pdecal->pnext;
					break;
				}

				tmp = tmp->pnext;
			}
		}
	}

    pdecal->psurface = NULL;
}


// Just reuse next decal in list
// A decal that spans multiple surfaces will use multiple decal_t pool entries, as each surface needs
// it's own.
decal_t* R_DecalAlloc( decal_t* pdecal )
{
	if (!pdecal)
    {
        int count;

        count = 0;		// Check for the odd possiblity of infinte loop
        do
        {
			gDecalCount++;
            if (gDecalCount >= MAX_DECALS)
                gDecalCount = 0;
			pdecal = gDecalPool + gDecalCount;	// reuse next decal
            count++;
        } while ((pdecal->flags & FDECAL_PERMANENT) && count < MAX_DECALS);
    }

    // If decal is already linked to a surface, unlink it.
	R_DecalUnlink(pdecal);

	return pdecal;
}


void R_DecalRemoveAll( int textureIndex )
{
    int	i;
    decal_t* pDecal;

    for (i = 0; i < MAX_DECALS; i++)
    {
        pDecal = &gDecalPool[i];

        if (pDecal->texture == textureIndex)
        {
            R_DecalUnlink(pDecal);
            memset(pDecal, 0, sizeof(decal_t));
        }
    }
}


// Calculate the rectangle for a decals on a single surface
// Used by R_DrawSurface
void R_DecalRect( decal_t* decals, vrect_t* decalrect, int surfmip )
{
    int	x, y, w, h;
    texture_t* texture;

    x = (r_drawsurf.surf->extents[0] >> surfmip) - 1;
    y = (r_drawsurf.surf->extents[1] >> surfmip) - 1;

    decalrect->x = x;
    decalrect->y = y;
    decalrect->width = 0;
    decalrect->height = 0;

    for (; decals; decals = decals->pnext)
    {
        texture = Draw_DecalTexture(decals->texture);

        // Compute pos on the surface
		w = decals->dx >> surfmip;
		h = decals->dy >> surfmip;

		if (decalrect->x > w)
			decalrect->x = w;
		if (decalrect->y > h)
			decalrect->y = h;

		w += (texture->width >> surfmip << decals->scale);
		h += (texture->height >> surfmip << decals->scale);

		if (decalrect->width < w)
			decalrect->width = w;
		if (decalrect->height < h)
			decalrect->height = h;
    }

    // Clamp the computed coordinates to the surface dimensions
	if (decalrect->x < 0)
		decalrect->x = 0;
	if (decalrect->y < 0)
		decalrect->y = 0;
	if (decalrect->width > x)
		decalrect->width = x;
	if (decalrect->height > y)
		decalrect->height = y;
}

// iterate over all surfaces on a node, looking for surfaces to decal
void R_DecalNode( mnode_t* node )
{
    mplane_t* splitplane;
    float		dist;

    if (!node)
        return;

	if (node->contents < 0)
		return;
    
    splitplane = node->plane;
    dist = DotProduct(gDecalPos, splitplane->normal) - splitplane->dist;

    // This is arbitrarily set to 10 right now.  In an ideal world we'd have the 
    // exact surface but we don't so, this tells me which planes are "sort of 
    // close" to the gunshot -- the gunshot is actually 4 units in front of the 
    // wall (see dlls\weapons.cpp). We also need to check to see if the decal 
    // actually intersects the texture space of the surface, as this method tags
    // parallel surfaces in the same node always.
    // JAY: This still tags faces that aren't correct at edges because we don't 
    // have a surface normal

    if (dist > gDecalSize)
    {
        R_DecalNode(node->children[0]);
    }
    else if (dist < -gDecalSize)
    {
        R_DecalNode(node->children[1]);
    }
    else
    {
        if (dist < DECAL_DISTANCE && dist > -DECAL_DISTANCE)
        {
            int			w, h, scale;
            float		s, t, size;
            msurface_t* surf;
            int			i;
            mtexinfo_t* tex;

            surf = gDecalModel->surfaces + node->firstsurface;

            // iterate over all surfaces in the node
            for (i = 0; i < node->numsurfaces; i++, surf++)
            {
                if ((surf->flags & (SURF_DRAWTILED | SURF_DRAWTURB)))
                    continue;

                tex = surf->texinfo;

                size = Length(tex->vecs[0]);
                if (size < 0)
                    size = -size;

                if (fabs(size) == 0)
                    continue;

                // project decal center into the texture space of the surface
                s = DotProduct(gDecalPos, tex->vecs[0]) + tex->vecs[0][3] - surf->texturemins[0];
                t = DotProduct(gDecalPos, tex->vecs[1]) + tex->vecs[1][3] - surf->texturemins[1];

                if (size < 1.0)
                {
                    scale = 0;
                    w = gDecalTexture->width << scale;
                    h = gDecalTexture->height << scale;
                }
                else if (size > 7.0)
                {
                    scale = 3;
                    w = gDecalTexture->width << scale;
                    h = gDecalTexture->height << scale;
                }
                else
                {
					scale = gBits[(int)size];
                    w = gDecalTexture->width << scale;
                    h = gDecalTexture->height << scale;
                }

                // move s,t to upper left corner
                s -= (w * 0.5);
                t -= (h * 0.5);

                if (s <= -w || t <= -h ||
                    s > (surf->extents[0] + w) || t > (surf->extents[1] + h))
                {
                    continue; // nope
                }

                // stamp it
				R_DecalCreate(surf, gDecalIndex, scale, s, t);
            }
        }

        R_DecalNode(node->children[0]);
        R_DecalNode(node->children[1]);
    }
}

int DecalListAdd( DECALLIST* pList, int count )
{
    int         i;
    vec3_t      tmp;
    DECALLIST* pdecal;

	pdecal = pList + count;
	for (i = 0; i < count; i++)
	{
		if (pdecal->name == pList[i].name &&
			pdecal->entityIndex == pList[i].entityIndex)
		{
			VectorSubtract(pdecal->position, pList[i].position, tmp);	// Merge
			if (Length(tmp) < 2)	// UNDONE: Tune this '2' constant
				return count;
		}
	}

	// This is a new decal
	return count + 1;
}


typedef int (*qsortFunc_t)( const void *, const void * );

int DecalDepthCompare( const DECALLIST* elem1, const DECALLIST* elem2 )
{
	if (elem1->depth > elem2->depth)
		return -1;
	if (elem1->depth < elem2->depth)
		return 1;

	return 0;
}

int DecalListCreate( DECALLIST* pList )
{
	int total = 0;
	int i, depth;

    for (i = 0; i < MAX_DECALS; i++)
    {
		decal_t* pdecals;
		texture_t* ptexture;

		// Decal is in use and is not a custom decal
		if (!gDecalPool[i].psurface || (gDecalPool[i].flags & FDECAL_CUSTOM))
			continue;

		// compute depth
		pdecals = gDecalPool[i].psurface->pdecals;
		depth = 0;
		while (pdecals && pdecals != &gDecalPool[i])
		{
			depth++;
			pdecals = pdecals->pnext;
		}
		pList[total].depth = depth;
		pList[total].flags = gDecalPool[i].flags;

		pList[total].entityIndex = R_DecalUnProject(&gDecalPool[i], pList[total].position);

		ptexture = Draw_DecalTexture(gDecalPool[i].texture);
		pList[total].name = ptexture->name;

		// Check to see if the decal should be addedo
		total = DecalListAdd(pList, total);
    }

	// Sort the decals lowest depth first, so they can be re-applied in order
	qsort(pList, total, sizeof(DECALLIST), (qsortFunc_t)DecalDepthCompare);

	return total;
}
// ---------------------------------------------------------

int R_DecalUnProject( decal_t* pdecal, vec_t* position )
{
    float s, t, scale, inverseScale;
    float planedist;
    mtexinfo_t* pTexinfo;
    texture_t* ptexture;
    int w, h, shift, entityIndex = 0;

    if (!pdecal || !pdecal->psurface)
        return -1;

    pTexinfo = pdecal->psurface->texinfo;
    scale = fabs(Length(pTexinfo->vecs[0]));
    if (scale != 0.0)
		inverseScale = (1.0 / scale) * (1.0 / scale);
    else
		inverseScale = 0.0;

	ptexture = Draw_DecalTexture(pdecal->texture);

    if (scale < 1.0)
    {
        shift = 0;
        w = ptexture->width << shift;
        h = ptexture->height << shift;
    }
    else if (scale > 7.0)
    {
        shift = 3;
        w = ptexture->width << shift;
        h = ptexture->height << shift;
    }
    else
    {
        shift = gBits[(int)scale];
        w = ptexture->width << shift;
        h = ptexture->height << shift;
    }

    s = (float)pdecal->dx + (float)(w >> 1) + (float)pdecal->psurface->texturemins[0] - pTexinfo->vecs[0][3];
    t = (float)pdecal->dy + (float)(h >> 1) + (float)pdecal->psurface->texturemins[1] - pTexinfo->vecs[1][3];

    VectorScale(pTexinfo->vecs[0], s * inverseScale, position);
    VectorMA(position, t * inverseScale, pTexinfo->vecs[1], position);

    if (pdecal->psurface->flags & SURF_PLANEBACK)
        planedist = pdecal->psurface->plane->dist + 2.0;
    else
        planedist = pdecal->psurface->plane->dist - 2.0;

	VectorMA(position, planedist, pdecal->psurface->plane->normal, position);

	entityIndex = pdecal->entityIndex;

    if (pdecal->entityIndex)
    {
        hull_t* phull;
        vec3_t temp;
        edict_t* pEdict;
        model_t* pModel = NULL;

        pEdict = &sv.edicts[entityIndex];
        if (pEdict->v.modelindex)
			pModel = sv.models[pEdict->v.modelindex];

        // Make sure it's a brush model
        if (!pModel || pModel->type != mod_brush)
            return 0;

        if (pEdict->v.angles[0] || pEdict->v.angles[1] || pEdict->v.angles[2])
        {
            vec3_t forward, right, up;
			AngleVectorsTranspose(pEdict->v.angles, forward, right, up);

            position[0] = DotProduct(position, forward);
            position[1] = DotProduct(position, right);
            position[1] = DotProduct(position, up);
        }

		if (pModel->firstmodelsurface)
        {
			phull = &pModel->hulls[0]; // always use #0 hull
			VectorAdd(pEdict->v.origin, phull->clip_mins, temp);
			VectorAdd(temp, position, position);
        }
    }

	return entityIndex;
}


// Shoots a decal onto the surface of the BSP.  position is the center of the decal in world coords
void R_DecalShoot_( texture_t* ptexture, int index, int entity, int modelIndex, vec_t* position, int flags )
{
	mnode_t* pnodes;
	cl_entity_t* pent;

	VectorCopy(position, gDecalPos);	// Pass position in global

	pent = &cl_entities[entity];
	if (pent)
	{
		gDecalModel = pent->model;
		if (!gDecalModel)
		{
			if (modelIndex)
				gDecalModel = cl.model_precache[modelIndex];

			if (!gDecalModel)
			{
				if (sv.active)
					gDecalModel = sv.models[sv.edicts[entity].v.modelindex];
			}
		}
	}
	else
	{
		gDecalModel = NULL;
	}

	if (!pent || !gDecalModel || gDecalModel->type != mod_brush || !ptexture)
	{
		Con_DPrintf("Decals must hit mod_brush!\n");
		return;
	}

	pnodes = gDecalModel->nodes;

	if (entity)
	{
		hull_t* phull;
		vec3_t temp;

		if (gDecalModel->firstmodelsurface)
		{
			phull = &gDecalModel->hulls[0]; // always use #0 hull

			VectorSubtract(position, phull->clip_mins, temp);
			VectorSubtract(temp, pent->origin, gDecalPos);
			pnodes += phull->firstclipnode;
		}

		if (pent->angles[0] || pent->angles[1] || pent->angles[2])
		{
			vec3_t forward, right, up;
			AngleVectors(pent->angles, forward, right, up);
			VectorCopy(gDecalPos, temp);

			gDecalPos[0] = DotProduct(temp, forward);
			gDecalPos[1] = -DotProduct(temp, right);
			gDecalPos[2] = DotProduct(temp, up);
		}
	}

	// More state used by R_DecalNode()
	gDecalEntity = entity;
	gDecalTexture = ptexture;
	gDecalIndex = index;
	gDecalFlags = flags;
	gDecalSize = ptexture->width >> 1;

	if (gDecalSize < (int)(ptexture->height >> 1))
		gDecalSize = ptexture->height >> 1;

	R_DecalNode(pnodes);
}

// Shoots a decal onto the surface of the BSP.  position is the center of the decal in world coords
// This is called from cl_parse.c, cl_tent.c
void R_DecalShoot( int textureIndex, int entity, int modelIndex, vec_t* position, int flags )
{
	texture_t* ptexture;

	ptexture = Draw_DecalTexture(textureIndex);
	R_DecalShoot_(ptexture, textureIndex, entity, modelIndex, position, flags);
}

void R_CustomDecalShoot( texture_t* ptexture, int playernum, int entity, int modelIndex, vec_t* position, int flags )
{
	int plindex = ~playernum;
	R_DecalShoot_(ptexture, plindex, entity, modelIndex, position, flags);
}

// Check for intersecting decals on this surface
decal_t* R_DecalIntersect( decal_t* plist, int* pcount, int x, int y )
{
	decal_t* plast;
	int         dist, lastDist, dx, dy;
	unsigned int maxWidth;
	texture_t* ptexture;

	plast = NULL;

	lastDist = 0xFFFF;
	*pcount = 0;

	maxWidth = (gDecalTexture->width * 3) / 2;

	while (plist)
	{
		ptexture = Draw_DecalTexture(plist->texture);

		// Don't steal bigger decals and replace them with smaller decals
		// Don't steal permanent decals
		if (!(plist->flags & FDECAL_PERMANENT))
		{
			if (maxWidth >= ptexture->width)
			{
				// Now figure out the part of the projection that intersects plist's
				// clip box [0,0,1,1].
				dx = x - plist->dx - (ptexture->width / 2);
				if (dx < 0)
					dx = -dx;

				dy = y - plist->dy - (ptexture->height / 2);
				if (dy < 0)
					dy = -dy;

				if (dy <= dx)
					dist = dx + (dy / 2);
				else
					dist = dy + (dx / 2);

				if (dist < 6)
				{
					*pcount += 1;

					if (!plast || dist <= lastDist)
					{
						lastDist = dist;
						plast = plist;
					}
				}
			}
		}

		plist = plist->pnext;
	}

	return plast;
}

extern int LeafId( msurface_t* psurface );

// Allocate and initialize a decal from the pool, on surface with offsets x, y
void R_DecalCreate( msurface_t* psurface, int textureIndex, int scale, int x, int y )
{
	decal_t* pdecal, * pold;
	int				count;

	if (r_drawflat.value == 2.0)
		Con_Printf("Leaf: %d\n", LeafId(psurface));

	pold = R_DecalIntersect(psurface->pdecals, &count, x + (gDecalTexture->width / 2), y + (gDecalTexture->height / 2));
	if (count < MAX_OVERLAP_DECALS)
		pold = NULL;

	pdecal = R_DecalAlloc(pold);
	pdecal->texture = textureIndex;
	pdecal->flags = gDecalFlags;
	pdecal->dx = x;
	pdecal->dy = y;
	pdecal->pnext = NULL;

	if (psurface->pdecals)
	{
		pold = psurface->pdecals;
		while (pold->pnext)
			pold = pold->pnext;
		pold->pnext = pdecal;
	}
	else
	{
		psurface->pdecals = pdecal;
	}

	// Tag surface
	pdecal->psurface = psurface;

	// Set scaling
	pdecal->scale = scale;
	pdecal->entityIndex = gDecalEntity;

	R_InvalidateSurface(psurface);
}

// Set up a palette for a given decal based on its texture and store it
void R_DecalCacheSurface( unsigned char* psource, unsigned short* ppalette, unsigned char* pdest )
{
	int				i, j;
	unsigned char   pix;
	unsigned short* pColor;

	for (i = 0; i < blocksize; i++)
	{
		for (j = 0; j < blocksize; j++)
		{
			pix = psource[j];
			pColor = &ppalette[pix * 4];
			pdest[0] = pColor[2];
			pdest[1] = pColor[1];
			pdest[2] = pColor[0];
			pdest[3] = pix;
			pdest += 4;
		}
		psource += sourcetstep;
	}

	r_offset += sourcevstep;
	if (r_offset >= r_stepback)
		r_offset -= r_stepback;
}

// Render all active decals that are applied on surfaces of various types
void R_DrawDecals( decal_t* list, byte* decalbuffer, int s, int t )
{
	for (; list; list = list->pnext)
	{
		int x, y, w, h, srcMod, destMod, mip;
		int i, j;
		int vblocks, hblocks;
		int savew;
		unsigned char* psource, * pdest, pix;
		unsigned char* ppalette, * pColor;
		texture_t* texture;

		if (list->scale)
		{
			R_DrawScaledDecal(list, decalbuffer, s, t);
			continue;
		}

		mip = r_drawsurf.surfmip;

		// Render a non-scaled regular-sized decal
		texture = Draw_DecalTexture(list->texture);

		x = (list->dx >> mip) - s;
		y = (list->dy >> mip) - t;
		w = texture->width >> mip;
		h = texture->height >> mip;

		if (x >= blocksize || y >= blocksize)
			continue;
		if (x <= -w || y <= -h)
			continue;

		ppalette = (unsigned char*)((byte*)texture + texture->paloffset);
		psource = (unsigned char*)((byte*)texture + texture->offsets[mip]);

		savew = w;
		w += x;
		h += y;

		if (y < 0)
		{
			psource -= y * savew;
			y = 0;
		}
		if (x < 0)
		{
			psource -= x;
			x = 0;
		}

		if (w > blocksize)
			w = blocksize;
		if (h > blocksize)
			h = blocksize;

		vblocks = w - x;
		hblocks = h - y;

		pdest = decalbuffer + ((x + y * blocksize) * 4);
		destMod = (blocksize - vblocks) * 4;
		srcMod = savew;

		if (texture->name[0] == '{')
		{
			for (i = hblocks; i != 0; i--)
			{
				for (j = 0; j < vblocks; j++)
				{
					pix = psource[j];
					if (pix != TRANSPARENT_COLOR)
					{
						pColor = &ppalette[pix * 3];
						pdest[0] = pColor[2];
						pdest[1] = pColor[1];
						pdest[2] = pColor[0];
						pdest[3] = 0;
					}
					pdest += 4;
				}
				psource += srcMod;
				pdest += destMod;
			}
		}
		else
		{
			for (i = hblocks; i != 0; i--)
			{
				for (j = 0; j < vblocks; j++)
				{
					pix = psource[j];
					if (pix != 0)
					{
						pColor = ppalette;
						pdest[0] += (unsigned short)(pix * (pColor[768 - 3] - pdest[0])) >> 8;
						pdest[1] += (unsigned short)(pix * (pColor[768 - 2] - pdest[1])) >> 8;
						pdest[2] += (unsigned short)(pix * (pColor[768 - 1] - pdest[2])) >> 8;
						pdest[3] = 0;
					}
					pdest += 4;
				}
				psource += srcMod;
				pdest += destMod;
			}
		}
	}
}

// Render a scaled decal that is applied on a surface with a type
void R_DrawScaledDecal( decal_t* list, byte* decalbuffer, int s, int t )
{
	int x, y, w, h, mod, scale, stepbits;
	int	i, j;
	int	vblocks, hblocks, scaledblocks, scaledvblocks;
	unsigned char* psource, * pdest, pix;
	unsigned char* ppalette, * pColor;
	texture_t* texture;
	int savew;

	scale = (1 << list->scale) - 1;

	// Render a scaled decal with variable size
	texture = Draw_DecalTexture(list->texture);

	x = (list->dx >> r_drawsurf.surfmip) - s;
	y = (list->dy >> r_drawsurf.surfmip) - t;
	w = texture->width >> r_drawsurf.surfmip;
	h = texture->height >> r_drawsurf.surfmip;

	savew = w;
	w <<= list->scale;
	h <<= list->scale;

	if (x >= blocksize || y >= blocksize)
		return;
	if (x <= -w || y <= -h)
		return;

	ppalette = (unsigned char*)((byte*)texture + texture->paloffset);
	psource = (unsigned char*)((byte*)texture + texture->offsets[r_drawsurf.surfmip]);

	stepbits = scale;

	w += x;
	h += y;

	scaledblocks = 0;

	if (y < 0)
	{
		psource += savew * (-y >> list->scale);
		stepbits -= (-y & scale);
		y = 0;
	}
	if (x < 0)
	{
		psource += (-x >> list->scale);
		scaledblocks = (-x & scale);
		x = 0;
	}

	if (w > blocksize)
		w = blocksize;
	if (h > blocksize)
		h = blocksize;

	vblocks = w - x;
	hblocks = h - y;
	scaledvblocks = vblocks + scaledblocks;

	pdest = decalbuffer + ((x + y * blocksize) * 4);
	mod = (blocksize - vblocks) * 4;

	if (!list->scale)
		return; // can't render without a scale factor

	if (texture->name[0] == '{')
	{
		for (i = hblocks; i != 0; i--)
		{
			for (j = scaledblocks; j < scaledvblocks; j++)
			{
				int slot = j >> list->scale;

				pix = psource[slot];
				if (pix != TRANSPARENT_COLOR)
				{
					pColor = &ppalette[pix * 3];
					pdest[0] = pColor[2];
					pdest[1] = pColor[1];
					pdest[2] = pColor[0];
					pdest[3] = 0;
				}
				pdest += 4;
			}
			pdest += mod;

			if (stepbits != 0)
			{
				stepbits--;
			}
			else
			{
				stepbits = scale;
				psource += savew;
			}
		}
	}
	else
	{
		for (i = hblocks; i != 0; i--)
		{
			for (j = scaledblocks; j < scaledvblocks; j++)
			{
				int slot = j >> list->scale;

				pix = psource[slot];
				if (pix != 0)
				{
					pColor = ppalette;
					pdest[0] += (unsigned short)(pix * (pColor[768 - 3] - pdest[0])) >> 8;
					pdest[1] += (unsigned short)(pix * (pColor[768 - 2] - pdest[1])) >> 8;
					pdest[2] += (unsigned short)(pix * (pColor[768 - 1] - pdest[2])) >> 8;
					pdest[3] = 0;
				}
				pdest += 4;
			}
			pdest += mod;

			if (stepbits != 0)
			{
				stepbits--;
			}
			else
			{
				stepbits = scale;
				psource += savew;
			}		
		}
	}
}

// Draw a texture for the decal block, calculate the lighting
unsigned short* R_DecalLightSurface( byte* psource, unsigned short* prowdest )
{
	colorVec        clightleft, clightright, clightleftstep, clightrightstep;
	colorVec        clight, clightstep;
	int             i, j;
	uint32          r, g, b;

	int fFullBright = FALSE;

	ColorCopy(r_lightptr[0], clightleft);
	ColorCopy(r_lightptr[1], clightright);

	r_lightptr += r_lightwidth;

	// Compute the color component steps for the left and right edges of the block
	ColorSubtract(r_lightptr[0], clightleft, clightleftstep);
	ColorShift(clightleftstep, blockdivshift);
	ColorSubtract(r_lightptr[1], clightright, clightrightstep);
	ColorShift(clightrightstep, blockdivshift);

	for (i = 0; i < blocksize; i++)
	{
		ColorSubtract(clightright, clightleft, clightstep);
		ColorShift(clightstep, blockdivshift);
		ColorCopy(clightleft, clight);

		for (j = 0; j < blocksize; j++)
		{
			fFullBright = (r_drawsurf.texture->name[2] == '~' && psource[3] > 223) ? TRUE : FALSE;
			if (fFullBright)
			{
				r = psource[0];
				g = psource[1];
				b = psource[2];

				if (is15bit)
				{
					prowdest[j] = PACKEDRGB555(r, g, b);
				}
				else
				{
					prowdest[j] = PACKEDRGB565(r, g, b);
				}
			}
			else
			{
				r = r_lut[(clight.r & 0xFF00) + psource[0]];
				g = r_lut[(clight.g & 0xFF00) + psource[1]];
				b = r_lut[(clight.b & 0xFF00) + psource[2]];

				if (is15bit)
				{
					prowdest[j] = PACKEDRGB555(r, g, b);
				}
				else
				{
					prowdest[j] = PACKEDRGB565(r, g, b);
				}
			}
			psource += 4;

			ColorAdd(clight, clightstep, clight);
		}

		ColorAdd(clightleft, clightleftstep, clightleft);
		ColorAdd(clightright, clightrightstep, clightright);

		prowdest += (surfrowbytes / 2);
	}

	return prowdest;
}

// Draw a texture for the decal block, fullbright mode
unsigned short* R_DecalFullbrightSurface( byte* psource, unsigned short* prowdest )
{
	int             i, j;
	uint32          r, g, b;

	r_lightptr += r_lightwidth;

	for (i = 0; i < blocksize; i++)
	{
		for (j = 0; j < blocksize; j++)
		{
			r = psource[0];
			g = psource[1];
			b = psource[2];

			if (is15bit)
			{
				prowdest[j] = PACKEDRGB555(r, g, b);
			}
			else
			{
				prowdest[j] = PACKEDRGB565(r, g, b);
			}
			psource += 4;
		}

		prowdest += (surfrowbytes / 2);
	}

	return prowdest;
}