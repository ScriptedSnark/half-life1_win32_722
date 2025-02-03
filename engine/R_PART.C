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

void R_RocketTrail( vec_t *start, vec_t *end, int type )
{
	// TODO: Implement
}

// TODO: Implement