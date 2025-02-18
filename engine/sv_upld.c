#include "quakedef.h"
#include "server.h"
#include "decal.h"
#include "hashpak.h"

/*
================
SV_ModelIndex

================
*/
int SV_ModelIndex( char* name )
{
	int		i;

	if (!name || !name[0])
		return 0;

	for (i = 0; i < MAX_MODELS && sv.model_precache[i]; i++)
		if (!strcmp(sv.model_precache[i], name))
			return i;
	if (i == MAX_MODELS || !sv.model_precache[i])
		Sys_Error("SV_ModelIndex: model %s not precached", name);
	return i;
}

/*
================
SV_FlushSignon

Moves to the next signon buffer if needed
================
*/
void SV_FlushSignon( void )
{
	if (sv.signon.cursize < sv.signon.maxsize - 100)
		return;

	if (sv.num_signon_buffers == MAX_SIGNON_BUFFERS - 1)
		Sys_Error("sv.num_signon_buffers == MAX_SIGNON_BUFFERS-1");

	sv.signon_buffer_size[sv.num_signon_buffers - 1] = sv.signon.cursize;
	sv.signon.data = sv.signon_buffers[sv.num_signon_buffers];
	sv.num_signon_buffers++;
	sv.signon.cursize = 0;
}

// TODO: Implement

/*
================
SV_AddResource

Put this very resource into resources array
================
*/
void SV_AddResource( resourcetype_t type, const char *name, int size, byte flags, int index )
{
	resource_t*		res;

	res = &sv.resources[sv.num_resources];

	if (sv.num_resources >= MAX_RESOURCES)
		Sys_Error("Too many resources on server.");

	sv.num_resources++;

	res->type = type;
	strcpy(res->szFileName, name);
	res->nDownloadSize = size;
	res->nIndex = index;
	res->ucFlags = flags;
}

/*
================
SV_CreateResourceList

Add resources to precache list
================
*/
void SV_CreateResourceList( void )
{
	int			i, nSize;
	char*		s;
	qboolean	bAddedFirst = FALSE;
	FILE*		pf;

	sv.num_resources = 0;

	for (i = 1; i < MAX_SOUNDS; i++)
	{
		s = sv.sound_precache[i];

		if (!s || s[0] == 0)
			break;

		if (s[0] == '!')
		{
			if (!bAddedFirst)
			{
				SV_AddResource(t_sound, "!", 0, RES_FATALIFMISSING, i);
				bAddedFirst = TRUE;
			}
		}
		else
		{
			if (svs.maxclients <= 1)
			{
				SV_AddResource(t_sound, s, 0, 0, i);
			}
			else
			{
				nSize = COM_FindFile(va("sound/%s", s), NULL, &pf);
				if (pf)
					fclose(pf);
				if (nSize == -1)
					nSize = 0;

				SV_AddResource(t_sound, s, nSize, 0, i);
			}
		}
	}

	for (i = 1; i < MAX_MODELS; i++)
	{
		s = sv.model_precache[i];

		if (!s || s[0] == 0)
			break;

		if (svs.maxclients <= 1)
		{
			SV_AddResource(t_model, s, 0, RES_FATALIFMISSING, i);
		}
		else
		{
			nSize = COM_FindFile(s, NULL, &pf);
			if (pf)
				fclose(pf);
			if (nSize == -1)
				nSize = 0;

			SV_AddResource(t_model, s, nSize, RES_FATALIFMISSING, i);
		}
	}

	for (i = 0; i < sv_decalnamecount; i++)
	{
		SV_AddResource(t_decal, sv_decalnames[i].name , Draw_DecalSize(i), 0, i);
	}
}

/*
==================
SV_ClearResourceLists

==================
*/
void SV_ClearResourceLists(client_t* cl)
{
	// TODO: Implement
}

/*
==================
SV_RequestMissingResourcesFromClients

This is called each frame to do checks on players if they uploaded all files that where requested from them.
==================
*/
void SV_RequestMissingResourcesFromClients( void )
{
	// TODO: Implement
}

/*
==================
SV_ParseResourceList
==================
*/
void SV_ParseResourceList( void )
{
	// TODO: Implement
}