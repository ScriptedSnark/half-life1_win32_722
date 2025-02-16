// sv_user.c -- server code for moving users

#include "quakedef.h"
#include "protocol.h"

edict_t	*sv_player;

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

void SV_PreRunCmd( void )
{
	// TODO: Implement
}

// TODO: Implement

void SV_PostRunCmd(	void );
void SV_RunCmd( usercmd_t* ucmd )
{
	// TODO: Implement
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