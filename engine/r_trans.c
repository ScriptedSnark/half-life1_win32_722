// r_trans.c - transparent objects

#include "quakedef.h"
#include "pmove.h"
#include "r_studio.h"
#include "r_trans.h"
#if !defined( GLQUAKE )
#include "d_local.h"
#endif

qboolean r_intentities;

int max_translucent_objects;

typedef struct
{
    cl_entity_t* pEnt;
    float distance;
} transObjRef;

int numTransObjs = 0;
int maxTransObjs = 0;
transObjRef* transObjects = NULL;

void R_DrawAliasModel( cl_entity_t* e );
void R_DrawSpriteModel( cl_entity_t* e );
int R_BmodelCheckBBox( model_t* clmodel, float* minmaxs );

/*
=================
R_AllocObjects
=================
*/
void R_AllocObjects( int nMax )
{
	if (transObjects)
		Con_Printf("Transparent objects reallocate\n");

	transObjects = (transObjRef*)malloc(sizeof(transObjRef) * nMax);
	memset(transObjects, 0, sizeof(transObjRef) * nMax);

	maxTransObjs = nMax;
}

/*
=================
R_DestroyObjects

Release all transparent objects
=================
*/
void R_DestroyObjects( void )
{
	if (transObjects)
	{
		free(transObjects);
		transObjects = NULL;
	}

	maxTransObjs = 0;
}

float GlowBlend( cl_entity_t* pEntity )
{
	vec3_t tmp;
	float dist, brightness;
	pmtrace_t trace;

	VectorSubtract(r_entorigin, r_origin, tmp);
	dist = Length(tmp);

	pmove.usehull = 2;
	if (r_traceglow.value)
		trace = PM_PlayerMove(r_origin, r_entorigin, PM_GLASS_IGNORE);
	else
		trace = PM_PlayerMove(r_origin, r_entorigin, PM_GLASS_IGNORE | PM_STUDIO_IGNORE);

	if ((1.0 - trace.fraction) * dist > 8.0)
		return 0.0;

	if (pEntity->renderfx == kRenderFxNoDissipation)
	{
		return pEntity->renderamt * (1.0 / 255.0);
	}

	// UNDONE: Tweak these magic numbers (19000 - falloff & 200 - sprite size)
	brightness = 19000.0 / (dist * dist);
	if (brightness < 0.05)
		brightness = 0.05;
	if (brightness > 1.0)
		brightness = 1.0;

	pEntity->scale = dist * (1.0 / 200.0);
	return brightness;
}

/*
=================
RotatedBBox

Calculate min/max coords of an oriented bounding box
=================
*/
void RotatedBBox( vec_t* mins, vec_t* maxs, vec_t* angles, vec_t* tmins, vec_t* tmaxs )
{
    int     i;
    float   v, max;

    if (!angles[0] && !angles[1] && !angles[2])
    {
        VectorCopy(mins, tmins);
        VectorCopy(maxs, tmaxs);
    }
    else
    {
        max = 0.0;
        for (i = 0; i < 3; i++)
        {
            v = fabs(mins[i]);
            if (v > max)
                max = v;
            v = fabs(maxs[i]);
            if (v > max)
                max = v;
        }
        tmaxs[0] = max;
        tmaxs[1] = tmaxs[0];
        tmaxs[2] = tmaxs[1];
        tmins[0] = -max;
        tmins[1] = tmins[0];
        tmins[2] = tmins[1];
    }
}

/*
=================
AddTEntity

Add a transparent entity to a list of transparent objects
=================
*/
void AddTEntity( cl_entity_t* pEnt )
{
	int     i;
	float   dist;
	vec3_t  v;

	if (numTransObjs >= maxTransObjs)
		Sys_Error("AddTentity: Too many objects");

	if (!pEnt->model || pEnt->model->type != mod_brush || pEnt->rendermode != kRenderTransAlpha)
	{
		VectorAdd(pEnt->model->maxs, pEnt->model->mins, v);
		VectorScale(v, 0.5, v);
		VectorAdd(v, pEnt->origin, v);
		VectorSubtract(r_origin, v, v);

		dist = DotProduct(v, v);
	}
	else
	{
		// max distance
		dist = 1E9F;
	}

	i = numTransObjs;
	while (i > 0)
	{
		if (transObjects[i - 1].distance >= dist)
			break;

		transObjects[i].pEnt = transObjects[i - 1].pEnt;
		transObjects[i].distance = transObjects[i - 1].distance;
		i--;
	}

	transObjects[i].pEnt = pEnt;
	transObjects[i].distance = dist;
	numTransObjs++;
}

/*
=================
AppendTEntity

Append a transparent entity to a list of transparent objects
=================
*/
void AppendTEntity( cl_entity_t* pEnt )
{
	float   dist;
	vec3_t  v;

	if (numTransObjs >= maxTransObjs)
		Sys_Error("AddTentity: Too many objects");

	VectorAdd(pEnt->model->mins, pEnt->model->maxs, v);
	VectorScale(v, 0.5, v);
	VectorAdd(v, pEnt->origin, v);
	VectorSubtract(r_origin, v, v);

	dist = DotProduct(v, v);

	transObjects[numTransObjs].pEnt = pEnt;
	transObjects[numTransObjs].distance = dist;
	numTransObjs++;
}

#if defined( GLQUAKE )
float r_blend;	// blending amount in [0..1] range

/*
=============
R_DrawTEntitiesOnList
=============
*/
void R_DrawTEntitiesOnList( void )
{
	int     i;

	if (!r_drawentities.value)
		return;

	// Handle all trans objects in the list
	for (i = 0; i < numTransObjs; i++)
	{
		currententity = transObjects[i].pEnt;

		r_blend = CL_FxBlend(currententity);
		if (r_blend <= 0.0)
			continue;

		r_blend /= 255.0;

		// Glow is only for sprite models
		if (currententity->rendermode == kRenderGlow && currententity->model->type != mod_sprite)
			Con_Printf("Non-sprite set to glow!\n");

		switch (currententity->model->type)
		{
		case mod_brush:
			R_DrawBrushModel(currententity);
			break;

		case mod_sprite:
			if (currententity->body)
			{
				float* pAttachment;

				pAttachment = R_GetAttachmentPoint(currententity->skin, currententity->body);
				VectorCopy(pAttachment, r_entorigin);
			}
			else
			{
				VectorCopy(currententity->origin, r_entorigin);
			}

			// Glow sprite
			if (currententity->rendermode == kRenderGlow)
			{
				r_blend *= GlowBlend(currententity);
			}

			if (r_blend != 0.0)
			{
				R_DrawSpriteModel(currententity);
			}
			break;

		case mod_alias:
			R_DrawAliasModel(currententity);
			break;

		case mod_studio:
			R_StudioDrawModel(STUDIO_RENDER | STUDIO_EVENTS);
			break;
		}
	}

	numTransObjs = 0;
	r_blend = 1.0;
}
#else
int r_blend;	// blending amount in [0..255] range

/*
=============
D_DrawTranslucentTexture
=============
*/
void D_DrawTranslucentTexture( espan_t* pspan )
{
	int				count, spancount;
    unsigned short* pbase, * pdest;
    fixed16_t		s, t, snext, tnext, sstep, tstep;
    float			sdivz, tdivz, zi, z, du, dv, spancountminus1;
    float			sdivz8stepu, tdivz8stepu, zi8stepu;
    int				izi, izistep;
    short* pzbuff;

    set_fpu_cw();

	sstep = 0;	// keep compiler happy
	tstep = 0;	// ditto

    pbase = (unsigned short*)cacheblock;

	sdivz8stepu = d_sdivzstepu * 8;
	tdivz8stepu = d_tdivzstepu * 8;
	zi8stepu = d_zistepu * 8;

// we count on FP exceptions being turned off to avoid range problems
    izistep = (int)(d_zistepu * 0x8000 * 0x10000);
    d_zidist = izistep;

    do
    {
        pdest = (unsigned short*)((byte*)d_viewbuffer +
				(screenwidth * pspan->v) + pspan->u * 2);
        pzbuff = &zspantable[pspan->v][pspan->u];

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
		if (s > bbextents)
			s = bbextents;
		else if (s < 0)
			s = 0;

        d_zidist = (int)(tdivz * z);
		t = (int)(tdivz * z) + tadjust;
		if (t > bbextentt)
			t = bbextentt;
		else if (t < 0)
			t = 0;

        if (zi > 0.9)
        {
            izi = 0x7F000000;
            izistep = 0;
        }
        else
        {
        // we count on FP exceptions being turned off to avoid range problems
            izi = (int)(zi * 0x8000 * 0x10000);
            d_zidist = izi;
        }

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

                d_zidist = (int)(sdivz * z);
				snext = (int)(sdivz * z) + sadjust;
				if (snext > bbextents)
					snext = bbextents;
				else if (snext < 8)
					snext = 8;	// prevent round-off error on <0 steps from
								//  from causing overstepping & running off the
								//  edge of the texture

                d_zidist = (int)(tdivz * z);
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

                d_zidist = (int)(sdivz * z);
				snext = (int)(sdivz * z) + sadjust;
				if (snext > bbextents)
					snext = bbextents;
				else if (snext < 8)
					snext = 8;	// prevent round-off error on <0 steps from
								//  from causing overstepping & running off the
								//  edge of the texture

                d_zidist = (int)(tdivz * z);
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

            if (is15bit)
            {
                do
                {
                    if (*pzbuff < izi >> 16)
                    {
                        unsigned int newcolor = pbase[(s >> 16) + (t >> 16) * cachewidth];
                        unsigned int oldcolor = *pdest;
                        unsigned int deltacolor, signbits, blendbits, mask;

                        const unsigned int highbits = (0x80000000 | 0x00200000 | 0x00000400);
                        const unsigned int lowbits = (0x40000000 | 0x00100000 | 0x00000200);
                        const unsigned int redblue = (0x7C00 | 0x001F);
                        const unsigned int green = (0x03E0);

                        blendbits = r_blend & ~7;

                        oldcolor = ((oldcolor & redblue) << 16) | (oldcolor & green);
                        newcolor = ((newcolor & redblue) << 16) | (newcolor & green);
                        deltacolor = ((newcolor | highbits) - oldcolor) >> 1;
                        
                        mask = (1 << 8);
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

                    pdest++;
                    pzbuff++;
                    s += sstep;
                    t += tstep;
                    izi += izistep;
                } while (--spancount > 0);
            }
            else
            {
                do
                {
                    if (*pzbuff < izi >> 16)
                    {
                        unsigned int newcolor = pbase[(s >> 16) + (t >> 16) * cachewidth];
                        unsigned int oldcolor = *pdest;
                        unsigned int deltacolor, signbits, blendbits, mask;

                        const unsigned int highbits = (0x00000000 | 0x00200000 | 0x00000800);
                        const unsigned int lowbits = (0x80000000 | 0x00100000 | 0x00000400);
                        const unsigned int redblue = (0xF800 | 0x001F);
                        const unsigned int green = (0x07E0);

                        oldcolor = ((oldcolor & redblue) << 16) | (oldcolor & green);
                        newcolor = ((newcolor & redblue) << 16) | (newcolor & green);
                        deltacolor = ((newcolor | highbits) - oldcolor) >> 1;

                        if ((newcolor & 0xF8000000) >= (oldcolor & 0xF8000000))
                            deltacolor |= 0x80000000;

                        mask = (1 << 8);
                        blendbits = r_blend & ~7;
                        signbits = (~deltacolor & lowbits);

                        do
                        {
                            mask >>= 1;
                            oldcolor &= ~highbits;
                            deltacolor = signbits | deltacolor & ~lowbits;

                            if ((mask & blendbits) != 0)
                            {
                                blendbits ^= mask;
                                oldcolor += deltacolor;
                            }
                            deltacolor >>= 1;
                        } while (blendbits);

                        *pdest = ((oldcolor >> 16) & redblue) | (oldcolor & green);
                    }

                    pdest++;
                    pzbuff++;
                    s += sstep;
                    t += tstep;
                    izi += izistep;
                } while (--spancount > 0);
            }

            s = snext;
            t = tnext;

		} while (count > 0);

	} while ((pspan = pspan->pnext) != NULL);

    restore_fpu_cw();
}

/*
=============
D_DrawTranslucentColor
=============
*/
void D_DrawTranslucentColor( espan_t* pspan )
{
	int				count, spancount;
	unsigned short* pdest;
	float			zi, du, dv;
	float           zi8stepu;
	unsigned int    packedColor;
	int	            r, g, b, a, oneMinusAlpha, alpha;
	int             izi, izistep;
	short* pzbuff;

	set_fpu_cw();

	zi8stepu = d_zistepu * 8;

// we count on FP exceptions being turned off to avoid range problems
	izistep = (int)(d_zistepu * 0x8000 * 0x10000);
	d_zidist = izistep;

	do
	{
		pdest = (unsigned short*)((byte*)d_viewbuffer +
				(screenwidth * pspan->v) + pspan->u * 2);
		pzbuff = &zspantable[pspan->v][pspan->u];

		count = pspan->count;

	// calculate the initial s/z, t/z, 1/z, s, and t and clamp
		du = (float)pspan->u;
		dv = (float)pspan->v;

		zi = d_ziorigin + dv * d_zistepv + du * d_zistepu;

		if (zi > 0.9)
		{
			izi = 0x7F000000;
			izistep = 0;
		}
		else
		{
		// we count on FP exceptions being turned off to avoid range problems
			izi = (int)(zi * 0x8000 * 0x10000);
			d_zidist = izi;
		}

		r = currententity->rendercolor.r;
		g = currententity->rendercolor.g;
		b = currententity->rendercolor.b;
		a = currententity->renderamt;

		do
		{
		// calculate s and t at the far end of the span
			if (count >= 8)
				spancount = 8;
			else
				spancount = count;

			count -= spancount;

			oneMinusAlpha = 255 - a;

			alpha = (192 * (word)a) & 0xFF00;
			packedColor = red_64klut[r + alpha] | green_64klut[g + alpha] | blue_64klut[b + alpha];

			if (is15bit)
			{
				do
				{
					if (*pzbuff < izi >> 16)
					{
						*pdest = packedColor + (
							((oneMinusAlpha * (*pdest & 0x7C00)) >> 8) & 0x7C00 |
							((oneMinusAlpha * (*pdest & 0x03E0)) >> 8) & 0x03E0 |
							((oneMinusAlpha * (*pdest & 0x001F)) >> 8) & 0x001F);
					}

					pdest++;
					pzbuff++;
					izi += izistep;
				} while (--spancount > 0);
			}
			else
			{
				do
				{
					if (*pzbuff < izi >> 16)
					{
						*pdest = packedColor + (
							((oneMinusAlpha * (*pdest & 0xF800)) >> 8) & 0xF800 |
							((oneMinusAlpha * (*pdest & 0x07E0)) >> 8) & 0x07E0 |
							((oneMinusAlpha * (*pdest & 0x001F)) >> 8) & 0x001F);
					}

					pdest++;
					pzbuff++;
					izi += izistep;
				} while (--spancount > 0);
			}

		} while (count > 0);

	} while ((pspan = pspan->pnext) != NULL);

	restore_fpu_cw();
}

/*
=============
D_DrawTranslucentAdd
=============
*/
void D_DrawTranslucentAdd( espan_t* pspan )
{
	int				count, spancount;
	unsigned short* pbase, * pdest;
	fixed16_t		s, t, snext, tnext, sstep, tstep;
	float			sdivz, tdivz, zi, z, du, dv, spancountminus1;
	float			sdivz8stepu, tdivz8stepu, zi8stepu;
	int				izi, izistep;
	short* pzbuff;

	set_fpu_cw();

	sstep = 0;	// keep compiler happy
	tstep = 0;	// ditto

	pbase = (unsigned short*)cacheblock;

	sdivz8stepu = d_sdivzstepu * 8;
	tdivz8stepu = d_tdivzstepu * 8;
	zi8stepu = d_zistepu * 8;

// we count on FP exceptions being turned off to avoid range problems
	izistep = (int)(d_zistepu * 0x8000 * 0x10000);
	d_zidist = izistep;

	do
	{
		pdest = (unsigned short*)((byte*)d_viewbuffer +
				(screenwidth * pspan->v) + pspan->u * 2);
		pzbuff = &zspantable[pspan->v][pspan->u];

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
		if (s > bbextents)
			s = bbextents;
		else if (s < 0)
			s = 0;

		d_zidist = (int)(tdivz * z);
		t = (int)(tdivz * z) + tadjust;
		if (t > bbextentt)
			t = bbextentt;
		else if (t < 0)
			t = 0;

		if (zi > 0.9)
		{
			izi = 0x7F000000;
			izistep = 0;
		}
		else
		{
		// we count on FP exceptions being turned off to avoid range problems
			izi = (int)(zi * 0x8000 * 0x10000);
			d_zidist = izi;
		}

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

				d_zidist = (int)(sdivz * z);
				snext = (int)(sdivz * z) + sadjust;
				if (snext > bbextents)
					snext = bbextents;
				else if (snext < 8)
					snext = 8;	// prevent round-off error on <0 steps from
								//  from causing overstepping & running off the
								//  edge of the texture

				d_zidist = (int)(tdivz * z);
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

				d_zidist = (int)(sdivz * z);
				snext = (int)(sdivz * z) + sadjust;
				if (snext > bbextents)
					snext = bbextents;
				else if (snext < 8)
					snext = 8;	// prevent round-off error on <0 steps from
								//  from causing overstepping & running off the
								//  edge of the texture

				d_zidist = (int)(tdivz * z);
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

			if (is15bit)
			{
				do
				{
					if (*pzbuff < izi >> 16)
					{
						unsigned int newcolor = pbase[(s >> 16) + (t >> 16) * cachewidth];
						unsigned int oldcolor = *pdest;
						unsigned int deltacolor, carrybits = 0, blendbits, mask;

						const unsigned int highbits = (0x80000000 | 0x00200000 | 0x00000400);
						const unsigned int lowbits = (0x40000000 | 0x00100000 | 0x00000200);
						const unsigned int redblue = (0x7C00 | 0x001F);
						const unsigned int green = (0x03E0);

						oldcolor = ((oldcolor & redblue) << 16) | (oldcolor & green);
						newcolor = ((newcolor & redblue) << 16) | (newcolor & green);

						blendbits = r_blend & ~15;
						if (blendbits < 240)
						{
							deltacolor = newcolor >> 1;
							mask = (1 << 8);

							do
							{
								mask >>= 1;
								deltacolor &= ~lowbits;
								oldcolor &= ~highbits;

								if (blendbits & mask)
								{
									oldcolor += deltacolor;
									blendbits ^= mask;
									carrybits |= (oldcolor & highbits);
								}
								deltacolor >>= 1;
							} while (blendbits);
						}
						else
						{
							oldcolor += newcolor;
							carrybits = (oldcolor & highbits);
						}

						if (carrybits)
						{
							// adjust the color
							oldcolor |= highbits - (carrybits >> 5);
						}

						*pdest = ((oldcolor >> 16) & redblue) | (oldcolor & green);
					}

					pdest++;
					pzbuff++;
					s += sstep;
					t += tstep;
					izi += izistep;
				} while (--spancount > 0);
			}
			else
			{
				do
				{
					if (*pzbuff < izi >> 16)
					{
						unsigned int newcolor = pbase[(s >> 16) + (t >> 16) * cachewidth];
						unsigned int oldcolor = *pdest, prevcolor;
						unsigned int deltacolor, carrybits = 0, blendbits, mask;

						const unsigned int highbits = (0x00000000 | 0x00200000 | 0x00000800);
						const unsigned int lowbits = (0x00000000 | 0x00100000 | 0x00000400);
						const unsigned int redblue = (0xF800 | 0x001F);
						const unsigned int green = (0x07E0);

						oldcolor = ((oldcolor & redblue) << 16) | (oldcolor & green);
						prevcolor = oldcolor;
						newcolor = ((newcolor & redblue) << 16) | (newcolor & green);

						blendbits = r_blend & ~15;
						if (blendbits < 240)
						{
							deltacolor = newcolor >> 1;
							mask = (1 << 8);

							do
							{
								mask >>= 1;
								deltacolor &= ~(0x80000000 | lowbits);
								oldcolor &= ~highbits;

								if (blendbits & mask)
								{
									prevcolor = oldcolor;
									oldcolor += deltacolor;
									blendbits ^= mask;
									carrybits |= (oldcolor & highbits);

									if (prevcolor > oldcolor)
										carrybits |= 1;
								}
								deltacolor >>= 1;
							} while (blendbits);
						}
						else
						{
							oldcolor += newcolor;
							carrybits = (oldcolor & highbits);

							if (prevcolor > oldcolor)
								carrybits |= 1;
						}

						if (carrybits)
						{
							// adjust the color
							carrybits = _rotr(carrybits, 1);
							oldcolor |= ((carrybits | lowbits) - (carrybits >> 5)) << 1;
						}

						*pdest = ((oldcolor >> 16) & redblue) | (oldcolor & green);
					}

					pdest++;
					pzbuff++;
					s += sstep;
					t += tstep;
					izi += izistep;
				} while (--spancount > 0);
			}

			s = snext;
			t = tnext;

		} while (count > 0);

	} while ((pspan = pspan->pnext) != NULL);

	restore_fpu_cw();
}

/*
=============
D_DrawTransHoles
=============
*/
void D_DrawTransHoles( espan_t* pspan )
{
	fixed16_t		s, t;
	unsigned short* pbase, * pdest;
	short* pzbuff;
	int				count, spancount;
	fixed16_t		snext, tnext, sstep, tstep;
	float			sdivz, tdivz, zi, z, du, dv, spancountminus1;
	float			sdivz8stepu, tdivz8stepu, zi8stepu;
	int				izi, izistep;

	set_fpu_cw();

	sstep = 0;	// keep compiler happy
	tstep = 0;	// ditto

	pbase = (unsigned short*)cacheblock;

	sdivz8stepu = d_sdivzstepu * 8;
	tdivz8stepu = d_tdivzstepu * 8;
	zi8stepu = d_zistepu * 8;

// we count on FP exceptions being turned off to avoid range problems
	izistep = (int)(d_zistepu * 0x8000 * 0x10000);
	d_zidist = izistep;

	do
	{
		pdest = (unsigned short*)((byte*)d_viewbuffer +
				(screenwidth * pspan->v) + pspan->u * 2);
		pzbuff = &zspantable[pspan->v][pspan->u];

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
		if (s > bbextents)
			s = bbextents;
		else if (s < 0)
			s = 0;

		d_zidist = (int)(tdivz * z);
		t = (int)(tdivz * z) + tadjust;
		if (t > bbextentt)
			t = bbextentt;
		else if (t < 0)
			t = 0;

		if (zi > 0.9)
		{
			izi = 0x7F000000;
			izistep = 0;
		}
		else
		{
		// we count on FP exceptions being turned off to avoid range problems
			izi = (int)(zi * 0x8000 * 0x10000);
			d_zidist = izi;
		}

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

				d_zidist = (int)(sdivz * z);
				snext = (int)(sdivz * z) + sadjust;
				if (snext > bbextents)
					snext = bbextents;
				else if (snext < 8)
					snext = 8;	// prevent round-off error on <0 steps from
								//  from causing overstepping & running off the
								//  edge of the texture

				d_zidist = (int)(tdivz * z);
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

				d_zidist = (int)(sdivz * z);
				snext = (int)(sdivz * z) + sadjust;
				if (snext > bbextents)
					snext = bbextents;
				else if (snext < 8)
					snext = 8;	// prevent round-off error on <0 steps from
								//  from causing overstepping & running off the
								//  edge of the texture

				d_zidist = (int)(tdivz * z);
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
				if (*pzbuff < izi >> 16)
				{
					unsigned short newcolor;

					newcolor = pbase[(s >> 16) + (t >> 16) * cachewidth];
					if (newcolor != 0x001F)
					{
						*pdest = newcolor;
						*pzbuff = izi >> 16;
					}
				}

				pdest++;
				pzbuff++;
				s += sstep;
				t += tstep;
				izi += izistep;
			} while (--spancount > 0);

			s = snext;
			t = tnext;

		} while (count > 0);

	} while ((pspan = pspan->pnext) != NULL);

	restore_fpu_cw();
}

/*
=============
R_DrawTEntitiesOnList
=============
*/
void R_DrawTEntitiesOnList( void )
{
	int         i, j, k, clipflags;
	vec3_t		oldorigin;
	model_t* clmodel;
	float		minmaxs[6];

	edge_t	ledges[NUMSTACKEDGES +
		((CACHE_SIZE - 1) / sizeof(edge_t)) + 1];
	surf_t	lsurfs[NUMSTACKSURFACES +
		((CACHE_SIZE - 1) / sizeof(surf_t)) + 1];

	if (!r_drawentities.value)
		return;

	if (auxedges)
	{
		r_edges = auxedges;
	}
	else
	{
		r_edges = (edge_t*)
			(((long)&ledges[0] + CACHE_SIZE - 1) & ~(CACHE_SIZE - 1));
	}

	if (r_surfsonstack)
	{
		surfaces = (surf_t*)
			(((long)&lsurfs[0] + CACHE_SIZE - 1) & ~(CACHE_SIZE - 1));
		surf_max = &surfaces[r_cnumsurfs];
	// surface 0 doesn't really exist; it's just a dummy because index 0
	// is used to indicate no edge attached to surface
		surfaces--;
		R_SurfacePatch();
	}

	VectorCopy(modelorg, oldorigin);

	r_dlightframecount = r_framecount;

	r_intentities = TRUE; // mark as transparent

	insubmodel = TRUE;

	for (i = 0; i < numTransObjs; i++)
	{
		currententity = transObjects[i].pEnt;
		r_blend = CL_FxBlend(currententity);
		if (r_blend == 0)
			continue;

		if (currententity->rendermode == kRenderGlow && currententity->model->type != mod_sprite)
			Con_Printf("Non-sprite set to glow!\n");

		switch (currententity->model->type)
		{
		case mod_brush:

			clmodel = currententity->model;

			RotatedBBox(clmodel->mins, clmodel->maxs, currententity->angles, &minmaxs[0], &minmaxs[3]);

			// see if the bounding box lets us trivially reject, also sets
			// trivial accept status
			for (j = 0; j < 3; j++)
			{
				minmaxs[j] += currententity->origin[j];
				minmaxs[3 + j] += currententity->origin[j];
			}

			clipflags = R_BmodelCheckBBox(clmodel, minmaxs);

			if (clipflags != BMODEL_FULLY_CLIPPED)
			{
				R_BeginEdgeFrame();

				VectorCopy(currententity->origin, r_entorigin);
				VectorSubtract(r_origin, r_entorigin, modelorg);
			// FIXME: is this needed?
				VectorCopy(modelorg, r_worldmodelorg);

				r_pcurrentvertbase = clmodel->vertexes;

			// FIXME: stop transforming twice
				R_RotateBmodel();

			// calculate dynamic lighting for bmodel if it's not an
			// instanced model
				if (currententity->rendermode == kRenderTransAlpha && clmodel->firstmodelsurface != 0)
				{
					for (k = 0; k < MAX_DLIGHTS; k++)
					{
						vec3_t saveOrigin;

						if ((cl_dlights[k].die < cl.time) ||
							(!cl_dlights[k].radius))
							continue;

						VectorCopy(cl_dlights[k].origin, saveOrigin); // save prev
						VectorSubtract(cl_dlights[k].origin, currententity->origin, cl_dlights[k].origin);

						R_MarkLights(&cl_dlights[k], 1 << k,
							clmodel->nodes + clmodel->hulls[0].firstclipnode);

						VectorCopy(saveOrigin, cl_dlights[k].origin);
					}
				}

				switch (currententity->rendermode)
				{
				case kRenderTransColor:
					d_drawspans = D_DrawTranslucentColor;
					break;
				case kRenderTransTexture:
					d_drawspans = D_DrawTranslucentTexture;
					break;
				case kRenderTransAlpha:
					d_drawspans = D_DrawTransHoles;
					break;
				case kRenderTransAdd:
					d_drawspans = D_DrawTranslucentAdd;
					break;
				}

				if (r_drawpolys | r_drawculledpolys)
				{
					R_ZDrawSubmodelPolys(clmodel);
				}
				else
				{
					r_pefragtopnode = NULL;

					for (j = 0; j < 3; j++)
					{
						r_emins[j] = minmaxs[j];
						r_emaxs[j] = minmaxs[3 + j];
					}

					R_SplitEntityOnNode2(cl.worldmodel->nodes);

					if (r_pefragtopnode)
					{
						r_clipflags = clipflags;

						currententity->topnode = r_pefragtopnode;

					// falls entirely in one leaf, so we just put all the
					// edges in the edge list and let 1/z sorting handle
					// drawing order
						R_DrawSubmodelPolygons(clmodel, clipflags);

						currententity->topnode = NULL;
					}
				}

			// put back world rotation and frustum clipping		
			// FIXME: R_RotateBmodel should just work off base_vxx
				VectorCopy(base_vpn, vpn);
				VectorCopy(base_vup, vup);
				VectorCopy(base_vright, vright);
				VectorCopy(oldorigin, modelorg);
				R_TransformFrustum();
				R_ScanEdges();
			}

			break;

		case mod_sprite:
			switch (currententity->rendermode)
			{
			case kRenderTransTexture:
				spritedraw = D_SpriteDrawSpansTrans;
				break;
			case kRenderGlow:
				spritedraw = D_SpriteDrawSpansGlow;
				break;
			case kRenderTransAlpha:
				spritedraw = D_SpriteDrawSpansAlpha;
				break;
			case kRenderTransAdd:
				spritedraw = D_SpriteDrawSpansAdd;
				break;
			default:
				spritedraw = D_SpriteDrawSpans;
				break;
			}

			// look up for attachment
			if (currententity->body)
			{
				float* pAttachment = R_GetAttachmentPoint(currententity->skin, currententity->body);
				VectorCopy(pAttachment, r_entorigin);
			}
			else
			{
				VectorCopy(currententity->origin, r_entorigin);
			}

			VectorSubtract(r_origin, r_entorigin, modelorg);

			// Glow sprite
			if (currententity->rendermode == kRenderGlow)
				r_blend *= GlowBlend(currententity);

			if (r_blend != 0)
			{
				R_DrawSprite();
			}
			break;

		case mod_studio:
		{
			void (*poldpolysetdraw)(spanpackage_t*);

			poldpolysetdraw = polysetdraw;

			switch (currententity->rendermode)
			{
			case kRenderTransAdd:
				polysetdraw = D_PolysetDrawSpansTransAdd;
				break;

			default:
				polysetdraw = D_PolysetDrawSpansTrans;
				break;
			}

			R_StudioDrawModel(STUDIO_RENDER | STUDIO_EVENTS);

			polysetdraw = poldpolysetdraw;
		}
		break;

		default:
			break;
		}
	}

	d_drawspans = D_DrawSpans8;

	insubmodel = FALSE;

	r_intentities = FALSE;

	numTransObjs = 0;
}
#endif