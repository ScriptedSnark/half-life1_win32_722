#include "quakedef.h"
#include "pmove.h"
#include "r_studio.h"

pmtrace_t g_Trace;
int PM_global_testContents;

static	hull_t		box_hull;
static	dclipnode_t	box_clipnodes[6];
static	mplane_t	box_planes[6];

extern vec3_t player_mins[3];
extern vec3_t player_maxs[3];

/*
===================
PM_InitBoxHull

Set up the planes and clipnodes so that the six floats of a bounding box
can just be stored out and get a proper hull_t structure.
===================
*/
void PM_InitBoxHull( void )
{
	int		i;
	int		side;

	box_hull.clipnodes = &box_clipnodes[0];
	box_hull.planes = &box_planes[0];
	box_hull.firstclipnode = 0;
	box_hull.lastclipnode = 5;

	for (i = 0; i < 6; i++)
	{
		box_clipnodes[i].planenum = i;

		side = i & 1;

		box_clipnodes[i].children[side] = CONTENTS_EMPTY;
		if (i != 5)
			box_clipnodes[i].children[side ^ 1] = i + 1;
		else
			box_clipnodes[i].children[side ^ 1] = CONTENTS_SOLID;

		box_planes[i].type = i >> 1;
		box_planes[i].normal[i >> 1] = 1;
	}
}


/*
===================
PM_HullForBox

To keep everything totally uniform, bounding boxes are turned into small
BSP trees instead of being compared directly.
===================
*/
hull_t* PM_HullForBox( vec_t* mins, vec_t* maxs )
{
	box_planes[0].dist = maxs[0];
	box_planes[1].dist = mins[0];
	box_planes[2].dist = maxs[1];
	box_planes[3].dist = mins[1];
	box_planes[4].dist = maxs[2];
	box_planes[5].dist = mins[2];

	return &box_hull;
}


/*
==================
PM_HullPointContents

==================
*/
int PM_HullPointContents( hull_t* hull, int num, vec_t* p )
{
	float		d;
	dclipnode_t* node;
	mplane_t*	plane;

	if (hull->firstclipnode >= hull->lastclipnode)
		return CONTENTS_EMPTY;

	while (num >= 0)
	{
		if (num < hull->firstclipnode || num > hull->lastclipnode)
			Sys_Error("PM_HullPointContents: bad node number");

		node = hull->clipnodes + num;
		plane = hull->planes + node->planenum;

		if (plane->type < 3)
			d = p[plane->type] - plane->dist;
		else
			d = DotProduct(plane->normal, p) - plane->dist;

		if (d < 0)
			num = node->children[1];
		else
			num = node->children[0];
	}

	return num;
}

/*
==================
PM_SimulateLinkContents

==================
*/
int PM_SimulateLinkContents( vec_t* p, int* pIndex )
{
	int		i;
	physent_t* pe;
	vec3_t	test;
	hull_t* hull;
	int		num;
	int		cont;

	num = pmove.numphysent;

	for (i = 1; i < num; i++)
	{
		pe = &pmove.physents[i];

		if (pe->solid != SOLID_NOT || !pe->model)
			continue;

		VectorSubtract(p, pe->origin, test);

		hull = pe->model->hulls;
		cont = PM_HullPointContents(hull, hull[0].firstclipnode, test);
		if (cont != CONTENTS_EMPTY)
		{
			if (pIndex)
				*pIndex = pe->info;

			return pe->skin;
		}
	}

	return CONTENTS_EMPTY;
}

/*
==================
PM_WaterEntity

==================
*/
int PM_PointContents( vec_t* p )
{
	hull_t* hull;
	int		cont, entityContents;

	hull = pmove.physents[0].model->hulls;

	entityContents = PM_HullPointContents(hull, hull[0].firstclipnode, p);
	if (entityContents <= CONTENTS_CURRENT_0 && entityContents >= CONTENTS_CURRENT_DOWN)
		entityContents = CONTENTS_WATER;

	if (entityContents == CONTENTS_SOLID)
		return entityContents;

	cont = PM_SimulateLinkContents(p, NULL);
	if (cont != CONTENTS_EMPTY)
		return cont;

	return entityContents;	
}

/*
==================
PM_WaterEntity

==================
*/
int PM_WaterEntity( vec_t* p )
{
	hull_t* hull;
	int		num, entityIndex;
	int		cont;

	entityIndex = -1;
	hull = pmove.physents[0].model->hulls;
	num = pmove.numphysent;
	cont = PM_HullPointContents(hull, hull[0].firstclipnode, p);
	if (cont < CONTENTS_SOLID)
		entityIndex = 0;

	if (cont != CONTENTS_SOLID)
		PM_SimulateLinkContents(p, &entityIndex);

	return entityIndex;
}

/*
==================
PM_TruePointContents

==================
*/
int PM_TruePointContents( vec_t* p )
{
	hull_t* hull;

	hull = pmove.physents[0].model->hulls;
	if (hull != NULL)
		return PM_HullPointContents(hull, hull[0].firstclipnode, p);

	return CONTENTS_EMPTY;
}

/*
===============================================================================

LINE TESTING IN HULLS

===============================================================================
*/

// 1/32 epsilon to keep floating point happy
#define	DIST_EPSILON	(0.03125)

/*
==================
PM_RecursiveHullCheck

==================
*/
qboolean PM_RecursiveHullCheck( hull_t* hull, int num, float p1f, float p2f, vec_t* p1, vec_t* p2, pmtrace_t* trace )
{
	dclipnode_t* node;
	mplane_t* plane;
	float		t1, t2;
	float		frac;
	int			i;
	vec3_t		mid;
	int			side;
	int			otherside;
	float		midf;
	qboolean	retval = FALSE;

// check for empty
	if (num < 0)
	{
		if (num != CONTENTS_SOLID)
		{
			trace->allsolid = FALSE;
			if (num == CONTENTS_EMPTY)
				trace->inopen = TRUE;
			else
				trace->inwater = TRUE;
		}
		else
			trace->startsolid = TRUE;
		return TRUE;		// empty
	}

	if (hull->firstclipnode >= hull->lastclipnode)
	{
		trace->inopen = TRUE;
		trace->allsolid = FALSE;
		return TRUE;
	}

	if (num < hull->firstclipnode || num > hull->lastclipnode)
		Sys_Error("PM_RecursiveHullCheck: bad node number");

//
// find the point distances
//
	node = hull->clipnodes + num;
	plane = hull->planes + node->planenum;

	if (plane->type < 3)
	{
		t1 = p1[plane->type] - plane->dist;
		t2 = p2[plane->type] - plane->dist;
	}
	else
	{
		t1 = DotProduct(plane->normal, p1) - plane->dist;
		t2 = DotProduct(plane->normal, p2) - plane->dist;
	}

#if 1
	if (t1 >= 0 && t2 >= 0)
		return PM_RecursiveHullCheck(hull, node->children[0], p1f, p2f, p1, p2, trace);
	if (t1 < 0 && t2 < 0)
		return PM_RecursiveHullCheck(hull, node->children[1], p1f, p2f, p1, p2, trace);
#else
	if ((t1 >= DIST_EPSILON && t2 >= DIST_EPSILON) || (t2 > t1 && t1 >= 0))
		return PM_RecursiveHullCheck (hull, node->children[0], p1f, p2f, p1, p2, trace);
	if ((t1 <= -DIST_EPSILON && t2 <= -DIST_EPSILON) || (t2 < t1 && t1 <= 0))
		return PM_RecursiveHullCheck (hull, node->children[1], p1f, p2f, p1, p2, trace);
#endif

// put the crosspoint DIST_EPSILON pixels on the near side
	if (t1 < 0)
		frac = (t1 + DIST_EPSILON) / (t1 - t2);
	else
		frac = (t1 - DIST_EPSILON) / (t1 - t2);
	if (frac < 0)
		frac = 0;
	if (frac > 1)
		frac = 1;

	midf = p1f + (p2f - p1f) * frac;
	for (i = 0; i < 3; i++)
		mid[i] = p1[i] + frac * (p2[i] - p1[i]);

	side = (t1 < 0);

// move up to the node
	if (!PM_RecursiveHullCheck(hull, node->children[side], p1f, midf, p1, mid, trace))
		return FALSE;

#ifdef PARANOID
	if (PM_HullPointContents(pm_hullmodel, mid, node->children[side])
		== CONTENTS_SOLID)
	{
		Con_Printf("mid PointInHullSolid\n");
		return FALSE;
	}
#endif

	otherside = side ^ 1;

	if (PM_HullPointContents(hull, node->children[otherside], mid)
		!= CONTENTS_SOLID)
		// go past the node
		return PM_RecursiveHullCheck(hull, node->children[otherside], midf, p2f, mid, p2, trace);

	if (trace->allsolid)
		return FALSE;		// never got out of the solid area

//==================
// the other side of the node is solid, this is the impact point
//==================
	if (!side)
	{
		VectorCopy(plane->normal, trace->plane.normal);
		trace->plane.dist = plane->dist;
	}
	else
	{
		VectorSubtract(vec3_origin, plane->normal, trace->plane.normal);
		trace->plane.dist = -plane->dist;
	}

	while (PM_HullPointContents(hull, hull->firstclipnode, mid)
		== CONTENTS_SOLID)
	{ // shouldn't really happen, but does occasionally
		frac -= 0.05;
		if (frac < 0)
		{
			trace->fraction = midf;
			VectorCopy(mid, trace->endpos);
			Con_DPrintf("Trace backed up past 0.0.\n");
			return FALSE;
		}
		midf = p1f + (p2f - p1f) * frac;
		for (i = 0; i < 3; i++)
			mid[i] = p1[i] + frac * (p2[i] - p1[i]);
	}

	trace->fraction = midf;
	VectorCopy(mid, trace->endpos);

	return FALSE;
}

/*
=================
PM_HullForStudioModel

=================
*/
hull_t* PM_HullForStudioModel( model_t* pModel, vec_t* offset, float frame, int sequence, const vec_t* angles, const vec_t* origin, const byte* pcontroller, const byte* pblending, int* pNumHulls )
{
	vec3_t size;
	VectorSubtract(player_maxs[pmove.usehull], player_mins[pmove.usehull], size);
	VectorScale(size, 0.5, size);
	VectorCopy(vec3_origin, offset);
	return R_StudioHull(pModel, frame, sequence, angles, origin, size, pcontroller, pblending, pNumHulls);
}

/*
=================
PM_TestPlayerPosition

=================
*/
int PM_TestPlayerPosition( float* pos )
{
	int		i;
	physent_t* pe;
	vec3_t	mins, maxs, test;
	vec3_t	offset;
	hull_t* hull;
	qboolean rotated = FALSE;
	int		j, numhulls;

	// check if the player is currently inside a solid object
	g_Trace = PM_PlayerMove(pmove.origin, pmove.origin, PM_NORMAL);

	for (i = 0; i < pmove.numphysent; i++)
	{
		pe = &pmove.physents[i];
		if (pe->model && pe->solid == SOLID_NOT && pe->skin != 0)
			continue;

		VectorCopy(pe->origin, offset);
		numhulls = 1;

		// get the clipping hull
		if (pe->model)
		{
			switch (pmove.usehull)
			{
			case 0:
				// regular
				hull = &pe->model->hulls[1];
				break;
			case 1:
				// standing
				hull = &pe->model->hulls[3];
				break;
			case 2:
				// crouching
				hull = &pe->model->hulls[0];
				break;
			default:
				hull = &pe->model->hulls[1];
				break;
			}

			VectorSubtract(hull->clip_mins, player_mins[pmove.usehull], offset);
			VectorAdd(offset, pe->origin, offset);
		}
		else
		{
			if (pe->studiomodel && pe->studiomodel->type == mod_studio &&
				((pe->studiomodel->flags & STUDIO_TRACE_HITBOX) || pmove.usehull == 2))
			{
				hull = PM_HullForStudioModel(pe->studiomodel, offset, pe->frame, pe->sequence, pe->angles,
					pe->origin, pe->controller, pe->blending, &numhulls);
			}
			else
			{
				VectorSubtract(pe->mins, player_maxs[pmove.usehull], mins);
				VectorSubtract(pe->maxs, player_mins[pmove.usehull], maxs);
				hull = PM_HullForBox(mins, maxs);
			}
		}

		VectorSubtract(pos, offset, test);

		// Rotate the start and end into the model's frame of reference.
		rotated = pe->solid == SOLID_BSP && (pe->angles[0] != 0 || pe->angles[1] != 0 || pe->angles[2] != 0);
		if (rotated)
		{
			vec3_t forward, right, up;
			vec3_t temp;

			AngleVectors(pe->angles, forward, right, up);

			VectorCopy(test, temp);
			test[0] = DotProduct(forward, temp);
			test[1] = -DotProduct(right, temp);
			test[2] = DotProduct(up, temp);
		}

		if (numhulls == 1)
		{
			PM_global_testContents = PM_HullPointContents(hull, hull->firstclipnode, test);
			if (PM_global_testContents == CONTENTS_SOLID)
				return i;
		}
		else
		{
			for (j = 0; j < numhulls; j++)
			{
				PM_global_testContents = PM_HullPointContents(&hull[j], hull[j].firstclipnode, test);
				if (PM_global_testContents == CONTENTS_SOLID)
					return i;
			}
		}
	}

	return -1;
}

/*
=================
PM_PlayerMove

Perform a trace from a starting point to an ending point and determine what it intersects with
traceFlags - whether it should only check against the
world geometry or should ignore glass
=================
*/
pmtrace_t PM_PlayerMove( vec_t* start, vec_t* end, int traceFlags )
{
	int		i;
	pmtrace_t trace, total;
	vec3_t	offset;
	vec3_t	start_l, end_l;
	hull_t* hull;
	physent_t* pe;
	vec3_t	mins, maxs;
	qboolean rotated = FALSE;

	int		numhulls, j;

// fill in a default trace
	memset(&total, 0, sizeof(pmtrace_t));
	total.fraction = 1;
	total.ent = -1;
	VectorCopy(end, total.endpos);

	for (i = 0; i < pmove.numphysent; i++)
	{
		pe = &pmove.physents[i];

		if (pe->model && pe->solid == SOLID_NOT && pe->skin != 0)
			continue;

		// Ignore glass if needed
		if ((traceFlags & PM_GLASS_IGNORE) && pe->rendermode != kRenderNormal)
			continue;

		// PM_HullForEntity(ent, mins, maxs, offset);
		VectorCopy(pe->origin, offset);
		numhulls = 1;

	// get the clipping hull
		if (pe->model)
		{
			switch (pmove.usehull)
			{
			case 0:
				// regular
				hull = &pe->model->hulls[1];
				break;
			case 1:
				// standing
				hull = &pe->model->hulls[3];
				break;
			case 2:
				// crouching
				hull = &pe->model->hulls[0];
				break;
			default:
				hull = &pe->model->hulls[1];
				break;
			}

			VectorSubtract(hull->clip_mins, player_mins[pmove.usehull], offset);
			VectorAdd(offset, pe->origin, offset);
		}
		else
		{
			hull = NULL;

			if (pe->studiomodel)
			{
				if (traceFlags & PM_STUDIO_IGNORE)
					continue;

				if (pe->studiomodel->type == mod_studio &&
					(pe->studiomodel->flags & STUDIO_TRACE_HITBOX || (pmove.usehull == 2 && !(traceFlags & PM_STUDIO_BOX))))
				{
					hull = PM_HullForStudioModel(pe->studiomodel, offset, pe->frame, pe->sequence, pe->angles,
						pe->origin, pe->controller, pe->blending, &numhulls);
				}
			}

			if (!hull)
			{
				VectorSubtract(pe->mins, player_maxs[pmove.usehull], mins);
				VectorSubtract(pe->maxs, player_mins[pmove.usehull], maxs);
				hull = PM_HullForBox(mins, maxs);
			}
		}

		VectorSubtract(start, offset, start_l);
		VectorSubtract(end, offset, end_l);

		// Rotate the start and end into the model's frame of reference.
		rotated = pe->solid == SOLID_BSP && (pe->angles[0] != 0 || pe->angles[1] != 0 || pe->angles[2] != 0);
		if (rotated)
		{
			vec3_t forward, right, up;
			vec3_t temp;

			AngleVectors(pe->angles, forward, right, up);

			VectorCopy(start_l, temp);
			start_l[0] = DotProduct(forward, temp);
			start_l[1] = -DotProduct(right, temp);
			start_l[2] = DotProduct(up, temp);

			VectorCopy(end_l, temp);
			end_l[0] = DotProduct(forward, temp);
			end_l[1] = -DotProduct(right, temp);
			end_l[2] = DotProduct(up, temp);
		}

	// fill in a default trace
		memset(&trace, 0, sizeof(trace));
		trace.fraction = 1;
		trace.allsolid = TRUE;
//		trace.startsolid = TRUE;
		VectorCopy(end, trace.endpos);

		if (numhulls == 1)
		{
		// trace a line through the apropriate clipping hull
			PM_RecursiveHullCheck(hull, hull->firstclipnode, 0, 1, start_l, end_l, &trace);
		}
		else
		{
			int closest = 0;
			pmtrace_t testtrace;

			for (j = 0; j < numhulls; j++)
			{
				memset(&testtrace, 0, sizeof(testtrace));
				testtrace.fraction = 1;
				testtrace.allsolid = TRUE;
				VectorCopy(end, testtrace.endpos);

				PM_RecursiveHullCheck(&hull[j], hull[j].firstclipnode, 0, 1, start_l, end_l, &testtrace);

				if (j == 0 || testtrace.allsolid || testtrace.startsolid || testtrace.fraction < trace.fraction)
				{
					qboolean remember = (trace.startsolid == FALSE);
					memcpy(&trace, &testtrace, sizeof(trace));

					if (!remember)
						trace.startsolid = TRUE;

					closest = j;
				}

				trace.hitgroup = SV_HitgroupForStudioHull(closest);
			}
		}

		if (trace.allsolid)
			trace.startsolid = TRUE;
		if (trace.startsolid)
			trace.fraction = 0;

		if (trace.fraction != 1)
		{
			if (rotated)
			{
				vec3_t forward, right, up;
				vec3_t temp;

				AngleVectorsTranspose(pe->angles, forward, right, up);

				VectorCopy(trace.plane.normal, temp);
				trace.plane.normal[0] = DotProduct(forward, temp);
				trace.plane.normal[1] = DotProduct(right, temp);
				trace.plane.normal[2] = DotProduct(up, temp);
			}

			// Compute the end position of the trace.

			trace.endpos[0] = start[0] + trace.fraction * (end[0] - start[0]);
			trace.endpos[1] = start[1] + trace.fraction * (end[1] - start[1]);
			trace.endpos[2] = start[2] + trace.fraction * (end[2] - start[2]);
		}

	// did we clip the move?
		if (total.fraction > (double)trace.fraction)
		{
			total = trace;
			total.ent = i;
		}
	}

	return total;
}