// sv_user.c -- server code for moving users

#include "quakedef.h"
#include "pmove.h"
#include "view.h"

edict_t* sv_player;

// world
float* angles;
float* origin;
float* velocity;

usercmd_t	cmd;

void SV_PostRunCmd(	void );

/*
==================
SV_NextUpload
==================
*/
void SV_NextUpload(void)
{
	// TODO: Implement
}

// TODO: Implement

void DropPunchAngle( void )
{
	float	len;

	len = VectorNormalize(sv_player->v.punchangle);

	len -= (len * 0.5 + 10.0) * host_frametime;
	if (len < 0)
		len = 0;
	VectorScale(sv_player->v.punchangle, len, sv_player->v.punchangle);
}

void SV_PreRunCmd( void )
{
	// TODO: Implement
}

//============================================================================

vec3_t	pmove_mins, pmove_maxs;

/*
====================
AddLinksToPmove

====================
*/
void AddLinksToPmove( areanode_t* node )
{
	// TODO: Implement
}

/*
===========
SV_RunCmd
===========
*/
void SV_RunCmd( usercmd_t* ucmd )
{
	edict_t* ent;
	trace_t		trace;
	pmtrace_t* touch;
	int			i, n;
	float		oldmsec;

	cmd = *ucmd;

	// chop up very long commands
	if (cmd.msec > 50)
	{
		oldmsec = ucmd->msec;
		cmd.msec = oldmsec / 2;
		SV_RunCmd(&cmd);
		cmd.msec = oldmsec / 2;
		cmd.impulse = 0;
		SV_RunCmd(&cmd);
		return;
	}

	DropPunchAngle();

	VectorCopy(vec3_origin, sv_player->v.clbasevelocity);

	if (sv_player->v.fixangle == 0)
		VectorCopy(ucmd->angles, sv_player->v.v_angle);

	sv_player->v.button = ucmd->buttons;
	if (ucmd->impulse)
		sv_player->v.impulse = ucmd->impulse;

	// Checks if an entity is standing on a moving entity to adjust the velocity
	if (sv_player->v.flags & FL_ONGROUND)
	{
		edict_t* groundentity;

		groundentity = sv_player->v.groundentity;
		if (groundentity)
		{
			if (groundentity->v.flags & FL_CONVEYOR)
			{
				if (sv_player->v.flags & FL_BASEVELOCITY)
				{
					VectorMA(sv_player->v.basevelocity, groundentity->v.speed, groundentity->v.movedir, sv_player->v.basevelocity);
				}
				else
				{
					VectorScale(groundentity->v.movedir, groundentity->v.speed, sv_player->v.basevelocity);
				}
				sv_player->v.flags |= FL_BASEVELOCITY;
			}
		}
	}

	frametime = ucmd->msec * 0.001;
	if (frametime > 0.1)
		frametime = 0.1;

	if (!(sv_player->v.flags & FL_BASEVELOCITY))
	{
		// Apply momentum (add in half of the previous frame of velocity first)
		VectorMA(sv_player->v.velocity, 1.0 + (frametime * 0.5), sv_player->v.basevelocity, sv_player->v.velocity);
		VectorCopy(vec3_origin, sv_player->v.basevelocity);
	}

	sv_player->v.flags &= ~FL_BASEVELOCITY;

	if (!host_client->spectator)
	{
		gGlobalVariables.time = sv.time;
		gEntityInterface.pfnPlayerPreThink(sv_player);

		SV_RunThink(sv_player);
	}

	pmove.usehull = 0;

	if (sv_player->v.flags & FL_DUCKING)
	{
		cmd.forwardmove *= PLAYER_DUCKING_MULTIPLIER;
		cmd.sidemove *= PLAYER_DUCKING_MULTIPLIER;
		cmd.upmove *= PLAYER_DUCKING_MULTIPLIER;
		pmove.usehull = 1;
	}

	if (sv_player->v.flags & (FL_FROZEN | FL_ONTRAIN))
	{
		cmd.forwardmove = 0;
		cmd.sidemove = 0;
		cmd.upmove = 0;
	}

	if (sv_player->v.basevelocity[0] || sv_player->v.basevelocity[1] || sv_player->v.basevelocity[2])
		VectorCopy(sv_player->v.basevelocity, sv_player->v.clbasevelocity);

	VectorAdd(sv_player->v.v_angle, sv_player->v.punchangle, sv_player->v.v_angle);
	sv_player->v.angles[ROLL] = V_CalcRoll(sv_player->v.angles, sv_player->v.velocity) * 4;
	if (sv_player->v.fixangle == 0)
	{
		sv_player->v.angles[PITCH] = -sv_player->v.v_angle[PITCH] / 3;
		sv_player->v.angles[YAW] = sv_player->v.v_angle[YAW];
	}

	for (i = 0; i < 3; i++)
		pmove.origin[i] = sv_player->v.origin[i] + (sv_player->v.mins[i] - player_mins[pmove.usehull][i]);
	VectorCopy(sv_player->v.velocity, pmove.velocity);
	VectorCopy(sv_player->v.movedir, pmove.movedir);
	VectorCopy(sv_player->v.v_angle, pmove.angles);
	VectorCopy(sv_player->v.basevelocity, pmove.basevelocity);
	VectorCopy(sv_player->v.view_ofs, pmove.view_ofs);

	pmove.gravity = sv_player->v.gravity;
	pmove.friction = sv_player->v.friction;
	pmove.spectator = host_client->spectator;
	pmove.waterjumptime = sv_player->v.teleport_time;
	pmove.cmd = cmd;
	pmove.dead = sv_player->v.health <= 0;
	pmove.oldbuttons = host_client->oldbuttons;
	pmove.movetype = sv_player->v.movetype;
	pmove.flags = sv_player->v.flags;

	for (i = 0; i < 3; i++)
	{
		pmove_mins[i] = pmove.origin[i] - 256;
		pmove_maxs[i] = pmove.origin[i] + 256;
	}

	if (host_client->maxspeed)
	{
		float maxspeed;

		maxspeed = host_client->maxspeed;
		if (maxspeed >= sv_maxspeed.value)
			maxspeed = sv_maxspeed.value;
		pmove.maxspeed = maxspeed;
	}
	else
	{
		pmove.maxspeed = sv_maxspeed.value;
	}

	pmove.numphysent = 1;
	pmove.physents[0].model = sv.worldmodel;

	AddLinksToPmove(sv_areanodes);

	if (pm_pushfix.value)
		SV_LinkEdict(sv_player, TRUE);

	pmove.player_index = NUM_FOR_EDICT(sv_player);

	PlayerMove(TRUE);

	if (pmove.movetype == MOVETYPE_WALK)
		pmove.friction = 1.0;

	host_client->oldbuttons = pmove.oldbuttons;
	sv_player->v.teleport_time = pmove.waterjumptime;
	sv_player->v.waterlevel = waterlevel;
	sv_player->v.watertype = watertype;
	sv_player->v.flags = pmove.flags;
	sv_player->v.friction = pmove.friction;

	if (onground != -1)
	{
		sv_player->v.flags = (int)sv_player->v.flags | FL_ONGROUND;
		sv_player->v.groundentity = EDICT_NUM(pmove.physents[onground].info);
	}
	else
		sv_player->v.flags = (int)sv_player->v.flags & ~FL_ONGROUND;

	onground = onground != -1;

	for (i = 0; i < 3; i++)
		sv_player->v.origin[i] = pmove.origin[i] - (sv_player->v.mins[i] - player_mins[pmove.usehull][i]);

	if (!pm_pushfix.value)
		VectorCopy(pmove.velocity, sv_player->v.velocity);

	origin = sv_player->v.origin;
	velocity = sv_player->v.velocity;

	VectorCopy(pmove.basevelocity, sv_player->v.basevelocity);

	if (!pm_pushfix.value)
	{
		if (!host_client->spectator)
		{
			// link into place and touch triggers
			SV_LinkEdict(sv_player, TRUE);

			vec3_t save_velocity;
			VectorCopy(sv_player->v.velocity, save_velocity);

			// touch other objects
			for (i = 0; i < pmove.numtouch; i++)
			{
				touch = &pmove.touchindex[i];
				n = pmove.physents[touch->ent].info;
				ent = EDICT_NUM(n);
				if ((g_playertouch[n / 8] & (1 << (n % 8))) || (ent->v.flags & FL_SPECTATOR))
					continue;

				trace.allsolid = touch->allsolid;
				trace.startsolid = touch->startsolid;
				trace.inopen = touch->inopen;
				trace.inwater = touch->inwater;
				trace.fraction = touch->fraction;
				VectorCopy(touch->endpos, trace.endpos);
				VectorCopy(touch->plane.normal, trace.plane.normal);
				trace.plane.dist = touch->plane.dist;
				trace.hitgroup = touch->hitgroup;
				trace.ent = ent;

				VectorCopy(touch->deltavelocity, sv_player->v.velocity);
				SV_Impact(ent, sv_player, &trace);
			}

			VectorCopy(save_velocity, sv_player->v.velocity);
		}
	}
	else
	{
		if (!host_client->spectator)
		{
			vec3_t velocity;
			VectorCopy(vec3_origin, velocity);

			// touch other objects
			for (i = 0; i < pmove.numtouch; i++)
			{
				touch = &pmove.touchindex[i];
				VectorAdd(velocity, touch->deltavelocity, velocity);
			}

			PM_CheckVelocity();

			VectorAdd(pmove.velocity, velocity, sv_player->v.velocity);
			SV_LinkEdict(sv_player, TRUE);
		}
	}

	SV_PostRunCmd();
}

void SV_PostRunCmd( void )
{
	gGlobalVariables.time = sv.time;

	if (host_client->spectator)
	{
		gEntityInterface.pfnSpectatorThink(sv_player);
	}
	else
	{
		gEntityInterface.pfnPlayerPostThink(sv_player);
	}
}

/*
===================
SV_ExecuteClientMessage

The current net_message is parsed for the given client
===================
*/
void SV_ExecuteClientMessage( client_t* cl )
{
	int				c, nCommandValidatedStatus;
	char*			s;
	usercmd_t		oldest, oldcmd, newcmd, nullcmd;
	client_frame_t* frame;
	vec3_t			o;
	qboolean		move_issued = FALSE; //only allow one move command
	int				checksumIndex;
	byte			checksum, calculatedChecksum;

	// calc ping time
	frame = &cl->frames[cl->netchan.incoming_acknowledged & UPDATE_MASK];
	frame->ping_time = realtime - cl->frames[cl->netchan.incoming_acknowledged & UPDATE_MASK].frame_time - frame->senttime; //cl->frames[cl->netchan.incoming_acknowledged & UPDATE_MASK].entities.num_entities wtf?


	// make sure the reply sequence number matches the incoming
	// sequence number 
	if (cl->netchan.incoming_sequence >= cl->netchan.outgoing_sequence)
		cl->netchan.outgoing_sequence = cl->netchan.incoming_sequence;
	else
		cl->send_message = FALSE;	// don't reply, sequences have slipped

	// save time for ping calculations
	cl->frames[cl->netchan.outgoing_sequence & UPDATE_MASK].senttime = realtime;
	cl->frames[cl->netchan.outgoing_sequence & UPDATE_MASK].frame_time = host_frametime;
	cl->frames[cl->netchan.outgoing_sequence & UPDATE_MASK].ping_time = -1;

	host_client = cl;
	sv_player = host_client->edict;

	// mark time so clients will know how much to predict
	// other players
	cl->localtime = sv.time;
	cl->delta_sequence = -1;	// no delta unless requested

	while (2)
	{
		if (msg_badread)
		{
			Con_Printf("SV_ReadClientMessage: badread\n");
			SV_DropClient(cl, FALSE);
			return;
		}

		c = MSG_ReadByte();
		if (c == -1)
			break;

		switch (c)
		{
			default:
				Con_Printf("SV_ReadClientMessage: unknown command char\n");
				SV_DropClient(cl, FALSE);
				return;
		case clc_nop:
			break;
		case clc_move:
			if (move_issued)
				return;		// someone is trying to cheat...

			move_issued = TRUE;
			memset(&nullcmd, 0, sizeof(nullcmd));
			checksumIndex = msg_readcount;
			checksum = MSG_ReadByte();
			MSG_ReadUsercmd(&oldest, &nullcmd);
			MSG_ReadUsercmd(&oldcmd, &oldest);
			MSG_ReadUsercmd(&newcmd, &oldcmd);

			// don't move if haven't spawned yet
			if (!cl->active && !cl->spawned || !sv.active)
				break;

			// if the checksum fails, ignore the rest of the packet
			calculatedChecksum = COM_BlockSequenceCRCByte(
							   net_message.data + checksumIndex + 1,
							   msg_readcount - checksumIndex - 1,
							   cl->netchan.incoming_sequence);

			if (calculatedChecksum != checksum)
			{
				Con_DPrintf("Failed command checksum for %s (%d != %d)/%d\n", cl->name, calculatedChecksum, checksum, cl->netchan.incoming_sequence);
				return;
			}

			if (sv.paused || svs.maxclients <= 1 && key_dest != key_game || (sv_player->v.flags & FL_FROZEN) != 0)
			{
				newcmd.buttons = 0;
				newcmd.msec = 0;

				newcmd.forwardmove = 0;
				newcmd.sidemove = 0;
				newcmd.upmove = 0;

				if ((sv_player->v.flags & FL_FROZEN) != 0)
					newcmd.impulse = 0;

				memcpy(&oldest, &newcmd, sizeof(oldest));
				memcpy(&oldcmd, &newcmd, sizeof(oldcmd));
				VectorCopy(host_client->edict->v.v_angle, newcmd.angles);
			}
			else
			{
				VectorCopy(newcmd.angles, host_client->edict->v.v_angle);
			}

			host_client->edict->v.button = newcmd.buttons;

			if (newcmd.impulse)
				host_client->edict->v.impulse = newcmd.impulse;

			host_client->edict->v.light_level = newcmd.lightlevel;

			SV_PreRunCmd();
			if (net_drop < 16)
			{
				if (net_drop > 2)
				{
					while (net_drop > 2)
					{
						SV_RunCmd(&cl->lastcmd);
						net_drop--;
					}
				}
				if (net_drop > 1)
					SV_RunCmd(&oldest);
				if (net_drop > 0)
					SV_RunCmd(&oldcmd);
			}
			SV_RunCmd(&newcmd);
			cl->lastcmd = newcmd;
			cl->lastcmd.buttons = 0; // avoid multiple fires on lag
			break;
		case clc_stringcmd:
			s = MSG_ReadString();
			nCommandValidatedStatus = !host_client->privileged ? 0 : 2;
			if (!Q_strncasecmp(s, "status", sizeof("status") - 1)
				  || !Q_strncasecmp(s, "god", sizeof("god") - 1)
				  || !Q_strncasecmp(s, "customrsrclist", sizeof("customrsrclist") - 1)
				  || !Q_strncasecmp(s, "notarget", sizeof("notarget") - 1)
				  || !Q_strncasecmp(s, "fly", sizeof("fly") - 1)
				  || !Q_strncasecmp(s, "name", sizeof("name") - 1)
				  || !Q_strncasecmp(s, "noclip", sizeof("noclip") - 1)
				  || !Q_strncasecmp(s, "tell", sizeof("tell") - 1)
				  || !Q_strncasecmp(s, "color", sizeof("color") - 1)
				  || !Q_strncasecmp(s, "kill", sizeof("kill") - 1)
				  || !Q_strncasecmp(s, "pause", sizeof("pause") - 1)
				  || !Q_strncasecmp(s, "spawn", sizeof("spawn") - 1)
				  || !Q_strncasecmp(s, "new", sizeof("new") - 1)
				  || !Q_strncasecmp(s, "dropclient", sizeof("dropclient") - 1)
				  || !Q_strncasecmp(s, "begin", sizeof("begin") - 1)
				  || !Q_strncasecmp(s, "prespawn", sizeof("prespawn") - 1)
				  || !Q_strncasecmp(s, "kick", sizeof("kick") - 1)
				  || !Q_strncasecmp(s, "ping", sizeof("ping") - 1)
				  || !Q_strncasecmp(s, "download", sizeof("download") - 1)
				  || !Q_strncasecmp(s, "resourcelist", sizeof("resourcelist") - 1)
				  || !Q_strncasecmp(s, "nextdl", sizeof("nextdl") - 1)
				  || !Q_strncasecmp(s, "sv_print_custom", sizeof("sv_print_custom") - 1)
				  || !Q_strncasecmp(s, "ban", sizeof("ban") - 1)
				  || !Q_strncasecmp(s, "ptrack", sizeof("ptrack") - 1)) {
				nCommandValidatedStatus = 1; // this is a command which does varying results for every player
			}
			if (nCommandValidatedStatus == 2) // the player is privileged, he can execute anything on the server
			{
				Cbuf_InsertText(s);
			}
			else if (nCommandValidatedStatus == 1)
			{
				Cmd_ExecuteString(s, src_client);
			}
			else // we don't know this command, let the game dll handle it
			{
				Cmd_TokenizeString(s);
				gEntityInterface.pfnClientCommand(host_client->edict);
			}
			break;
		case clc_delta:
			cl->delta_sequence = MSG_ReadByte();
			break;
		case clc_tmove:
			o[0] = MSG_ReadCoord();
			o[1] = MSG_ReadCoord();
			o[2] = MSG_ReadCoord();
			if (host_client->spectator)
			{
				VectorCopy(o, sv_player->v.origin);
				SV_LinkEdict(sv_player, FALSE);
			}
			break;
		case clc_upload:
			SV_NextUpload();
			break;
		case clc_resourcelist:
			SV_ParseResourceList();
			break;
		}
	}
}