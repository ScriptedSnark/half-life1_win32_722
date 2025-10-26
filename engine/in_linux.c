// in_win.c -- windows 95 mouse and joystick code
// 02/21/97 JCB Added extended DirectInput code to support external controllers.

#include "quakedef.h"
#include "winquake.h"

extern int iMouseInUse;

// mouse variables
cvar_t	m_filter = { "m_filter", "0" };

int			mouse_buttons;
int			mouse_oldbuttonstate;
POINT		current_pos;
int			mouse_x, mouse_y, old_mouse_x, old_mouse_y, mx_accum, my_accum;

static qboolean	restore_spi;
static int		originalmouseparms[3], newmouseparms[3] = { 0, 0, 1 };
qboolean	mouseactive;
qboolean		mouseinitialized;
static qboolean	mouseparmsvalid, mouseactivatetoggle;
static int	mouseshowtoggle = 1;

// joystick defines and variables
// where should defines be moved?
#define JOY_ABSOLUTE_AXIS	0x00000000		// control like a joystick
#define JOY_RELATIVE_AXIS	0x00000010		// control like a mouse, spinner, trackball
#define	JOY_MAX_AXES		6				// X, Y, Z, R, U, V
#define JOY_AXIS_X			0
#define JOY_AXIS_Y			1
#define JOY_AXIS_Z			2
#define JOY_AXIS_R			3
#define JOY_AXIS_U			4
#define JOY_AXIS_V			5

enum _ControlList
{
	AxisNada = 0,
	AxisForward,
	AxisLook,
	AxisSide,
	AxisTurn
};

DWORD	dwAxisMap[JOY_MAX_AXES];
DWORD	dwControlMap[JOY_MAX_AXES];
int	pdwRawValue[JOY_MAX_AXES];

// none of these cvars are saved over a session
// this means that advanced controller configuration needs to be executed
// each time.  this avoids any problems with getting back to a default usage
// or when changing from one controller to another.  this way at least something
// works.
cvar_t	in_joystick = { "joystick", "0" };
cvar_t	joy_name = { "joyname", "joystick" };
cvar_t	joy_advanced = { "joyadvanced", "0" };
cvar_t	joy_advaxisx = { "joyadvaxisx", "0" };
cvar_t	joy_advaxisy = { "joyadvaxisy", "0" };
cvar_t	joy_advaxisz = { "joyadvaxisz", "0" };
cvar_t	joy_advaxisr = { "joyadvaxisr", "0" };
cvar_t	joy_advaxisu = { "joyadvaxisu", "0" };
cvar_t	joy_advaxisv = { "joyadvaxisv", "0" };
cvar_t	joy_forwardthreshold = { "joyforwardthreshold", "0.15" };
cvar_t	joy_sidethreshold = { "joysidethreshold", "0.15" };
cvar_t	joy_pitchthreshold = { "joypitchthreshold", "0.15" };
cvar_t	joy_yawthreshold = { "joyyawthreshold", "0.15" };
cvar_t	joy_forwardsensitivity = { "joyforwardsensitivity", "-1.0" };
cvar_t	joy_sidesensitivity = { "joysidesensitivity", "-1.0" };
cvar_t	joy_pitchsensitivity = { "joypitchsensitivity", "1.0" };
cvar_t	joy_yawsensitivity = { "joyyawsensitivity", "-1.0" };
cvar_t	joy_wwhack1 = { "joywwhack1", "0.0" };
cvar_t	joy_wwhack2 = { "joywwhack2", "0.0" };

int			joy_avail, joy_advancedinit, joy_haspov;
DWORD		joy_oldbuttonstate, joy_oldpovstate;

int			joy_id;
DWORD		joy_flags;
DWORD		joy_numbuttons;

SDL_GameController *s_pJoystick = NULL;

// forward-referenced functions
void IN_StartupJoystick( void );
void Joy_AdvancedUpdate_f( void );
void IN_JoyMove( usercmd_t* cmd );


/*
===========
Force_CenterView_f
===========
*/
void Force_CenterView_f( void )
{
	if (!iMouseInUse)
	{
		cl.viewangles[PITCH] = 0;
	}
}


/*
===========
IN_UpdateClipCursor
===========
*/
void IN_UpdateClipCursor( void )
{
}


/*
===========
IN_ShowMouse
===========
*/
void IN_ShowMouse( void )
{
}


/*
===========
IN_HideMouse
===========
*/
void IN_HideMouse( void )
{
}


/*
===========
IN_ActivateMouse
===========
*/
void IN_ActivateMouse( void )
{
	if (mouseinitialized)
	{
#ifdef _WIN32
		if (mouseparmsvalid)
			restore_spi = SystemParametersInfo (SPI_SETMOUSE, 0, newmouseparms, 0);

#endif

		mouseactive = TRUE;
	}
}


/*
===========
IN_SetQuakeMouseState
===========
*/
void IN_SetQuakeMouseState( void )
{
}


/*
===========
IN_DeactivateMouse
===========
*/
void IN_DeactivateMouse( void )
{
	mouseactivatetoggle = FALSE;

	if (mouseinitialized)
	{
#ifdef _WIN32
		if (restore_spi)
			SystemParametersInfo (SPI_SETMOUSE, 0, originalmouseparms, 0);

#endif

		mouseactive = FALSE;
	}
}


/*
===========
IN_StartupMouse
===========
*/
void IN_StartupMouse( void )
{
	if (COM_CheckParm("-nomouse"))
		return;
	
	mouseinitialized = TRUE;
#ifdef _WIN32
	mouseparmsvalid = SystemParametersInfo(SPI_GETMOUSE, 0, originalmouseparms, 0);

	if (mouseparmsvalid)
	{
		if (COM_CheckParm("-noforcemspd"))
			newmouseparms[2] = originalmouseparms[2];

		if (COM_CheckParm("-noforcemaccel"))
		{
			newmouseparms[0] = originalmouseparms[0];
			newmouseparms[1] = originalmouseparms[1];
		}

		if (COM_CheckParm("-noforcemparms"))
		{
			newmouseparms[0] = originalmouseparms[0];
			newmouseparms[1] = originalmouseparms[1];
			newmouseparms[2] = originalmouseparms[2];
		}
	}
#endif
	mouse_buttons = 3;
}


/*
===========
IN_Init
===========
*/
void IN_Init( void )
{
	// mouse variables
	Cvar_RegisterVariable(&m_filter);

	// joystick variables
	Cvar_RegisterVariable(&in_joystick);
	Cvar_RegisterVariable(&joy_name);
	Cvar_RegisterVariable(&joy_advanced);
	Cvar_RegisterVariable(&joy_advaxisx);
	Cvar_RegisterVariable(&joy_advaxisy);
	Cvar_RegisterVariable(&joy_advaxisz);
	Cvar_RegisterVariable(&joy_advaxisr);
	Cvar_RegisterVariable(&joy_advaxisu);
	Cvar_RegisterVariable(&joy_advaxisv);
	Cvar_RegisterVariable(&joy_forwardthreshold);
	Cvar_RegisterVariable(&joy_sidethreshold);
	Cvar_RegisterVariable(&joy_pitchthreshold);
	Cvar_RegisterVariable(&joy_yawthreshold);
	Cvar_RegisterVariable(&joy_forwardsensitivity);
	Cvar_RegisterVariable(&joy_sidesensitivity);
	Cvar_RegisterVariable(&joy_pitchsensitivity);
	Cvar_RegisterVariable(&joy_yawsensitivity);
	Cvar_RegisterVariable(&joy_wwhack1);
	Cvar_RegisterVariable(&joy_wwhack2);

	Cmd_AddCommand("force_centerview", Force_CenterView_f);
	Cmd_AddCommand("joyadvancedupdate", Joy_AdvancedUpdate_f);

	IN_StartupMouse();
	IN_StartupJoystick();
}

/*
===========
IN_Shutdown
===========
*/
void IN_Shutdown( void )
{
	IN_DeactivateMouse();
	IN_ShowMouse();
}


/*
===========
IN_MouseEvent
===========
*/
void IN_MouseEvent( int mstate )
{
	int	i;

	if (mouseactive)
	{
	// perform button actions
		for (i = 0; i < mouse_buttons; i++)
		{
			if ((mstate & (1 << i)) &&
				!(mouse_oldbuttonstate & (1 << i)))
			{
				Key_Event(K_MOUSE1 + i, TRUE);
			}

			if (!(mstate & (1 << i)) &&
				(mouse_oldbuttonstate & (1 << i)))
			{
				Key_Event(K_MOUSE1 + i, FALSE);
			}
		}

		mouse_oldbuttonstate = mstate;
	}
}


/*
===========
IN_MouseMove
===========
*/
void IN_MouseMove( usercmd_t* cmd )
{
	int					mx, my;

	if (iMouseInUse)
		return;

#ifdef _WIN32
	GetCursorPos(&current_pos);
#else
    SDL_GetGlobalMouseState(&current_pos.x, &current_pos.y);
#endif

	mx = current_pos.x + mx_accum - window_center_x;
	my = current_pos.y + my_accum - window_center_y;
	mx_accum = 0;
	my_accum = 0;

//	if (mx || my)
//		Con_DPrintf("mx=%d, my=%d\n", mx, my);

	if (m_filter.value)
	{
		mouse_x = (mx + old_mouse_x) * 0.5;
		mouse_y = (my + old_mouse_y) * 0.5;
	}
	else
	{
		mouse_x = mx;
		mouse_y = my;
	}

	old_mouse_x = mx;
	old_mouse_y = my;

	mouse_x *= sensitivity.value;
	mouse_y *= sensitivity.value;

// add mouse X/Y movement to cmd
	if ((in_strafe.state & 1) || (lookstrafe.value && (in_mlook.state & 1)))
		cmd->sidemove += m_side.value * mouse_x;
	else
		cl.viewangles[YAW] -= m_yaw.value * mouse_x;

	if (in_mlook.state & 1)
		V_StopPitchDrift();

	if ((in_mlook.state & 1) && !(in_strafe.state & 1))
	{
		cl.viewangles[PITCH] += m_pitch.value * mouse_y;
		if (cl.viewangles[PITCH] > cl_pitchdown.value)
			cl.viewangles[PITCH] = cl_pitchdown.value;
		if (cl.viewangles[PITCH] < -cl_pitchup.value)
			cl.viewangles[PITCH] = -cl_pitchup.value;
	}
	else
	{
		if ((in_strafe.state & 1) && noclip_anglehack)
			cmd->upmove -= m_forward.value * mouse_y;
		else
			cmd->forwardmove -= m_forward.value * mouse_y;
	}

// if the mouse has moved, force it to the center, so there's room to move
	if (mx || my)
	{
#ifdef _WIN32
		SetCursorPos(window_center_x, window_center_y);
#else
        SDL_WarpMouseGlobal(window_center_x, window_center_y);
#endif
	}
}


/*
===========
IN_Move
===========
*/
void IN_Move( usercmd_t* cmd )
{
	if (!iMouseInUse && mouseactive)
	{
		IN_MouseMove(cmd);
	}

	IN_JoyMove(cmd);
}


/*
===========
IN_Accumulate
===========
*/
void IN_Accumulate( void )
{
	//only accumulate mouse if we are not moving the camera with the mouse
	if (!iMouseInUse)
	{
		if (mouseactive)
		{
#ifdef _WIN32
			GetCursorPos(&current_pos);
#else
			SDL_GetGlobalMouseState(&current_pos.x, &current_pos.y);
#endif

			mx_accum += current_pos.x - window_center_x;
			my_accum += current_pos.y - window_center_y;

		// force the mouse to the center, so there's room to move
#ifdef _WIN32
		SetCursorPos(window_center_x, window_center_y);
#else
        SDL_WarpMouseGlobal(window_center_x, window_center_y);
#endif
		}
	}
}


/*
===================
IN_ClearStates
===================
*/
void IN_ClearStates( void )
{
	if (mouseactive)
	{
		mx_accum = 0;
		my_accum = 0;
		mouse_oldbuttonstate = 0;
	}
}


/*
===============
IN_StartupJoystick
===============
*/
void IN_StartupJoystick (void) 
{ 
	// abort startup if user requests no joystick
	if (COM_CheckParm("-nojoy"))
		return;

	static float flLastCheck = 0.0f;
	if ( flLastCheck > 0.0f && (Sys_FloatTime()-flLastCheck)  < 1.0f )
		return;

	//Con_Printf("IN_StartupJoystick, %f\n", flLastCheck);

	flLastCheck = Sys_FloatTime();

	int nJoysticks = SDL_NumJoysticks();
	if ( nJoysticks > 0 )
	{
		if ( s_pJoystick == NULL )
		{
			for ( int i = 0; i < nJoysticks; i++ )
			{
				if ( SDL_IsGameController( i ) )
				{
					s_pJoystick = SDL_GameControllerOpen( i );
					if ( s_pJoystick )
					{
						//save the joystick's number of buttons and POV status
						joy_numbuttons = SDL_CONTROLLER_BUTTON_MAX;
						joy_haspov = 0;
						
						// old button and POV states default to no buttons pressed
						joy_oldbuttonstate = joy_oldpovstate = 0;
						
						// mark the joystick as available and advanced initialization not completed
						// this is needed as cvars are not available during initialization
						Con_Printf ("joystick found %s\n\n", SDL_GameControllerName(s_pJoystick)); 
						joy_avail = 1; 
						joy_advancedinit = 0;
						break;
					}

				}
			}
		}
	}
	else
	{
		if ( s_pJoystick )
			SDL_GameControllerClose( s_pJoystick );
		s_pJoystick = NULL;
		if ( joy_avail )
		{
			joy_avail = 0; 
			Con_DPrintf ("joystick not found -- driver not present\n\n");
		}
	}
	
}


/*
===========
RawValuePointer
===========
*/
int RawValuePointer (int axis)
{
	switch (axis)
	{
		default:
		case JOY_AXIS_X:
			return SDL_GameControllerGetAxis( s_pJoystick, SDL_CONTROLLER_AXIS_LEFTX );
		case JOY_AXIS_Y:
			return SDL_GameControllerGetAxis( s_pJoystick, SDL_CONTROLLER_AXIS_LEFTY );
		case JOY_AXIS_Z:
			return SDL_GameControllerGetAxis( s_pJoystick, SDL_CONTROLLER_AXIS_RIGHTX );
		case JOY_AXIS_R:
			return SDL_GameControllerGetAxis( s_pJoystick, SDL_CONTROLLER_AXIS_RIGHTY );
		
	}
}


/*
===========
Joy_AdvancedUpdate_f
===========
*/
void Joy_AdvancedUpdate_f( void )
{
	// called once by IN_ReadJoystick and by user whenever an update is needed
	// cvars are now available
	int	i;
	DWORD dwTemp;

	// initialize all the maps
	for (i = 0; i < JOY_MAX_AXES; i++)
	{
		dwAxisMap[i] = AxisNada;
		dwControlMap[i] = JOY_ABSOLUTE_AXIS;
		pdwRawValue[i] = RawValuePointer(i);
	}

	if (joy_advanced.value == 0.0)
	{
		// default joystick initialization
		// 2 axes only with joystick control
		dwAxisMap[JOY_AXIS_X] = AxisTurn;
		// dwControlMap[JOY_AXIS_X] = JOY_ABSOLUTE_AXIS;
		dwAxisMap[JOY_AXIS_Y] = AxisForward;
		// dwControlMap[JOY_AXIS_Y] = JOY_ABSOLUTE_AXIS;
	}
	else
	{
		if (Q_strcmp(joy_name.string, "joystick") != 0)
		{
			// notify user of advanced controller
			Con_Printf("\n%s configured\n\n", joy_name.string);
		}

		// advanced initialization here
		// data supplied by user via joy_axisn cvars
		dwTemp = (DWORD)joy_advaxisx.value;
		dwAxisMap[JOY_AXIS_X] = dwTemp & 0x0000000f;
		dwControlMap[JOY_AXIS_X] = dwTemp & JOY_RELATIVE_AXIS;
		dwTemp = (DWORD)joy_advaxisy.value;
		dwAxisMap[JOY_AXIS_Y] = dwTemp & 0x0000000f;
		dwControlMap[JOY_AXIS_Y] = dwTemp & JOY_RELATIVE_AXIS;
		dwTemp = (DWORD)joy_advaxisz.value;
		dwAxisMap[JOY_AXIS_Z] = dwTemp & 0x0000000f;
		dwControlMap[JOY_AXIS_Z] = dwTemp & JOY_RELATIVE_AXIS;
		dwTemp = (DWORD)joy_advaxisr.value;
		dwAxisMap[JOY_AXIS_R] = dwTemp & 0x0000000f;
		dwControlMap[JOY_AXIS_R] = dwTemp & JOY_RELATIVE_AXIS;
		dwTemp = (DWORD)joy_advaxisu.value;
		dwAxisMap[JOY_AXIS_U] = dwTemp & 0x0000000f;
		dwControlMap[JOY_AXIS_U] = dwTemp & JOY_RELATIVE_AXIS;
		dwTemp = (DWORD)joy_advaxisv.value;
		dwAxisMap[JOY_AXIS_V] = dwTemp & 0x0000000f;
		dwControlMap[JOY_AXIS_V] = dwTemp & JOY_RELATIVE_AXIS;
	}
}


/*
===========
IN_Commands
===========
*/
void IN_Commands( void )
{
int		i, key_index;

	if (!joy_avail)
	{
		return;
	}

	DWORD	buttonstate, povstate;
	
	// loop through the joystick buttons
	// key a joystick event or auxillary event for higher number buttons for each state change
	buttonstate = 0;
	for ( i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++ )
	{
		if ( SDL_GameControllerGetButton( s_pJoystick, (SDL_GameControllerButton)i ) )
		{
			buttonstate |= 1<<i;
		}
	}
	
	for (i = 0; i < JOY_MAX_AXES; i++)
	{
		pdwRawValue[i] = RawValuePointer(i);
	}

	for (i=0 ; i < (int)joy_numbuttons ; i++)
	{
		if ( (buttonstate & (1<<i)) && !(joy_oldbuttonstate & (1<<i)) )
		{
			key_index = (i < 4) ? K_JOY1 : K_AUX1;
			Key_Event (key_index + i, 1);
			//Con_Printf ("Button %d pressed\n", i);
		}

		if ( !(buttonstate & (1<<i)) && (joy_oldbuttonstate & (1<<i)) )
		{
			key_index = (i < 4) ? K_JOY1 : K_AUX1;
			Key_Event (key_index + i, 0);
			//Con_Printf ("Button %d released\n", i);
		}
	}
	joy_oldbuttonstate = buttonstate;

	if (joy_haspov)
	{
		// convert POV information into 4 bits of state information
		// this avoids any potential problems related to moving from one
		// direction to another without going through the center position
		povstate = 0;
		// determine which bits have changed and key an auxillary event for each change
		for (i=0 ; i < 4 ; i++)
		{
			if ( (povstate & (1<<i)) && !(joy_oldpovstate & (1<<i)) )
			{
				Key_Event (K_AUX29 + i, 1);
			}

			if ( !(povstate & (1<<i)) && (joy_oldpovstate & (1<<i)) )
			{
				Key_Event (K_AUX29 + i, 0);
			}
		}
		joy_oldpovstate = povstate;
	}
}


/*
===============
IN_ReadJoystick
===============
*/
qboolean IN_ReadJoystick( void )
{
	SDL_JoystickUpdate();
	return 1;
}


/*
===========
IN_JoyMove
===========
*/
void IN_JoyMove( usercmd_t *cmd )
{
	float	speed, aspeed;
	float	fAxisValue, fTemp;
	int		i;

	// complete initialization if first time in
	// this is needed as cvars are not available at initialization time
	if (joy_advancedinit != 1)
	{
		Joy_AdvancedUpdate_f();
		joy_advancedinit = 1;
	}

	// verify joystick is available and that the user wants to use it
	if (!joy_avail || !in_joystick.value)
	{
		return;
	}

	// collect the joystick data, if possible
	if (IN_ReadJoystick() != TRUE)
	{
		return;
	}

	if (in_speed.state & 1)
		speed = cl_movespeedkey.value;
	else
		speed = 1;
	aspeed = speed * host_frametime;

	// loop through the axes
	for (i = 0; i < JOY_MAX_AXES; i++)
	{
		// get the floating point zero-centered, potentially-inverted data for the current axis
		fAxisValue = (float)pdwRawValue[i];
		// move centerpoint to zero
		fAxisValue -= 32768.0;

		if (joy_wwhack2.value != 0.0)
		{
			if (dwAxisMap[i] == AxisTurn)
			{
				// this is a special formula for the Logitech WingMan Warrior
				// y=ax^b; where a = 300 and b = 1.3
				// also x values are in increments of 800 (so this is factored out)
				// then bounds check result to level out excessively high spin rates
				fTemp = 300.0 * pow(abs(fAxisValue) / 800.0, 1.3);
				if (fTemp > 14000.0)
					fTemp = 14000.0;
				// restore direction information
				fAxisValue = (fAxisValue > 0.0) ? fTemp : -fTemp;
			}
		}

		// convert range from -32768..32767 to -1..1 
		fAxisValue /= 32768.0;

		switch (dwAxisMap[i])
		{
		case AxisForward:
			if ((joy_advanced.value == 0.0) && (in_mlook.state & 1))
			{
				// user wants forward control to become look control
				if (fabs(fAxisValue) > joy_pitchthreshold.value)
				{
					// if mouse invert is on, invert the joystick pitch value
					// only absolute control support here (joy_advanced is 0)
					if (m_pitch.value < 0.0)
					{
						cl.viewangles[PITCH] -= (fAxisValue * joy_pitchsensitivity.value) * aspeed * cl_pitchspeed.value;
					}
					else
					{
						cl.viewangles[PITCH] += (fAxisValue * joy_pitchsensitivity.value) * aspeed * cl_pitchspeed.value;
					}
					V_StopPitchDrift();
				}
				else
				{
					// no pitch movement
					// disable pitch return-to-center unless requested by user
					// *** this code can be removed when the lookspring bug is fixed
					// *** the bug always has the lookspring feature on
					if (lookspring.value == 0.0)
						V_StopPitchDrift();
				}
			}
			else
			{
				// user wants forward control to be forward control
				if (fabs(fAxisValue) > joy_forwardthreshold.value)
				{
					cmd->forwardmove += (fAxisValue * joy_forwardsensitivity.value) * speed * cl_forwardspeed.value;
				}
			}
			break;

		case AxisSide:
			if (fabs(fAxisValue) > joy_sidethreshold.value)
			{
				cmd->sidemove += (fAxisValue * joy_sidesensitivity.value) * speed * cl_sidespeed.value;
			}
			break;

		case AxisTurn:
			if ((in_strafe.state & 1) || (lookstrafe.value && (in_mlook.state & 1)))
			{
				// user wants turn control to become side control
				if (fabs(fAxisValue) > joy_sidethreshold.value)
				{
					cmd->sidemove -= (fAxisValue * joy_sidesensitivity.value) * speed * cl_sidespeed.value;
				}
			}
			else
			{
				// user wants turn control to be turn control
				if (fabs(fAxisValue) > joy_yawthreshold.value)
				{
					if (dwControlMap[i] == JOY_ABSOLUTE_AXIS)
					{
						cl.viewangles[YAW] += (fAxisValue * joy_yawsensitivity.value) * aspeed * cl_yawspeed.value;
					}
					else
					{
						cl.viewangles[YAW] += (fAxisValue * joy_yawsensitivity.value) * speed * 180.0;
					}

				}
			}
			break;

		case AxisLook:
			if (in_mlook.state & 1)
			{
				if (fabs(fAxisValue) > joy_pitchthreshold.value)
				{
					// pitch movement detected and pitch movement desired by user
					if (dwControlMap[i] == JOY_ABSOLUTE_AXIS)
					{
						cl.viewangles[PITCH] += (fAxisValue * joy_pitchsensitivity.value) * aspeed * cl_pitchspeed.value;
					}
					else
					{
						cl.viewangles[PITCH] += (fAxisValue * joy_pitchsensitivity.value) * speed * 180.0;
					}
					V_StopPitchDrift();
				}
				else
				{
					// no pitch movement
					// disable pitch return-to-center unless requested by user
					// *** this code can be removed when the lookspring bug is fixed
					// *** the bug always has the lookspring feature on
					if (lookspring.value == 0.0)
						V_StopPitchDrift();
				}
			}
			break;

		default:
			break;
		}
	}

	// bounds check pitch
	if (cl.viewangles[PITCH] > cl_pitchdown.value)
		cl.viewangles[PITCH] = cl_pitchdown.value;
	if (cl.viewangles[PITCH] < -cl_pitchup.value)
		cl.viewangles[PITCH] = -cl_pitchup.value;
}