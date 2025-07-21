/* ZOID
 *
 * Player camera tracking in Spectator mode
 *
 * This takes over player controls for spectator automatic camera.
 * Player moves as a spectator, but the camera tracks and enemy player
 */

#include "quakedef.h"
#include "winquake.h"
#include "pmove.h"

static vec3_t desired_position; // where the camera wants to be
static qboolean locked = FALSE;

// track high fragger
cvar_t cl_hightrack = { "cl_hightrack", "0" };

//cvar_t cl_chasecam = { "cl_chasecam", "0" };

//cvar_t cl_camera_maxpitch = { "cl_camera_maxpitch", "10" };
//cvar_t cl_camera_maxyaw = { "cl_camera_maxyaw", "30" };

qboolean cam_forceview;
vec3_t cam_viewangles;
double cam_lastviewtime;

int spec_track = 0; // player# of who we are tracking
int autocam = CAM_NONE;

qboolean Cam_IsTracking( int cam );

static void vectoangles( vec_t* vec, vec_t* ang )
{
	float	forward;
	float	yaw, pitch;

	if (vec[1] == 0 && vec[0] == 0)
	{
		yaw = 0;
		if (vec[2] > 0)
			pitch = 90;
		else
			pitch = 270;		
	}
	else
	{
		yaw = (int)(atan2(vec[1], vec[0]) * 180 / M_PI);
		if (yaw < 0)
			yaw += 360;

		forward = sqrt(vec[0] * vec[0] + vec[1] * vec[1]);
		pitch = (int)(atan2(vec[2], forward) * 180 / M_PI);
		if (pitch < 0)
			pitch += 360;
	}

	ang[0] = pitch;
	ang[1] = yaw;
	ang[2] = 0;
}

static float vlen( vec_t* v )
{
	return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

void Cam_Unlock( void )
{
	if (Cam_IsTracking(autocam))
	{
		MSG_WriteByte(&cls.netchan.message, clc_stringcmd);
		MSG_WriteString(&cls.netchan.message, "ptrack");
		locked = FALSE;
	}
}

void Cam_Lock( int playernum )
{
	char st[40];

	sprintf(st, "ptrack %i", playernum);
	MSG_WriteByte(&cls.netchan.message, clc_stringcmd);
	MSG_WriteString(&cls.netchan.message, st);
	spec_track = playernum;
	cam_forceview = TRUE;
	locked = FALSE;
}

pmtrace_t Cam_DoTrace( vec_t* vec1, vec_t* vec2 )
{
#if 0
	memset(&pmove, 0, sizeof(pmove));

	pmove.numphysent = 1;
	VectorCopy(vec3_origin, pmove.physents[0].origin);
	pmove.physents[0].model = cl.worldmodel;
#endif

	VectorCopy(vec1, pmove.origin);
	return PM_PlayerMove(pmove.origin, vec2, 0);
}

// Returns distance or 9999 if invalid for some reason
static float Cam_TryFlyby( player_state_t* self, player_state_t* player, vec_t* vec, qboolean checkvis )
{
	// TODO: Implement
}

// Is player visible?
static qboolean Cam_IsVisible( player_state_t* player, vec_t* vec )
{
	pmtrace_t trace;
	vec3_t v;
	float d;

	trace = Cam_DoTrace(player->origin, vec);
	if (trace.fraction != 1 || /*trace.inopen ||*/ trace.inwater)
		return FALSE;
	// check distance, don't let the player get too far away or too close
	VectorSubtract(player->origin, vec, v);
	d = vlen(v);
	if (d < 8)
		return FALSE;
	return TRUE;
}

static qboolean InitFlyby( player_state_t* self, player_state_t* player, int checkvis )
{
	float f, max;
	vec3_t vec, vec2;
	vec3_t forward, right, up;

	VectorCopy(player->viewangles, vec);
	vec[0] = 0;
	AngleVectors(vec, forward, right, up);
//	for (i = 0; i < 3; i++)
//		forward[i] *= 3;

	max = 1000;

	if (autocam != CAM_FIRSTPERSON && autocam != CAM_TOPDOWN)
	{
		VectorAdd(forward, up, vec2);
		VectorAdd(vec2, right, vec2);
		if ((f = Cam_TryFlyby(self, player, vec2, checkvis)) < max)
		{
			max = f;
			VectorCopy(vec2, vec);
		}
		VectorAdd(forward, up, vec2);
		VectorSubtract(vec2, right, vec2);
		if ((f = Cam_TryFlyby(self, player, vec2, checkvis)) < max)
		{
			max = f;
			VectorCopy(vec2, vec);
		}
		VectorAdd(forward, right, vec2);
		if ((f = Cam_TryFlyby(self, player, vec2, checkvis)) < max)
		{
			max = f;
			VectorCopy(vec2, vec);
		}
		VectorSubtract(forward, right, vec2);
		if ((f = Cam_TryFlyby(self, player, vec2, checkvis)) < max)
		{
			max = f;
			VectorCopy(vec2, vec);
		}
		VectorAdd(forward, up, vec2);
		if ((f = Cam_TryFlyby(self, player, vec2, checkvis)) < max)
		{
			max = f;
			VectorCopy(vec2, vec);
		}
		VectorSubtract(forward, up, vec2);
		if ((f = Cam_TryFlyby(self, player, vec2, checkvis)) < max)
		{
			max = f;
			VectorCopy(vec2, vec);
		}
		VectorAdd(up, right, vec2);
		VectorSubtract(vec2, forward, vec2);
		if ((f = Cam_TryFlyby(self, player, vec2, checkvis)) < max)
		{
			max = f;
			VectorCopy(vec2, vec);
		}
		VectorSubtract(up, right, vec2);
		VectorSubtract(vec2, forward, vec2);
		if ((f = Cam_TryFlyby(self, player, vec2, checkvis)) < max)
		{
			max = f;
			VectorCopy(vec2, vec);
		}
		// invert
		VectorSubtract(vec3_origin, forward, vec2);
		if ((f = Cam_TryFlyby(self, player, vec2, checkvis)) < max)
		{
			max = f;
			VectorCopy(vec2, vec);
		}
		VectorCopy(forward, vec2);
		if ((f = Cam_TryFlyby(self, player, vec2, checkvis)) < max)
		{
			max = f;
			VectorCopy(vec2, vec);
		}
		// invert
		VectorSubtract(vec3_origin, right, vec2);
		if ((f = Cam_TryFlyby(self, player, vec2, checkvis)) < max)
		{
			max = f;
			VectorCopy(vec2, vec);
		}
		VectorCopy(right, vec2);
		if ((f = Cam_TryFlyby(self, player, vec2, checkvis)) < max)
		{
			max = f;
			VectorCopy(vec2, vec);
		}
	}

	// ack, can't find him
	if (max >= 1000)
	{
		Cam_Unlock();
		return FALSE;
	}
	locked = TRUE;
	VectorCopy(vec, desired_position);
	return TRUE;
}

static void Cam_CheckHighTarget( void )
{
	int i, j;
	player_info_t* s;

	j = -1;
	for (i = 0; i < MAX_CLIENTS; i++)
	{
		s = &cl.players[i];
		if (s->name[0] && !s->spectator)
		{
			j = i;
		}
	}
	if (j >= 0)
	{
		if (!locked)
			Cam_Lock(j);
	}
	else
		Cam_Unlock();
}

void Cam_GetPredictedTopDownOrigin( vec_t* v )
{
	float height;
	vec3_t vec;

	height = vec3_origin[2] + 800;

	vec[0] = 0;
	vec[1] = 0;
	vec[2] = vec3_origin[2] - height;
	vectoangles(vec, v);
	v[0] = -v[0];
}

void Cam_GetTopDownOrigin( vec_t* source, vec_t* dest )
{
	vec3_t v;
	pmtrace_t trace;
	float len;

	VectorCopy(source, v);
	v[2] += 800;
	// v is endpos
	// fake a player move
	trace = Cam_DoTrace(source, v);
	if (trace.fraction < 1.0)
		trace.endpos[2] -= 1;

	VectorSubtract(trace.endpos, source, v);
	VectorScale(v, 0.5, v);
	VectorAdd(source, v, dest);
	len = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	if (len < 32)
	{
		VectorCopy(source, dest);
		dest[2] += 32;
	}
}

void Cam_GetPredictedFirstPersonOrigin( vec_t* v )
{
	int		i;
	float	f, ft, lerp;
	float	angleDelta, originDelta;
	vec3_t	curAngles, oldAngles, newAngles;
	frame_t* frame, * prevframe;
	player_state_t* state, * prevstate;

	frame = &cl.frames[cls.netchan.incoming_sequence & UPDATE_MASK];
	prevframe = &cl.frames[(cls.netchan.incoming_sequence - 1) & UPDATE_MASK];

	state = &frame->playerstate[spec_track];
	prevstate = &prevframe->playerstate[spec_track];

	ft = state->received_time - prevstate->received_time;
	if (ft == 0 || prevframe->receivedtime == -1 || frame->receivedtime == -1)
	{
		// can't interpolate
		VectorCopy(state->viewangles, v);
		return;
	}

	// calculate interpolation
	f = realtime - state->received_time;
	if (f < 0)
		f = 0;
	lerp = f / ft;
	if (lerp > 1)
		lerp = 1;

	VectorCopy(state->viewangles, curAngles);
	VectorCopy(prevstate->viewangles, oldAngles);

	// interpolate view angles and origin
	for (i = 0; i < 3; i++)
	{
		if (curAngles[i] < 0)
			curAngles[i] += 360;
		if (oldAngles[i] < 0)
			oldAngles[i] += 360;

		angleDelta = curAngles[i] - oldAngles[i];
		while (angleDelta > 180)
			angleDelta -= 360;
		while (angleDelta <= -180)
			angleDelta += 360;
		newAngles[i] = curAngles[i] + angleDelta * lerp;

		originDelta = state->origin[i] - prevstate->origin[i];
		cl.simorg[i] = state->origin[i] + originDelta * lerp;
	}

	VectorCopy(newAngles, v);
}

void Cam_TrackFirstPerson( void )
{
	player_state_t* player, * self;
	frame_t* frame;

	if (!Cam_IsTracking(autocam) || cls.state != ca_active)
		return;

	if (locked && (!cl.players[spec_track].name[0] || cl.players[spec_track].spectator))
	{
		locked = FALSE;
		if (cl_hightrack.value)
			Cam_CheckHighTarget();
		else
			Cam_Unlock();		
		return;
	}

	frame = &cl.frames[cls.netchan.incoming_sequence & UPDATE_MASK];
	player = frame->playerstate + spec_track;
	self = frame->playerstate + cl.playernum;

	VectorCopy(player->origin, desired_position);

	// move there locally immediately
	VectorCopy(desired_position, self->origin);

	MSG_WriteByte(&cls.netchan.message, clc_tmove);
	MSG_WriteCoord(&cls.netchan.message, desired_position[0]);
	MSG_WriteCoord(&cls.netchan.message, desired_position[1]);
	MSG_WriteCoord(&cls.netchan.message, desired_position[2]);
}

void Cam_TrackTopDown( void )
{
	player_state_t* player, * self;
	frame_t* frame;
	vec3_t v;

	if (!Cam_IsTracking(autocam) || cls.state != ca_active)
		return;

	if (locked && (!cl.players[spec_track].name[0] || cl.players[spec_track].spectator))
	{
		locked = FALSE;
		if (cl_hightrack.value)
			Cam_CheckHighTarget();
		else
			Cam_Unlock();		
		return;
	}

	frame = &cl.frames[cls.netchan.incoming_sequence & UPDATE_MASK];
	player = frame->playerstate + spec_track;
	self = frame->playerstate + cl.playernum;

	VectorCopy(player->origin, v);
	Cam_GetTopDownOrigin(v, desired_position);

	// move there locally immediately
	VectorCopy(desired_position, self->origin);

	MSG_WriteByte(&cls.netchan.message, clc_tmove);
	MSG_WriteCoord(&cls.netchan.message, desired_position[0]);
	MSG_WriteCoord(&cls.netchan.message, desired_position[1]);
	MSG_WriteCoord(&cls.netchan.message, desired_position[2]);
}

// ZOID
//
// Take over the user controls and track a player.
// We find a nice position to watch the player and move there
void Cam_Track( usercmd_t* cmd )
{
	// TODO: Implement
}

#if 0
static float adjustang( float current, float ideal, float speed )
{
	float move;

	current = anglemod(current);
	ideal = anglemod(ideal);

	if (current == ideal)
		return current;

	move = ideal - current;
	if (ideal > current)
	{
		if (move >= 180)
			move = move - 360;
	}
	else
	{
		if (move <= -180)
			move = move + 360;
	}
	if (move > 0)
	{
		if (move > speed)
			move = speed;
	}
	else
	{
		if (move < -speed)
			move = -speed;
	}

//Con_Printf("c/i: %4.2f/%4.2f move: %4.2f\n", current, ideal, move);
	return anglemod(current + move);
}
#endif

#if 0
void Cam_SetView( void )
{
	return;
	player_state_t* player, * self;
	frame_t* frame;
	vec3_t vec, vec2;

	if (cls.state != ca_active || !cl.spectator ||
		!autocam || !locked)
		return;

	frame = &cl.frames[cls.netchan.incoming_sequence & UPDATE_MASK];
	player = frame->playerstate + spec_track;
	self = frame->playerstate + cl.playernum;

	VectorSubtract(player->origin, cl.simorg, vec);
	if (cam_forceview) {
		cam_forceview = FALSE;
		vectoangles(vec, cam_viewangles);
		cam_viewangles[0] = -cam_viewangles[0];
	}
	else {
		vectoangles(vec, vec2);
		vec2[PITCH] = -vec2[PITCH];

		cam_viewangles[PITCH] = adjustang(cam_viewangles[PITCH], vec2[PITCH], cl_camera_maxpitch.value);
		cam_viewangles[YAW] = adjustang(cam_viewangles[YAW], vec2[YAW], cl_camera_maxyaw.value);
	}
	VectorCopy(cam_viewangles, cl.viewangles);
	VectorCopy(cl.viewangles, cl.simangles);
}
#endif

qboolean Cam_IsTracking( int cam )
{
	switch (cam)
	{
	case CAM_NONE:
		return FALSE;
	case CAM_TRACK:
		return TRUE;
	case CAM_FIRSTPERSON:
		return TRUE;
	case CAM_TOPDOWN:
		return TRUE;
	}

	return FALSE;
}

char* Cam_GetModeDescription( int cam )
{
	switch (cam)
	{
	case CAM_NONE:
		return "No camera";
	case CAM_TRACK:
		return "Tracking";
	case CAM_FIRSTPERSON:
		return "1st Person";
	case CAM_TOPDOWN:
		return "Top Down";
	}

	return "Unknown";
}

void Cam_FinishMove( usercmd_t* cmd )
{
	// TODO: Implement
}

void Cam_Reset( void )
{
	autocam = CAM_NONE;
	spec_track = 0;
}

void CL_InitCam( void )
{
	Cvar_RegisterVariable(&cl_hightrack);
//	Cvar_RegisterVariable(&cl_chasecam);
//	Cvar_RegisterVariable(&cl_camera_maxpitch);
//	Cvar_RegisterVariable(&cl_camera_maxyaw);
}