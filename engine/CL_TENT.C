// cl_tent.c -- client side temporary entities

#include "quakedef.h"
#include "cl_tent.h"
#include "pr_cmds.h"
#include "decal.h"
#include "r_efx.h"

static int gTempEntFrame = 0;

TEMPENTITY gTempEnts[MAX_TEMP_ENTITIES], * gpTempEntFree, * gpTempEntActive;


sfx_t* cl_sfx_ric1;
sfx_t* cl_sfx_ric2;
sfx_t* cl_sfx_ric3;
sfx_t* cl_sfx_ric4;
sfx_t* cl_sfx_ric5;
sfx_t* cl_sfx_r_exp1;
sfx_t* cl_sfx_r_exp2;
sfx_t* cl_sfx_r_exp3;
sfx_t* cl_sfx_pl_shell1;
sfx_t* cl_sfx_pl_shell2;
sfx_t* cl_sfx_pl_shell3;

sfx_t* cl_sfx_sshell1;
sfx_t* cl_sfx_sshell2;
sfx_t* cl_sfx_sshell3;

sfx_t* cl_sfx_wood1;
sfx_t* cl_sfx_wood2;
sfx_t* cl_sfx_wood3;

sfx_t* cl_sfx_metal1;
sfx_t* cl_sfx_metal2;
sfx_t* cl_sfx_metal3;

sfx_t* cl_sfx_glass1;
sfx_t* cl_sfx_glass2;
sfx_t* cl_sfx_glass3;

sfx_t* cl_sfx_concrete1;
sfx_t* cl_sfx_concrete2;
sfx_t* cl_sfx_concrete3;

sfx_t* cl_sfx_flesh1;
sfx_t* cl_sfx_flesh2;
sfx_t* cl_sfx_flesh3;
sfx_t* cl_sfx_flesh4;
sfx_t* cl_sfx_flesh5;
sfx_t* cl_sfx_flesh6;

sfx_t* cl_sfx_geiger1;
sfx_t* cl_sfx_geiger2;
sfx_t* cl_sfx_geiger3;
sfx_t* cl_sfx_geiger4;
sfx_t* cl_sfx_geiger5;
sfx_t* cl_sfx_geiger6;


sfx_t* cl_sfx_imp;
sfx_t* cl_sfx_rail;


model_t* cl_sprite_white;
model_t* cl_sprite_dot;
model_t* cl_sprite_lightning;
model_t* cl_sprite_glow;

// Muzzle flash sprites
model_t* cl_sprite_muzzleflash[3];
model_t* cl_sprite_ricochet;
model_t* cl_sprite_shell;

// Tracers
cvar_t tracerSpeed = { "tracerspeed", "6000" };
cvar_t tracerOffset = { "traceroffset", "30" };
cvar_t tracerLength = { "tracerlength", "0.8" };
cvar_t tracerRed = { "tracerred", "0.8" };
cvar_t tracerGreen = { "tracergreen", "0.8" };
cvar_t tracerBlue = { "tracerblue", "0.4" };
cvar_t tracerAlpha = { "traceralpha", "0.5" };

cvar_t egon_amplitude = { "egon_amplitude", "0.0" };

/*
=================
CL_TentModel

Force precache of a model used by temporary entities
=================
*/
model_t* CL_TentModel( char* name )
{
	model_t* pModel;

	pModel = Mod_ForName(name, TRUE);

	Mod_MarkClient(pModel);
	return pModel;
}

/*
=================
CL_InitTEnts

Initialize TE system
=================
*/
void CL_InitTEnts( void )
{
	Cvar_RegisterVariable(&tracerSpeed);
	Cvar_RegisterVariable(&tracerOffset);
	Cvar_RegisterVariable(&tracerLength);
	Cvar_RegisterVariable(&tracerRed);
	Cvar_RegisterVariable(&tracerGreen);
	Cvar_RegisterVariable(&tracerBlue);
	Cvar_RegisterVariable(&tracerAlpha);

	cl_sfx_ric1 = S_PrecacheSound("weapons/ric1.wav");
	cl_sfx_ric2 = S_PrecacheSound("weapons/ric2.wav");
	cl_sfx_ric3 = S_PrecacheSound("weapons/ric3.wav");
	cl_sfx_ric4 = S_PrecacheSound("weapons/ric4.wav");
	cl_sfx_ric5 = S_PrecacheSound("weapons/ric5.wav");

	cl_sfx_r_exp1 = S_PrecacheSound("weapons/explode3.wav");
	cl_sfx_r_exp2 = S_PrecacheSound("weapons/explode4.wav");
	cl_sfx_r_exp3 = S_PrecacheSound("weapons/explode5.wav");

	cl_sfx_pl_shell1 = S_PrecacheSound("player/pl_shell1.wav");
	cl_sfx_pl_shell2 = S_PrecacheSound("player/pl_shell2.wav");
	cl_sfx_pl_shell3 = S_PrecacheSound("player/pl_shell3.wav");

	cl_sfx_sshell1 = S_PrecacheSound("weapons/sshell1.wav");
	cl_sfx_sshell2 = S_PrecacheSound("weapons/sshell2.wav");
	cl_sfx_sshell3 = S_PrecacheSound("weapons/sshell3.wav");

	cl_sfx_wood1 = S_PrecacheSound("debris/wood1.wav");
	cl_sfx_wood2 = S_PrecacheSound("debris/wood2.wav");
	cl_sfx_wood3 = S_PrecacheSound("debris/wood3.wav");

	cl_sfx_metal1 = S_PrecacheSound("debris/metal1.wav");
	cl_sfx_metal2 = S_PrecacheSound("debris/metal2.wav");
	cl_sfx_metal3 = S_PrecacheSound("debris/metal3.wav");

	cl_sfx_glass1 = S_PrecacheSound("debris/glass1.wav");
	cl_sfx_glass2 = S_PrecacheSound("debris/glass2.wav");
	cl_sfx_glass3 = S_PrecacheSound("debris/glass3.wav");

	cl_sfx_concrete1 = S_PrecacheSound("debris/concrete1.wav");
	cl_sfx_concrete2 = S_PrecacheSound("debris/concrete2.wav");
	cl_sfx_concrete3 = S_PrecacheSound("debris/concrete3.wav");

	cl_sfx_flesh1 = S_PrecacheSound("debris/flesh1.wav");
	cl_sfx_flesh2 = S_PrecacheSound("debris/flesh2.wav");
	cl_sfx_flesh3 = S_PrecacheSound("debris/flesh3.wav");
	cl_sfx_flesh4 = S_PrecacheSound("debris/flesh5.wav");
	cl_sfx_flesh5 = S_PrecacheSound("debris/flesh6.wav");
	cl_sfx_flesh6 = S_PrecacheSound("debris/flesh7.wav");

	cl_sfx_geiger1 = S_PrecacheSound("player/geiger1.wav");
	cl_sfx_geiger2 = S_PrecacheSound("player/geiger2.wav");
	cl_sfx_geiger3 = S_PrecacheSound("player/geiger3.wav");
	cl_sfx_geiger4 = S_PrecacheSound("player/geiger4.wav");
	cl_sfx_geiger5 = S_PrecacheSound("player/geiger5.wav");
	cl_sfx_geiger6 = S_PrecacheSound("player/geiger6.wav");

	cl_sprite_dot = CL_TentModel("sprites/dot.spr");
	cl_sprite_lightning = CL_TentModel("sprites/lgtning.spr");
	cl_sprite_white = CL_TentModel("sprites/white.spr");
	cl_sprite_glow = CL_TentModel("sprites/animglow01.spr");

	cl_sprite_muzzleflash[0] = CL_TentModel("sprites/muzzleflash1.spr");
	cl_sprite_muzzleflash[1] = CL_TentModel("sprites/muzzleflash2.spr");
	cl_sprite_muzzleflash[2] = CL_TentModel("sprites/muzzleflash3.spr");

	cl_sprite_ricochet = CL_TentModel("sprites/richo1.spr");
	cl_sprite_shell = CL_TentModel("sprites/wallpuff.spr");

	CL_TempEntInit();
}

int ModelFrameCount( model_t* model )
{
	int count;

	count = 1;

	if (model)
	{
		if (model->type == mod_sprite)
		{
			msprite_t* psprite;

			psprite = (msprite_t*)model->cache.data;
			count = psprite->numframes;
		}
		else if (model->type == mod_studio)
		{
//			count = R_StudioBodyVariations(model); TODO: Implement
		}

		if (count < 1)
			count = 1;
	}

	return count;
}

/*
===============
R_TracerEffect

===============
*/
void R_TracerEffect( vec_t* start, vec_t* end )
{
	vec3_t	temp, vel;
	float	len;

	if (tracerSpeed.value <= 0)
		tracerSpeed.value = 3;

	VectorSubtract(end, start, temp);
	len = Length(temp);

	VectorScale(temp, 1.0 / len, temp);
	VectorScale(temp, RandomLong(-10, 9) + tracerOffset.value, vel);
	VectorAdd(start, vel, start);
	VectorScale(temp, tracerSpeed.value, vel);

	R_TracerParticles(start, vel, len / tracerSpeed.value);
}


// TODO: Implement


/*
===============
R_Sprite_WallPuff

Create a wallpuff
===============
*/
void R_Sprite_WallPuff( TEMPENTITY* pTemp, float scale )
{
	// TODO: Implement
}


// TODO: Implement


/*
=================
CL_ParseTEnt
=================
*/
void CL_ParseTEnt( void )
{
	char	c;
	int		type;
	int		soundtype;
	int		iRand;
	int		entnumber;
	int		modelindex;
	vec3_t	pos;
	vec3_t	size;
	vec3_t	dir;
	vec3_t	endpos;
	dlight_t* dl;
	int		startEnt, endEnt;
	int		startFrame;
	int		count;
	float	life;
	float	scale;
	float	r, g, b, a;
	float	frameRate;
	int		flags;

	type = MSG_ReadByte();
	switch (type)
	{
	case TE_BEAMPOINTS:
	case TE_BEAMENTPOINT:
	case TE_BEAMENTS:
	{
		float width;
		float amplitude;
		float flSpeed;

		if (type == TE_BEAMENTS)
		{
			startEnt = MSG_ReadShort();
			endEnt = MSG_ReadShort();
		}
		else if (type == TE_BEAMENTPOINT)
		{
			startEnt = MSG_ReadShort();
			endpos[0] = MSG_ReadCoord();
			endpos[1] = MSG_ReadCoord();
			endpos[2] = MSG_ReadCoord();
		}
		else
		{
			pos[0] = MSG_ReadCoord();
			pos[1] = MSG_ReadCoord();
			pos[2] = MSG_ReadCoord();
			endpos[0] = MSG_ReadCoord();
			endpos[1] = MSG_ReadCoord();
			endpos[2] = MSG_ReadCoord();
		}

		modelindex = MSG_ReadShort();

		startFrame = MSG_ReadByte();
		frameRate = MSG_ReadByte() * 0.1;

		life = MSG_ReadByte() * 0.1;
		width = MSG_ReadByte() * 0.1;
		amplitude = MSG_ReadByte() * 0.01;

		r = MSG_ReadByte() / 255.0;
		g = MSG_ReadByte() / 255.0;
		b = MSG_ReadByte() / 255.0;
		a = MSG_ReadByte() / 255.0;

		flSpeed = MSG_ReadByte() * 0.1;

		// TODO: Implement
		break;
	}

		// TODO: Implement

	case TE_EXPLOSION:			// rocket explosion
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();

		modelindex = MSG_ReadShort();

		scale = MSG_ReadByte() * 0.1;
		frameRate = MSG_ReadByte();

		if (scale != 0)
		{
			// TODO: Implement

			// big flash
			dl = CL_AllocDlight(0);
			VectorCopy(pos, dl->origin);
			dl->radius = 200;
			dl->color.r = 250;
			dl->color.g = 250;
			dl->color.b = 150;
			dl->die = cl.time + 0.01;
			dl->decay = 800;


			// red glow
			dl = CL_AllocDlight(0);
			VectorCopy(pos, dl->origin);
			dl->radius = 150;
			dl->color.r = 255;
			dl->color.g = 190;
			dl->color.b = 40;
			dl->die = cl.time + 1.0;
			dl->decay = 200;
		}

		// sound
		switch (RandomLong(0, 2))
		{
		case 0:
			S_StartDynamicSound(-1, CHAN_AUTO, cl_sfx_r_exp1, pos, VOL_NORM, 0.3, 0, PITCH_NORM);
			break;
		case 1:
			S_StartDynamicSound(-1, CHAN_AUTO, cl_sfx_r_exp2, pos, VOL_NORM, 0.3, 0, PITCH_NORM);
			break;
		case 2:
			S_StartDynamicSound(-1, CHAN_AUTO, cl_sfx_r_exp3, pos, VOL_NORM, 0.3, 0, PITCH_NORM);
			break;
		}
		break;

		// TODO: Implement

	case TE_SMOKE:		// alphablend sprite, move vertically 30 pps
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();
		modelindex = MSG_ReadShort();
		scale = MSG_ReadByte() * 0.1;
		frameRate = MSG_ReadByte();
		// TODO: Implement
		break;

	case TE_TRACER:
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();
		endpos[0] = MSG_ReadCoord();
		endpos[1] = MSG_ReadCoord();
		endpos[2] = MSG_ReadCoord();
		R_TracerEffect(pos, endpos);
		break;

		// TODO: Implement

	case TE_SPARKS:
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();
//		R_SparkEffect(pos, 8, -200, 200); TODO: Implement
		break;

		// TODO: Implement

	case TE_BSPDECAL:
	case TE_DECAL:
	case TE_WORLDDECAL:
	case TE_WORLDDECALHIGH:
	case TE_DECALHIGH:
	{
		int decalTextureIndex;

		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();

		if (type == TE_BSPDECAL)
		{
			decalTextureIndex = MSG_ReadShort();

			modelindex = 0; // Never remove it
			flags = FDECAL_PERMANENT;
		}
		else
		{
			decalTextureIndex = MSG_ReadByte();

			modelindex = 0;
			flags = 0;
		}

		if (type == TE_DECAL || type == TE_DECALHIGH || type == TE_BSPDECAL)
		{
			entnumber = MSG_ReadShort();

			if (type == TE_BSPDECAL && entnumber)
			{
				modelindex = MSG_ReadShort();
			}
		}
		else
		{
			entnumber = 0;
		}

		if (type == TE_DECALHIGH || type == TE_WORLDDECALHIGH)
			decalTextureIndex += 256; // texture index is greater than 256

		if (entnumber >= cl.max_edicts)
			Sys_Error("Decal: entity = %i", entnumber);

		if (r_decals.value)
		{
			R_DecalShoot(Draw_DecalIndex(decalTextureIndex), entnumber, modelindex, pos, flags);
		}
		break;
	}

		// TODO: Implement

	case TE_DLIGHT:
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();

		dl = CL_AllocDlight(0);
		VectorCopy(pos, dl->origin);
		dl->radius = (float)(MSG_ReadByte() * 10.0);
		dl->color.r = MSG_ReadByte();
		dl->color.g = MSG_ReadByte();
		dl->color.b = MSG_ReadByte();
		dl->die = cl.time + MSG_ReadByte() * 0.1;
		dl->decay = MSG_ReadByte() * 10.0;
		break;

	case TE_ELIGHT:
		// TODO: Implement
		break;

		// TODO: Implement

	case TE_MODEL:
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();

		dir[0] = MSG_ReadCoord();
		dir[1] = MSG_ReadCoord();
		dir[2] = MSG_ReadCoord();

		endpos[0] = 0.0;
		endpos[1] = MSG_ReadAngle();
		endpos[2] = 0.0;

		modelindex = MSG_ReadShort();
		soundtype = MSG_ReadByte();
		life = MSG_ReadByte() * 0.1;

		//R_TempModel(pos, dir, endpos, life, modelindex, soundtype); TODO: Implement
		break;

		// TODO: Implement
		
	case TE_BREAKMODEL:
	{
		float frandom;

		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();

		size[0] = MSG_ReadCoord();
		size[1] = MSG_ReadCoord();
		size[2] = MSG_ReadCoord();

		dir[0] = MSG_ReadCoord();
		dir[1] = MSG_ReadCoord();
		dir[2] = MSG_ReadCoord();

		frandom = MSG_ReadByte() * 10.0;
		modelindex = MSG_ReadShort();
		count = MSG_ReadByte();
		life = MSG_ReadByte() * 0.1;
		c = MSG_ReadByte();

		// TODO: Implement
		break;
	}

	case TE_GUNSHOTDECAL:
	{
		int decalTextureIndex;
		TEMPENTITY* pTemp;

		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();

		entnumber = MSG_ReadShort();
		decalTextureIndex = MSG_ReadByte();	

		pTemp = CL_TempEntAlloc(pos, cl_sprite_shell);
		R_Sprite_WallPuff(pTemp, 0.3);

		iRand = RandomLong(0, 32767);
		if (iRand < 16383)
		{
			switch (iRand % 5)
			{
			case 0:
				S_StartDynamicSound(-1, CHAN_AUTO, cl_sfx_ric1, pos, VOL_NORM, 1.0, 0, PITCH_NORM);
				break;
			case 1:
				S_StartDynamicSound(-1, CHAN_AUTO, cl_sfx_ric2, pos, VOL_NORM, 1.0, 0, PITCH_NORM);
				break;
			case 2:
				S_StartDynamicSound(-1, CHAN_AUTO, cl_sfx_ric3, pos, VOL_NORM, 1.0, 0, PITCH_NORM);
				break;
			case 3:
				S_StartDynamicSound(-1, CHAN_AUTO, cl_sfx_ric4, pos, VOL_NORM, 1.0, 0, PITCH_NORM);
				break;
			case 4:
				S_StartDynamicSound(-1, CHAN_AUTO, cl_sfx_ric5, pos, VOL_NORM, 1.0, 0, PITCH_NORM);
				break;
			}
		}

		if (entnumber >= cl.max_edicts)
			Sys_Error("Decal: entity = %i", entnumber);

		if (r_decals.value)
		{
			R_DecalShoot(Draw_DecalIndex(decalTextureIndex), entnumber, 0, pos, 0);
		}
		break;
	}

		// TODO: Implement

	default:
		Sys_Error("CL_ParseTEnt: bad type");
		break;
	}
}

// TODO: Implement

/*
=================
CL_TempEntInit

Initialize temp entities
=================
*/
void CL_TempEntInit( void )
{
	Q_memset(gTempEnts, 0, sizeof(gTempEnts));

	// Fix up pointers
	for (int i = 0; i < MAX_TEMP_ENTITIES - 1; ++i)
	{
		gTempEnts[i].next = &gTempEnts[i + 1];
	}

	gTempEnts[MAX_TEMP_ENTITIES - 1].next = NULL;

	gpTempEntFree = gTempEnts;
	gpTempEntActive = NULL;
}

// TODO: Implement

/*
=================
CL_TempEntAlloc

Allocate temp entity
=================
*/
TEMPENTITY* CL_TempEntAlloc( vec_t* org, model_t* model )
{
	TEMPENTITY* ent;

	if (gpTempEntFree)
	{
		if (!model)
			return NULL;

		ent = gpTempEntFree;

		gpTempEntFree = gpTempEntFree->next;

		memset(&ent->entity, 0, sizeof(cl_entity_t));

		ent->flags = 0;
		ent->entity.colormap = vid.colormap;

		// set the model
		ent->entity.model = model;

		// set the render mode and special effects
		ent->entity.rendermode = kRenderNormal;
		ent->entity.renderfx = kRenderFxNone;

		ent->fadeSpeed = 0.5;

		// when should we disappear?
		ent->die = cl.time + 0.75;

		VectorCopy(org, ent->entity.origin);

		// link it with another active temp entity if any
		ent->next = gpTempEntActive;

		gpTempEntActive = ent;

		return ent;
	}
	else
	{
		Con_DPrintf("Overflow %d temporary ents!", MAX_TEMP_ENTITIES);
	}

	return NULL;
}

/*
=============
CL_TempEntPlaySound

play collide bounce sound
=============
*/
void CL_TempEntPlaySound( TEMPENTITY *pTemp, float damp )
{
	char*		soundname;
	float		fvol, zvel;
	sfx_t*		bouncesounds[6];
	int			sndamt, pitch, sndnum;
	qboolean	shell;

	soundname = NULL;
	shell = FALSE;

	memset(bouncesounds, 0, sizeof(bouncesounds));

	switch (pTemp->hitSound)
	{
	case BOUNCE_GLASS:
		sndamt = 3;
		bouncesounds[0] = cl_sfx_glass1;
		bouncesounds[1] = cl_sfx_glass2;
		bouncesounds[2] = cl_sfx_glass3;
		fvol = 0.8f;
		break;
	case BOUNCE_METAL:
		sndamt = 3;
		bouncesounds[0] = cl_sfx_metal1;
		bouncesounds[1] = cl_sfx_metal2;
		bouncesounds[2] = cl_sfx_metal3;
		fvol = 0.8f;
		break;
	case BOUNCE_FLESH:
		sndamt = 6;
		bouncesounds[0] = cl_sfx_flesh1;
		bouncesounds[1] = cl_sfx_flesh2;
		bouncesounds[2] = cl_sfx_flesh3;
		bouncesounds[3] = cl_sfx_flesh4;
		bouncesounds[4] = cl_sfx_flesh5;
		bouncesounds[5] = cl_sfx_flesh6;
		fvol = 0.8f;
		break;
	case BOUNCE_WOOD:
		sndamt = 3;
		bouncesounds[0] = cl_sfx_wood1;
		bouncesounds[1] = cl_sfx_wood2;
		bouncesounds[2] = cl_sfx_wood3;
		fvol = 0.8f;
		break;
	case BOUNCE_SHRAP: //"SHRAP"
		sndamt = 5;
		bouncesounds[0] = cl_sfx_ric1;
		bouncesounds[1] = cl_sfx_ric2;
		bouncesounds[2] = cl_sfx_ric3;
		bouncesounds[3] = cl_sfx_ric4;
		bouncesounds[4] = cl_sfx_ric5;
		fvol = 0.8f;
		break;
	case BOUNCE_SHELL:
		sndamt = 3;
		shell = TRUE;
		bouncesounds[0] = cl_sfx_pl_shell1;
		bouncesounds[1] = cl_sfx_pl_shell2;
		bouncesounds[2] = cl_sfx_pl_shell3;
		fvol = 0.8f;
		break;
	case BOUNCE_CONCRETE:
		sndamt = 3;
		bouncesounds[0] = cl_sfx_concrete1;
		bouncesounds[1] = cl_sfx_concrete2;
		bouncesounds[2] = cl_sfx_concrete3;
		fvol = 0.8f;
		break;
	case BOUNCE_SHOTSHELL:
		sndamt = 3;
		shell = TRUE;
		bouncesounds[0] = cl_sfx_sshell1;
		bouncesounds[1] = cl_sfx_sshell2;
		bouncesounds[2] = cl_sfx_sshell3;
		fvol = 0.5f;
		break;
	default:
		return;
	}

	zvel = abs(pTemp->entity.baseline.origin[2]);

	if (shell)
	{
		if (zvel < 200 && RandomLong(0, 3))
			return;
	}
	else
	{
		if (RandomLong(0, 5))
			return;
	}

	if (damp > 0)
	{
		if (RandomLong(0, 3))
		{
			pitch = 100;
		}
		else
		{
			pitch = 100;
			if (!shell)
				pitch = RandomLong(90, 124);
		}

		if (shell)
		{
			fvol *= min(1, zvel / 350);
		}
		else
		{
			fvol *= min(1, zvel / 450);
		}

		sndnum = RandomLong(0, sndamt - 1);

		S_StartDynamicSound(-1, CHAN_AUTO, bouncesounds[sndnum], pTemp->entity.origin, fvol, ATTN_NORM, 0, pitch);
	}
}

/*
=============
CL_AddVisibleEntity

Adds a client entity to the list of visible entities if it's within the PVS
=============
*/
int CL_AddVisibleEntity( cl_entity_t* ent )
{
	vec3_t	world_mins, world_maxs;
	int		i;

	if (!ent->model)
		return FALSE; // invalid entity was passed into this function

	if (cl_numvisedicts >= MAX_VISEDICTS)
		return FALSE; // object list is full

	for (i = 0; i < 3; ++i)
	{
		world_mins[i] = ent->model->mins[i] + ent->origin[i];
		world_maxs[i] = ent->model->maxs[i] + ent->origin[i];
	}

	if (!PVSNode(cl.worldmodel->nodes, world_mins, world_maxs))
		return FALSE;

	ent->index = -1;

	memcpy(&cl_visedicts[cl_numvisedicts], ent, sizeof(cl_entity_t));

	cl_numvisedicts++;

	return TRUE;
}

/*
=============
CL_UpdateTEnts

Simulation and cleanup of temporary entities
=============
*/
extern cvar_t sv_gravity;
extern qboolean SV_RecursiveHullCheck(hull_t *hull, int num, float p1f, float p2f, vec3_t p1, vec3_t p2, trace_t *trace);
void CL_TempEntUpdate( void )
{
	double		frametime;
	TEMPENTITY* pTemp, *pprev, *pnext;
	float		freq, gravity, gravitySlow, life, fastFreq;
	int			i;

	// Nothing to simulate
	if (!gpTempEntActive || !cl.worldmodel)
		return;

	// !!!BUGBUG	-- This needs to be time based
	gTempEntFrame = (gTempEntFrame + 1) & 31;

	frametime = cl.time - cl.oldtime;

	pTemp = gpTempEntActive;

	// !!! Don't simulate while paused....  This is sort of a hack, revisit.
	if (frametime <= 0)
	{
		while (pTemp)
		{
			CL_AddVisibleEntity(&pTemp->entity);
			pTemp = pTemp->next;
		}

		return;
	}

	pprev = NULL;
	freq = cl.time * 0.01;
	fastFreq = cl.time * 5.5;
	gravity = -frametime * sv_gravity.value;
	gravitySlow = gravity * 0.5;

	while (pTemp)
	{
		int active;

		active = 1;

		life = pTemp->die - cl.time;
		pnext = pTemp->next;

		if (life < 0)
		{
			if (pTemp->flags & FTENT_FADEOUT)
			{
				if (pTemp->entity.rendermode == kRenderNormal)
					pTemp->entity.rendermode = kRenderTransTexture;
				pTemp->entity.renderamt = pTemp->entity.baseline.renderamt * (1 + life * pTemp->fadeSpeed);
				if (pTemp->entity.renderamt <= 0)
					active = 0;

			}
			else
				active = 0;
		}

		if (!active)		// Kill it
		{
			pTemp->next = gpTempEntFree;
			gpTempEntFree = pTemp;
			if (!pprev)	// Deleting at head of list
				gpTempEntActive = pnext;
			else
				pprev->next = pnext;
		}
		else
		{
			pprev = pTemp;

			VectorCopy(pTemp->entity.origin, pTemp->entity.prevorigin);

			if (pTemp->flags & FTENT_SINEWAVE)
			{
				pTemp->x += pTemp->entity.baseline.origin[0] * frametime;
				pTemp->y += pTemp->entity.baseline.origin[1] * frametime;

				pTemp->entity.origin[0] = pTemp->x + sin(pTemp->entity.baseline.origin[2] + cl.time * pTemp->entity.prevframe) * (10 * pTemp->entity.scale);
				pTemp->entity.origin[1] = pTemp->y + sin(pTemp->entity.baseline.origin[2] + fastFreq + 0.7) * (8 * pTemp->entity.scale);
				pTemp->entity.origin[2] += pTemp->entity.baseline.origin[2] * frametime;
			}
			else if (pTemp->flags & FTENT_SPIRAL)
			{
				float s, c;
				s = sin(pTemp->entity.baseline.origin[2] + fastFreq);
				c = cos(pTemp->entity.baseline.origin[2] + fastFreq);

				pTemp->entity.origin[0] += pTemp->entity.baseline.origin[0] * frametime + 8 * sin(cl.time * 20 + (int)pTemp);
				pTemp->entity.origin[1] += pTemp->entity.baseline.origin[1] * frametime + 4 * sin(cl.time * 30 + (int)pTemp);
				pTemp->entity.origin[2] += pTemp->entity.baseline.origin[2] * frametime;
			}
			else
			{
				for (i = 0; i < 3; i++)
					pTemp->entity.origin[i] += pTemp->entity.baseline.origin[i] * frametime;
			}

			if (pTemp->flags & FTENT_SPRANIMATE)
			{
				pTemp->entity.frame += frametime * pTemp->entity.framerate;
				if (pTemp->entity.frame >= pTemp->frameMax)
				{
					pTemp->entity.frame = pTemp->entity.frame - (int)(pTemp->entity.frame);

					// this animating sprite isn't set to loop, so destroy it.
					pTemp->die = cl.time;
				}
			}
			else if (pTemp->flags & FTENT_SPRCYCLE)
			{
				pTemp->entity.frame += frametime * 10;
				if (pTemp->entity.frame >= pTemp->frameMax)
				{
					pTemp->entity.frame = pTemp->entity.frame - (int)(pTemp->entity.frame);
				}
			}

// Experiment
#if 0
			if (pTemp->flags & FTENT_SCALE)
				pTemp->entity.scale += 20.0 * (frametime / pTemp->entity.scale);
#endif

			if (pTemp->flags & FTENT_ROTATE)
			{
				pTemp->entity.angles[0] += pTemp->entity.baseline.angles[0] * frametime;
				pTemp->entity.angles[1] += pTemp->entity.baseline.angles[1] * frametime;
				pTemp->entity.angles[2] += pTemp->entity.baseline.angles[2] * frametime;
			}

			if (pTemp->flags & FTENT_COLLIDEWORLD)
			{
				trace_t t;
				t.fraction = 1.0;
				t.allsolid = TRUE;

				SV_RecursiveHullCheck(cl.worldmodel->hulls, cl.worldmodel->hulls[0].firstclipnode,
				  0.0,
				  1.0,
				  pTemp->entity.prevorigin,
				  pTemp->entity.origin, &t);

				if (t.fraction != 1.0)
				{
					float damp, proj;

					VectorMA(pTemp->entity.prevorigin, t.fraction * frametime, pTemp->entity.baseline.origin, pTemp->entity.origin);
					damp = 1;
					if (pTemp->flags & (FTENT_GRAVITY | FTENT_SLOWGRAVITY))
					{
						damp = 0.5;
						if (t.plane.normal[2] > 0.9) //Hit floor?
						{
							if (pTemp->entity.baseline.origin[2] <= 0 && pTemp->entity.baseline.origin[2] >= gravity*3)
							{
								damp = 0;
								pTemp->flags &= ~(FTENT_ROTATE | FTENT_GRAVITY | FTENT_SLOWGRAVITY | FTENT_COLLIDEWORLD | FTENT_SMOKETRAIL);
								pTemp->entity.angles[0] = 0;
								pTemp->entity.angles[2] = 0;
							}
						}
					}
					if (pTemp->hitSound)
						CL_TempEntPlaySound(pTemp, damp);
					// Reflect velocity
					if (damp != 0)
					{
						proj = DotProduct(pTemp->entity.baseline.origin, t.plane.normal);
						VectorMA(pTemp->entity.baseline.origin, -proj * 2, t.plane.normal, pTemp->entity.baseline.origin);
						// Reflect rotation (fake)

						pTemp->entity.angles[1] = -pTemp->entity.angles[1];
					}

					if (damp != 1)
					{

						VectorScale(pTemp->entity.baseline.origin, damp, pTemp->entity.baseline.origin);
						VectorScale(pTemp->entity.angles, 0.9, pTemp->entity.angles);
					}
				}
			}

			if (pTemp->flags & FTENT_FLICKER && pTemp->entity.effects == gTempEntFrame)
			{
				dlight_t* dl = CL_AllocDlight(0);
				dl->origin[0] = pTemp->entity.origin[0];
				dl->origin[1] = pTemp->entity.origin[1];
				dl->origin[2] = pTemp->entity.origin[2];
				dl->radius = 60.0;
				dl->color.r = 255;
				dl->color.g = 120;
				dl->color.b = 0;
				// die on next frame
				dl->die = cl.time + 0.01;
			}

			if (pTemp->flags & FTENT_SMOKETRAIL)
			{
				R_RocketTrail(pTemp->entity.prevorigin, pTemp->entity.origin, 1);
			}

			if (pTemp->flags & FTENT_GRAVITY)
				pTemp->entity.baseline.origin[2] += gravity;
			else if (pTemp->flags & FTENT_SLOWGRAVITY)
				pTemp->entity.baseline.origin[2] += gravitySlow;

			// Cull to PVS (not frustum cull, just PVS)
			if (!CL_AddVisibleEntity(&pTemp->entity))
			{
				pTemp->die = cl.time; // If we can't draw it this frame, just dump it.
				pTemp->flags &= ~FTENT_FADEOUT; // Don't fade out, just die
			}
		}

		pTemp = pnext;
	}
}

/*
================
CL_FxBlend

================
*/
int CL_FxBlend( cl_entity_t* ent )
{
	int blend = 0;
	float offset;

	offset = (int)ent * 363.0;

	switch (ent->renderfx)
	{
	case kRenderFxPulseSlow:
		blend = ent->renderamt + sin(cl.time * 2.0 + offset) * 16.0;
		break;
	case kRenderFxPulseFast:
		blend = ent->renderamt + sin(cl.time * 8.0 + offset) * 16.0;
		break;
	case kRenderFxPulseSlowWide:
		blend = ent->renderamt + sin(cl.time * 2.0 + offset) * 64.0;
		break;
	case kRenderFxPulseFastWide:
		blend = ent->renderamt + sin(cl.time * 8.0 + offset) * 64.0;
		break;
	case kRenderFxFadeSlow:
		if (ent->renderamt > 0)
			ent->renderamt -= 1;
		else
			ent->renderamt = 0;
			
		blend = ent->renderamt;
		break;
	case kRenderFxFadeFast:
		if (ent->renderamt > 3)
			ent->renderamt -= 4;
		else
			ent->renderamt = 0;
			
		blend = ent->renderamt;
		break;
	case kRenderFxSolidSlow:
		if (ent->renderamt < 255)
			ent->renderamt += 1;	
		else
			ent->renderamt = 255;

		blend = ent->renderamt;
		break;
	case kRenderFxSolidFast:
		if (ent->renderamt < 252)
			ent->renderamt += 4;
		else
			ent->renderamt = 255;

		blend = ent->renderamt;
		break;
	case kRenderFxStrobeSlow:
		blend = sin(cl.time * 4.0 + offset) * 20.0;
		if (blend < 0)
			blend = 0;
		else
			blend = ent->renderamt;		
		break;
	case kRenderFxStrobeFast:
		blend = sin(cl.time * 16.0 + offset) * 20.0;
		if (blend < 0)
			blend = 0;
		else
			blend = ent->renderamt;
		break;

	case kRenderFxStrobeFaster:
		blend = sin(cl.time * 36.0 + offset) * 20.0;
		if (blend < 0)
			blend = 0;
		else
			blend = ent->renderamt;
		break;

	case kRenderFxFlickerSlow:
		blend = sin(cl.time * 17.0 + offset) + sin(cl.time * 2.0) * 20.0;
		if (blend < 0)
			blend = 0;
		else
			blend = ent->renderamt;
		break;

	case kRenderFxFlickerFast:
		blend = sin(cl.time * 23.0 + offset) + sin(cl.time * 16.0) * 20.0;
		if (blend < 0)
			blend = 0;
		else
			blend = ent->renderamt;
		break;
	case kRenderFxDistort:
	case kRenderFxHologram:
		{
			vec3_t tmp;
			VectorSubtract(ent->origin, r_origin, tmp);

			float dist = DotProduct(vpn, tmp);
			if (ent->renderfx == kRenderFxDistort)
				dist = 1.0;

			if (dist > 0.0)
			{
				ent->renderamt = 180;

				if (dist > 100)
				{
					blend = RandomLong(-32, 31) + (1 - (dist - 100) * 0.0025) * 180;
				}
				else
				{
					blend = RandomLong(-32, 31) + 180;
				}
			}
			else
			{
				blend = 0;
			}
		}
		break;
	default:
		blend = ent->renderamt;
		break;
	}

	// clamp it
	if (blend > 255)
		blend = 255;
	else if (blend < 0)
		blend = 0;

	return blend;
}

/*
=============================================================

  SPRITE MODELS

=============================================================
*/

/*
================
R_GetSpriteFrame
================
*/
mspriteframe_t* R_GetSpriteFrame( msprite_t* pSprite, int frame )
{
	mspriteframe_t* pspriteframe;

	// Invalid frame
	if ((frame >= pSprite->numframes) || (frame < 0))
	{
		Con_Printf("Sprite: no such frame %d\n", frame);
		frame = 0;
	}

	if (pSprite->frames[frame].type == SPR_SINGLE)
	{
		pspriteframe = pSprite->frames[frame].frameptr;
	}
	else
	{
		pspriteframe = NULL;
	}

	return pspriteframe;
}