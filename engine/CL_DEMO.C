// cl_demo.c

#include "quakedef.h"
#include "tmessage.h"
#include "cl_demo.h"

// TODO: Implement

char	gDemoMessageBuffer[4];

client_textmessage_t tm_demomessage =
{
	0, // effect
	255, 255, 255, 255,
	255, 255, 255, 255,
	-1.0f, // x
	-1.0f, // y
	0.0f, // fadein
	0.0f, // fadeout
	0.0f, // holdtime
	0.0f, // fxTime,
	DEMO_MESSAGE,  // pName message name.
	gDemoMessageBuffer   // pMessage
};

// TODO: Implement

/*
====================
CL_WriteDemoUpdate

====================
*/
void CL_WriteDemoUpdate( void )
{
	// TODO: Implement
}

// TODO: Implement

/*
====================
CL_RecordHUDCommand

====================
*/
void CL_RecordHUDCommand( char* cmdname )
{
	// TODO: Implement
}

/*
====================
CL_WriteDLLUpdate

====================
*/
void CL_WriteDLLUpdate( client_data_t* cdata )
{
	// TODO: Implement
}

/*
====================
CL_BeginDemoStartup
====================
*/
void CL_BeginDemoStartup( void )
{
	if (cls.demoheader)
		fclose(cls.demoheader);

	cls.demoheader = tmpfile();
	if (!cls.demoheader)
	{
		Con_DPrintf("ERROR: couldn't open temporary header file.\n");
		return;
	}
	Con_DPrintf("Spooling demo header.\n");
}




/*
====================
CL_StopPlayback

Called when a demo file runs out, or the user starts a game
====================
*/
void CL_StopPlayback( void )
{
	// TODO: Implement
}




/*
====================
CL_Stop_f

stop recording a demo
====================
*/
void CL_Stop_f( void )
{
	// TODO: Implement
}

/*
====================
CL_Record_f

//
====================
*/
void CL_Record_f( void )
{
	// TODO: Implement
}