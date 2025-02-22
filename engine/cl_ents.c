// cl_ents.c -- entity parsing and management

#include "quakedef.h"
#include "pr_cmds.h"
#include "cl_tent.h"
#include "pmove.h"
#include "customentity.h"

int cl_playerindex; // player index

int			cl_numvisedicts, cl_oldnumvisedicts, cl_numbeamentities;
cl_entity_t* cl_visedicts, * cl_newvisedicts;
cl_entity_t	cl_visedicts_list[2][MAX_VISEDICTS];

int packet_flags[166];

/*
=========================================================================

PACKET ENTITY PARSING / LINKING

=========================================================================
*/

/*
==================
CL_ReadDeltaFlags

Parse delta flags
==================
*/
int CL_ReadDeltaFlags( int* flags, int* bboxflags )
{
	int			bits, bboxbits;
	int			num;

	bboxbits = 0;

	bits = MSG_ReadByte();

	if (bits & U_MOREBITS)
		bits |= MSG_ReadByte() << 8;

	if (bits & U_EVENMOREBITS)
		bits |= MSG_ReadByte() << 16;

	if (bits & U_YETMOREBITS)
		bits |= MSG_ReadByte() << 24;

	if (bits < 0)
		bboxbits = MSG_ReadByte();

	if (bits & U_LONGENTITY)
		num = MSG_ReadShort();
	else
		num = MSG_ReadByte();

	*flags = bits;
	*bboxflags = bboxbits;

	return num;
}

/*
==================
CL_ParseCustomEntity

Can go from either a baseline or a previous packet_entity
==================
*/
int	custombitcounts[32];	/// just for protocol profiling
void CL_ParseCustomEntity( entity_state_t* from, entity_state_t* to, int bits, int custombits, int number )
{
	int			i;

	// set everything to the state we are delta'ing from
	*to = *from;

	to->entityType = ENTITY_BEAM;
	to->number = number;

	// count the bits for net profiling
	for (i = 0; i < 32; i++)
	{
		if (bits & (1 << i))
			custombitcounts[i]++;
	}

	to->flags = bits;

	if (bits & U_BEAM_STARTX)
		to->origin[0] = MSG_ReadCoord();

	if (bits & U_BEAM_STARTY)
		to->origin[1] = MSG_ReadCoord();

	if (bits & U_BEAM_STARTZ)
		to->origin[2] = MSG_ReadCoord();

	if (bits & U_BEAM_ENDX)
		to->angles[0] = MSG_ReadCoord();

	if (bits & U_BEAM_ENDY)
		to->angles[1] = MSG_ReadCoord();

	if (bits & U_BEAM_ENDZ)
		to->angles[2] = MSG_ReadCoord();

	if (bits & U_BEAM_ENTS)
	{
		to->sequence = MSG_ReadShort();
		to->skin = MSG_ReadShort();
	}

	if (bits & U_BEAM_TYPE)
		to->rendermode = MSG_ReadByte();

	if (bits & U_BEAM_MODEL)
	{
		to->modelindex = MSG_ReadShort();
		if (to->modelindex >= MAX_MODELS)
			Host_Error("CL_ParseCustomEntity: bad model number");
	}

	if (bits & U_BEAM_WIDTH)
		to->scale = MSG_ReadByte() * 0.1;

	if (bits & U_BEAM_NOISE)
		to->body = MSG_ReadByte();

	if (bits & U_BEAM_RENDER)
	{
		to->rendercolor.r = MSG_ReadByte();
		to->rendercolor.g = MSG_ReadByte();
		to->rendercolor.b = MSG_ReadByte();
		to->renderfx = MSG_ReadByte();
	}

	if (bits & U_BEAM_BRIGHTNESS)
		to->renderamt = MSG_ReadByte();

	if (bits & U_BEAM_FRAME)
		to->frame = MSG_ReadByte();

	if (bits & U_BEAM_SCROLL)
		to->animtime = MSG_ReadByte() * 0.1;
}

/*
==================
CL_ParseDelta

Can go from either a baseline or a previous packet_entity
==================
*/
int	bitcounts[32];	/// just for protocol profiling
void CL_ParseDelta( entity_state_t* from, entity_state_t* to, int bits, int bboxbits, int number )
{
	int			i;

	int v5; // ecx
	int Byte; // ebp
	double v8; // st7
	float v11; // [esp+10h] [ebp-Ch]
	float Word; // [esp+10h] [ebp-Ch]
	float v13; // [esp+10h] [ebp-Ch]
	float v14; // [esp+10h] [ebp-Ch]
	float v15; // [esp+10h] [ebp-Ch]
	float v16; // [esp+10h] [ebp-Ch]
	float v17; // [esp+10h] [ebp-Ch]
	float v18; // [esp+18h] [ebp-4h]
	float v19; // [esp+18h] [ebp-4h]

	// set everything to the state we are delta'ing from
	*to = *from;

	to->entityType = ENTITY_NORMAL;
	to->number = number;

	// count the bits for net profiling
	v5 = 0;
	do
	{
		if (((1 << v5) & bits) != 0)
			++bitcounts[v5];
		++v5;
	} while (v5 < 32);

	for (i = 0; i < 8; ++i)
	{
		if (((1 << i) & bboxbits) != 0)
			++bitcounts[i + 32];
	}

	to->flags = bits;

	if ((bits & 0x800) != 0)
		to->modelindex = MSG_ReadShort();

	if ((bits & 0x80) != 0)
	{
		v11 = (double)MSG_ReadWord() / 256.0;
		to->frame = v11;
	}

	if ((bits & 0x40) != 0)
		to->movetype = MSG_ReadByte();

	if ((bits & 0x40000) != 0)
		to->colormap = MSG_ReadByte();

	if ((bits & 0x80000) != 0)
	{
		to->skin = MSG_ReadShort();
		to->solid = MSG_ReadByte();
	}

	if ((bits & 0x40000000) != 0)
	{
		Word = (float)MSG_ReadWord();
		to->scale = Word * 0.00390625;
	}

	if ((bits & 0x4000) != 0)
		to->effects = MSG_ReadByte();
	if ((bits & 2) != 0)
		to->origin[0] = MSG_ReadCoord();
	if ((bits & 0x200) != 0)
		to->angles[0] = MSG_ReadHiresAngle();
	if ((bits & 4) != 0)
		to->origin[1] = MSG_ReadCoord();
	if ((bits & 0x20) != 0)
		to->angles[1] = MSG_ReadHiresAngle();
	if ((bits & 8) != 0)
		to->origin[2] = MSG_ReadCoord();
	if ((bits & 0x400) != 0)
		to->angles[2] = MSG_ReadHiresAngle();

	if ((bits & 0x1000) != 0)
	{
		Byte = MSG_ReadByte();
		v13 = (float)(int)(__int64)(cl.time * 100.0);
		v8 = v13;
		v14 = (float)(unsigned __int8)(__int64)v13;
		v15 = (v8 - v14) / 100.0;
		v18 = v15;
		v16 = (float)MSG_ReadByte();
		v19 = v16 / 100.0 + v18;

//		if (v10) TODO: Potential uninitialized memory, solve
		{
			do
				v19 = v19 - 2.56;
			while (v19 > cl.time + 0.1);
		}

		to->sequence = Byte;
		to->animtime = v19;
	}
	if ((bits & 0x10000) != 0)
	{
		v17 = (double)MSG_ReadChar() / 16.0;
		to->framerate = v17;
	}
	if ((bits & 0x1000000) != 0)
		to->controller[0] = MSG_ReadByte();
	if ((bits & 0x2000000) != 0)
		to->controller[1] = MSG_ReadByte();
	if ((bits & 0x4000000) != 0)
		to->controller[2] = MSG_ReadByte();
	if ((bits & 0x8000000) != 0)
		to->controller[3] = MSG_ReadByte();
	if ((bits & 0x10000000) != 0)
		to->blending[0] = MSG_ReadByte();
	if ((bits & 0x20000000) != 0)
		to->blending[1] = MSG_ReadByte();
	if ((bits & 0x400000) != 0)
		to->body = MSG_ReadByte();

	if ((bits & 0x100000) != 0)
	{
		to->rendermode = MSG_ReadByte();
		to->renderamt = MSG_ReadByte();
		to->rendercolor.r = MSG_ReadByte();
		to->rendercolor.g = MSG_ReadByte();
		to->rendercolor.b = MSG_ReadByte();
		to->renderfx = MSG_ReadByte();
	}

	if ((bboxbits & 1) != 0)
		to->mins[0] = MSG_ReadCoord();
	if ((bboxbits & 2) != 0)
		to->mins[1] = MSG_ReadCoord();
	if ((bboxbits & 4) != 0)
		to->mins[2] = MSG_ReadCoord();
	if ((bboxbits & 8) != 0)
		to->maxs[0] = MSG_ReadCoord();
	if ((bboxbits & 16) != 0)
		to->maxs[1] = MSG_ReadCoord();
	if ((bboxbits & 32) != 0)
		to->maxs[2] = MSG_ReadCoord();
}

/*
=================
CL_FlushEntityPacket
=================
*/
void CL_FlushEntityPacket( void )
{
	int			num;
	entity_state_t	olde, newe;
	int			flags, bboxflags;
	unsigned int peeklong;

	memset(&olde, 0, sizeof(olde));

	cl.validsequence = 0;		// can't render a frame
	cl.frames[cls.netchan.incoming_sequence & UPDATE_MASK].invalid = TRUE;
	
	// read it all, but ignore it
	while (1)
	{
		peeklong = *(unsigned int*)&net_message.data[msg_readcount];
		if (peeklong)
			num = CL_ReadDeltaFlags(&flags, &bboxflags);
		else
			num = MSG_ReadLong();

		if (msg_badread)
		{	// something didn't parse right...
			Host_EndGame("msg_badread in packetentities\n");
		}

		if (!num)
			break;	// done

		if (flags & U_REMOVE)
			continue;

		if (flags & U_CUSTOM)
		{
			CL_ParseCustomEntity(&olde, &newe, flags, bboxflags, num);
		}
		else
		{
			CL_ParseDelta(&olde, &newe, flags, bboxflags, num);
		}
	}
}

/*
==================
CL_ParsePacketEntities

An svc_packetentities has just been parsed, deal with the
rest of the data stream.
==================
*/
void CL_ParsePacketEntities( qboolean delta )
{
	int			oldpacket, newpacket;
	packet_entities_t* oldp, * newp, dummy;
	int			oldindex, newindex;
	int			num, newnum, oldnum;
	int			flags, bboxflags;
	qboolean	full;
	byte		from;
	unsigned int peeklong;
	int			newp_number; // # of entities in new packet.

	if (cls.signon == 2)
	{
		// We are done with signon sequence.
		cls.signon = SIGNONS;

		// Clear loading plaque.
		CL_SignonReply();
	}

	// Frame # of packet we are deltaing to
	newpacket = cls.netchan.incoming_sequence & UPDATE_MASK;

	// Packed entities for current frame
	newp = &cl.frames[newpacket].packet_entities;
	cl.frames[newpacket].invalid = FALSE;

	// Retrieve size of packet.
	newp_number = MSG_ReadShort();
	if (!newp_number)
		newp_number = 1;

	if (newp->entities)
		free(newp->entities);

	newp->entities = (entity_state_t*)malloc(sizeof(entity_state_t) * newp_number);
	if (!newp->entities)
		Sys_Error("CL_ParsePacketEntities:  Failed to allocate space for %i entities.\n", newp_number);

	memset(newp->entities, 0, sizeof(entity_state_t) * newp_number);

	newp->num_entities = newp_number;

	if (delta)
	{
		from = MSG_ReadByte();

		oldpacket = cl.frames[newpacket].delta_sequence;

		if ((from & UPDATE_MASK) != (oldpacket & UPDATE_MASK))
			Con_DPrintf("WARNING: from mismatch\n");
	}
	else
	{
		cls.demowaiting = FALSE;
		oldpacket = -1;
	}

	full = FALSE;
	if (oldpacket != -1)
	{
		if (cls.netchan.outgoing_sequence - oldpacket >= UPDATE_BACKUP - 1)
		{	// we can't use this, it is too old
			CL_FlushEntityPacket();
			if (newp->entities)
				free(newp->entities);
			dummy.num_entities = 0;
			dummy.entities = NULL;
			return;
		}
		cl.validsequence = cls.netchan.incoming_sequence;
		oldp = &cl.frames[oldpacket & UPDATE_MASK].packet_entities;
	}
	else
	{
		// Use a dummy packet entity as default. If the old packet is valid, 
		// it will be overwritten
		oldp = &dummy;
		dummy.num_entities = 0;
		dummy.entities = NULL;
		cl.validsequence = cls.netchan.incoming_sequence;
		full = TRUE;
	}

	oldindex = 0;
	newindex = 0;
	newp->num_entities = 0;

	while (1)
	{
		peeklong = *(unsigned int*)&net_message.data[msg_readcount];
		if (peeklong)
			num = CL_ReadDeltaFlags(&flags, &bboxflags);
		else
			num = MSG_ReadLong();

		if (msg_badread)
		{	// something didn't parse right...
			Host_EndGame("msg_badread in packetentities\n");
		}

		if (!num)
		{
			while (oldindex < oldp->num_entities)
			{	// copy all the rest of the entities from the old packet
//				Con_Printf("copy %i\n", oldp->entities[oldindex].number);
				if (newindex >= MAX_PACKET_ENTITIES)
					Host_EndGame("CL_ParsePacketEntities: newindex == MAX_PACKET_ENTITIES");
				newp->entities[newindex] = oldp->entities[oldindex];
				newindex++;
				oldindex++;
			}
			break;
		}
		newnum = num;
		oldnum = oldindex >= oldp->num_entities ? 9999 : oldp->entities[oldindex].number;

		while (newnum > oldnum)
		{
			if (full)
			{
				Con_Printf("WARNING: oldcopy on full update");
				CL_FlushEntityPacket();

				if (newp->entities)
					free(newp->entities);
				dummy.num_entities = 0;
				dummy.entities = NULL;
				return;
			}

//			Con_Printf("copy %i\n", oldnum);
			// copy one of the old entities over to the new packet unchanged
			if (newindex >= MAX_PACKET_ENTITIES)
				Host_EndGame("CL_ParsePacketEntities: newindex == MAX_PACKET_ENTITIES");
			newp->entities[newindex] = oldp->entities[oldindex];
			newindex++;
			oldindex++;
			oldnum = oldindex >= oldp->num_entities ? 9999 : oldp->entities[oldindex].number;
		}

		if (newnum < oldnum)
		{	// new from baseline
//Con_Printf("baseline %i\n", newnum);
			if (flags & U_REMOVE)
			{
				if (full)
				{
					cl.validsequence = 0;
					Con_Printf("WARNING: U_REMOVE on full update\n");
					CL_FlushEntityPacket();
					if (newp->entities)
						free(newp->entities);
					newp->num_entities = 0;
					newp->entities = NULL;
					return;
				}
				continue;
			}
			if (newindex >= MAX_PACKET_ENTITIES)
				Host_EndGame("CL_ParsePacketEntities: newindex == MAX_PACKET_ENTITIES");

			CL_EntityNum(newnum);

			if (flags & U_CUSTOM)
			{
				CL_ParseCustomEntity(&cl_entities[newnum].baseline, &newp->entities[newindex], flags, bboxflags, newnum);
			}
			else
			{
				CL_ParseDelta(&cl_entities[newnum].baseline, &newp->entities[newindex], flags, bboxflags, newnum);
			}
			newindex++;
			continue;
		}

		if (newnum == oldnum)
		{	// delta from previous
			if (full)
			{
				cl.validsequence = 0;
				Con_Printf("WARNING: delta on full update");
			}
			if (flags & U_REMOVE)
			{
				oldindex++;
				R_KillDeadBeams(oldnum);
				continue;
			}
//Con_Printf("delta %i\n",newnum);
			if (flags & U_CUSTOM)
			{
				CL_ParseCustomEntity(&oldp->entities[oldindex], &newp->entities[newindex], flags, bboxflags, newnum);
			}
			else
			{
				CL_ParseDelta(&oldp->entities[oldindex], &newp->entities[newindex], flags, bboxflags, newnum);
			}
			newindex++;
			oldindex++;
		}
	}

	newp->num_entities = newindex;
}

// TODO: Implement

/*
===============
CL_PrintEntity

===============
*/
void CL_PrintEntity( cl_entity_t* ent )
{
	Con_DPrintf("----------------------------\n");
	Con_DPrintf("T %.2f", cl.time);
	Con_DPrintf(":Gap %.2f\n", cl.time - ent->animtime);
	Con_DPrintf("#%i", ent->index);

	if (ent->model)
		Con_DPrintf(":%s\n", ent->model->name);
	else
		Con_DPrintf(":?\n");

	Con_DPrintf("AT %4.2f ST %4.2f S %i F %4.1f\n", ent->animtime, ent->sequencetime, ent->sequence, ent->frame);
	Con_DPrintf("PA %4.2f PS %i FR %.1f\n", ent->prevanimtime, ent->prevsequence, ent->framerate);
	Con_DPrintf("C0 %i:%i BL %i:%i\n", ent->controller[0], ent->prevcontroller[0], ent->blending[0], ent->prevblending[0]);
	Con_DPrintf("O : %.0f %.0f %.0f\n", ent->origin[0], ent->origin[1], ent->origin[2]);
	Con_DPrintf("PO: %.0f %.0f %.0f\n", ent->prevorigin[0], ent->prevorigin[1], ent->prevorigin[2]);
	Con_DPrintf("RM:  %i RA: %i RX: %i PF %4.2f\n", ent->rendermode, ent->renderamt, ent->renderfx, ent->prevframe);
	Con_DPrintf("MT: %i:", ent->movetype);

	if (ent->effects & EF_NOINTERP)
		Con_DPrintf("NoInterp ");
	if (ent->effects & EF_NODRAW)
		Con_DPrintf("NoDraw ");
	if (ent->effects & EF_LIGHT)
		Con_DPrintf("Light ");
	if (ent->effects & EF_BRIGHTFIELD)
		Con_DPrintf("BFLD ");
	if (ent->effects & EF_MUZZLEFLASH)
		Con_DPrintf("MUZ ");
	if (ent->effects & EF_BRIGHTLIGHT)
		Con_DPrintf("BLT ");
	if (ent->effects & EF_DIMLIGHT)
		Con_DPrintf("Dim ");
	if (ent->effects & EF_INVLIGHT)
		Con_DPrintf("Inv ");

	if (ent->resetlatched)
		Con_DPrintf("F ");

	Con_DPrintf("\n");
}

/*
==================
CL_ProcessEntityUpdate

Update entity data after new frame
==================
*/
void CL_ProcessEntityUpdate( cl_entity_t* ent, entity_state_t* state, qboolean sync )
{
	ent->index = state->number;

	ent->model = cl.model_precache[state->modelindex];
	if (ent->model && sync)
	{
		if (ent->model->synctype == ST_RAND)
			ent->syncbase = RandomFloat(0.0, 1.0);
		else
			ent->syncbase = 0.0;
	}

	ent->colormap = (byte*)vid.colormap;
	ent->scoreboard = NULL;
	ent->effects = state->effects;
	ent->skin = state->skin;

	ent->frame = state->frame;
	ent->framerate = state->framerate;

	ent->rendermode = state->rendermode;
	ent->renderamt = state->renderamt;
	ent->rendercolor.r = state->rendercolor.r;
	ent->rendercolor.g = state->rendercolor.g;
	ent->rendercolor.b = state->rendercolor.b;
	ent->renderfx = state->renderfx;

	ent->controller[0] = state->controller[0];
	ent->controller[1] = state->controller[1];
	ent->controller[2] = state->controller[2];
	ent->controller[3] = state->controller[3];

	ent->blending[0] = state->blending[0];
	ent->blending[1] = state->blending[1];

	VectorCopy(state->origin, ent->origin);
	VectorCopy(state->angles, ent->angles);

	ent->scale = state->scale;
	ent->sequence = state->sequence;
	ent->animtime = state->animtime;
	ent->movetype = state->movetype;
	ent->body = state->body;
}

// TODO: Implement

void CL_PlayerFlashlight( void )
{
	cl_entity_t* ent;

	ent = &cl_entities[cl.playernum + 1];

	if (ent->effects & (EF_BRIGHTLIGHT | EF_DIMLIGHT) && cl.worldmodel)
	{
		if (!cl.pLight || cl.pLight->key != 1)
			cl.pLight = CL_AllocDlight(1);

		if (ent->effects & EF_BRIGHTLIGHT)
		{
			cl.pLight->color.b = 250;
			cl.pLight->color.g = 250;
			cl.pLight->color.r = 250;
			cl.pLight->radius = 400;
			VectorCopy(ent->origin, cl.pLight->origin);
			cl.pLight->origin[2] += 16;
		}
		else
		{
			VectorCopy(ent->origin, cl.pLight->origin);
			cl.pLight->origin[2] += cl.viewheight;

			// TODO: Implement
		}
	}
	else
	{
		if (cl.pLight && cl.pLight->key == 1)
		{
			cl.pLight->die = cl.time;
		}
		cl.pLight = NULL;
	}

	// apply muzzle to our weapon too if we have muzzle effect ourselves
	if (cl_entities[cl.viewentity].effects & EF_MUZZLEFLASH)
		cl.viewent.effects |= EF_MUZZLEFLASH;
}

/*
===============
CL_LinkPacketEntities

===============
*/
void CL_LinkPacketEntities( void )
{
	cl_entity_t* ent, * ent2, nullent;
	packet_entities_t* pack;
	entity_state_t* s1, * s2;
	float				f;
	model_t* model;
	vec3_t				old_origin;
	float				autorotate;
	int					pnum;
	int					flags;
	dlight_t* dl;

	pack = &cl.frames[cls.netchan.incoming_sequence & UPDATE_MASK].packet_entities;

	autorotate = anglemod(100 * cl.time);

	f = 0;		// FIXME: no interpolation right now

	for (pnum = 0; pnum < pack->num_entities; pnum++)
	{
		s1 = &pack->entities[pnum];
		s2 = s1;	// FIXME: no interpolation right now

		// if set to invisible, skip
		if (!s1->modelindex)
			continue;

		// create a new entity
		if (cl_numvisedicts == MAX_VISEDICTS)
			break;		// object list is full

		if (s1->effects & EF_NODRAW)
			continue;

		ent = &cl_visedicts[cl_numvisedicts];
		cl_numvisedicts++;

		ent2 = &cl_entities[s1->number];

		memcpy(ent, ent2, sizeof(cl_entity_t));
		memcpy(&nullent, ent2, sizeof(cl_entity_t));

		flags = packet_flags[ent->index >> 3] & (1 << (ent->index & 7));

		CL_ProcessEntityUpdate(ent, s1, TRUE);

		model = ent->model;

		if (s1->entityType != ENTITY_NORMAL)
		{
			ent->movetype = s1->modelindex;
			if (model && model->type != mod_sprite)
				Sys_Error("Bad model on beam");

			ent->prevsequence = ent->sequence;

			VectorCopy(ent->origin, ent->prevorigin);
			VectorCopy(ent->angles, ent->prevangles);

			memcpy(&cl_entities[ent->index], ent, sizeof(cl_entity_t));
			cl_numvisedicts--;
			
			if (cl_numbeamentities >= MAX_BEAMENTS)
			{
				Con_DPrintf("Overflow beam entity list!\n");
				continue;
			}

 			cl_beamentities[cl_numbeamentities] = &cl_entities[ent->index];
			cl_numbeamentities++;
			continue;
		}

		if (ent->animtime != nullent.animtime)
		{
			if (ent->sequence != nullent.sequence)
			{
				ent->sequencetime = ent->animtime;
				ent->prevsequence = nullent.sequence;
				memcpy(ent->prevseqblending, nullent.blending, 2);
			}

			ent->prevanimtime = nullent.animtime;

			memcpy(ent->prevcontroller, nullent.controller, 4);
			memcpy(ent->prevblending, nullent.blending, 2);

			VectorCopy(nullent.origin, ent->prevorigin);
			VectorCopy(nullent.angles, ent->prevangles);
		}
		else
		{
			if (!VectorCompare(ent->origin, nullent.origin))
			{
				ent->lastmove = cl.time + 0.2;
				VectorCopy(ent->origin, ent->prevorigin);
				VectorCopy(ent->angles, ent->prevangles);
			}
		}

		// TODO: Implement (figure out the flag)
		if ((s1->flags & 0x200000) && ent->lastmove < cl.time)
		{
			ent->movetype = MOVETYPE_STEP;
		}
		else
		{
			ent->movetype = MOVETYPE_NONE;
		}

		if (flags)
		{
			if (ent->effects & EF_NOINTERP)
			{
				ent->prevsequence = ent->sequence;
				ent->animtime = cl.time;
				ent->prevanimtime = cl.time;
				VectorCopy(ent->origin, ent->prevorigin);
				VectorCopy(ent->angles, ent->prevangles);
			}
		}
		else
		{
			ent->resetlatched = TRUE;
			ent->prevsequence = ent->sequence;
			ent->animtime = cl.time;
			ent->prevanimtime = cl.time;
			VectorCopy(ent->origin, ent->prevorigin);
			VectorCopy(ent->angles, ent->prevangles);
		}

		if (ent->effects & EF_BRIGHTFIELD)
			R_EntityParticles(ent);
		
		if (ent->index != 1)
		{
			if (ent->effects & EF_BRIGHTLIGHT)
			{
				dl = CL_AllocDlight(ent->index);
				VectorCopy(ent->origin, dl->origin);
				dl->origin[2] += 16;
				dl->color.r = dl->color.g = dl->color.b = 250;
				dl->radius = RandomFloat(400, 431);
				dl->die = cl.time + 0.001;
			}

			if (ent->effects & EF_DIMLIGHT)
			{
				dl = CL_AllocDlight(ent->index);
				VectorCopy(ent->origin, dl->origin);
				dl->color.r = dl->color.g = dl->color.b = 100;
				dl->radius = RandomFloat(200.0, 231.0);
				dl->die = cl.time + 0.001;
			}
		}

		if (ent->effects & EF_LIGHT)
		{
			R_RocketFlare(ent->origin);

			dl = CL_AllocDlight(ent->index);
			VectorCopy(ent->origin, dl->origin);
			dl->color.r = dl->color.g = dl->color.b = 100;
			dl->radius = 200;
			dl->die = cl.time + 0.001;
		}

		if (model)
		{
			if (ent->model->flags & EF_ROCKET)
			{
				R_RocketTrail(old_origin, ent->origin, 0);

				dl = CL_AllocDlight(ent->index);
				VectorCopy(ent->origin, dl->origin);
				dl->color.r = dl->color.g = dl->color.b = 200;
				dl->radius = 200;
				dl->die = cl.time + 0.01;
			}
			else if (ent->model->flags & EF_GRENADE)
				R_RocketTrail(old_origin, ent->origin, 1);
			else if (ent->model->flags & EF_GIB)
				R_RocketTrail(old_origin, ent->origin, 2);
			else if (ent->model->flags & EF_ZOMGIB)
				R_RocketTrail(old_origin, ent->origin, 4);
			else if (ent->model->flags & EF_TRACER)
				R_RocketTrail(old_origin, ent->origin, 3);
			else if (ent->model->flags & EF_TRACER2)
				R_RocketTrail(old_origin, ent->origin, 5);
			else if (ent->model->flags & EF_TRACER3)
				R_RocketTrail(old_origin, ent->origin, 6);
		}

		memcpy(&cl_entities[ent->index], ent, sizeof(cl_entity_t));
	}
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
	int				msec;
	int				flags;
	player_info_t*	info;
	player_state_t* state;
	int				num;
	int				i;
	cl_entity_t*	ent;
	qboolean		spectator = FALSE;
	int				stub1;

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

	stub1 = MSG_ReadLong(); // TODO: Implement (state + 144) =
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
		VectorCopy(state->command.angles, state->viewangles);
	}
	else
	{
		VectorCopy(vec3_origin, state->viewangles);
	}

	for (i = 0; i < 3; i++)
	{
		if (flags & (4 << i))
		{
			state->velocity[i] = MSG_ReadShort();
		}
		else
		{
			state->velocity[i] = 0.0;
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

	ent = &cl_entities[num + 1];

	// TODO: Implement

	VectorCopy(state->origin, ent->origin);
	VectorCopy(state->viewangles, ent->angles);

	// Player pitch is inverted
	ent->angles[PITCH] /= -3.0;


	// TODO: Implement
}

// TODO: Implement

/*
=============
CL_LinkPlayers

Create visible entities in the correct position
for all current players
=============
*/
void CL_LinkPlayers( void )
{
	int				j;
	player_info_t	*info;
	player_state_t	*state;
	player_state_t	exact;
	double			playertime;
	vec_t			lerp_factor;
	vec_t*			start;
	vec_t*			end;
	vec_t*			current;
	frame_t*		frame;
	cl_entity_t*	ent;
	int				msec;
	int				oldphysent;
	int				flags;
	dlight_t*		dl;

	vec_t			previous_x, previous_y, previous_z;

	start = cl.mvelocity[1];
	end = cl.mvelocity[2];

	lerp_factor = cl.frame_lerp;

	for (current = start; current < end; current += 3) {
		previous_x = current[-3];
		previous_y = current[-2];
		previous_z = current[-1];

		current[0] = (previous_x - current[0]) * lerp_factor + current[0];
		current[1] = (previous_y - current[1]) * lerp_factor + current[1];
		current[2] = (previous_z - current[2]) * lerp_factor + current[2];
	}

	playertime = realtime - cls.latency + 0.02;
	if (playertime > realtime)
		playertime = realtime;

	frame = &cl.frames[cl.parsecount & UPDATE_MASK];


	for (j = 0, info = cl.players, state = frame->playerstate; j < MAX_CLIENTS
		; j++, info++, state++)
	{
		if (state->messagenum == cl.parsecount
			&& (cam_thirdperson || cl.viewentity - j != 1 || chase_active.value != 0.0) //if we are in third person or seeing our PoV...
			&& state->modelindex != 0 //make sure that the entity is visible...
			&& (state->effects & EF_NODRAW) == 0 //of course, it also should not have EF_NODRAW
			&& (cl.spectator == FALSE || autocam != 2 || spec_track != j))
		{
			// grab an entity to fill in
			if (cl_numvisedicts >= MAX_VISEDICTS)
				break;		// object list is full

			ent = &cl_visedicts[cl_numvisedicts];
			cl_numvisedicts++;

			memcpy(ent, &cl_entities[j + 1], sizeof(cl_entity_t));
			ent->index = j + 1;

			flags = packet_flags[ent->index >> 3] & (1 << (ent->index & 7));

			// only predict half the move to minimize overruns
			msec = 500 * (playertime - state->state_time);
			if (cl.playernum == j || msec <= 0 || !cl_predict_players.value)
			{
				VectorCopy(state->origin, ent->origin);
//Con_DPrintf ("nopredict\n");
			}
			else
			{
				// predict players movement
				if (msec > 255)
					msec = 255;
				state->command.msec = msec;
//Con_DPrintf ("predict: %i\n", msec);

				oldphysent = pmove.numphysent;
				CL_SetSolidPlayers(j);

				CL_PredictUsercmd(state, &exact, &state->command, FALSE);

				pmove.numphysent = oldphysent;
				VectorCopy(exact.origin, ent->origin);
			}

			VectorCopy(ent->origin, ent->prevorigin);

			ent->colormap = info->translations;
			if (state->modelindex == cl_playerindex)
				ent->scoreboard = info;
			else
				ent->scoreboard = 0;

			if (!flags || (ent->effects & EF_NOINTERP))
			{
				ent->prevsequence = ent->sequence;
				ent->animtime = cl.time;
				ent->prevanimtime = cl.time;
				VectorCopy(ent->origin, ent->prevorigin);
				VectorCopy(ent->angles, ent->prevangles);
			}

			if (cl.viewentity - j != 1)
			{
				if ((ent->effects & EF_BRIGHTLIGHT) != 0)
				{
					dl = CL_AllocDlight(3);
					VectorCopy(ent->origin, dl->origin);
					dl->origin[2] = dl->origin[2] + 16.0;
					dl->color.b = 250;
					dl->color.g = 250;
					dl->color.r = 250;
					dl->radius = RandomFloat(400.0, 431.0);
					// die on next frame
				}
				if ((ent->effects & EF_DIMLIGHT) != 0)
				{
					dl = CL_AllocDlight(3);
					VectorCopy(ent->origin, dl->origin);
					dl->color.b = 100;
					dl->color.g = 100;
					dl->color.r = 100;
					dl->radius = RandomFloat(200.0, 231.0);
					dl->die = cl.time + 0.001;
				}
			}
			if (cl_printplayers.value)
				CL_PrintEntity(ent);
		}
	}

	CL_PlayerFlashlight();
}

//======================================================================

/*
===============
CL_SetSolid

Builds all the pmove physents for the current frame
===============
*/
void CL_SetSolidEntities( void )
{
	int		i;
	frame_t* frame;
	packet_entities_t* pak;
	entity_state_t* state;
	physent_t* pe;
	model_t* model;

	pmove.physents[0].model = cl.worldmodel;
	VectorCopy(vec3_origin, pmove.physents[0].origin);
	pmove.physents[0].info = 0;
	pmove.numphysent = 1;

	frame = &cl.frames[parsecountmod];
	pak = &frame->packet_entities;

	for (i = 0; i < pak->num_entities; i++)
	{
		state = &pak->entities[i];

		if (!state->modelindex)
			continue;

		model = cl.model_precache[state->modelindex];
		if (!model)
			continue;

		if (state->solid == SOLID_TRIGGER)
			continue;
		if (state->solid == SOLID_NOT && state->skin >= CONTENTS_EMPTY)
			continue;

		if (model->hulls[1].firstclipnode || model->type == mod_studio)
		{
			pe = &pmove.physents[pmove.numphysent];
			if (model->type == mod_studio)
			{
				pe->model = NULL;
				pe->studiomodel = cl.model_precache[state->modelindex];
			}
			else
			{
				pe->studiomodel = NULL;
				pe->model = model;
			}

			VectorCopy(state->origin, pe->origin);
			VectorCopy(state->angles, pe->angles);

			VectorCopy(state->mins, pe->mins);
			VectorCopy(state->maxs, pe->maxs);

			pe->solid = state->solid;
			pe->skin = state->skin;
			pe->rendermode = state->rendermode;

			pe->frame = state->frame;
			pe->sequence = state->sequence;
			memcpy(pe->controller, state->controller, 4);
			memcpy(pe->blending, state->blending, 2);

			pmove.numphysent++;
		}
	}
}

// TODO: Implement

/*
==================
CL_SetUpPlayerPrediction

Calculate the new position of players, without other player clipping
We do this to set up real player prediction.
Players are predicted twice, first without clipping other players,
then with clipping against them.
This sets up the first phase.
==================
*/
void CL_SetUpPlayerPrediction( qboolean dopred )
{
	// TODO: Implement
}

/*
=================
CL_SetSolidPlayers

Builds all the pmove physents for the current frame
Note that CL_SetUpPlayerPrediction() must be called first!
pmove must be setup with world and solid entity hulls before calling
(via CL_PredictMove)
=================
*/
void CL_SetSolidPlayers( int playernum )
{
	// TODO: Implement
}

/*
===============
CL_EmitEntities

Builds the visedicts array for cl.time

Made up of: clients, packet_entities, nails, and tents
===============
*/
void CL_EmitEntities( void )
{
	int i, j;
	cl_entity_t* ent, * slot;

	if (cls.state != ca_active)
	{
		cl_oldnumvisedicts = 0;
		cl_numvisedicts = 0;
		cl_numbeamentities = 0;
		memset(packet_flags, 0, sizeof(packet_flags));
		return;
	}

	if (!cl.validsequence)
		return;

	if (cls.demoplayback)
	{
		if (!cls.skipdemomessage)
			return;

		cls.skipdemomessage = FALSE;
	}

	//if (cls.demorecording) TODO: Implement
	//	CL_WriteUpdate();

	cl_oldnumvisedicts = cl_numvisedicts;
	slot = cl_visedicts_list[(cls.netchan.incoming_sequence + 1) & 1];
	cl_newvisedicts = slot;

	memset(packet_flags, 0, sizeof(packet_flags));

	if (cl_numvisedicts > 0)
	{
		cl_newvisedicts = cl_visedicts_list[(cls.netchan.incoming_sequence + 1) & 1];

		for (i = 0; i < cl_numvisedicts; i++, slot++)
		{
			if (slot->index <= 0)
				continue;

			ent = &cl_entities[slot->index];

			packet_flags[slot->index >> 3] |= 1 << (slot->index & 7);

			ent->prevframe = slot->prevframe;

			if (slot->index > cl.maxclients)
				ent->frame = slot->frame;

			for (j = 0; j < 4; j++)
			{
				VectorCopy(slot->origin, ent->attachment[j]);
			}
		}
	}

	cl_numvisedicts = 0;
	cl_numbeamentities = 0;
	cl_visedicts = cl_visedicts_list[cls.netchan.incoming_sequence & 1];

	// Compute last interpolation amount
	cl.frame_lerp = CL_LerpPoint();

	CL_LinkPlayers();
	CL_LinkPacketEntities();

	CL_TempEntUpdate();

	if (cl.spectator)
	{
		//FF: sub_10020190 ~ CL_Disconnect ~ 33 C0 A3 38 15 05 0E A3 34 15 05 0E C3
	}
}