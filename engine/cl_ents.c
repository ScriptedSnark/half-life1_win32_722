// cl_ents.c -- entity parsing and management

#include "quakedef.h"

int cl_playerindex; // player index

/*
==================
CL_ParsePacketEntities

An svc_packetentities has just been parsed, deal with the
rest of the data stream.
==================
*/
void CL_ParsePacketEntities( qboolean delta )
{
	if (cls.signon == 2)
	{
		cls.signon = SIGNONS;
		CL_SignonReply();
	}

	// TODO: Implement
}

//========================================

/*
===================
CL_ParsePlayerinfo
===================
*/
extern int parsecountmod;
extern double parsecounttime;
void CL_ParsePlayerinfo( void )
{
	int			msec;
	int			flags;
	player_info_t* info;
	player_state_t* state;
	int			num;
	int			i;
	qboolean	spectator = FALSE;

	num = MSG_ReadByte();
	if (num & PN_SPECTATOR)
	{
		spectator = TRUE;
		num &= ~PN_SPECTATOR;
	}

	if (num > MAX_CLIENTS)
		Sys_Error("CL_ParsePlayerinfo: bad num");

	info = &cl.players[num];
	info->spectator = spectator;

	state = &cl.frames[parsecountmod].playerstate[num];

	// TODO: Implement

	flags = state->flags = MSG_ReadLong();

	int stub1 = MSG_ReadLong(); // TODO: Implement (state + 144) =
	state->messagenum = cl.parsecount;
	state->origin[0] = MSG_ReadCoord();
	state->origin[1] = MSG_ReadCoord();
	state->origin[2] = MSG_ReadCoord();

	// TODO: Implement

	state->frame = MSG_ReadByte();

	// Count player bits
	for (i = 0; i < MAX_CLIENTS; i++)
	{
		if (flags & (1 << i))
			g_playerbits[i]++;
	}

	// the other player's last move was likely some time
	// before the packet was sent out, so accurately track
	// the exact time it was valid at
	if (flags & PF_MSEC)
	{
		msec = MSG_ReadByte();
		state->state_time = parsecounttime - msec * 0.001;
	}
	else
	{
		state->state_time = parsecounttime;
	}

	if (flags & PF_COMMAND)
	{
		usercmd_t nullcmd;
		memset(&nullcmd, 0, sizeof(nullcmd));
		MSG_ReadUsercmd(&state->command, &nullcmd);
		// TODO: Implement
	}
	else
	{
		// TODO: Implement
	}

	for (i = 0; i < 3; i++)
	{
		if (flags & (4 << i))
		{
			MSG_ReadShort(); // TODO: Implement (= to ?)
			// TODO: Implement
		}
		else
		{
			// TODO: Implement
		}
	}

	if (flags & PF_MODEL)
		state->modelindex = MSG_ReadShort();
	else
		state->modelindex = cl_playerindex;

	if (flags & PF_SKINNUM)
		state->skinnum = MSG_ReadByte();
	else
		state->skinnum = 0;

	if (flags & PF_EFFECTS)
		state->effects = MSG_ReadByte();
	else
		state->effects = 0;

	if (flags & PF_WEAPONMODEL)
		state->weaponmodel = MSG_ReadShort();
	else
		state->weaponmodel = 0;

	if (flags & PF_MOVETYPE)
		state->movetype = MSG_ReadByte();
	else
		state->movetype = 0;

	if (flags & PF_SEQUENCE)
	{
		state->sequence = MSG_ReadByte();

		// TODO: Implement

		MSG_ReadByte(); // TODO: Implement

		// TODO: Implement
	}

	if (flags & PF_RENDER)
	{
		state->rendermode = MSG_ReadByte();
		state->renderamt = MSG_ReadByte();
		state->rendercolor.r = MSG_ReadByte();
		state->rendercolor.g = MSG_ReadByte();
		state->rendercolor.b = MSG_ReadByte();
		state->renderfx = MSG_ReadByte();
	}
	else
	{
		state->rendermode = 0;
		state->renderamt = 0;
		state->rendercolor.r = 0;
		state->rendercolor.g = 0;
		state->rendercolor.b = 0;
		state->renderfx = 0;
	}

	if (flags & PF_FRAMERATE)
		state->framerate = MSG_ReadChar() / 16.0;
	else
		state->framerate = 1;

	if (flags & PF_BODY)
		state->body = MSG_ReadByte();
	else
		state->body = 0;

	if (flags & PF_CONTROLLER1)
		state->controller[0] = MSG_ReadByte();
	else
		state->controller[0] = 0;

	if (flags & PF_CONTROLLER2)
		state->controller[1] = MSG_ReadByte();
	else
		state->controller[1] = 0;

	if (flags & PF_CONTROLLER3)
		state->controller[2] = MSG_ReadByte();
	else
		state->controller[2] = 0;

	if (flags & PF_CONTROLLER4)
		state->controller[3] = MSG_ReadByte();
	else
		state->controller[3] = 0;

	if (flags & PF_BLENDING1)
		state->blending[0] = MSG_ReadByte();
	else
		state->blending[0] = 0;

	if (flags & PF_BLENDING2)
		state->blending[1] = MSG_ReadByte();
	else
		state->blending[0] = 0;

	if (flags & PF_BASEVELOCITY)
	{
		state->basevelocity[0] = MSG_ReadShort();
		state->basevelocity[1] = MSG_ReadShort();
		state->basevelocity[2] = MSG_ReadShort();
	}
	else
	{
		VectorCopy(vec3_origin, state->basevelocity);
	}

	if (flags & PF_FRICTION)
	{
		state->friction = MSG_ReadShort();
	}
	else
	{
		state->friction = 1.0;
	}

	if (flags & PF_PING)
		MSG_ReadShort(); // TODO: Implement (info->ping = ???)

	// TODO: Implement
}

// TODO: Implement

void CL_SetUpPlayerPrediction( qboolean dopred )
{
	// TODO: Implement
}


void CL_EmitEntities( void )
{
	// TODO: Implement
}