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


#define	MAX_PHYSENTS 600 		  // Must have room for all entities in the world.
typedef struct
{
	vec3_t			origin;               // Model's origin in world coordinates.
	model_t*		model;		          // only for bsp models
	model_t*		studiomodel;		  // SOLID_BBOX, but studio clip intersections.
	vec3_t			mins, maxs;	          // only for non-bsp models
	int				info;		          // For client or server to use to identify (index into edicts or cl_entities)
	vec3_t			angles;               // rotated entities need this info for hull testing to work.

	int				solid;				  // Triggers and func_door type WATER brushes are SOLID_NOT
	int				skin;                 // BSP Contents for such things like fun_door water brushes.
	int				rendermode;			  // So we can ignore glass

	// Complex collision detection.
	float			frame;
	int				sequence;
	byte			controller[4];
	byte			blending[2];
} physent_t;


typedef struct
{
	int			player_index;	// So we don't try to run the PM_CheckStuck nudging too quickly.
	qboolean	server;			// For debugging, are we running physics code on server side?

	vec3_t		origin;			// Movement origin.
	vec3_t		angles;			// Movement view angles.
	vec3_t		velocity;		// Current movement direction.
	vec3_t		movedir;		// For waterjumping, a forced forward velocity so we can fly over lip of ledge.
	vec3_t		basevelocity;	// Velocity of the conveyor we are standing, e.g.

	// For ducking/dead
	vec3_t		view_ofs;		// Our eye position.

	int			flags;			// FL_ONGROUND, FL_DUCKING, etc.
	int			usehull;		// 0 = regular player hull, 1 = ducked player hull, 2 = point hull
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
	physent_t	physents[MAX_PHYSENTS];

	// input to run through physics.
	usercmd_t	cmd;
	
	// Trace results for objects we collided with.
	int				numtouch;
	pmtrace_t		touchindex[MAX_PHYSENTS];
} playermove_t;

// movevars_t                  // Physics variables.
typedef struct
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
extern	int		waterlevel;
extern	int		watertype;

extern	cvar_t	cl_showclip;
extern	cvar_t	cl_printclip;

extern	cvar_t	pm_nostudio;
extern	cvar_t	pm_nocomplex;
extern	cvar_t	pm_worldonly;
extern	cvar_t	pm_pushfix;
extern	cvar_t	pm_nostucktouch;

void PlayerMove( qboolean server );
void Pmove_Init( void );

int PM_TestPlayerPosition( float* pos );
pmtrace_t PM_PlayerMove( vec_t* start, vec_t* end, int traceFlags );

#endif // PMOVE_H