#include "quakedef.h"
#include "winquake.h"
#include "pmove.h"

cvar_t	cl_nopred = { "cl_nopred", "0" };
cvar_t	cl_pushlatency = { "pushlatency", "-500" };
cvar_t	cl_dumpents = { "cl_dumpents", "0" };
cvar_t	cl_showpred = { "cl_showpred", "0" };


/*
==============
CL_PredictUsercmd
==============
*/
void CL_PredictUsercmd( player_state_t* from, player_state_t* to, usercmd_t* u, qboolean spectator )
{
	usercmd_t   cmd;

	// chop up very long commands
	if (u->msec > 50)
	{
		player_state_t	temp;
		usercmd_t	split;

		split = *u;
		split.msec /= 2;

		CL_PredictUsercmd(from, &temp, &split, spectator);
		CL_PredictUsercmd(&temp, to, &split, spectator);
		return;
	}

	cmd = *u;
	*to = *from;

	VectorCopy(from->origin, pmove.origin);
	VectorCopy(from->velocity, pmove.velocity);
	VectorCopy(from->basevelocity, pmove.basevelocity);

	VectorCopy(cmd.angles, pmove.angles);
	// Player pitch is inverted
	pmove.angles[PITCH] /= -3.0;
	// Adjust client view angles to match values used on server.
	if (pmove.angles[YAW] > 180.0f)
	{
		pmove.angles[YAW] -= 360.0f;
	}
	
	pmove.usehull = 0;
	pmove.friction = from->friction;
	pmove.oldbuttons = from->oldbuttons;
	pmove.waterjumptime = from->waterjumptime;
	pmove.spectator = spectator;
	pmove.dead = cl.stats[STAT_HEALTH] <= 0;
	pmove.movetype = from->movetype;
	pmove.gravity = 1.0;
	pmove.flags = from->physflags;

//
// adjust movement for ducking
//
	if (pmove.flags & FL_DUCKING)
	{
		cmd.forwardmove *= 0.333;
		cmd.sidemove *= 0.333;
		cmd.upmove *= 0.333;
		pmove.usehull = 1;
	}

	if (pmove.flags & (FL_FROZEN | FL_ONTRAIN))
	{
		cmd.forwardmove = 0;
		cmd.sidemove = 0;
		cmd.upmove = 0;
	}

	pmove.cmd = cmd;
	pmove.player_index = to->number;

	float maxspeed;

	maxspeed = cl.players[pmove.player_index].maxspeed;
	if (maxspeed)
	{
		if (maxspeed >= movevars.maxspeed)
			maxspeed = movevars.maxspeed;
		pmove.maxspeed = maxspeed;
	}
	else
	{
		pmove.maxspeed = movevars.maxspeed;
	}

	PlayerMove(FALSE);

	VectorCopy(pmove.origin, to->origin);
	VectorCopy(pmove.velocity, to->velocity);
	VectorCopy(pmove.basevelocity, to->basevelocity);

	to->waterjumptime = pmove.waterjumptime;
	to->oldbuttons = pmove.cmd.buttons;
	to->onground = onground;
	to->friction = pmove.friction;
	to->movetype = pmove.movetype;
	to->physflags = pmove.flags;
}



/*
==============
CL_PredictMove
==============
*/
void CL_PredictMove( void )
{
	int			i, j;
	float		f;
	frame_t* from, * to = NULL;
	float       targettime;
	int			oldphysent;

	if (cl_pushlatency.value > 0)
		Cvar_Set("pushlatency", "0");

	targettime = 0 - cls.latency - cl_pushlatency.value * 0.001;
	if (targettime > 0)
		targettime = 0;

	targettime += realtime;

	if (cl.intermission)
		return;

	if (!cl.validsequence)
		return;

	if (cls.netchan.outgoing_sequence - cls.netchan.incoming_sequence >= UPDATE_BACKUP - 1)
		return;

	VectorCopy(cl.viewangles, cl.simangles);

	if (cls.demoplayback)
		return;

	// this is the last frame received from the server
	from = &cl.frames[cls.netchan.incoming_sequence & UPDATE_MASK];

	if (cl.spectator)
	{
		if (autocam == CAM_FIRSTPERSON)
		{
			VectorCopy(from->playerstate[spec_track].velocity, cl.simvel);
			VectorCopy(from->playerstate[spec_track].origin, cl.simorg);
			return;
		}

		if (autocam == CAM_TOPDOWN)
		{
			vec3_t pos;
			Cam_GetTopDownOrigin(from->playerstate[spec_track].origin, pos);
			VectorCopy(from->playerstate[spec_track].velocity, cl.simvel);
			VectorCopy(pos, cl.simorg);
			return;
		}
	}

	if (cl_nopred.value)
	{
		VectorCopy(from->playerstate[cl.playernum].velocity, cl.simvel);
		VectorCopy(from->playerstate[cl.playernum].origin, cl.simorg);
		return;
	}

	// predict forward until cl.time <= to->senttime
	oldphysent = pmove.numphysent;
	CL_SetSolidPlayers(cl.playernum);

//	to = &cl.frames[cls.netchan.incoming_sequence & UPDATE_MASK];

	j = 0;
	for (i = 1; i < UPDATE_BACKUP - 1 && cls.netchan.incoming_sequence + i <
			cls.netchan.outgoing_sequence; i++)
	{
		j++;
		to = &cl.frames[(cls.netchan.incoming_sequence + i) & UPDATE_MASK];
		CL_PredictUsercmd(&from->playerstate[cl.playernum]
			, &to->playerstate[cl.playernum], &to->cmd, cl.spectator);
		if (to->senttime >= targettime)
			break;
		from = to;
	}

	if (!j)
		to = from;

	pmove.numphysent = oldphysent;

	if (i == UPDATE_BACKUP - 1 || !to)
		return;		// net hasn't deliver packets in a long time...

	// now interpolate some fraction of the final frame
	if (to->senttime == from->senttime)
		f = 0;
	else
	{
		f = (targettime - from->senttime) / (to->senttime - from->senttime);

		if (f < 0)
			f = 0;
	}

	for (i = 0; i < 3; i++)
	{
		if (fabs(to->playerstate[cl.playernum].origin[i] - from->playerstate[cl.playernum].origin[i]) > 128)
		{	// teleported, so don't lerp
			VectorCopy(to->playerstate[cl.playernum].velocity, cl.simvel);
			VectorCopy(to->playerstate[cl.playernum].origin, cl.simorg);
			VectorCopy(cl.simorg, to->playerstate[cl.playernum].prevorigin);
			return;
		}
	}

	for (i = 0; i < 3; i++)
	{
		cl.simorg[i] = from->playerstate[cl.playernum].origin[i]
			+ f * (to->playerstate[cl.playernum].origin[i] - from->playerstate[cl.playernum].origin[i]);
		cl.simvel[i] = from->playerstate[cl.playernum].velocity[i]
			+ f * (to->playerstate[cl.playernum].velocity[i] - from->playerstate[cl.playernum].velocity[i]);
	}

	VectorCopy(cl.simorg, to->playerstate[cl.playernum].prevorigin);

	if (cl_showpred.value)
	{
		CL_Particle(cl.simorg, 10, 15, 32, -1);
	}
}


/*
==============
CL_InitPrediction
==============
*/
void CL_InitPrediction( void )
{
	Cvar_RegisterVariable(&cl_pushlatency);
	Cvar_RegisterVariable(&cl_nopred);

	Cvar_RegisterVariable(&pm_nostudio);
	Cvar_RegisterVariable(&pm_nocomplex);
	Cvar_RegisterVariable(&pm_worldonly);
	Cvar_RegisterVariable(&pm_nostucktouch);
}