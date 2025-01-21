// cl_demo.c

#include "quakedef.h"
#include "cl_demo.h"

void CL_RecordHUDCommand( char* cmdname )
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