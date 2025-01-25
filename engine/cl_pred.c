#include "quakedef.h"
#include "winquake.h"
#include "pmove.h"

cvar_t	cl_nopred = { "cl_nopred", "0" };
cvar_t	cl_pushlatency = { "pushlatency", "-500" };


/*
==============
CL_PredictUsercmd
==============
*/
void CL_PredictUsercmd( player_state_t* from, player_state_t* to, usercmd_t* u, qboolean spectator )
{
	return; // TODO: Remove

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

	// TODO: Implement
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
		// TODO: Implement

		return;
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

	// TODO: Implement
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

	// TODO: Implement
}