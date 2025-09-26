// d_sprite.c: software top-level rasterization driver module for drawing
// sprites

#include "quakedef.h"
#include "d_local.h"

static int		sprite_height;
static int		minindex, maxindex;
static sspan_t* sprite_spans;

void (*spritedraw)(sspan_t* pspan) = D_SpriteDrawSpans;

/*
=====================
D_SpriteDrawSpans
=====================
*/
void D_SpriteDrawSpans( sspan_t* pspan )
{
	int			count, spancount;
	int			izi;
	byte* pbase;
	unsigned short* pdest;
	fixed16_t	s, t, snext, tnext, sstep, tstep;
	float		sdivz, tdivz, z, du, dv, spancountminus1;
	byte		btemp;
	short* pz;

	set_fpu_cw();

	sstep = 0;	// keep compiler happy
	tstep = 0;	// ditto

	pbase = cacheblock;

	z = (float)0x10000 / d_ziorigin;
	d_zidist = (int)(d_ziorigin * (float)0x8000);

	izi = d_zidist;

	do
	{
		pdest = (unsigned short*)(d_viewbuffer + ((screenwidth * pspan->v) + pspan->u * 2));
		pz = &zspantable[pspan->v][pspan->u];

		count = pspan->count;

		if (count <= 0)
			goto NextSpan;

	// calculate the initial s/z, t/z, 1/z, s, and t and clamp
		du = (float)pspan->u;
		dv = (float)pspan->v;

		sdivz = d_sdivzorigin + dv * d_sdivzstepv + du * d_sdivzstepu;
		tdivz = d_tdivzorigin + dv * d_tdivzstepv + du * d_tdivzstepu;

		d_zidist = (int)(sdivz * z);
		s = d_zidist + sadjust;
		if (s > bbextents)
			s = bbextents;
		else if (s < 0)
			s = 0;

		d_zidist = (int)(tdivz * z);
		t = d_zidist + tadjust;
		if (t > bbextentt)
			t = bbextentt;
		else if (t < 0)
			t = 0;

		spancount = count;

	// calculate s/z, t/z, zi->fixed s and t at last pixel in span (so
	// can't step off polygon), clamp, calculate s and t steps across
	// span by division, biasing steps low so we don't run off the
	// texture
		spancountminus1 = (float)(spancount - 1);
		sdivz += d_sdivzstepu * spancountminus1;
		tdivz += d_tdivzstepu * spancountminus1;
		d_zidist = (int)(sdivz * z);

		snext = d_zidist + sadjust;
		if (snext > bbextents)
			snext = bbextents;
		else if (snext < 8)
			snext = 8;	// prevent round-off error on <0 steps from
								//  from causing overstepping & running off the
								//  edge of the texture

		d_zidist = (int)(tdivz * z);

		tnext = d_zidist + tadjust;
		if (tnext > bbextentt)
			tnext = bbextentt;
		else if (tnext < 8)
			tnext = 8;	// guard against round-off error on <0 steps

		if (spancount > 1)
		{
			sstep = (snext - s) / (spancount - 1);
			tstep = (tnext - t) / (spancount - 1);
		}

		do
		{
			btemp = pbase[(s >> 16) + (t >> 16) * cachewidth];
			if (btemp != 255)
			{
				if (*pz <= izi)
				{
					*pz = izi;
					*pdest = r_palette[btemp];
				}
			}

			pdest++;
			pz++;
			s += sstep;
			t += tstep;
		} while (--spancount > 0);

	NextSpan:
		pspan++;

	} while (pspan->count != DS_SPAN_LIST_END);

	restore_fpu_cw();
}

/*
=====================
D_SpriteDrawSpansTrans
=====================
*/
void D_SpriteDrawSpansTrans( sspan_t* pspan )
{
	int			count, spancount;
	int			izi;
	byte* pbase;
	unsigned short* pdest;
	fixed16_t	s, t, snext, tnext, sstep, tstep;
	float		sdivz, tdivz, z, du, dv, spancountminus1;
	byte		btemp;
	short* pz;

	set_fpu_cw();

	sstep = 0;	// keep compiler happy
	tstep = 0;	// ditto

	pbase = cacheblock;

	z = (float)0x10000 / d_ziorigin;
	d_zidist = (int)(d_ziorigin * (float)0x8000);

	izi = d_zidist;

	do
	{
		pdest = (unsigned short*)(d_viewbuffer + ((screenwidth * pspan->v) + pspan->u * 2));
		pz = &zspantable[pspan->v][pspan->u];

		count = pspan->count;

		if (count <= 0)
			goto NextSpan;

	// calculate the initial s/z, t/z, 1/z, s, and t and clamp
		du = (float)pspan->u;
		dv = (float)pspan->v;

		sdivz = d_sdivzorigin + dv * d_sdivzstepv + du * d_sdivzstepu;
		tdivz = d_tdivzorigin + dv * d_tdivzstepv + du * d_tdivzstepu;

		d_zidist = (int)(sdivz * z);
		s = d_zidist + sadjust;
		if (s > bbextents)
			s = bbextents;
		else if (s < 0)
			s = 0;

		d_zidist = (int)(tdivz * z);
		t = d_zidist + tadjust;
		if (t > bbextentt)
			t = bbextentt;
		else if (t < 0)
			t = 0;

		spancount = count;

	// calculate s/z, t/z, zi->fixed s and t at last pixel in span (so
	// can't step off polygon), clamp, calculate s and t steps across
	// span by division, biasing steps low so we don't run off the
	// texture
		spancountminus1 = (float)(spancount - 1);
		sdivz += d_sdivzstepu * spancountminus1;
		tdivz += d_tdivzstepu * spancountminus1;
		d_zidist = (int)(sdivz * z);

		snext = d_zidist + sadjust;
		if (snext > bbextents)
			snext = bbextents;
		else if (snext < 8)
			snext = 8;	// prevent round-off error on <0 steps from
								//  from causing overstepping & running off the
								//  edge of the texture

		d_zidist = (int)(tdivz * z);

		tnext = d_zidist + tadjust;
		if (tnext > bbextentt)
			tnext = bbextentt;
		else if (tnext < 8)
			tnext = 8;	// guard against round-off error on <0 steps

		if (spancount > 1)
		{
			sstep = (snext - s) / (spancount - 1);
			tstep = (tnext - t) / (spancount - 1);
		}

		do
		{
			btemp = pbase[(s >> 16) + (t >> 16) * cachewidth];
			if (btemp != 255)
			{
				if (*pz <= izi)
				{
					unsigned short r, g, b, oldcolor, newcolor;

					oldcolor = *pdest;
					newcolor = r_palette[btemp];

					if (is15bit)
					{
						r = ScaleToColor(oldcolor, newcolor, 0x7C00, r_blend) & 0x7C00;
						g = ScaleToColor(oldcolor, newcolor, 0x03E0, r_blend) & 0x03E0;
						b = ScaleToColor(oldcolor, newcolor, 0x001F, r_blend) & 0x001F;

						*pdest = r | g | b;
					}
					else
					{
						r = ScaleToColor(oldcolor, newcolor, 0xF800, r_blend) & 0xF800;
						g = ScaleToColor(oldcolor, newcolor, 0x07E0, r_blend) & 0x07E0;
						b = ScaleToColor(oldcolor, newcolor, 0x001F, r_blend) & 0x001F;

						*pdest = r | g | b;
					}
				}
			}

			pdest++;
			pz++;
			s += sstep;
			t += tstep;
		} while (--spancount > 0);

	NextSpan:
		pspan++;

	} while (pspan->count != DS_SPAN_LIST_END);

	restore_fpu_cw();
}

/*
=====================
D_SpriteDrawSpansAdd
=====================
*/
void D_SpriteDrawSpansAdd( sspan_t* pspan )
{
	int			count, spancount;
	int			izi;
	byte* pbase;
	unsigned short* pdest;
	fixed16_t	s, t, snext, tnext, sstep, tstep;
	float		sdivz, tdivz, z, du, dv, spancountminus1;
	byte        btemp;
	short* pz;

	set_fpu_cw();

	sstep = 0;	// keep compiler happy
	tstep = 0;	// ditto

	pbase = cacheblock;

	z = (float)0x10000 / d_ziorigin;
	d_zidist = (int)(d_ziorigin * (float)0x8000);

	izi = d_zidist;

	do
	{
		pdest = (unsigned short*)(d_viewbuffer + ((screenwidth * pspan->v) + pspan->u * 2));
		pz = &zspantable[pspan->v][pspan->u];

		count = pspan->count;

		if (count <= 0)
			goto NextSpan;

	// calculate the initial s/z, t/z, 1/z, s, and t and clamp
		du = (float)pspan->u;
		dv = (float)pspan->v;

		sdivz = d_sdivzorigin + dv * d_sdivzstepv + du * d_sdivzstepu;
		tdivz = d_tdivzorigin + dv * d_tdivzstepv + du * d_tdivzstepu;

		d_zidist = (int)(sdivz * z);
		s = d_zidist + sadjust;
		if (s > bbextents)
			s = bbextents;
		else if (s < 0)
			s = 0;

		d_zidist = (int)(tdivz * z);
		t = d_zidist + tadjust;
		if (t > bbextentt)
			t = bbextentt;
		else if (t < 0)
			t = 0;

		spancount = count;

	// calculate s/z, t/z, zi->fixed s and t at last pixel in span (so
	// can't step off polygon), clamp, calculate s and t steps across
	// span by division, biasing steps low so we don't run off the
	// texture
		spancountminus1 = (float)(spancount - 1);
		sdivz += d_sdivzstepu * spancountminus1;
		tdivz += d_tdivzstepu * spancountminus1;
		d_zidist = (int)(sdivz * z);

		snext = d_zidist + sadjust;
		if (snext > bbextents)
			snext = bbextents;
		else if (snext < 8)
			snext = 8;	// prevent round-off error on <0 steps from
								//  from causing overstepping & running off the
								//  edge of the texture

		d_zidist = (int)(tdivz * z);

		tnext = d_zidist + tadjust;
		if (tnext > bbextentt)
			tnext = bbextentt;
		else if (tnext < 8)
			tnext = 8;	// guard against round-off error on <0 steps

		if (spancount > 1)
		{
			sstep = (snext - s) / (spancount - 1);
			tstep = (tnext - t) / (spancount - 1);
		}

		if (is15bit)
		{
			do
			{
				unsigned int newcolor;

				btemp = pbase[(s >> 16) + (t >> 16) * cachewidth];

				newcolor = r_palette[btemp];
				if (newcolor != 0)
				{
					if (*pz <= izi)
					{
						unsigned int oldcolor = *pdest;
						unsigned int carrybits;

						const unsigned int highbits = (0x80000000 | 0x00200000 | 0x00000800);
						const unsigned int lowbits = (0x80000000 | 0x00200000 | 0x00000400);
						const unsigned int redblue = (0x7C00 | 0x001F);
						const unsigned int green = (0x03E0);

						oldcolor = ((oldcolor & redblue) << 16) | (oldcolor & green);
						oldcolor = ((newcolor & redblue) << 16) | (newcolor & green) + oldcolor;

						carrybits = (oldcolor & lowbits);
						if (carrybits)
						{
							// adjust the color
							oldcolor |= lowbits - (carrybits >> 5);
						}

						*pdest = ((oldcolor >> 16) & redblue) | (oldcolor & green);
					}
				}

				pdest++;
				pz++;
				s += sstep;
				t += tstep;
			} while (--spancount > 0);
		}
		else
		{
			do
			{
				unsigned int newcolor;

				btemp = pbase[(s >> 16) + (t >> 16) * cachewidth];

				newcolor = r_palette[btemp];
				if (newcolor != 0)
				{
					if (*pz <= izi)
					{
						unsigned int oldcolor = *pdest, prevcolor;
						unsigned int carrybits;

						const unsigned int highbits = (0x200000 | 0x000800);
						const unsigned int lowbits = (0x100000 | 0x000400);
						const unsigned int redblue = (0xF800 | 0x001F);
						const unsigned int green = (0x07E0);

						prevcolor = ((oldcolor & redblue) << 16) | (oldcolor & green);
						oldcolor = (((newcolor & redblue) << 16) | (newcolor & green)) + prevcolor;

						carrybits = (oldcolor & highbits);
						if (prevcolor > oldcolor)
							carrybits |= 1;

						if (carrybits)
						{
							// adjust the color
							carrybits = _rotr(carrybits, 1);
							oldcolor |= ((carrybits | lowbits) - (carrybits >> 5)) << 1;
						}

						*pdest = ((oldcolor >> 16) & redblue) | (oldcolor & green);
					}
				}

				pdest++;
				pz++;
				s += sstep;
				t += tstep;
			} while (--spancount > 0);
		}

	NextSpan:
		pspan++;

	} while (pspan->count != DS_SPAN_LIST_END);

	restore_fpu_cw();
}

/*
=====================
D_SpriteDrawSpansGlow
=====================
*/
void D_SpriteDrawSpansGlow( sspan_t* pspan )
{
	int			count, spancount;
	int			izi;
	byte* pbase;
	unsigned short* pdest;
	fixed16_t	s, t, snext, tnext, sstep, tstep;
	float		sdivz, tdivz, z, du, dv, spancountminus1;
	byte        btemp;

	set_fpu_cw();

	sstep = 0;	// keep compiler happy
	tstep = 0;	// ditto

	pbase = cacheblock;

	z = (float)0x10000 / d_ziorigin;
	d_zidist = (int)(d_ziorigin * (float)0x8000);

	izi = d_zidist;

	do
	{
		pdest = (unsigned short*)(d_viewbuffer + ((screenwidth * pspan->v) + pspan->u * 2));

		count = pspan->count;

		if (count <= 0)
			goto NextSpan;

	// calculate the initial s/z, t/z, 1/z, s, and t and clamp
		du = (float)pspan->u;
		dv = (float)pspan->v;

		sdivz = d_sdivzorigin + dv * d_sdivzstepv + du * d_sdivzstepu;
		tdivz = d_tdivzorigin + dv * d_tdivzstepv + du * d_tdivzstepu;

		d_zidist = (int)(sdivz * z);
		s = d_zidist + sadjust;
		if (s > bbextents)
			s = bbextents;
		else if (s < 0)
			s = 0;

		d_zidist = (int)(tdivz * z);
		t = d_zidist + tadjust;
		if (t > bbextentt)
			t = bbextentt;
		else if (t < 0)
			t = 0;

		spancount = count;

	// calculate s/z, t/z, zi->fixed s and t at last pixel in span (so
	// can't step off polygon), clamp, calculate s and t steps across
	// span by division, biasing steps low so we don't run off the
	// texture
		spancountminus1 = (float)(spancount - 1);
		sdivz += d_sdivzstepu * spancountminus1;
		tdivz += d_tdivzstepu * spancountminus1;
		d_zidist = (int)(sdivz * z);

		snext = d_zidist + sadjust;
		if (snext > bbextents)
			snext = bbextents;
		else if (snext < 8)
			snext = 8;	// prevent round-off error on <0 steps from
								//  from causing overstepping & running off the
								//  edge of the texture

		d_zidist = (int)(tdivz * z);

		tnext = d_zidist + tadjust;
		if (tnext > bbextentt)
			tnext = bbextentt;
		else if (tnext < 8)
			tnext = 8;	// guard against round-off error on <0 steps

		if (spancount > 1)
		{
			sstep = (snext - s) / (spancount - 1);
			tstep = (tnext - t) / (spancount - 1);
		}

		if (is15bit)
		{
			do
			{
				unsigned int newcolor;

				btemp = pbase[(s >> 16) + (t >> 16) * cachewidth];

				newcolor = r_palette[btemp];
				if (newcolor != 0)
				{
					unsigned int oldcolor = *pdest;
					unsigned int carrybits;

					const unsigned int highbits = (0x80000000 | 0x00200000 | 0x00000800);
					const unsigned int lowbits = (0x80000000 | 0x00200000 | 0x00000400);
					const unsigned int redblue = (0x7C00 | 0x001F);
					const unsigned int green = (0x03E0);

					oldcolor = ((oldcolor & redblue) << 16) | (oldcolor & green);
					oldcolor = ((newcolor & redblue) << 16) | (newcolor & green) + oldcolor;

					carrybits = (oldcolor & lowbits);
					if (carrybits)
					{
						// adjust the color
						oldcolor |= lowbits - (carrybits >> 5);
					}

					*pdest = ((oldcolor >> 16) & redblue) | (oldcolor & green);
				}

				pdest++;
				s += sstep;
				t += tstep;
			} while (--spancount > 0);
		}
		else
		{
			do
			{
				unsigned int newcolor;

				btemp = pbase[(s >> 16) + (t >> 16) * cachewidth];

				newcolor = r_palette[btemp];
				if (newcolor != 0)
				{
					unsigned int oldcolor = *pdest, prevcolor;
					unsigned int carrybits;

					const unsigned int highbits = (0x00000000 | 0x200000 | 0x000800);
					const unsigned int lowbits = (0x00000000 | 0x100000 | 0x000400);
					const unsigned int redblue = (0xF800 | 0x001F);
					const unsigned int green = (0x07E0);

					prevcolor = ((oldcolor & redblue) << 16) | (oldcolor & green);
					oldcolor = (((newcolor & redblue) << 16) | (newcolor & green)) + prevcolor;

					carrybits = (oldcolor & highbits);
					if (prevcolor > oldcolor)
						carrybits |= 1;

					if (carrybits)
					{
						// adjust the color
						carrybits = _rotr(carrybits, 1);
						oldcolor |= ((carrybits | lowbits) - (carrybits >> 5)) << 1;
					}

					*pdest = ((oldcolor >> 16) & redblue) | (oldcolor & green);
				}

				pdest++;
				s += sstep;
				t += tstep;
			} while (--spancount > 0);
		}

	NextSpan:
		pspan++;

	} while (pspan->count != DS_SPAN_LIST_END);

	restore_fpu_cw();
}

/*
=====================
D_SpriteDrawSpansAlpha
=====================
*/
void D_SpriteDrawSpansAlpha( sspan_t* pspan )
{
	int			count, spancount;
	int			izi;
	byte* pbase;
	unsigned short* pdest;
	fixed16_t	s, t, snext, tnext, sstep, tstep;
	float		sdivz, tdivz, z, du, dv, spancountminus1;
	byte        btemp;
	short* pz;
	unsigned int spritecolor = r_palette[255];

	const unsigned int redblue = is15bit ? (0x7C00 | 0x001F) : (0xF800 | 0x001F);
	const unsigned int green = is15bit ? (0x03E0) : (0x07E0);
	unsigned int newcolor = ((spritecolor & redblue) << 16) | (spritecolor & green);

	set_fpu_cw();

	sstep = 0;	// keep compiler happy
	tstep = 0;	// ditto

	pbase = cacheblock;

	z = (float)0x10000 / d_ziorigin;
	d_zidist = (int)(d_ziorigin * (float)0x8000);

	izi = d_zidist;

	do
	{
		pdest = (unsigned short*)(d_viewbuffer + ((screenwidth * pspan->v) + pspan->u * 2));
		pz = &zspantable[pspan->v][pspan->u];

		count = pspan->count;

		if (count <= 0)
			goto NextSpan;

	// calculate the initial s/z, t/z, 1/z, s, and t and clamp
		du = (float)pspan->u;
		dv = (float)pspan->v;

		sdivz = d_sdivzorigin + dv * d_sdivzstepv + du * d_sdivzstepu;
		tdivz = d_tdivzorigin + dv * d_tdivzstepv + du * d_tdivzstepu;

		d_zidist = (int)(sdivz * z);
		s = d_zidist + sadjust;
		if (s > bbextents)
			s = bbextents;
		else if (s < 0)
			s = 0;

		d_zidist = (int)(tdivz * z);
		t = d_zidist + tadjust;
		if (t > bbextentt)
			t = bbextentt;
		else if (t < 0)
			t = 0;

		spancount = count;

	// calculate s/z, t/z, zi->fixed s and t at last pixel in span (so
	// can't step off polygon), clamp, calculate s and t steps across
	// span by division, biasing steps low so we don't run off the
	// texture
		spancountminus1 = (float)(spancount - 1);
		sdivz += d_sdivzstepu * spancountminus1;
		tdivz += d_tdivzstepu * spancountminus1;
		d_zidist = (int)(sdivz * z);

		snext = d_zidist + sadjust;
		if (snext > bbextents)
			snext = bbextents;
		else if (snext < 8)
			snext = 8;	// prevent round-off error on <0 steps from
								//  from causing overstepping & running off the
								//  edge of the texture

		d_zidist = (int)(tdivz * z);

		tnext = d_zidist + tadjust;
		if (tnext > bbextentt)
			tnext = bbextentt;
		else if (tnext < 8)
			tnext = 8;	// guard against round-off error on <0 steps

		if (spancount > 1)
		{
			sstep = (snext - s) / (spancount - 1);
			tstep = (tnext - t) / (spancount - 1);
		}

		if (is15bit)
		{
			do
			{
				btemp = pbase[(s >> 16) + (t >> 16) * cachewidth];
				if (btemp != 0)
				{
					if (*pz <= izi)
					{
						unsigned int oldcolor = *pdest;
						unsigned int deltacolor, signbits, blendbits, mask;

						const unsigned int highbits = (0x80000000 | 0x00200000 | 0x00000400);
						const unsigned int lowbits = (0x40000000 | 0x00100000 | 0x00000200);

						oldcolor = ((oldcolor & redblue) << 16) | (oldcolor & green);
						deltacolor = ((newcolor | highbits) - oldcolor) >> 1;

						mask = (1 << 8);
						blendbits = btemp & 240;
						signbits = (~deltacolor & lowbits);

						do
						{
							mask >>= 1;
							oldcolor &= ~highbits;
							deltacolor = signbits | deltacolor & ~lowbits;

							if (blendbits & mask)
							{
								blendbits ^= mask;
								oldcolor += deltacolor;
							}
							deltacolor >>= 1;
						} while (blendbits);

						*pdest = ((oldcolor >> 16) & redblue) | (oldcolor & green);
					}
				}

				pdest++;
				pz++;
				s += sstep;
				t += tstep;
			} while (--spancount > 0);
		}
		else
		{
			do
			{
				btemp = pbase[(s >> 16) + (t >> 16) * cachewidth];
				if (btemp != 0)
				{
					if (*pz <= izi)
					{
						unsigned int oldcolor = *pdest;
						unsigned int deltacolor, signbits, blendbits, mask;

						const unsigned int highbits = (0x00000000 | 0x00200000 | 0x00000800);
						const unsigned int lowbits = (0x80000000 | 0x00100000 | 0x00000400);

						oldcolor = ((oldcolor & redblue) << 16) | (oldcolor & green);
						deltacolor = ((newcolor | highbits) - oldcolor) >> 1;

						if ((newcolor & 0xF8000000) >= (oldcolor & 0xF8000000))
							deltacolor |= 0x80000000;

						mask = (1 << 8);
						blendbits = btemp & 240;
						signbits = (~deltacolor & lowbits);

						do
						{
							mask >>= 1;
							oldcolor &= ~highbits;
							deltacolor = signbits | deltacolor & ~lowbits;

							if (blendbits & mask)
							{
								blendbits ^= mask;
								oldcolor += deltacolor;
							}
							deltacolor >>= 1;
						} while (blendbits);

						*pdest = ((oldcolor >> 16) & redblue) | (oldcolor & green);
					}
				}

				pdest++;
				pz++;
				s += sstep;
				t += tstep;
			} while (--spancount > 0);
		}

	NextSpan:
		pspan++;

	} while (pspan->count != DS_SPAN_LIST_END);

	restore_fpu_cw();
}


/*
=====================
D_SpriteScanLeftEdge
=====================
*/
void D_SpriteScanLeftEdge( void )
{
	int			i, v, itop, ibottom, lmaxindex;
	emitpoint_t* pvert, * pnext;
	sspan_t* pspan;
	float		du, dv, vtop, vbottom, slope;
	fixed16_t	u, u_step;

	set_fpu_cw();

	pspan = sprite_spans;
	i = minindex;
	if (i == 0)
		i = r_spritedesc.nump;

	lmaxindex = maxindex;
	if (lmaxindex == 0)
		lmaxindex = r_spritedesc.nump;

	vtop = ceil(r_spritedesc.pverts[i].v);

	do
	{
		pvert = &r_spritedesc.pverts[i];
		pnext = pvert - 1;

		vbottom = ceil(pnext->v);

		if (vtop < vbottom)
		{
			du = pnext->u - pvert->u;
			dv = pnext->v - pvert->v;
			slope = du / dv;
			u_step = (int)(slope * 0x10000);
			d_zidist = u_step;
		// adjust u to ceil the integer portion
			u = (int)((pvert->u + (slope * (vtop - pvert->v))) * 0x10000) +
				(0x10000 - 1);
			itop = (int)vtop;
			ibottom = (int)vbottom;
			d_zidist = ibottom;

			for (v = itop; v < ibottom; v++)
			{
				pspan->u = u >> 16;
				pspan->v = v;
				u += u_step;
				pspan++;
			}
		}

		vtop = vbottom;

		i--;
		if (i == 0)
			i = r_spritedesc.nump;

	} while (i != lmaxindex);

	restore_fpu_cw();
}


/*
=====================
D_SpriteScanRightEdge
=====================
*/
void D_SpriteScanRightEdge( void )
{
	int			i, v, itop, ibottom;
	emitpoint_t* pvert, * pnext;
	sspan_t* pspan;
	float		du, dv, vtop, vbottom, slope, uvert, unext, vvert, vnext;
	fixed16_t	u, u_step;

	set_fpu_cw();

	pspan = sprite_spans;
	i = minindex;

	vvert = r_spritedesc.pverts[i].v;
	if (vvert < r_refdef.fvrecty_adj)
		vvert = r_refdef.fvrecty_adj;
	if (vvert > r_refdef.fvrectbottom_adj)
		vvert = r_refdef.fvrectbottom_adj;

	vtop = ceil(vvert);

	do
	{
		pvert = &r_spritedesc.pverts[i];
		pnext = pvert + 1;

		vnext = pnext->v;
		if (vnext < r_refdef.fvrecty_adj)
			vnext = r_refdef.fvrecty_adj;
		if (vnext > r_refdef.fvrectbottom_adj)
			vnext = r_refdef.fvrectbottom_adj;

		vbottom = ceil(vnext);

		if (vtop < vbottom)
		{
			uvert = pvert->u;
			if (uvert < r_refdef.fvrectx_adj)
				uvert = r_refdef.fvrectx_adj;
			if (uvert > r_refdef.fvrectright_adj)
				uvert = r_refdef.fvrectright_adj;

			unext = pnext->u;
			if (unext < r_refdef.fvrectx_adj)
				unext = r_refdef.fvrectx_adj;
			if (unext > r_refdef.fvrectright_adj)
				unext = r_refdef.fvrectright_adj;

			du = unext - uvert;
			dv = vnext - vvert;
			slope = du / dv;
			u_step = (int)(slope * 0x10000);
		// adjust u to ceil the integer portion
			u = (int)((uvert + (slope * (vtop - vvert))) * 0x10000) +
				(0x10000 - 1);
			itop = (int)vtop;
			ibottom = (int)vbottom;
			d_zidist = ibottom;

			for (v = itop; v < ibottom; v++)
			{
				pspan->count = (u >> 16) - pspan->u;
				u += u_step;
				pspan++;
			}
		}

		vtop = vbottom;
		vvert = vnext;

		i++;
		if (i == r_spritedesc.nump)
			i = 0;

	} while (i != maxindex);

	pspan->count = DS_SPAN_LIST_END;	// mark the end of the span list 

	restore_fpu_cw();
}


/*
=====================
D_SpriteCalculateGradients
=====================
*/
void D_SpriteCalculateGradients( void )
{
	vec3_t		p_saxis, p_taxis, p_temp1;
    float		distinv, scale;

	TransformVector(r_spritedesc.vright, p_saxis);
	TransformVector(r_spritedesc.vup, p_taxis);

	scale = 1.0 / r_spritedesc.scale;
	VectorScale(p_saxis, scale, p_saxis);
	VectorScale(p_taxis, -scale, p_taxis);

	d_zistepu = 0.0;
	d_zistepv = 0.0;

    distinv = 1.0 / (-DotProduct(modelorg, r_spritedesc.vpn));

	d_ziorigin = distinv;

	d_sdivzstepu = p_saxis[0] * xscaleinv;
	d_tdivzstepu = p_taxis[0] * xscaleinv;

	d_sdivzstepv = -(p_saxis[1] * yscaleinv);
	d_tdivzstepv = -(p_taxis[1] * yscaleinv);

	d_sdivzorigin = p_saxis[2] - xcenter * d_sdivzstepu -
		ycenter * d_sdivzstepv;
	d_tdivzorigin = p_taxis[2] - xcenter * d_tdivzstepu -
		ycenter * d_tdivzstepv;

	TransformVector(modelorg, p_temp1);

	sadjust = ((fixed16_t)(DotProduct(p_temp1, p_saxis) * 0x10000 + 0.5)) -
		(-(cachewidth >> 1) << 16);
	tadjust = ((fixed16_t)(DotProduct(p_temp1, p_taxis) * 0x10000 + 0.5)) -
		(-(sprite_height >> 1) << 16);

// -1 (-epsilon) so we never wander off the edge of the texture
	bbextents = (cachewidth << 16) - 1;
	bbextentt = (sprite_height << 16) - 1;
}


/*
=====================
D_DrawSprite
=====================
*/
void D_DrawSprite( void )
{
	int			i, nump;
	float		ymin, ymax;
	emitpoint_t* pverts;
	sspan_t		spans[MAXHEIGHT + 1];

	sprite_spans = spans;

// find the top and bottom vertices, and make sure there's at least one scan to
// draw
	ymin = 999999.9;
	ymax = -999999.9;
	pverts = r_spritedesc.pverts;

	for (i = 0; i < r_spritedesc.nump; i++)
	{
		if (pverts->v < ymin)
		{
			ymin = pverts->v;
			minindex = i;
		}

		if (pverts->v > ymax)
		{
			ymax = pverts->v;
			maxindex = i;
		}

		pverts++;
	}

	ymin = ceil(ymin);
	ymax = ceil(ymax);

	if (ymin >= ymax)
		return;		// doesn't cross any scans at all

	cachewidth = r_spritedesc.pspriteframe->width;
	sprite_height = r_spritedesc.pspriteframe->height;
	cacheblock = r_spritedesc.pspriteframe->pixels;

// copy the first vertex to the last vertex, so we don't have to deal with
// wrapping
	nump = r_spritedesc.nump;
	pverts = r_spritedesc.pverts;
	pverts[nump] = pverts[0];

	D_SpriteCalculateGradients();
	D_SpriteScanLeftEdge();
	D_SpriteScanRightEdge();

	(*spritedraw)(sprite_spans);
}