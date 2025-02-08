#include "quakedef.h"
#include "pr_cmds.h"

#define MAX_BEAMS				128		// Max simultaneous beams
#define MAX_PARTICLES			2048	// default max # of particles at one
										//  time
#define ABSOLUTE_MIN_PARTICLES	512		// no fewer than this no matter what's
										//  on the command line

// particle ramps
int		ramp1[8] = { 0x6F, 0x6D, 0x6B, 0x69, 0x67, 0x65, 0x63, 0x61 };
int		ramp2[8] = { 0x6F, 0x6E, 0x6D, 0x6C, 0x6B, 0x6A, 0x68, 0x66 };
int		ramp3[6] = { 0x6D, 0x6B, 6, 5, 4, 3 };

// spark ramps
int		gSparkRamp[9] = { 0x0FE, 0x0FD, 0x0FC, 0x6F, 0x6E, 0x6D, 0x6C, 0x67, 0x60 };

color24 gTracerColors[] =
{
	{ 255, 255, 255 },		// White
	{ 255, 0, 0 },			// Red
	{ 0, 255, 0 },			// Green
	{ 0, 0, 255 },			// Blue
	{ 0, 0, 0 },			// Tracer default, filled in from cvars, etc.
	{ 255, 167, 17 },		// Yellow-orange sparks
	{ 255, 130, 90 },		// Yellowish streaks (garg)
	{ 55, 60, 144 },		// Blue egon streak
	{ 255, 130, 90 },		// More Yellowish streaks (garg)
	{ 255, 140, 90 },		// More Yellowish streaks (garg)
	{ 200, 130, 90 },		// More red streaks (garg)
	{ 255, 120, 70 },		// Darker red streaks (garg)
};

particle_t* active_particles, * free_particles, * gpActiveTracers;

particle_t* particles;
int			r_numparticles;
int			cl_numbeamentities;
cl_entity_t	cl_beamentities[MAX_BEAMENTS];

vec3_t			r_pright, r_pup, r_ppn;

BEAM*		gBeams, * gpFreeBeams, * gpActiveBeams;

// Forward declarations
void R_TracerDraw( void );
void R_BeamDrawList( void );

void R_BeamInit( void )
{
	gBeams = (BEAM*)Hunk_AllocName(sizeof(BEAM) * MAX_BEAMS, "lightning");
}

void R_BeamClear( void )
{
	int i;

	gpFreeBeams = &gBeams[0];
	gpActiveBeams = NULL;

	memset(gBeams, 0, sizeof(BEAM) * MAX_BEAMS);

	for (i = 0; i < MAX_BEAMS; i++)
	{
		gBeams[i].next = &gBeams[i + 1];
	}

	gBeams[MAX_BEAMS - 1].next = NULL;
}

/*
===============
R_InitParticles
===============
*/
void R_InitParticles( void )
{
	int		i;

	i = COM_CheckParm("-particles");
	if (i)
	{
		r_numparticles = Q_atoi(com_argv[i + 1]);
		if (r_numparticles < ABSOLUTE_MIN_PARTICLES)
			r_numparticles = ABSOLUTE_MIN_PARTICLES;
	}
	else
	{
		r_numparticles = MAX_PARTICLES;
	}

	particles = (particle_t*)Hunk_AllocName(r_numparticles * sizeof(particle_t), "particles");

	R_BeamInit();
}

void R_DarkFieldParticles( cl_entity_t* ent )
{
	int			i, j, k;
	particle_t* p;
	float		vel;
	vec3_t		dir;
	vec3_t		org;

	org[0] = ent->origin[0];
	org[1] = ent->origin[1];
	org[2] = ent->origin[2];
	for (i = -16; i < 16; i += 8)
	{
		for (j = -16; j < 16; j += 8)
		{
			for (k = 0; k < 32; k += 8)
			{
				if (!free_particles)
					return;
				p = free_particles;
				free_particles = p->next;
				p->next = active_particles;
				active_particles = p;

				p->die = cl.time + RandomFloat(0.2, 0.34);
				p->color = RandomLong(150, 155);
#if defined( GLQUAKE )
				p->packedColor = 0;
#else
				p->packedColor = hlRGB(host_basepal, p->color);
#endif
				p->type = pt_slowgrav;

				dir[0] = j * 8;
				dir[1] = i * 8;
				dir[2] = k * 8;

				p->org[0] = org[0] + i + RandomLong(0, 3);
				p->org[1] = org[1] + j + RandomLong(0, 3);
				p->org[2] = org[2] + k + RandomLong(0, 3);

				VectorNormalize(dir);
				vel = RandomFloat(50.0, 113.0);
				VectorScale(dir, vel, p->vel);
			}
		}
	}
}


/*
===============
R_EntityParticles
===============
*/

#define NUMVERTEXNORMALS	162
extern	float	r_avertexnormals[NUMVERTEXNORMALS][3];
vec3_t	avelocities[NUMVERTEXNORMALS];
float	beamlength = 16;
vec3_t	avelocity = { 23, 7, 3 };
float	partstep = 0.01;
float	timescale = 0.01;

void R_EntityParticles( cl_entity_t* ent )
{
	int			count;
	int			i;
	particle_t* p;
	float		angle;
	float		sr, sp, sy, cr, cp, cy;
	vec3_t		forward;
	float		dist;

	dist = 64;
	count = 50;

	if (!avelocities[0][0])
	{
		for (i = 0; i < NUMVERTEXNORMALS * 3; i++)
			avelocities[0][i] = RandomFloat(0, 2.55);
	}

	for (i = 0; i < NUMVERTEXNORMALS; i++)
	{
		angle = cl.time * avelocities[i][0];
		sy = sin(angle);
		cy = cos(angle);
		angle = cl.time * avelocities[i][1];
		sp = sin(angle);
		cp = cos(angle);
		angle = cl.time * avelocities[i][2];
		sr = sin(angle);
		cr = cos(angle);

		forward[0] = cp * cy;
		forward[1] = cp * sy;
		forward[2] = -sp;

		if (!free_particles)
			return;

		p = free_particles;
		free_particles = p->next;
		p->next = active_particles;
		active_particles = p;

		p->die = cl.time + 0.01;
		p->color = 111;
#if defined( GLQUAKE )
		p->packedColor = 0;
#else
		p->packedColor = hlRGB(host_basepal, p->color);
#endif
		p->type = pt_explode;
	
		p->org[0] = ent->origin[0] + r_avertexnormals[i][0] * dist + forward[0] * beamlength;
		p->org[1] = ent->origin[1] + r_avertexnormals[i][1] * dist + forward[1] * beamlength;
		p->org[2] = ent->origin[2] + r_avertexnormals[i][2] * dist + forward[2] * beamlength;
	}
}

/*
===============
R_ClearParticles
===============
*/
void R_ClearParticles( void )
{
	int		i;

	free_particles = &particles[0];
	active_particles = NULL;

	gpActiveTracers = NULL;

	for (i = 0; i < r_numparticles; i++)
		particles[i].next = &particles[i + 1];
	particles[r_numparticles - 1].next = NULL;

	R_BeamClear();
}

/*
===============
R_FreeDeadParticles

Free dead particles
===============
*/
void R_FreeDeadParticles( particle_t** ppparticles )
{
	particle_t* kill;
	particle_t* p;

	// kill all the ones hanging direcly off the base pointer
	for (;; )
	{
		kill = *ppparticles;
		if (kill && kill->die < cl.time)
		{
			*ppparticles = kill->next;
			kill->next = free_particles;
			free_particles = kill;
			continue;
		}
		break;
	}

	// kill off all the others
	for (p = *ppparticles; p; p = p->next)
	{
		for (;; )
		{
			kill = p->next;
			if (kill && kill->die < cl.time)
			{
				p->next = kill->next;
				kill->next = free_particles;
				free_particles = kill;
				continue;
			}
			break;
		}
	}
}

/*
===============
R_ReadPointFile_f
===============
*/
void R_ReadPointFile_f( void )
{
	FILE* f;
	vec3_t	org;
	int		r;
	int		c;
	particle_t* p;
	char	name[128];

	if (!cl.worldmodel)
		return;

	sprintf(name, "maps/%s.pts", sv.name);

	COM_FOpenFile(name, &f);
	if (!f)
	{
		Con_Printf("couldn't open %s\n", name);
		return;
	}

	Con_Printf("Reading %s...\n", name);
	c = 0;
	for (;; )
	{
		r = fscanf(f, "%f %f %f\n", &org[0], &org[1], &org[2]);
		if (r != 3)
			break;
		c++;

		if (!free_particles)
		{
			Con_Printf("Not enough free particles\n");
			break;
		}

		p = free_particles;
		free_particles = p->next;
		p->next = active_particles;
		active_particles = p;

		p->die = 99999;
		p->type = pt_static;
		p->color = -(short)c & 15;
#if defined( GLQUAKE )
		p->packedColor = 0;
#else
		p->packedColor = hlRGB(host_basepal, p->color);
#endif

		VectorCopy(vec3_origin, p->vel);
		VectorCopy(org, p->org);
	}

	fclose(f);
	Con_Printf("%i points read\n", c);
}

/*
===============
R_ParseParticleEffect

Parse an effect out of the server message
===============
*/
void R_ParseParticleEffect( void )
{
	vec3_t		org, dir;
	int			i, count, msgcount, color;

	for (i = 0; i < 3; i++)
		org[i] = MSG_ReadCoord();
	for (i = 0; i < 3; i++)
		dir[i] = MSG_ReadChar() * (1.0 / 16);
	msgcount = MSG_ReadByte();
	color = MSG_ReadByte();

	if (msgcount == 255)
		count = 1024;
	else
		count = msgcount;

	R_RunParticleEffect(org, dir, color, count);
}

/*
===============
R_ParticleExplosion

===============
*/
void R_ParticleExplosion( vec_t* org )
{
	int			i, j;
	particle_t* p;

	for (i = 0; i < 1024; i++)
	{
		if (!free_particles)
			return;

		p = free_particles;
		free_particles = p->next;
		p->next = active_particles;
		active_particles = p;
		
		p->die = cl.time + 5.0;
		p->color = ramp1[0];
		p->ramp = RandomLong(0, 3);
#if defined( GLQUAKE )
		p->packedColor = 0;
#else
		p->packedColor = hlRGB(host_basepal, p->color);
#endif

		if (i & 1)
		{
			p->type = pt_explode;

			while (1)
			{
				for (j = 0; j < 3; j++)
				{
					p->vel[j] = RandomFloat(-512, 512);
				}

				if (DotProduct(p->vel, p->vel) <= 262144.0)
					break;
			}

			for (j = 0; j < 3; j++)
			{
				p->org[j] = org[j] + p->vel[j] * 0.25;
			}
		}
		else
		{
			p->type = pt_explode2;

			while (1)
			{
				for (j = 0; j < 3; j++)
				{
					p->vel[j] = RandomFloat(-512, 512);
				}

				if (DotProduct(p->vel, p->vel) <= 262144.0)
					break;
			}

			for (j = 0; j < 3; j++)
			{
				p->org[j] = org[j] + p->vel[j] * 0.25;
			}
		}
	}
}

/*
===============
R_ParticleExplosion2

===============
*/
void R_ParticleExplosion2( vec_t* org, int colorStart, int colorLength )
{
	int			i, j;
	particle_t* p;
	int			colorMod = 0;

	for (i = 0; i < 512; i++)
	{
		if (!free_particles)
			return;

		p = free_particles;
		free_particles = p->next;
		p->next = active_particles;
		active_particles = p;
		
		p->die = cl.time + 0.3;
		p->color = colorStart + (colorMod % colorLength);
#if defined( GLQUAKE )
		p->packedColor = 0;
#else
		p->packedColor = hlRGB(host_basepal, p->color);
#endif

		colorMod++;

		p->type = pt_blob;

		for (j = 0; j < 3; j++)
		{
			p->org[j] = org[j] + RandomFloat(-16, 16);
			p->vel[j] = RandomFloat(-256, 256);
		}
	}
}




// TODO: Implement

/*
===============
R_TracerParticles
===============
*/
particle_t* R_TracerParticles( vec_t* org, vec_t* vel, float life )
{
	int		i;
	particle_t* p;

	if (!free_particles)
		return NULL;
	
	p = free_particles;
	free_particles = p->next;
	p->next = gpActiveTracers;
	gpActiveTracers = p;

	p->die = cl.time + life;
	p->type = pt_static;
	p->packedColor = 255;
	p->color = 4;

	p->ramp = tracerLength.value;

	for (i = 0; i < 3; i++)
	{
		p->org[i] = org[i];
		p->vel[i] = vel[i];
	}

	return p;
}

/*
===============
R_RunParticleEffect
===============
*/
void R_RunParticleEffect( vec_t* org, vec_t* dir, int color, int count )
{
	// TODO: Implement
}

/*
===============
R_FlickerParticles
===============
*/
void R_FlickerParticles( vec_t* org )
{
	// TODO: Implement
}

/*
===============
R_SparkStreaks
===============
*/
void R_SparkStreaks( vec_t* pos, int count, int velocityMin, int velocityMax )
{
	// TODO: Implement
}

// TODO: Implement

/*
===============
R_LargeFunnel
===============
*/
void R_LargeFunnel( vec_t* org, int reverse )
{
	// TODO: Implement
}

// TODO: Implement

void R_RocketTrail( vec_t *start, vec_t *end, int type )
{
	// TODO: Implement
}

// TODO: Implement

/*
===============
R_DrawParticles
===============
*/
void R_DrawParticles( void )
{
	// TODO: Implement
}

// TODO: Implement

/*
===============
ScreenTransform

Converts a world coordinate to a screen coordinate
Returns true if it's Z clipped, false otherwise
===============
*/
int ScreenTransform( vec_t* point, vec_t* screen )
{
	float w;

#if defined ( GLQUAKE )
	screen[0] = gWorldToScreen[0] * point[0] + gWorldToScreen[4] * point[1] + gWorldToScreen[8] * point[2] + gWorldToScreen[12];
	screen[1] = gWorldToScreen[1] * point[0] + gWorldToScreen[5] * point[1] + gWorldToScreen[9] * point[2] + gWorldToScreen[13];
	w = gWorldToScreen[3] * point[0] + gWorldToScreen[7] * point[1] + gWorldToScreen[11] * point[2] + gWorldToScreen[15];
#else
	vec3_t out;
	VectorSubtract(point, r_origin, out);
	TransformVector(out, screen);

	w = screen[2];
#endif

	if (w != 0.0)
	{
		w = 1.0 / w;
		screen[0] *= w;
		screen[1] *= w;
	}

	return w <= 0.0;
}

// TODO: Implement
