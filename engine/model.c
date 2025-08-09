// model.c -- model loading and caching

// models are the only shared resource between a client and server running
// on the same machine.

#include "quakedef.h"

char loadname[32];

void SW_Mod_Init( void )
{
}

/*
===============
Mod_Init

Caches the data if needed
===============
*/
void* Mod_Extradata( model_t* mod )
{
	// TODO: Implement
	return NULL;
}

/*
===============
Mod_PointInLeaf
===============
*/
mleaf_t* Mod_PointInLeaf( vec_t* p, model_t* model )
{
	// TODO: Implement
	return NULL;
}

/*
===================
Mod_ClearAll
===================
*/
void Mod_ClearAll( void )
{
	// TODO: Implement
}

/*
==================
Mod_LoadModel

Loads a model into the cache
==================
*/
model_t* Mod_LoadModel( model_t* mod, qboolean crash )
{
	// TODO: Implement
	return NULL;
}

/*
==================
Mod_ForName

Loads in a model for the given name
==================
*/
model_t* Mod_ForName( char* name, qboolean crash )
{
	// TODO: Implement
	return NULL;
}

void Mod_MarkClient( model_t* pModel )
{
	// TODO: Implement
}




//=============================================================================

/*
================
Mod_Print
================
*/
void Mod_Print( void )
{
	// TODO: Implement
}