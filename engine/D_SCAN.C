// d_scan.c
//
// Portable C scan-level rasterization code, all pixel depths.

#include "quakedef.h"
#include "d_local.h"
#include "pr_cmds.h"

int     d_depth, d_fader, d_fadeg, d_fadeb, d_fadelevel;
float   d_fadestart, d_fadetime;
int     d_zidist;

#define MAX_WATER_PALETTE 512
short   gWaterPalette[MAX_WATER_PALETTE];
word* gWaterLastPalette = NULL;

#define MAX_FOG_ZSPANS 64
colorVec d_fogtable[MAX_FOG_ZSPANS][64];

struct
{
	int sMask;  // s mask
	int tMask;  // t mask
	int tShift; // texture offset to adjust the positioning
} gTilemap;

void D_BuildFogTable( qboolean blend )
{
	// TODO: Implement
}

void D_SetFlash( void )
{
	d_fadelevel = 255;
	d_fader = 255;
	d_fadeg = 255;
	d_fadeb = 255;
	d_fadetime = 0.5;
	d_fadestart = cl.time;
	D_BuildFogTable(TRUE);
}

void D_SetHurt( void )
{
	d_fadelevel = 255;
	d_fader = 255;
	d_fadeg = 32;
	d_fadeb = 32;
	d_fadestart = cl.time;
	d_fadetime = 0.5;
	D_BuildFogTable(TRUE);
}

void D_SetFadeColor( int r, int g, int b, int fog )
{
	if (cl.sf.fadeEnd > cl.time)
		return;

	d_fadestart = cl.time;
	d_fadetime = 0.0;
	d_fadelevel = 128;

	if (r != d_fader || g != d_fadeg || b != d_fadeb || fog != d_depth)
	{
		d_fader = r;
		d_fadeg = g;
		d_fadeb = b;
		d_depth = fog;

		D_BuildFogTable(TRUE);
	}
}

// Set the fade parameters
void D_SetScreenFade( int r, int g, int b, int alpha, int type )
{
	d_fadestart = cl.time;
	d_fadetime = 0.0;
	d_fadelevel = alpha;

	d_fader = r;
	d_fadeg = g;
	d_fadeb = b;
	d_depth = 0;

	D_BuildFogTable(type != 0 ? TRUE : FALSE);
}

// Initialize fade parameters, build the fog table
void D_InitFade( qboolean blend )
{
	d_fadelevel = 128;
	d_fadetime = 0.0;
	d_fadestart = 0.0;
	d_fader = 32;
	d_fadeg = 32;
	d_fadeb = 128;
	D_BuildFogTable(blend);
}

/*
=============
D_WarpScreen

// this performs a slight compression of the screen at the same time as
// the sine warp, to keep the edges from wrapping
=============
*/
void D_WarpScreen( void )
{
	// TODO: Implement
}

//-----------------------------------------------------------------------------
// Water texture
//-----------------------------------------------------------------------------
short watertex[CYCLE * CYCLE];
short watertex2[CYCLE * CYCLE];
short gWaterTextureBuffer[CYCLE * CYCLE];

short* gWaterTex = watertex, * gWaterTexOld = watertex2;
float gDropFrame, gWaterFrame = 0.0f;

void WaterTextureDrop( int x, int y, int amount )
{
	gWaterTexOld[((y & CYCLE_MASK) << 7) + (x & CYCLE_MASK)] += amount;
	gWaterTexOld[((y & CYCLE_MASK) << 7) + ((x + 1) & CYCLE_MASK)] += (amount >> 2);
	gWaterTexOld[((y & CYCLE_MASK) << 7) + ((x - 1) & CYCLE_MASK)] += (amount >> 2);
	gWaterTexOld[(((y + 1) & CYCLE_MASK) << 7) + (x & CYCLE_MASK)] += (amount >> 2);
	gWaterTexOld[(((y - 1) & CYCLE_MASK) << 7) + (x & CYCLE_MASK)] += (amount >> 2);
}

// Build a water texture palette from the given source palette
void WaterTexturePalette( short* pPalette, word* pSourcePalette )
{
    int i;

    for (i = 0; i < 512; i++)
    {
        int r, g, b;

        if (i < 256)
        {
			int blend;

			blend = ((i * (pSourcePalette[6] - pSourcePalette[10])) >> 8);
			r = pSourcePalette[10] + blend;

			blend = ((i * (pSourcePalette[5] - pSourcePalette[9])) >> 8);
			g = pSourcePalette[9] + blend;

			blend = ((i * (pSourcePalette[4] - pSourcePalette[8])) >> 8);
			b = pSourcePalette[8] + blend;
        }
        else
        {
			int blend;

			blend = (((i - 256) * (pSourcePalette[2] - pSourcePalette[6])) >> 8);
			r = pSourcePalette[6] + blend;

			blend = (((i - 256) * (pSourcePalette[1] - pSourcePalette[5])) >> 8);
			g = pSourcePalette[5] + blend;

			blend = (((i - 256) * (pSourcePalette[0] - pSourcePalette[4])) >> 8);
			b = pSourcePalette[4] + blend;
        }

        if (is15bit)
        {
            pPalette[i] = PACKEDRGB555(r, g, b);
        }
        else
        {
            pPalette[i] = PACKEDRGB565(r, g, b);
        }
    }

    D_SetFadeColor(pSourcePalette[14], pSourcePalette[13], pSourcePalette[12], pSourcePalette[18]);
}

// Simple smoothing filter to even out the water texture
void WaterTextureSmooth( short* pOldBuffer, short* pNewBuffer )
{
	int i, p;
	
	for (i = 0; i < (CYCLE * CYCLE); i++)
	{
		p = ((pOldBuffer[(i + CYCLE) & 0x3FFF] +
			pOldBuffer[((i + CYCLE) - (CYCLE - 1)) & 0x3FFF] +
			pOldBuffer[((i + CYCLE) - (CYCLE + 1)) & 0x3FFF] +
			pOldBuffer[((i + CYCLE) - (CYCLE * 2)) & 0x3FFF]) >> 1) - pNewBuffer[i];

		pNewBuffer[i] = p - (p >> 6);
	}
}

void WaterTextureSwap( void )
{
	short* tmp;

	// Swap the buffers
	tmp = gWaterTex;
	gWaterTex = gWaterTexOld;
	gWaterTexOld = tmp;
}

float WaterTextureClear( void )
{
	float time;

	// The time since the last drop
	time = cl.time - 0.1;
	gWaterFrame = time;
	gDropFrame = time;

	return 0.1;
}

// Main routine to update the water texture
void WaterTextureUpdate( word* pPalette, float dropTime, texture_t* texture )
{
	int		i;
	int		newPalette;
	int		shift;
	float	time;
	byte* samples;

	// Calculate the time
	time = cl.time - gWaterFrame;
	if (time < 0.0)
		time = WaterTextureClear();
	else if (time < 0.05)
		time = 0.0;

	gWaterFrame += time;

	if (time > 0.0)
	{
		WaterTextureSwap();

		if (gWaterFrame - gDropFrame > dropTime)
		{
			gDropFrame = gWaterFrame;
			WaterTextureDrop(RandomLong(0, 0x7FFF), RandomLong(0, 0x7FFF), RandomLong(0, 0x3FF));
		}

		WaterTextureSmooth(gWaterTexOld, gWaterTex);
	}

	if (gWaterLastPalette == pPalette)
	{
		newPalette = FALSE;
	}
	else
	{
		for (i = 0; i < 256; i++)
		{
			// Pack into 15/16-bit
			gWaterPalette[i] = hlRGB(pPalette, i);
		}

		newPalette = TRUE;
		D_SetFadeColor(pPalette[14], pPalette[13], pPalette[12], pPalette[18]);
		gWaterLastPalette = pPalette;
	}

	shift = (Q_log2(texture->width) & 0xFF);
	if (time > 0.0 || newPalette)
	{
		int x, y;

		samples = (byte*)((byte*)texture + texture->offsets[0]);

		for (i = 0; i < (CYCLE * CYCLE); i++)
		{
			x = (i + (gWaterTex[i] >> 4)) & (texture->width - 1);
			y = ((i >> 7) - (gWaterTex[i] >> 4)) & (texture->height - 1);
			gWaterTextureBuffer[i] = gWaterPalette[samples[x + (y << shift)]];
		}
	}

	cacheblock = (pixel_t*)gWaterTextureBuffer;
	cachewidth = CYCLE;

	TilingSetup(CYCLE_MASK, CYCLE_MASK, 7);

}

// Set s/t masks and the texture shifting for water surfaces and scrolling textures
void TilingSetup( int sMask, int tMask, int tShift )
{
	gTilemap.sMask = sMask;
	gTilemap.tMask = tMask;
	gTilemap.tShift = tShift;
}

/*
=============
D_DrawTiled8
=============
*/
void D_DrawTiled8( espan_t* pspan )
{
	int				count, spancount;
	unsigned short* pbase, * pdest;
	fixed16_t		s, t, snext, tnext, sstep, tstep;
	float			sdivz, tdivz, zi, z, du, dv;
	float			sdivz16stepu, tdivz16stepu, zi16stepu;

	set_fpu_cw();

	pbase = (unsigned short*)cacheblock;

	sdivz16stepu = d_sdivzstepu * 16;
	tdivz16stepu = d_tdivzstepu * 16;
	zi16stepu = d_zistepu * 16;

	do
	{
		pdest = (unsigned short*)((byte*)d_viewbuffer +
				(screenwidth * pspan->v) + pspan->u * 2);

		count = pspan->count;

	// calculate the initial s/z, t/z, 1/z, s, and t and clamp
		du = (float)pspan->u;
		dv = (float)pspan->v;

		sdivz = d_sdivzorigin + dv * d_sdivzstepv + du * d_sdivzstepu;
		tdivz = d_tdivzorigin + dv * d_tdivzstepv + du * d_tdivzstepu;
		zi = d_ziorigin + dv * d_zistepv + du * d_zistepu;
		z = (float)0x10000 / zi;	// prescale to 16.16 fixed-point

		d_zidist = (int)(sdivz * z);
		s = (int)(sdivz * z) + sadjust;

		d_zidist = (int)(tdivz * z);
		t = (int)(tdivz * z) + tadjust;

		do
		{
		// calculate s and t at the far end of the span          
			if (count >= 16)
				spancount = 16;
			else
				spancount = count;

			count -= spancount;

		// calculate s/z, t/z, zi->fixed s and t at far end of span,
		// calculate s and t steps across span by shifting
			sdivz += sdivz16stepu;
			tdivz += tdivz16stepu;
			zi += zi16stepu;
			z = (float)0x10000 / zi;	// prescale to 16.16 fixed-point

			d_zidist = (int)(sdivz * z);
			snext = (int)(sdivz * z) + sadjust;

			d_zidist = (int)(tdivz * z);
			tnext = (int)(tdivz * z) + tadjust;

			sstep = (snext - s) >> 4;
			tstep = (tnext - t) >> 4;

			do
			{
				*pdest++ = pbase[((s >> 16) & gTilemap.sMask) + (((t >> 16) & gTilemap.tMask) << gTilemap.tShift)];
				s += sstep;
				t += tstep;
			} while (--spancount > 0);

			s = snext;
			t = tnext;

		} while (count > 0);

	} while ((pspan = pspan->pnext) != NULL);

	restore_fpu_cw();
}

/*
=============
D_DrawTiled8Trans
=============
*/
void D_DrawTiled8Trans( espan_t* pspan )
{
	int				count, spancount, izistep;
	int				izi;
	unsigned short* pbase, * pdest;
	fixed16_t		s, t, snext, tnext, sstep, tstep;
	float			sdivz, tdivz, zi, z, du, dv;
	float			sdivz16stepu, tdivz16stepu, zi16stepu;
	short* pz;

	pbase = (unsigned short*)cacheblock;

	sdivz16stepu = d_sdivzstepu * 16;
	tdivz16stepu = d_tdivzstepu * 16;
	zi16stepu = d_zistepu * 16;

// we count on FP exceptions being turned off to avoid range problems
	izistep = (int)(d_zistepu * 0x8000 * 0x10000);

	do
	{
		pdest = (unsigned short*)((byte*)d_viewbuffer +
				(screenwidth * pspan->v) + pspan->u * 2);
		pz = &zspantable[pspan->v][pspan->u];

		count = pspan->count;

	// calculate the initial s/z, t/z, 1/z, s, and t and clamp
		du = (float)pspan->u;
		dv = (float)pspan->v;

		sdivz = d_sdivzorigin + dv * d_sdivzstepv + du * d_sdivzstepu;
		tdivz = d_tdivzorigin + dv * d_tdivzstepv + du * d_tdivzstepu;
		zi = d_ziorigin + dv * d_zistepv + du * d_zistepu;
		z = (float)0x10000 / zi;	// prescale to 16.16 fixed-point

		s = (int)(sdivz * z) + sadjust;
		t = (int)(tdivz * z) + tadjust;

		if (zi > 0.9)
		{
			izi = 0x7F000000;
			izistep = 0;
		}
		else
		{
			// we count on FP exceptions being turned off to avoid range problems
			izi = (int)(zi * 0x8000 * 0x10000);
		}

		do
		{
		// calculate s and t at the far end of the span          
			if (count >= 16)
				spancount = 16;
			else
				spancount = count;

			count -= spancount;

		// calculate s/z, t/z, zi->fixed s and t at far end of span,
		// calculate s and t steps across span by shifting
			sdivz += sdivz16stepu;
			tdivz += tdivz16stepu;
			zi += zi16stepu;
			z = (float)0x10000 / zi;	// prescale to 16.16 fixed-point

			snext = (int)(sdivz * z) + sadjust;
			tnext = (int)(tdivz * z) + tadjust;

			sstep = (snext - s) >> 4;
			tstep = (tnext - t) >> 4;

			do
			{
				if (*pz < (izi >> 16))
				{
					unsigned short r, g, b;
					unsigned short newcolor, oldcolor;

					newcolor = pbase[((s >> 16) & gTilemap.sMask) + (((t >> 16) & gTilemap.tMask) << gTilemap.tShift)];
					oldcolor = *pdest;

					if (is15bit)
					{
						r = ScaleToColor(oldcolor, newcolor, 0x7C00, r_blend) & 0x7C00;
						g = ScaleToColor(oldcolor, newcolor, 0x03E0, r_blend) & 0x03E0;
						b = ScaleToColor(oldcolor, newcolor, 0x001F, r_blend) & 0x001F;
					}
					else
					{
						r = ScaleToColor(oldcolor, newcolor, 0xF800, r_blend) & 0xF800;
						g = ScaleToColor(oldcolor, newcolor, 0x07E0, r_blend) & 0x07E0;
						b = ScaleToColor(oldcolor, newcolor, 0x001F, r_blend) & 0x001F;
					}

					*pdest = r | g | b;
				}

				pdest++;
				pz++;
				s += sstep;
				t += tstep;
				izi += izistep;
			} while (--spancount > 0);

			s = snext;
			t = tnext;

		} while (count > 0);

	} while ((pspan = pspan->pnext) != NULL);
}

#if	!id386

/*
=============
D_DrawSpans8
=============
*/
void D_DrawSpans8( espan_t* pspan )
{
	int				count, spancount;
    unsigned short* pbase, * pdest;
	fixed16_t		s, t, snext, tnext, sstep, tstep;
	float			sdivz, tdivz, zi, z, du, dv, spancountminus1;
	float			sdivz8stepu, tdivz8stepu, zi8stepu;

	sstep = 0;	// keep compiler happy
	tstep = 0;	// ditto

	pbase = (unsigned short*)cacheblock;

	sdivz8stepu = d_sdivzstepu * 8;
	tdivz8stepu = d_tdivzstepu * 8;
	zi8stepu = d_zistepu * 8;

	do
	{
		pdest = (unsigned short*)((byte*)d_viewbuffer +
		    (screenwidth * pspan->v) + pspan->u * 2);

		count = pspan->count;

	// calculate the initial s/z, t/z, 1/z, s, and t and clamp
		du = (float)pspan->u;
		dv = (float)pspan->v;

        sdivz = d_sdivzorigin + dv * d_sdivzstepv + du * d_sdivzstepu;
        tdivz = d_tdivzorigin + dv * d_tdivzstepv + du * d_tdivzstepu;
        zi = d_ziorigin + dv * d_zistepv + du * d_zistepu;
		z = (float)0x10000 / zi;	// prescale to 16.16 fixed-point

		s = (int)(sdivz * z) + sadjust;
		if (s > bbextents)
			s = bbextents;
		else if (s < 0)
			s = 0;

		t = (int)(tdivz * z) + tadjust;
		if (t > bbextentt)
			t = bbextentt;
		else if (t < 0)
			t = 0;

		do
		{
		// calculate s and t at the far end of the span
			if (count >= 8)
				spancount = 8;
			else
				spancount = count;

			count -= spancount;

			if (count)
			{
			// calculate s/z, t/z, zi->fixed s and t at far end of span,
			// calculate s and t steps across span by shifting
				sdivz += sdivz8stepu;
				tdivz += tdivz8stepu;
				zi += zi8stepu;
				z = (float)0x10000 / zi;	// prescale to 16.16 fixed-point

				snext = (int)(sdivz * z) + sadjust;
				if (snext > bbextents)
					snext = bbextents;
				else if (snext < 8)
					snext = 8;	// prevent round-off error on <0 steps from
								//  from causing overstepping & running off the
								//  edge of the texture

				tnext = (int)(tdivz * z) + tadjust;
				if (tnext > bbextentt)
					tnext = bbextentt;
				else if (tnext < 8)
					tnext = 8;	// guard against round-off error on <0 steps

				sstep = (snext - s) >> 3;
				tstep = (tnext - t) >> 3;
			}
			else
			{
			// calculate s/z, t/z, zi->fixed s and t at last pixel in span (so
			// can't step off polygon), clamp, calculate s and t steps across
			// span by division, biasing steps low so we don't run off the
			// texture
				spancountminus1 = (float)(spancount - 1);
				sdivz += d_sdivzstepu * spancountminus1;
				tdivz += d_tdivzstepu * spancountminus1;
				zi += d_zistepu * spancountminus1;
				z = (float)0x10000 / zi;	// prescale to 16.16 fixed-point
				snext = (int)(sdivz * z) + sadjust;
				if (snext > bbextents)
					snext = bbextents;
				else if (snext < 8)
					snext = 8;	// prevent round-off error on <0 steps from
								//  from causing overstepping & running off the
								//  edge of the texture

				tnext = (int)(tdivz * z) + tadjust;
				if (tnext > bbextentt)
					tnext = bbextentt;
				else if (tnext < 8)
					tnext = 8;	// guard against round-off error on <0 steps

				if (spancount > 1)
				{
					sstep = (snext - s) / (spancount - 1);
					tstep = (tnext - t) / (spancount - 1);
				}
			}

			do
			{
				*pdest++ = pbase[(s >> 16) + (t >> 16) * cachewidth];
				s += sstep;
				t += tstep;
			} while (--spancount > 0);

			s = snext;
			t = tnext;

		} while (count > 0);

	} while ((pspan = pspan->pnext) != NULL);
}

#endif


#if	!id386

/*
=============
D_DrawZSpans
=============
*/
void D_DrawZSpans( espan_t* pspan )
{
	int				count, doublecount, izistep;
	int				izi;
	short* pdest;
	unsigned		ltemp;
	double			zi;
	float			du, dv;

// FIXME: check for clamping/range problems
// we count on FP exceptions being turned off to avoid range problems
	izistep = (int)(d_zistepu * 0x8000 * 0x10000);

	do
	{
        pdest = &zspantable[pspan->v][pspan->u];

		count = pspan->count;

	// calculate the initial 1/z
		du = (float)pspan->u;
		dv = (float)pspan->v;

        zi = d_ziorigin + dv * d_zistepv + du * d_zistepu;
	// we count on FP exceptions being turned off to avoid range problems
		izi = (int)(zi * 0x8000 * 0x10000);

		if ((long)pdest & 0x02)
		{
			*pdest++ = (short)(izi >> 16);
			izi += izistep;
			count--;
		}

		if ((doublecount = count >> 1) > 0)
		{
			do
			{
				ltemp = izi >> 16;
				izi += izistep;
				ltemp |= izi & 0xFFFF0000;
				izi += izistep;
				*(int*)pdest = ltemp;
				pdest += 2;
			} while (--doublecount > 0);
		}

		if (count & 1)
			*pdest = (short)(izi >> 16);

	} while ((pspan = pspan->pnext) != NULL);
}

#endif