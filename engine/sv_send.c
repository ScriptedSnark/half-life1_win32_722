// sv_main.c -- server main program

#include "quakedef.h"
#include "cmodel.h"

#define CHAN_AUTO   0
#define CHAN_WEAPON 1
#define CHAN_VOICE  2
#define CHAN_ITEM   3
#define CHAN_BODY   4

typedef struct full_packet_entities_s
{
	int num_entities;
	entity_state_t entities[MAX_PACKET_ENTITIES];
} full_packet_entities_t;

extern int sv_playermodel;

/*
==================
SV_SendUserReg
==================
*/
void SV_SendUserReg( sizebuf_t* sb )
{
	UserMsg* pMsg;

	for (pMsg = sv_gpNewUserMsgs; pMsg; pMsg = pMsg->next)
	{
		MSG_WriteByte(sb, svc_newusermsg);
		MSG_WriteByte(sb, pMsg->iMsg);
		MSG_WriteByte(sb, pMsg->iSize);
		MSG_WriteLong(sb, *(int*)&pMsg->szName[0]);
		MSG_WriteLong(sb, *(int*)&pMsg->szName[4]);
		MSG_WriteLong(sb, *(int*)&pMsg->szName[8]);
		MSG_WriteLong(sb, (int)pMsg->next);
	}
}

/*
=================
SV_SendBan
=================
*/
void SV_SendBan( void )
{
	// TODO: Implement
}

/*
=================
SV_FilterPacket
=================
*/
qboolean SV_FilterPacket( void )
{
	// TODO: Implement

	return FALSE;
}

/*
==================
SV_WriteCustomEntityDeltaToClient

==================
*/
void SV_WriteCustomEntityDeltaToClient( entity_state_t* from, entity_state_t* to, sizebuf_t* msg, qboolean force )
{
	// TODO: Refactor

	int v4; // esi
	int rendermode; // eax
	int v6; // ecx
	float *origin; // ebp
	float *v8; // ebx
	int v9; // ecx
	float *angles; // ebp
	float *v11; // ebx
	int number; // eax
	int v13; // [esp-4h] [ebp-20h]
	unsigned __int32 v14; // [esp+10h] [ebp-Ch]
	float v15; // [esp+14h] [ebp-8h]
	double v16; // [esp+14h] [ebp-8h]
	float v17; // [esp+14h] [ebp-8h]
	double v18; // [esp+14h] [ebp-8h]

	v4 = 8449;
	rendermode = to->rendermode;
	if (from->rendermode != rendermode)
		v4 = 9473;
	v14 = to->rendermode & 0xF;
	if (v14 <= 1)
	{
		v6 = 0;
		origin = from->origin;
		v8 = to->origin;
		do
		{
			v15 = *v8 - *origin;
			v16 = v15;
			if (v16 < -0.1 || v16 > 0.1)
				v4 |= 2 << v6;
			++origin;
			++v8;
			++v6;
		} while (v6 < 3);
		if ((rendermode & 0xF) == 0)
		{
			v9 = 0;
			angles = from->angles;
			v11 = to->angles;
			do
			{
				v17 = *v11 - *angles;
				v18 = v17;
				if (v18 < -0.1 || v18 > 0.1)
					v4 |= 32 << v9;
				++angles;
				++v11;
				++v9;
			} while (v9 < 3);
		}
	}
	if ((v14 == 2 || v14 == 1) && (to->sequence != from->sequence || to->skin != from->skin))
		v4 |= 0x200u;
	if (from->modelindex != to->modelindex)
		v4 |= 0x800u;
	if (from->scale != to->scale)
		v4 |= 0x1000u;
	if (from->body != to->body)
		v4 |= 0x4000u;
	if (from->rendercolor.r != to->rendercolor.r
	  || to->rendercolor.g != from->rendercolor.g
	  || to->rendercolor.b != from->rendercolor.b
	  || to->renderfx != from->renderfx)
	{
		v4 |= 0x10000u;
	}
	if (from->renderamt != to->renderamt)
		v4 |= 0x20000u;
	if (from->frame != to->frame)
		v4 |= 0x80000u;
	if ((__int64)from->animtime != (__int64)to->animtime)
		v4 |= 0x40000u;
	number = to->number;
	if (number >= 256)
		v4 |= 0x8000u;
	if (v4 >= 256)
		v4 |= 1u;
	if (v4 >= 0x10000)
		v4 |= 0x100u;
	if (v4 >= 0x1000000)
		v4 |= 0x800000u;
	if (!number)
		Sys_Error("Unset entity number");
	if (v4 || force)
	{
		MSG_WriteByte(msg, v4);
		if ((v4 & 1) != 0)
			MSG_WriteByte(msg, v4 >> 8);
		if ((v4 & 0x100) != 0)
			MSG_WriteByte(msg, v4 >> 16);
		if ((v4 & 0x800000) != 0)
			MSG_WriteByte(msg, v4 >> 24);
		v13 = to->number;
		if ((v4 & 0x8000) != 0)
			MSG_WriteShort(msg, v13);
		else
			MSG_WriteByte(msg, v13);
		if ((v4 & 2) != 0)
			MSG_WriteCoord(msg, to->origin[0]);
		if ((v4 & 4) != 0)
			MSG_WriteCoord(msg, to->origin[1]);
		if ((v4 & 8) != 0)
			MSG_WriteCoord(msg, to->origin[2]);
		if ((v4 & 0x20) != 0)
			MSG_WriteCoord(msg, to->angles[0]);
		if ((v4 & 0x40) != 0)
			MSG_WriteCoord(msg, to->angles[1]);
		if ((v4 & 0x80) != 0)
			MSG_WriteCoord(msg, to->angles[2]);
		if ((v4 & 0x200) != 0)
		{
			MSG_WriteShort(msg, to->sequence);
			MSG_WriteShort(msg, to->skin);
		}
		if ((v4 & 0x400) != 0)
			MSG_WriteByte(msg, to->rendermode);
		if ((v4 & 0x800) != 0)
			MSG_WriteShort(msg, to->modelindex);
		if ((v4 & 0x1000) != 0)
			MSG_WriteByte(msg, (__int64)to->scale);
		if ((v4 & 0x4000) != 0)
			MSG_WriteByte(msg, to->body);
		if ((v4 & 0x10000) != 0)
		{
			MSG_WriteByte(msg, to->rendercolor.r);
			MSG_WriteByte(msg, to->rendercolor.g);
			MSG_WriteByte(msg, to->rendercolor.b);
			MSG_WriteByte(msg, to->renderfx);
		}
		if ((v4 & 0x20000) != 0)
			MSG_WriteByte(msg, to->renderamt);
		if ((v4 & 0x80000) != 0)
			MSG_WriteByte(msg, (__int64)to->frame);
		if ((v4 & 0x40000) != 0)
			MSG_WriteByte(msg, (__int64)to->animtime);
	}
}

/*
==================
SV_WriteDelta

Writes part of a packetentities message.
Can delta from either a baseline or a previous packet_entity
==================
*/
void SV_WriteDelta( entity_state_t* from, entity_state_t* to, sizebuf_t* msg, qboolean force )
{
	int     bits, bboxbits;
	int		i, num;
	float   miss;

	if (to->entityType != ENTITY_NORMAL)
	{
		SV_WriteCustomEntityDeltaToClient(from, to, msg, force);
		return;
	}

// send an update
	bits = 0;
	bboxbits = 0;

	for (i = 0; i < 3; i++)
	{
		miss = to->origin[i] - from->origin[i];
		if (miss < -0.1 || miss > 0.1)
			bits |= (U_ORIGIN1 << i);
	}

	if (from->angles[0] != to->angles[0])
		bits |= U_ANGLE1;
	if (from->angles[1] != to->angles[1])
		bits |= U_ANGLE2;
	if (from->angles[2] != to->angles[2])
		bits |= U_ANGLE3;

	if (from->movetype != to->movetype)
		bits |= U_MOVETYPE;

	if (from->colormap != to->colormap)
		bits |= U_COLORMAP;

	if (from->skin != to->skin || to->solid != from->solid)
		bits |= U_CONTENTS;

	if (from->frame != to->frame)
		bits |= U_FRAME;

	if (from->scale != to->scale)
		bits |= U_SCALE;

	if (from->effects != to->effects)
		bits |= U_EFFECTS;

	if (from->modelindex != to->modelindex)
		bits |= U_MODELINDEX;

	if (from->animtime != to->animtime || to->sequence != from->sequence)
		bits |= U_SEQUENCE;

	if (from->framerate != to->framerate)
		bits |= U_FRAMERATE;

	if (from->body != to->body)
		bits |= U_BODY;

	if (from->controller[0] != to->controller[0])
		bits |= U_CONTROLLER1;
	if (from->controller[1] != to->controller[1])
		bits |= U_CONTROLLER2;
	if (from->controller[2] != to->controller[2])
		bits |= U_CONTROLLER3;
	if (from->controller[3] != to->controller[3])
		bits |= U_CONTROLLER4;

	if (from->blending[0] != to->blending[0])
		bits |= U_BLENDING1;
	if (from->blending[1] != to->blending[1])
		bits |= U_BLENDING2;

	if (from->rendermode != to->rendermode
		|| to->renderamt != from->renderamt
		|| to->renderfx != from->renderfx
		|| to->rendercolor.r != from->rendercolor.r
		|| from->rendercolor.g != to->rendercolor.g
		|| to->rendercolor.b != from->rendercolor.b)
	{
		bits |= U_RENDER;
	}

	if (to->animtime != 0.0 && to->velocity[0] == 0.0 && to->velocity[1] == 0.0 && to->velocity[2] == 0.0)
		bits |= 0x200000u; //?

	for (i = 0; i < 3; i++)
	{
		miss = to->mins[i] - from->mins[i];
		if (miss < -0.1 || miss > 0.1)
			bboxbits |= (U_BBOXMINS1 << i);

		miss = to->maxs[i] - from->maxs[i];
		if (miss < -0.1 || miss > 0.1)
			bboxbits |= (U_BBOXMAXS1 << i);
	}

	// Check for large entity number
	num = to->number;
	if (num >= MAX_PACKET_ENTITIES)
		bits |= U_LONGENTITY;

	if (bits >= U_EVENMOREBITS)
		bits |= U_MOREBITS;

	if (bits >= U_FRAMERATE)
		bits |= U_EVENMOREBITS;

	if (bits >= U_CONTROLLER1)
		bits |= U_YETMOREBITS;

	if (bboxbits)
		bits |= U_YETMOREBITS | U_EVENMOREBITS | U_MOREBITS | 0x80000000;

	//
	// write the message
	//
	if (!num)
		Sys_Error("Unset entity number");

	if (!bits && !bboxbits && !force)
		return;		// nothing to send!

	MSG_WriteByte(msg, bits & 255);

	if (bits & U_MOREBITS)
		MSG_WriteByte(msg, (bits >> 8) & 255);

	if (bits & U_EVENMOREBITS)
		MSG_WriteByte(msg, (bits >> 16) & 255);

	if (bits & U_YETMOREBITS)
		MSG_WriteByte(msg, (bits >> 24) & 255);

	if (bits & 0x80000000)
		MSG_WriteByte(msg, bboxbits);

	if (bits & U_LONGENTITY)
		MSG_WriteShort(msg, to->number);
	else
		MSG_WriteByte(msg, to->number);

	if (bits & U_MODELINDEX)
		MSG_WriteShort(msg, to->modelindex);
	if (bits & U_FRAME)
		MSG_WriteWord(msg, to->frame * 256);
	if (bits & U_MOVETYPE)
		MSG_WriteByte(msg, to->movetype);
	if (bits & U_COLORMAP)
		MSG_WriteByte(msg, to->colormap);
	if (bits & U_CONTENTS)
	{
		MSG_WriteShort(msg, to->skin);
		MSG_WriteByte(msg, to->solid);
	}
	if (bits & U_SCALE)
		MSG_WriteWord(msg, to->scale * 256);
	if (bits & U_EFFECTS)
		MSG_WriteByte(msg, to->effects);

	if (bits & U_ORIGIN1)
		MSG_WriteCoord(msg, to->origin[0]);
	if (bits & U_ANGLE1)
		MSG_WriteHiresAngle(msg, to->angles[0]);
	if (bits & U_ORIGIN2)
		MSG_WriteCoord(msg, to->origin[1]);
	if (bits & U_ANGLE2)
		MSG_WriteHiresAngle(msg, to->angles[1]);
	if (bits & U_ORIGIN3)
		MSG_WriteCoord(msg, to->origin[2]);
	if (bits & U_ANGLE3)
		MSG_WriteHiresAngle(msg, to->angles[2]);

	if (bits & U_SEQUENCE)
	{
		MSG_WriteByte(msg, to->sequence);
		MSG_WriteByte(msg, (byte)(to->animtime * 100));
	}

	if (bits & U_FRAMERATE)
		MSG_WriteChar(msg, to->framerate * 16);

	if (bits & U_CONTROLLER1)
		MSG_WriteByte(msg, to->controller[0]);
	if (bits & U_CONTROLLER2)
		MSG_WriteByte(msg, to->controller[1]);
	if (bits & U_CONTROLLER3)
		MSG_WriteByte(msg, to->controller[2]);
	if (bits & U_CONTROLLER4)
		MSG_WriteByte(msg, to->controller[3]);

	if (bits & U_BLENDING1)
		MSG_WriteByte(msg, to->blending[0]);
	if (bits & U_BLENDING2)
		MSG_WriteByte(msg, to->blending[1]);

	if (bits & U_BODY)
		MSG_WriteByte(msg, to->body);

	if (bits & U_RENDER)
	{
		MSG_WriteByte(msg, to->rendermode);
		MSG_WriteByte(msg, to->renderamt);
		MSG_WriteByte(msg, to->rendercolor.r);
		MSG_WriteByte(msg, to->rendercolor.g);
		MSG_WriteByte(msg, to->rendercolor.b);
		MSG_WriteByte(msg, to->renderfx);
	}

	if (bboxbits & U_BBOXMINS1)
		MSG_WriteCoord(msg, to->mins[0]);
	if (bboxbits & U_BBOXMINS2)
		MSG_WriteCoord(msg, to->mins[1]);
	if (bboxbits & U_BBOXMINS3)
		MSG_WriteCoord(msg, to->mins[2]);
	if (bboxbits & U_BBOXMAXS1)
		MSG_WriteCoord(msg, to->maxs[0]);
	if (bboxbits & U_BBOXMAXS2)
		MSG_WriteCoord(msg, to->maxs[1]);
	if (bboxbits & U_BBOXMAXS3)
		MSG_WriteCoord(msg, to->maxs[2]);
}

/*
=============
SV_EmitPacketEntities

Writes a delta update of a packet_entities_t to the message.
=============
*/
void SV_EmitPacketEntities( client_t* client, packet_entities_t* to, sizebuf_t* msg )
{
	int         oldnum, newnum;
	int         oldindex;
	int         newindex;
	int         oldmax;
	unsigned int        bits;
	edict_t             *ent;
	packet_entities_t   *from;
	entity_state_t      *state;

	// this is the frame that we are going to delta update from
	if (client->delta_sequence != -1)
	{
		from = &client->frames[client->delta_sequence & UPDATE_MASK].entities;
		oldmax = from->num_entities;

		MSG_WriteByte(msg, svc_deltapacketentities);
		MSG_WriteShort(msg, to->num_entities);
		MSG_WriteByte(msg, client->delta_sequence);
	}
	else
	{
		oldmax = 0;	// no delta update
		from = NULL;

		MSG_WriteByte(msg, svc_packetentities);
		MSG_WriteShort(msg, to->num_entities);
	}

	newindex = 0;
	oldindex = 0;

//Con_Printf ("---%i to %i ----\n", client->delta_sequence & UPDATE_MASK
//			, client->netchan.outgoing_sequence & UPDATE_MASK);

	while (newindex < to->num_entities || oldindex < oldmax)
	{
		newnum = newindex >= to->num_entities ? 9999 : to->entities[newindex].number;
		oldnum = oldindex >= oldmax ? 9999 : from->entities[oldindex].number;

		if (newnum == oldnum)
		{	// delta update from old position
//Con_Printf ("delta %i\n", newnum);
			state = &to->entities[newindex];
			SV_WriteDelta(&from->entities[oldindex], state, msg, FALSE);
			newindex++;
			oldindex++;
			continue;
		}

		if (newnum < oldnum)
		{	// this is a new entity, send it from the baseline
			ent = EDICT_NUM(newnum);
//Con_Printf ("baseline %i\n", newnum);
			SV_WriteDelta(&ent->baseline, &to->entities[newindex], msg, TRUE);
			newindex++;
			continue;
		}

		if (newnum > oldnum)
		{	// the old entity isn't present in the new message
//Con_Printf ("remove %i\n", oldnum);
			bits = U_REMOVE;
			if (oldnum >= MAX_PACKET_ENTITIES)
				bits |= U_LONGENTITY;

			if (bits >= U_EVENMOREBITS)
				bits |= U_MOREBITS;

			if (bits >= U_FRAMERATE)
				bits |= U_EVENMOREBITS;

			if (bits >= U_CONTROLLER1)
				bits |= U_YETMOREBITS;

			MSG_WriteByte(msg, bits & 255);

			if (bits & U_MOREBITS)
				MSG_WriteByte(msg, (bits >> 8) & 255);

			if (bits & U_EVENMOREBITS)
				MSG_WriteByte(msg, (bits >> 16) & 255);

			if (bits & U_YETMOREBITS)
				MSG_WriteByte(msg, (bits >> 24) & 255);

			oldindex++;

			if (bits & U_LONGENTITY)
				MSG_WriteShort(msg, oldnum);
			else
				MSG_WriteByte(msg, oldnum);

			continue;
		}
	}

	MSG_WriteLong(msg, 0);	// end of packetentities
}

/*
=============
SV_WritePlayersToClient

=============
*/
void SV_WritePlayersToClient( client_t* client, byte* pvs, sizebuf_t* msg )
{
	int			i, j, k;
	client_t*	cl;
	edict_t*	ent, *clent;
	int			msec;
	usercmd_t	cmd, nullcmd;
	int			pflags;

	clent = client->edict;

	for (j = 0, cl = svs.clients; j < MAX_CLIENTS; j++, cl++)
	{
		if (!cl->spawned && !cl->active)
			continue;

		ent = cl->edict;

		// ZOID visibility tracking
		if (ent != clent &&
			!(client->spec_track && client->spec_track - 1 == j))
		{
			if (cl->spectator)
				continue;

			// ignore if not touching a PV leaf
			for (i = 0; i < ent->num_leafs; i++)
				if (pvs[ent->leafnums[i] >> 3] & (1 << (ent->leafnums[i] & 7)))
					break;
			if (i == ent->num_leafs)
				continue;		// not visible
		}

		pflags = PF_MSEC | PF_COMMAND;
		if (bShouldUpdatePing)
			pflags = PF_PING | PF_COMMAND | PF_MSEC;

		if (ent->v.modelindex != sv_playermodel)
			pflags |= PF_MODEL;
		for (i = 0; i < 3; i++)
			if (ent->v.velocity[i])
				pflags |= PF_VELOCITY1 << i;
		if (ent->v.effects)
			pflags |= PF_EFFECTS;
		if (ent->v.skin)
			pflags |= PF_SKINNUM;
		if (ent->v.health <= 0)
			pflags |= PF_DEAD;
		if (ent->v.mins[2] != -24)
			pflags |= PF_GIB;

		if (cl->spectator)
		{	// only sent origin and velocity to spectators
			pflags &= PF_VELOCITY1 | PF_VELOCITY2 | PF_VELOCITY3;
		}
		else if (ent == clent)
		{	// don't send a lot of data on personal entity
			pflags &= ~(PF_MSEC);
		}
		if (ent->v.weaponmodel)
			pflags |= PF_WEAPONMODEL;
		if (ent->v.movetype)
			pflags |= PF_MOVETYPE;
		if (ent->v.sequence || ent->v.animtime != 0.0)
			pflags |= PF_SEQUENCE;
		if (ent->v.rendermode
		  || ent->v.renderamt != 0.0
		  || ent->v.renderfx
		  || ent->v.rendercolor[0] != 0.0
		  || ent->v.rendercolor[1] != 0.0
		  || ent->v.rendercolor[2] != 0.0)
		{
			pflags |= PF_RENDER;
		}
		if (ent->v.framerate != 1.0)
			pflags |= PF_FRAMERATE;
		if (ent->v.body)
			pflags |= PF_BODY;
		for (i = 0; i < 4; i++)
		{
			if (ent->v.controller[i])
				pflags |= PF_CONTROLLER1 << i;
		}
		for (i = 0; i < 2; i++)
		{
			if (ent->v.blending[i])
				pflags |= PF_BLENDING1 << i;
		}
		if (ent->v.clbasevelocity[0] != 0 || ent->v.clbasevelocity[1] != 0 || ent->v.clbasevelocity[2] != 0)
			pflags |= PF_BASEVELOCITY;
		if (ent->v.friction != 1.0)
			pflags |= PF_FRICTION;

		MSG_WriteByte(msg, svc_playerinfo);
		k = j;
		if (cl->spectator)
			k |= PN_SPECTATOR;
		MSG_WriteByte(msg, k);
		MSG_WriteLong(msg, pflags);
		MSG_WriteLong(msg, ent->v.flags);

		for (i = 0; i < 3; i++)
			MSG_WriteCoord(msg, ent->v.origin[i]);

		MSG_WriteByte(msg, ent->v.frame);

		if (pflags & PF_MSEC)
		{
			msec = 1000 * (sv.time - cl->localtime);
			if (msec > 255)
				msec = 255;
			MSG_WriteByte(msg, msec);
		}

		if (pflags & PF_COMMAND)
		{
			cmd = cl->lastcmd;

			if (ent->v.health <= 0)
			{	// don't show the corpse looking around...
				cmd.angles[0] = 0;
				cmd.angles[1] = ent->v.angles[1];
				cmd.angles[0] = 0;
			}

			cmd.buttons = 0;	// never send buttons
			cmd.impulse = 0;	// never send impulses

			memset(&nullcmd, 0, sizeof(nullcmd));

			MSG_WriteDeltaUsercmd(msg, &cmd, &nullcmd);
		}

		for (i = 0; i < 3; i++)
			if (pflags & (PF_VELOCITY1 << i))
				MSG_WriteShort(msg, ent->v.velocity[i]);

		if (pflags & PF_MODEL)
			MSG_WriteShort(msg, ent->v.modelindex);

		if (pflags & PF_SKINNUM)
			MSG_WriteByte(msg, ent->v.skin);

		if (pflags & PF_EFFECTS)
			MSG_WriteByte(msg, ent->v.effects);

		if (pflags & PF_WEAPONMODEL)
			MSG_WriteShort(msg, SV_ModelIndex(pr_strings + ent->v.weaponmodel));

		if (pflags & PF_MOVETYPE)
			MSG_WriteByte(msg, ent->v.movetype);

		if (pflags & PF_SEQUENCE)
		{
			MSG_WriteByte(msg, ent->v.sequence);
			MSG_WriteByte(msg, (byte) (ent->v.animtime * 100));
		}

		if (pflags & PF_RENDER)
		{
			MSG_WriteByte(msg, ent->v.rendermode);
			MSG_WriteByte(msg, ent->v.renderamt);
			MSG_WriteByte(msg, ent->v.rendercolor[0]);
			MSG_WriteByte(msg, ent->v.rendercolor[1]);
			MSG_WriteByte(msg, ent->v.rendercolor[2]);
			MSG_WriteByte(msg, ent->v.renderfx);
		}
		if (pflags & PF_FRAMERATE)
			MSG_WriteChar(msg, ent->v.framerate * 16);

		if (pflags & PF_BODY)
			MSG_WriteByte(msg, ent->v.body);

		if (pflags & PF_CONTROLLER1)
			MSG_WriteByte(msg, ent->v.controller[0]);
		if (pflags & PF_CONTROLLER2)
			MSG_WriteByte(msg, ent->v.controller[1]);
		if (pflags & PF_CONTROLLER3)
			MSG_WriteByte(msg, ent->v.controller[2]);
		if (pflags & PF_CONTROLLER4)
			MSG_WriteByte(msg, ent->v.controller[3]);

		if (pflags & PF_BLENDING1)
			MSG_WriteByte(msg, ent->v.blending[0]);
		if (pflags & PF_BLENDING2)
			MSG_WriteByte(msg, ent->v.blending[1]);

		if (pflags & PF_BASEVELOCITY)
		{
			MSG_WriteShort(msg, ent->v.clbasevelocity[0]);
			MSG_WriteShort(msg, ent->v.clbasevelocity[1]);
			MSG_WriteShort(msg, ent->v.clbasevelocity[2]);
		}

		if (pflags & PF_FRICTION)
			MSG_WriteShort(msg, ent->v.friction);

		if (pflags & PF_PING)
			MSG_WriteShort(msg, SV_CalcPing(cl));
	}
}

/*
==================
SV_AddToFullPack

Return 1 if the entity state has been filled in for the ent and the entity will be propagated to the client, 0 otherwise

state is the server maintained copy of the state info that is transmitted to the client
a MOD could alter values copied into state to send the "host" a different look for a particular entity update, etc.
e and ent are the entity that is being added to the update, if 1 is returned
host is the player's edict of the player whom we are sending the update to
player is 1 if the ent/e is a player and 0 otherwise
pSet is either the PAS or PVS that we previous set up.  We can use it to ask the engine to filter the entity against the PAS or PVS.
we could also use the pas/ pvs that we set in SetupVisibility, if we wanted to.  Caching the value is valid in that case, but still only for the current frame
==================
*/
void SV_AddToFullPack( full_packet_entities_t* pack, int e, byte* pSet )
{
	int					i;
	edict_t*			ent;
	entity_state_t*		state;

	ent = &sv.edicts[e];

	// don't send if flagged for NODRAW and it's not the host getting the message
	if (ent->v.effects == EF_NODRAW)
		return;

	// Ignore ents without valid / visible models
	if (!ent->v.modelindex || !(pr_strings + ent->v.model))
		return;

	// If pSet is NULL, then the test will always succeed and the entity will be added to the update
	if (pSet)
	{
		if (ent->num_leafs < 0)
			if (!CM_HeadnodeVisible(&sv.worldmodel->nodes[ent->leafnums[0]], pSet))
				return;

		// ignore if not touching a PV leaf
		for (i = 0; i < ent->num_leafs; i++)
			if (pSet[ent->leafnums[i] >> 3] & (1 << (ent->leafnums[i] & 7)))
				break;
		if (i == ent->num_leafs)
			return;		// not visible
	}

	if (pack->num_entities >= MAX_PACKET_ENTITIES)
	{
		Con_DPrintf("Too many entities in visible packet list.\n");
		return;
	}
	// Assign index so we can track this entity from frame to frame and
	//  delta from it.
	pack->entities[pack->num_entities].number = e;
	state = &pack->entities[pack->num_entities];
	pack->num_entities++;

	state->flags = 0;
	state->entityType = ENTITY_NORMAL;

	// Flag custom entities.
	if (ent->v.flags & FL_CUSTOMENTITY)
	{
		state->entityType = ENTITY_BEAM;
	}
		
	VectorCopy(ent->v.origin, state->origin);
	VectorCopy(ent->v.angles, state->angles);
	VectorCopy(ent->v.mins, state->mins);
	VectorCopy(ent->v.maxs, state->maxs);
	VectorCopy(ent->v.velocity, state->velocity);

	state->modelindex = ent->v.modelindex;
	state->frame = ent->v.frame;
	state->skin = ent->v.skin;
	state->colormap = ent->v.colormap;
	state->solid = ent->v.solid;
	state->effects = ent->v.effects;
	state->scale = ent->v.scale;
	state->movetype = ent->v.movetype;
	state->animtime = ent->v.animtime;
	state->sequence = ent->v.sequence;
	state->framerate = ent->v.framerate;
	state->body = ent->v.body;

	for (i = 0; i < 4; i++)
	{
		state->controller[i] = ent->v.controller[i];
	}

	for (i = 0; i < 2; i++)
	{
		state->blending[i] = ent->v.blending[i];
	}

	state->rendermode = ent->v.rendermode;
	state->renderamt = ent->v.renderamt;
	state->renderfx = ent->v.renderfx;
	state->rendercolor.r = ent->v.rendercolor[0];
	state->rendercolor.g = ent->v.rendercolor[1];
	state->rendercolor.b = ent->v.rendercolor[2];
}

/*
==================
SV_WriteEntitiesToClient

==================
*/
extern byte* SV_FatPVS(vec3_t org); //FF: remove me after refactoring
									// TODO: Implement ^
void SV_WriteEntitiesToClient( client_t* client, sizebuf_t* msg )
{
	int		i;
	byte*	pvs;
	vec3_t	org;
	packet_entities_t*	pack;
	edict_t* clent;
	client_frame_t*		frame;
	entity_state_t*		state;
	full_packet_entities_t fullpack;

	// this is the frame we are creating
	frame = &client->frames[client->netchan.incoming_sequence & UPDATE_MASK];

	// find the client's PVS
	clent = client->edict;
	VectorAdd(clent->v.origin, clent->v.view_ofs, org);
	pvs = SV_FatPVS(org);

	// send over the players in the PVS
	SV_WritePlayersToClient(client, pvs, msg);

	// put other visible entities into either a packet_entities or a nails message
	pack = &frame->entities;
	pack->num_entities = 0;

	if (pack->entities)
		free(pack->entities);

	pack->entities = NULL;

	fullpack.num_entities = 0;
	for (i = svs.maxclients + 1; i < sv.num_edicts; i++)
	{
		SV_AddToFullPack(&fullpack, i, pvs);
	}

	pack->num_entities = i = fullpack.num_entities;

	if (i == 0)
		i = 1;

	state = (entity_state_t *)malloc(sizeof(entity_state_t) * i);
	pack->entities = state;
	if (!state)
		Sys_Error("Failed to allocate space for %i packet entities\n", i);

	if (pack->num_entities)
		memcpy(pack->entities, fullpack.entities, sizeof(entity_state_t) * pack->num_entities); // TODO: There might be a problem
	else
		memset(pack->entities, 0, sizeof(entity_state_t));

	SV_EmitPacketEntities(client, pack, msg);
}

/*
=======================
SV_CleanupEnts

Send datagram to the client who need one
=======================
*/
void SV_CleanupEnts( void )
{
	int			i;
	edict_t*	ent;

	for (i = 0, ent = sv.edicts; i < sv.max_edicts; i++, ent++)
	{
		ent->v.effects &= ~EF_MUZZLEFLASH;
		ent->v.effects &= ~EF_NOINTERP;
	}
}

/*
==================
SV_WriteClientdataToMessage

==================
*/
void SV_WriteClientdataToMessage( client_t* client, sizebuf_t* msg )
{
	int		i, flags;
	edict_t* ent;

	flags = 0;

	ent = client->edict;

	// send the chokecount for r_netgraph
	if (client->chokecount)
	{
		MSG_WriteByte(msg, svc_chokecount);
		MSG_WriteByte(msg, client->chokecount);
		client->chokecount = 0;
	}

	SV_SetIdealPitch();

	// a fixangle might get lost in a dropped packet.  Oh well.
	if (ent->v.fixangle)
	{
		if (ent->v.fixangle == 2)
		{
			MSG_WriteByte(msg, svc_addangle);
			MSG_WriteHiresAngle(msg, ent->v.avelocity[1]);
			ent->v.avelocity[1] = 0;
		}
		else
		{
			MSG_WriteByte(msg, svc_setangle);
			for (i = 0; i < 3; i++)
				MSG_WriteHiresAngle(msg, ent->v.angles[i]);
		}
		ent->v.fixangle = 0;
	}

	if (ent->v.view_ofs[2] != 22)
		flags |= SU_VIEWHEIGHT;
	if (ent->v.idealpitch != 0.0)
		flags |= SU_IDEALPITCH;
	if (ent->v.weapons)
		flags |= SU_WEAPONS;
	if (ent->v.flags & FL_ONGROUND)
		flags |= SU_ONGROUND;
	if (ent->v.waterlevel >= 2)
		flags |= SU_INWATER;
	if (ent->v.waterlevel >= 3)
		flags |= SU_FULLYINWATER;
	if (ent->v.viewmodel)
		flags |= SU_ITEMS;

	for (i = 0; i < 3; i++)
	{
		if (ent->v.punchangle[i])
		{
			flags |= (SU_PUNCH1 << i);
		}
		if (ent->v.velocity[i])
		{
			flags |= (SU_VELOCITY1 << i);
		}
	}

	MSG_WriteByte(msg, svc_clientdata);
	MSG_WriteShort(msg, flags);
	if (flags & SU_VIEWHEIGHT)
		MSG_WriteChar(msg, ent->v.view_ofs[2]);
	if (flags & SU_IDEALPITCH)
		MSG_WriteChar(msg, ent->v.idealpitch);
	for (i = 0; i < 3; ++i)
	{
		if (flags & (SU_PUNCH1 << i))
			MSG_WriteHiresAngle(msg, ent->v.punchangle[i]);
		if (flags & (SU_VELOCITY1 << i))
			MSG_WriteChar(msg, ent->v.velocity[i]);
	}
	if (flags & SU_WEAPONS)
		MSG_WriteLong(msg, ent->v.weapons);
	if (flags & SU_ITEMS)
		MSG_WriteShort(msg, SV_ModelIndex(pr_strings + ent->v.viewmodel));
	MSG_WriteShort(msg, ent->v.health);
}

/*
=======================
SV_SendClientDatagram

Send datagram to the client who need one
=======================
*/
qboolean SV_SendClientDatagram( client_t* client )
{
	sizebuf_t	msg;
	byte		data[MAX_DATAGRAM];

	msg.maxsize = MAX_DATAGRAM;
	msg.data = data;
	msg.cursize = 0;
	msg.allowoverflow = TRUE;
	msg.overflowed = FALSE;

	MSG_WriteByte(&msg, svc_time);
	MSG_WriteFloat(&msg, sv.time);

	// add the client specific data to the datagram
	SV_WriteClientdataToMessage(client, &msg);

	// send over all the objects that are in the PVS
	// this will include clients, a packetentities, and
	// possibly a nails update
	SV_WriteEntitiesToClient(client, &msg);

	// Did we overflow the message buffer?  If so, just ignore it.
	if (client->datagram.overflowed)
	{
		Con_Printf("WARNING: datagram overflowed for %s\n", client->name);
	}
	else
	{
		// Otherwise, copy the client->datagram into the message stream, too.
		SZ_Write(&msg, client->datagram.data, client->datagram.cursize);
	}

	// Clear up the client datagram because we've just copied it.
	SZ_Clear(&client->datagram);

	if (!scr_graphmean.value)
	{
		if (nDatagramBytesSent < msg.cursize)
			nDatagramBytesSent = msg.cursize;
	}
	else
	{
		++nDatagrams;
		nDatagramBytesSent += msg.cursize;
	}

	// send deltas over reliable stream
	if (msg.overflowed)
	{
		Con_Printf("WARNING: msg overflowed for %s\n", client->name);
		SZ_Clear(&msg);
	}
	
	// Send the datagram
	Netchan_Transmit(&client->netchan, msg.cursize, data);

	return TRUE;
}

/*
=======================
SV_UpdateToReliableMessages

=======================
*/
void SV_UpdateToReliableMessages( void )
{
	int			i, j;
	client_t*	client;
	UserMsg*	pMsg;

// check for changes to be sent over the reliable streams
	for (i = 0, host_client = svs.clients; i < svs.maxclients; i++, host_client++)
	{
		if (!host_client->edict)
			continue;

		if (host_client->fakeclient) //this is a client controlled entirely by the game .dll, let it handle the stuff
			continue;

		if (!sv_gpNewUserMsgs) //no new UserMsgs were registered
			continue;

		if (host_client->active || host_client->connected)
		{
			SV_SendUserReg(&host_client->netchan.message);
		}
	}

	// Link new user messages to the global messages chain
	if (sv_gpNewUserMsgs)
	{
		pMsg = sv_gpUserMsgs;
		if (pMsg != NULL)
		{
			while (pMsg->next)
			{
				pMsg = pMsg->next;
			}
			pMsg->next = sv_gpNewUserMsgs;
		}
		else
		{
			sv_gpUserMsgs = sv_gpNewUserMsgs;
		}
		sv_gpNewUserMsgs = NULL; //nullify it so we don't send "svc_newusermsg" each frame
	}

	// Clear the server datagram if it overflowed.
	if (sv.datagram.overflowed)
	{
		Con_DPrintf("sv.datagram overflowed!\n");
		SZ_Clear(&sv.datagram);
	}

	// Now send the reliable and server datagrams to all clients.
	for (j = 0, client = svs.clients; j < svs.maxclients; j++, client++)
	{
		if (!client->active || client->fakeclient)
			continue;

		SZ_Write(&client->netchan.message, sv.reliable_datagram.data, sv.reliable_datagram.cursize);
		SZ_Write(&client->datagram, sv.datagram.data, sv.datagram.cursize);
	}

	// Now clear the reliable and datagram buffers.
	SZ_Clear(&sv.reliable_datagram);
	SZ_Clear(&sv.datagram);
}

/*
=======================
SV_SendClientMessages
=======================
*/
void SV_SendClientMessages(void)
{
	int			i;

	// update frags, names, etc
	SV_UpdateToReliableMessages();

#ifndef SWDS
	nReliableBytesSent = 0;
	nDatagramBytesSent = 0;
	nReliables = 0;
	nDatagrams = 0;
	bUnreliableOverflow = FALSE;
#endif //SWDS

	if (g_LastScreenUpdateTime <= sv.time)
	{
		bShouldUpdatePing = TRUE;
		g_LastScreenUpdateTime = sv.time + 1;
	}

	for (i = 0, host_client = svs.clients; i < svs.maxclients; i++, host_client++)
	{
		if ((host_client->active || host_client->connected || host_client->spawned) && !host_client->fakeclient)
		{
			// Clear the client datagram if it overflowed, as well as disconnect him/her.
			if (host_client->netchan.message.overflowed)
			{
				SZ_Clear(&host_client->netchan.message);
				SZ_Clear(&host_client->datagram);

				SV_BroadcastPrintf("%s overflowed\n", host_client->name);
				Con_Printf("WARNING: reliable overflow for %s\n", host_client->name);

				SV_DropClient(host_client, FALSE);

				// we must notify them about the disconnect
				host_client->send_message = TRUE;

				host_client->netchan.cleartime = 0;	// don't choke this message
			}

			// only send messages if the client has sent one
			// and the bandwidth is not choked
			if (!host_client->send_message)
				continue;

			host_client->send_message = FALSE; // try putting this after choke?

			if (!Netchan_CanPacket(&host_client->netchan))
			{
				host_client->chokecount++;
				continue;		// bandwidth choke
			}

			// Append the unreliable data (player updates and packet entities)
			if (host_client->active)
			{
				SV_SendClientDatagram(host_client);
			}
			else
			{
				// Connected, but inactive, just send reliable, sequenced info.
				Netchan_Transmit(&host_client->netchan, 0, NULL);
			}
		}
	}

	// TODO: Implement (this is related to SCR_NetGraph)
	/*sub_DFC0FA0(nReliableBytesSent, nReliables, FALSE);
	sub_DFC0FA0(nDatagramBytesSent, !bUnreliableOverflow ? nDatagrams : (MAX_MSGLEN + 1), TRUE);*/
	bShouldUpdatePing = FALSE;

	// Allow game .dll to run code, including unsetting EF_MUZZLEFLASH and EF_NOINTERP on effects fields
	SV_CleanupEnts();
}
