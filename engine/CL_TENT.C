// cl_tent.c -- client side temporary entities

#include "quakedef.h"
#include "cl_tent.h"
#include "pr_cmds.h"
#include "decal.h"

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

			// Never remove it
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

		// TODO: Implement
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

		// TODO: Implement
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

Allocate temp entity ( normal/low priority )
=================
*/
TEMPENTITY* CL_TempEntAlloc( vec_t* org, model_t* model )
{
	// TODO: Implement
	return NULL;
}

// TODO: Implement

void CL_TempEntUpdate( void )
{
	// TODO: Implement
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