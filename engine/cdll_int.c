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
#include "cl_draw.h"
#include "tmessage.h"
#include "hud_handlers.h"

client_sprite_t* SPR_GetList( char* psz, int* piCount );

void CL_ResetButtonBits( int bits );
int CL_ButtonBits( int );

// Global table of exported engine functions to client dll
cl_enginefunc_t cl_enginefuncs =
{
	SPR_Load,
	SPR_Frames,
	SPR_Height,
	SPR_Width,
	SPR_Set,
	SPR_Draw,
	SPR_DrawHoles,
	SPR_DrawAdditive,
	SPR_EnableScissor,
	SPR_DisableScissor,
	SPR_GetList,
	Draw_FillRGBA,
	GetScreenInfo,
	SetCrosshair,
	hudRegisterVariable,
	hudGetCvarFloat,
	hudGetCvarString,
	hudAddCommand,
	hudHookUserMsg,
	hudServerCmd,
	hudClientCmd,
	hudGetPlayerInfo,
	hudPlaySoundByName,
	hudPlaySoundByIndex,
	AngleVectors,
	TextMessageGet,
	TextMessageDrawCharacter,
	Draw_String,
	hudDrawConsoleStringLen,
	hudConsolePrint
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

#define LOAD_IFACE_FUNC(func, hModule, pszName)							\
	func = DECLTYPE(func)(GetProcAddress(hModule, pszName));			\
	if (!func)												\
		Sys_Error("could not link client.dll function " pszName "\n")

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

	LOAD_IFACE_FUNC(cl_funcs.pInitFunc, hModule, "Initialize");
	LOAD_IFACE_FUNC(cl_funcs.pHudVidInitFunc, hModule, "HUD_VidInit");
	LOAD_IFACE_FUNC(cl_funcs.pHudInitFunc, hModule, "HUD_Init");
	LOAD_IFACE_FUNC(cl_funcs.pHudRedrawFunc, hModule, "HUD_Redraw");
	LOAD_IFACE_FUNC(cl_funcs.pHudUpdateClientDataFunc, hModule, "HUD_UpdateClientData");
	LOAD_IFACE_FUNC(cl_funcs.pHudResetFunc, hModule, "HUD_Reset");

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
ClientDLL_HudReset

==============
*/
void ClientDLL_HudReset( void )
{
	cl_funcs.pHudResetFunc();
}

/*
=================
SPR_GetList

Loads a sprite list. This is a text file defining a list of HUD elements
Free the returned list with COM_FreeFile
=================
*/
client_sprite_t* SPR_GetList( char* psz, int* piCount )
{
	char* pfile;
	int		iCount, i;
	client_sprite_t* ps, * pret;

	pfile = (char*)COM_LoadFile(psz, 2, NULL);
	if (!pfile)
		return NULL;

	pfile = COM_Parse(pfile);
	iCount = atoi(com_token);
	if (!iCount)
		return NULL;
	
	pret = (client_sprite_t*)Hunk_Alloc(sizeof(client_sprite_t) * iCount);
	if (pret)
	{
		ps = pret;

		for (i = 0; i < iCount; i++, ps++)
		{
			pfile = COM_Parse(pfile);
			strcpy(ps->szName, com_token);

			pfile = COM_Parse(pfile);
			ps->iRes = atoi(com_token);

			pfile = COM_Parse(pfile);
			strcpy(ps->szSprite, com_token);

			pfile = COM_Parse(pfile);
			ps->rc.left = atoi(com_token);
			pfile = COM_Parse(pfile);
			ps->rc.top = atoi(com_token);
			pfile = COM_Parse(pfile);
			ps->rc.right = ps->rc.left + atoi(com_token);
			pfile = COM_Parse(pfile);
			ps->rc.bottom = ps->rc.top + atoi(com_token);
		}

		if (piCount)
			*piCount = iCount;
	}

	return pret;
}