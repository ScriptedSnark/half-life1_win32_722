//
// cdll_exp.c
//
// 4-23-98  JOHN
//  this file implements the functions exported the the client-side (HUD-drawing) DLL
//

#include "quakedef.h"
#include "cdll_int.h"


// TODO: Implement






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
	// TODO: Implement
}

// Plays a sound by index
void hudPlaySoundByIndex( int iSound, float volume )
{
	// TODO: Implement
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