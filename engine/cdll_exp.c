//
// cdll_exp.c
//
// 4-23-98  JOHN
//  this file implements the functions exported the the client-side (HUD-drawing) DLL
//

#include "quakedef.h"
#include "cdll_int.h"

int vid_stretched;
int GetScreenInfo( SCREENINFO* pscrinfo )
{
	if (!pscrinfo)
		return 0;

	if (pscrinfo->iSize != sizeof(SCREENINFO))
		return 0;

	pscrinfo->iWidth = vid.width;
	pscrinfo->iHeight = vid.height;
#if defined ( GLQUAKE )
	pscrinfo->iFlags = SCRINFO_SCREENFLASH;
#else
	pscrinfo->iFlags = 0;
	if (vid_stretched)
		pscrinfo->iFlags |= SCRINFO_STRETCHED;
#endif
	pscrinfo->iCharHeight = Draw_MessageFontInfo(pscrinfo->charWidths);
	return sizeof(SCREENINFO);
}

int hudRegisterVariable( char* szName, char* szValue )
{
	cvar_t* cv;

	cv = (cvar_t*)Z_Malloc(sizeof(cvar_t));
	cv->name = szName;
	cv->string = szValue;

	Cvar_RegisterVariable(cv);

	return 1;
}

float hudGetCvarFloat( char* szName )
{
	cvar_t* cv;

	cv = Cvar_FindVar(szName);
	if (!cv)
		return 0.0f;

	return cv->value;
}

char* hudGetCvarString( char* szName )
{
	cvar_t* cv;

	cv = Cvar_FindVar(szName);
	if (!cv)
		return NULL;

	return cv->string;
}

int hudAddCommand( char* cmd_name, void (*function)(void) )
{
	Cmd_AddHUDCommand(cmd_name, function);
	return 1;
}

int hudServerCmd( char* pszCmdString )
{
	char buf[256];
	// just like the client typed "cmd xxxxx" at the console

	strcpy(buf, "cmd ");
	strncat(buf, pszCmdString, 250);

	Cmd_TokenizeString(buf);
	Cmd_ForwardToServer();

	return TRUE;
}

int hudClientCmd( char* pszCmdString )
{
	if (!pszCmdString)
		return 0;

	Cbuf_AddText(pszCmdString);
	Cbuf_AddText("\n");
	return 1;
}

int hudHookUserMsg( char* szMsgName, pfnUserMsgHook pfn )
{
	return HookServerMsg(szMsgName, pfn) != NULL;
}

// player info handler
void hudGetPlayerInfo( int ent_num, hud_player_info_t* pinfo )
{
	ent_num -= 1; // player list if offset by 1 from ents

	if (ent_num >= cl.maxclients ||
		 ent_num < 0 ||
		 !cl.players[ent_num].name[0])
	{
		pinfo->name = NULL;
		return;
	}

	pinfo->name = cl.players[ent_num].name;
	pinfo->ping = cl.players[ent_num].ping;
	pinfo->thisplayer = ent_num == cl.playernum;
	pinfo->spectator = cl.players[ent_num].spectator;
	pinfo->packetloss = 0;	
}

// Plays a sound by name
void hudPlaySoundByName( char* szSound, float volume )
{
	sfx_t* sfx;

	volume = clamp(volume, 0.0, 1.0);

	sfx = S_PrecacheSound(szSound);
	if (!sfx)
	{
		Con_DPrintf("invalid sound %s\n", szSound);
		return;
	}

	S_StartDynamicSound(cl.viewentity, -1, sfx, r_origin, volume, 1.0, 0, PITCH_NORM);
}

// Plays a sound by index
void hudPlaySoundByIndex( int iSound, float volume )
{
	volume = clamp(volume, 0.0, 1.0);

	if (iSound > MAX_SOUNDS)
	{
		Con_DPrintf("invalid sound %i\n", iSound);
		return;
	}

	S_StartDynamicSound(cl.viewentity, -1, cl.sound_precache[iSound], r_origin, volume, 1.0, 0, PITCH_NORM);
}

// Gets the length in pixels of a string if it were drawn onscreen
void hudDrawConsoleStringLen( const char* string, int* length, int* height )
{
	*length = Draw_StringLen((char*)string);
	*height = draw_chars->rowheight;
}

void hudConsolePrint( const char* string )
{
	Con_Print((char*)string);
}