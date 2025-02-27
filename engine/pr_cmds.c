#include "quakedef.h"
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



















// TODO: Implement

// Returns surface instance by specified position
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

// TODO: Implement

// Sets trace vars for global servers variables
void SV_SetGlobalTrace( trace_t* ptrace )
{
	// TODO: Implement
}

// TODO: Implement

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

int PF_IsMapValid_I( char *mapname )
{
	FILE*	pfMap;
	char	cBuf[MAX_OSPATH];

	sprintf(cBuf, "maps/%.32s.bsp", mapname);
	return COM_FindFile(cBuf, NULL, &pfMap) > -1;
}