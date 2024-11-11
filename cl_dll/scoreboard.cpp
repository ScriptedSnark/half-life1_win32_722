//
// Scoreboard.cpp
//
// implementation of CHudScoreboard class
//

#include "hud.h"
#include "util.h"
#include "parsemsg.h"

#include <string.h>
#include <stdio.h>

DECLARE_COMMAND( m_Scoreboard, ShowScores );
DECLARE_COMMAND( m_Scoreboard, HideScores );

DECLARE_MESSAGE( m_Scoreboard, ScoreInfo );

int CHudScoreboard :: Init( void )
{
	gHUD.AddHudElem( this );

	// Hook messages & commands here
	HOOK_COMMAND( "+showscores", ShowScores );
	HOOK_COMMAND( "-showscores", HideScores );

	HOOK_MESSAGE( ScoreInfo );

	InitHUDData();

	return 1;
}


int CHudScoreboard :: VidInit( void )
{
	// Load sprites here

	return 1;
}

void CHudScoreboard :: InitHUDData( void )
{
	memset( m_PlayerExtraInfo, 0, sizeof m_PlayerExtraInfo );
	m_iLastKilledBy = 0;
	m_fLastKillTime = 0;
	m_iPlayerNum = 0;

	m_iFlags &= ~HUD_ACTIVE;  // starts out inactive

	m_iFlags |= HUD_INTERMISSION; // is always drawn during an intermission
}

/* The scoreboard
We have a minimum width of 1-320 - we could have the field widths scale with it?
*/

// X positions
// relative to the side of the scoreboard
#define NAME_RANGE_MIN  20
#define NAME_RANGE_MAX  145
#define KILLS_RANGE_MIN 130
#define KILLS_RANGE_MAX 170
#define DIVIDER_POS		180
#define DEATHS_RANGE_MIN  185
#define DEATHS_RANGE_MAX  210
#define PING_RANGE_MIN	230
#define PING_RANGE_MAX	270

#define SCOREBOARD_WIDTH 320
		

// Y positions
#define ROW_GAP  13
#define ROW_RANGE_MIN 15
#define ROW_RANGE_MAX ( ScreenHeight - 50 )

int CHudScoreboard :: Draw( float fTime )
{
	if ( !m_iShowscoresHeld && gHUD.m_Health.m_iHealth > 0 && !gHUD.m_iIntermission )
		return 1;

	GetAllPlayersInfo();

	// just sort the list on the fly
	// list is sorted first by frags, then by deaths
	float list_slot = 0;
	int xpos_rel = (ScreenWidth - SCOREBOARD_WIDTH) / 2;

	// print the heading line
	int ypos = ROW_RANGE_MIN + (list_slot * ROW_GAP);
	int	xpos = NAME_RANGE_MIN + xpos_rel;

	gHUD.DrawHudString( xpos, ypos, NAME_RANGE_MAX + xpos_rel, "player", 255, 140, 0 );
	gHUD.DrawHudStringReverse( KILLS_RANGE_MAX + xpos_rel, ypos, 0, "kills", 255, 140, 0 );
	gHUD.DrawHudString( DIVIDER_POS + xpos_rel, ypos, ScreenWidth, "/", 255, 140, 0 );
	gHUD.DrawHudString( DEATHS_RANGE_MIN + xpos_rel + 5, ypos, ScreenWidth, "deaths", 255, 140, 0 );
	gHUD.DrawHudString( PING_RANGE_MAX + xpos_rel - 20, ypos, ScreenWidth, "ping", 255, 140, 0 );

	list_slot += 1.2;
	ypos = ROW_RANGE_MIN + (list_slot * ROW_GAP);
	xpos = NAME_RANGE_MIN + xpos_rel;
	FillRGBA( xpos - 5, ypos, PING_RANGE_MAX - 5, 1, 255, 140, 0, 255);  // draw the seperator line
	
	list_slot += 0.8;

	// draw the players, in order
	while ( 1 )
	{
		// Find the top ranking player
		int highest_frags = -99999;	int lowest_deaths = 99999;
		int best_player = 0;

		for ( int i = 1; i < MAX_PLAYERS; i++ )
		{
			if ( m_PlayerInfoList[i].name && m_PlayerExtraInfo[i].frags >= highest_frags )
			{
				extra_player_info_t* pl_info = &m_PlayerExtraInfo[i];
				if (pl_info->frags > highest_frags || pl_info->deaths < lowest_deaths)
				{
					best_player = i;
					lowest_deaths = pl_info->deaths;
					highest_frags = pl_info->frags;
				}
			}
		}

		if ( !best_player )
			break;

		// draw out the best player
		hud_player_info_t* pl_info = &m_PlayerInfoList[best_player];

		int ypos = ROW_RANGE_MIN + (list_slot * ROW_GAP);

		// check we haven't drawn too far down
		if ( ypos > ROW_RANGE_MAX )  // don't draw to close to the lower border
			break;

		int xpos = NAME_RANGE_MIN + xpos_rel;
		int r = 255, g = 255, b = 255;
		if ( best_player == m_iLastKilledBy && m_fLastKillTime && m_fLastKillTime > gHUD.m_flTime )
		{
			if ( pl_info->thisplayer )
			{  // green is the suicide color? i wish this could do grey...
				FillRGBA( NAME_RANGE_MIN + xpos_rel - 5, ypos, PING_RANGE_MAX - 5, ROW_GAP, 80, 155, 0, 70 );
			}
			else
			{  // Highlight the killers name - overlay the background in red,  then draw the score text over it
				FillRGBA( NAME_RANGE_MIN + xpos_rel - 5, ypos, PING_RANGE_MAX - 5, ROW_GAP, 255, 0, 0, ((float)15 * (float)(m_fLastKillTime - gHUD.m_flTime)) );
			}
		}
		else if ( pl_info->thisplayer ) // if it is their name, draw it a different color
		{
			// overlay the background in blue,  then draw the score text over it
			FillRGBA( NAME_RANGE_MIN + xpos_rel - 5, ypos, PING_RANGE_MAX - 5, ROW_GAP, 0, 0, 255, 70 );
		}

		// draw their name (left to right)
		xpos = NAME_RANGE_MIN + xpos_rel;
		gHUD.DrawHudString( xpos, ypos, NAME_RANGE_MAX + xpos_rel, pl_info->name, r, g, b);

		// draw kills (right to left)
		xpos = KILLS_RANGE_MAX + xpos_rel;
		gHUD.DrawHudNumberString( xpos, ypos, KILLS_RANGE_MIN + xpos_rel, m_PlayerExtraInfo[best_player].frags, r, g, b );

		// draw divider
		xpos = DIVIDER_POS + xpos_rel;
		gHUD.DrawHudString( xpos, ypos, xpos + 200, "/", r, g, b );

		// draw deaths
		xpos = DEATHS_RANGE_MAX + xpos_rel;
		gHUD.DrawHudNumberString( xpos, ypos, DEATHS_RANGE_MIN + xpos_rel, m_PlayerExtraInfo[best_player].deaths, r, g, b );

		// draw ping & packetloss
		xpos = PING_RANGE_MAX + xpos_rel;
		gHUD.DrawHudNumberString( xpos, ypos, PING_RANGE_MIN + xpos_rel, m_PlayerInfoList[best_player].ping, 255, 160, 0 );

		/*  Packetloss removed on Kelly 'shipping nazi' Bailey's orders
			if ( m_PlayerInfoList[best_player].packetloss >= 63 )
			{
				UnpackRGB( r, g, b, RGB_REDISH );
				sprintf( buf, " !!!!" );
			}
			else
			{
				sprintf( buf, " %d", m_PlayerInfoList[best_player].packetloss );
			}

			gHUD.DrawHudString( xpos, ypos, xpos+50, buf, r, g, b );
		*/

		pl_info->name = NULL;  // set the name to be NULL, so this client won't get drawn again
		list_slot++;
	}

	return 1;
}


void CHudScoreboard :: GetAllPlayersInfo( void )
{
	for ( int i = 1; i < MAX_PLAYERS; i++ )
	{
		GetPlayerInfo( i, &m_PlayerInfoList[i] );

		if ( m_PlayerInfoList[i].thisplayer )
			m_iPlayerNum = i;  // !!!HACK: this should be initialized elsewhere... maybe gotten from the engine
	}
}

int CHudScoreboard :: MsgFunc_ScoreInfo( const char *pszName, int iSize, void *pbuf )
{
	m_iFlags |= HUD_ACTIVE;

	BEGIN_READ( pbuf, iSize );
	short cl = READ_BYTE();
	short frags = READ_SHORT();
	short deaths = READ_SHORT();

	if ( cl > 0 && cl <= MAX_PLAYERS )
	{
		m_PlayerExtraInfo[cl].frags = frags;
		m_PlayerExtraInfo[cl].deaths = deaths;
	}

	return 1;
}

void CHudScoreboard :: DeathMsg( int killer, int victim )
{
	// if we were the one killed,  or the world killed us, set the scoreboard to indicate suicide
	if ( victim == m_iPlayerNum || killer == 0 )
	{
		m_iLastKilledBy = killer ? killer : m_iPlayerNum;
		m_fLastKillTime = gHUD.m_flTime + 10;	// display who we were killed by for 10 seconds

		if ( killer == m_iPlayerNum )
			m_iLastKilledBy = m_iPlayerNum;
	}
}



void CHudScoreboard :: UserCmd_ShowScores( void )
{
	m_iShowscoresHeld = TRUE;
}

void CHudScoreboard :: UserCmd_HideScores( void )
{
	m_iShowscoresHeld = FALSE;
}
