// r_studio.c: routines for setting up to draw 3DStudio models 

#include "quakedef.h"

#if defined( GLQUAKE )
int				r_ambientlight;					// ambient world light
float			r_shadelight;					// direct world light

int				r_amodels_drawn;
int				r_smodels_total;				// cookie
#endif

// Get number of body variations
int R_StudioBodyVariations( model_t* model )
{
	// TODO: Implement
	return 0;
}

// TODO: Implement

/*
====================
R_StudioDrawModel

====================
*/
int R_StudioDrawModel( int flags )
{
	// TODO: Implement
	return 1;
}

/*
====================
R_StudioDrawPlayer

====================
*/
int R_StudioDrawPlayer( int flags, player_state_t* pplayer )
{
	// TODO: Implement
	return 1;
}

// TODO: Implement