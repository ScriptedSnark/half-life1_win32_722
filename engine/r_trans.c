// r_trans.c - transparent objects

#include "quakedef.h"
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

// TODO: Implement

/*
=================
AddTEntity

Add a transparent entity to a list of transparent objects
=================
*/
void AddTEntity( cl_entity_t* pEnt )
{
	// TODO: Implement
}

// TODO: Implement

#if defined( GLQUAKE )
float r_blend;	// blending amount in [0..1] range

/*
=============
R_DrawTEntitiesOnList
=============
*/
void R_DrawTEntitiesOnList( void )
{
	// TODO: Implement
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