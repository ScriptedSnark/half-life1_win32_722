//
// death notice
//
#include "hud.h"
#include "util.h"
#include "parsemsg.h"

#include <string.h>
#include <stdio.h>


DECLARE_MESSAGE( m_DeathNotice, DeathMsg );

struct DeathNoticeItem {
	char szKiller[MAX_PLAYER_NAME_LENGTH];
	char szVictim[MAX_PLAYER_NAME_LENGTH];
	int iId;	// the index number of the associated sprite
	int iSuicide;
	float flDisplayTime;
};

#define MAX_DEATHNOTICES	4
static int DEATHNOTICE_DISPLAY_TIME = 6;

#define DEATHNOTICE_TOP		20

DeathNoticeItem rgDeathNoticeList[ MAX_DEATHNOTICES + 1 ];


int CHudDeathNotice :: Init( void )
{
	gHUD.AddHudElem( this );

	HOOK_MESSAGE( DeathMsg );

	CVAR_CREATE( "hud_deathnotice_time", "6" );

	return 1;
}


void CHudDeathNotice :: InitHUDData( void )
{
	memset( rgDeathNoticeList, 0, sizeof(rgDeathNoticeList) );
}


int CHudDeathNotice :: VidInit( void )
{
	return 1;
}

int CHudDeathNotice :: Draw( float flTime )
{
	int x, y, r, g, b;

	for ( int i = 0; i < MAX_DEATHNOTICES; i++ )
	{
		if ( rgDeathNoticeList[i].iId == 0 )
			break;  // we've gone through them all

		if ( rgDeathNoticeList[i].flDisplayTime < flTime )
		{ // display time has expired
			// remove the current item from the list
			memmove( &rgDeathNoticeList[i], &rgDeathNoticeList[i+1], sizeof(DeathNoticeItem) * (MAX_DEATHNOTICES - i) );
			i--;  // continue on the next item;  stop the counter getting incremented
			continue;
		}

		rgDeathNoticeList[i].flDisplayTime = min( rgDeathNoticeList[i].flDisplayTime, gHUD.m_flTime + DEATHNOTICE_DISPLAY_TIME );

		// Draw the death notice

		y = DEATHNOTICE_TOP + (20 * i);  //!!!

		int id = (rgDeathNoticeList[i].iId == -1) ? HUD_d_skull : rgDeathNoticeList[i].iId;
		x = ScreenWidth - ConsoleStringLen(rgDeathNoticeList[i].szVictim) - (gHUD.m_rgrcRects[id].right - gHUD.m_rgrcRects[id].left);

		if ( !rgDeathNoticeList[i].iSuicide )
		{
			x -= (5 + ConsoleStringLen( rgDeathNoticeList[i].szKiller ) );

			// Draw killers name
			x = 5 + DrawConsoleString( x, y, rgDeathNoticeList[i].szKiller );
		}

		r = 255;  g = 80;	b = 0;

		// Draw death weapon
		SPR_Set( gHUD.m_rghSprites[id], r, g, b );
		SPR_DrawAdditive( 0, x, y, &gHUD.m_rgrcRects[id] );

		x += (gHUD.m_rgrcRects[id].right - gHUD.m_rgrcRects[id].left);

		// Draw victims name
		x = DrawConsoleString( x, y, rgDeathNoticeList[i].szVictim );
	}

	return 1;
}

// This message handler may be better off elsewhere
int CHudDeathNotice :: MsgFunc_DeathMsg( const char *pszName, int iSize, void *pbuf )
{
	m_iFlags |= HUD_ACTIVE;

	BEGIN_READ( pbuf, iSize );

	int killer = READ_BYTE();
	int victim = READ_BYTE();

	char killedwith[32];
	strcpy( killedwith, "d_" );
	strncat( killedwith, READ_STRING(), 32 );

	gHUD.m_Scoreboard.DeathMsg( killer, victim );

	int i;
	for ( i = 0; i < MAX_DEATHNOTICES; i++ )
	{
		if ( rgDeathNoticeList[i].iId == 0 )
			break;
	}

	if ( i == MAX_DEATHNOTICES )
	{ // move the rest of the list forward to make room for this item
		memmove( rgDeathNoticeList, rgDeathNoticeList+1, sizeof(DeathNoticeItem) * MAX_DEATHNOTICES );
		i = MAX_DEATHNOTICES - 1;
	}

	gHUD.m_Scoreboard.GetAllPlayersInfo();

	char *killer_name = gHUD.m_Scoreboard.m_PlayerInfoList[ killer ].name;
	char *victim_name = gHUD.m_Scoreboard.m_PlayerInfoList[ victim ].name;
	if ( !killer_name )
		killer_name = "";
	if ( !victim_name )
		victim_name = "";

	strncpy( rgDeathNoticeList[i].szKiller, killer_name, MAX_PLAYER_NAME_LENGTH );
	strncpy( rgDeathNoticeList[i].szVictim, victim_name, MAX_PLAYER_NAME_LENGTH );

	if ( killer == victim || killer == 0 )
		rgDeathNoticeList[i].iSuicide = TRUE;

	// Find the sprite in the list
	int spr = GetSpriteIndex( killedwith );

	rgDeathNoticeList[i].iId = spr;

	DEATHNOTICE_DISPLAY_TIME = CVAR_GET_FLOAT( "hud_deathnotice_time" );
	rgDeathNoticeList[i].flDisplayTime = gHUD.m_flTime + DEATHNOTICE_DISPLAY_TIME;

	// record the death notice in the console
	if ( rgDeathNoticeList[i].iSuicide )
	{
		ConsolePrint( rgDeathNoticeList[i].szVictim );

		if ( !strcmp( killedwith, "d_world" ) )
		{
			ConsolePrint( " died" );
		}
		else
		{
			ConsolePrint( " killed self" );
		}
	}
	else
	{
		ConsolePrint( rgDeathNoticeList[i].szKiller );
		ConsolePrint( " killed " );
		ConsolePrint( rgDeathNoticeList[i].szVictim );
	}

	if ( killedwith && *killedwith && (*killedwith > 13 ) && strcmp( killedwith, "d_world" ) )
	{
		ConsolePrint( " with " );
		ConsolePrint( killedwith+2 ); // skip over the "d_" part
	}

	ConsolePrint( "\n" );

	return 1;
}




