#include "quakedef.h"

#define MAX_BEAMS				128		// Max simultaneous beams
#define MAX_PARTICLES			2048	// default max # of particles at one
										//  time
#define ABSOLUTE_MIN_PARTICLES	512		// no fewer than this no matter what's
										//  on the command line

// TODO: Implement

particle_t* active_particles, * free_particles, * gpActiveTracers;

particle_t* particles;
int			r_numparticles;

// TODO: Implement

// Forward declarations
void R_TracerDraw( void );
void R_BeamDrawList( void );

// TODO: Implement

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

	// TODO: Implement
}

// TODO: Implement

/*
===============
R_ClearParticles
===============
*/
void R_ClearParticles( void )
{
	int		i;

	active_particles = NULL;
	gpActiveTracers = NULL;

	free_particles = &particles[0];

	for (i = 0; i < r_numparticles; i++)
		particles[i].next = &particles[i + 1];

	particles[r_numparticles - 1].next = NULL;

	// TODO: Implement
}

// TODO: Implement

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

	// TODO: Implement
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