#include "quakedef.h"
#include "pr_cmds.h"
#include "r_triangle.h"
#include "customentity.h"

#define MAX_BEAMS				128		// Max simultaneous beams
#define MAX_PARTICLES			2048	// default max # of particles at one
										//  time
#define ABSOLUTE_MIN_PARTICLES	512		// no fewer than this no matter what's
										//  on the command line

// particle ramps
int		ramp1[8] = { 0x6F, 0x6D, 0x6B, 0x69, 0x67, 0x65, 0x63, 0x61 };
int		ramp2[8] = { 0x6F, 0x6E, 0x6D, 0x6C, 0x6B, 0x6A, 0x68, 0x66 };
int		ramp3[8] = { 0x6D, 0x6B, 6, 5, 4, 3, 0, 0 };

// spark ramps
int		gSparkRamp[9] = { 0xFE, 0xFD, 0xFC, 0x6F, 0x6E, 0x6D, 0x6C, 0x67, 0x60 };

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
void R_BeamDraw( BEAM* pbeam, float frametime );
void R_BeamDrawList( void );
int R_BeamCull( vec_t* start, vec_t* end, int pvsOnly );


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

/*
===============
R_BlobExplosion

===============
*/
void R_BlobExplosion( vec_t* org )
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

		p->die = cl.time + RandomFloat(1, 1.4);

		if (i & 1)
		{
			p->type = pt_blob;
			p->color = RandomLong(66, 71);
#if defined( GLQUAKE )
			p->packedColor = 0;
#else
			p->packedColor = hlRGB(host_basepal, p->color);
#endif

			for (j = 0; j < 3; j++)
			{
				p->org[j] = org[j] + RandomFloat(-16, 16);
				p->vel[j] = RandomFloat(-256, 256);
			}
		}
		else
		{
			p->type = pt_blob2;
			p->color = RandomLong(150, 155);
#if defined( GLQUAKE )
			p->packedColor = 0;
#else
			p->packedColor = hlRGB(host_basepal, p->color);
#endif

			for (j = 0; j < 3; j++)
			{
				p->org[j] = org[j] + RandomFloat(-16, 16);
				p->vel[j] = RandomFloat(-256, 256);
			}
		}
	}
}

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

	p->type = pt_static;
	p->color = 4;
	p->packedColor = 255;
	p->die = cl.time + life;

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
	int		i, j;
	particle_t* p;

	for (i = 0; i < count; i++)
	{
		if (!free_particles)
			return;

		p = free_particles;
		free_particles = p->next;
		p->next = active_particles;
		active_particles = p;

		if (count == 1024)
		{
			// rocket explosion
			p->die = cl.time + 5.0;
			p->color = ramp1[0];
#if defined( GLQUAKE )
			p->packedColor = 0;
#else
			p->packedColor = hlRGB(host_basepal, ramp1[0]);
#endif
			p->ramp = RandomLong(0, 3);

			if (i & 1)
			{
				p->type = pt_explode;
				for (j = 0; j < 3; j++)
				{
					p->org[j] = org[j] + RandomFloat(-16, 16);
					p->vel[j] = RandomFloat(-256, 256);
				}
			}
			else
			{
				p->type = pt_explode2;
				for (j = 0; j < 3; j++)
				{
					p->org[j] = org[j] + RandomFloat(-16, 16);
					p->vel[j] = RandomFloat(-256, 256);
				}
			}
		}
		else
		{
			p->die = cl.time + RandomFloat(0, 0.4);
			p->color = (color & ~7) + RandomLong(0, 7);
#if defined( GLQUAKE )
			p->packedColor = 0;
#else
			p->packedColor = hlRGB(host_basepal, p->color);
#endif
			p->type = pt_slowgrav;

			for (j = 0; j < 3; j++)
			{
				p->org[j] = org[j] + RandomFloat(-8, 8);
				p->vel[j] = dir[j] * 15;
			}
		}
	}
}

/*
===============
R_FlickerParticles
===============
*/
void R_FlickerParticles( vec_t* org )
{
	int		i, j;
	particle_t* p;

	for (i = 0; i < 15; i++)
	{
		if (!free_particles)
			return;

		p = free_particles;
		free_particles = p->next;
		p->next = active_particles;
		active_particles = p;

		for (j = 0; j < 3; j++)
		{
			p->org[j] = org[j];
			p->vel[j] = RandomFloat(-32, 32);
		}

		p->vel[2] = RandomFloat(80, 143);

		p->color = 254;
		p->ramp = 0;
#if defined( GLQUAKE )
		p->packedColor = 0;
#else
		p->packedColor = hlRGB(host_basepal, p->color);
#endif
		p->die = cl.time + 2.0;
		p->type = pt_blob2;
	}
}

/*
===============
R_SparkStreaks
===============
*/
void R_SparkStreaks( vec_t* pos, int count, int velocityMin, int velocityMax )
{
	int		i, j;
	particle_t* p;

	for (i = 0; i < count; i++)
	{
		if (!free_particles)
			return;

		p = free_particles;
		free_particles = p->next;
		p->next = gpActiveTracers;
		gpActiveTracers = p;

		p->color = 5;
		p->type = pt_grav;
		p->packedColor = 255;
		p->die = cl.time + RandomFloat(0.1, 0.5);
		p->ramp = 0.5;

		VectorCopy(pos, p->org);

		for (j = 0; j < 3; j++)
			p->vel[j] = RandomFloat(velocityMin, velocityMax);
	}
}

/*
===============
R_StreakSplash
===============
*/
void R_StreakSplash( vec_t* pos, vec_t* dir, int color, int count, float speed, int velocityMin, int velocityMax )
{
	int		i, j;
	particle_t* p;
	vec3_t	initialVelocity;

	VectorScale(dir, speed, initialVelocity);

	for (i = 0; i < count; i++)
	{
		if (!free_particles)
			return;

		p = free_particles;
		free_particles = p->next;
		p->next = gpActiveTracers;
		gpActiveTracers = p;
		
		p->color = color;
		p->packedColor = 255;
		p->type = pt_grav;
		p->die = cl.time + RandomFloat(0.1, 0.5);
		p->ramp = 1;

		for (j = 0; j < 3; j++)
		{
			p->org[j] = pos[j];
			p->vel[j] = initialVelocity[j] + RandomFloat(velocityMin, velocityMax);
		}
	}
}

/*
===============
R_LavaSplash

===============
*/
void R_LavaSplash( vec_t* org )
{
	int			i, j, k;
	particle_t* p;
	float		vel;
	vec3_t		dir;

	for (i = -16; i < 16; i++)
	{
		for (j = -16; j < 16; j++)
		{
			for (k = 0; k < 1; k++)
			{
				if (!free_particles)
					return;

				p = free_particles;
				free_particles = p->next;
				p->next = active_particles;
				active_particles = p;

				p->die = cl.time + RandomFloat(2, 2.62);
				p->type = pt_slowgrav;
				p->color = RandomLong(224, 231);
#if defined( GLQUAKE )
				p->packedColor = 0;
#else
				p->packedColor = hlRGB(host_basepal, p->color);
#endif

				dir[0] = j * 8 + RandomFloat(0, 7);
				dir[1] = i * 8 + RandomFloat(0, 7);
				dir[2] = 256;

				p->org[0] = org[0] + dir[0];
				p->org[1] = org[1] + dir[1];
				p->org[2] = org[2] + RandomFloat(0, 63);

				VectorNormalize(dir);
				vel = RandomFloat(50, 113);
				VectorScale(dir, vel, p->vel);
			}
		}
	}
}

/*
===============
R_LargeFunnel
===============
*/
void R_LargeFunnel( vec_t* org, int reverse )
{
	int			i, j;
	particle_t* p;
	float		vel;
	vec3_t		dir, dest;
	float		flDist;

	for (i = -256; i <= 256; i += 32)
	{
		for (j = -256; j <= 256; j += 32)
		{
			if (!free_particles)
				return;

			p = free_particles;
			free_particles = p->next;
			p->next = active_particles;
			active_particles = p;

			if (reverse)
			{
				VectorCopy(org, p->org);

				dest[0] = org[0] + i;
				dest[1] = org[1] + j;
				dest[2] = org[2] + RandomFloat(100, 800);

				// send particle heading to dest at a random speed
				VectorSubtract(dest, p->org, dir);

				vel = dest[2] / 8.0;// velocity based on how far particle starts from org
			}
			else
			{
				p->org[0] = org[0] + i;
				p->org[1] = org[1] + j;
				p->org[2] = org[2] + RandomFloat(100, 800);

				// send particle heading to dest at a random speed
				VectorSubtract(org, p->org, dir);

				vel = p->org[2] / 8.0;// velocity based on how far particle starts from org
			}

			p->type = pt_static;
			p->color = 244;
#if defined( GLQUAKE )
			p->packedColor = 0;
#else
			p->packedColor = hlRGB(host_basepal, p->color);
#endif

			flDist = VectorNormalize(dir);	// save the distance

			if (vel < 64)
			{
				vel = 64;
			}

			vel += RandomFloat(64, 128);
			VectorScale(dir, vel, p->vel);

			// die right when you get there
			p->die = cl.time + (flDist / vel);
		}
	}
}

/*
===============
R_TeleportSplash

Quake1 teleport splash
===============
*/
void R_TeleportSplash( vec_t* org )
{
	int			i, j, k;
	particle_t* p;
	float		vel;
	vec3_t		dir;

	for (i = -16; i < 16; i += 4)
	{
		for (j = -16; j < 16; j += 4)
		{
			for (k = -24; k < 32; k += 4)
			{
				if (!free_particles)
					return;

				p = free_particles;
				free_particles = p->next;
				p->next = active_particles;
				active_particles = p;

				p->die = cl.time + RandomFloat(0.2, 0.34);
				p->color = RandomLong(7, 14);
				p->type = pt_slowgrav;
#if defined( GLQUAKE )
				p->packedColor = 0;
#else
				p->packedColor = hlRGB(host_basepal, p->color);
#endif

				dir[0] = j * 8;
				dir[1] = i * 8;
				dir[2] = k * 8;

				p->org[0] = org[0] + i + RandomFloat(0, 3);
				p->org[1] = org[1] + j + RandomFloat(0, 3);
				p->org[2] = org[2] + k + RandomFloat(0, 3);

				VectorNormalize(dir);
				vel = RandomFloat(50, 113);
				VectorScale(dir, vel, p->vel);
			}
		}
	}
}

/*
===============
R_ShowLine
===============
*/
void R_ShowLine( vec_t* start, vec_t* end )
{
	vec3_t		vec;
	float		len;
	particle_t* p;
	int			dec = 5;

	static int tracercount;

	VectorSubtract(end, start, vec);
	len = VectorNormalize(vec);
	VectorScale(vec, dec, vec);

	while (len > 0)
	{
		len -= dec;

		if (!free_particles)
			return;

		p = free_particles;
		free_particles = p->next;
		p->next = active_particles;
		active_particles = p;

		VectorCopy(vec3_origin, p->vel);

		p->die = cl.time + 30;
		p->type = pt_static;
		p->color = 75;
#if defined( GLQUAKE )
		p->packedColor = 0;
#else
		p->packedColor = hlRGB(host_basepal, p->color);
#endif

		VectorCopy(start, p->org);
		VectorAdd(start, vec, start);
	}
}

/*
===============
R_BloodStream

===============
*/
void R_BloodStream( vec_t* org, vec_t* dir, int pcolor, int speed )
{
	// Add our particles
	vec3_t	dirCopy;
	float	arc;
	int		count;
	int		count2;
	particle_t* p;
	float	num;
	int		speedCopy = speed;

	VectorNormalize(dir);

	arc = 0.05;
	for (count = 0; count < 100; count++)
	{
		if (!free_particles)
			return;

		p = free_particles;
		free_particles = p->next;
		p->next = active_particles;
		active_particles = p;

		p->die = cl.time + 2.0;
		p->color = pcolor + RandomLong(0, 9);
#if defined( GLQUAKE )
		p->packedColor = 0;
#else
		p->packedColor = hlRGB(host_basepal, p->color);
#endif

		p->type = pt_vox_grav;

		VectorCopy(org, p->org);
		VectorCopy(dir, dirCopy);
		dirCopy[2] -= arc;

		arc -= 0.005;

		VectorScale(dirCopy, speedCopy, p->vel);
		speedCopy -= 0.00001; // make last few drip
	}

	arc = 0.075;
	for (count = 0; count < (speed / 5); count++)
	{
		if (!free_particles)
			return;

		p = free_particles;
		free_particles = p->next;
		p->next = active_particles;
		active_particles = p;

		p->die = cl.time + 3.0;
		p->type = pt_vox_slowgrav;
		p->color = pcolor + RandomLong(0, 9);
#if defined( GLQUAKE )
		p->packedColor = 0;
#else
		p->packedColor = hlRGB(host_basepal, p->color);
#endif

		VectorCopy(org, p->org);
		VectorCopy(dir, dirCopy);
		dirCopy[2] -= arc;

		arc -= 0.005;

		num = RandomFloat(0, 1);
		speedCopy = speed * num;
		num *= 1.7;

		VectorScale(dirCopy, num, dirCopy); // randomize a bit
		VectorScale(dirCopy, speedCopy, p->vel);

		for (count2 = 0; count2 < 2; count2++)
		{
			if (!free_particles)
				return;

			p = free_particles;
			free_particles = p->next;
			p->next = active_particles;
			active_particles = p;

			p->die = cl.time + 3.0;
			p->type = pt_vox_slowgrav;
			p->color = pcolor + RandomLong(0, 9);
#if defined( GLQUAKE )
			p->packedColor = 0;
#else
			p->packedColor = hlRGB(host_basepal, p->color);
#endif

			p->org[0] = org[0] + RandomFloat(-1, 1);
			p->org[1] = org[1] + RandomFloat(-1, 1);
			p->org[2] = org[2] + RandomFloat(-1, 1);

			VectorCopy(dir, dirCopy);
			dirCopy[2] -= arc;

			VectorScale(dirCopy, num, dirCopy); // randomize a bit
			VectorScale(dirCopy, speedCopy, p->vel);
		}
	}
}

/*
===============
R_Blood

===============
*/
void R_Blood( vec_t* org, vec_t* dir, int pcolor, int speed )
{
	vec3_t	dirCopy;
	vec3_t	orgCopy;
	float	arc;
	int		count;
	int		count2;
	particle_t* p;
	int		pspeed;

	VectorNormalize(dir);

	pspeed = speed * 3;

	arc = 0.06;
	for (count = 0; count < (speed / 2); count++)
	{
		orgCopy[0] = org[0] + RandomFloat(-3, 3);
		orgCopy[1] = org[1] + RandomFloat(-3, 3);
		orgCopy[2] = org[2] + RandomFloat(-3, 3);

		dirCopy[0] = dir[0] + RandomFloat(-arc, arc);
		dirCopy[1] = dir[1] + RandomFloat(-arc, arc);
		dirCopy[2] = dir[2] + RandomFloat(-arc, arc);

		for (count2 = 0; count2 < 8; count2++)
		{
			if (!free_particles)
				return;

			p = free_particles;
			free_particles = p->next;
			p->next = active_particles;
			active_particles = p;

			p->die = cl.time + 1.5;
			p->color = pcolor + RandomLong(0, 9);
			p->type = pt_vox_grav;
#if defined( GLQUAKE )
			p->packedColor = 0;
#else
			p->packedColor = hlRGB(host_basepal, p->color);
#endif

			p->org[0] = orgCopy[0] + RandomFloat(-1, 1);
			p->org[1] = orgCopy[1] + RandomFloat(-1, 1);
			p->org[2] = orgCopy[2] + RandomFloat(-1, 1);

			VectorScale(dirCopy, pspeed, p->vel);
		}

		pspeed -= speed;
	}
}

/*
===============
R_RocketTrail
===============
*/
void R_RocketTrail( vec_t *start, vec_t *end, int type )
{
	vec3_t	vec, right, up;
	float	len;
	int		j;
	particle_t* p;
	int		dec;

	static int tracercount;

	VectorSubtract(end, start, vec);

	len = VectorNormalize(vec);

	if (type == 7)
	{
		dec = 1;
		VectorMatrix(vec, right, up);
	}
	else if (type < 128)
	{
		dec = 3;
	}
	else
	{
		dec = 1;
		type -= 128;
	}

	VectorScale(vec, dec, vec);

	while (len > 0)
	{
		len -= dec;

		if (!free_particles)
			return;

		p = free_particles;
		free_particles = p->next;
		p->next = active_particles;
		active_particles = p;

		VectorCopy(vec3_origin, p->vel);

		p->die = cl.time + 2.0;

		switch (type)
		{
		case 0: // rocket trail
			p->ramp = RandomLong(0, 3);
			p->type = pt_fire;
			p->color = ramp3[(int)p->ramp];
			for (j = 0; j < 3; j++)
				p->org[j] = start[j] + RandomFloat(-3, 3);
			break;

		case 1:	// smoke
			p->ramp = RandomLong(2, 5);
			p->type = pt_fire;
			p->color = ramp3[(int)p->ramp];
			for (j = 0; j < 3; j++)
				p->org[j] = start[j] + RandomFloat(-3, 3);
			break;
	
		case 2:	// blood
			p->type = pt_grav;
			p->color = RandomLong(67, 70);
			for (j = 0; j < 3; j++)
				p->org[j] = start[j] + RandomFloat(-3, 3);
			break;

		case 3:
		case 5:	// tracer
			p->die = cl.time + 0.5;
			p->type = pt_static;

			if (type == 3)
				p->color = 52 + (tracercount & 4) * 2;
			else
				p->color = 230 + (tracercount & 4) * 2;

			VectorCopy(start, p->org);
			tracercount++;

			if (tracercount & 1)
			{
				p->vel[0] = 30 * vec[1];
				p->vel[1] = 30 * -vec[0];
			}
			else
			{
				p->vel[0] = 30 * -vec[1];
				p->vel[1] = 30 * vec[0];
			}
			break;

		case 4: // slight blood
			p->type = pt_grav;
			p->color = RandomLong(67, 70);
			for (j = 0; j < 3; j++)
				p->org[j] = start[j] + RandomFloat(-3, 3);
			len -= 3;
			break;

		case 6:	// voor trail
			p->ramp = RandomLong(0, 3);
			p->type = pt_fire;
			p->color = ramp3[(int)p->ramp];
			VectorCopy(start, p->org);
			break;

		case 7:	// explosion tracer
		{
			float s, c, x, y;

			j = RandomLong(0, 0xFFFF);
			s = sin(j);
			c = cos(j);

			j = RandomLong(8, 16);
			y = s * j;
			x = c * j;

			p->org[0] = start[0] + right[0] * y + up[0] * x;
			p->org[1] = start[1] + right[1] * y + up[1] * x;
			p->org[2] = start[2] + right[2] * y + up[2] * x;
			
			VectorSubtract(start, p->org, p->vel);
			VectorScale(p->vel, 2.0, p->vel);
			VectorMA(p->vel, RandomFloat(96, 111), vec, p->vel);

			p->die = cl.time + 2.0;
			p->ramp = RandomLong(0, 3);
			p->color = ramp3[(int)p->ramp];
			p->type = pt_explode2;
			break;
		}
		}

#if defined( GLQUAKE )
		p->packedColor = 0;
#else
		p->packedColor = hlRGB(host_basepal, p->color);
#endif

		VectorAdd(start, vec, start);
	}
}

/*
===============
R_DrawParticles
===============
*/
extern	cvar_t	sv_gravity;

void R_DrawParticles( void )
{
	particle_t* p;
	float			grav;
	int				i;
	float			time2, time3;
	float			time1;
	float			dvel;
	float			frametime;
#ifdef GLQUAKE
	vec3_t			up, right;
	float			scale;

	GL_Bind(particletexture);
	qglEnable(GL_ALPHA_TEST);
	qglEnable(GL_BLEND);
	qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	qglBegin(GL_TRIANGLES);

	VectorScale(vup, 1.5, up);
	VectorScale(vright, 1.5, right);
#else
	D_StartParticles();

	VectorScale(vright, xscaleshrink, r_pright);
	VectorScale(vup, yscaleshrink, r_pup);
	VectorCopy(vpn, r_ppn);
#endif

	frametime = cl.time - cl.oldtime;
	time3 = frametime * 15;
	time2 = frametime * 10; // 15;
	time1 = frametime * 5;
	grav = frametime * sv_gravity.value * 0.05;
	dvel = 4 * frametime;

	R_FreeDeadParticles(&active_particles);

	for (p = active_particles; p; p = p->next)
	{
		if (p->type != pt_blob)
		{
#if defined ( GLQUAKE )
			word* pb;
			byte rgba[4];

			// hack a scale up to keep particles from disapearing
			scale = (p->org[0] - r_origin[0]) * vpn[0] + (p->org[1] - r_origin[1]) * vpn[1]
				+ (p->org[2] - r_origin[2]) * vpn[2];

			if (scale < 20)
				scale = 1;
			else
				scale = 1 + scale * 0.004;

			pb = &host_basepal[4 * p->color];
			rgba[0] = pb[2];
			rgba[1] = pb[1];
			rgba[2] = pb[0];
			rgba[3] = 255;

			qglColor3ubv(rgba);
			qglTexCoord2f(0, 0);
			qglVertex3fv(p->org);
			qglTexCoord2f(1, 0);
			qglVertex3f(p->org[0] + up[0] * scale, p->org[1] + up[1] * scale, p->org[2] + up[2] * scale);
			qglTexCoord2f(0, 1);
			qglVertex3f(p->org[0] + right[0] * scale, p->org[1] + right[1] * scale, p->org[2] + right[2] * scale);
#else
			D_DrawParticle(p);
#endif
		}

		p->org[0] += p->vel[0] * frametime;
		p->org[1] += p->vel[1] * frametime;
		p->org[2] += p->vel[2] * frametime;

		switch (p->type)
		{
		case pt_grav:
			p->vel[2] -= grav * 20;
			break;

		case pt_slowgrav:
			p->vel[2] = grav;
			break;

		case pt_fire:
			p->ramp += time1;

			if (p->ramp >= 6)
			{
				p->die = -1;
			}
			else
			{
				p->color = ramp3[(int)p->ramp];
#if defined( GLQUAKE )
				p->packedColor = 0;
#else
				p->packedColor = hlRGB(host_basepal, p->color);
#endif
			}

			p->vel[2] += grav;
			break;

		case pt_explode:
			p->ramp += time2;

			if (p->ramp >= 8)
			{
				p->die = -1;
			}
			else
			{
				p->color = ramp1[(int)p->ramp];
#if defined( GLQUAKE )
				p->packedColor = 0;
#else
				p->packedColor = hlRGB(host_basepal, p->color);
#endif
			}

			for (i = 0; i < 3; i++)
				p->vel[i] *= (dvel + 1.0);

			p->vel[2] -= grav;
			break;

		case pt_explode2:
			p->ramp += time3;

			if (p->ramp >= 8)
			{
				p->die = -1;
			}
			else
			{
				p->color = ramp2[(int)p->ramp];
#if defined( GLQUAKE )
				p->packedColor = 0;
#else
				p->packedColor = hlRGB(host_basepal, p->color);
#endif
			}

			for (i = 0; i < 3; i++)
				p->vel[i] *= (1.0 - frametime);

			p->vel[2] -= grav;
			break;

		case pt_blob:
		case pt_blob2:
			p->ramp += time2;

			if (p->ramp >= 9) // bounds check
				p->ramp = 0;

			// set spark color
			p->color = gSparkRamp[(int)p->ramp];
#if defined( GLQUAKE )
			p->packedColor = 0;
#else
			p->packedColor = hlRGB(host_basepal, p->color);
#endif

			p->vel[0] *= (1.0 - (frametime * 0.5));
			p->vel[1] *= (1.0 - (frametime * 0.5));
			p->vel[2] -= grav * 5.0;

			if (RandomLong(0, 3))
			{
				p->type = pt_blob;
			}
			else
			{
				p->type = pt_blob2;
			}
			break;

		case pt_vox_slowgrav:
			p->vel[2] -= grav * 4;
			break;

		case pt_vox_grav:
			p->vel[2] -= grav * 8;
			break;
		}
	}

#ifdef GLQUAKE
	qglEnd();
#endif

	R_TracerDraw();
	R_BeamDrawList();

#ifdef GLQUAKE
	qglDisable(GL_BLEND);
	qglDisable(GL_ALPHA_TEST);
#else
	D_EndParticles();
#endif
}

float	gTracerSize[10] = { 1.5, 0.5, 1, 1, 1, 1, 1, 1, 1, 1 };

/*
===============
R_TracerDraw

Draw tracer effects, like sparks, gun tracers etc
===============
*/
void R_TracerDraw( void )
{
	float		scale;
	float		attenuation;
	float		frametime;
	float		clipDist;
	float		gravity;
	float		size;
	vec3_t		up, right;
	vec3_t		start, end;
	particle_t* p;
	vec3_t		screenLast, screen;
	vec3_t		tmp, normal;
//	int			clip;

	qboolean	draw = TRUE;

	if (!gpActiveTracers)
		return; // no tracers to draw

	gTracerColors[4].r = tracerRed.value * tracerAlpha.value * 255;
	gTracerColors[4].g = tracerGreen.value * tracerAlpha.value * 255;
	gTracerColors[4].b = tracerBlue.value * tracerAlpha.value * 255;

	frametime = cl.time - cl.oldtime;

	R_FreeDeadParticles(&gpActiveTracers);

	VectorScale(vup, 1.5, up);
	VectorScale(vright, 1.5, right);

	if (R_TriangleSpriteTexture(cl_sprite_dot, 0))
	{
		gravity = sv_gravity.value * frametime;
		size = DotProduct(r_origin, vpn);

		scale = 1.0 - frametime * 0.9;
		if (scale < 0.0)
			scale = 0.0;

		tri_GL_RenderMode(kRenderTransAdd);
		tri_GL_CullFace(TRI_NONE);

		for (p = gpActiveTracers; p; p = p->next)
		{
			color24* pColor;

			pColor = &gTracerColors[p->color];

			attenuation = (p->die - cl.time);
			if (attenuation > 0.1)
				attenuation = 0.1;

			VectorScale(p->vel, (p->ramp * attenuation), end);
			VectorAdd(p->org, end, end);
			VectorCopy(p->org, start);

			draw = TRUE;

			if (ScreenTransform(start, screen) || ScreenTransform(end, screenLast))
			{
				float fraction;
				float dist1, dist2;

				dist1 = DotProduct(vpn, start) - size;
				dist2 = DotProduct(vpn, end) - size;

				if (dist1 <= 0.0 && dist2 <= 0.0)
					draw = FALSE;

				if (draw == TRUE)
				{
					clipDist = dist2 - dist1;
					if (clipDist < 0.01)
						draw = FALSE;
				}

				if (draw == TRUE)
				{
					fraction = dist1 / clipDist;

					if (dist1 > 0)
					{
						end[0] = start[0] + (end[0] - start[0]) * fraction;
						end[1] = start[1] + (end[1] - start[1]) * fraction;
						end[2] = start[2] + (end[2] - start[2]) * fraction;
					}
					else
					{
						start[0] = start[0] + (end[0] - start[0]) * fraction;
						start[1] = start[1] + (end[1] - start[1]) * fraction;
						start[2] = start[2] + (end[2] - start[2]) * fraction;
					}

					// Transform point into screen space
					ScreenTransform(start, screen);
					ScreenTransform(end, screenLast);
				}
			}

			if (draw == TRUE)
			{
				// Transform point into screen space
				ScreenTransform(start, screen);
				ScreenTransform(end, screenLast);

				// Build world-space normal to screen-space direction vector
				VectorSubtract(screenLast, screen, tmp);

				// We don't need Z, we're in screen space
				tmp[2] = 0;

				VectorNormalize(tmp);

				// build point along noraml line (normal is -y, x)
				VectorScale(vup, tmp[0] * gTracerSize[p->type], normal);
				VectorMA(normal, -tmp[1] * gTracerSize[p->type], vright, normal);

				qglBegin(GL_QUADS);

				tri_GL_Color4ub(pColor->r, pColor->g, pColor->b, p->packedColor);

				tri_GL_Brightness(0);
				qglTexCoord2f(0, 0);
				qglVertex3f(start[0] + normal[0], start[1] + normal[1], start[2] + normal[2]);

				tri_GL_Brightness(1);
				qglTexCoord2f(0, 1);
				qglVertex3f(end[0] + normal[0], end[1] + normal[1], end[2] + normal[2]);

				tri_GL_Brightness(1);
				qglTexCoord2f(1, 1);
				qglVertex3f(end[0] - normal[0], end[1] - normal[1], end[2] - normal[2]);

				tri_GL_Brightness(0);
				qglTexCoord2f(1, 0);
				qglVertex3f(start[0] - normal[0], start[1] - normal[1], start[2] - normal[2]);

				p->org[0] = frametime * p->vel[0] + p->org[0];
				p->org[1] = frametime * p->vel[1] + p->org[1];
				p->org[2] = frametime * p->vel[2] + p->org[2];

				if (p->type == pt_grav)
				{
					p->vel[0] *= scale;
					p->vel[1] *= scale;
					p->vel[2] -= gravity;

					p->packedColor = 255 * (p->die - cl.time) * 2;

					if (p->packedColor > 255)
						p->packedColor = 255;
				}
				else if (p->type == pt_slowgrav)
				{
					p->vel[2] = gravity * 0.05;
				}

				qglEnd();
			}
		}

		tri_GL_CullFace(TRI_FRONT);
		tri_GL_RenderMode(kRenderNormal);
	}
}

//-----------------------------------------------------------------------------
//
// Beams
//
//-----------------------------------------------------------------------------

BEAM* R_BeamAlloc( void )
{
	BEAM* pbeam;

	if (!gpFreeBeams)
		return NULL;

	pbeam = gpFreeBeams;
	gpFreeBeams = pbeam->next;
	pbeam->next = gpActiveBeams;
	gpActiveBeams = pbeam;

	return pbeam;
}

BEAM* R_BeamLightning( vec_t* start, vec_t* end, int modelIndex, float life, float width, float amplitude, float brightness, float speed )
{
	BEAM* pbeam;

	pbeam = R_BeamAlloc();
	if (!pbeam)
		return NULL;
	
	pbeam->die = cl.time;

	if (modelIndex < 0)
		return NULL;

	R_BeamSetup(pbeam, start, end, modelIndex, life, width, amplitude, brightness, speed);

	return pbeam;
}

void R_BeamSetup( BEAM* pbeam, vec_t* start, vec_t* end, int modelIndex, float life, float width, float amplitude, float brightness, float speed )
{
	model_t* sprite;

	sprite = cl.model_precache[modelIndex];
	if (!sprite)
		return;

	pbeam->type = TE_BEAMPOINTS;
	pbeam->modelIndex = modelIndex;
	pbeam->frame = 0;
	pbeam->frameRate = 0;
	pbeam->frameCount = ModelFrameCount(sprite);

	VectorCopy(start, pbeam->source);
	VectorCopy(end, pbeam->target);
	VectorSubtract(end, start, pbeam->delta);

	pbeam->freq = cl.time * speed;
	pbeam->die = cl.time + life;
	pbeam->width = width;
	pbeam->amplitude = amplitude;
	pbeam->brightness = brightness;
	pbeam->speed = speed;

	if (amplitude >= 0.5)
		pbeam->segments = Length(pbeam->delta) * 0.25 + 3;	// once per 4 pixels
	else
		pbeam->segments = Length(pbeam->delta) * 0.075 + 3; // once per 16 pixels

	pbeam->flags = 0;
}

void SetBeamAttributes( BEAM* pbeam, float r, float g, float b, float framerate, int startFrame )
{
	pbeam->frameRate = framerate;
	pbeam->frame = startFrame;

	pbeam->r = r;
	pbeam->g = g;
	pbeam->b = b;
}

// Initialize beam from server entity
void R_DrawBeamEntList( float frametime )
{
	BEAM	beam;
	int		i;
	int		beamType;

	cl_entity_t* ent;

	for (i = 0; i < cl_numbeamentities; i++)
	{
		ent = &cl_beamentities[i];

		// Set up the beam
		beamType = ent->rendermode & 0xF;

		R_BeamSetup(&beam, ent->origin, ent->angles, ent->movetype, 0.0, ent->scale, ent->body * 0.01,
			CL_FxBlend(ent) / 255.0, ent->animtime);

		SetBeamAttributes(&beam,
			ent->rendercolor.r / 255.0,
			ent->rendercolor.g / 255.0,
			ent->rendercolor.b / 255.0,
			0.0,
			ent->frame);
		
		// Handle code from relinking.
		switch (beamType)
		{
		case BEAM_ENTPOINT:
			beam.type = TE_BEAMPOINTS;
			beam.flags = FBEAM_ENDENTITY;
			beam.startEntity = 0;
			beam.endEntity = ent->skin;
			break;

		case BEAM_ENTS:
			beam.type = TE_BEAMPOINTS;
			beam.flags = (FBEAM_STARTENTITY | FBEAM_ENDENTITY);
			beam.startEntity = ent->sequence;
			beam.endEntity = ent->skin;
			break;

		case BEAM_POINTS:
			// Already set up
			break;
		}

		if (ent->rendermode & BEAM_FSINE)
			beam.flags |= FBEAM_SINENOISE;
		if (ent->rendermode & BEAM_FSOLID)
			beam.flags |= FBEAM_SOLID;
		if (ent->rendermode & BEAM_FSHADEIN)
			beam.flags |= FBEAM_SHADEIN;
		if (ent->rendermode & BEAM_FSHADEOUT)
			beam.flags |= FBEAM_SHADEOUT;

		// Draw it
		R_BeamDraw(&beam, frametime);
	}
}

BEAM* R_BeamEnts( int startEnt, int endEnt, int modelIndex, float life, float width, float amplitude, float brightness, float speed, int startFrame, float framerate, float r, float g, float b )
{
	BEAM* pbeam;
	cl_entity_t* start, * end;

	if (life != 0.0)
	{
		start = &cl_entities[BEAMENT_ENTITY(startEnt)];
		if (!start->model)
			return NULL;

		end = &cl_entities[BEAMENT_ENTITY(endEnt)];
		if (!end->model)
			return NULL;
	}

	pbeam = R_BeamLightning(vec3_origin, vec3_origin, modelIndex, life, width, amplitude, brightness, speed);
	if (!pbeam)
		return NULL;

	pbeam->type = TE_BEAMPOINTS;
	pbeam->flags = (FBEAM_STARTENTITY | FBEAM_ENDENTITY);

	if (life == 0.0)
	{
		pbeam->flags |= FBEAM_FOREVER;
	}

	pbeam->startEntity = startEnt;
	pbeam->endEntity = endEnt;

	SetBeamAttributes(pbeam, r, g, b, framerate, startFrame);

	return pbeam;
}

// Creates a beam between an entity and a point
BEAM* R_BeamEntPoint( int startEnt, vec_t* end, int modelIndex, float life, float width, float amplitude, float brightness, float speed, int startFrame, float framerate, float r, float g, float b )
{
	BEAM* pbeam;
	cl_entity_t* start;

	if (life != 0.0)
	{
		start = &cl_entities[BEAMENT_ENTITY(startEnt)];
		if (!start->model)
			return NULL;
	}

	pbeam = R_BeamLightning(vec3_origin, end, modelIndex, life, width, amplitude, brightness, speed);
	if (!pbeam)
		return NULL;

	pbeam->type = TE_BEAMPOINTS;
	pbeam->flags = FBEAM_STARTENTITY;

	if (life == 0.0)
	{
		pbeam->flags |= FBEAM_FOREVER;
	}

	pbeam->startEntity = startEnt;
	pbeam->endEntity = 0;

	SetBeamAttributes(pbeam, r, g, b, framerate, startFrame);

	return pbeam;
}

BEAM* R_BeamPoints( vec_t* start, vec_t* end, int modelIndex, float life, float width, float amplitude, float brightness, float speed, int startFrame, float framerate, float r, float g, float b )
{
	BEAM* pbeam;

	// don't start temporary beams out of the PVS
	if (life != 0.0 && !R_BeamCull(start, end, TRUE))
		return NULL;

	pbeam = R_BeamLightning(start, end, modelIndex, life, width, amplitude, brightness, speed);
	if (!pbeam)
		return NULL;

	if (life == 0.0)
	{
		pbeam->flags |= FBEAM_FOREVER;
	}

	SetBeamAttributes(pbeam, r, g, b, framerate, startFrame);

	return pbeam;
}

BEAM* R_BeamCirclePoints( int type, vec_t* start, vec_t* end, int modelIndex, float life, float width, float amplitude, float brightness, float speed, int startFrame, float framerate, float r, float g, float b )
{
	BEAM* pbeam;

	pbeam = R_BeamLightning(start, end, modelIndex, life, width, amplitude, brightness, speed);
	if (!pbeam)
		return NULL;

	pbeam->type = type;

	if (life == 0.0)
	{
		pbeam->flags |= FBEAM_FOREVER;
	}

	SetBeamAttributes(pbeam, r, g, b, framerate, startFrame);

	return pbeam;
}

BEAM* R_BeamFollow( int startEnt, int modelIndex, float life, float width, float r, float g, float b, float brightness )
{
	BEAM* pbeam;

	pbeam = R_BeamLightning(vec3_origin, vec3_origin, modelIndex, life, width, life, brightness, 1.0);
	if (!pbeam)
		return NULL;

	pbeam->type = TE_BEAMFOLLOW;
	pbeam->flags = FBEAM_STARTENTITY;

	pbeam->startEntity = startEnt;

	SetBeamAttributes(pbeam, r, g, b, 1.0, 0);

	return pbeam;
}

// Create a beam ring between two entities
BEAM* R_BeamRing( int startEnt, int endEnt, int modelIndex, float life, float width, float amplitude, float brightness, float speed, int startFrame, float framerate, float r, float g, float b )
{
	BEAM* pbeam;
	cl_entity_t* start, * end;

	if (life != 0.0)
	{
		start = &cl_entities[startEnt];
		if (!start->model)
			return NULL;

		end = &cl_entities[endEnt];
		if (!end->model)
			return NULL;
	}

	pbeam = R_BeamLightning(vec3_origin, vec3_origin, modelIndex, life, width, amplitude, brightness, speed);
	if (!pbeam)
		return NULL;

	pbeam->type = TE_BEAMRING;
	pbeam->flags = (FBEAM_STARTENTITY | FBEAM_ENDENTITY);

	if (life == 0.0)
	{
		pbeam->flags |= FBEAM_FOREVER;
	}

	pbeam->startEntity = startEnt;
	pbeam->endEntity = endEnt;
	
	SetBeamAttributes(pbeam, r, g, b, framerate, startFrame);

	return pbeam;
}

// Iterates through active list and kills beams associated with deadEntity
void R_KillDeadBeams( int deadEntity )
{
	BEAM* pbeam;
	BEAM* pnewlist;
	BEAM* pnext;
	particle_t* pHead;

	pbeam = gpActiveBeams;  // Old list.
	pnewlist = NULL;		// New list.

	while (pbeam)
	{
		pnext = pbeam->next;
		if (pbeam->startEntity != deadEntity)   // Link into new list.
		{
			pbeam->next = pnewlist;
			pnewlist = pbeam;

			pbeam = pnext;
			continue;
		}

		pbeam->flags &= ~(FBEAM_STARTENTITY | FBEAM_ENDENTITY);
		if (pbeam->type != TE_BEAMFOLLOW)
		{
			// Die Die Die!
			pbeam->die = cl.time - 0.1;

			// Kill off particles
			pHead = pbeam->particles;
			while (pHead)
			{
				pHead->die = cl.time - 0.1;
				pHead = pHead->next;
			}

			// Free particles that have died off.
			R_FreeDeadParticles(&pbeam->particles);

			// Clear us out
			memset(pbeam, 0, sizeof(*pbeam));

			// Now link into free list;
			pbeam->next = gpFreeBeams;
			gpFreeBeams = pbeam;
		}
		else
		{
			// Stay active
			pbeam->next = pnewlist;
			pnewlist = pbeam;
		}
		pbeam = pnext;
	}

	// We now have a new list with the bogus stuff released.
	gpActiveBeams = pnewlist;
}

void R_BeamKill( int deadEntity )
{
	BEAM* pbeam;

	pbeam = gpActiveBeams;
	while (pbeam)
	{
		if ((pbeam->flags & FBEAM_STARTENTITY) && pbeam->startEntity == deadEntity)
		{
			pbeam->flags &= ~(FBEAM_STARTENTITY | FBEAM_FOREVER);

			if (pbeam->type != TE_BEAMFOLLOW)
				pbeam->die = cl.time;
		}

		if ((pbeam->flags & FBEAM_ENDENTITY) && pbeam->endEntity == deadEntity)
		{
			pbeam->flags &= ~(FBEAM_ENDENTITY | FBEAM_FOREVER);
			pbeam->die = cl.time;
		}

		pbeam = pbeam->next;
	}
}

/*
===============
R_Implosion
===============
*/
void R_Implosion( vec_t* end, float radius, int count, float life )
{
	int		i;
	vec3_t	start, temp;
	vec3_t	vel;

	for (i = 0; i < count; i++)
	{
		temp[0] = (radius / 100) * RandomFloat(-100, 100);
		temp[1] = (radius / 100) * RandomFloat(-100, 100);
		temp[2] = (radius / 100) * RandomFloat(0, 100);

		VectorAdd(temp, end, start);
		VectorScale(temp, -1.0 / life, vel);

		R_TracerParticles(start, vel, life);
	}
}

#define NOISE_DIVISIONS		128
float	gNoise[NOISE_DIVISIONS + 1];

//		freq2 += step * 0.1;
// Fractal noise generator, power of 2 wavelength
void Noise( float* noise, int divs )
{
	int div2;

	div2 = divs >> 1;

	if (divs < 2)
		return;

	// noise is normalized to +/- scale
	noise[div2] = (noise[0] + noise[divs]) * 0.5 + divs * RandomFloat(-0.125, 0.125);
	if (div2 > 1)
	{
		Noise(&noise[div2], div2);
		Noise(noise, div2);
	}
}

void SineNoise( float* noise, int divs )
{
	int i;
	float freq, freq2;
	float step = M_PI / (float)divs;

	freq = 0;
	freq2 = 0;
	for (i = 0; i < divs; i++)
	{
		noise[i] = sin(freq);
		freq += step;
	}
}

// Converts a world coordinate to a screen coordinate
// Returns true if it's Z clipped, false otherwise
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

/*
===============
R_DrawSegs

Draw segmented beams
===============
*/
void R_DrawSegs( vec_t* source, vec_t* delta, float width, float scale, float freq, float speed, int segments, int flags )
{
	int				i, noiseIndex, noiseStep;
	float			div, length, fraction, factor, vLast, vStep, brightness;
	vec3_t			last1, last2, point, screen, screenLast, tmp, normal;

	if (segments < 2)
		return;
	
	if (segments > NOISE_DIVISIONS)		// UNDONE: Allow more segments?
	{
		segments = NOISE_DIVISIONS;
	}

	length = Length(delta) * 0.01;

	// Don't lose all of the noise/texture on short beams
	if (length < 0.5)
		length = 0.5;

	div = 1.0 / (segments - 1);

	vStep = length * div;	// Texture length texels per space pixel

	vLast = fmod(freq * speed, 1);	// Scroll speed 3.5 -- initial texture position, scrolls 3.5/sec (1.0 is entire texture)

	if (flags & FBEAM_SINENOISE)
	{
		if (segments < 16)
			segments = 16;

		scale *= 100;
		length = segments * (1.0 / 10);
	}
	else
	{
		scale *= length;
	}

	ScreenTransform(source, screenLast);
	VectorMA(source, div, delta, point);
	ScreenTransform(point, screen);

	// build world-space normal to screen-space direction vector
	VectorSubtract(screen, screenLast, tmp);
	// we don't need Z, we're in screen space
	tmp[2] = 0.0;
	VectorNormalize(tmp);
	VectorScale(vup, tmp[0], normal);	// Build start along noraml line (normal is -y, x)
	VectorMA(normal, -tmp[1], vright, normal);

	// Make a wide line
	VectorMA(source, width, normal, last1);
	VectorMA(source, -width, normal, last2);

	// Iterator to resample noise waveform (it needs to be generated in powers of 2)
	noiseStep = (int)((float)NOISE_DIVISIONS * div * 65536.0);
	noiseIndex = noiseStep;
	
	// Sine noise beams have different length calculations
	if (flags & FBEAM_SINENOISE)
	{
		noiseIndex = 0;
	}

	brightness = 1.0f;
	if (flags & FBEAM_SHADEIN)
		brightness = 0.0;

	for (i = 1; i < segments; i++)
	{
		fraction = i * div;

		tri_GL_Brightness(brightness);
		qglTexCoord2f(0, vLast);
		qglVertex3fv(last1);
		tri_GL_Brightness(brightness);
		qglTexCoord2f(1, vLast);
		qglVertex3fv(last2);

		if (flags & FBEAM_SHADEIN)
		{
			brightness = fraction;
		}
		else if (flags & FBEAM_SHADEOUT)
		{
			brightness = 1.0 - fraction;
		}

		VectorMA(source, fraction, delta, point);

		// Distort using noise
		if (scale != 0)
		{
			factor = gNoise[noiseIndex >> 16] * scale;

			if (flags & FBEAM_SINENOISE)
			{
				VectorMA(point, factor * sin(fraction * M_PI * length + freq), vup, point);
				// rotate the noise along the perpendicluar axis a bit to keep the bolt from looking diagonal
				VectorMA(point, factor * cos(fraction * M_PI * length + freq), vright, point);
			}
			else
			{
				VectorMA(point, factor, vup, point);
				VectorMA(point, factor * cos(fraction * M_PI * 3.0 + freq), vright, point);
			}
		}

		ScreenTransform(point, screen);

		// build world-space normal to screen-space direction vector
		VectorSubtract(screen, screenLast, tmp);

		// we don't need Z, we're in screen space
		tmp[2] = 0.0;

		VectorNormalize(tmp);
		VectorScale(vup, tmp[0], normal);	// Build start along noraml line (normal is -y, x)
		VectorMA(normal, -tmp[1], vright, normal);

		// Make a wide line
		VectorMA(point, width, normal, last1);
		VectorMA(point, -width, normal, last2);

		vLast += vStep; // advance texture scroll (v axis only)
		tri_GL_Brightness(brightness);
		qglTexCoord2f(1, vLast);
		qglVertex3fv(last2);
		tri_GL_Brightness(brightness);
		qglTexCoord2f(0, vLast);
		qglVertex3fv(last1);

		VectorCopy(screen, screenLast);

		noiseIndex += noiseStep;
		vLast = fmod(vLast, 1.0);
	};
}




// TODO: Implement

/*
===============
R_BeamCull

Cull beam by bbox
===============
*/
int R_BeamCull( vec_t* start, vec_t* end, int pvsOnly )
{
	// TODO: Implement
	return 0;
}

// Draw all beam entities
void R_BeamDraw( BEAM* pbeam, float frametime )
{
	// TODO: Implement
}

// TODO: Implement

// Update beams created by temp entity system
void R_BeamDrawList( void )
{
	// TODO: Implement
}

// TODO: Implement