// world.c -- world query functions

#include "quakedef.h"

static	areanode_t	sv_areanodes[AREA_NODES];
static	int			sv_numareanodes;

// TODO: Implement

int SV_HullPointContents( hull_t* hull, int num, vec_t* p );

// TODO: Implement

/*
===============
SV_HullForBsp

Forcing to select BSP hull
===============
*/
hull_t* SV_HullForBsp( edict_t* ent, const vec_t* mins, const vec_t* maxs, vec_t* offset )
{
	// TODO: Implement
	return NULL;
}

// TODO: Implement

/*
===============
SV_CreateAreaNode

===============
*/
areanode_t* SV_CreateAreaNode( int depth, vec_t* mins, vec_t* maxs )
{
	areanode_t* anode;
	vec3_t		size;
	vec3_t		mins1, maxs1, mins2, maxs2;

	anode = &sv_areanodes[sv_numareanodes];
	sv_numareanodes++;

	ClearLink(&anode->trigger_edicts);
	ClearLink(&anode->solid_edicts);

	if (depth == AREA_DEPTH)
	{
		anode->axis = -1;
		anode->children[0] = anode->children[1] = NULL;
		return anode;
	}

	VectorSubtract(maxs, mins, size);
	if (size[0] > size[1])
		anode->axis = 0;
	else
		anode->axis = 1;

	anode->dist = 0.5 * (maxs[anode->axis] + mins[anode->axis]);
	VectorCopy(mins, mins1);
	VectorCopy(mins, mins2);
	VectorCopy(maxs, maxs1);
	VectorCopy(maxs, maxs2);

	maxs1[anode->axis] = mins2[anode->axis] = anode->dist;

	anode->children[0] = SV_CreateAreaNode(depth + 1, mins2, maxs2);
	anode->children[1] = SV_CreateAreaNode(depth + 1, mins1, maxs1);

	return anode;
}

/*
===============
SV_ClearWorld

===============
*/
void SV_ClearWorld( void )
{
	// TODO: Implement

	memset(sv_areanodes, 0, sizeof(sv_areanodes));
	sv_numareanodes = 0;
	SV_CreateAreaNode(0, sv.worldmodel->mins, sv.worldmodel->maxs);
}

/*
===============
SV_UnlinkEdict

===============
*/
void SV_UnlinkEdict( edict_t* ent )
{
	if (!ent->area.prev)
		return;		// not linked in anywhere
	RemoveLink(&ent->area);
	ent->area.prev = ent->area.next = NULL;
}

/*
====================
SV_TouchLinks
====================
*/
void SV_TouchLinks( edict_t* ent, areanode_t* node )
{
	link_t* l, * next;
	edict_t* touch;
	model_t* pModel;

	// touch linked edicts
	for (l = node->trigger_edicts.next; l != &node->trigger_edicts; l = next)
	{
		next = l->next;
		touch = EDICT_FROM_AREA(l);
		if (touch == ent)
			continue;
		if (touch->v.solid != SOLID_TRIGGER)
			continue;
		if (ent->v.absmin[0] > touch->v.absmax[0]
			|| ent->v.absmin[1] > touch->v.absmax[1]
			|| ent->v.absmin[2] > touch->v.absmax[2]
			|| ent->v.absmax[0] < touch->v.absmin[0]
			|| ent->v.absmax[1] < touch->v.absmin[1]
			|| ent->v.absmax[2] < touch->v.absmin[2])
			continue;

		// check brush triggers accuracy
		pModel = sv.models[touch->v.modelindex];

		if (pModel && pModel->type == mod_brush)
		{
			// force to select bsp-hull
			int contents;
			hull_t* hull;
			vec3_t offset;
			vec3_t localPosition;

			hull = SV_HullForBsp(touch, ent->v.mins, ent->v.maxs, offset);

			// offset the test point appropriately for this hull
			VectorSubtract(ent->v.origin, offset, localPosition);

			// test hull for intersection with this model
			contents = SV_HullPointContents(hull, hull->firstclipnode, localPosition);
			if (contents != CONTENTS_SOLID)
				continue;
		}

		gGlobalVariables.time = sv.time;
		gEntityInterface.pfnTouch(touch, ent);
	}

// recurse down both sides
	if (node->axis == -1)
		return;

	if (ent->v.absmax[node->axis] > node->dist)
		SV_TouchLinks(ent, node->children[0]);
	if (ent->v.absmin[node->axis] < node->dist)
		SV_TouchLinks(ent, node->children[1]);
}


/*
===============
SV_FindTouchedLeafs

===============
*/
void SV_FindTouchedLeafs( edict_t *ent, mnode_t *node, int *topnode )
{
	mplane_t* splitplane;
	mleaf_t* leaf;
	int			sides;
	int			leafnum;

	if (node->contents == CONTENTS_SOLID)
		return;

// add an efrag if the node is a leaf

	if (node->contents < 0)
	{
		if (ent->num_leafs > (MAX_ENT_LEAFS - 1))
		{
			// continue counting leafs,
			// so we know how many it's overrun
			ent->num_leafs = (MAX_ENT_LEAFS + 1);
		}
		else
		{
			leaf = (mleaf_t*)node;
			leafnum = leaf - sv.worldmodel->leafs - 1;

			ent->leafnums[ent->num_leafs] = leafnum;
			ent->num_leafs++;
		}
		return;
	}

// NODE_MIXED

	splitplane = node->plane;
	sides = BOX_ON_PLANE_SIDE(ent->v.absmin, ent->v.absmax, splitplane);

	if (sides == 3 && *topnode == -1)
		*topnode = node - sv.worldmodel->nodes;

// recurse down the contacted sides
	if (sides & 1)
		SV_FindTouchedLeafs(ent, node->children[0], topnode);

	if (sides & 2)
		SV_FindTouchedLeafs(ent, node->children[1], topnode);
}

/*
===============
SV_LinkEdict

===============
*/
void SV_LinkEdict( edict_t *ent, qboolean touch_triggers )
{
	areanode_t* node;
	int topnode;

	if (ent->area.prev)
		SV_UnlinkEdict(ent);	// unlink from old position

	if (ent == sv.edicts)
		return;		// don't add the world

	if (ent->free)
		return;

	// set the abs box
	gEntityInterface.pfnSetAbsBox(ent);

	if (ent->v.movetype == MOVETYPE_FOLLOW && ent->v.aiment)
	{
		ent->num_leafs = ent->v.aiment->num_leafs;
		memcpy(ent->leafnums, ent->v.aiment->leafnums, sizeof(ent->leafnums));
	}
	else
	{
		topnode = -1;

		// link to PVS leafs
		ent->num_leafs = 0;

		if (ent->v.modelindex)
			SV_FindTouchedLeafs(ent, sv.worldmodel->nodes, &topnode);

		if (ent->num_leafs > MAX_ENT_LEAFS)
		{
			ent->num_leafs = -1; // so we use headnode instead
			ent->leafnums[0] = topnode;
		}
	}

	// ignore non-solid bodies
	if (ent->v.solid == SOLID_NOT && ent->v.skin >= CONTENTS_EMPTY)
		return;

	if (ent->v.solid == SOLID_BSP && !sv.models[ent->v.modelindex] && !strlen(&pr_strings[ent->v.model]))
	{
		Con_DPrintf("Inserted %s with no model\n", &pr_strings[ent->v.classname]);
		return;
	}

// find the first node that the ent's box crosses
	node = sv_areanodes;
	while (1)
	{
		if (node->axis == -1)
			break;
		if (ent->v.absmin[node->axis] > node->dist)
			node = node->children[0];
		else if (ent->v.absmax[node->axis] < node->dist)
			node = node->children[1];
		else
			break;		// crosses the node
	}

// link it in

	if (ent->v.solid == SOLID_TRIGGER)
		InsertLinkBefore(&ent->area, &node->trigger_edicts);
	else
		InsertLinkBefore(&ent->area, &node->solid_edicts);

// if touch_triggers, touch all entities at this node and decend for more
	if (touch_triggers)
		SV_TouchLinks(ent, sv_areanodes);
}

// TODO: Implement

/*
===============================================================================

POINT TESTING IN HULLS

===============================================================================
*/

#if	!id386

/*
===============
SV_HullPointContents

===============
*/
int SV_HullPointContents( hull_t* hull, int num, const vec_t* p )
{
	float		d;
	dclipnode_t* node;
	mplane_t* plane;

	while (num >= 0)
	{
		if (num < hull->firstclipnode || num > hull->lastclipnode)
			Sys_Error("SV_HullPointContents: bad node number");

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

#endif	// !id386

// TODO: Implement

/*
===============================================================================

LINE TESTING IN HULLS

===============================================================================
*/

// 1/32 epsilon to keep floating point happy
#define	DIST_EPSILON	(0.03125)

/*
==================
SV_RecursiveHullCheck

==================
*/
qboolean SV_RecursiveHullCheck( hull_t* hull, int num, float p1f, float p2f, vec_t* p1, vec_t* p2, trace_t* trace )
{
	dclipnode_t	*node;
	mplane_t	*plane;
	float		t1, t2;
	float		frac;
	int			i;
	vec3_t		mid;
	int			side;
	float		midf;

// check for empty
	if (num < 0)
	{
		if (num != CONTENTS_SOLID)
		{
			trace->allsolid = FALSE;
			if (num == CONTENTS_EMPTY)
				trace->inopen = TRUE;
			else if (num != CONTENTS_TRANSLUCENT)
				trace->inwater = TRUE;
		}
		else
			trace->startsolid = TRUE;

		return TRUE;		// empty
	}

	if (num < hull->firstclipnode || num > hull->lastclipnode || !hull->planes)
		Sys_Error("SV_RecursiveHullCheck: bad node number");

//
// find the point distances
//
	node = hull->clipnodes + num;
	plane = hull->planes + hull->clipnodes[num].planenum;

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
		return SV_RecursiveHullCheck(hull, node->children[0], p1f, p2f, p1, p2, trace);
	if (t1 < 0 && t2 < 0)
		return SV_RecursiveHullCheck(hull, node->children[1], p1f, p2f, p1, p2, trace);
#else
	if ((t1 >= DIST_EPSILON && t2 >= DIST_EPSILON) || (t2 > t1 && t1 >= 0))
		return SV_RecursiveHullCheck(hull, node->children[0], p1f, p2f, p1, p2, trace);
	if ((t1 <= -DIST_EPSILON && t2 <= -DIST_EPSILON) || (t2 < t1 && t1 <= 0))
		return SV_RecursiveHullCheck(hull, node->children[1], p1f, p2f, p1, p2, trace);
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
	if (!SV_RecursiveHullCheck(hull, node->children[side], p1f, midf, p1, mid, trace))
		return FALSE;

#ifdef PARANOID
	if (SV_HullPointContents(sv_hullmodel, mid, node->children[side])
	== CONTENTS_SOLID)
	{
		Con_Printf("mid PointInHullSolid\n");
		return FALSE;
	}
#endif

	// NOTE: this recursion can not be optimized because mid would need to be duplicated on a stack
	if (SV_HullPointContents(hull, node->children[side ^ 1], mid) != CONTENTS_SOLID)
	{
		// go past the node
		return SV_RecursiveHullCheck(hull, node->children[side ^ 1], midf, p2f, mid, p2, trace);
	}

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

	while (SV_HullPointContents(hull, hull->firstclipnode, mid)
			== CONTENTS_SOLID)
	{ // shouldn't really happen, but does occasionally
		frac -= 0.1;
		if (frac < 0)
		{
			trace->fraction = midf;
			VectorCopy(mid, trace->endpos);
			Con_DPrintf("backup past 0\n");
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
===============
SV_ClipMoveToEntity

Handles selection or creation of a clipping hull, and offseting (and
eventually rotation) of the end points
===============
*/
trace_t SV_ClipMoveToEntity( edict_t* ent, const vec_t* start, const vec_t* mins, const vec_t* maxs, const vec_t* end )
{
	// TODO: Implement
	trace_t t = { 0 };

	return t;
}

// TODO: Implement

/*
==================
SV_MoveBounds
==================
*/
void SV_MoveBounds( const vec_t* start, const vec_t* mins, const vec_t* maxs, const vec_t* end, vec_t* boxmins, vec_t* boxmaxs )
{
	// TODO: Implement
}

// TODO: Implement

/*
===============
SV_Move
===============
*/
trace_t SV_Move( const vec_t *start, const vec_t *mins, const vec_t *maxs, const vec_t *end, int type, edict_t *passedict, qboolean monsterClipBrush )
{
	// TODO: Implement
	trace_t trace;
	memset(&trace, 0, sizeof(trace));
	return trace;
}

// TODO: Implement