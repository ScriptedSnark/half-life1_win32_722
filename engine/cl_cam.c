/* ZOID
 *
 * Player camera tracking in Spectator mode
 *
 * This takes over player controls for spectator automatic camera.
 * Player moves as a spectator, but the camera tracks and enemy player
 */

#include "quakedef.h"
#include "winquake.h"

qboolean locked = FALSE;

// TODO: Implement

int spec_track = 0; // player# of who we are tracking
int autocam = CAM_NONE;