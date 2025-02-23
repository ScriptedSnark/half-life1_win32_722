#include "quakedef.h"
#include "pmove.h"

movevars_t		movevars;

playermove_t	pmove;

int			onground;
int			waterlevel;
int			watertype;

float		frametime;

vec3_t		forward, right, up;

cvar_t	cl_showclip = { "cl_showclip", "0" };
cvar_t	cl_printclip = { "cl_printclip", "0" };

cvar_t	pm_nostudio = { "pm_nostudio", "0" };
cvar_t	pm_nocomplex = { "pm_nocomplex", "0" };
cvar_t	pm_worldonly = { "pm_worldonly", "0" };
cvar_t	pm_pushfix = { "pm_pushfix", "0" };
cvar_t	pm_nostucktouch = { "pm_nostucktouch", "0" };

vec3_t	player_mins[4] = {{-16, -16, -36}, {-16, -16, -18}, {0, 0, 0}, 0};
vec3_t	player_maxs[4] = {{16, 16, 36}, {16, 16, 18}, {0, 0, 0}, 0};

void PM_InitBoxHull( void );

void Pmove_Init( void )
{
	// TODO: Implement
}

char* PM_NameForContents( int contents )
{
	char* name;

	switch (contents)
	{
	case CONTENTS_EMPTY:
		name = "EMPTY";
		break;
	case CONTENTS_SOLID:
		name = "SOLID";
		break;
	case CONTENTS_WATER:
		name = "WATER";
		break;
	case CONTENTS_SLIME:
		name = "SLIME";
		break;
	case CONTENTS_LAVA:
		name = "LAVA";
		break;
	case CONTENTS_SKY:
		name = "SKY";
		break;
	case CONTENTS_ORIGIN:
		name = "ORIGIN";
		break;
	case CONTENTS_CLIP:
		name = "CLIP";
		break;
	case CONTENTS_CURRENT_0:
		name = "C0";
		break;
	case CONTENTS_CURRENT_90:
		name = "C90";
		break;
	case CONTENTS_CURRENT_180:
		name = "C180";
		break;
	case CONTENTS_CURRENT_270:
		name = "C270";
		break;
	case CONTENTS_CURRENT_UP:
		name = "CUP";
		break;
	case CONTENTS_CURRENT_DOWN:
		name = "CDOWN";
		break;
	case CONTENTS_TRANSLUCENT:
		name = "TRANSLUCENT";
		break;
	default:
		name = "UNKNOWN";
		break;
	}

	return name;
}

// TODO: Implement

/*
=============
PlayerMove

Returns with origin, angles, and velocity modified in place.

Numtouch and touchindex[] will be set if any of the physents
were contacted during the move.
=============
*/
void PlayerMove( qboolean server )
{
	// Are we running server code?
	pmove.server = server;

	// Assume we don't touch anything
	pmove.numtouch = 0;

	// # of msec to apply movement
	frametime = pmove.cmd.msec * 0.001;

	// Convert view angles to vectors
	AngleVectors(pmove.angles, forward, right, up);

	// TODO: Implement
}