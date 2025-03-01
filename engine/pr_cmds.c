#include "quakedef.h"
#include "sv_proto.h"
#include "cmodel.h"
#include "decal.h"
#include "pr_cmds.h"

/*
===============================================================================

						BUILT-IN FUNCTIONS

===============================================================================
*/

/*
==============
PF_makevectors

Writes new values for v_forward, v_up, and v_right based on angles
makevectors(vector)
==============
*/
void PF_makevectors_I( const float* rgflVector )
{
	AngleVectors(rgflVector, gGlobalVariables.v_forward, gGlobalVariables.v_right, gGlobalVariables.v_up);
}

float PF_Time( void )
{
	return Sys_FloatTime();
}

/*
=================
PF_setorigin

This is the only valid way to move an object without using the physics of the world (setting velocity and waiting).  Directly changing origin will not set internal links correctly, so clipping would be messed up.  This should be called when an object is spawned, and then only if it is teleported.

setorigin (entity, origin)
=================
*/
void PF_setorigin_I( edict_t* e, const float* org )
{
	VectorCopy(org, e->v.origin);
	SV_LinkEdict(e, FALSE);
}


void SetMinMaxSize( edict_t* e, float *min, float *max, qboolean rotate )
{
	float* angles;
	vec3_t	rmin, rmax;
	float	bounds[2][3];
	float	xvector[2], yvector[2];
	float	a;
	vec3_t	base, transformed;
	int		i, j, k, l;

	for (i = 0; i < 3; i++)
		if (min[i] > max[i])
			Host_Error("backwards mins/maxs");

	rotate = FALSE;		// FIXME: implement rotation properly again

	if (!rotate)
	{
		VectorCopy(min, rmin);
		VectorCopy(max, rmax);
	}
	else
	{
	// find min / max for rotations
		angles = e->v.angles;

		a = angles[1] / 180 * M_PI;

		xvector[0] = cos(a);
		xvector[1] = sin(a);
		yvector[0] = -sin(a);
		yvector[1] = cos(a);

		VectorCopy(min, bounds[0]);
		VectorCopy(max, bounds[1]);

		rmin[0] = rmin[1] = rmin[2] = 9999;
		rmax[0] = rmax[1] = rmax[2] = -9999;

		for (i = 0; i <= 1; i++)
		{
			base[0] = bounds[i][0];
			for (j = 0; j <= 1; j++)
			{
				base[1] = bounds[j][1];
				for (k = 0; k <= 1; k++)
				{
					base[2] = bounds[k][2];

					// transform the point
					transformed[0] = xvector[0] * base[0] + yvector[0] * base[1];
					transformed[1] = xvector[1] * base[0] + yvector[1] * base[1];
					transformed[2] = base[2];

					for (l = 0; l < 3; l++)
					{
						if (transformed[l] < rmin[l])
							rmin[l] = transformed[l];
						if (transformed[l] > rmax[l])
							rmax[l] = transformed[l];
					}
				}
			}
		}
	}

// set derived values
	VectorCopy(min, e->v.mins);
	VectorCopy(max, e->v.maxs);
	VectorSubtract(max, min, e->v.size);

	SV_LinkEdict(e, FALSE);
}

/*
=================
PF_setsize

the size box is rotated by the current angle

setsize (entity, minvector, maxvector)
=================
*/
void PF_setsize_I( edict_t* e, float* rgflMin, float* rgflMax )
{
	SetMinMaxSize(e, rgflMin, rgflMax, FALSE);
}


/*
===============
PF_setmodel_I

setmodel(entity, model)
===============
*/
void PF_setmodel_I( edict_t* e, const char* m )
{
	int		i;
	char** check;
	model_t* mod;

// check to see if model was properly precached
	for (i = 0, check = sv.model_precache; *check; i++, check++)
		if (!strcmp(*check, m))
			break;

	if (!*check)
		Host_Error("no precache: %s\n", m);


	e->v.model = m - pr_strings;
	e->v.modelindex = i; // SV_ModelIndex(m);

	mod = sv.models[(int)e->v.modelindex];  // Mod_ForName(m, TRUE);

	if (mod)
		SetMinMaxSize(e, mod->mins, mod->maxs, TRUE);
	else
		SetMinMaxSize(e, vec3_origin, vec3_origin, TRUE);
}

int PF_modelindex( const char* pstr )
{
	return SV_ModelIndex((char*)pstr);
}

int ModelFrames( int modelIndex )
{
	model_t* pModel;

	if (modelIndex <= 0 || modelIndex >= MAX_MODELS)
	{
		Con_DPrintf("Bad sprite index!\n");
		return 1;
	}

	pModel = sv.models[modelIndex];
	return ModelFrameCount(pModel);
}

/*
=================
PF_bprint

broadcast print to everyone on server

bprint(value)
=================
*/
void PF_bprint( char* s )
{
	SV_BroadcastPrintf("%s", s);
}

/*
=================
PF_sprint

single print to a specific client

sprint(clientent, value)
=================
*/
void PF_sprint( char* s, int entnum )
{
	client_t* client;

	if (entnum < 1 || entnum > svs.maxclients)
	{
		Con_Printf("tried to sprint to a non-client\n");
		return;
	}

	client = &svs.clients[entnum - 1];

	MSG_WriteChar(&client->netchan.message, svc_print);
	MSG_WriteString(&client->netchan.message, s);
}

/*
=================
ClientPrintf

Sends a message to the client console
print_chat outputs to the console, just as print_console
=================
*/
void ClientPrintf( edict_t* pEdict, PRINT_TYPE ptype, const char* szMsg )
{
	client_t* client;
	int entnum;

	entnum = NUM_FOR_EDICT(pEdict);
	if (entnum < 1 || entnum > svs.maxclients)
	{
		Con_Printf("tried to sprint to a non-client\n");
		return;
	}

	client = &svs.clients[entnum - 1];

	switch (ptype)
	{
	case print_center:
		MSG_WriteChar(&client->netchan.message, svc_centerprint);
		MSG_WriteString(&client->netchan.message, (char*)szMsg);
		break;
	case print_chat:
	case print_console:
		MSG_WriteByte(&client->netchan.message, svc_print);
		MSG_WriteString(&client->netchan.message, (char*)szMsg);
		break;
	default:
		Con_Printf("invalid PRINT_TYPE %i\n", ptype);
		break;
	}
}

/*
=================
PF_vectoyaw_I

Converts a direction vector to a yaw angle
=================
*/
float PF_vectoyaw_I( const float* rgflVector )
{
	float yaw = 0;

	if (rgflVector[1] == 0 && rgflVector[0] == 0)
		return 0;

	yaw = (int)(atan2(rgflVector[1], rgflVector[0]) * 180 / M_PI);
	if (yaw < 0)
		yaw += 360;

	return yaw;
}


/*
=================
PF_vectoangles

vector vectoangles(vector)
=================
*/
void PF_vectoangles_I( const float* rgflVectorIn, float* rgflVectorOut )
{
	VectorAngles(rgflVectorIn, rgflVectorOut);
}

/*
=================
PF_particle_I

particle(origin, color, count)
=================
*/
void PF_particle_I( const float* org, const float* dir, float color, float count )
{
	SV_StartParticle(org, dir, color, count);
}


/*
=================
PF_ambientsound

=================
*/
void PF_ambientsound_I( edict_t* entity, float* pos, const char* samp, float vol, float attenuation, int fFlags, int pitch )
{
	char** check;
	int			i, soundnum;
	int			ent;
	sizebuf_t* pout;

	if (samp[0] == '!')
	{
		fFlags |= SND_SENTENCE;

		soundnum = atoi(samp + 1);
		if (soundnum >= CVOXFILESENTENCEMAX)
		{
			Con_Printf("invalid sentence number: %s", samp + 1);
			return;
		}
	}
	else
	{// check to see if samp was properly precached
		for (soundnum = 0, check = sv.sound_precache; *check; check++, soundnum++)
			if (!strcmp(*check, samp))
				break;

		if (!*check)
		{
			Con_Printf("no precache: %s\n", samp);
			return;
		}
	}

	ent = NUM_FOR_EDICT(entity);

	pout = &sv.signon;
	if (!(fFlags & SND_SPAWNING))
		pout = &sv.datagram;

// add an svc_spawnambient command to the level signon packet

	MSG_WriteByte(pout, svc_spawnstaticsound);
	for (i = 0; i < 3; i++)
		MSG_WriteCoord(pout, pos[i]);

	MSG_WriteShort(pout, soundnum);

	MSG_WriteByte(pout, vol * 255);
	MSG_WriteByte(pout, attenuation * 64);
	MSG_WriteShort(pout, ent);
	MSG_WriteByte(pout, pitch);
	MSG_WriteByte(pout, fFlags);
}

/*
=================
PF_sound_I

Each entity can have eight independant sound sources, like voice,
weapon, feet, etc.

Channel 0 is an auto-allocate channel, the others override anything
allready running on that entity/channel pair.

An attenuation of 0 will play full volume everywhere in the level.
Larger attenuations will drop off.

=================
*/
void PF_sound_I( edict_t* entity, int channel, const char* sample, float volume, float attenuation, int fFlags, int pitch )
{
	if (volume < 0 || volume > 255)
		Sys_Error("SV_StartSound: volume = %i", (int)volume);

	if (attenuation < 0 || attenuation > 4)
		Sys_Error("SV_StartSound: attenuation = %f", attenuation);

	if (channel < CHAN_AUTO || channel > CHAN_NETWORKVOICE_BASE)
		Sys_Error("SV_StartSound: channel = %i", channel);

	if (pitch < 0 || pitch > 255)
		Sys_Error("SV_StartSound: pitch = %i", pitch);

	SV_StartSound(entity, channel, sample, volume * 255, attenuation, fFlags, pitch);
}

/*
=================
PF_traceline_Shared

Used for use tracing and shot targeting
Traces are blocked by bbox and exact bsp entityes, and also slide box entities
if the tryents flag is set.

traceline (vector1, vector2, nomonsters, ent)
=================
*/
void PF_traceline_Shared( const float* v1, const float* v2, int nomonsters, edict_t* ent )
{
	trace_t trace;
	trace = SV_Move(v1, vec3_origin, vec3_origin, v2, nomonsters, ent, FALSE);

	gGlobalVariables.trace_flags = 0;

	SV_SetGlobalTrace(&trace);
}

void PF_traceline_DLL( const float* v1, const float* v2, int fNoMonsters, edict_t* pentToSkip, TraceResult* ptr )
{
	PF_traceline_Shared(v1, v2, fNoMonsters, pentToSkip ? pentToSkip : &sv.edicts[0]);
	ptr->fAllSolid = gGlobalVariables.trace_allsolid;
	ptr->fStartSolid = gGlobalVariables.trace_startsolid;
	ptr->fInOpen = gGlobalVariables.trace_inopen;
	ptr->fInWater = gGlobalVariables.trace_inwater;
	ptr->flFraction = gGlobalVariables.trace_fraction;
	ptr->flPlaneDist = gGlobalVariables.trace_plane_dist;
	ptr->pHit = gGlobalVariables.trace_ent;
	VectorCopy(gGlobalVariables.trace_endpos, ptr->vecEndPos);
	VectorCopy(gGlobalVariables.trace_plane_normal, ptr->vecPlaneNormal);
	ptr->iHitgroup = gGlobalVariables.trace_hitgroup;
}

vec_t gHullMins[4][3] = {
	{ 0.0, 0.0, 0.0 },
	{ -16.0, -16.0, -36.0 },
	{ -32.0, -32.0, -32.0 },
	{ -16.0, -16.0, -18.0 },
};

vec_t gHullMaxs[4][3] = {
	{ 0.0, 0.0, 0.0 },
	{ 16.0, 16.0, 36.0 },
	{ 32.0, 32.0, 32.0 },
	{ 16.0, 16.0, 18.0 },
};

void TraceHull( const float* v1, const float* v2, int fNoMonsters, int hullNumber, edict_t* pentToSkip, TraceResult* ptr )
{
	trace_t trace;

	if (hullNumber < 0 || hullNumber > 3)
		hullNumber = 0;

	trace = SV_Move(v1, gHullMins[hullNumber], gHullMaxs[hullNumber], v2, fNoMonsters, pentToSkip, FALSE);

	ptr->fAllSolid = trace.allsolid;
	ptr->fStartSolid = trace.startsolid;
	ptr->fInOpen = trace.inopen;
	ptr->fInWater = trace.inwater;
	ptr->flFraction = trace.fraction;
	ptr->flPlaneDist = trace.plane.dist;
	ptr->pHit = trace.ent;
	ptr->iHitgroup = trace.hitgroup;
	VectorCopy(trace.endpos, ptr->vecEndPos);
	VectorCopy(trace.plane.normal, ptr->vecPlaneNormal);
}

void TraceSphere( const float* v1, const float* v2, int fNoMonsters, float radius, edict_t* pentToSkip, TraceResult* ptr )
{
	Sys_Error("TraceSphere not yet implemented!\n");
}

void TraceModel( const float* v1, const float* v2, edict_t* pent, TraceResult* ptr )
{
	trace_t trace;
	int oldSolid, oldMovetype;
	model_t* pmodel;

	pmodel = sv.models[pent->v.modelindex];
	if (pmodel && pmodel->type == mod_brush)
	{
		oldSolid = pent->v.solid;
		oldMovetype = pent->v.movetype;
		pent->v.solid = SOLID_BSP;
		pent->v.movetype = MOVETYPE_PUSH;
	}

	trace = SV_ClipMoveToEntity(pent, v1, vec3_origin, vec3_origin, v2);

	if (pmodel && pmodel->type == mod_brush)
	{
		pent->v.solid = oldSolid;
		pent->v.movetype = oldMovetype;
	}

	ptr->fAllSolid = trace.allsolid;
	ptr->fStartSolid = trace.startsolid;
	ptr->fInOpen = trace.inopen;
	ptr->fInWater = trace.inwater;
	ptr->flFraction = trace.fraction;
	ptr->flPlaneDist = trace.plane.dist;
	ptr->pHit = trace.ent;
	ptr->iHitgroup = trace.hitgroup;
	VectorCopy(trace.endpos, ptr->vecEndPos);
	VectorCopy(trace.plane.normal, ptr->vecPlaneNormal);
}

/*
=================
SurfaceAtPoint

Returns surface instance by specified position
=================
*/
msurface_t* SurfaceAtPoint( model_t* pModel, mnode_t* node, vec_t* start, vec_t* end )
{
	float		front, back, frac;
	int			side;
	mplane_t* plane;
	vec3_t		mid;
	msurface_t* surf;
	int			s, t, ds, dt;
	int			i;
	mtexinfo_t* tex;

	if (node->contents < 0)
		return NULL;

	plane = node->plane;
	front = DotProduct(start, plane->normal) - plane->dist;
	back = DotProduct(end, plane->normal) - plane->dist;

	// test the front side first
	side = front < 0;

	// If they're both on the same side of the plane, don't bother to split
	// just check the appropriate child
	if ((back < 0.0) == side)
		return SurfaceAtPoint(pModel, node->children[side], start, end);

	// calculate mid point
	frac = front / (front - back);

	mid[0] = start[0] + (end[0] - start[0]) * frac;
	mid[1] = start[1] + (end[1] - start[1]) * frac;
	mid[2] = start[2] + (end[2] - start[2]) * frac;

	// go down front side
	surf = SurfaceAtPoint(pModel, node->children[side], start, mid);
	if (surf)
		return surf;

	if ((back < 0.0) == side)
		return NULL;

	// check for impact on this node
	for (i = 0; i < node->numsurfaces; i++)
	{
		surf = &pModel->surfaces[node->firstsurface + i];
		tex = surf->texinfo;

		s = DotProduct(mid, tex->vecs[0]) + tex->vecs[0][3];
		t = DotProduct(mid, tex->vecs[1]) + tex->vecs[1][3];

		if (s < surf->texturemins[0] || t < surf->texturemins[1])
			continue;

		ds = s - surf->texturemins[0];
		dt = t - surf->texturemins[1];

		// Check this surface to see if there's an intersection
		if (ds > surf->extents[0] || dt > surf->extents[1])
			continue;

		return surf;
	}

	// go down back side
	surf = SurfaceAtPoint(pModel, node->children[!side], mid, end);
	return surf;
}

const char* TraceTexture( edict_t* pTextureEntity, const float* v1, const float* v2 )
{
	model_t* pmodel;
	msurface_t* psurf;
	int		firstnode;
	vec3_t	start, end;

	firstnode = 0;

	if (pTextureEntity)
	{
		vec3_t offset;
		hull_t* phull; 
		int modelindex;

		modelindex = pTextureEntity->v.modelindex;
		pmodel = sv.models[modelindex];
		if (!pmodel || pmodel->type != mod_brush)
			return NULL;

		phull = SV_HullForBsp(pTextureEntity, vec3_origin, vec3_origin, offset);
		VectorSubtract(v1, offset, start);
		VectorSubtract(v2, offset, end);

		firstnode = phull->firstclipnode;

		if (pTextureEntity->v.angles[0] != 0 || pTextureEntity->v.angles[1] != 0 || pTextureEntity->v.angles[2] != 0)
		{
			vec3_t forward, right, up;
			vec3_t temp;

			AngleVectors(pTextureEntity->v.angles, forward, right, up);

			VectorCopy(start, temp);
			start[0] = DotProduct(forward, temp);
			start[1] = -DotProduct(right, temp);
			start[2] = DotProduct(up, temp);

			VectorCopy(end, temp);
			end[0] = DotProduct(forward, temp);
			end[1] = -DotProduct(right, temp);
			end[2] = DotProduct(up, temp);
		}
	}
	else
	{
		pmodel = sv.worldmodel;
		VectorCopy(v1, start);
		VectorCopy(v2, end);
	}

	if (pmodel && pmodel->type == mod_brush)
	{
		if (pmodel->nodes)
		{
			psurf = SurfaceAtPoint(pmodel, &pmodel->nodes[firstnode], start, end);
			if (psurf)
				return psurf->texinfo->texture->name;
		}
	}
	return NULL;
}

void PF_TraceToss_Shared( edict_t* ent, edict_t* ignore )
{
	trace_t trace;
	trace = SV_Trace_Toss(ent, ignore);

	SV_SetGlobalTrace(&trace);
}

// Set trace data for globalvars_t
void SV_SetGlobalTrace( trace_t* ptrace )
{
	gGlobalVariables.trace_allsolid = ptrace->allsolid;
	gGlobalVariables.trace_startsolid = ptrace->startsolid;
	gGlobalVariables.trace_fraction = ptrace->fraction;
	gGlobalVariables.trace_inwater = ptrace->inwater;
	gGlobalVariables.trace_inopen = ptrace->inopen;
	VectorCopy(ptrace->endpos, gGlobalVariables.trace_endpos);
	VectorCopy(ptrace->plane.normal, gGlobalVariables.trace_plane_normal);
	gGlobalVariables.trace_plane_dist = ptrace->plane.dist;
	gGlobalVariables.trace_ent = ptrace->ent ? ptrace->ent : &sv.edicts[0];
	gGlobalVariables.trace_hitgroup = ptrace->hitgroup;
}

// This simulates tossing the entity using its current origin, velocity, angular velocity, angles and gravity
void PF_TraceToss_DLL( edict_t* pent, edict_t* pentToIgnore, TraceResult* ptr )
{
	PF_TraceToss_Shared(pent, pentToIgnore ? pentToIgnore : &sv.edicts[0]);
	ptr->fAllSolid = gGlobalVariables.trace_allsolid;
	ptr->fStartSolid = gGlobalVariables.trace_startsolid;
	ptr->fInOpen = gGlobalVariables.trace_inopen;
	ptr->fInWater = gGlobalVariables.trace_inwater;
	ptr->flFraction = gGlobalVariables.trace_fraction;
	ptr->pHit = gGlobalVariables.trace_ent;
	VectorCopy(gGlobalVariables.trace_endpos, ptr->vecEndPos);
	VectorCopy(gGlobalVariables.trace_plane_normal, ptr->vecPlaneNormal);
	ptr->flPlaneDist = gGlobalVariables.trace_plane_dist;
	ptr->iHitgroup = gGlobalVariables.trace_hitgroup;
}

int TraceMonsterHull( edict_t* pEdict, const float* v1, const float* v2, int fNoMonsters, edict_t* pentToSkip, TraceResult* ptr )
{
	trace_t trace;
	qboolean monsterClip;

	monsterClip = (pEdict->v.flags & FL_MONSTERCLIP) ? TRUE : FALSE;
	trace = SV_Move(v1, pEdict->v.mins, pEdict->v.maxs, v2, fNoMonsters, pentToSkip, monsterClip);

	if (ptr)
	{
		ptr->fAllSolid = trace.allsolid;
		ptr->fStartSolid = trace.startsolid;
		ptr->fInOpen = trace.inopen;
		ptr->fInWater = trace.inwater;
		ptr->flPlaneDist = trace.plane.dist;
		ptr->pHit = trace.ent;
		ptr->iHitgroup = trace.hitgroup;
		ptr->flFraction = trace.fraction;
		VectorCopy(trace.endpos, ptr->vecEndPos);
		VectorCopy(trace.plane.normal, ptr->vecPlaneNormal);
	}

	if (trace.allsolid || trace.fraction != 1.0)
		return TRUE;

	return FALSE;
}

//============================================================================

byte	checkpvs[MAX_MAP_LEAFS / 8];

int PF_newcheckclient( int check )
{
	int		i;
	byte* pvs;
	edict_t* ent;
	mleaf_t* leaf;
	vec3_t	org;

// cycle to the next one

	if (check < 1)
		check = 1;
	if (check > svs.maxclients)
		check = svs.maxclients;

	if (check == svs.maxclients)
		i = 1;
	else
		i = check + 1;

	for (; ; i++)
	{
		if (i == svs.maxclients + 1)
			i = 1;

		ent = &sv.edicts[i];

		if (i == check)
			break;	// didn't find anything else

		if (ent->free)
			continue;
		if (!ent->pvPrivateData)
			continue;
		if (ent->v.flags & FL_NOTARGET)
			continue;

	// anything that is a client, or has a client as an enemy
		break;
	}

// get the PVS for the entity
	VectorAdd(ent->v.view_ofs, ent->v.origin, org);
	leaf = Mod_PointInLeaf(org, sv.worldmodel);
	pvs = Mod_LeafPVS(leaf, sv.worldmodel);
	memcpy(checkpvs, pvs, (sv.worldmodel->numleafs + 7) >> 3);

	return i;
}

/*
=================
PF_checkclient_I

Returns a client (or object that has a client enemy) that would be a
valid target.

If there are more than one valid options, they are cycled each frame

If (self.origin + self.viewofs) is not in the PVS of the current target,
it is not returned at all.

name checkclient ()
=================
*/
#define	MAX_CHECK	16
int c_invis, c_notvis;
edict_t* PF_checkclient_I( edict_t* pEdict )
{
	edict_t* ent;
	mleaf_t* leaf;
	int		l;
	vec3_t	view;

// find a new check if on a new frame
	if (sv.time - sv.lastchecktime >= 0.1)
	{
		sv.lastcheck = PF_newcheckclient(sv.lastcheck);
		sv.lastchecktime = sv.time;
	}

// return check if it might be visible
	ent = &sv.edicts[sv.lastcheck];
	if (ent->free || !ent->pvPrivateData)
	{
		return &sv.edicts[0];
	}

// if current entity can't possibly see the check entity, return 0
	VectorAdd(pEdict->v.origin, pEdict->v.view_ofs, view);
	leaf = Mod_PointInLeaf(view, sv.worldmodel);
	l = (leaf - sv.worldmodel->leafs) - 1;
	if (l < 0 || !((1 << (l & 7)) & checkpvs[l >> 3]))
	{
		c_notvis++;
		return &sv.edicts[0];
	}

// might be able to see it
	c_invis++;
	return ent;
}

/*
=================
PVSNode

Check the box in PVS and get node
=================
*/
mnode_t* PVSNode( mnode_t* node, vec_t* emins, vec_t* emaxs )
{
	mplane_t* splitplane;
	int sides;
	mnode_t* splitNode;

	if (node->visframe != r_visframecount)
		return NULL;

	if (node->contents < 0)
		return node->contents != CONTENT_SOLID ? node : NULL;

	splitplane = node->plane;

	sides = BOX_ON_PLANE_SIDE(emins, emaxs, splitplane);

	if (sides & 1)
	{
		splitNode = PVSNode(node->children[0], emins, emaxs);
		if (splitNode)
			return splitNode;
	}

	if (sides & 2)
		return PVSNode(node->children[1], emins, emaxs);

	return NULL;
}

/*
=================
PVSMark

Mark the leaves and nodes that are in the PVS for the specified leaf
=================
*/
void PVSMark( model_t* pmodel, byte* ppvs )
{
	mnode_t* node;
	int		i;

	r_visframecount++;

	for (i = 0; i < pmodel->numleafs; i++)
	{
		if ((1 << (i & 7)) & ppvs[i >> 3])
		{
			node = (mnode_t*)&pmodel->leafs[i + 1];
			do
			{
				if (node->visframe == r_visframecount)
					break;
				node->visframe = r_visframecount;
				node = node->parent;
			} while (node);
		}
	}
}

void R_MarkLeaves( void );

/*
=================
PVSFindEntities

Finds an entity chain in the PVS
=================
*/
edict_t* PVSFindEntities( edict_t* pplayer )
{
	int		i;
	edict_t* pent, * pchain, * pentPVS;
	vec3_t	org;
	byte* ppvs;
	mleaf_t* pleaf;

	VectorAdd(pplayer->v.view_ofs, pplayer->v.origin, org);

	pleaf = Mod_PointInLeaf(org, sv.worldmodel);
	ppvs = Mod_LeafPVS(pleaf, sv.worldmodel);

	PVSMark(sv.worldmodel, ppvs);
	pchain = sv.edicts;

	for (i = 1; i < sv.num_edicts; i++)
	{
		pent = &sv.edicts[i];
		if (pent->free)
			continue;

		if (pent->v.movetype == MOVETYPE_FOLLOW && pent->v.aiment)
			pentPVS = pent->v.aiment;
		else
			pentPVS = pent;

		if (PVSNode(sv.worldmodel->nodes, pentPVS->v.absmin, pentPVS->v.absmax))
		{
			pent->v.chain = pchain;
			pchain = pent;
		}
	}

	if (cl.worldmodel)
	{
		r_oldviewleaf = NULL;
		R_MarkLeaves();
	}

	return pchain;
}

//============================================================================

qboolean ValidCmd( const char* pCmd )
{
	int len;

	len = strlen(pCmd);
	return (len != 0) && (pCmd[len - 1] == '\n' || pCmd[len - 1] == ';');
}

/*
=================
PF_stuffcmd_I

Sends text over to the client's execution buffer

stuffcmd (clientent, value)
=================
*/
void PF_stuffcmd_I( edict_t* pEdict, char* szFmt, ... )
{
	int		entnum;
	client_t* old;
	va_list	argptr;
	static char szOut[1024];

	entnum = NUM_FOR_EDICT(pEdict);

	va_start(argptr, szFmt);
	vsprintf(szOut, szFmt, argptr);
	va_end(argptr);

	if (entnum < 1 || entnum > svs.maxclients)
	{
		Con_Printf("\n!!!\n\nStuffCmd:  Some entity tried to stuff '%s' to console "
			"buffer of entity %i when maxclients was set to %i, ignoring\n\n",
			szOut, entnum, svs.maxclients);
		return;
	}
	
	if (!ValidCmd(szOut))
	{
		Con_Printf("Tried to stuff bad command %s\n", szOut);
		return;
	}
	
	old = host_client;
	host_client = &svs.clients[entnum - 1];
	Host_ClientCommands("%s", szOut);
	host_client = old;
}

/*
=================
PF_localcmd_I

Sends text over to the client's execution buffer

localcmd (string)
=================
*/
void PF_localcmd_I( char* str )
{
	if (!ValidCmd(str))
	{
		Con_Printf("Error, bad server command %s\n", str);
		return;
	}

	Cbuf_AddText(str);	
}

/*
=================
FindEntityInSphere

Try to find an entity in a sphere
=================
*/
edict_t* FindEntityInSphere( edict_t* pEdictStartSearchAfter, const float* org, float rad )
{
	edict_t* ent;
	float	eorg;
	int		e, i, j;
	float	distSquared;

	if (pEdictStartSearchAfter)
		e = NUM_FOR_EDICT(pEdictStartSearchAfter);
	else
		e = 0;

	for (i = e + 1; i < sv.num_edicts; i++)
	{
		ent = &sv.edicts[i];
		if (ent->free || !ent->v.classname)
			continue;

		if (i <= svs.maxclients && !svs.clients[i - 1].active)
			continue;

		distSquared = 0.0;
		for (j = 0; j < 3 && distSquared <= (rad * rad); j++)
		{
			if (org[j] < ent->v.absmin[j])
				eorg = org[j] - ent->v.absmin[j];
			else if (org[j] > ent->v.absmax[j])
				eorg = org[j] - ent->v.absmax[j];
			else
				eorg = 0.0;

			distSquared += eorg * eorg;
		}

		// Make sure it's inside the sphere.
		if (distSquared <= (rad * rad))
		{
			return ent;
		}
	}

	return &sv.edicts[0];
}

/*
=================
PF_Spawn_I

Allocates an edict for use with an entity
=================
*/
edict_t* PF_Spawn_I( void )
{
	edict_t* pedict = ED_Alloc();
	return pedict;
}

/*
=================
CreateNamedEntity

Create specified entity, allocate private data
=================
*/
edict_t* CreateNamedEntity( int className )
{
	edict_t* pedict;
	ENTITYINIT pEntityInit;

	if (!className)
		Sys_Error("Spawned a NULL entity!");

	pedict = ED_Alloc();
	pedict->v.classname = className;

	pEntityInit = GetEntityInit(&pr_strings[className]);
	if (!pEntityInit)
	{
		ED_Free(pedict);
		Con_DPrintf("Can't create entity: %s\n", &pr_strings[className]);
		return NULL;
	}
	
	pEntityInit(&pedict->v);
	return pedict;
}

/*
=================
PF_Remove_I

Immediately remove the given entity
=================
*/
void PF_Remove_I( edict_t* ed )
{
	ED_Free(ed);
}

/*
=================
PF_find_Shared

Tries to find an entity by string
=================
*/
edict_t* PF_find_Shared( int eStartSearchAfter, int iFieldToMatch, const char* szValueToFind )
{
	int		e;
	char* t;
	edict_t* ed;

	for (e = eStartSearchAfter + 1; e < sv.num_edicts; e++)
	{
		ed = &sv.edicts[e];
		if (ed->free)
			continue;

		t = &pr_strings[*(string_t*)((size_t)&ed->v + iFieldToMatch)];
		if (t == NULL || t == &pr_strings[0])
			continue;

		if (!strcmp(t, szValueToFind))
			return ed;
	}

	return &sv.edicts[0];
}

int iGetIndex( const char* pszField )
{
	char sz[512];

	strcpy(sz, pszField);
	_strlwr(sz);

	if (!strcmp(sz, "classname"))
		return offsetof(entvars_t, classname);
	if (!strcmp(sz, "model"))
		return offsetof(entvars_t, model);
	if (!strcmp(sz, "viewmodel"))
		return offsetof(entvars_t, viewmodel);
	if (!strcmp(sz, "weaponmodel"))
		return offsetof(entvars_t, weaponmodel);
	if (!strcmp(sz, "netname"))
		return offsetof(entvars_t, netname);
	if (!strcmp(sz, "target"))
		return offsetof(entvars_t, target);
	if (!strcmp(sz, "targetname"))
		return offsetof(entvars_t, targetname);
	if (!strcmp(sz, "message"))
		return offsetof(entvars_t, message);
	if (!strcmp(sz, "noise"))
		return offsetof(entvars_t, noise);
	if (!strcmp(sz, "noise1"))
		return offsetof(entvars_t, noise1);
	if (!strcmp(sz, "noise2"))
		return offsetof(entvars_t, noise2);
	if (!strcmp(sz, "noise3"))
		return offsetof(entvars_t, noise3);
	if (!strcmp(sz, "globalname"))
		return offsetof(entvars_t, globalname);

	return -1;
}

edict_t* FindEntityByString( edict_t* pEdictStartSearchAfter, const char* pszField, const char* pszValue )
{
	int e;
	int iField = iGetIndex(pszField);
	if (iField == -1)
		return NULL;

	if (!pszValue)
		return NULL;

	if (pEdictStartSearchAfter)
		e = NUM_FOR_EDICT(pEdictStartSearchAfter);
	else
		e = 0;

	return PF_find_Shared(e, iField, pszValue);
}

// Returns light_level for clients and world, otherwise
// the color of the floor that the entity is standing on
int GetEntityIllum( edict_t* pEnt )
{
	int iReturn;
	colorVec cvFloorColor = { 0, 0, 0, 0 };
	int iIndex;

	if (!pEnt)
		return -1;

	iIndex = NUM_FOR_EDICT(pEnt);
	if (iIndex <= svs.maxclients)
	{
		// the player has a more accurate level of light
		// coming from the client side
		return pEnt->v.light_level;
	}

	if (cls.state != ca_connected && cls.state != ca_uninitialized && cls.state != ca_active)
		return 128;

	cvFloorColor = cl_entities[iIndex].cvFloorColor;
	iReturn = (cvFloorColor.r + cvFloorColor.g + cvFloorColor.b) / 3;

	return iReturn;
}

void PR_CheckEmptyString( const char* s )
{
	if (s[0] <= ' ')
		Host_Error("Bad string: %s", s);
}

/*
=================
PF_precache_sound_I

=================
*/
int PF_precache_sound_I( char* s )
{
	int	i;

	if (s && s[0] == '!')
		Host_Error("PF_precache_sound_I: '%s' do not precache sentence names!", s);

	if (sv.state == ss_loading)
	{
		PR_CheckEmptyString(s);

		for (i = 0; i < MAX_SOUNDS; i++)
		{
			if (!sv.sound_precache[i])
			{
				sv.sound_precache[i] = s;
				return i;
			}
			if (!strcmp(sv.sound_precache[i], s))
				return i;
		}
		Host_Error("PF_precache_sound_I: '%s' overflow", s);
	}
	else
	{
		// check if it's already precached
		for (i = 0; i < MAX_SOUNDS; i++)
		{
			if (sv.sound_precache[i])
			{
				if (!strcmp(sv.sound_precache[i], s))
					return i;
			}
		}
		Host_Error("PF_precache_sound_I: '%s' Precache can only be done in spawn functions", s);
	}

	return i;
}

/*
=================
PF_precache_model_I

=================
*/
int PF_precache_model_I( char* s )
{
	int	i;

	if (sv.state == ss_loading)
	{
		PR_CheckEmptyString(s);

		for (i = 0; i < MAX_MODELS; i++)
		{
			if (!sv.model_precache[i])
			{
				sv.model_precache[i] = s;
				sv.models[i] = Mod_ForName(s, TRUE);
				return i;
			}
			if (!strcmp(sv.model_precache[i], s))
				return i;
		}	
		Host_Error("PF_precache_model_I: '%s' overflow", s);
	}
	else
	{
		// check if it's already precached
		for (i = 0; i < MAX_MODELS; i++)
		{
			if (sv.model_precache[i])
			{
				if (!strcmp(sv.model_precache[i], s))
					return i;
			}
		}
		Host_Error("PF_precache_model_I: '%s' Precache can only be done in spawn functions", s);
	}

	return i;
}

/*
=================
PF_IsMapValid_I

Checks if the given filename is a valid map
=================
*/
int PF_IsMapValid_I( char* mapname )
{
	char cBuf[128];
	FILE* f;

	sprintf(cBuf, "maps/%.32s.bsp", mapname);
	return COM_FindFile(cBuf, 0, &f) > -1;
}

int PF_NumberOfEntities_I( void )
{
	int ent_count, i;

	ent_count = 0;
	for (i = 0; i < sv.num_edicts; i++)
	{
		if (sv.edicts[i].free)
			continue;
		ent_count++;
	}
	return ent_count;
}

/*
===============
PF_walkmove_I

float(float yaw, float dist, int iMode) walkmove
===============
*/
int PF_walkmove_I( edict_t* ent, float yaw, float dist, int iMode )
{
	vec3_t	move;
	int 	returnValue;

	if (!(ent->v.flags & (FL_ONGROUND | FL_FLY | FL_SWIM)))
		return FALSE;

	yaw = yaw * M_PI * 2 / 360;

	move[0] = cos(yaw) * dist;
	move[1] = sin(yaw) * dist;
	move[2] = 0;

	switch (iMode)
	{
	default:
	case WALKMOVE_NORMAL:
		returnValue = SV_movestep(ent, move, TRUE);
		break;
	case WALKMOVE_WORLDONLY:
		returnValue = SV_movetest(ent, move, TRUE);
		break;
	case WALKMOVE_CHECKONLY:
		returnValue = SV_movestep(ent, move, FALSE);
		break;
	}

	return returnValue;
}

/*
===============
PF_droptofloor_I

void() droptofloor
===============
*/
int PF_droptofloor_I( edict_t* ent )
{
	vec3_t		end;
	trace_t		trace;
	qboolean	monsterClip;	
	
	monsterClip = (ent->v.flags & FL_MONSTERCLIP) ? TRUE : FALSE;

	VectorCopy(ent->v.origin, end);
	end[2] -= 256;

	trace = SV_Move(ent->v.origin, ent->v.mins, ent->v.maxs, end, MOVE_NORMAL, ent, monsterClip);

	if (trace.allsolid)
		return -1;

	if (trace.fraction == 1)
		return 0;

	VectorCopy(trace.endpos, ent->v.origin);
	SV_LinkEdict(ent, FALSE);
	ent->v.flags |= FL_ONGROUND;
	ent->v.groundentity = trace.ent;
	return 1;
}

/*
===============
PF_DecalIndex

Gets the decal index by name
===============
*/
int PF_DecalIndex( const char* name )
{
	int i;

	for (i = 0; i < sv_decalnamecount; i++)
	{
		if (!_strcmpi(sv_decalnames[i].name, name))
			return i;
	}

	return -1;
}

/*
===============
PF_lightstyle_I

void(float style, string value) lightstyle
===============
*/
void PF_lightstyle_I( int style, char* val )
{
	client_t* client;
	int j;

// change the string in sv
	sv.lightstyles[style] = val;

// send message to all clients on this server
	if (sv.state != ss_active)
		return;

	for (j = 0, client = svs.clients; j < svs.maxclients; j++, client++)
		if (client->active || client->spawned)
		{
			MSG_WriteChar(&client->netchan.message, svc_lightstyle);
			MSG_WriteChar(&client->netchan.message, style);
			MSG_WriteString(&client->netchan.message, val);
		}
}

/*
=============
PF_checkbottom_I
=============
*/
int PF_checkbottom_I( edict_t* pEdict )
{
	return SV_CheckBottom(pEdict);
}

/*
=============
PF_pointcontents_I
=============
*/
int PF_pointcontents_I( const float* rgflVector )
{
	return SV_PointContents(rgflVector);
}

/*
=============
PF_aim_I

Pick a vector for the player to shoot along
vector aim(entity, missilespeed)
=============
*/
cvar_t	sv_aim = { "sv_aim", "1", FALSE, TRUE };
void PF_aim_I( edict_t* ent, float speed, float* rgflReturn )
{
	edict_t* check;
	vec3_t	start, dir, end, bestdir;
	int		i, j;
	trace_t	tr;
	float	dist, bestdist;

// try sending a trace straight
	VectorCopy(gGlobalVariables.v_forward, dir);
	VectorAdd(ent->v.origin, ent->v.view_ofs, start); // get eye position
	VectorMA(start, 2048, dir, end);
	tr = SV_Move(start, vec3_origin, vec3_origin, end, MOVE_NORMAL, ent, FALSE);
	if (tr.ent && tr.ent->v.takedamage == DAMAGE_AIM
		&& (!teamplay.value || ent->v.team <= 0 || ent->v.team != tr.ent->v.team))
	{
		VectorCopy(gGlobalVariables.v_forward, rgflReturn);
		return;
	}

// try all possible entities
	VectorCopy(dir, bestdir);
	bestdist = sv_aim.value;

	for (i = 1; i < sv.num_edicts; i++)
	{
		check = &sv.edicts[i];
		if (check->v.takedamage != DAMAGE_AIM)
			continue;
		if (check == ent)
			continue;
		if (teamplay.value && ent->v.team > 0 && ent->v.team == check->v.team)
			continue;	// don't aim at teammate
		for (j = 0; j < 3; j++)
			end[j] = check->v.origin[j]
			+ 0.75 * (check->v.mins[j] + check->v.maxs[j]);
		VectorSubtract(end, start, dir);
		VectorNormalize(dir);
		dist = DotProduct(dir, gGlobalVariables.v_forward);
		if (dist < bestdist)
			continue;	// too far to turn	
		tr = SV_Move(start, vec3_origin, vec3_origin, end, MOVE_NORMAL, ent, FALSE);
		if (tr.ent == check)
		{	// can shoot at this one
			bestdist = dist;
			VectorCopy(dir, bestdir);
		}
	}

	VectorCopy(bestdir, rgflReturn);
}

/*
==============
PF_changeyaw_I

This was a major timewaster in progs, so it was converted to C
==============
*/
void PF_changeyaw_I( edict_t* ent )
{
	float		ideal, current, move, speed;

	current = anglemod(ent->v.angles[1]);
	ideal = ent->v.ideal_yaw;
	speed = ent->v.yaw_speed;

	if (current == ideal)
		return;
	move = ideal - current;
	if (ideal > current)
	{
		if (move >= 180)
			move = move - 360;
	}
	else
	{
		if (move <= -180)
			move = move + 360;
	}
	if (move > 0)
	{
		if (move > speed)
			move = speed;
	}
	else
	{
		if (move < -speed)
			move = -speed;
	}

	ent->v.angles[1] = anglemod(current + move);
}

/*
==============
PF_changepitch_I

Change the entity's pitch angle to approach its ideal pitch
==============
*/
void PF_changepitch_I( edict_t* ent )
{
	float		ideal, current, move, speed;

	current = anglemod(ent->v.angles[0]);
	ideal = ent->v.idealpitch;
	speed = ent->v.pitch_speed;

	if (current == ideal)
		return;
	move = ideal - current;
	if (ideal > current)
	{
		if (move >= 180)
			move = move - 360;
	}
	else
	{
		if (move <= -180)
			move = move + 360;
	}
	if (move > 0)
	{
		if (move > speed)
			move = speed;
	}
	else
	{
		if (move < -speed)
			move = -speed;
	}

	ent->v.angles[0] = anglemod(current + move);
}





















// TODO: Implement

#define IA	16807
#define IM	2147483647
#define IQ	127773
#define IR	2836

#define NTAB	32
#define NDIV (1+(IM-1)/NTAB)

static int32 idum = 0;

void SeedRandomNumberGenerator( void )
{
	idum = -(int)time(NULL);
	if (idum > 1000)
	{
		idum = -idum;
	}
	else if (idum > -1000)
	{
		idum -= 22261048;
	}
}

int32 ran1( void )
{
	int j;
	int32 k;
	static int32 iy = 0;
	static int32 iv[NTAB];

	if (idum <= 0 || !iy)
	{
		if (-(idum) < 1)
			idum = 1;
		else
			idum = -(idum);

		for (j = NTAB + 7; j >= 0; j--)
		{
			k = (idum) / IQ;
			idum = IA * (idum - k * IQ) - IR * k;

			if (idum < 0)
				idum += IM;
			if (j < NTAB)
				iv[j] = idum;
		}

		iy = iv[0];
	}

	k = (idum) / IQ;
	idum = IA * (idum - k * IQ) - IR * k;

	if (idum < 0)
		idum += IM;

	j = iy / NDIV;
	iy = iv[j];
	iv[j] = idum;

	return iy;
}

#define AM (1.0/IM)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)

float fran1( void )
{
	float temp = (float)AM * ran1();

	if (temp > RNMX)
		return (float)RNMX;
	else
		return temp;
}

// Generate a random float number in the range [ flLow, flHigh ]
float RandomFloat( float flLow, float flHigh )
{
	float fl;

	fl = fran1(); // float in [0..1)
	return (fl * (flHigh - flLow)) + flLow; // float in [low..high)
}

#define MAX_RANDOM_RANGE 0x7FFFFFFFUL

// Generate a random long number in the range [ lLow, lHigh ]
int RandomLong( long lLow, long lHigh )
{
	uint32 maxAcceptable;
	uint32 x;
	uint32 n;

	x = lHigh - lLow + 1;
	if (x <= 0)
	{
		return lLow;
	}

	// The following maps a uniform distribution on the interval [0..MAX_RANDOM_RANGE]
	// to a smaller, client-specified range of [0..x-1] in a way that doesn't bias
	// the uniform distribution unfavorably. Even for a worst case x, the loop is
	// guaranteed to be taken no more than half the time, so for that worst case x,
	// the average number of times through the loop is 2. For cases where x is
	// much smaller than MAX_RANDOM_RANGE, the average number of times through the
	// loop is very close to 1.
	maxAcceptable = MAX_RANDOM_RANGE - ((MAX_RANDOM_RANGE + 1) % x);

	do
	{
		n = ran1();
	} while (n > maxAcceptable);

	return lLow + (n % x);
}

// TODO: Implement