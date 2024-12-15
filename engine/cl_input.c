// cl.input.c  -- builds an intended movement command to send to the server

// Quake is a trademark of Id Software, Inc., (c) 1996 Id Software, Inc. All
// rights reserved.

#include "quakedef.h"
#include "winquake.h"

/*
===============================================================================

KEY BUTTONS

Continuous button event tracking is complicated by the fact that two different
input sources (say, mouse button 1 and the control key) can both press the
same button, but the button should only be released when both of the
pressing key have been released.

When a key event issues a button command (+forward, +attack, etc), it appends
its key number as a parameter to the command so it can be matched up with
the release.

state bit 0 is the current state of the key
state bit 1 is edge triggered on the up to down transition
state bit 2 is edge triggered on the down to up transition

===============================================================================
*/

int in_cancel;

kbutton_t	in_mlook, in_klook;
kbutton_t	in_left, in_right, in_forward, in_back;
kbutton_t	in_lookup, in_lookdown, in_moveleft, in_moveright;
kbutton_t	in_strafe, in_speed, in_use, in_jump, in_duck, in_attack, in_attack2, in_reload;
kbutton_t	in_up, in_down;

int			in_impulse;

enum ECAM_Command
{
	CAM_COMMAND_NONE = 0,
	CAM_COMMAND_TOTHIRDPERSON = 1,
	CAM_COMMAND_TOFIRSTPERSON = 2
};

cvar_t		cam_command		 = { "cam_command", "0" };		// tells camera to go to thirdperson
cvar_t		cam_snapto		 = { "cam_snapto", "0" };		// snap to thirdperson view
cvar_t		cam_idealyaw	 = { "cam_idealyaw", "90" };	// thirdperson yaw
cvar_t		cam_idealpitch	 = { "cam_idealpitch", "0" };	// thirperson pitch
cvar_t		cam_idealdist	 = { "cam_idealdist", "64" };	// thirdperson distance
cvar_t		cam_contain		 = { "cam_contain", "0" };		// contain camera to world

cvar_t		c_maxpitch		 = { "c_maxpitch", "90.0" };
cvar_t		c_minpitch		 = { "c_minpitch", "0.0" };
cvar_t		c_maxyaw		 = { "c_maxyaw",   "135.0" };
cvar_t		c_minyaw		 = { "c_minyaw",   "-135.0" };
cvar_t		c_maxdistance	 = { "c_maxdistance",   "200.0" };
cvar_t		c_mindistance	 = { "c_mindistance",   "30.0" };

cvar_t		cl_anglespeedkey = { "cl_anglespeedkey", "0.67" };
cvar_t		cl_yawspeed		 = { "cl_yawspeed", "210" };
cvar_t		cl_pitchspeed	 = { "cl_pitchspeed", "225" };
cvar_t		cl_upspeed		 = { "cl_upspeed", "320" };
cvar_t		cl_forwardspeed	 = { "cl_forwardspeed", "400", TRUE };
cvar_t		cl_backspeed	 = { "cl_backspeed", "400", TRUE };
cvar_t		cl_sidespeed	 = { "cl_sidespeed", "400" };
cvar_t		cl_movespeedkey	 = { "cl_movespeedkey", "0.3" };
cvar_t		cl_pitchup		 = { "cl_pitchup", "89" };
cvar_t		cl_pitchdown	 = { "cl_pitchdown", "89" };

// pitch, yaw, dist
vec3_t cam_ofs;


// In third person
int cam_thirdperson;
int cam_mousemove; //true if we are moving the cam with the mouse, False if not
int iMouseInUse = 0;
int cam_distancemove;
extern int mouse_x, mouse_y;  //used to determine what the current x and y values are
int cam_old_mouse_x, cam_old_mouse_y; //holds the last ticks mouse movement
POINT		cam_mouse;
int cam_distancemove;

static kbutton_t cam_pitchup, cam_pitchdown, cam_yawleft, cam_yawright;
static kbutton_t cam_in, cam_out, cam_move;

#define CAM_DIST_DELTA 1.0
#define CAM_ANGLE_DELTA 2.5
#define CAM_ANGLE_SPEED 2.5
#define CAM_MIN_DIST 30.0
#define CAM_ANGLE_MOVE .5
#define MAX_ANGLE_DIFF 10.0
#define PITCH_MAX 90.0
#define PITCH_MIN 0
#define YAW_MAX  135.0
#define YAW_MIN	 -135.0


//======================Forward Declarations========================
void CAM_Init( void );
void CAM_ToThirdPerson( void );
void CAM_ToFirstPerson( void );
void CAM_StartMouseMove( void );
void CAM_EndMouseMove( void );
void CAM_StartDistance( void );
void CAM_EndDistance( void );

/*
============
KeyDown
============
*/
void KeyDown( kbutton_t *b )
{
	int		k;
	char* c;

	c = Cmd_Argv(1);
	if (c[0])
		k = atoi(c);
	else
		k = -1;		// typed manually at the console for continuous down

	if (k == b->down[0] || k == b->down[1])
		return;		// repeating key

	if (!b->down[0])
		b->down[0] = k;
	else if (!b->down[1])
		b->down[1] = k;
	else
	{
		Con_DPrintf("Three keys down for a button '%c' '%c' '%c'!\n", b->down[0], b->down[1], c);
		return;
	}

	if (b->state & 1)
		return;		// still down
	b->state |= 1 + 2;	// down + impulse down
}

/*
============
KeyUp
============
*/
void KeyUp( kbutton_t *b )
{
	int		k;
	char* c;

	c = Cmd_Argv(1);
	if (c[0])
		k = atoi(c);
	else
	{ // typed manually at the console, assume for unsticking, so clear all
		b->down[0] = b->down[1] = 0;
		b->state = 4;	// impulse up
		return;
	}

	if (b->down[0] == k)
		b->down[0] = 0;
	else if (b->down[1] == k)
		b->down[1] = 0;
	else
		return;		// key up without corresponding down (menu pass through)
	if (b->down[0] || b->down[1])
	{
		//Con_Printf("Keys down for button: '%c' '%c' '%c' (%d,%d,%d)!\n", b->down[0], b->down[1], c, b->down[0], b->down[1], c);
		return;		// some other key is still holding it down
	}

	if (!(b->state & 1))
		return;		// still up (this should not happen)

	b->state &= ~1;		// now up
	b->state |= 4; 		// impulse up
}


void IN_KLookDown( void ) { KeyDown(&in_klook); }
void IN_KLookUp( void ) { KeyUp(&in_klook); }
void IN_MLookDown( void ) { KeyDown(&in_mlook); }
void IN_MLookUp( void ) {
	KeyUp(&in_mlook);
	if (!(in_mlook.state & 1) && lookspring.value)
		V_StartPitchDrift();
}
void IN_UpDown( void ) { KeyDown(&in_up); }
void IN_UpUp( void ) { KeyUp(&in_up); }
void IN_DownDown( void ) { KeyDown(&in_down); }
void IN_DownUp( void ) { KeyUp(&in_down); }
void IN_LeftDown( void ) { KeyDown(&in_left); }
void IN_LeftUp( void ) { KeyUp(&in_left); }
void IN_RightDown( void ) { KeyDown(&in_right); }
void IN_RightUp( void ) { KeyUp(&in_right); }
void IN_ForwardDown( void ) { KeyDown(&in_forward); }
void IN_ForwardUp( void ) { KeyUp(&in_forward); }
void IN_BackDown( void ) { KeyDown(&in_back); }
void IN_BackUp( void ) { KeyUp(&in_back); }
void IN_LookupDown( void ) { KeyDown(&in_lookup); }
void IN_LookupUp( void ) { KeyUp(&in_lookup); }
void IN_LookdownDown( void ) { KeyDown(&in_lookdown); }
void IN_LookdownUp( void ) { KeyUp(&in_lookdown); }
void IN_MoveleftDown( void ) { KeyDown(&in_moveleft); }
void IN_MoveleftUp( void ) { KeyUp(&in_moveleft); }
void IN_MoverightDown( void ) { KeyDown(&in_moveright); }
void IN_MoverightUp( void ) { KeyUp(&in_moveright); }

void IN_SpeedDown( void ) { KeyDown(&in_speed); }
void IN_SpeedUp( void ) { KeyUp(&in_speed); }
void IN_StrafeDown( void ) { KeyDown(&in_strafe); }
void IN_StrafeUp( void ) { KeyUp(&in_strafe); }

void IN_AttackDown( void ) { KeyDown(&in_attack); }
void IN_AttackUp( void ) 
{
	KeyUp(&in_attack); 
	in_cancel = 0;
}

void IN_Attack2Down( void ) { KeyDown(&in_attack2); }
void IN_Attack2Up( void ) { KeyUp(&in_attack2); }

void IN_UseDown( void ) { KeyDown(&in_use); }
void IN_UseUp( void ) { KeyUp(&in_use); }
void IN_JumpDown( void ) { KeyDown(&in_jump); }
void IN_JumpUp( void ) { KeyUp(&in_jump); }

void IN_DuckDown( void ) { KeyDown(&in_duck); }
void IN_DuckUp( void ) { KeyUp(&in_duck); }

// Special handling
void IN_Cancel( void )
{
	in_cancel = 1;
}

void IN_Impulse( void ) { in_impulse = Q_atoi(Cmd_Argv(1)); }

void IN_ReloadDown( void ) { KeyDown(&in_reload); }
void IN_ReloadUp( void ) { KeyUp(&in_reload); }

/*
===============
CL_KeyState

Returns 0.25 if a key was pressed and released during the frame,
0.5 if it was pressed and held
0 if held then released, and
1.0 if held for the entire time
===============
*/
float CL_KeyState( kbutton_t* key )
{
	float		val = 0.0;
	int			impulsedown, impulseup, down;

	impulsedown = key->state & 2;
	impulseup = key->state & 4;
	down = key->state & 1;

	if (impulsedown && !impulseup)
	{
		// pressed and held this frame?
		val = down ? 0.5 : 0.0;
	}

	if (impulseup && !impulsedown)
	{
		// released this frame?
		val = down ? 0.0 : 0.0;
	}

	if (!impulsedown && !impulseup)
	{
		// held the entire frame?
		val = down ? 1.0 : 0.0;
	}

	if (impulsedown && impulseup)
	{
		if (down)
		{
			// released and re-pressed this frame
			val = 0.75;
		}
		else
		{
			// pressed and released this frame
			val = 0.25;
		}
	}

	// clear impulses
	key->state &= 1;
	return val;
}

/*
================
CL_AdjustAngles

Moves the local angle positions
================
*/
void CL_AdjustAngles( void )
{
	float	speed;
	float	up, down;

	if (in_speed.state & 1)
	{
		speed = host_frametime * cl_anglespeedkey.value;
	}
	else
	{
		speed = host_frametime;
	}

	if (!(in_strafe.state & 1))
	{
		cl.viewangles[YAW] += speed * cl_yawspeed.value * CL_KeyState(&in_left);
		cl.viewangles[YAW] -= speed * cl_yawspeed.value * CL_KeyState(&in_right);
		cl.viewangles[YAW] = anglemod(cl.viewangles[YAW]);
	}
	if (in_klook.state & 1)
	{
		V_StopPitchDrift();
		cl.viewangles[PITCH] -= speed * cl_pitchspeed.value * CL_KeyState(&in_forward);
		cl.viewangles[PITCH] += speed * cl_pitchspeed.value * CL_KeyState(&in_back);
	}

	up = CL_KeyState(&in_lookup);
	down = CL_KeyState(&in_lookdown);

	cl.viewangles[PITCH] -= speed * cl_pitchspeed.value * up;
	cl.viewangles[PITCH] += speed * cl_pitchspeed.value * down;

	if (up || down)
		V_StopPitchDrift();

	if (cl.viewangles[PITCH] > cl_pitchdown.value)
		cl.viewangles[PITCH] = cl_pitchdown.value;
	if (cl.viewangles[PITCH] < -cl_pitchup.value)
		cl.viewangles[PITCH] = -cl_pitchup.value;

	if (cl.viewangles[ROLL] > 50)
		cl.viewangles[ROLL] = 50;
	if (cl.viewangles[ROLL] < -50)
		cl.viewangles[ROLL] = -50;
}

/*
================
CL_BaseMove

Send the intended movement message to the server
================
*/
void CL_BaseMove( usercmd_t *cmd )
{
	CL_AdjustAngles();

	memset( cmd, 0, sizeof( *cmd ) );

	VectorCopy( cl.viewangles, cmd->angles );
	if (in_strafe.state & 1)
	{
		cmd->sidemove += cl_sidespeed.value * CL_KeyState( &in_right );
		cmd->sidemove -= cl_sidespeed.value * CL_KeyState( &in_left );
	}

	cmd->sidemove += cl_sidespeed.value * CL_KeyState( &in_moveright );
	cmd->sidemove -= cl_sidespeed.value * CL_KeyState( &in_moveleft );

	cmd->upmove += cl_upspeed.value * CL_KeyState( &in_up );
	cmd->upmove -= cl_upspeed.value * CL_KeyState( &in_down );

	if (!(in_klook.state & 1))
	{
		cmd->forwardmove += cl_forwardspeed.value * CL_KeyState( &in_forward );
		cmd->forwardmove -= cl_backspeed.value * CL_KeyState( &in_back );
	}

//
// adjust for speed key
//
	if (in_speed.state & 1)
	{
		cmd->forwardmove *= cl_movespeedkey.value;
		cmd->sidemove *= cl_movespeedkey.value;
		cmd->upmove *= cl_movespeedkey.value;
	}
}

/*
============
CL_ButtonBits

Returns appropriate button info for keyboard and mouse state
Set bResetState to 1 to clear old state info
============
*/
int CL_ButtonBits( int bResetState )
{
	int bits = 0;

	if (in_attack.state & 3)
	{
		bits |= IN_ATTACK;
	}

	if (in_duck.state & 3)
	{
		bits |= IN_DUCK;
	}

	if (in_jump.state & 3)
	{
		bits |= IN_JUMP;
	}

	if (in_forward.state & 3)
	{
		bits |= IN_FORWARD;
	}

	if (in_back.state & 3)
	{
		bits |= IN_BACK;
	}

	if (in_use.state & 3)
	{
		bits |= IN_USE;
	}

	if (in_cancel)
	{
		bits |= IN_CANCEL;
	}

	if (in_left.state & 3)
	{
		bits |= IN_LEFT;
	}

	if (in_right.state & 3)
	{
		bits |= IN_RIGHT;
	}

	if (in_moveleft.state & 3)
	{
		bits |= IN_MOVELEFT;
	}

	if (in_moveright.state & 3)
	{
		bits |= IN_MOVERIGHT;
	}

	if (in_attack2.state & 3)
	{
		bits |= IN_ATTACK2;
	}

	if (in_reload.state & 3)
	{
		bits |= IN_RELOAD;
	}

	if (bResetState)
	{
		in_attack.state &= ~2;
		in_duck.state &= ~2;
		in_jump.state &= ~2;
		in_forward.state &= ~2;
		in_back.state &= ~2;
		in_use.state &= ~2;
		in_left.state &= ~2;
		in_right.state &= ~2;
		in_moveleft.state &= ~2;
		in_moveright.state &= ~2;
		in_attack2.state &= ~2;
		in_reload.state &= ~2;
	}

	return bits;
}

/*
============
CL_ResetButtonBits

============
*/
void CL_ResetButtonBits( int bits )
{
	int bitsNew = CL_ButtonBits(0) ^ bits;

	// Has the attack button been changed
	if (bitsNew & IN_ATTACK)
	{
		// Was it pressed? or let go?
		if (bits & IN_ATTACK)
		{
			KeyDown(&in_attack);
		}
		else
		{
			// totally clear state
			in_attack.state &= ~7;
		}
	}
}

/*
============
CL_InitInput
============
*/
void CL_InitInput( void )
{
	Cmd_AddCommand("+moveup", IN_UpDown);
	Cmd_AddCommand("-moveup", IN_UpUp);
	Cmd_AddCommand("+movedown", IN_DownDown);
	Cmd_AddCommand("-movedown", IN_DownUp);
	Cmd_AddCommand("+left", IN_LeftDown);
	Cmd_AddCommand("-left", IN_LeftUp);
	Cmd_AddCommand("+right", IN_RightDown);
	Cmd_AddCommand("-right", IN_RightUp);
	Cmd_AddCommand("+forward", IN_ForwardDown);
	Cmd_AddCommand("-forward", IN_ForwardUp);
	Cmd_AddCommand("+back", IN_BackDown);
	Cmd_AddCommand("-back", IN_BackUp);
	Cmd_AddCommand("+lookup", IN_LookupDown);
	Cmd_AddCommand("-lookup", IN_LookupUp);
	Cmd_AddCommand("+lookdown", IN_LookdownDown);
	Cmd_AddCommand("-lookdown", IN_LookdownUp);
	Cmd_AddCommand("+strafe", IN_StrafeDown);
	Cmd_AddCommand("-strafe", IN_StrafeUp);
	Cmd_AddCommand("+moveleft", IN_MoveleftDown);
	Cmd_AddCommand("-moveleft", IN_MoveleftUp);
	Cmd_AddCommand("+moveright", IN_MoverightDown);
	Cmd_AddCommand("-moveright", IN_MoverightUp);
	Cmd_AddCommand("+speed", IN_SpeedDown);
	Cmd_AddCommand("-speed", IN_SpeedUp);
	Cmd_AddCommand("+attack", IN_AttackDown);
	Cmd_AddCommand("-attack", IN_AttackUp);
	Cmd_AddCommand("+attack2", IN_Attack2Down);
	Cmd_AddCommand("-attack2", IN_Attack2Up);
	Cmd_AddCommand("+use", IN_UseDown);
	Cmd_AddCommand("-use", IN_UseUp);
	Cmd_AddCommand("+jump", IN_JumpDown);
	Cmd_AddCommand("-jump", IN_JumpUp);
	Cmd_AddCommand("impulse", IN_Impulse);
	Cmd_AddCommand("+klook", IN_KLookDown);
	Cmd_AddCommand("-klook", IN_KLookUp);
	Cmd_AddCommand("+mlook", IN_MLookDown);
	Cmd_AddCommand("-mlook", IN_MLookUp);
	Cmd_AddCommand("+duck", IN_DuckDown);
	Cmd_AddCommand("-duck", IN_DuckUp);
	Cmd_AddCommand("+reload", IN_ReloadDown);
	Cmd_AddCommand("-reload", IN_ReloadUp);
	CAM_Init();
}

float MoveToward( float cur, float goal, float maxspeed )
{
	if (cur != goal)
	{
		if (abs(cur - goal) > 180.0)
		{
			if (cur < goal)
				cur += 360.0;
			else
				cur -= 360.0;
		}

		if (cur < goal)
		{
			if (cur < goal - 1.0)
				cur += (goal - cur) / 4.0;
			else
				cur = goal;
		}
		else
		{
			if (cur > goal + 1.0)
				cur -= (cur - goal) / 4.0;
			else
				cur = goal;
		}
	}


	// bring cur back into range
	if (cur < 0)
		cur += 360.0;
	else if (cur >= 360)
		cur -= 360;

	return cur;
}

extern trace_t SV_ClipMoveToEntity( edict_t *ent, vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end );

void CAM_Think( void )
{
	vec3_t origin;
	vec3_t ext, pnt, camForward, camRight, camUp;
	float dist;
	vec3_t camAngles;
	int i;
	trace_t trace;

	switch ((int)cam_command.value)
	{
	case CAM_COMMAND_TOTHIRDPERSON:
		CAM_ToThirdPerson();
		break;

	case CAM_COMMAND_TOFIRSTPERSON:
		CAM_ToFirstPerson();
		break;

	case CAM_COMMAND_NONE:
	default:
		break;
	}

	if (!cam_thirdperson)
		return;

	if (cam_contain.value)
	{
		ext[0] = ext[1] = ext[2] = 0.0;
	}

	camAngles[ PITCH ] = cam_idealpitch.value;
	camAngles[ YAW ] = cam_idealyaw.value;
	dist = cam_idealdist.value;
	//
	//movement of the camera with the mouse
	//
	if (cam_mousemove)
	{
		//get windows cursor position
		GetCursorPos(&cam_mouse);
		//check for X delta values and adjust accordingly
		//eventually adjust YAW based on amount of movement
	  //don't do any movement of the cam using YAW/PITCH if we are zooming in/out the camera	
		if (!cam_distancemove)
		{

			//keep the camera within certain limits around the player (ie avoid certain bad viewing angles)  
			if (cam_mouse.x > window_center_x)
			{
				//if ((camAngles[ YAW ]>=225.0)||(camAngles[ YAW ]<135.0))
				if (camAngles[ YAW ] < c_maxyaw.value)
				{
					camAngles[ YAW ] += (CAM_ANGLE_MOVE)*((cam_mouse.x - window_center_x) / 2);
				}
				if (camAngles[ YAW ] > c_maxyaw.value)
				{

					camAngles[ YAW ] = c_maxyaw.value;
				}
			}
			else if (cam_mouse.x < window_center_x)
			{
				//if ((camAngles[ YAW ]<=135.0)||(camAngles[ YAW ]>225.0))
				if (camAngles[ YAW ] > c_minyaw.value)
				{
					camAngles[ YAW ] -= (CAM_ANGLE_MOVE)* ((window_center_x - cam_mouse.x) / 2);

				}
				if (camAngles[ YAW ] < c_minyaw.value)
				{
					camAngles[ YAW ] = c_minyaw.value;

				}
			}

			//check for y delta values and adjust accordingly
			//eventually adjust PITCH based on amount of movement
			//also make sure camera is within bounds
			if (cam_mouse.y > window_center_y)
			{
				if (camAngles[ PITCH ] < c_maxpitch.value)
				{
					camAngles[ PITCH ] += (CAM_ANGLE_MOVE)* ((cam_mouse.y - window_center_y) / 2);
				}
				if (camAngles[ PITCH ] > c_maxpitch.value)
				{
					camAngles[ PITCH ] = c_maxpitch.value;
				}
			}
			else if (cam_mouse.y < window_center_y)
			{
				if (camAngles[ PITCH ] > c_minpitch.value)
				{
					camAngles[ PITCH ] -= (CAM_ANGLE_MOVE)*((window_center_y - cam_mouse.y) / 2);
				}
				if (camAngles[ PITCH ] < c_minpitch.value)
				{								 
					camAngles[ PITCH ] = c_minpitch.value;
				}
			}

			//set old mouse coordinates to current mouse coordinates
			//since we are done with the mouse

			cam_old_mouse_y = cam_mouse.y*sensitivity.value;

			SetCursorPos(window_center_x, window_center_y);
		}
	}

	//Nathan code here
	if (CL_KeyState(&cam_pitchup))
		camAngles[ PITCH ] += CAM_ANGLE_DELTA;
	else if (CL_KeyState(&cam_pitchdown))
		camAngles[ PITCH ] -= CAM_ANGLE_DELTA;

	if (CL_KeyState(&cam_yawleft))
		camAngles[ YAW ] -= CAM_ANGLE_DELTA;
	else if (CL_KeyState(&cam_yawright))
		camAngles[ YAW ] += CAM_ANGLE_DELTA;

	if (CL_KeyState(&cam_in))
	{
		dist -= CAM_DIST_DELTA;
		if (dist < CAM_MIN_DIST)
		{
			// If we go back into first person, reset the angle
			camAngles[ PITCH ] = 0;
			camAngles[ YAW ] = 0;
			dist = CAM_MIN_DIST;
		}

	}
	else if (CL_KeyState(&cam_out))
		dist += CAM_DIST_DELTA;

	if (cam_distancemove)
	{
		if (cam_mouse.y > window_center_y)
		{
			if (dist < c_maxdistance.value)
			{
				dist += CAM_DIST_DELTA * ((cam_mouse.y - window_center_y) / 2);
			}
			if (dist > c_maxdistance.value)
			{
				dist = c_maxdistance.value;
			}
		}
		else if (cam_mouse.y < window_center_y)
		{
			if (dist > c_mindistance.value)
			{
				dist -= (CAM_DIST_DELTA)*((window_center_y - cam_mouse.y) / 2);
			}
			if (dist < c_mindistance.value)
			{
				dist = c_mindistance.value;
			}
		}
		//set old mouse coordinates to current mouse coordinates
		//since we are done with the mouse
		cam_old_mouse_x = cam_mouse.x*sensitivity.value;
		cam_old_mouse_y = cam_mouse.y*sensitivity.value;
		SetCursorPos(window_center_x, window_center_y);
	}
	if (cam_contain.value)
	{
		// check new ideal
		VectorCopy(origin, pnt);
		AngleVectors(camAngles, camForward, camRight, camUp);
		for (i = 0; i < 3; i++)
			pnt[i] += -dist * camForward[i];

		// check line from r_refdef.vieworg to pnt
		trace = SV_ClipMoveToEntity(sv.edicts, r_refdef.vieworg, ext, ext, pnt);
		if (trace.fraction == 1.0)
		{
			// update ideal
			cam_idealpitch.value = camAngles[ PITCH ];
			cam_idealyaw.value = camAngles[ YAW ];
			cam_idealdist.value = dist;
		}
	}
	else
	{
		// update ideal
		cam_idealpitch.value = camAngles[ PITCH ];
		cam_idealyaw.value = camAngles[ YAW ];
		cam_idealdist.value = dist;
	}

	// Move towards ideal
	VectorCopy(cam_ofs, camAngles);

	if (cam_snapto.value)
	{
		camAngles[ YAW ] = cam_idealyaw.value + cl.viewangles[ YAW ];
		camAngles[ PITCH ] = cam_idealpitch.value + cl.viewangles[ PITCH ];
		camAngles[2] = cam_idealdist.value;
	}
	else
	{
		if (camAngles[ YAW ] - cl.viewangles[ YAW ] != cam_idealyaw.value)
			camAngles[ YAW ] = MoveToward(camAngles[ YAW ], cam_idealyaw.value + cl.viewangles[ YAW ], CAM_ANGLE_SPEED);

		if (camAngles[ PITCH ] - cl.viewangles[ PITCH ] != cam_idealpitch.value)
			camAngles[ PITCH ] = MoveToward(camAngles[ PITCH ], cam_idealpitch.value + cl.viewangles[ PITCH ], CAM_ANGLE_SPEED);

		if (abs(camAngles[2] - cam_idealdist.value) < 2.0)
			camAngles[2] = cam_idealdist.value;
		else
			camAngles[2] += (cam_idealdist.value - camAngles[2]) / 4.0;
	}
	if (cam_contain.value)
	{
		// Test new position
		dist = camAngles[ ROLL ];
		camAngles[ ROLL ] = 0;

		VectorCopy(origin, pnt);
		AngleVectors(camAngles, camForward, camRight, camUp);
		for (i = 0; i < 3; i++)
			pnt[i] += -dist * camForward[i];

		// check line from r_refdef.vieworg to pnt
		ext[0] = ext[1] = ext[2] = 0.0;
		trace = SV_ClipMoveToEntity(sv.edicts, r_refdef.vieworg, ext, ext, pnt);
		if (trace.fraction != 1.0)
			return;
	}
	cam_ofs[0] = camAngles[0];
	cam_ofs[1] = camAngles[1];
	cam_ofs[2] = dist;
}

void CAM_PitchUpDown( void ) { KeyDown(&cam_pitchup); }
void CAM_PitchUpUp( void ) { KeyUp(&cam_pitchup); }
void CAM_PitchDownDown( void ) { KeyDown(&cam_pitchdown); }
void CAM_PitchDownUp( void ) { KeyUp(&cam_pitchdown); }
void CAM_YawLeftDown( void ) { KeyDown(&cam_yawleft); }
void CAM_YawLeftUp( void ) { KeyUp(&cam_yawleft); }
void CAM_YawRightDown( void ) { KeyDown(&cam_yawright); }
void CAM_YawRightUp( void ) { KeyUp(&cam_yawright); }
void CAM_InDown( void ) { KeyDown(&cam_in); }
void CAM_InUp( void ) { KeyUp(&cam_in); }
void CAM_OutDown( void ) { KeyDown(&cam_out); }
void CAM_OutUp( void ) { KeyUp(&cam_out); }

void CAM_ToThirdPerson( void )
{
	//we are not in 3rd person view mode, so let's enter it.
	if (!cam_thirdperson)
	{
		//we are going into 3rd person view.
		cam_thirdperson = 1;

		cam_ofs[ YAW ] = cl.viewangles[ YAW ]; 
		cam_ofs[ PITCH ] = cl.viewangles[ PITCH ]; 
		cam_ofs[2] = CAM_MIN_DIST;
	}

	Cvar_SetValue("cam_command", 0);
}

void CAM_ToFirstPerson( void )
{
	//we are going out of 3rd person view.
	cam_thirdperson = 0;

	Cvar_SetValue("cam_command", 0);
}

void CAM_ToggleSnapto( void )
{
	cam_snapto.value = !cam_snapto.value;
}

void CAM_Init( void )
{
	Cmd_AddCommand("+campitchup", CAM_PitchUpDown);
	Cmd_AddCommand("-campitchup", CAM_PitchUpUp);
	Cmd_AddCommand("+campitchdown", CAM_PitchDownDown);
	Cmd_AddCommand("-campitchdown", CAM_PitchDownUp);
	Cmd_AddCommand("+camyawleft", CAM_YawLeftDown);
	Cmd_AddCommand("-camyawleft", CAM_YawLeftUp);
	Cmd_AddCommand("+camyawright", CAM_YawRightDown);
	Cmd_AddCommand("-camyawright", CAM_YawRightUp);
	Cmd_AddCommand("+camin", CAM_InDown);
	Cmd_AddCommand("-camin", CAM_InUp);
	Cmd_AddCommand("+camout", CAM_OutDown);
	Cmd_AddCommand("-camout", CAM_OutUp);
	Cmd_AddCommand("thirdperson", CAM_ToThirdPerson);
	Cmd_AddCommand("firstperson", CAM_ToFirstPerson);
	Cmd_AddCommand("+cammousemove", CAM_StartMouseMove);
	Cmd_AddCommand("-cammousemove", CAM_EndMouseMove);
	Cmd_AddCommand("+camdistance", CAM_StartDistance);
	Cmd_AddCommand("-camdistance", CAM_EndDistance);
	Cmd_AddCommand("snapto", CAM_ToggleSnapto);
	Cvar_RegisterVariable(&cam_command);
	Cvar_RegisterVariable(&cam_snapto);
	Cvar_RegisterVariable(&cam_idealyaw);
	Cvar_RegisterVariable(&cam_idealpitch);
	Cvar_RegisterVariable(&cam_idealdist);
	Cvar_RegisterVariable(&cam_contain);
	Cvar_RegisterVariable(&c_maxpitch);
	Cvar_RegisterVariable(&c_minpitch);
	Cvar_RegisterVariable(&c_maxyaw);
	Cvar_RegisterVariable(&c_minyaw);
	Cvar_RegisterVariable(&c_maxdistance);
	Cvar_RegisterVariable(&c_mindistance);
}

void CAM_ClearStates( void )
{
	cam_pitchup.state = 0;
	cam_pitchdown.state = 0;
	cam_yawleft.state = 0;
	cam_yawright.state = 0;
	cam_in.state = 0;
	cam_out.state = 0;

	cam_thirdperson = 0;
	cam_command.value = 0;
	cam_mousemove = 0;

	cam_snapto.value = 0;
	cam_distancemove = 0;

	cam_ofs[ PITCH ] = 0.0;
	cam_ofs[ YAW ] = 0.0;
	cam_ofs[ ROLL ] = CAM_MIN_DIST;

	cam_idealpitch.value = cl.viewangles[ PITCH ];
	cam_idealyaw.value = cl.viewangles[ YAW ];
	cam_idealdist.value = CAM_MIN_DIST;
}

void CAM_StartMouseMove( void )
{
	//only move the cam with mouse if we are in third person.
	if (cam_thirdperson)
	{
		//set appropriate flags and initialize the old mouse position
		//variables for mouse camera movement
		if (!cam_mousemove)
		{
			cam_mousemove = 1;
			iMouseInUse = 1;
			GetCursorPos(&cam_mouse);

			cam_old_mouse_x = cam_mouse.x*sensitivity.value;
			cam_old_mouse_y = cam_mouse.y*sensitivity.value;
		}
	}
	//we are not in 3rd person view..therefore do not allow camera movement
	else
	{
		cam_mousemove = 0;
		iMouseInUse = 0;
	}
}

//the key has been released for camera movement
//tell the engine that mouse camera movement is off
void CAM_EndMouseMove( void )
{
	cam_mousemove = 0;
	iMouseInUse = 0;
}

//----------------------------------------------------------
//routines to start the process of moving the cam in or out 
//using the mouse
//----------------------------------------------------------
void CAM_StartDistance( void )
{
	//only move the cam with mouse if we are in third person.
	if (cam_thirdperson)
	{
		//set appropriate flags and initialize the old mouse position
		//variables for mouse camera movement
		if (!cam_distancemove)
		{
			cam_distancemove = 1;
			cam_mousemove = 1;
			iMouseInUse = 1;
			GetCursorPos(&cam_mouse);
			cam_old_mouse_x = cam_mouse.x*sensitivity.value;
			cam_old_mouse_y = cam_mouse.y*sensitivity.value;
		}
	}
	//we are not in 3rd person view..therefore do not allow camera movement
	else
	{
		cam_distancemove = 0;
		cam_mousemove = 0;
		iMouseInUse = 0;
	}
}

//the key has been released for camera movement
//tell the engine that mouse camera movement is off
void CAM_EndDistance( void )
{
	cam_distancemove = 0;
	cam_mousemove = 0;
	iMouseInUse = 0;
}