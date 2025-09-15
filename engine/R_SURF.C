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
			blocklights[i].r = (lightgammatable[blocklights[i].r >> 6] & 1020) << 6;

		if (blocklights[i].g > 0xFF00)
			blocklights[i].g = 0xFF00;
		else
			blocklights[i].g = (lightgammatable[blocklights[i].g >> 6] & 1020) << 6;

		if (blocklights[i].b > 0xFF00)
			blocklights[i].b = 0xFF00;
		else
			blocklights[i].b = (lightgammatable[blocklights[i].b >> 6] & 1020) << 6;
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
			if (r_mmx.value)
			{
				pblockdrawer = R_DrawSurfaceBlock16MMX;
			}
			else
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

// Pre-computed random table for texture animation
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

			prowdest = (unsigned short*)((byte*)prowdest + surfrowbytes);
			psource += sourcetstep;
		}

		r_offset += sourcevstep;

		if (r_offset >= r_stepback)
			r_offset -= r_stepback;
	}

	// restore the FPU state
	__asm fldcw fpu_cw
}

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

    if (!gHasMMXTechnology)
        return;

    static unsigned long long mm_mul = 0x555555555555;          // 0.33333 in 16.16 fixed point

    static unsigned long long mm1_and1 = 0x0F800F800F800F8;     // mm1 reg 1-st pand
    static unsigned long long mm3_and1 = 0x0FC000000FC00;       // mm3 reg 1-st pand 
    static unsigned long long mm3_and2 = 0x0F8000000F800;       // mm3 reg 2-nd pand 
    static unsigned long long mm1_and2 = 0x2000000420000004;    // mm1 reg 2-nd pand
    static unsigned long long mm1_and3 = 0x2000000820000008;    // mm1 reg 3-rd pand
    static unsigned long long mm6_last = 0ull, mm7_last = 0ull; // mm6 prev & mm7 prev
    
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
		pmulhw      mm4, mm_mul                             ; apply the block multiplier
		punpcklwd   mm5, mm3
		punpckldq   mm5, mm1
		psrlw       mm5, 1
		pmulhw      mm5, mm_mul                             ; apply the block multiplier

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
		pmulhw      mm6, mm_mul                             ; apply the block multiplier
		punpcklwd   mm7, mm3
		punpckldq   mm7, mm1
		mov         ebx, texture
		movd        mm1, blockdivshift
		psrlw       mm7, 1
		pmulhw      mm7, mm_mul                             ; apply the block multiplier

		movq        mm6_last, mm6                           ; copy
		psubw       mm6, mm4
		movq        mm7_last, mm7                           ; copy
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
		pand        mm1, mm1_and1
		pand        mm3, mm3_and2
		pmaddwd     mm1, mm1_and3
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
		movq        mm4, mm6_last                           ; copy
		movq        mm5, mm7_last                           ; copy
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
		pmulhw      mm4, mm_mul                             ; apply the block multiplier
		punpcklwd   mm5, mm3
		punpckldq   mm5, mm1
		psrlw       mm5, 1
		pmulhw      mm5, mm_mul                             ; apply the block multiplier
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
		pmulhw      mm6, mm_mul                             ; apply the block multiplier
		punpcklwd   mm7, mm3
		punpckldq   mm7, mm1
		mov         ebx, texture
		movd        mm1, blockdivshift
		psrlw       mm7, 1
		pmulhw      mm7, mm_mul                             ; apply the block multiplier

		movq        mm6_last, mm6                           ; copy
		psubw       mm6, mm4
		movq        mm7_last, mm7                           ; copy
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
		pand        mm1, mm1_and1
		pand        mm3, mm3_and1
		pmaddwd     mm1, mm1_and2
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
		movq        mm4, mm6_last                           ; copy
		movq        mm5, mm7_last                           ; copy
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


void R_DrawSurfaceBlock16Holes( void )
{
	// TODO: Implement
}


void R_DrawSurfaceBlock16Fullbright1( void )
{
	// TODO: Implement
}


void R_DrawSurfaceBlock16Fullbright2( void )
{
	// TODO: Implement
}


void R_DrawSurfaceBlock16Fullbright3( void )
{
	// TODO: Implement
}


void R_DrawSurfaceBlock16Fullbright4( void )
{
	// TODO: Implement
}


void R_DrawSurfaceBlock16Fullbright5( void )
{
	// TODO: Implement
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
	// TODO: Implement
}


void R_DecalRemoveAll( int textureIndex )
{
	// TODO: Implement
}

// Calculate the rectangle for a decals on a single surface
// Used by R_DrawSurface
void R_DecalRect( decal_t* decals, vrect_t* decalrect, int surfmip )
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

// Set up a palette for a given decal based on its texture and store it
void R_DecalCacheSurface( unsigned char* psource, unsigned short* ppalette, unsigned char* pdest )
{
	// TODO: Implement
}

// Render all active decals that are applied on surfaces of various types
void R_DrawDecals( decal_t* list, byte* decalbuffer, int s, int t )
{
	// TODO: Implement
}

// Render a scaled decal that is applied on a surface with a type
void R_DrawScaledDecal( decal_t* list, byte* decalbuffer, int s, int t )
{
	// TODO: Implement
}

// Draw a texture for the decal block, calculate the lighting
unsigned short* R_DecalLightSurface( byte* psource, unsigned short* prowdest )
{
	// TODO: Implement
	return NULL;
}

// Draw a texture for the decal block, fullbright mode
unsigned short* R_DecalFullbrightSurface( byte* psource, unsigned short* prowdest )
{
	// TODO: Implement
	return NULL;
}