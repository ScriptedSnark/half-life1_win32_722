// cl_ents.c -- entity parsing and management

#include "quakedef.h"
#include "cl_demo.h"
#include "cl_tent.h"
#include "pr_cmds.h"
#include "pmove.h"
#include "customentity.h"

int cl_playerindex; // player index

int			cl_numvisedicts, cl_oldnumvisedicts, cl_numbeamentities;
cl_entity_t* cl_visedicts, * cl_oldvisedicts;
cl_entity_t	cl_visedicts_list[2][MAX_VISEDICTS];

typedef struct
{
	int flags;
	int physflags;
	int index;
	qboolean active;
	vec3_t origin;	// predicted origin
} predicted_player;
predicted_player predicted_players[MAX_CLIENTS];

int packet_flags[166];

/*
=========================================================================

PACKET ENTITY PARSING / LINKING

=========================================================================
*/

/*
==================
CL_ParseDeltaFlags

==================
*/
int CL_ParseDeltaFlags( int* flags, int* bboxflags )
{
	int			i, bits, bboxbits;
	int			num;

	bboxbits = 0;

	bits = MSG_ReadByte();

	if (bits & U_MOREBITS)
	{
		// read in the low order bits
		i = MSG_ReadByte();
		bits |= i << 8;
	}

	if (bits & U_EVENMOREBITS)
	{
		i = MSG_ReadByte();
		bits |= i << 16;
	}

	if (bits & U_YETMOREBITS)
	{
		i = MSG_ReadByte();
		bits |= i << 24;
	}

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
int	bitcounts[32 + 8];	/// just for protocol profiling
void CL_ParseDelta( entity_state_t* from, entity_state_t* to, int bits, int bboxbits, int number )
{
	int			i;

	// set everything to the state we are delta'ing from
	*to = *from;

	to->entityType = ENTITY_NORMAL;
	to->number = number;

	// count the bits for net profiling
	for (i = 0; i < 32; i++)
	{
		if ((1 << i) & bits)
			bitcounts[i]++;
	}

	for (i = 0; i < 8; i++)
	{
		if ((1 << i) & bboxbits)
			bitcounts[32 + i]++;
	}

	to->flags = bits;

	if (bits & U_MODELINDEX)
		to->modelindex = MSG_ReadShort();

	if (bits & U_FRAME)
		to->frame = MSG_ReadWord() / 256.0;

	if (bits & U_MOVETYPE)
		to->movetype = MSG_ReadByte();

	if (bits & U_COLORMAP)
		to->colormap = MSG_ReadByte();

	if (bits & U_CONTENTS)
	{
		to->skin = MSG_ReadShort();
		to->solid = MSG_ReadByte();
	}

	if (bits & U_SCALE)
		to->scale = MSG_ReadWord() / 256.0;

	if (bits & U_EFFECTS)
		to->effects = MSG_ReadByte();

	if (bits & U_ORIGIN1)
		to->origin[0] = MSG_ReadCoord();

	if (bits & U_ANGLE1)
		to->angles[0] = MSG_ReadHiresAngle();

	if (bits & U_ORIGIN2)
		to->origin[1] = MSG_ReadCoord();

	if (bits & U_ANGLE2)
		to->angles[1] = MSG_ReadHiresAngle();

	if (bits & U_ORIGIN3)
		to->origin[2] = MSG_ReadCoord();

	if (bits & U_ANGLE3)
		to->angles[2] = MSG_ReadHiresAngle();

	if (bits & U_SEQUENCE)
	{
		int	sequence;
		float animtime, delta;

		sequence = MSG_ReadByte();

		delta = (int)(cl.time * 100.0) - (byte)(cl.time * 100.0);
		animtime = (MSG_ReadByte() / 100.0) + (delta / 100.0);
		while (animtime > cl.time + 0.1)
			animtime -= 2.56;

		to->sequence = sequence;
		to->animtime = animtime;
	}

	if (bits & U_FRAMERATE)
		to->framerate = MSG_ReadChar() / 16.0;

	if (bits & U_CONTROLLER1)
		to->controller[0] = MSG_ReadByte();

	if (bits & U_CONTROLLER2)
		to->controller[1] = MSG_ReadByte();

	if (bits & U_CONTROLLER3)
		to->controller[2] = MSG_ReadByte();

	if (bits & U_CONTROLLER4)
		to->controller[3] = MSG_ReadByte();

	if (bits & U_BLENDING1)
		to->blending[0] = MSG_ReadByte();

	if (bits & U_BLENDING2)
		to->blending[1] = MSG_ReadByte();

	if (bits & U_BODY)
		to->body = MSG_ReadByte();

	if (bits & U_RENDER)
	{
		to->rendermode = MSG_ReadByte();
		to->renderamt = MSG_ReadByte();
		to->rendercolor.r = MSG_ReadByte();
		to->rendercolor.g = MSG_ReadByte();
		to->rendercolor.b = MSG_ReadByte();
		to->renderfx = MSG_ReadByte();
	}

	if (bboxbits & U_BBOXMINS1)
		to->mins[0] = MSG_ReadCoord();

	if (bboxbits & U_BBOXMINS2)
		to->mins[1] = MSG_ReadCoord();

	if (bboxbits & U_BBOXMINS3)
		to->mins[2] = MSG_ReadCoord();

	if (bboxbits & U_BBOXMAXS1)
		to->maxs[0] = MSG_ReadCoord();

	if (bboxbits & U_BBOXMAXS2)
		to->maxs[1] = MSG_ReadCoord();

	if (bboxbits & U_BBOXMAXS3)
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
			num = CL_ParseDeltaFlags(&flags, &bboxflags);
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
			num = CL_ParseDeltaFlags(&flags, &bboxflags);
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

	memcpy(ent->controller, state->controller, 4);
	memcpy(ent->blending, state->blending, 2);

	VectorCopy(state->origin, ent->origin);
	VectorCopy(state->angles, ent->angles);

	ent->scale = state->scale;
	ent->sequence = state->sequence;
	ent->animtime = state->animtime;
	ent->movetype = state->movetype;
	ent->body = state->body;
}

/*
==================
CL_Particle
==================
*/
void CL_Particle( vec_t* origin, int color, float life, int zpos, int zvel )
{
	particle_t* p;

	if (!free_particles)
		return;

	p = free_particles;
	free_particles = p->next;
	p->next = active_particles;
	active_particles = p;

	p->die = cl.time + life;
	p->color = color;
#if defined ( GLQUAKE )
	p->packedColor = 0;
#else
	p->packedColor = hlRGB(host_basepal, color);
#endif
	p->type = pt_static;

	VectorCopy(vec3_origin, p->vel);
	p->vel[2] += zvel;

	VectorCopy(origin, p->org);
	p->org[2] += zpos;
}

/*
===============
CL_PlayerFlashlight
===============
*/
#define FLASHLIGHT_DISTANCE		2000
void CL_PlayerFlashlight( void )
{
	cl_entity_t* ent;
	dlight_t* dl;

	ent = cl_entities + cl.playernum + 1;

	if (ent->effects & (EF_BRIGHTLIGHT | EF_DIMLIGHT) && cl.worldmodel)
	{
		if (cl.pLight && cl.pLight->key == 1)
			dl = cl.pLight;
		else
			dl = CL_AllocDlight(1);

		if (ent->effects & EF_BRIGHTLIGHT)
		{
			dl->color.r = dl->color.g = dl->color.b = 250;
			dl->radius = 400;
			VectorCopy(ent->origin, dl->origin);
			dl->origin[2] += 16;
		}
		else
		{
			pmtrace_t trace;
			vec3_t end;
			float falloff;

			VectorCopy(ent->origin, dl->origin);
			dl->origin[2] += cl.viewheight;
			VectorMA(dl->origin, FLASHLIGHT_DISTANCE, vpn, end);

			// Trace a line outward, use studio box to avoid expensive studio hull calcs
			pmove.usehull = 2;
			Pmove_Init();
			trace = PM_PlayerMove(dl->origin, end, PM_STUDIO_BOX);

			if (trace.ent > 0)
			{
				// If we hit a studio model, light it at it's origin so it lights properly (no falloff)
				if (pmove.physents[trace.ent].studiomodel)
				{
					VectorCopy(pmove.physents[trace.ent].origin, trace.endpos);
				}
			}

			VectorCopy(trace.endpos, dl->origin);
			falloff = trace.fraction * FLASHLIGHT_DISTANCE;

			if (falloff < 500)
				falloff = 1.0;
			else
				falloff = 500.0 / (falloff);
			
			falloff *= falloff;
			dl->radius = 80;
			dl->color.r = dl->color.g = dl->color.b = 255 * falloff;
#if 0
			dlight_t* halo = CL_AllocDlight(2);
			halo->color.r = halo->color.g = halo->color.b = 60;
			halo->radius = 200;
			VectorCopy(ent->origin, halo->origin);
			halo->origin[2] += 16;
			halo->die = cl.time + 0.2;
#endif
		}

		cl.pLight = dl;
		dl->die = cl.time + 0.2;
		CL_TouchLight(dl);
	}
	else
	{
		if (cl.pLight && cl.pLight->key == 1)
			cl.pLight->die = cl.time;

		cl.pLight = NULL;
	}

	// Add a muzzle flash to the weapon model if the player is flashing
	if (cl_entities[cl.viewentity].effects & EF_MUZZLEFLASH)
	{
		cl.viewent.effects |= EF_MUZZLEFLASH;
	}
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

		if ((s1->flags & U_MONSTERMOVE) && ent->lastmove < cl.time)
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
				dl->radius = RandomFloat(200, 231);
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
	vec3_t			prevorigin, prevangles;

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

	state->received_time = realtime;
	state->number = num;

	flags = state->flags = MSG_ReadLong();

	state->physflags = MSG_ReadLong();
	state->messagenum = cl.parsecount;
	state->origin[0] = MSG_ReadCoord();
	state->origin[1] = MSG_ReadCoord();
	state->origin[2] = MSG_ReadCoord();

	VectorSubtract(state->origin, state->prevorigin, state->predorigin);

	state->frame = MSG_ReadByte();

	// Count player bits
	for (i = 0; i < MAX_CLIENTS; i++)
	{
		if (flags & (1 << i))
			playerbitcounts[i]++;
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
		MSG_ReadDeltaUsercmd(&state->command, &nullcmd);
		VectorCopy(state->command.angles, state->viewangles);
	}
	else
	{
		VectorCopy(vec3_origin, state->viewangles);
	}

	for (i = 0; i < 3; i++)
	{
		if (flags & (PF_VELOCITY1 << i))
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
		int	sequence;
		float animtime, delta;

		sequence = MSG_ReadByte();

		delta = (int)(cl.time * 100.0) - (byte)(cl.time * 100.0);
		animtime = (MSG_ReadByte() / 100.0) + (delta / 100.0);
		while (animtime > cl.time + 0.1)
			animtime -= 2.56;

		state->sequence = sequence;
		state->animtime = animtime;
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
		info->ping = MSG_ReadShort();

	ent = &cl_entities[num + 1];

	VectorCopy(ent->origin, prevorigin);
	VectorCopy(ent->angles, prevangles);

	VectorCopy(state->origin, ent->origin);
	VectorCopy(state->viewangles, ent->angles);
	// Player pitch is inverted
	ent->angles[PITCH] /= -3.0;

	ent->frame = state->frame;
	ent->movetype = state->movetype;
	ent->effects = state->effects;
	ent->model = cl.model_precache[state->modelindex];
	ent->skin = state->skinnum;
	ent->body = state->body;

	if (ent->animtime != state->animtime)
	{
		if (ent->sequence != state->sequence)
		{
			ent->sequencetime = state->animtime;
			ent->prevsequence = ent->sequence;
			memcpy(ent->prevseqblending, ent->blending, 2);
			ent->sequence = state->sequence;
		}

		ent->prevanimtime = ent->animtime;
		memcpy(ent->prevcontroller, ent->controller, 4);
		memcpy(ent->prevblending, ent->blending, 2);
		VectorCopy(prevorigin, ent->prevorigin);
		VectorCopy(prevangles, ent->prevangles);

		ent->animtime = state->animtime;
	}

	ent->colormap = info->translations;
	if (state->modelindex == cl_playerindex)
		ent->scoreboard = info;		// use custom skin
	else
		ent->scoreboard = NULL;

	ent->rendermode = state->rendermode;
	ent->renderamt = state->renderamt;
	ent->rendercolor = state->rendercolor;
	ent->renderfx = state->renderfx;

	ent->framerate = state->framerate;

	memcpy(ent->controller, state->controller, 4);
	memcpy(ent->blending, state->blending, 2);
}

/*
=============
CL_LinkPlayers

Create visible entities in the correct position
for all current players
=============
*/
void CL_LinkPlayers( void )
{
	int				i, j;
	player_info_t* info;
	player_state_t* state;
	player_state_t	exact;
	double			playertime;
	cl_entity_t* ent;
	int				msec;
	frame_t* frame;
	int				oldphysent;
	int				flags;
	float			frac;
	dlight_t* dl;

	frac = cl.frame_lerp;

//
// interpolate player info
//
	for (i = 0; i < 3; i++)
		cl.mvelocity[1][i] = cl.mvelocity[1][i] +
		frac * (cl.mvelocity[0][i] - cl.mvelocity[1][i]);

	playertime = realtime - cls.latency + 0.02;
	if (playertime > realtime)
		playertime = realtime;

	frame = &cl.frames[cl.parsecount & UPDATE_MASK];

	for (j = 0, info = cl.players, state = frame->playerstate; j < MAX_CLIENTS
		; j++, info++, state++)
	{
		if (state->messagenum != cl.parsecount)
			continue;	// not present this frame

		// the player object never gets added
		if (!cam_thirdperson && cl.viewentity == j + 1 && !chase_active.value)
			continue;

		if (!state->modelindex)
			continue;

		if (state->effects & EF_NODRAW)
			continue;

		if (cl.spectator && autocam == CAM_FIRSTPERSON && spec_track == j)
			continue;

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
//Con_DPrintf("nopredict\n");
		}
		else
		{
			// predict players movement
			if (msec > 255)
				msec = 255;
			state->command.msec = msec;
//Con_DPrintf("predict: %i\n", msec);

			oldphysent = pmove.numphysent;
			CL_SetSolidPlayers(j);
			CL_PredictUsercmd(state, &exact, &state->command, FALSE);
			pmove.numphysent = oldphysent;
			VectorCopy(exact.origin, ent->origin);
		}
		
		VectorCopy(ent->origin, ent->prevorigin);

		ent->colormap = info->translations;
		if (state->modelindex == cl_playerindex)
			ent->scoreboard = info;		// use custom skin
		else
			ent->scoreboard = NULL;

		if (!flags || (ent->effects & EF_NOINTERP))
		{
			ent->prevsequence = ent->sequence;
			ent->animtime = cl.time;
			ent->prevanimtime = cl.time;
			VectorCopy(ent->origin, ent->prevorigin);
			VectorCopy(ent->angles, ent->prevangles);
		}

		if (cl.viewentity != j + 1)
		{
			if (ent->effects & EF_BRIGHTLIGHT)
			{
				dl = CL_AllocDlight(3);
				VectorCopy(ent->origin, dl->origin);
				dl->origin[2] += 16;
				dl->color.r = dl->color.g = dl->color.b = 250;
				dl->radius = RandomFloat(400, 431);
				dl->die = cl.time + 0.001;
			}

			if (ent->effects & EF_DIMLIGHT)
			{
				dl = CL_AllocDlight(3);
				VectorCopy(ent->origin, dl->origin);
				dl->color.r = dl->color.g = dl->color.b = 100;
				dl->radius = RandomFloat(200, 231);
				dl->die = cl.time + 0.001;
			}
		}

		if (cl_printplayers.value)
		{
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

/*
==================
CL_GetPredictedOrigin

==================
*/
void CL_GetPredictedOrigin( int playernum, vec_t* origin )
{
	predicted_player* pplayer;

	if (playernum < 0 || playernum >= MAX_CLIENTS)
	{
		VectorCopy(vec3_origin, origin);
		Con_DPrintf("CL_GetPredictedOrigin called with bogus player # %i\n", playernum);
		return;
	}

	pplayer = &predicted_players[playernum];
	if (!pplayer->active)
	{
		VectorCopy(vec3_origin, origin);
		Con_DPrintf("CL_GetPredictedOrigin called on inactive player # %i\n", playernum);
		return;
	}

	VectorCopy(pplayer->origin, origin);
}

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
	int				j;
	player_state_t* state;
	player_state_t	exact;
	double			playertime;
	int				msec;
	frame_t* frame;
	predicted_player* pplayer;

	playertime = realtime - cls.latency + 0.02;
	if (playertime > realtime)
		playertime = realtime;

	frame = &cl.frames[cl.parsecount & UPDATE_MASK];

	for (j = 0, pplayer = predicted_players, state = frame->playerstate;
		j < MAX_CLIENTS;
		j++, pplayer++, state++)
	{
		pplayer->active = FALSE;

		if (state->messagenum != cl.parsecount)
			continue;	// not present this frame

		if (!state->modelindex)
			continue;

		if (state->flags & EF_NODRAW)
			continue;

		pplayer->active = TRUE;
		pplayer->index = 1;
		pplayer->flags = state->flags;
		pplayer->physflags = state->physflags;

		if (!(pplayer->physflags & FL_DUCKING))
			pplayer->index = 0;

		// note that the local player is special, since he moves locally
		// we use his last predicted postition
		if (j == cl.playernum || (cl.spectator && autocam == CAM_FIRSTPERSON && j == spec_track))
		{
			VectorCopy(cl.frames[cls.netchan.outgoing_sequence & UPDATE_MASK].playerstate[cl.playernum].origin,
				pplayer->origin);
		}
		else
		{
			// only predict half the move to minimize overruns
			msec = 500 * (playertime - state->state_time);
			if (msec <= 0 ||
				!cl_predict_players.value ||
				!dopred)
			{
				VectorCopy(state->origin, pplayer->origin);
	//Con_DPrintf("nopredict\n");
			}
			else
			{
				// predict players movement
				if (msec > 255)
					msec = 255;
				state->command.msec = msec;
	//Con_DPrintf("predict: %i\n", msec);

				CL_PredictUsercmd(state, &exact, &state->command, FALSE);
				VectorCopy(exact.origin, pplayer->origin);
			}
		}
	}
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
	int		j;
	extern	vec3_t	player_mins[3];
	extern	vec3_t	player_maxs[3];
	predicted_player* pplayer;
	physent_t *pent;

	if (!cl_solid_players.value)
		return;

	pent = pmove.physents + pmove.numphysent;

	for (j = 0, pplayer = predicted_players; j < MAX_CLIENTS; j++, pplayer++)
	{
		if (!pplayer->active)
			continue;	// not present this frame

		// the player object never gets added
		if (j == playernum)
			continue;

		if (pplayer->flags & PF_DEAD)
			continue; // dead players aren't solid

		pent->model = NULL;
		pent->skin = 0;
		pent->solid = SOLID_BBOX;
		VectorCopy(pplayer->origin, pent->origin);
		VectorCopy(player_mins[pplayer->index], pent->mins);
		VectorCopy(player_maxs[pplayer->index], pent->maxs);
		VectorCopy(vec3_origin, pent->angles);
		pmove.numphysent++;
		pent++;
	}
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

	if (cls.demorecording)
		CL_WriteDemoUpdate();

	cl_oldnumvisedicts = cl_numvisedicts;
	slot = cl_visedicts_list[(cls.netchan.incoming_sequence + 1) & 1];
	cl_oldvisedicts = slot;

	memset(packet_flags, 0, sizeof(packet_flags));

	if (cl_numvisedicts > 0)
	{
		cl_oldvisedicts = cl_visedicts_list[(cls.netchan.incoming_sequence + 1) & 1];

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
		if (autocam == CAM_FIRSTPERSON)
		{
			vec3_t pos;
			Cam_GetPredictedFirstPersonOrigin(pos);
			VectorCopy(pos, cl.viewangles);
			VectorCopy(pos, cl.simangles);
		}

		if (autocam == CAM_TOPDOWN)
		{
			vec3_t pos;
			Cam_GetPredictedTopDownOrigin(pos);
			VectorCopy(pos, cl.viewangles);
			VectorCopy(pos, cl.simangles);
		}
	}
}