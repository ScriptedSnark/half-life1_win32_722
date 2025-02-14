// r_trans.c - transparent objects

#include "quakedef.h"
#include "r_studio.h"
#include "r_trans.h"

qboolean r_intentities;

int max_translucent_objects;

typedef struct
{
    cl_entity_t* pEnt;
    float distance;
} transObjRef;

int numTransObjs;
int maxTransObjs;
transObjRef* transObjects;

void R_DrawAliasModel( cl_entity_t* e );
void R_DrawSpriteModel( cl_entity_t* e );
int R_BmodelCheckBBox( model_t* clmodel, float* minmaxs );

/*
=================
R_AllocObjects
=================
*/
void R_AllocObjects( int nMax )
{
	if (transObjects)
		Con_Printf("Transparent objects reallocate\n");

	transObjects = (transObjRef*)malloc(sizeof(transObjRef) * nMax);
	memset(transObjects, 0, sizeof(transObjRef) * nMax);

	maxTransObjs = nMax;
}

/*
=================
R_DestroyObjects

Release all transparent objects
=================
*/
void R_DestroyObjects( void )
{
	if (transObjects)
	{
		free(transObjects);
		transObjects = NULL;
	}

	maxTransObjs = 0;
}

float GlowBlend( cl_entity_t* pEntity )
{
	// TODO: Implement
    return 1.0;
}

/*
=================
AddTEntity

Add a transparent entity to a list of transparent objects
=================
*/
void AddTEntity( cl_entity_t* pEnt )
{
	int     i;
	float   dist;
	vec3_t  v;

	if (numTransObjs >= maxTransObjs)
		Sys_Error("AddTentity: Too many objects");

	if (!pEnt->model || pEnt->model->type != mod_brush || pEnt->rendermode != kRenderTransAlpha)
	{
		VectorAdd(pEnt->model->maxs, pEnt->model->mins, v);
		VectorScale(v, 0.5, v);
		VectorAdd(v, pEnt->origin, v);
		VectorSubtract(r_origin, v, v);

		dist = DotProduct(v, v);
	}
	else
	{
		// max distance
		dist = 1E9F;
	}

	i = numTransObjs;
	while (i > 0)
	{
		if (transObjects[i - 1].distance >= dist)
			break;

		transObjects[i].pEnt = transObjects[i - 1].pEnt;
		transObjects[i].distance = transObjects[i - 1].distance;
		i--;
	}

	transObjects[i].pEnt = pEnt;
	transObjects[i].distance = dist;
	numTransObjs++;
}

/*
=================
AppendTEntity

Append a transparent entity to a list of transparent objects
=================
*/
void AppendTEntity( cl_entity_t* pEnt )
{
	float   dist;
	vec3_t  v;

	if (numTransObjs >= maxTransObjs)
		Sys_Error("AddTentity: Too many objects");

	VectorAdd(pEnt->model->mins, pEnt->model->maxs, v);
	VectorScale(v, 0.5, v);
	VectorAdd(v, pEnt->origin, v);
	VectorSubtract(r_origin, v, v);

	dist = DotProduct(v, v);

	transObjects[numTransObjs].pEnt = pEnt;
	transObjects[numTransObjs].distance = dist;
	numTransObjs++;
}

#if defined( GLQUAKE )
float r_blend;	// blending amount in [0..1] range

/*
=============
R_DrawTEntitiesOnList
=============
*/
void R_DrawTEntitiesOnList( void )
{
	int     i;

	if (!r_drawentities.value)
		return;

	// Handle all trans objects in the list
	for (i = 0; i < numTransObjs; i++)
	{
		currententity = transObjects[i].pEnt;

		r_blend = CL_FxBlend(currententity);
		if (r_blend <= 0.0)
			continue;

		r_blend /= 255.0;

		// Glow is only for sprite models
		if (currententity->rendermode == kRenderGlow && currententity->model->type != mod_sprite)
			Con_Printf("Non-sprite set to glow!\n");

		switch (currententity->model->type)
		{
		case mod_brush:
			R_DrawBrushModel(currententity);
			break;

		case mod_sprite:
			if (currententity->body)
			{
				float* pAttachment;

				pAttachment = R_GetAttachmentPoint(currententity->skin, currententity->body);
				VectorCopy(pAttachment, r_entorigin);
			}
			else
			{
				VectorCopy(currententity->origin, r_entorigin);
			}

			// Glow sprite
			if (currententity->rendermode == kRenderGlow)
			{
				r_blend *= GlowBlend(currententity);
			}

			if (r_blend != 0.0)
			{
				R_DrawSpriteModel(currententity);
			}
			break;

		// TODO: Implement

		case mod_studio:
			R_StudioDrawModel(STUDIO_RENDER | STUDIO_EVENTS);
			break;
		}
	}

	numTransObjs = 0;
	r_blend = 1.0;
}
#else
int r_blend;	// blending amount in [0..255] range

// TODO: Implement

/*
=============
R_DrawTEntitiesOnList
=============
*/
void R_DrawTEntitiesOnList( void )
{
	// TODO: Implement
}
#endif