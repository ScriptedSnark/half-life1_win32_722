#include <time.h>

#include "quakedef.h"

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