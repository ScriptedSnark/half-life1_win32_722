#include "quakedef.h"
#include "server.h"

#include "world.h"

edict_t* EDICT_NUM( int n )
{
	if (n < 0 ||
		n >= sv.max_edicts)
		Sys_Error("EDICT_NUM: bad number %i", n);
	return &sv.edicts[n];
}

int NUM_FOR_EDICT( const edict_t* e )
{
	int		b;

	b = e - sv.edicts;

	if (b < 0 || b >= sv.num_edicts)
		Sys_Error("NUM_FOR_EDICT: bad pointer");
	return b;
}

// TODO: Implement

/*
================
ReleaseEntityDLLFields

================
*/
void FreeEntPrivateData( edict_t* ent );
void ReleaseEntityDLLFields( edict_t* ent )
{
	FreeEntPrivateData(ent);
}

/*
================
InitEntityDLLFields

================
*/
void InitEntityDLLFields( edict_t* ent )
{
	ent->v.pContainingEntity = ent;
}

/*
================
FreeEntPrivateData

================
*/
void FreeEntPrivateData( edict_t* ent )
{
	if (ent->pvPrivateData)
		free(ent->pvPrivateData);

	ent->pvPrivateData = NULL;
}

/*
=================
ED_ClearEdict

Sets everything to NULL
=================
*/
void ED_ClearEdict( edict_t* e )
{
	memset(&e->v, 0, sizeof(e->v));
	e->free = FALSE;
	ReleaseEntityDLLFields(e);
	InitEntityDLLFields(e);
}

/*
=================
ED_Alloc

Either finds a free edict, or allocates a new one.
Try to avoid reusing an entity that was recently freed, because it
can cause the client to think the entity morphed into something else
instead of being removed and recreated, which can cause interpolated
angles and bad trails.
=================
*/
edict_t* ED_Alloc( void )
{
	int			i;
	edict_t		*e;

	if (sv.num_edicts <= svs.maxclients + 1)
	{
		if (!sv.max_edicts)
			Sys_Error("ED_Alloc: No edicts yet");

		Sys_Error("ED_Alloc: no free edicts");

		sv.num_edicts++;

		e = &sv.edicts[svs.maxclients + 1];
		ED_ClearEdict(e);

		return e;
	}

	for (i = svs.maxclients + 1; i < sv.num_edicts; i++)
	{
		if (sv.num_edicts <= i + 1)
		{
			if (sv.num_edicts <= svs.maxclients + 1)
			{
				if (!sv.max_edicts)
					Sys_Error("ED_Alloc: No edicts yet");

				Sys_Error("ED_Alloc: no free edicts");
			}
		}

		e = &sv.edicts[i];
		// the first couple seconds of server time can involve a lot of
		// freeing and allocating, so relax the replacement policy
		if (e->free && (e->freetime < 2 || sv.time - e->freetime > 0.5))
		{
			ED_ClearEdict(e);
			return e;
		}
	}

	sv.num_edicts++;
	e = &sv.edicts[i];
	ED_ClearEdict(e);

	return e;
}

/*
=================
ED_Free

Marks the edict as free
FIXME: walk all entities and NULL out references to this entity
=================
*/
void ED_Free( edict_t* ed )
{
	if (ed->free)
		return; // allready freed

	SV_UnlinkEdict(ed);		// unlink from world bsp

	ed->free = TRUE;
	ed->serialnumber++;
	ed->v.flags = 0;
	ed->v.model = 0;
	ed->v.takedamage = 0;
	ed->v.modelindex = 0;
	ed->v.colormap = 0;
	ed->v.skin = 0;
	ed->v.frame = 0;
	ed->v.scale = 0;
	ed->v.gravity = 0;
	VectorCopy(vec3_origin, ed->v.origin);
	VectorCopy(vec3_origin, ed->v.angles);
	ed->v.solid = 0;
	ed->v.nextthink = -1;

	ed->freetime = sv.time;
}

// TODO: Implement

/*
====================
ED_ParseEdict

Parses an edict out of the given string, returning the new position
ed should be a properly initialized empty edict.
Used for initial level load and for savegames.
====================
*/
char *ED_ParseEdict( char *data, edict_t *ent )
{
	// TODO: Implement

	return NULL;
}

/*
================
ED_LoadFromFile

The entities are directly placed in the array, rather than allocated with
ED_Alloc, because otherwise an error loading the map would have entity
number references out of order.

Creates a server's entity / program execution context by
parsing textual entity definitions out of an ent file.

Used for both fresh maps and savegame loads.  A fresh map would also need
to call ED_CallSpawnFunctions () to let the objects initialize themselves.
================
*/
void ED_LoadFromFile( char* data )
{
	edict_t		*ent;
	int			inhibit;

	ent = NULL;
	inhibit = 0;

	gGlobalVariables.time = sv.time;

	// parse ents
	while (1)
	{
// parse the opening brace	
		data = COM_Parse(data);
		if (!data)
			break;
		if (com_token[0] != '{')
			Sys_Error("ED_LoadFromFile: found %s when expecting {", com_token);

		if (!ent)
		{
			ent = &sv.edicts[0];
			ReleaseEntityDLLFields(ent);
			InitEntityDLLFields(ent);
		}
		else
		{
			ent = ED_Alloc();
		}
		data = ED_ParseEdict(data, ent);

		if (ent->free)
			continue; // parse failed?

// remove things from different skill levels or deathmatch
		if (deathmatch.value)
		{
			if (((int)ent->v.spawnflags & SF_DECAL_NOTINDEATHMATCH))
			{
				ED_Free(ent);
				inhibit++;
				continue;
			}
		}

//
// immediately call spawn function
//
		if (!ent->v.classname)
		{
			Con_Printf("No classname for:\n");
			ED_Free(ent);
			continue;
		}

		// kill instantly if the appropriate flag is set
		if (gEntityInterface.pfnSpawn(ent) < 0 || (ent->v.flags & FL_KILLME) != 0)
			ED_Free(ent);

		SV_FlushSignon();
	}

	Con_DPrintf("%i entities inhibited\n", inhibit);
}