// d_polyset.c: routines for drawing sets of polygons sharing the same
// texture (used for Alias models)

#include "quakedef.h"
#include "d_local.h"

// TODO: put in span spilling to shrink list size
// !!! if this is changed, it must be changed in d_polysa.s too !!!
#define DPS_MAXSPANS			MAXHEIGHT + 1	
									// 1 extra for spanpackage that marks end

int r_p0[6], r_p1[6], r_p2[6];

byte* d_pcolormap;

int			d_aflatcolor;
int			d_xdenom;

edgetable* pedgetable;

edgetable	edgetables[12] = {
	{0, 1, r_p0, r_p2, NULL, 2, r_p0, r_p1, r_p2 },
	{0, 2, r_p1, r_p0, r_p2, 1, r_p1, r_p2, NULL},
	{1, 1, r_p0, r_p2, NULL, 1, r_p1, r_p2, NULL},
	{0, 1, r_p1, r_p0, NULL, 2, r_p1, r_p2, r_p0 },
	{0, 2, r_p0, r_p2, r_p1, 1, r_p0, r_p1, NULL},
	{0, 1, r_p2, r_p1, NULL, 1, r_p2, r_p0, NULL},
	{0, 1, r_p2, r_p1, NULL, 2, r_p2, r_p0, r_p1 },
	{0, 2, r_p2, r_p1, r_p0, 1, r_p2, r_p0, NULL},
	{0, 1, r_p1, r_p0, NULL, 1, r_p1, r_p2, NULL},
	{1, 1, r_p2, r_p1, NULL, 1, r_p0, r_p1, NULL},
	{1, 1, r_p1, r_p0, NULL, 1, r_p2, r_p0, NULL},
	{0, 1, r_p0, r_p2, NULL, 1, r_p0, r_p1, NULL},
};

// FIXME: some of these can become statics
int				a_sstepxfrac, a_tstepxfrac, r_lstepx, a_ststepxwhole;
int				r_sstepx, r_tstepx, r_lstepy, r_sstepy, r_tstepy;
int				r_zistepx, r_zistepy;
int				d_aspancount, d_countextrastep;

spanpackage_t* a_spans;
spanpackage_t* d_pedgespanpackage;
static int				ystart;
byte* d_pdest, * d_ptex;
short* d_pz;
int						d_sfrac, d_tfrac, d_light, d_zi;
int						d_ptexextrastep, d_sfracextrastep;
int						d_tfracextrastep, d_lightextrastep, d_pdestextrastep;
int						d_lightbasestep, d_pdestbasestep, d_ptexbasestep;
int						d_sfracbasestep, d_tfracbasestep;
int						d_ziextrastep, d_zibasestep;
int						d_pzextrastep, d_pzbasestep;

typedef struct {
	int		quotient;
	int		remainder;
} adivtab_t;

static adivtab_t	adivtab[32 * 32] = {
#include "adivtab.h"
};

byte* skintable[MAX_LBM_HEIGHT];
int		skinwidth;
byte* skinstart;

#if	!id386

/*
================
D_PolysetDraw
================
*/
void D_PolysetDraw( void )
{
	spanpackage_t	spans[DPS_MAXSPANS + 1 +
		((CACHE_SIZE - 1) / sizeof(spanpackage_t)) + 1];
	// one extra because of cache line pretouching

	mtriangle_t* ptri;
	finalvert_t* pfv, * index0, * index1, * index2;
	int				i;
	int				lnumtriangles;

	a_spans = (spanpackage_t*)
		(((long)&spans[0] + CACHE_SIZE - 1) & ~(CACHE_SIZE - 1));

	if (r_affinetridesc.drawtype)
	{
		D_DrawSubdiv();
	}
	else
	{
		D_DrawNonSubdiv();
	}
}


/*
================
D_PolysetDrawFinalVerts
================
*/
void D_PolysetDrawFinalVerts( finalvert_t* fv, int numverts )
{
	int		i, z;
	short* zbuf;

	for (i = 0; i < numverts; i++, fv++)
	{
	// valid triangle coordinates for filling can include the bottom and
	// right clip edges, due to the fill rule; these shouldn't be drawn
		if ((fv->v[0] < r_refdef.vrectright) &&
			(fv->v[1] < r_refdef.vrectbottom))
		{
			z = fv->v[5] >> 16;
			zbuf = zspantable[fv->v[1]] + fv->v[0];
			if (z >= *zbuf)
			{
				int		pix;

				*zbuf = z;
				pix = skintable[fv->v[3] >> 16][fv->v[2] >> 16];

				int32 r, g, b;
				word* pColor;
				int llight;
				unsigned short* lpdest;
							
				lpdest = (unsigned short*)&d_viewbuffer[d_scantable[fv->v[1]] + fv->v[0] * 2];

				pColor = &r_palette[pix * 4];
				llight = fv->v[4];
				r = r_lut[pColor[0] + llight & 0xFF00];
				g = r_lut[pColor[1] + llight & 0xFF00];
				b = r_lut[pColor[2] + llight & 0xFF00];

				if (is15bit)
				{
					*lpdest = PACKEDRGB555(r, g, b);
				}
				else
				{
					*lpdest = PACKEDRGB565(r, g, b);
				}
			}
		}
	}
}

float acolormap[1024];

/*
================
D_DrawSubdiv
================
*/
void D_DrawSubdiv( void )
{
	mtriangle_t* ptri;
	finalvert_t* pfv, * index0, * index1, * index2;
	int				i;
	int				lnumtriangles;

	pfv = r_affinetridesc.pfinalverts;
	ptri = r_affinetridesc.ptriangles;
	lnumtriangles = r_affinetridesc.numtriangles;

	for (i = 0; i < lnumtriangles; i++)
	{
		index0 = pfv + ptri[i].vertindex[0];
		index1 = pfv + ptri[i].vertindex[1];
		index2 = pfv + ptri[i].vertindex[2];

		if (((index0->v[1] - index1->v[1]) *
			  (index0->v[0] - index2->v[0]) -
			  (index0->v[0] - index1->v[0]) *
			  (index0->v[1] - index2->v[1])) >= 0)
		{
			continue;
		}

		d_pcolormap = &((byte*)acolormap)[index0->v[4] & 0xFF00];

		if (ptri[i].facesfront)
		{
			D_PolysetRecursiveTriangle(index0->v, index1->v, index2->v);
		}
		else
		{
			int		s0, s1, s2;

			s0 = index0->v[2];
			s1 = index1->v[2];
			s2 = index2->v[2];

			if (index0->flags & ALIAS_ONSEAM)
				index0->v[2] += r_affinetridesc.seamfixupX16;
			if (index1->flags & ALIAS_ONSEAM)
				index1->v[2] += r_affinetridesc.seamfixupX16;
			if (index2->flags & ALIAS_ONSEAM)
				index2->v[2] += r_affinetridesc.seamfixupX16;

			D_PolysetRecursiveTriangle(index0->v, index1->v, index2->v);

			index0->v[2] = s0;
			index1->v[2] = s1;
			index2->v[2] = s2;
		}
	}
}


/*
================
D_DrawNonSubdiv
================
*/
void D_DrawNonSubdiv( void )
{
	mtriangle_t* ptri;
	finalvert_t* pfv, * index0, * index1, * index2;
	int				i;
	int				lnumtriangles;

	pfv = r_affinetridesc.pfinalverts;
	ptri = r_affinetridesc.ptriangles;
	lnumtriangles = r_affinetridesc.numtriangles;

	for (i = 0; i < lnumtriangles; i++, ptri++)
	{
		index0 = pfv + ptri->vertindex[0];
		index1 = pfv + ptri->vertindex[1];
		index2 = pfv + ptri->vertindex[2];

		d_xdenom = (index0->v[1] - index1->v[1]) *
			(index0->v[0] - index2->v[0]) -
			(index0->v[0] - index1->v[0]) * (index0->v[1] - index2->v[1]);

		if (d_xdenom >= 0)
		{
			continue;
		}

		r_p0[0] = index0->v[0];		// u
		r_p0[1] = index0->v[1];		// v
		r_p0[2] = index0->v[2];		// s
		r_p0[3] = index0->v[3];		// t
		r_p0[4] = index0->v[4];		// light
		r_p0[5] = index0->v[5];		// iz

		r_p1[0] = index1->v[0];
		r_p1[1] = index1->v[1];
		r_p1[2] = index1->v[2];
		r_p1[3] = index1->v[3];
		r_p1[4] = index1->v[4];
		r_p1[5] = index1->v[5];

		r_p2[0] = index2->v[0];
		r_p2[1] = index2->v[1];
		r_p2[2] = index2->v[2];
		r_p2[3] = index2->v[3];
		r_p2[4] = index2->v[4];
		r_p2[5] = index2->v[5];

		if (!ptri->facesfront)
		{
			if (index0->flags & ALIAS_ONSEAM)
				r_p0[2] += r_affinetridesc.seamfixupX16;
			if (index1->flags & ALIAS_ONSEAM)
				r_p1[2] += r_affinetridesc.seamfixupX16;
			if (index2->flags & ALIAS_ONSEAM)
				r_p2[2] += r_affinetridesc.seamfixupX16;
		}

		D_PolysetSetEdgeTable();
		D_RasterizeAliasPolySmooth();
	}
}


/*
================
D_PolysetRecursiveTriangle
================
*/
void D_PolysetRecursiveTriangle( int* lp1, int* lp2, int* lp3 )
{
	int* temp;
	int		d;
	int		_new[6];
	int		z;
	short* zbuf;

	d = lp2[0] - lp1[0];
	if (d < -1 || d > 1)
		goto split;
	d = lp2[1] - lp1[1];
	if (d < -1 || d > 1)
		goto split;

	d = lp3[0] - lp2[0];
	if (d < -1 || d > 1)
		goto split2;
	d = lp3[1] - lp2[1];
	if (d < -1 || d > 1)
		goto split2;

	d = lp1[0] - lp3[0];
	if (d < -1 || d > 1)
		goto split3;
	d = lp1[1] - lp3[1];
	if (d < -1 || d > 1)
	{
	split3:
		temp = lp1;
		lp1 = lp3;
		lp3 = lp2;
		lp2 = temp;

		goto split;
	}

	return;			// entire tri is filled

split2:
	temp = lp1;
	lp1 = lp2;
	lp2 = lp3;
	lp3 = temp;

split:
// split this edge
	_new[0] = (lp1[0] + lp2[0]) >> 1;
	_new[1] = (lp1[1] + lp2[1]) >> 1;
	_new[2] = (lp1[2] + lp2[2]) >> 1;
	_new[3] = (lp1[3] + lp2[3]) >> 1;
	_new[5] = (lp1[5] + lp2[5]) >> 1;

// draw the point if splitting a leading edge
	if (lp2[1] > lp1[1])
		goto nodraw;
	if ((lp2[1] == lp1[1]) && (lp2[0] < lp1[0]))
		goto nodraw;


	z = _new[5] >> 16;
	zbuf = zspantable[_new[1]] + _new[0];
	if (z >= *zbuf)
	{
		int		pix;
		unsigned short* lpdest;

		*zbuf = z;
		pix = d_pcolormap[skintable[_new[3] >> 16][_new[2] >> 16]];
		lpdest = (unsigned short*)&d_viewbuffer[d_scantable[_new[1]] + _new[0]];
		*lpdest = hlRGB(r_palette, pix);
	}

nodraw:
// recursively continue
	D_PolysetRecursiveTriangle(lp3, lp1, _new);
	D_PolysetRecursiveTriangle(lp3, _new, lp2);
}

#endif	// !id386

/*
================
D_RasterizeAliasPolySmooth
================
*/
void D_RasterizeAliasPolySmooth( void )
{
	// TODO: Implement
}

/*
================
D_PolysetSetEdgeTable
================
*/
void D_PolysetSetEdgeTable( void )
{
	// TODO: Implement
}