// sv_edict.c -- entity dictionary

#include "quakedef.h"
#include "world.h"
#include "pr_edict.h"

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
	edict_t* e;

	for (i = svs.maxclients + 1; i < sv.num_edicts; i++)
	{
		e = &sv.edicts[i];
		// the first couple seconds of server time can involve a lot of
		// freeing and allocating, so relax the replacement policy
		if (e->free && (e->freetime < 2 || sv.time - e->freetime > 0.5))
		{
			ED_ClearEdict(e);
			return e;
		}
	}

	if (i >= sv.max_edicts)
	{
		if (sv.max_edicts == 0)
			Sys_Error("ED_Alloc: No edicts yet");
		Sys_Error("ED_Alloc: no free edicts");
	}

	sv.num_edicts++;
	e = sv.edicts + i;
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

	// release the DLL entity that's attached to this edict, if any
	FreeEntPrivateData(ed);

	ed->free = TRUE;

	ed->serialnumber++;

	ed->v.flags = 0;
	ed->v.model = 0;
	ed->v.takedamage = DAMAGE_NO;
	ed->v.modelindex = 0;
	ed->v.colormap = 0;
	ed->v.skin = 0;
	ed->v.frame = 0;
	ed->v.scale = 0;
	ed->v.gravity = 0;
	VectorCopy(vec3_origin, ed->v.origin);
	VectorCopy(vec3_origin, ed->v.angles);
	ed->v.nextthink = -1;
	ed->v.solid = SOLID_NOT;

	ed->freetime = sv.time;
}

//===========================================================================

/*
eval_t *GetEdictFieldValue( edict_t *ed, char *field )
{
	ddef_t			*def = NULL;
	int				i;
	static int		rep = 0;

	for (i = 0; i < GEFV_CACHESIZE; i++)
	{
		if (!strcmp(field, gefvCache[i].field))
		{
			def = gefvCache[i].pcache;
			goto Done;
		}
	}

	def = ED_FindField(field);

	if (strlen(field) < MAX_FIELD_LEN)
	{
		gefvCache[rep].pcache = def;
		strcpy(gefvCache[rep].field, field);
		rep ^= 1;
	}

Done:
	if (!def)
		return NULL;

	return (eval_t*)((char*)&ed->v + def->ofs * 4);
}*/

/*
=============
ED_Count

For debugging
=============
*/
void ED_Count( void )
{
	int		i;
	edict_t* ent;
	int		active, models, solid, step;

	active = models = solid = step = 0;
	for (i = 0; i < sv.num_edicts; i++)
	{
		ent = &sv.edicts[i];
		if (ent->free)
			continue;
		active++;
		if (ent->v.solid)
			solid++;
		if (ent->v.model)
			models++;
		if (ent->v.movetype == MOVETYPE_STEP)
			step++;
	}

	Con_Printf("num_edicts:%3i\n", sv.num_edicts);
	Con_Printf("active    :%3i\n", active);
	Con_Printf("view      :%3i\n", models);
	Con_Printf("touch     :%3i\n", solid);
	Con_Printf("step      :%3i\n", step);
}

//============================================================================


/*
=============
ED_NewString
=============
*/
char* ED_NewString( const char* string )
{
	char*	new, *new_p;
	int		i, l;

	l = strlen(string) + 1;
	new = (char*)Hunk_Alloc(l);
	new_p = new;

	for (i = 0; i < l; i++)
	{
		if (string[i] == '\\' && i < l - 1)
		{
			i++;
			if (string[i] == 'n')
				*new_p++ = '\n';
			else
				*new_p++ = '\\';
		}
		else
			*new_p++ = string[i];
	}

	return new;
}

/*
====================
ED_ParseEdict

Parses an edict out of the given string, returning the new position
ed should be a properly initialized empty edict.
Used for initial level load and for savegames.
====================
*/
char* ED_ParseEdict( char* data, edict_t* ent )
{
	qboolean	init;
	char		keyname[256];
	int			n;
	ENTITYINIT	pEntityInit;
	char* className;
	KeyValueData kvd;

	init = FALSE;

// clear it
	if (ent != sv.edicts)	// hack
		memset(&ent->v, 0, sizeof(ent->v));

	InitEntityDLLFields(ent);

	SuckOutClassname(data, ent);

	className = (char*)(pr_strings + ent->v.classname);
	pEntityInit = GetEntityInit(className);
	if (pEntityInit)
	{
		pEntityInit(&ent->v);
		init = TRUE;
	}
	else
	{
		pEntityInit = GetEntityInit("custom");
		if (pEntityInit)
		{
			pEntityInit(&ent->v);
			kvd.szValue = className;
			kvd.szClassName = "custom";
			kvd.szKeyName = "customclass";
			kvd.fHandled = FALSE;
			gEntityInterface.pfnKeyValue(ent, &kvd);
			init = TRUE;
		}
		else
		{
			Con_Printf("Can't init %s\n", className);
		}
	}

// go through all the dictionary pairs
	while (1)
	{
	// parse key
		data = COM_Parse(data);
		if (com_token[0] == '}')
			break;
		if (!data)
			Sys_Error("ED_ParseEntity: EOF without closing brace");

		strcpy(keyname, com_token);

		// another hack to fix heynames with trailing spaces
		n = strlen(keyname);
		while (n && keyname[n - 1] == ' ')
		{
			keyname[n - 1] = 0;
			n--;
		}

	// parse value
		data = COM_Parse(data);
		if (!data)
			Sys_Error("ED_ParseEntity: EOF without closing brace");

		if (com_token[0] == '}')
			Sys_Error("ED_ParseEntity: closing brace without data");

		className = &pr_strings[ent->v.classname];
		if (className && !strcmp(className, com_token))
			continue;

// anglehack is to allow QuakeEd to write single scalar angles
// and allow them to be turned into vectors. (FIXME...)
		if (!strcmp(keyname, "angle"))
		{
			float y = atof(com_token);
			if (y >= 0)
			{
				sprintf(com_token, "%f %f %f", ent->v.angles[0], y, ent->v.angles[2]);
			}
			else if ((int)y == -1)
			{
				sprintf(com_token, "-90 0 0");
			}
			else
			{
				sprintf(com_token, "90 0 0");
			}

			strcpy(keyname, "angles");
		}

		kvd.szClassName = className;
		kvd.szKeyName = keyname;
		kvd.szValue = com_token;
		kvd.fHandled = FALSE;
		gEntityInterface.pfnKeyValue(ent, &kvd);
	}

	if (!init)
	{
		ent->free = TRUE;
		ent->serialnumber++;
	}

	return data;
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
	edict_t* ent;
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
			ent = sv.edicts;
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
		if (deathmatch.value && ((int)ent->v.spawnflags & SF_NOTINDEATHMATCH))
		{
			ED_Free(ent);
			inhibit++;
			continue;
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

		// look for the spawn function
		if (gEntityInterface.pfnSpawn(ent) < 0 || (ent->v.flags & FL_KILLME))
			ED_Free(ent);

		SV_FlushSignon();
	}

	Con_DPrintf("%i entities inhibited\n", inhibit);
}

/*
===============
PR_Init
===============
*/
void PR_Init( void )
{
	//Cmd_AddCommand("edict", ED_PrintEdict_f);
	//Cmd_AddCommand("edicts", ED_PrintEdicts);
	//Cmd_AddCommand("edictcount", ED_Count);
	//Cmd_AddCommand("profile", PR_Profile_f);
}



edict_t* EDICT_NUM( int n )
{
	if (n < 0 || n >= sv.max_edicts)
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

/*
===============
qboolean SuckOutClassname( char* szInputStream, edict_t* pEdict )

===============
*/
void SuckOutClassname( char* szInputStream, edict_t* pEdict )
{
	char		szKeyName[256];
	KeyValueData kvd;
	char* pszLastString;

	while (1)
	{
		pszLastString = COM_Parse(szInputStream);
		if (com_token[0] == '}')
			break;

		strcpy(szKeyName, com_token);

		szInputStream = COM_Parse(pszLastString);
		if (!strcmp(szKeyName, "classname"))
		{
			kvd.szKeyName = szKeyName;
			kvd.szClassName = NULL;
			kvd.szValue = com_token;
			kvd.fHandled = FALSE;
			gEntityInterface.pfnKeyValue(pEdict, &kvd);

			if (!kvd.fHandled)
				Host_Error("SuckOutClassname: parse error");

			return;
		}
	}
}

void ReleaseEntityDLLFields( edict_t* pEdict )
{
	FreeEntPrivateData(pEdict);
}

void InitEntityDLLFields( edict_t* pEdict )
{
	pEdict->v.pContainingEntity = pEdict;
}

//============================================================================

//
// Request engine to allocate "cb" bytes on the entity's private data pointer.
//
void* PvAllocEntPrivateData( edict_t* pEdict, long cb )
{
	FreeEntPrivateData(pEdict);

	if (cb <= 0)
		return NULL;

	pEdict->pvPrivateData = calloc(1, cb);
	return pEdict->pvPrivateData;
}

void* PvEntPrivateData( edict_t* pEdict )
{
	if (!pEdict)
		return NULL;

	return pEdict->pvPrivateData;
}

//
// Release the private data memory, if any.
//
void FreeEntPrivateData( edict_t* pEdict )
{
	if (pEdict->pvPrivateData)
		free(pEdict->pvPrivateData);
	pEdict->pvPrivateData = NULL;
}

edict_t* PEntityOfEntOffset( int iEntOffset )
{
	return (edict_t*)((char*)sv.edicts + iEntOffset);
}

int EntOffsetOfPEntity( const edict_t* pEdict )
{
	return (char*)pEdict - (char*)sv.edicts;
}

int IndexOfEdict( const edict_t* pEdict )
{
	int index;

	if (!pEdict)
		return 0;

	index = pEdict - sv.edicts;
	if (index < 0 || index > sv.max_edicts)
		Sys_Error("Bad entity in IndexOfEdict()");

	return index;
}

edict_t* PEntityOfEntIndex( int iEntIndex )
{
	edict_t* pEdict;

	if (iEntIndex < 0 || iEntIndex >= sv.max_edicts)
		return NULL;

	pEdict = EDICT_NUM(iEntIndex);
	if (pEdict->free || !pEdict->pvPrivateData)
	{
		if (iEntIndex >= svs.maxclients || pEdict->free)
			return NULL;
	}

	return pEdict;
}

char* SzFromIndex( int iString )
{
	return pr_strings + iString;
}

entvars_t* GetVarsOfEnt( edict_t* pEdict )
{
	return &pEdict->v;
}

edict_t* FindEntityByVars( entvars_t* pvars )
{
	int i;

	for (i = 0; i < sv.num_edicts; i++)
	{
		edict_t* pEdict = &sv.edicts[i];

		if (&pEdict->v == pvars)
			return pEdict;
	}

	return NULL;
}

float CVarGetFloat( const char* szVarName )
{
	return Cvar_VariableValue((char*)szVarName);
}

const char* CVarGetString( const char* szVarName )
{
	return Cvar_VariableString((char*)szVarName);
}

void CVarSetFloat( const char* szVarName, float flValue )
{
	Cvar_SetValue((char*)szVarName, flValue);
}

void CVarSetString( const char* szVarName, const char* szValue )
{
	Cvar_Set((char*)szVarName, (char*)szValue);
}

/*
=================
AllocEngineString

This will allocate memory from the hunk
Each call will allocate a new memory, no actual pooling of strings will occur
=================
*/
int AllocEngineString( const char* szValue )
{
	return ED_NewString(szValue) - pr_strings;
}

void SaveSpawnParms( edict_t* pEdict )
{
	int eoffset = NUM_FOR_EDICT(pEdict);
	client_t* client = NULL;

	if (eoffset < 1 || eoffset > svs.maxclients)
		Host_Error("Entity is not a client");
}

void* GetModelPtr( edict_t* pEdict )
{
	return Mod_Extradata(sv.models[pEdict->v.modelindex]);
}