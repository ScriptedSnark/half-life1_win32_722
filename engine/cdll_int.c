//
//  cdll_int.c
//
// 4-23-98  
// JOHN:  implementation of interface between client-side DLL and game engine.
//  The cdll shouldn't have to know anything about networking or file formats.
//  This file is Win32-dependant
//


#include "quakedef.h"
#include "winquake.h"
#include "screen.h"
#include "cl_demo.h"
#include "hud_handlers.h"

void CL_ResetButtonBits( int bits );
int CL_ButtonBits( int );

// Global table of exported engine functions to client dll
cl_enginefunc_t cl_enginefuncs =
{
	NULL, // TODO: Implement
	NULL, // TODO: Implement
	NULL, // TODO: Implement
	NULL, // TODO: Implement
	NULL, // TODO: Implement
	NULL, // TODO: Implement
	NULL, // TODO: Implement
	NULL, // TODO: Implement
	NULL, // TODO: Implement
	NULL, // TODO: Implement
	NULL, // TODO: Implement
	NULL, // TODO: Implement
	NULL, // TODO: Implement
	NULL, // TODO: Implement
	NULL, // TODO: Implement
	NULL, // TODO: Implement
	NULL, // TODO: Implement
	NULL, // TODO: Implement
	NULL, // TODO: Implement
	NULL, // TODO: Implement
	NULL, // TODO: Implement
	NULL, // TODO: Implement
	NULL, // TODO: Implement
	NULL, // TODO: Implement
	NULL, // TODO: Implement
	NULL, // TODO: Implement
	NULL, // TODO: Implement
	NULL, // TODO: Implement
	NULL, // TODO: Implement
	NULL // TODO: Implement
};

// Pointers to the exported client functions themselves
typedef struct
{
	int  (*pInitFunc)( struct cl_enginefuncs_s* pEnginefuncs, int iVersion );
	void (*pHudInitFunc)( void );
	int  (*pHudVidInitFunc)( void );
	int  (*pHudRedrawFunc)( float, int );
	int  (*pHudUpdateClientDataFunc)( client_data_t* , float );
	void (*pHudResetFunc)( void );
} cldll_func_t;

cldll_func_t cl_funcs;

/*
==============
ClientDLL_Init

Loads the client .dll
==============
*/
void ClientDLL_Init( void )
{
	int		i;
	HMODULE hModule;
	char	szDllName[512];
	char* pszGameDir;

	i = COM_CheckParm("-game");

	pszGameDir = com_argv[i + 1];
	if (i && pszGameDir && pszGameDir[0])
		sprintf(szDllName, "%s\\cl_dlls\\client.dll", pszGameDir);
	else
		sprintf(szDllName, "valve\\cl_dlls\\client.dll");

	hModule = LoadLibrary(szDllName);
	if (!hModule)
		Sys_Error("could not load library %s", szDllName);

	cl_funcs.pInitFunc = DECLTYPE(cl_funcs.pInitFunc)GetProcAddress(hModule, "Initialize");
	if (!cl_funcs.pInitFunc)
		Sys_Error("could not link client.dll function Initialize\n");

	cl_funcs.pHudVidInitFunc = DECLTYPE(cl_funcs.pHudVidInitFunc)GetProcAddress(hModule, "HUD_VidInit");
	if (!cl_funcs.pHudVidInitFunc)
		Sys_Error("could not link client.dll function HUD_VidInit\n");

	cl_funcs.pHudInitFunc = DECLTYPE(cl_funcs.pHudInitFunc)GetProcAddress(hModule, "HUD_Init");
	if (!cl_funcs.pHudInitFunc)
		Sys_Error("could not link client.dll function HUD_Init\n");

	cl_funcs.pHudRedrawFunc = DECLTYPE(cl_funcs.pHudRedrawFunc)GetProcAddress(hModule, "HUD_Redraw");
	if (!cl_funcs.pHudRedrawFunc)
		Sys_Error("could not link client.dll function HUD_Redraw\n");

	cl_funcs.pHudUpdateClientDataFunc = DECLTYPE(cl_funcs.pHudUpdateClientDataFunc)GetProcAddress(hModule, "HUD_UpdateClientData");
	if (!cl_funcs.pHudUpdateClientDataFunc)
		Sys_Error("could not link client.dll function HUD_UpdateClientData\n");

	cl_funcs.pHudResetFunc = DECLTYPE(cl_funcs.pHudResetFunc)GetProcAddress(hModule, "HUD_Reset");
	if (!cl_funcs.pHudResetFunc)
		Sys_Error("could not link client.dll function HUD_Reset\n");

	cl_funcs.pInitFunc(&cl_enginefuncs, CLDLL_INTERFACE_VERSION);
}

/*
==============
ClientDLL_HudVidInit

Called when the game initializes and whenever the vid_mode is changed
 so the HUD can reinitialize itself.
==============
*/
void ClientDLL_HudVidInit( void )
{
	if (!cl_funcs.pHudVidInitFunc)
		Sys_Error(__FILE__ ", line %d: could not link client DLL for HUD Vid initialization", __LINE__);

	cl_funcs.pHudVidInitFunc();
}

/*
==============
ClientDLL_HudInit

Called to initialize the client library
This occurs after the engine has loaded the client and has initialized all other systems
==============
*/
void ClientDLL_HudInit( void )
{
	if (!cl_funcs.pHudInitFunc)
		Sys_Error(__FILE__ ", line %d: could not link client DLL for HUD initialization", __LINE__);

	cl_funcs.pHudInitFunc();
}

/*
==============
ClientDLL_HudRedraw

Called to redraw the HUD
==============
*/
void ClientDLL_HudRedraw( int intermission )
{
	cl_funcs.pHudRedrawFunc(cl.time, intermission);
}

/*
==============
ClientDLL_UpdateClientData

Called every frame while running a map
==============
*/
void ClientDLL_UpdateClientData( void )
{
	client_data_t cdat;
	client_data_t oldcdat;
	int bits;

	if (!cls.demoplayback && !cl.spectator)
	{
		memset(&cdat, 0, sizeof(cdat));

		cdat.viewheight = cl.viewheight;
		cdat.maxspeed = cl.maxspeed;

		VectorCopy(cl.viewangles, cdat.viewangles);
		VectorCopy(cl.punchangle, cdat.punchangle);
		VectorCopy(cl_entities[cl.viewentity].origin, cdat.origin);
		
		cdat.iKeyBits = CL_ButtonBits(0);
		bits = cdat.iKeyBits;

		cdat.fov = scr_fov_value;
		cdat.iWeaponBits = cl.weapons;

		if (cls.demorecording)
		{
			memcpy(&oldcdat, &cdat, sizeof(oldcdat));
		}

		if (cl_funcs.pHudUpdateClientDataFunc(&cdat, cl.time))
		{
			cl.viewheight = cdat.viewheight;
			cl.maxspeed = cdat.maxspeed;

			VectorCopy(cdat.viewangles, cl.viewangles);
			VectorCopy(cdat.punchangle, cl.punchangle);

			scr_fov_value = cdat.fov;

			if (bits != cdat.iKeyBits && cdat.iKeyBits - bits != -1)
				Con_DPrintf("HUD changed ikeybits, xor %i diff %i\n", cdat.iKeyBits ^ bits, cdat.iKeyBits - bits);

			CL_ResetButtonBits(cdat.iKeyBits);
		}

		if (cls.demorecording)
			CL_WriteDLLUpdate(&oldcdat);
	}
}

/*
==============
ClientDLL_DemoUpdateClientData

Updates client data for demo
==============
*/
void ClientDLL_DemoUpdateClientData( client_data_t* cdat )
{
	if (cl_funcs.pHudUpdateClientDataFunc(cdat, cl.time))
	{
		cl.viewheight = cdat->viewheight;
		cl.maxspeed = cdat->maxspeed;

		VectorCopy(cdat->viewangles, cl.viewangles);
		VectorCopy(cdat->punchangle, cl.punchangle);

		scr_fov_value = cdat->fov;

		CL_ResetButtonBits(cdat->iKeyBits);
	}
}

/*
==============
ClientDLL_Reset

==============
*/
void ClientDLL_Reset( client_data_t* cdat )
{
	cl_funcs.pHudResetFunc();
}