// pmove.h
#if !defined( PMOVE_H )
#define PMOVE_H

// movevars_t                  // Physics variables.
typedef struct movevars_s movevars_t;

struct movevars_s
{
	float	gravity;           // Gravity for map
	float	stopspeed;         // Deceleration when not moving
	float	maxspeed;          // Max allowed speed
	float	spectatormaxspeed;
	float	accelerate;        // Acceleration factor
	float	airaccelerate;     // Same for when in open air
	float	wateraccelerate;   // Same for when in water
	float	friction;
	float	edgefriction;	   // Extra friction near dropofs 
	float	waterfriction;     // Less in water
	float	entgravity;        // 1.0
	float	bounce;            // Wall bounce value. 1.0
	float	stepsize;          // sv_stepsize;
	float	maxvelocity;       // maximum server velocity.
	float	zmax;			   // Max z-buffer range (for GL)
	float	waveHeight;		   // Water wave height (for GL)
	char	skyName[32];	   // Name of the sky map
};

extern movevars_t	movevars;

extern int			onground;

#endif // PMOVE_H