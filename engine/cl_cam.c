/* ZOID
 *
 * Player camera tracking in Spectator mode
 *
 * This takes over player controls for spectator automatic camera.
 * Player moves as a spectator, but the camera tracks and enemy player
 */

#include "quakedef.h"
#include "winquake.h"




//======================================================= FINISH LINE

qboolean locked = FALSE;

// TODO: Implement

// pitch, yaw, dist
vec3_t cam_ofs;

int spec_track = 0; // player# of who we are tracking
int autocam = CAM_NONE;

// TODO: Implement

void Cam_GetPredictedTopDownOrigin( vec_t* vec )
{
	// TODO: Implement
}

// TODO: Implement

void Cam_GetTopDownOrigin( vec_t* source, vec_t* vec )
{
	// TODO: Implement
}

void Cam_GetPredictedFirstPersonOrigin( vec_t* vec )
{
	// TODO: Implement
}

// TODO: Implement



// TODO: Implement