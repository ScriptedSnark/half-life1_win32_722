// pmove.h
#if !defined( PMOVE_H )
#define PMOVE_H

#define PM_NORMAL			0x00000000
#define PM_STUDIO_IGNORE	0x00000001		// Skip studio models
#define PM_STUDIO_BOX		0x00000002		// Use boxes for non-complex studio models (even in traceline)
#define PM_GLASS_IGNORE		0x00000004		// Ignore entities with non-normal rendermode

typedef struct
{
	vec3_t	normal;
	float	dist;
} pmplane_t;

typedef struct
{
	qboolean	allsolid;	      // if true, plane is not valid
	qboolean	startsolid;	      // if true, the initial point was in a solid area
	qboolean	inopen, inwater;  // End point is in empty space or in water
	float		fraction;		  // time completed, 1.0 = didn't hit anything
	vec3_t		endpos;			  // final position
	pmplane_t	plane;		      // surface normal at impact
	int			ent;			  // entity at impact
	vec3_t      deltavelocity;    // Change in player's velocity caused by impact.  
								  // Only run on server.
	int         hitgroup;
} pmtrace_t;

typedef struct
{
	// TODO: Implement
	
	qboolean	server;			// For debugging, are we running physics code on server side?

	vec3_t		origin;			// Movement origin.
	vec3_t		angles;			// Movement view angles.
	vec3_t		velocity;		// Current movement direction.
	vec3_t		movedir;		// For waterjumping, a forced forward velocity so we can fly over lip of ledge.
	vec3_t		basevelocity;	// Velocity of the conveyor we are standing, e.g.

	// For ducking/dead
	vec3_t		view_ofs;		// Our eye position.

	int			flags;			// FL_ONGROUND, FL_DUCKING, etc.

	int			usehull;

	// TODO: Implement
	

	float		gravity;		// Our current gravity and friction.
	float		friction;
	int			oldbuttons;		// Buttons last usercmd
	float		waterjumptime;	// Amount of time left in jumping out of water cycle.
	qboolean	dead;			// Are we a dead player?
	int			spectator;		// Should we use spectator physics model?
	int			movetype;		// Our movement type, NOCLIP, WALK, FLY

	float		maxspeed;

	// world state
	// Number of entities to clip against.
	int			numphysent;

	// TODO: Implement
	

	usercmd_t	cmd;


	// TODO: Implement

} playermove_t;

// movevars_t                  // Physics variables.
typedef struct movevars_s
{
	float		gravity;           // Gravity for map
	float		stopspeed;         // Deceleration when not moving
	float		maxspeed;          // Max allowed speed
	float		spectatormaxspeed;
	float		accelerate;        // Acceleration factor
	float		airaccelerate;     // Same for when in open air
	float		wateraccelerate;   // Same for when in water
	float		friction;
	float		edgefriction;	   // Extra friction near dropofs 
	float		waterfriction;     // Less in water
	float		entgravity;        // 1.0
	float		bounce;            // Wall bounce value. 1.0
	float		stepsize;          // sv_stepsize;
	float		maxvelocity;       // maximum server velocity.
	float		zmax;			   // Max z-buffer range (for GL)
	float		waveHeight;		   // Water wave height (for GL)
	char		skyName[32];	   // Name of the sky map
} movevars_t;

extern	movevars_t		movevars;
extern	playermove_t	pmove;
extern	int		onground;

pmtrace_t PM_PlayerTrace( vec_t* start, vec_t* end, int traceFlags );

#endif // PMOVE_H