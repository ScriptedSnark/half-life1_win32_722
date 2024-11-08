//
//  cdll_int.c
//
// this implementation handles the linking of the engine to the DLL
//

#include "hud.h"
#include "util.h"
#include <string.h>

#define DLLEXPORT __declspec( dllexport )

cl_enginefunc_t gEngfuncs;
CHud gHUD;


/*
========================== 
    Initialize

Called when the DLL is first loaded.
==========================
*/
extern "C" 
{
int DLLEXPORT Initialize( cl_enginefunc_t *pEnginefuncs, int iVersion );
int DLLEXPORT HUD_VidInit( void );
int DLLEXPORT HUD_Init( void );
int DLLEXPORT HUD_Redraw( float flTime, int intermission );
int DLLEXPORT HUD_UpdateClientData( client_data_t *cdata, float flTime );
int DLLEXPORT HUD_Reset ( void );
}


int DLLEXPORT Initialize( cl_enginefunc_t *pEnginefuncs, int iVersion )
{
	gEngfuncs = *pEnginefuncs;

	//!!! mwh UNDONE We need to think about our versioning strategy. Do we want to try to be compatible
	// with previous versions, especially when we're only 'bonus' functionality? Should it be the engine
	// that decides if the DLL is compliant?

	if (iVersion != CLDLL_INTERFACE_VERSION)
		return 0;

	memcpy(&gEngfuncs, pEnginefuncs, sizeof(cl_enginefunc_t));

	return 1;
}


/*
==========================
	HUD_VidInit

Called when the game initializes
and whenever the vid_mode is changed
so the HUD can reinitialize itself.
==========================
*/

int DLLEXPORT HUD_VidInit( void )
{
	gHUD.VidInit();
	return 1;
}

/*
==========================
	HUD_Init

Called whenever the client connects
to a server.  Reinitializes all 
the hud variables.
==========================
*/

int DLLEXPORT HUD_Init( void )
{
	gHUD.Init();

	return 1;
}


/*
==========================
	HUD_Redraw

called every screen frame to
redraw the HUD.
===========================
*/

int DLLEXPORT HUD_Redraw( float time, int intermission )
{
	gHUD.Redraw( time, intermission );

	return 1;
}


/*
==========================
	HUD_UpdateClientData

called every time shared client
dll/engine data gets changed,
and gives the cdll a chance
to modify the data.

returns 1 if anything has been changed, 0 otherwise.
==========================
*/

int DLLEXPORT HUD_UpdateClientData(client_data_t *pcldata, float flTime )
{
	return gHUD.UpdateClientData(pcldata, flTime );
}

/*
==========================
	HUD_Reset

Called at start and end of demos to restore to "non"HUD state.
==========================
*/

int DLLEXPORT HUD_Reset( void )
{
	gHUD.VidInit();
	return 1;
}