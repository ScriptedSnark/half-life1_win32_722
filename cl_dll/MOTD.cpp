//
// MOTD.cpp
//
// for displaying a server-sent message of the day
//

#include "hud.h"
#include "util.h"
#include "parsemsg.h"

#include <string.h>
#include <stdio.h>

DECLARE_MESSAGE( m_MOTD, MOTD );

int CHudMOTD::MOTD_DISPLAY_TIME;

int CHudMOTD :: Init( void )
{
	gHUD.AddHudElem( this );

	HOOK_MESSAGE( MOTD );

	CVAR_CREATE( "motd_display_time", "6" );

	m_iFlags &= ~HUD_ACTIVE;  // start out inactive
	m_szMOTD[0] = 0;

	return 1;
}

int CHudMOTD :: VidInit( void )
{
	// Load sprites here

	return 1;
}

void CHudMOTD :: Reset( void )
{
	m_iFlags &= ~HUD_ACTIVE;  // start out inactive
	m_szMOTD[0] = 0;
	m_iLines = 0;
	m_flActiveTill = 0;
}

#define LINE_HEIGHT  13

int CHudMOTD :: Draw( float fTime )
{
	// Draw MOTD line-by-line

	if ( m_flActiveTill < gHUD.m_flTime )
	{ // finished with MOTD, disable it
		m_szMOTD[0] = 0;
		m_iLines = 0;
		m_iFlags &= ~HUD_ACTIVE;
		return 1;
	}

	// cap activetill time to the display time
	m_flActiveTill = min( gHUD.m_flTime + MOTD_DISPLAY_TIME, m_flActiveTill );

	// find the top of where the MOTD should be drawn,  so the whole thing is centered in the screen
	int ypos = max(((ScreenHeight - (m_iLines * LINE_HEIGHT)) / 2) - 40, 30 ); // shift it up slightly
	char *ch = m_szMOTD;
	while ( *ch )
	{
		int line_length = 0;  // count the length of the current line
		char* next_line;
		for ( next_line = ch; *next_line != '\n' && *next_line != 0; next_line++ )
			line_length += gHUD.m_scrinfo.charWidths[ *next_line ];
		char *top = next_line;
		if ( *top == '\n' )
			*top = 0;
		else
			top = NULL;

		// find where to start drawing the line
		int xpos = (ScreenWidth - line_length) / 2;

		gHUD.DrawHudString( xpos, ypos, ScreenWidth, ch, 255, 180, 0 );

		ypos += LINE_HEIGHT;

		if ( top )  // restore 
			*top = '\n';
		ch = next_line;
		if ( *ch == '\n' )
			ch++;

		if ( ypos > (ScreenHeight - 20) )
			break;  // don't let it draw too low
	}
	
	return 1;
}

int CHudMOTD :: MsgFunc_MOTD( const char *pszName, int iSize, void *pbuf )
{
	if ( m_iFlags & HUD_ACTIVE )
	{
		Reset(); // clear the current MOTD in prep for this one
	}

	BEGIN_READ( pbuf, iSize );

	int is_finished = READ_BYTE();
	strcat( m_szMOTD, READ_STRING() );

	if ( is_finished )
	{
		m_iFlags |= HUD_ACTIVE;

		MOTD_DISPLAY_TIME = CVAR_GET_FLOAT( "motd_display_time" );

		m_flActiveTill = gHUD.m_flTime + MOTD_DISPLAY_TIME;

		for ( char *sz = m_szMOTD; *sz != 0; sz++ )  // count the number of lines in the MOTD
		{
			if ( *sz == '\n' )
				m_iLines++;
		}
	}

	return 1;
}

