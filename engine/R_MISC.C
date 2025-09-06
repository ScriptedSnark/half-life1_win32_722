// r_misc.c

#include "quakedef.h"
#include "winquake.h"
#include "shake.h"
#include "water.h"

/*
===============
R_CheckVariables
===============
*/
void R_CheckVariables( void )
{
	static float	oldbright, oldmap, oldstyle;

	if (r_fullbright.value != oldbright || r_lightmap.value != oldmap || r_lightstyle.value != oldstyle)
	{
		oldbright = r_fullbright.value;
		oldmap = r_lightmap.value;
		oldstyle = r_lightstyle.value;
		D_FlushCaches();	// so all lighting changes
	}
}


/*
============
Show

Debugging use
============
*/
void Show( void )
{
	vrect_t	vr;

	vr.x = vr.y = 0;
	vr.width = vid.width;
	vr.height = vid.height;
	vr.pnext = NULL;
	VID_Update(&vr);
}


/*
====================
R_TimeRefresh_f

For program optimization
====================
*/
void R_TimeRefresh_f( void )
{
	int			i;
	float		start, stop, time;
	int			startangle;
	vrect_t		vr;

	startangle = r_refdef.viewangles[1];

	start = Sys_FloatTime();
	for (i = 0; i < 128; i++)
	{
		r_refdef.viewangles[1] = i / 128.0 * 360.0;

		VID_LockBuffer();

		R_RenderView();

		VID_UnlockBuffer();

		vr.x = r_refdef.vrect.x;
		vr.y = r_refdef.vrect.y;
		vr.width = r_refdef.vrect.width;
		vr.height = r_refdef.vrect.height;
		vr.pnext = NULL;
		VID_Update(&vr);
	}
	stop = Sys_FloatTime();
	time = stop - start;
	Con_Printf("%f seconds (%f fps)\n", time, 128 / time);

	r_refdef.viewangles[1] = (float)startangle;
}


/*
================
R_LineGraph

Only called by R_DisplayTime
================
*/
void R_LineGraph( int x, int y, int h )
{
	int		i;
	unsigned short* dest;
	int		s;

// FIXME: should be disabled on no-buffer adapters, or should be in the driver

	x += r_refdef.vrect.x;
	y += r_refdef.vrect.y;

	dest = (unsigned short*)(vid.buffer + vid.rowbytes * y + x * 2);

	s = r_graphheight.value;

	if (h > s)
		h = s;

	for (i = 0; i < h; i++, dest -= vid.rowbytes)
	{
		dest[0] = 0xffff;
		*(dest - (vid.rowbytes >> 1)) = 0x30;
	}
	for (; i < s; i++, dest -= vid.rowbytes)
	{
		dest[0] = 0x30;
		*(dest - (vid.rowbytes >> 1)) = 0x30;
	}
}

/*
==============
R_TimeGraph

Performance monitoring tool
==============
*/
#define	MAX_TIMINGS		100
extern float mouse_x, mouse_y;
void R_TimeGraph( void )
{
	static	int		timex;
	int		a;
	static float	r_time2;
	static byte	r_timings[MAX_TIMINGS];
	int		x;

	a = (r_time1 - r_time2) / 0.01;
	r_time2 = r_time1;
//a = fabs(mouse_y * 0.05);
//a = (int)((r_refdef.vieworg[2] + 1024) / 1) % (int)r_graphheight.value;
//a = fabs(velocity[0]) / 20;
//a = ((int)fabs(origin[0]) / 8) % 20;
//a = (cl.idealpitch + 30) / 5;
	r_timings[timex] = a;
	a = timex;

	if (r_refdef.vrect.width <= MAX_TIMINGS)
		x = r_refdef.vrect.width - 1;
	else
		x = r_refdef.vrect.width -
			(r_refdef.vrect.width - MAX_TIMINGS) / 2;
	do
	{
		R_LineGraph(x, r_refdef.vrect.height - 2, r_timings[a]);
		if (x == 0)
			break;		// screen too small to hold entire thing
		x--;
		a--;
		if (a == -1)
			a = MAX_TIMINGS - 1;
	} while (a != timex);

	timex = (timex + 1) % MAX_TIMINGS;
}


/*
=============
R_PrintTimes
=============
*/
void R_PrintTimes( void )
{
	float	r_time2;
	float		ms;

	r_time2 = Sys_FloatTime();

	ms = 1000.0 * (r_time2 - r_time1);

	if (r_speeds.value > 1.0)
	{
		static float s_ms;
		s_ms = (1.0 - 1.0 / r_speeds.value) * s_ms + ms * (1.0 / r_speeds.value);
		ms = s_ms;
	}

	Con_DPrintf("%5.1f ms %3i/%3i/%3i poly %3i surf\n",
		ms, c_faceclip, r_polycount, r_drawnpolycount, c_surf);
	c_surf = 0;
}


/*
=============
R_PrintDSpeeds
=============
*/
void R_PrintDSpeeds( void )
{
	float	ms, dp_time, r_time2, rw_time, db_time, se_time, de_time, dv_time;

	r_time2 = Sys_FloatTime();

	dp_time = (dp_time2 - dp_time1) * 1000;
	rw_time = (rw_time2 - rw_time1) * 1000;
	db_time = (db_time2 - db_time1) * 1000;
	se_time = (se_time2 - se_time1) * 1000;
	de_time = (de_time2 - de_time1) * 1000;
	dv_time = (dv_time2 - dv_time1) * 1000;
	ms = (r_time2 - r_time1) * 1000;

	Con_DPrintf("%3i %4.1fp %3iw %4.1fb %3is %4.1fe %4.1fv\n",
		(int)ms, dp_time, (int)rw_time, db_time, (int)se_time, de_time,
		dv_time);
}


/*
=============
R_ScreenLuminance
=============
*/
void R_ScreenLuminance( void )
{
	int		i, j, count;
	float	r, g, b;
	float	rSum, gSum, bSum;
	float	lum;
	float	lumcolors[32];
	unsigned short* pDest;

	count = 0;

	r = 0.0;
	g = 0.0;
	b = 0.0;

	for (i = 0; i < 32; i++)
		lumcolors[i] = pow(i / 31.0, v_gamma.value / 1.8) * 256.0;

	if (vid.buffer)
	{
		pDest = (unsigned short*)(vid.buffer + (vid.width >> 1) + ((vid.width * vid.height) >> 1));

		if (vid.height != 0)
		{
			rSum = 0.0;
			gSum = 0.0;
			bSum = 0.0;

			for (i = 0; i < (int)(vid.height >> 1); i++)
			{
				if (vid.width != 0)
				{
					for (j = 0; j < (int)(vid.width >> 1); j++)
					{
						unsigned short pix = *pDest;

						if (is15bit)
						{
							rSum += lumcolors[(pix & 0x7C00) >> 10];
							gSum += lumcolors[(pix & 0x03E0) >> 5];
							bSum += lumcolors[pix & 0x001F];
						}
						else
						{
							rSum += lumcolors[(pix & 0xF800) >> 11];
							gSum += lumcolors[(pix & 0x07E0) >> 6];
							bSum += lumcolors[pix & 0x001F];
						}

						// Draw screen rectangle
						if (i == 0 || j == 0 || i == (vid.height >> 1) - 1 || j == (vid.width >> 1) - 1)
							*pDest = ~pix;

						pDest++;
						count++;
					}
				}

				pDest = (unsigned short*)((byte*)pDest + vid.width);
			}

			r = rSum;
			g = gSum;
			b = bSum;
		}
	}

	lum = r / (float)count * 0.299 + g / (float)count * 0.587 + b / (float)count * 0.114;
	Con_DPrintf("Lum %3.0f : %3.0fr %3.0fg %3.0fb\n", lum, r / (float)count, g / (float)count, b / (float)count);
}


/*
=============
R_PrintAliasStats
=============
*/
void R_PrintAliasStats( void )
{
	Con_Printf("%3i polygon model drawn\n", r_amodels_drawn);
}


/*
===================
R_TransformFrustum
===================
*/
void R_TransformFrustum( void )
{
	int		i;
	vec3_t	v, v2;

	for (i = 0; i < 4; i++)
	{
		v[0] = screenedge[i].normal[2];
		v[1] = -screenedge[i].normal[0];
		v[2] = screenedge[i].normal[1];

		v2[0] = v[1] * vright[0] + v[2] * vup[0] + v[0] * vpn[0];
		v2[1] = v[1] * vright[1] + v[2] * vup[1] + v[0] * vpn[1];
		v2[2] = v[1] * vright[2] + v[2] * vup[2] + v[0] * vpn[2];

		VectorCopy(v2, view_clipplanes[i].normal);

		view_clipplanes[i].dist = DotProduct(modelorg, v2);
	}
}


#if	!id386

/*
================
TransformVector
================
*/
void TransformVector( vec_t* in, vec_t* out )
{
	out[0] = DotProduct(in, vright);
	out[1] = DotProduct(in, vup);
	out[2] = DotProduct(in, vpn);
}

#endif


/*
================
R_TransformPlane
================
*/
void R_TransformPlane( mplane_t* p, float* normal, float* dist )
{
	float	d;

	d = DotProduct(r_origin, p->normal);
	*dist = p->dist - d;
// TODO: when we have rotating entities, this will need to use the view matrix
	TransformVector(p->normal, normal);
}


/*
===============
R_SetUpFrustumIndexes
===============
*/
void R_SetUpFrustumIndexes( void )
{
	int		i, j, * pindex;

	pindex = r_frustum_indexes;

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 3; j++)
		{
			if (view_clipplanes[i].normal[j] < 0)
			{
				pindex[j] = j;
				pindex[j + 3] = j + 3;
			}
			else
			{
				pindex[j] = j + 3;
				pindex[j + 3] = j;
			}
		}

	// FIXME: do just once at start
		pfrustum_indexes[i] = pindex;
		pindex += 6;
	}
}


/*
===============
R_SetupFrame
===============
*/
void R_SetupFrame( void )
{
	int				edgecount;
	vrect_t			vrect;
	float			w, h;
	int				alpha;

// don't allow cheats in multiplayer
	if (cl.maxclients > 1)
	{
		Cvar_Set("r_draworder", "0");
		Cvar_Set("r_fullbright", "0");
		Cvar_Set("r_ambient_r", "0");
		Cvar_Set("r_ambient_g", "0");
		Cvar_Set("r_ambient_b", "0");
		Cvar_Set("r_drawflat", "0");
	}

	if (r_numsurfs.value)
	{
		if ((surface_p - surfaces) > r_maxsurfsseen)
			r_maxsurfsseen = surface_p - surfaces;

		Con_Printf("Used %d of %d surfs; %d max\n", surface_p - surfaces,
			surf_max - surfaces, r_maxsurfsseen);
	}

	if (r_numedges.value)
	{
		edgecount = edge_p - r_edges;

		if (edgecount > r_maxedgesseen)
			r_maxedgesseen = edgecount;

		Con_Printf("Used %d of %d edges; %d max\n", edgecount,
			r_numallocatededges, r_maxedgesseen);
	}

	r_refdef.ambientlight.r = r_ambient_r.value;

	if (r_refdef.ambientlight.r < 0)
		r_refdef.ambientlight.r = 0;

	r_refdef.ambientlight.g = r_ambient_g.value;

	if (r_refdef.ambientlight.g < 0)
		r_refdef.ambientlight.g = 0;

	r_refdef.ambientlight.b = r_ambient_b.value;

	if (r_refdef.ambientlight.b < 0)
		r_refdef.ambientlight.b = 0;

	if (!sv.active)
		r_draworder.value = 0;	// don't let cheaters look behind walls

	R_CheckVariables();

	R_AnimateLight();

	r_framecount++;

	numbtofpolys = 0;

// debugging
#if 0
	r_refdef.vieworg[0] = 80;
	r_refdef.vieworg[1] = 64;
	r_refdef.vieworg[2] = 40;
	r_refdef.viewangles[0] = 0;
	r_refdef.viewangles[1] = 46.763641357;
	r_refdef.viewangles[2] = 0;
#endif

// build the transformation matrix for the given view angles
	VectorCopy(r_refdef.vieworg, modelorg);
	VectorCopy(r_refdef.vieworg, r_origin);

	AngleVectors(r_refdef.viewangles, vpn, vright, vup);

// current viewleaf
	r_oldviewleaf = r_viewleaf;
	r_viewleaf = Mod_PointInLeaf(r_origin, cl.worldmodel);

	r_dowarpold = r_dowarp;
	r_dowarp = r_waterwarp.value && ((r_viewleaf->contents <= CONTENTS_WATER && r_viewleaf->contents != CONTENTS_TRANSLUCENT) || cl.waterlevel > 2);

	alpha = V_FadeAlpha();
	if (alpha)
	{
		r_dowarp = TRUE;
		D_SetScreenFade(cl.sf.fader, cl.sf.fadeg, cl.sf.fadeb, alpha, !(cl.sf.fadeFlags & FFADE_MODULATE));
	}

	if (r_dowarpold != r_dowarp || r_viewchanged || lcd_x.value)
	{
		if (r_dowarp)
		{
			if ((vid.width <= vid.maxwarpwidth) &&
				(vid.height <= vid.maxwarpheight))
			{
				vrect.x = 0;
				vrect.y = 0;
				vrect.width = vid.width;
				vrect.height = vid.height;

				R_ViewChanged(&vrect, sb_lines, vid.aspect);
			}
			else
			{
				w = vid.width;
				h = vid.height;

				if (w > vid.maxwarpwidth)
				{
					h *= (float)vid.maxwarpwidth / w;
					w = vid.maxwarpwidth;
				}

				if (h > vid.maxwarpheight)
				{
					h = vid.maxwarpheight;
					w *= (float)vid.maxwarpheight / h;
				}

				vrect.x = 0;
				vrect.y = 0;
				vrect.width = (int)w;
				vrect.height = (int)h;

				R_ViewChanged(&vrect,
					(int)((float)sb_lines * (h / (float)vid.height)),
					vid.aspect * (h / w) *
					((float)vid.width / (float)vid.height));
			}
		}
		else
		{
			vrect.x = 0;
			vrect.y = 0;
			vrect.width = vid.width;
			vrect.height = vid.height;

			R_ViewChanged(&vrect, sb_lines, vid.aspect);
		}

		r_viewchanged = FALSE;
	}

// start off with just the four screen edge clip planes
	R_TransformFrustum();

// save base values
	VectorCopy(vpn, base_vpn);
	VectorCopy(vright, base_vright);
	VectorCopy(vup, base_vup);
	VectorCopy(modelorg, base_modelorg);

	R_SetUpFrustumIndexes();

	r_cache_thrash = FALSE;

// clear frame counts
	c_faceclip = 0;
	d_spanpixcount = 0;
	r_polycount = 0;
	r_drawnpolycount = 0;
	r_wholepolycount = 0;
	r_amodels_drawn = 0;
	r_outofsurfaces = 0;
	r_outofedges = 0;

	D_SetupFrame();
}