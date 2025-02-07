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

/*
===============
R_FizzEffect

Create a fizz effect
===============
*/
void R_FizzEffect( cl_entity_t* pent, int modelIndex, int density )
{
	TEMPENTITY* pTemp;
	model_t* model;
	int				i, width, depth, count, frameCount;
	float			maxHeight, speed, xspeed, yspeed;
	vec3_t			origin;

	if (!pent->model || !modelIndex)
		return;

	model = cl.model_precache[modelIndex];
	if (!model)
		return;

	count = density + 1;

	maxHeight = pent->model->maxs[2] - pent->model->mins[2];
	width = pent->model->maxs[0] - pent->model->mins[0];
	depth = pent->model->maxs[1] - pent->model->mins[1];
	speed = (float)pent->rendercolor.r * 256.0 + (float)pent->rendercolor.g;
	if (pent->rendercolor.b)
		speed = -speed;

	xspeed = cos(pent->angles[YAW] * M_PI / 180) * speed;
	yspeed = sin(pent->angles[YAW] * M_PI / 180) * speed;

	frameCount = ModelFrameCount(model);

	for (i = 0; i < count; i++)
	{
		origin[0] = pent->model->mins[0] + RandomLong(0, width - 1);
		origin[1] = pent->model->mins[1] + RandomLong(0, depth - 1);
		origin[2] = pent->model->mins[2];
		pTemp = CL_TempEntAlloc(origin, model);
		if (!pTemp)
			return;

		pTemp->flags |= FTENT_SINEWAVE;

		pTemp->x = origin[0];
		pTemp->y = origin[1];

		float zspeed = RandomLong(80, 140);
		pTemp->entity.baseline.origin[0] = xspeed;
		pTemp->entity.baseline.origin[1] = yspeed;
		pTemp->entity.baseline.origin[2] = zspeed;
		pTemp->die = cl.time + (maxHeight / zspeed) - 0.1;
		pTemp->entity.frame = RandomLong(0, frameCount - 1);
		// Set sprite scale
		pTemp->entity.scale = 1.0 / RandomFloat(2, 5);
		pTemp->entity.rendermode = kRenderTransAlpha;
		pTemp->entity.renderamt = 255;
	}
}

/*
===============
R_Bubbles

Create bubbles
===============
*/
void R_Bubbles( vec_t* mins, vec_t* maxs, float height, int modelIndex, int count, float speed )
{
	TEMPENTITY* pTemp;
	model_t* model;
	int					i, frameCount;
	float				angle;
	vec3_t				origin;

	if (!modelIndex)
		return;

	model = cl.model_precache[modelIndex];
	if (!model)
		return;

	frameCount = ModelFrameCount(model);

	for (i = 0; i < count; i++)
	{
		origin[0] = RandomLong(mins[0], maxs[0]);
		origin[1] = RandomLong(mins[1], maxs[1]);
		origin[2] = RandomLong(mins[2], maxs[2]);
		pTemp = CL_TempEntAlloc(origin, model);
		if (!pTemp)
			return;

		pTemp->flags |= FTENT_SINEWAVE;

		pTemp->x = origin[0];
		pTemp->y = origin[1];

		angle = RandomLong(-3, 3);

		pTemp->entity.baseline.origin[0] = cos(angle) * speed;
		pTemp->entity.baseline.origin[1] = sin(angle) * speed;
		pTemp->entity.baseline.origin[2] = RandomLong(80, 140);
		pTemp->die = cl.time + ((height - (origin[2] - mins[2])) / pTemp->entity.baseline.origin[2]) - 0.1;
		pTemp->entity.frame = RandomLong(0, frameCount - 1);

		// Set sprite scale
		pTemp->entity.scale = 1.0 / RandomFloat(2, 5);
		pTemp->entity.rendermode = kRenderTransAlpha;
		pTemp->entity.renderamt = 255;
	}
}

/*
===============
R_BubbleTrail

Create bubble trail
===============
*/
void R_BubbleTrail( vec_t* start, vec_t* end, float height, int modelIndex, int count, float speed )
{
	TEMPENTITY* pTemp;
	model_t* model;
	int					i, frameCount;
	float				dist, angle;
	vec3_t				origin;

	if (!modelIndex)
		return;

	model = cl.model_precache[modelIndex];
	if (!model)
		return;

	frameCount = ModelFrameCount(model);

	for (i = 0; i < count; i++)
	{
		dist = RandomFloat(0, 1.0);
		origin[0] = start[0] + dist * (end[0] - start[0]);
		origin[1] = start[1] + dist * (end[1] - start[1]);
		origin[2] = start[2] + dist * (end[2] - start[2]);
		pTemp = CL_TempEntAlloc(origin, model);
		if (!pTemp)
			return;

		pTemp->flags |= FTENT_SINEWAVE;

		pTemp->x = origin[0];
		pTemp->y = origin[1];
		angle = RandomLong(-3, 3);

		float zspeed = RandomLong(80, 140);
		pTemp->entity.baseline.origin[0] = speed * cos(angle);
		pTemp->entity.baseline.origin[1] = speed * sin(angle);
		pTemp->entity.baseline.origin[2] = zspeed;
		pTemp->die = cl.time + ((height - (origin[2] - start[2])) / zspeed) - 0.1;
		pTemp->entity.frame = RandomLong(0, frameCount - 1);

		// Set sprite scale
		pTemp->entity.scale = 1.0 / RandomFloat(2, 5);
		pTemp->entity.rendermode = kRenderTransAlpha;
		pTemp->entity.renderamt = 255;
	}
}

/*
===============
R_Sprite_Trail

Line of moving glow sprites with gravity, fadeout, and collisions
===============
*/
void R_Sprite_Trail( int type, vec_t* start, vec_t* end, int modelIndex, int count, float life, float size, float amplitude, int renderamt, float speed )
{
	TEMPENTITY* ptemp;
	model_t* model;
	int				i, frameCount;
	vec3_t			delta, pos, dir;

	model = cl.model_precache[modelIndex];
	if (!model)
		return;

	frameCount = ModelFrameCount(model);

	VectorSubtract(end, start, delta);
	VectorCopy(delta, dir);
	VectorNormalize(dir);

	amplitude /= 256.0;

	for (i = 0; i < count; i++)
	{
		float scale = (float)i / ((float)count - 1.0);
		VectorMA(start, scale, delta, pos);

		ptemp = CL_TempEntAlloc(pos, model);
		if (!ptemp)
			return;

		ptemp->flags |= (FTENT_COLLIDEWORLD | FTENT_SPRCYCLE | FTENT_FADEOUT | FTENT_SLOWGRAVITY);

		VectorScale(dir, speed, ptemp->entity.baseline.origin);

		ptemp->entity.baseline.origin[0] += RandomLong(-127, 128) * amplitude;
		ptemp->entity.baseline.origin[1] += RandomLong(-127, 128) * amplitude;
		ptemp->entity.baseline.origin[2] += RandomLong(-127, 128) * amplitude;

		ptemp->entity.rendermode = kRenderGlow;
		ptemp->entity.renderfx = kRenderFxNoDissipation;
		ptemp->entity.renderamt = renderamt;
		ptemp->entity.baseline.renderamt = renderamt;
		ptemp->entity.scale = size;
		
		ptemp->entity.frame = RandomLong(0, frameCount - 1);
		ptemp->frameMax = frameCount;
		ptemp->die = cl.time + life + RandomFloat(0, 4);

		ptemp->entity.rendercolor.r = 255;
		ptemp->entity.rendercolor.g = 255;
		ptemp->entity.rendercolor.b = 255;
	}
}

/*
===============
R_TempSphereModel

Spherical shower of models, picks from set
===============
*/
void R_TempSphereModel( float* pos, float speed, float life, int count, int modelIndex )
{
	int					i, frameCount;
	TEMPENTITY* pTemp;
	model_t* model;

	if (!modelIndex)
		return;

	model = cl.model_precache[modelIndex];
	if (!model)
		return;

	frameCount = ModelFrameCount(model);

	// Create temporary models
	for (i = 0; i < count; i++)
	{
		pTemp = CL_TempEntAlloc(pos, model);
		if (!pTemp)
			return;

		pTemp->entity.body = RandomLong(0, frameCount - 1);

		if (RandomLong(0, 255) < 10)
			pTemp->flags |= FTENT_SLOWGRAVITY;
		else
			pTemp->flags |= FTENT_GRAVITY;

		if (RandomLong(0, 255) < 220)
		{
			pTemp->flags |= FTENT_ROTATE;
			pTemp->entity.baseline.angles[0] = RandomFloat(-256, -255);
			pTemp->entity.baseline.angles[1] = RandomFloat(-256, -255);
			pTemp->entity.baseline.angles[2] = RandomFloat(-256, -255);
		}

		if (RandomLong(0, 255) < 100)
		{
			pTemp->flags |= FTENT_SMOKETRAIL;
		}

		pTemp->flags |= FTENT_FLICKER | FTENT_COLLIDEWORLD;

		pTemp->entity.effects = i & 0x1F;
		pTemp->entity.rendermode = kRenderNormal;

		pTemp->entity.baseline.origin[0] = RandomFloat(-1, 1);
		pTemp->entity.baseline.origin[1] = RandomFloat(-1, 1);
		pTemp->entity.baseline.origin[2] = RandomFloat(-1, 1);

		VectorNormalize(pTemp->entity.baseline.origin);
		VectorScale(pTemp->entity.baseline.origin, speed, pTemp->entity.baseline.origin);

		pTemp->die = cl.time + life;
	}
}




//============================================================================================== <- FINISH LINE


// TODO: Implement

/*
=================
R_DefaultSprite

Create default sprite TE
=================
*/
TEMPENTITY* R_DefaultSprite( float* pos, int spriteIndex, float framerate )
{
	TEMPENTITY* pTemp;
	int				frameCount;
	model_t* pSprite;

	// don't spawn while paused
	if (cl.time == cl.oldtime)
		return NULL;

	pSprite = cl.model_precache[spriteIndex];

	if (!spriteIndex || !pSprite || pSprite->type != mod_sprite)
	{
		Con_DPrintf("No Sprite %d!\n", spriteIndex);
		return NULL;
	}

	frameCount = ModelFrameCount(cl.model_precache[spriteIndex]);

	pTemp = CL_TempEntAlloc(pos, pSprite);
	if (!pTemp)
		return NULL;

	pTemp->entity.scale = 1.0;
	pTemp->frameMax = frameCount;
	pTemp->flags |= FTENT_SPRANIMATE;
	if (framerate == 0)
		framerate = 10;

	VectorCopy(pos, pTemp->entity.origin);

	pTemp->entity.framerate = framerate;
	pTemp->entity.frame = 0;
	pTemp->die = cl.time + (float)frameCount / framerate;
	
	return pTemp;
}

/*
===============
R_Sprite_Smoke

Create sprite smoke
===============
*/
void R_Sprite_Smoke( TEMPENTITY* pTemp, float scale )
{
	if (!pTemp)
		return;

	pTemp->entity.rendermode = kRenderTransAlpha;	
	pTemp->entity.renderfx = kRenderFxNone;
	pTemp->entity.renderamt = 255;
	pTemp->entity.baseline.origin[2] = 30;
	int iColor = RandomLong(20, 35);
	pTemp->entity.rendercolor.r = iColor;
	pTemp->entity.rendercolor.g = iColor;
	pTemp->entity.rendercolor.b = iColor;
	pTemp->entity.origin[2] += 20;
	pTemp->entity.scale = scale;
}

/*
===============
R_Sprite_Explode

Create explosion sprite
===============
*/
void R_Sprite_Explode( TEMPENTITY* pTemp, float scale )
{
	if (!pTemp)
		return;

	pTemp->entity.rendermode = kRenderTransAdd;
	pTemp->entity.renderfx = kRenderFxNone;
	pTemp->entity.rendercolor.r = 0;
	pTemp->entity.rendercolor.g = 0;
	pTemp->entity.rendercolor.b = 0;
	pTemp->entity.scale = scale;
	pTemp->entity.origin[2] += 10;
	pTemp->entity.renderamt = 180;

	pTemp->entity.baseline.origin[2] = 8.0;

	if (RandomLong(0, 1))
	{
		pTemp->entity.angles[ROLL] = 180.0;
	}
}

/*
===============
R_Sprite_WallPuff

Create a wallpuff
===============
*/
void R_Sprite_WallPuff( TEMPENTITY* pTemp, float scale )
{
	if (!pTemp)
		return;

	pTemp->entity.rendermode = kRenderTransAlpha;
	pTemp->entity.renderamt = 255;
	pTemp->entity.rendercolor.r = 0;
	pTemp->entity.rendercolor.g = 0;
	pTemp->entity.rendercolor.b = 0;
	pTemp->entity.renderfx = kRenderFxNone;
	pTemp->entity.scale = scale;
	pTemp->entity.frame = 0;
	pTemp->entity.angles[ROLL] = RandomLong(0, 359);
	pTemp->die = cl.time + 0.01;
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
	float	flSpeed;
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
			// sprite
			R_Sprite_Explode(R_DefaultSprite(pos, modelindex, frameRate), scale);

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
		R_Sprite_Smoke(R_DefaultSprite(pos, modelindex, frameRate), scale);
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
	
	case TE_SPRITETRAIL:
	{
		float amplitude;

		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();

		endpos[0] = MSG_ReadCoord();
		endpos[1] = MSG_ReadCoord();
		endpos[2] = MSG_ReadCoord();

		modelindex = MSG_ReadShort();
		count = MSG_ReadByte();

		life = MSG_ReadByte() * 0.1;

		scale = MSG_ReadByte();
		if (scale == 0.0)
			scale = 1.0;
		else
			scale *= 0.1;

		amplitude = MSG_ReadByte() * 10.0;
		flSpeed = MSG_ReadByte() * 10.0;

		R_Sprite_Trail(type, pos, endpos, modelindex, count, life, scale, amplitude, 255, flSpeed);
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

	case TE_FIZZ:
	{
		int density;

		entnumber = MSG_ReadShort();
		if (entnumber >= cl.max_edicts)
			Sys_Error("Bubble: entity = %i", entnumber);

		modelindex = MSG_ReadShort();
		density = MSG_ReadByte();
		R_FizzEffect(&cl_entities[entnumber], modelindex, density);
		break;
	}

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

	case TE_EXPLODEMODEL:
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();

		flSpeed = MSG_ReadCoord();

		modelindex = MSG_ReadShort();
		count = MSG_ReadShort();
		life = MSG_ReadByte() * 0.1;

		R_TempSphereModel(pos, flSpeed, life, count, modelindex);
		break;
		
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
		
	case TE_PLAYERDECAL:
	{
		static int decalTextureIndex;
		int playernum;
		customization_t* pCust = NULL;
		texture_t* pTexture = NULL;

		flags = (type == TE_BSPDECAL) ? FDECAL_PERMANENT : 0;

		playernum = MSG_ReadByte() - 1;
		if (playernum < MAX_CLIENTS)
			pCust = cl.players[playernum].customdata.pNext;

		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();

		entnumber = MSG_ReadShort();
		decalTextureIndex = MSG_ReadByte();

		if (entnumber && type == TE_BSPDECAL)
		{
			modelindex = MSG_ReadShort();
		}
		else
		{
			modelindex = 0;
		}

		if (entnumber >= cl.max_edicts)
			Sys_Error("Decal: entity = %i", entnumber);

		if (pCust && pCust->pBuffer)
		{
			cachewad_t* pwad;

			pwad = (cachewad_t*)pCust->pInfo;
			if (pwad)
			{
				if ((pCust->resource.ucFlags & RES_CUSTOM) && pCust->resource.type == t_decal && pCust->bTranslated)
				{
					if (decalTextureIndex > pwad->lumpCount - 1)
						decalTextureIndex = pwad->lumpCount - 1;

					pCust->nUserData1 = decalTextureIndex;
					pTexture = (texture_t*)Draw_CustomCacheGet(pwad, pCust->pBuffer, decalTextureIndex);
				}
			}
		}

		if (r_decals.value)
		{
			if (pCust && pTexture)
			{
				R_CustomDecalShoot(pTexture, playernum, entnumber, modelindex, pos, flags);
			}
			else
			{
				R_DecalShoot(Draw_DecalIndex(decalTextureIndex), entnumber, modelindex, pos, flags);
			}
		}
		break;
	}

	case TE_BUBBLES:
	{
		float height;

		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();

		endpos[0] = MSG_ReadCoord();
		endpos[1] = MSG_ReadCoord();
		endpos[2] = MSG_ReadCoord();

		height = MSG_ReadCoord();
		modelindex = MSG_ReadShort();
		count = MSG_ReadByte();
		flSpeed = MSG_ReadCoord();
		R_Bubbles(pos, endpos, height, modelindex, count, flSpeed);
		break;
	}

	case TE_BUBBLETRAIL:
	{
		float height;

		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();

		endpos[0] = MSG_ReadCoord();
		endpos[1] = MSG_ReadCoord();
		endpos[2] = MSG_ReadCoord();

		height = MSG_ReadCoord();
		modelindex = MSG_ReadShort();
		count = MSG_ReadByte();
		flSpeed = MSG_ReadCoord();
		R_BubbleTrail(pos, endpos, height, modelindex, count, flSpeed);
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

/*
=================
R_GetSpriteAxes

Determine sprite orientation axes
=================
*/
void R_GetSpriteAxes( cl_entity_t* pEntity, int type, vec_t* forward, vec_t* right, vec_t* up )
{
	int i;
	float dot;
	float angle;
	float sr, cr;
	vec3_t tvec;

	// Automatically roll parallel sprites if requested
	if (pEntity->angles[2] != 0 && type == SPR_VP_PARALLEL)
	{
		type = SPR_VP_PARALLEL_ORIENTED;
	}

	switch (type)
	{
		case SPR_FACING_UPRIGHT:
		{
			// generate the sprite's axes, with r_vup straight up in worldspace.
			// This will not work if the view direction is very close to straight up or
			// down, because the cross product will be between two nearly parallel
			// vectors and starts to approach an undefined state, so we don't draw if
			// the two vectors are less than 1 degree apart
			tvec[0] = -modelorg[0];
			tvec[1] = -modelorg[1];
			tvec[2] = -modelorg[2];
			VectorNormalize(tvec);
			dot = tvec[2];
			if (dot > 0.999848 || dot < -0.999848)	// cos(1 degree) = 0.999848
				return;
			up[0] = 0;
			up[1] = 0;
			up[2] = 1;
			right[0] = tvec[1];
			right[1] = -tvec[0];
			right[2] = 0;
			VectorNormalize(right);
			forward[0] = -right[1];
			forward[1] = right[0];
			forward[2] = 0;
		}
		break;

		case SPR_VP_PARALLEL:
		{
			// generate the sprite's axes, completely parallel to the viewplane. There
			// are no problem situations, because the sprite is always in the same
			// position relative to the viewer
			for (i = 0; i < 3; i++)
			{
				up[i] = vup[i];
				right[i] = vright[i];
				forward[i] = vpn[i];
			}
		}
		break;

		case SPR_VP_PARALLEL_UPRIGHT:
		{
			// generate the sprite's axes, with vup straight up in worldspace.
			// This will not work if the view direction is very close to straight up or
			// down, because the cross product will be between two nearly parallel
			// vectors and starts to approach an undefined state, so we don't draw if
			// the two vectors are less than 1 degree apart
			dot = vpn[2];
			if (dot > 0.999848 || dot < -0.999848)
				return;
			up[0] = 0;
			up[1] = 0;
			up[2] = 1;
			right[0] = vpn[1];
			right[1] = -vpn[0];
			right[2] = 0;
			VectorNormalize(right);
			forward[0] = -right[1];
			forward[1] = right[0];
			forward[2] = 0;
		}
		break;

		case SPR_ORIENTED:
		{
			// generate the sprite's axes, according to the sprite's world orientation
			AngleVectors(pEntity->angles, forward, right, up);
		}
		break;

		case SPR_VP_PARALLEL_ORIENTED:
		{
			// generate the sprite's axes, parallel to the viewplane, but rotated in
			// that plane around the center according to the sprite entity's roll
			// angle. So vpn stays the same, but vright and vup rotate
			angle = currententity->angles[ROLL] * (M_PI * 2 / 360.0);
			sr = sin(angle);
			cr = cos(angle);

			for (i = 0; i < 3; i++)
			{
				forward[i] = vpn[i];
				right[i] = vright[i] * cr + vup[i] * sr;
				up[i] = vright[i] * -sr + vup[i] * cr;
			}
		}
		break;

		default:
			Sys_Error("R_DrawSprite: Bad sprite type %d", type);
			break;
	}
}

/*
=================
R_SpriteColor

=================
*/
void R_SpriteColor( colorVec* pColor, cl_entity_t* pEntity, int alpha )
{
	int a;

	if ((pEntity->rendermode == kRenderTransAdd) || (pEntity->rendermode == kRenderGlow))
	{
		a = alpha;
	}
	else
	{
		a = 256;
	}

	if ((pEntity->rendercolor.r == 0) && (pEntity->rendercolor.g == 0) && (pEntity->rendercolor.b == 0))
	{
		pColor->r = pColor->g = pColor->b = (255 * a) / 256;
	}
	else
	{
		pColor->r = (pEntity->rendercolor.r * a) / 256;
		pColor->g = (pEntity->rendercolor.g * a) / 256;
		pColor->b = (pEntity->rendercolor.b * a) / 256;
	}
}