//
// saytext.cpp
//
// implementation of CHudSayText class
//

#include "hud.h"
#include "util.h"
#include "parsemsg.h"

#include <string.h>
#include <stdio.h>

#define MAX_LINES	5
#define MAX_CHARS_PER_LINE	128  /* it can be less than this, depending on char size */

// allow 20 pixels on either side of the text
#define MAX_LINE_WIDTH  ( ScreenWidth - 40 )
#define LINE_START  10
static float SCROLL_SPEED = 5;

static char g_szLineBuffer[ MAX_LINES + 1 ][ MAX_CHARS_PER_LINE ];
static float flScrollTime = 0;  // the time at which the lines next scroll up

static int Y_START = 0;
static int line_height = 0;

DECLARE_MESSAGE( m_SayText, SayText );

int CHudSayText :: Init( void )
{
	gHUD.AddHudElem( this );

	HOOK_MESSAGE( SayText );

	InitHUDData();

	CVAR_CREATE( "hud_saytext_time", "5" );

	return 1;
}


void CHudSayText :: InitHUDData( void )
{
	memset( g_szLineBuffer, 0, sizeof g_szLineBuffer );
}

int CHudSayText :: VidInit( void )
{
	return 1;
}


void ScrollTextUp( void )
{
	ConsolePrint( g_szLineBuffer[0] ); // move the first line into the console buffer
	memmove( g_szLineBuffer[0], g_szLineBuffer[1], sizeof(g_szLineBuffer) - sizeof(g_szLineBuffer[0]) ); // overwrite the first line

	if ( g_szLineBuffer[0][0] == ' ' ) // also scroll up following lines
	{
		g_szLineBuffer[0][0] = 2;
		ScrollTextUp();
	}
}

int CHudSayText :: Draw( float flTime )
{
	int y = Y_START;

	if ( flScrollTime <= flTime )
	{
		if ( *g_szLineBuffer[0] )
		{
			flScrollTime = flTime + SCROLL_SPEED;
			// push the console up
			ScrollTextUp();
		}
		else
		{ // buffer is empty,  just disable drawing of this section
			m_iFlags &= ~HUD_ACTIVE;
		}
	}

	for ( int i = 0; i < MAX_LINES; i++ )
	{
		if ( *g_szLineBuffer[i] )
			DrawConsoleString( LINE_START, y, g_szLineBuffer[i] );

		y += line_height;
	}


	return 1;
}

int CHudSayText :: MsgFunc_SayText( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	int client_index = READ_BYTE();		// the client who spoke the message
	char* pszBuf = READ_STRING();		// the message

	// find an empty string slot
	int i;
	for (i = 0; i < MAX_LINES; i++)
	{
		if (!*g_szLineBuffer[i])
			break;
	}
	if (i == MAX_LINES)
	{
		// force scroll buffer up
		ScrollTextUp();
		i = MAX_LINES - 1;
	}

	strncpy(g_szLineBuffer[i], pszBuf, MAX_CHARS_PER_LINE - 1);

	// make sure the text fits in one line
	EnsureTextFitsInOneLineAndWrapIfHaveTo(i);

	// Set scroll time
	if (i == 0)
	{
		SCROLL_SPEED = CVAR_GET_FLOAT("hud_saytext_time");
		flScrollTime = gHUD.m_flTime + SCROLL_SPEED;
	}

	m_iFlags |= HUD_ACTIVE;
	PlaySound("common/menu2.wav", 0.5);

	if (ScreenHeight >= 480)
		Y_START = ScreenHeight - 45;
	else
		Y_START = ScreenHeight - 35;
	Y_START -= (line_height * MAX_LINES);
	
	return 1;
}

void CHudSayText :: EnsureTextFitsInOneLineAndWrapIfHaveTo( int line )
{
	int line_width = 0;
	GetConsoleStringSize( g_szLineBuffer[line], &line_width, &line_height );

	if ( (line_width + LINE_START) > MAX_LINE_WIDTH )
	{ // string is too long to fit on line
		// scan the string until we find what word is too long,  and wrap the end of the sentence after the word
		int length = LINE_START;
		int tmp_len = 0;
		char *last_break = NULL;
		for ( char *x = g_szLineBuffer[line]; *x != 0; x++ )
		{
			char buf[2];
			buf[1] = 0;

			if ( *x == ' ' && x != g_szLineBuffer[line] )  // store each line break,  except for the very first character
				last_break = x;

			buf[0] = *x;  // get the length of the current character
			GetConsoleStringSize( buf, &tmp_len, &line_height );
			length += tmp_len;

			if ( length > MAX_LINE_WIDTH )
			{  // needs to be broken up
				if ( !last_break )
					last_break = x-1;

				x = last_break;

				// find an empty string slot
				int j;
				for ( j = 0; j < MAX_LINES; j++ )
				{
					if ( ! *g_szLineBuffer[j] )
						break;
				}
				if ( j == MAX_LINES )
				{
					j = MAX_LINES - 1;
				}

				// copy remaining string into next buffer,  making sure it starts with a space character
				if ( (char)*last_break == (char)' ' )
				{
					int linelen = strlen(g_szLineBuffer[j]);
					int remaininglen = strlen(last_break);

					if ( (linelen - remaininglen) <= MAX_CHARS_PER_LINE )
						strcat( g_szLineBuffer[j], last_break );
				}
				else
				{
					if ( (strlen(g_szLineBuffer[j]) - strlen(last_break) - 2) < MAX_CHARS_PER_LINE )
					{
						strcat( g_szLineBuffer[j], " " );
						strcat( g_szLineBuffer[j], last_break );
					}
				}

				*last_break = 0; // cut off the last string

				EnsureTextFitsInOneLineAndWrapIfHaveTo( j );
				break;
			}
		}
	}
}