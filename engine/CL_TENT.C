// cl_tent.c -- client side temporary entities

#include "quakedef.h"
#include "cl_tent.h"
#include "pr_cmds.h"
#include "decal.h"
#include "r_trans.h"
#include "r_efx.h"

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

#define SHARD_VOLUME 12.0	// on shard ever n^3 units

/*
===============
R_BreakModel

Create model shattering shards
===============
*/
void R_BreakModel( float* pos, float* size, float* dir, float random, float life, int count, int modelIndex, char flags )
{
	int					i, frameCount;
	TEMPENTITY* pTemp;
	model_t* pModel;
	char				type;

	if (!modelIndex)
		return;

	type = flags & BREAK_TYPEMASK;

	pModel = cl.model_precache[modelIndex];
	if (!pModel)
		return;

	frameCount = ModelFrameCount(pModel);

	if (count == 0)
		// assume surface (not volume)
		count = (size[0] * size[1] + size[1] * size[2] + size[2] * size[0]) / (3 * SHARD_VOLUME * SHARD_VOLUME);

	for (i = 0; i < count; i++)
	{
		vec3_t vecSpot;

		// fill up the box with stuff
		
		vecSpot[0] = pos[0] + RandomFloat(-0.5, 0.5) * size[0];
		vecSpot[1] = pos[1] + RandomFloat(-0.5, 0.5) * size[1];
		vecSpot[2] = pos[2] + RandomFloat(-0.5, 0.5) * size[2];

		pTemp = CL_TempEntAlloc(vecSpot, pModel);
		if (!pTemp)
			break;

		// keep track of break_type, so we know how to play sound on collision
		pTemp->hitSound = type;

		if (pModel->type == mod_sprite)
			pTemp->entity.frame = RandomLong(0, frameCount - 1);
		else if (pModel->type == mod_studio)
			pTemp->entity.body = RandomLong(0, frameCount - 1);

		pTemp->flags |= FTENT_COLLIDEWORLD | FTENT_FADEOUT | FTENT_SLOWGRAVITY;

		if (RandomLong(0, 255) < 200)
		{
			pTemp->flags |= FTENT_ROTATE;
			pTemp->entity.baseline.angles[0] = RandomFloat(-256, 255);
			pTemp->entity.baseline.angles[1] = RandomFloat(-256, 255);
			pTemp->entity.baseline.angles[2] = RandomFloat(-256, 255);
		}

		if ((RandomLong(0, 255) < 100) && (flags & BREAK_SMOKE))
			pTemp->flags |= FTENT_SMOKETRAIL;

		if ((type == BREAK_GLASS) || (flags & BREAK_TRANS))
		{
			pTemp->entity.rendermode = kRenderTransTexture;
			pTemp->entity.renderamt = 128;
			pTemp->entity.baseline.renderamt = 128;
		}
		else
		{
			pTemp->entity.rendermode = kRenderNormal;
			pTemp->entity.baseline.renderamt = 255;		// Set this for fadeout
		}

		pTemp->entity.baseline.origin[0] = dir[0] + RandomFloat(-random, random);
		pTemp->entity.baseline.origin[1] = dir[1] + RandomFloat(-random, random);
		pTemp->entity.baseline.origin[2] = dir[2] + RandomFloat(0, random);

		pTemp->die = cl.time + life + RandomFloat(0, 1);	// Add an extra 0-1 secs of life
	}
}

/*
====================
R_TempSprite

Create sprite TE
====================
*/
TEMPENTITY* R_TempSprite( float* pos, float* dir, float scale, int modelIndex, int rendermode, int renderfx, float a, float life, int flags )
{
	TEMPENTITY* pTemp;
	model_t* model;
	int					frameCount;

	if (!modelIndex)
		return NULL;

	model = cl.model_precache[modelIndex];
	if (!model)
	{
		Con_Printf("No model %d!\n", modelIndex);
		return NULL;
	}

	frameCount = ModelFrameCount(model);

	pTemp = CL_TempEntAlloc(pos, model);
	if (!pTemp)
		return NULL;
	
	pTemp->frameMax = frameCount;
	pTemp->entity.framerate = 10;
	pTemp->entity.rendermode = rendermode;
	pTemp->entity.renderfx = renderfx;
	pTemp->entity.scale = scale;
	pTemp->entity.baseline.renderamt = a * 255;
	pTemp->entity.rendercolor.r = 255;
	pTemp->entity.rendercolor.g = 255;
	pTemp->entity.rendercolor.b = 255;
	pTemp->entity.renderamt = a * 255;

	pTemp->flags |= flags;

	VectorCopy(dir, pTemp->entity.baseline.origin);
	VectorCopy(pos, pTemp->entity.origin);
	if (life)
		pTemp->die = cl.time + life;
	else
		pTemp->die = cl.time + (frameCount * 0.1) + 1;

	pTemp->entity.frame = 0;
	return pTemp;
}

/*
===============
R_Sprite_Spray

Spray sprite
===============
*/
void R_Sprite_Spray( vec_t* pos, vec_t* dir, int modelIndex, int count, int speed, int iRand )
{
	TEMPENTITY* pTemp;
	model_t* pModel;
	float				noise;
	float				znoise;
	int					frameCount;
	int					i;

	noise = (float)iRand / 100;

	// more vertical displacement
	znoise = noise * 1.5;

	if (znoise > 1)
	{
		znoise = 1;
	}

	pModel = cl.model_precache[modelIndex];
	if (!pModel)
	{
		Con_Printf("No model %d!\n", modelIndex);
		return;
	}

	frameCount = ModelFrameCount(pModel) - 1;

	for (i = 0; i < count; i++)
	{
		pTemp = CL_TempEntAlloc(pos, pModel);
		if (!pTemp)
			return;

		pTemp->entity.rendermode = kRenderTransAlpha;
		pTemp->entity.renderamt = 255;
		pTemp->entity.baseline.renderamt = 255;
		pTemp->entity.renderfx = kRenderFxNoDissipation;
		pTemp->entity.scale = 0.5;
		pTemp->flags |= FTENT_FADEOUT | FTENT_SLOWGRAVITY;
		pTemp->fadeSpeed = 2.0;

		// make the spittle fly the direction indicated, but mix in some noise.
		pTemp->entity.baseline.origin[0] = dir[0] + RandomFloat(-noise, noise);
		pTemp->entity.baseline.origin[1] = dir[1] + RandomFloat(-noise, noise);
		pTemp->entity.baseline.origin[2] = dir[2] + RandomFloat(0, znoise);
		VectorScale(pTemp->entity.baseline.origin, RandomFloat((speed * 0.8), (speed * 1.2)), pTemp->entity.baseline.origin);

		VectorCopy(pos, pTemp->entity.origin);

		pTemp->die = cl.time + 0.35;

		pTemp->entity.frame = RandomLong(0, frameCount);
	}
}

void R_SparkEffect( float* pos, int count, int velocityMin, int velocityMax )
{
	R_SparkStreaks(pos, count, velocityMin, velocityMax);
	R_RicochetSprite(pos, cl_sprite_ricochet, 0.1, RandomFloat(0.5, 1.0));
}

void R_FunnelSprite( float* org, int modelIndex, int reverse )
{
	TEMPENTITY* pTemp;
	int				i, j;
	float			flDist, vel;
	vec3_t			dir, dest;
	model_t* model;
	int				frameCount;

	if (!modelIndex)
	{
		Con_Printf("No modelindex for funnel!!\n");
		return;
	}

	model = cl.model_precache[modelIndex];
	if (!model)
	{
		Con_Printf("No model %d!\n", modelIndex);
		return;
	}

	frameCount = ModelFrameCount(model);

	for (i = -256; i < 256; i += 32)
	{
		for (j = -256; j < 256; j += 32)
		{
			pTemp = CL_TempEntAlloc(org, model);
			if (!pTemp)
				return;

			if (reverse)
			{
				VectorCopy(org, pTemp->entity.origin);

				dest[0] = org[0] + i;
				dest[1] = org[1] + j;
				dest[2] = org[2] + RandomFloat(100, 800);

				// send particle heading to dest at a random speed
				VectorSubtract(dest, pTemp->entity.origin, dir);

				vel = dest[2] / 8;// velocity based on how far particle has to travel away from org
			}
			else
			{
				pTemp->entity.origin[0] = org[0] + i;
				pTemp->entity.origin[1] = org[1] + j;
				pTemp->entity.origin[2] = org[2] + RandomFloat(100, 800);

				// send particle heading to org at a random speed
				VectorSubtract(org, pTemp->entity.origin, dir);

				vel = pTemp->entity.origin[2] / 8;// velocity based on how far particle starts from org
			}

			pTemp->entity.framerate = 10;
			pTemp->entity.rendermode = kRenderGlow;
			pTemp->entity.renderfx = kRenderFxNoDissipation;
			pTemp->entity.renderamt = 200;
			pTemp->entity.baseline.renderamt = 200;

			pTemp->frameMax = frameCount;
			pTemp->entity.scale = RandomFloat(0.1f, 0.4f);
			pTemp->flags = FTENT_ROTATE | FTENT_FADEOUT;

			flDist = VectorNormalize(dir);	// save the distance

			if (vel < 64)
			{
				vel = 64;
			}

			vel += RandomFloat(64, 128);
			VectorScale(dir, vel, pTemp->entity.baseline.origin);

			pTemp->fadeSpeed = 2.0;
			pTemp->die = cl.time + (flDist / vel) - 0.5;
		}
	}
}

/*
=================
R_RicochetSprite

Create ricochet sprite
=================
*/
void R_RicochetSprite( float* pos, model_t* pmodel, float duration, float scale )
{
	TEMPENTITY* pTemp;

	pTemp = CL_TempEntAlloc(pos, pmodel);
	if (!pTemp)
		return;

	pTemp->entity.rendermode = kRenderGlow;
	pTemp->entity.renderfx = kRenderFxNoDissipation;
	pTemp->entity.renderamt = 200;
	pTemp->entity.baseline.renderamt = 200;
	pTemp->entity.scale = scale;
	pTemp->flags = FTENT_FADEOUT;

	VectorClear(pTemp->entity.baseline.origin);

	VectorCopy(pos, pTemp->entity.origin);

	pTemp->fadeSpeed = 8;
	pTemp->die = cl.time;

	pTemp->entity.frame = 0;
	pTemp->entity.angles[ROLL] = (45 * RandomLong(0, 7));
}

/*
===============
R_RocketFlare

===============
*/
void R_RocketFlare( float* pos )
{
	TEMPENTITY* pTemp;
	int					frameCount;

	if (cl.time == cl.oldtime)
		return;

	frameCount = ModelFrameCount(cl_sprite_glow);

	pTemp = CL_TempEntAlloc(pos, cl_sprite_glow);
	if (!pTemp)
		return;

	pTemp->frameMax = frameCount;
	pTemp->entity.rendermode = kRenderGlow;
	pTemp->entity.renderfx = kRenderFxNoDissipation;
	pTemp->entity.renderamt = 255;
	pTemp->entity.scale = 1.0;
	pTemp->entity.frame = RandomLong(0, frameCount - 1);
	VectorCopy(pos, pTemp->entity.origin);
	pTemp->die = cl.time + 0.01;
}

/*
===============
R_BloodSprite

Create blood sprite
===============
*/
void R_BloodSprite( vec_t* org, int colorindex, int modelIndex, int modelIndex2, float size )
{
	int				i, splatter;
	TEMPENTITY* pTemp;
	model_t* model;
	model_t* model2;
	int				frameCount, frameCount2;
	unsigned int	impactindex;
	unsigned int	spatterindex;
	color24			impactcolor;
	color24			splattercolor;

	impactindex = 4 * (colorindex + RandomLong(1, 3));
	spatterindex = 4 * (colorindex + RandomLong(1, 3)) + 4;

	impactcolor.r = host_basepal[impactindex + 2];
	impactcolor.g = host_basepal[impactindex + 1];
	impactcolor.b = host_basepal[impactindex + 0];

	splattercolor.r = host_basepal[spatterindex + 2];
	splattercolor.g = host_basepal[spatterindex + 1];
	splattercolor.b = host_basepal[spatterindex + 0];

	//Validate the model first
	if (modelIndex2 && (model2 = cl.model_precache[modelIndex2]))
	{
		frameCount2 = ModelFrameCount(model2);

		// Random amount of drips
		splatter = size + RandomLong(1, 8) + RandomLong(1, 8);

		for (i = 0; i < splatter; i++)
		{
			// Make some blood drips
			if (pTemp = CL_TempEntAlloc(org, model2))
			{
				pTemp->entity.rendermode = kRenderNormal;
				pTemp->entity.renderfx = kRenderFxNone;
				pTemp->entity.scale = RandomFloat(size / 15, size / 25);
				pTemp->flags = FTENT_ROTATE | FTENT_SLOWGRAVITY | FTENT_COLLIDEWORLD;

				pTemp->entity.rendercolor = splattercolor;
				pTemp->entity.baseline.renderamt = 250;
				pTemp->entity.renderamt = 250;

				pTemp->entity.baseline.origin[0] = RandomFloat(-96, 95);
				pTemp->entity.baseline.origin[1] = RandomFloat(-96, 95);
				pTemp->entity.baseline.origin[2] = RandomFloat(-32, 95);

				pTemp->entity.baseline.angles[0] = RandomFloat(-256, -255);
				pTemp->entity.baseline.angles[1] = RandomFloat(-256, -255);
				pTemp->entity.baseline.angles[2] = RandomFloat(-256, -255);

				pTemp->entity.framerate = 0;
				pTemp->die = cl.time + RandomFloat(1, 2);

				pTemp->entity.frame = RandomLong(1, frameCount2 - 1);
				if (pTemp->entity.frame > 8)
					pTemp->entity.frame = frameCount2 - 1;

				pTemp->entity.angles[ROLL] = RandomLong(0, 360);
			}
		}
	}

	// Impact particle
	if (modelIndex && (model = cl.model_precache[modelIndex]))
	{
		frameCount = ModelFrameCount(model);

		//Large, single blood sprite is a high-priority tent
		if (pTemp = CL_TempEntAlloc(org, model))
		{
			pTemp->entity.rendermode = kRenderNormal;
			pTemp->entity.renderfx = kRenderFxNone;
			pTemp->entity.scale = RandomFloat(size / 25, size / 35);
			pTemp->flags = FTENT_SPRANIMATE;

			pTemp->entity.rendercolor = impactcolor;
			pTemp->entity.baseline.renderamt = 250;
			pTemp->entity.renderamt = 250;

			VectorClear(pTemp->entity.baseline.origin);

			pTemp->entity.framerate = frameCount * 4; // Finish in 0.250 seconds
			pTemp->die = cl.time + (frameCount / pTemp->entity.framerate); // Play the whole thing Once

			pTemp->entity.frame = 0;
			pTemp->frameMax = frameCount;
			pTemp->entity.angles[ROLL] = RandomLong(0, 360);
		}
	}
}

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

/*
===============
R_MuzzleFlash

Play muzzle flash
===============
*/
void R_MuzzleFlash( float* pos1, int rand, int type )
{
	TEMPENTITY* pTemp;
	int index;
	float scale;
	int			frameCount;

	index = (type % 10) % 3;
	scale = (type / 10) * 0.1;
	if (scale == 0)
		scale = 0.5;

	frameCount = ModelFrameCount(cl_sprite_muzzleflash[index]);

	//Con_DPrintf("%d %f\n", index, scale);
	pTemp = CL_TempEntAlloc(pos1, cl_sprite_muzzleflash[index]);
	if (!pTemp)
		return;	
	pTemp->entity.rendermode = kRenderTransAdd;
	pTemp->entity.renderamt = 255;
	pTemp->entity.renderfx = kRenderFxNone;
	pTemp->entity.rendercolor.r = 255;
	pTemp->entity.rendercolor.g = 255;
	pTemp->entity.rendercolor.b = 255;
	VectorCopy(pos1, pTemp->entity.origin);
	pTemp->die = cl.time + 0.01;
	pTemp->entity.frame = RandomLong(0, frameCount - 1);
	pTemp->frameMax = frameCount;
	VectorCopy(vec3_origin, pTemp->entity.angles);

	pTemp->entity.scale = scale;

	if (index == 0)
	{
		// Rifle flash
		pTemp->entity.angles[ROLL] = RandomLong(0, 20);
	}
	else
	{
		pTemp->entity.angles[ROLL] = RandomLong(0, 359);
	}

	AppendTEntity(&pTemp->entity);
}

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
	int		speed;
	int		color;
	int		iRand;
	int		entnumber;
	int		modelindex, modelindex2;
	vec3_t	pos;
	vec3_t	size;
	vec3_t	dir;
	vec3_t	endpos;
	int		colorStart, colorLength;
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

		switch (type)
		{
		case TE_BEAMENTS:
			R_BeamEnts(startEnt, endEnt, modelindex, life, width, amplitude, a, flSpeed, startFrame, frameRate, r, g, b);
			break;
		case TE_BEAMENTPOINT:
			R_BeamEntPoint(startEnt, endpos, modelindex, life, width, amplitude, a, flSpeed, startFrame, frameRate, r, g, b);
			break;
		case TE_BEAMPOINTS:
			R_BeamPoints(pos, endpos, modelindex, life, width, amplitude, a, flSpeed, startFrame, frameRate, r, g, b);
			break;
		}
		break;
	}

	case TE_GUNSHOT:			// bullet hitting wall
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();
		R_RunParticleEffect(pos, vec3_origin, 0, 20);

		iRand = RandomLong(0, 0x7FFF);
		if (iRand < 0x3FFF)
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
		break;

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

			R_FlickerParticles(pos);

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

	case TE_TAREXPLOSION:			// Quake1 "tarbaby" explosion with sound
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();
		R_BlobExplosion(pos);
		S_StartDynamicSound(-1, CHAN_AUTO, cl_sfx_r_exp1, pos, VOL_NORM, 1.0, 0, PITCH_NORM);
		break;

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

	case TE_LIGHTNING:				// lightning bolts
	{
		float width;
		float amplitude;

		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();
		endpos[0] = MSG_ReadCoord();
		endpos[1] = MSG_ReadCoord();
		endpos[2] = MSG_ReadCoord();
		life = MSG_ReadByte() * 0.1;
		width = MSG_ReadByte() * 0.1;
		amplitude = MSG_ReadByte() * 0.01;
		modelindex = MSG_ReadShort();
		R_BeamLightning(pos, endpos, modelindex, life, width, amplitude, 0.6, 3.5);
		break;
	}

	case TE_SPARKS:
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();
		R_SparkEffect(pos, 8, -200, 200);
		break;

	case TE_LAVASPLASH:
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();
		R_LavaSplash(pos);
		break;

	case TE_TELEPORT:
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();
		R_TeleportSplash(pos);
		break;

	case TE_EXPLOSION2:
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();

		colorStart = MSG_ReadByte();
		colorLength = MSG_ReadByte();
		R_ParticleExplosion2(pos, colorStart, colorLength);

		// spawn some dynamic light
		dl = CL_AllocDlight(0);
		VectorCopy(pos, dl->origin);
		dl->radius = 350;
		dl->decay = 300;
		dl->die = cl.time + 0.5;
		S_StartDynamicSound(-1, CHAN_AUTO, cl_sfx_r_exp1, pos, 1.0, 0.6f, 0, 100);
		break;

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

	case TE_IMPLOSION:
	{
		float radius;

		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();
		radius = MSG_ReadByte();
		count = MSG_ReadByte();
		life = MSG_ReadByte() / 10.0;
		R_Implosion(pos, radius, count, life);
		break;
	}
	
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

	case TE_SPRITE:
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();
		modelindex = MSG_ReadShort();
		scale = MSG_ReadByte() * 0.1;
		a = MSG_ReadByte() / 255.0;
		R_TempSprite(pos, vec3_origin, scale, modelindex, kRenderTransAdd, kRenderFxNone, a, 0, FTENT_SPRANIMATE);
		break;

	case TE_BEAMSPRITE:
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();

		endpos[0] = MSG_ReadCoord();
		endpos[1] = MSG_ReadCoord();
		endpos[2] = MSG_ReadCoord();

		modelindex = MSG_ReadShort();	// beam modelindex
		modelindex2 = MSG_ReadShort();	// sprite modelindex

		R_BeamPoints(pos, endpos, modelindex, 0.01, 0.4, 0, RandomFloat(0.5, 0.655), 5, 0, 0, 1, 0, 0);
		R_TempSprite(endpos, vec3_origin, 0.1, modelindex2, kRenderTransAdd, kRenderFxNone, 0.35, 0.01, 0);
		break;

	case TE_BEAMTORUS:
	case TE_BEAMDISK:
	case TE_BEAMCYLINDER:
	{
		float width;
		float amplitude;

		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();

		endpos[0] = MSG_ReadCoord();
		endpos[1] = MSG_ReadCoord();
		endpos[2] = MSG_ReadCoord();

		modelindex = MSG_ReadShort();
		startFrame = MSG_ReadByte();
		frameRate = MSG_ReadByte() * 0.1;
		life = MSG_ReadByte() * 0.1;
		width = MSG_ReadByte();
		amplitude = MSG_ReadByte() * 0.01;

		r = MSG_ReadByte() / 255.0;
		g = MSG_ReadByte() / 255.0;
		b = MSG_ReadByte() / 255.0;
		a = MSG_ReadByte() / 225.0;

		flSpeed = MSG_ReadByte() * 0.1;

		R_BeamCirclePoints(type, pos, endpos, modelindex, life, width, amplitude, a, flSpeed, startFrame, frameRate, r, g, b);
		break;
	}

	case TE_BEAMFOLLOW:
	{
		float width;

		startEnt = MSG_ReadShort();
		modelindex = MSG_ReadShort();

		life = MSG_ReadByte() * 0.1;
		width = MSG_ReadByte();

		r = MSG_ReadByte() / 255.0;
		g = MSG_ReadByte() / 255.0;
		b = MSG_ReadByte() / 255.0;
		a = MSG_ReadByte() / 255.0;

		R_BeamFollow(startEnt, modelindex, life, width, r, g, b, a);
		break;
	}

	case TE_GLOWSPRITE:
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();

		modelindex = MSG_ReadShort();
		life = MSG_ReadByte() * 0.1;
		scale = MSG_ReadByte() * 0.1;
		a = MSG_ReadByte() / 255.0;

		R_TempSprite(pos, vec3_origin, scale, modelindex, kRenderGlow, kRenderFxNoDissipation, a, life, FTENT_FADEOUT);
		break;

		// TODO: Implement

	case TE_STREAK_SPLASH:
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();

		dir[0] = MSG_ReadCoord();
		dir[1] = MSG_ReadCoord();
		dir[2] = MSG_ReadCoord();

		color = MSG_ReadByte();
		count = MSG_ReadShort();
		speed = MSG_ReadShort();
		iRand = MSG_ReadShort();
		R_StreakSplash(pos, dir, color, count, speed, -iRand, iRand);
		break;

	case TE_DLIGHT:
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();

		dl = CL_AllocDlight(0);
		VectorCopy(pos, dl->origin);
		dl->radius = MSG_ReadByte() * 10.0;
		dl->color.r = MSG_ReadByte();
		dl->color.g = MSG_ReadByte();
		dl->color.b = MSG_ReadByte();
		dl->die = cl.time + MSG_ReadByte() * 0.1;
		dl->decay = MSG_ReadByte() * 10.0;
		break;

	case TE_ELIGHT:
	{
		float flTime;

		dl = CL_AllocElight(MSG_ReadShort());
		dl->origin[0] = MSG_ReadCoord();
		dl->origin[1] = MSG_ReadCoord();
		dl->origin[2] = MSG_ReadCoord();
		dl->radius = MSG_ReadCoord();
		dl->color.r = MSG_ReadByte();
		dl->color.g = MSG_ReadByte();
		dl->color.b = MSG_ReadByte();

		life = MSG_ReadByte() * 0.1;
		dl->die = cl.time + life;
		flTime = MSG_ReadCoord();
		if (life != 0)
			flTime /= life;

		dl->decay = flTime;
		break;
	}

	case TE_KILLBEAM:
		entnumber = MSG_ReadShort();
		R_BeamKill(entnumber);
		break;

	case TE_LARGEFUNNEL:
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();
		modelindex = MSG_ReadShort();
		flags = MSG_ReadShort();
		R_LargeFunnel(pos, flags);
		R_FunnelSprite(pos, modelindex, flags);
		break;

	case TE_BLOODSTREAM:
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();
		dir[0] = MSG_ReadCoord();
		dir[1] = MSG_ReadCoord();
		dir[2] = MSG_ReadCoord();
		color = MSG_ReadByte();
		speed = MSG_ReadByte();
		R_BloodStream(pos, dir, color, speed);
		break;
		
	case TE_SHOWLINE:
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();
		endpos[0] = MSG_ReadCoord();
		endpos[1] = MSG_ReadCoord();
		endpos[2] = MSG_ReadCoord();
		R_ShowLine(pos, endpos);
		break;

	case TE_BLOOD:
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();
		dir[0] = MSG_ReadCoord();
		dir[1] = MSG_ReadCoord();
		dir[2] = MSG_ReadCoord();
		color = MSG_ReadByte();
		speed = MSG_ReadByte();
		R_Blood(pos, dir, color, speed);
		break;

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

		R_TempModel(pos, dir, endpos, life, modelindex, soundtype);
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
		R_BreakModel(pos, size, dir, frandom, life, count, modelindex, c);
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

		iRand = RandomLong(0, 0x7FFF);
		if (iRand < 0x3FFF)
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

	case TE_SPRITE_SPRAY:
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();
		dir[0] = MSG_ReadCoord();
		dir[1] = MSG_ReadCoord();
		dir[2] = MSG_ReadCoord();
		modelindex = MSG_ReadShort();
		count = MSG_ReadByte();
		speed = MSG_ReadByte();
		iRand = MSG_ReadByte();
		R_Sprite_Spray(pos, dir, modelindex, count, speed * 2, iRand);
		break;

	case TE_ARMOR_RICOCHET:
		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();
		scale = MSG_ReadByte() * 0.1;
		R_RicochetSprite(pos, cl_sprite_ricochet, 0.1, scale);

		switch (RandomLong(0, 4))
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
		break;
		
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

	case TE_BLOODSPRITE:
	{
		float fsize;

		pos[0] = MSG_ReadCoord();
		pos[1] = MSG_ReadCoord();
		pos[2] = MSG_ReadCoord();
		modelindex = MSG_ReadShort();
		modelindex2 = MSG_ReadShort();
		color = MSG_ReadByte();
		fsize = MSG_ReadByte();
		R_BloodSprite(pos, color, modelindex, modelindex2, fsize);
		break;
	}

	default:
		Sys_Error("CL_ParseTEnt: bad type");
		break;
	}
}

/*
=================
R_TempModel

Create some simple physically simulated models
=================
*/
TEMPENTITY* R_TempModel( float* pos, float* dir, float* angles, float life, int modelIndex, int soundtype )
{
	TEMPENTITY* pTemp;
	model_t* model;
	int			frameCount;

	if (!modelIndex)
		return NULL;

	model = cl.model_precache[modelIndex];
	if (!model)
	{
		Con_Printf("No model %d!\n", modelIndex);
		return NULL;
	}

	frameCount = ModelFrameCount(model);

	pTemp = CL_TempEntAlloc(pos, model);
	if (!pTemp)
		return NULL;

	VectorCopy(angles, pTemp->entity.angles);

	pTemp->frameMax = frameCount;
	pTemp->flags |= (FTENT_COLLIDEWORLD | FTENT_GRAVITY);

	// keep track of shell type
	switch (soundtype)
	{
	case TE_BOUNCE_NULL:
		pTemp->hitSound = 0;
		break;
	case TE_BOUNCE_SHELL:
		pTemp->hitSound = BOUNCE_SHELL;
		pTemp->entity.baseline.angles[0] = RandomFloat(-512, 511);
		pTemp->entity.baseline.angles[1] = RandomFloat(-256, 255);
		pTemp->entity.baseline.angles[2] = RandomFloat(-256, 255);
		pTemp->flags |= FTENT_ROTATE;
		break;
	case TE_BOUNCE_SHOTSHELL:
		pTemp->hitSound = BOUNCE_SHOTSHELL;
		pTemp->entity.baseline.angles[0] = RandomFloat(-512, 511);
		pTemp->entity.baseline.angles[1] = RandomFloat(-256, 255);
		pTemp->entity.baseline.angles[2] = RandomFloat(-256, 255);
		pTemp->flags |= (FTENT_ROTATE | FTENT_SLOWGRAVITY);
		break;
	}

	VectorCopy(dir, pTemp->entity.baseline.origin);

	pTemp->die = cl.time + life;

	// is the model a sprite?
	if (model->type == mod_sprite)
	{
		pTemp->entity.frame = RandomLong(0, frameCount - 1);
	}
	else
	{
		pTemp->entity.body = RandomLong(0, frameCount - 1);
	}

	return pTemp;
}

/*
=================
CL_TempEntInit

Initialize temp entities
=================
*/
void CL_TempEntInit( void )
{
	memset(gTempEnts, 0, sizeof(gTempEnts));

	int i;

	for (i = 0; i < MAX_TEMP_ENTITIES; i++)
	{
		gTempEnts[i].next = &gTempEnts[i + 1];
	}
	gTempEnts[MAX_TEMP_ENTITIES - 1].next = NULL;
	gpTempEntFree = gTempEnts;
	gpTempEntActive = NULL;
}

/*
=================
CL_TempEntAlloc

Allocate temp entity
=================
*/
TEMPENTITY* CL_TempEntAlloc( vec_t* org, model_t* model )
{
	TEMPENTITY* pTemp;

	if (!gpTempEntFree || !model)
	{
		Con_DPrintf("Overflow %d temporary ents!\n", MAX_TEMP_ENTITIES);
		return NULL;
	}

	pTemp = gpTempEntFree;
	gpTempEntFree = pTemp->next;

	memset(&pTemp->entity, 0, sizeof(pTemp->entity));

	// Use these to set per-frame and termination conditions / actions
	pTemp->flags = FTENT_NONE;
	pTemp->entity.colormap = (byte*)vid.colormap;
	pTemp->die = cl.time + 0.75;
	pTemp->entity.model = model;
	pTemp->entity.rendermode = kRenderNormal;
	pTemp->entity.renderfx = kRenderFxNone;
	pTemp->fadeSpeed = 0.5;
	pTemp->hitSound = 0;

	VectorCopy(org, pTemp->entity.origin);

	pTemp->next = gpTempEntActive;
	gpTempEntActive = pTemp;

	return pTemp;
}

/*
=================
CL_TempEntPlaySound

Play sound when temp ent collides with something
=================
*/
void CL_TempEntPlaySound( TEMPENTITY* pTemp, float damp )
{
	sfx_t* rgsfx[6];
	int i;
	float fvol;
	int fShell = FALSE;
	int zvel;

	Q_memset(rgsfx, 0, sizeof(rgsfx));

	switch (pTemp->hitSound)
	{
	default:
		return;	// null sound

	case BOUNCE_GLASS:
		i = 3;
		fvol = 0.8f;
		rgsfx[0] = cl_sfx_glass1;
		rgsfx[1] = cl_sfx_glass2;
		rgsfx[2] = cl_sfx_glass3;
		break;

	case BOUNCE_METAL:
		i = 3;
		fvol = 0.8f;
		rgsfx[0] = cl_sfx_metal1;
		rgsfx[1] = cl_sfx_metal2;
		rgsfx[2] = cl_sfx_metal3;
		break;

	case BOUNCE_FLESH:
		i = 6;
		fvol = 0.8f;
		rgsfx[0] = cl_sfx_flesh1;
		rgsfx[1] = cl_sfx_flesh2;
		rgsfx[2] = cl_sfx_flesh3;
		rgsfx[3] = cl_sfx_flesh4;
		rgsfx[4] = cl_sfx_flesh5;
		rgsfx[5] = cl_sfx_flesh6;
		break;

	case BOUNCE_WOOD:
		i = 3;
		fvol = 0.8f;
		rgsfx[0] = cl_sfx_wood1;
		rgsfx[1] = cl_sfx_wood2;
		rgsfx[2] = cl_sfx_wood3;
		break;

	case BOUNCE_SHRAP:
		i = 5;
		fvol = 0.8f;
		rgsfx[0] = cl_sfx_ric1;
		rgsfx[1] = cl_sfx_ric2;
		rgsfx[2] = cl_sfx_ric3;
		rgsfx[3] = cl_sfx_ric4;
		rgsfx[4] = cl_sfx_ric5;
		break;

	case BOUNCE_SHELL:
		i = 3;
		fvol = 0.8f;
		rgsfx[0] = cl_sfx_pl_shell1;
		rgsfx[1] = cl_sfx_pl_shell2;
		rgsfx[2] = cl_sfx_pl_shell3;
		fShell = TRUE; // shell casings have different playback parameters
		break;

	case BOUNCE_CONCRETE:
		i = 3;
		fvol = 0.8f;
		rgsfx[0] = cl_sfx_concrete1;
		rgsfx[1] = cl_sfx_concrete2;
		rgsfx[2] = cl_sfx_concrete3;
		break;

	case BOUNCE_SHOTSHELL:
		i = 3;
		fvol = 0.5f;
		rgsfx[0] = cl_sfx_sshell1;
		rgsfx[1] = cl_sfx_sshell2;
		rgsfx[2] = cl_sfx_sshell3;
		fShell = TRUE; // shell casings have different playback parameters
		break;
	}

	zvel = abs(pTemp->entity.baseline.origin[2]);

	// only play one out of every n

	if (fShell)
	{
		// play first bounce, then 1 out of 3
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
		int pitch;

		if (fShell)
		{
			fvol *= min(1.0, ((float)zvel) / 350.0);
		}
		else
		{
			fvol *= min(1.0, ((float)zvel) / 450.0);
		}

		if (!RandomLong(0, 3) && !fShell)
		{
			pitch = RandomLong(90, 124);
		}
		else
		{
			pitch = PITCH_NORM;
		}

		S_StartDynamicSound(-1, CHAN_AUTO, rgsfx[RandomLong(0, i - 1)], pTemp->entity.origin,
			fvol, 1.0, 0, pitch);
	}
}

/*
=============
CL_AddVisibleEntity

Adds a client entity to the list of visible entities if it's within the PVS
=============
*/
int CL_AddVisibleEntity( cl_entity_t* pEntity)
{
	int i;
	vec3_t mins, maxs;

	if (!pEntity->model)
		return 0;

	if (cl_numvisedicts >= MAX_VISEDICTS)
		return 0; // object list is full

	for (i = 0; i < 3; i++)
	{
		mins[i] = pEntity->origin[i] + pEntity->model->mins[i];
		maxs[i] = pEntity->origin[i] + pEntity->model->maxs[i];
	}

	// does the box intersect a visible leaf?
	if (PVSNode(cl.worldmodel->nodes, mins, maxs))
	{
		pEntity->index = -1;

		memcpy(&cl_visedicts[cl_numvisedicts], pEntity, sizeof(cl_entity_t));
		cl_numvisedicts++;

		return TRUE;
	}

	return FALSE;
}

/*
=============
CL_UpdateTEnts

Simulation and cleanup of temporary entities
=============
*/
void CL_TempEntUpdate( void )
{
	static int gTempEntFrame = 0;
	int			i;
	TEMPENTITY* pTemp, * pnext, * pprev;
	float		freq, gravity, gravitySlow, life, fastFreq;
	double		frametime;

	// Nothing to simulate
	if (!gpTempEntActive)
		return;

	// Don't simulate while loading
	if (!cl.worldmodel)
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

					// destroy it.
					pTemp->die = cl.time;
					pTemp = pnext;
					continue;
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
				trace_t trace;
				memset(&trace, 0, sizeof(trace));
				trace.fraction = 1.0;
				trace.allsolid = TRUE;
				SV_RecursiveHullCheck(cl.worldmodel->hulls, cl.worldmodel->hulls[0].firstclipnode, 0.0, 1.0,
					pTemp->entity.prevorigin, pTemp->entity.origin, &trace);

				if (trace.fraction != 1)	// Decent collision now, and damping works
				{
					float damp, proj;

					// Place at contact point
					VectorMA(pTemp->entity.prevorigin, trace.fraction * frametime, pTemp->entity.baseline.origin, pTemp->entity.origin);
					// Damp velocity
					damp = 1.0;
					if (pTemp->flags & (FTENT_GRAVITY | FTENT_SLOWGRAVITY))
					{
						damp = 0.5;
						if (trace.plane.normal[2] > 0.9)		// Hit floor?
						{
							if (pTemp->entity.baseline.origin[2] <= 0 && pTemp->entity.baseline.origin[2] >= gravity * 3)
							{
								damp = 0;
								pTemp->flags &= ~(FTENT_ROTATE | FTENT_GRAVITY | FTENT_SLOWGRAVITY | FTENT_COLLIDEWORLD | FTENT_SMOKETRAIL);
								pTemp->entity.angles[0] = 0;
								pTemp->entity.angles[2] = 0;
							}
						}
					}

					if (pTemp->hitSound)
					{
						CL_TempEntPlaySound(pTemp, damp);
					}

					// Reflect velocity
					if (damp != 0)
					{
						proj = DotProduct(pTemp->entity.baseline.origin, trace.plane.normal);
						VectorMA(pTemp->entity.baseline.origin, -proj * 2, trace.plane.normal, pTemp->entity.baseline.origin);
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

			if (pTemp->flags & FTENT_FLICKER && gTempEntFrame == pTemp->entity.effects)
			{
				dlight_t* dl = CL_AllocDlight(0);
				VectorCopy(pTemp->entity.origin, dl->origin);
				dl->radius = 60;
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
====================
CL_FxTransform

====================
*/
void CL_FxTransform( cl_entity_t* ent, float* transform )
{
	switch (ent->renderfx)
	{
		case kRenderFxDistort:
		case kRenderFxHologram:
			if (RandomLong(0, 49) == 0)
			{
				int axis = RandomLong(0, 1);
				if (axis == 1) // Choose between x & z
					axis = 2;
				VectorScale(&transform[axis * 4], RandomFloat(1, 1.484), &transform[axis * 4]);
			}
			else if (RandomLong(0, 49) == 0)
			{
				float offset;
				int axis = RandomLong(0, 1);
				if (axis == 1) // Choose between x & z
					axis = 2;
				offset = RandomFloat(-10, 10);
				transform[(RandomLong(0, 2) * 4) + 3] += offset;
			}
			break;
	}
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

/*
=================
R_GetAttachmentPoint

=================
*/
float* R_GetAttachmentPoint( int entity, int attachment )
{
	cl_entity_t* pEntity;

	pEntity = &cl_entities[entity];

	if (attachment)
		return pEntity->attachment[attachment - 1];

	return pEntity->origin;
}