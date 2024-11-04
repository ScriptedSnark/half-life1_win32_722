// sys_win.c -- Win32 system interface code

#include "quakedef.h"
#include "winquake.h"
#include "exefuncs.h"

#include <io.h>

DWORD				gProcessorSpeed;
int					gHasMMXTechnology;

#ifdef _WIN32
DLL_EXPORT void		S_GetDSPointer( LPDIRECTSOUND* lpDS, LPDIRECTSOUNDBUFFER* lpDSBuf );
DLL_EXPORT void		*S_GetWAVPointer( void );
#endif

qboolean			Win32AtLeastV4;

#ifdef _WIN32
HWND*				pmainwindow;
#endif

qboolean			g_bInStartup = FALSE;
qboolean			g_bInactive = FALSE;

char				g_szProfileName[MAX_QPATH];
qboolean			g_bForceReloadOnCA_Active = FALSE;

// 0 = not active, 1 = active, 2 = pause
int					giActive = DLL_INACTIVE;
int					giStateInfo;

static double		pfreq;
static double		curtime = 0.0;
static double		lastcurtime = 0.0;
static int			lowshift;
qboolean			isDedicated;

int					g_bPaused = 0;
int					gLauncherPause = 0;

void	(*VID_LockBuffer)( void );
void	(*VID_UnlockBuffer)( void );
void	(*VID_Shutdown)( void );
void	(*VID_Update)( struct vrect_s* rects );
int		(*VID_ForceUnlockedAndReturnState)( void );
void	(*VID_SetDefaultMode)( void );
char*	(*VID_GetExtModeDescription)( int mode );
int		(*VID_ForceLockState)( int lk );
void	(*VID_GetVID)( struct viddef_s* pvid );
int		(*D_SurfaceCacheForRes)( int width, int height );
void	(*D_BeginDirectRect)( int width, int height, byte* data, int pic_width, int pic_height );
void	(*D_EndDirectRect)( int width, int height, int pic_width, int pic_height );
void	(*AppActivate)( int fActive, int minimize );
void	(*CDAudio_Play)( int track, int looping );
void	(*CDAudio_Pause)( void );
void	(*CDAudio_Resume)( void );
void	(*CDAudio_Update)( void );
void	(*ErrorMessage)( int nLevel, const char* pszErrorMessage );
void	(*Console_Printf)( char* fmt, ... );
void	(*Launcher_InitCmds)( void );
void	(*Launcher_GetCDKey)( char* pszCDKey, int* nLength, int* bDedicated );
void	(*Launcher_GetClientID)( void* pID );
void	(*Launcher_GetUUID)( void* pUUID, int* nLength, int* bDedicated );
void*	(*Launcher_VerifyMessage)( int nLength, byte* pKey, int nMsgLength, char* pMsg, int nSignLength, byte* pSign );
int		(*Launcher_GetCertificate)( void* pBuffer, int* nLength );
int		(*Launcher_RequestNewClientCertificate)( void );
int		(*Launcher_ValidateClientCertificate)( void* pBuffer, int nLength );

void Sys_InitFloatTime( void );
void SeedRandomNumberGenerator( void );

void MaskExceptions( void );
void Sys_PushFPCW_SetHigh( void );
void Sys_PopFPCW( void );
void Sys_TruncateFPU( void );

volatile int					sys_checksum;


/*
================
Sys_PageIn
================
*/
void Sys_PageIn( void* ptr, int size )
{
	byte* x;
	int		m, n;

// touch all the memory to make sure it's there. The 16-page skip is to
// keep Win 95 from thinking we're trying to page ourselves in (we are
// doing that, of course, but there's no reason we shouldn't)
	x = (byte*)ptr;

	for (n = 0; n < 4; n++)
	{
		for (m = 0; m < (size - 16 * 0x1000); m += 4)
		{
			sys_checksum += *(int*)&x[m];
			sys_checksum += *(int*)&x[m + 16 * 0x1000];
		}
	}
}


/*
===============================================================================

FILE IO

===============================================================================
*/

#define	MAX_HANDLES		10
FILE* sys_handles[MAX_HANDLES];

int	findhandle( void )
{
	int		i;

	for (i = 1; i < MAX_HANDLES; i++)
		if (!sys_handles[i])
			return i;
	Sys_Error("out of handles");
	return -1;
}

/*
================
filelength
================
*/
int filelength( FILE* f )
{
	int		pos;
	int		end;
	int		t;

	t = VID_ForceUnlockedAndReturnState();

	pos = ftell(f);
	fseek(f, 0, SEEK_END);
	end = ftell(f);
	fseek(f, pos, SEEK_SET);

	VID_ForceLockState(t);

	return end;
}

int Sys_FileOpenRead( char* path, int* hndl )
{
	FILE* f;
	int		i, retval;
	int		t;

	t = VID_ForceUnlockedAndReturnState();

	i = findhandle();

	f = fopen(path, "rb");

	if (!f)
	{
		*hndl = -1;
		retval = -1;
	}
	else
	{
		sys_handles[i] = f;
		*hndl = i;
		retval = filelength(f);
	}

	VID_ForceLockState(t);

	return retval;
}

int Sys_FileOpenWrite( char* path )
{
	FILE* f;
	int		i;
	int		t;

	t = VID_ForceUnlockedAndReturnState();

	i = findhandle();

	f = fopen(path, "wb");
	if (!f)
		Sys_Error("Error opening %s: %s", path, strerror(errno));
	sys_handles[i] = f;

	VID_ForceLockState(t);

	return i;
}

void Sys_FileClose( int handle )
{
	int		t;

	t = VID_ForceUnlockedAndReturnState();
	fclose(sys_handles[handle]);
	sys_handles[handle] = NULL;
	VID_ForceLockState(t);
}

void Sys_FileSeek( int handle, int position )
{
	int		t;

	t = VID_ForceUnlockedAndReturnState();
	fseek(sys_handles[handle], position, SEEK_SET);
	VID_ForceLockState(t);
}

int Sys_FileRead( int handle, void* dest, int count )
{
	int		t, x;

	t = VID_ForceUnlockedAndReturnState();
	x = fread(dest, 1, count, sys_handles[handle]);
	VID_ForceLockState(t);
	return x;
}

int Sys_FileWrite( int handle, void* data, int count )
{
	int		t, x;
	
	t = VID_ForceUnlockedAndReturnState();
	x = fwrite(data, 1, count, sys_handles[handle]);
	VID_ForceLockState(t);
	return x;
}

int	Sys_FileTime( char* path )
{
	FILE* f;
	int		t, retval;

	t = VID_ForceUnlockedAndReturnState();

	f = fopen(path, "rb");

	if (f)
	{
		fclose(f);
		retval = 1;
	}
	else
	{
		retval = -1;
	}

	VID_ForceLockState(t);
	return retval;
}

void Sys_mkdir( char* path )
{
	_mkdir(path);
}

int Sys_FileTell( int i )
{
	return ftell(sys_handles[i]);
}


/*
===============================================================================

SYSTEM IO

===============================================================================
*/

/*
================
Sys_MakeCodeWriteable
================
*/
void Sys_MakeCodeWriteable( unsigned long startaddr, unsigned long length )
{
	DWORD  flOldProtect;

	if (!VirtualProtect((LPVOID)startaddr, length, PAGE_READWRITE, &flOldProtect))
		Sys_Error("Protection change failed\n");
}


#ifndef _M_IX86

void Sys_SetFPCW( void )
{
}

void Sys_PushFPCW_SetHigh( void )
{
}

void Sys_PopFPCW( void )
{
}

void Sys_TruncateFPU( void )
{
}

void MaskExceptions( void )
{
}

#endif

/*
================
Sys_Init
================
*/
void Sys_Init( void )
{
	LARGE_INTEGER	PerformanceFreq;
	unsigned int	lowpart, highpart;

	MaskExceptions();
	Sys_SetFPCW();

	if (!QueryPerformanceFrequency(&PerformanceFreq))
		Sys_Error("No hardware timer available");

// get 32 out of the 64 time bits such that we have around
// 1 microsecond resolution
	lowpart = (unsigned int)PerformanceFreq.LowPart;
	highpart = (unsigned int)PerformanceFreq.HighPart;
	lowshift = 0;

	while (highpart || (lowpart > 2000000.0))
	{
		lowshift++;
		lowpart >>= 1;
		lowpart |= (highpart & 1) << 31;
		highpart >>= 1;
	}

	pfreq = 1.0 / (double)lowpart;

	Sys_InitFloatTime();
}


void Sys_Error( char* error, ... )
{
	va_list		argptr;
	char		text[1024];
	static qboolean bReentry = FALSE; // Don't meltdown

	va_start(argptr, error);
	vsprintf(text, error, argptr);
	va_end(argptr);

	if (bReentry)
	{
		fprintf(stderr, "%s\n", text);
		longjmp(host_abortserver, 2);
	}

	bReentry = TRUE;

	VID_ForceUnlockedAndReturnState();

	if (isDedicated)
	{
		vsprintf(text, error, argptr);
		if (Console_Printf)
		{
			Console_Printf("Error %s\n", text);
		}
	}
	else
	{
		if (ErrorMessage)
		{
			ErrorMessage(0, text);
		}
	}
}

void Sys_Warning( char* fmt, ... )
{
	va_list		argptr;
	char		text[1024];

	va_start(argptr, fmt);
	vsprintf(text, fmt, argptr);
	va_end(argptr);

	VID_ForceUnlockedAndReturnState();

	if (isDedicated)
	{
		vsprintf(text, fmt, argptr);
		if (Console_Printf)
		{
			Console_Printf("WARNING:  %s\n", text);
		}
	}
	else
	{
		if (ErrorMessage)
		{
			ErrorMessage(1, text);
		}
	}

	giActive = DLL_PAUSED;
}

void Sys_Printf( char* fmt, ... )
{
	va_list		argptr;
	char		text[1024];

	if (isDedicated)
	{
		va_start(argptr, fmt);
		vsprintf(text, fmt, argptr);
		va_end(argptr);

		if (Console_Printf)
		{
			Console_Printf(text);
		}
	}
}

void Sys_Quit( void )
{
	VID_ForceUnlockedAndReturnState();

	Host_Shutdown();

	if (isDedicated)
		FreeConsole();

	giActive = DLL_CLOSE;

	longjmp(host_abortserver, 1);
}


/*
================
Sys_FloatTime
================
*/
DLL_EXPORT double Sys_FloatTime( void )
{
	static int			sametimecount;
	static unsigned int	oldtime;
	static int			first = 1;
	LARGE_INTEGER		PerformanceCount;
	unsigned int		temp, t2;
	double				time;

	Sys_PushFPCW_SetHigh();

	QueryPerformanceCounter(&PerformanceCount);

	temp = ((unsigned int)PerformanceCount.LowPart >> lowshift) |
		((unsigned int)PerformanceCount.HighPart << (32 - lowshift));

	if (first)
	{
		oldtime = temp;
		first = 0;
	}
	else
	{
	// check for turnover or backward time
		if ((temp <= oldtime) && ((oldtime - temp) < 0x10000000))
		{
			oldtime = temp;	// so we can't get stuck
		}
		else
		{
			t2 = temp - oldtime;

			time = (double)t2 * pfreq;
			oldtime = temp;

			curtime += time;

			if (curtime == lastcurtime)
			{
				sametimecount++;

				if (sametimecount > 100000)
				{
					curtime += 1.0;
					sametimecount = 0;
				}
			}
			else
			{
				sametimecount = 0;
			}

			lastcurtime = curtime;
		}
	}

	Sys_PopFPCW();

	return curtime;
}


/*
================
Sys_InitFloatTime
================
*/
void Sys_InitFloatTime( void )
{
	int		j;

	Sys_FloatTime();

	j = COM_CheckParm("-starttime");

	if (j)
	{
		curtime = (double)(Q_atof(com_argv[j + 1]));
	}
	else
	{
		curtime = 0.0;
	}

	lastcurtime = curtime;
}

void Sys_Sleep( void )
{
	Sleep(1);
}


void Sys_SendKeyEvents( void )
{
	MSG        msg;

	if (g_bInStartup)
		return;

	while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
	// we always update if there are any event, even if we're paused
		scr_skipupdate = 0;
		if (g_bInactive)
			break;

		if (!GetMessage(&msg, NULL, 0, 0))
			Sys_Quit();

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (!g_bInactive)
		return;

	while (PeekMessage(&msg, NULL, WM_ACTIVATEAPP, WM_ACTIVATEAPP, PM_NOREMOVE))
	{
	// we always update if there are any event, even if we're paused
		scr_skipupdate = 0;

		if (!GetMessage(&msg, NULL, 0, 0))
			Sys_Quit();

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

/*
==============================================================================

 WINDOWS CRAP

==============================================================================
*/


/*
==================
WinMain
==================
*/
HINSTANCE	global_hInstance;
char* argv[MAX_NUM_ARGVS];
static char* empty_string = "";

void VID_Stub( void )
{
}

BOOL WINAPI DllMain(
	HINSTANCE hinstDLL,
	DWORD fdwReason,
	LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		global_hInstance = hinstDLL;
	}
	else if (fdwReason == DLL_PROCESS_DETACH)
	{
		VID_LockBuffer = VID_Stub;
		VID_UnlockBuffer = VID_Stub;
		VID_Shutdown = VID_Stub;

		ReleaseEntityDlls();
	}
	return TRUE;
}

int	giSubState;

void Dispatch_Substate( int iSubState )
{
	giSubState = iSubState;
}

DLL_EXPORT void GameSetSubState( int iSubState )
{
	if (iSubState & 2)
	{
		Dispatch_Substate(1);
	}
	else if (iSubState != 1)
	{
		Dispatch_Substate(iSubState);
	}
}

DLL_EXPORT void GameSetState( int iState )
{
	giActive = iState;
}

qboolean gfBackground;

DLL_EXPORT void GameSetBackground( qboolean bNewSetting )
{
	gfBackground = bNewSetting;
}

// Called when the game starts up
DLL_EXPORT int GameInit( char* lpCmdLine, unsigned char* pMem, int iSize, exefuncs_t* pef, void* pwnd, char* profile, int bIsDedicated )
{
	quakeparms_t	parms;
	static	char	cwd[1024];

	host_initialized = FALSE;

	memset(g_szProfileName, 0, sizeof(g_szProfileName));
	strcpy(g_szProfileName, profile);

	// Grab main window pointer
	pmainwindow = (HWND*)pwnd;

	// Initialize exe funcs
	Console_Printf							= pef->Console_Printf;

	VID_LockBuffer							= pef->VID_LockBuffer;
	VID_UnlockBuffer						= pef->VID_UnlockBuffer;
	VID_Shutdown							= pef->VID_Shutdown;
	VID_Update								= pef->VID_Update;
	VID_ForceLockState						= pef->VID_ForceLockState;
	VID_ForceUnlockedAndReturnState			= pef->VID_ForceUnlockedAndReturnState;
	VID_SetDefaultMode						= pef->VID_SetDefaultMode;
	VID_GetExtModeDescription				= pef->VID_GetExtModeDescription;

	D_SurfaceCacheForRes					= pef->D_SurfaceCacheForRes;

	VID_GetVID								= pef->VID_GetVID;
	
	D_BeginDirectRect						= pef->D_BeginDirectRect;
	D_EndDirectRect							= pef->D_EndDirectRect;

	AppActivate								= pef->AppActivate;

	CDAudio_Play							= pef->CDAudio_Play;
	CDAudio_Resume							= pef->CDAudio_Resume;
	CDAudio_Pause							= pef->CDAudio_Pause;
	CDAudio_Update							= pef->CDAudio_Update;

	gHasMMXTechnology						= pef->fMMX;
	gProcessorSpeed							= pef->iCPUMhz;

	ErrorMessage							= pef->ErrorMessage;

	Launcher_InitCmds						= pef->Launcher_InitCmds;
	Launcher_GetCDKey						= pef->Launcher_GetCDKey;
	Launcher_GetClientID					= pef->Launcher_GetClientID;
	Launcher_VerifyMessage					= pef->Launcher_VerifyMessage;

	Launcher_GetCertificate					= pef->Launcher_GetCertificate;
	Launcher_RequestNewClientCertificate	= pef->Launcher_RequestNewClientCertificate;
	Launcher_ValidateClientCertificate		= pef->Launcher_ValidateClientCertificate;

	// Check that we are running on Win32
	OSVERSIONINFO vinfo;
	vinfo.dwOSVersionInfoSize = sizeof(vinfo);

	if (!GetVersionEx(&vinfo))
	{
		Sys_Error("Couldn't get OS info");
	}

	if (vinfo.dwMajorVersion < 4)
	{
		Win32AtLeastV4 = FALSE;
	}
	else
	{
		Win32AtLeastV4 = TRUE;
	}

	SeedRandomNumberGenerator();

	// Set default FPU control word to truncate (chop) mode for optimized _ftol()
	// This does not "stick", the mode is restored somewhere down the line.
	Sys_TruncateFPU();

	if (!GetCurrentDirectory(sizeof(cwd), cwd))
		Sys_Error("Couldn't determine current directory");

	if (cwd[Q_strlen(cwd) - 1] == '/')
		cwd[Q_strlen(cwd) - 1] = 0;

	parms.basedir = cwd;
	parms.cachedir = NULL;

	parms.argc = 1;
	argv[0] = empty_string;

	while (*lpCmdLine && (parms.argc < MAX_NUM_ARGVS))
	{
		while (*lpCmdLine && ((*lpCmdLine <= 32) || (*lpCmdLine > 126)))
			lpCmdLine++;

		if (*lpCmdLine)
		{
			argv[parms.argc] = lpCmdLine;
			parms.argc++;

			while (*lpCmdLine && ((*lpCmdLine > 32) && (*lpCmdLine <= 126)))
				lpCmdLine++;

			if (*lpCmdLine)
			{
				*lpCmdLine = 0;
				lpCmdLine++;
			}

		}
	}

	parms.argv = argv;

	COM_InitArgv(parms.argc, parms.argv);

	parms.argc = com_argc;
	parms.argv = com_argv;

	// Remember that this is a dedicated server
	isDedicated = bIsDedicated;

	parms.membase = pMem;
	parms.memsize = iSize;

	Sys_Init();

// because sound is off until we become active
	S_BlockSound();

	Sys_Printf("Host_Init\n");
	if (!Host_Init(&parms))
		return FALSE;

	// Load in the game .dll
	LoadEntityDLLs(host_parms.basedir);

	// Start up networking
	NET_Config(TRUE);

	/* return success of application */
	return TRUE;
}


DLL_EXPORT int GetGameInfo( struct GameInfo_s* pGI, char* pszChannel )
{
	// TODO: Implement
	return 1;
}

void ReloadProfile( char* pszPlayerName )
{
	// TODO: Implement
}

DLL_EXPORT void ForceReloadProfile( char* pszPlayerName )
{
	memset(g_szProfileName, 0, sizeof(g_szProfileName));
	strcpy(g_szProfileName, pszPlayerName);

	g_bForceReloadOnCA_Active = TRUE;
	ReloadProfile(pszPlayerName);
}

DLL_EXPORT void SetStartupMode( qboolean bMode )
{
	g_bInStartup = bMode;
}

DLL_EXPORT void SetMessagePumpDisableMode( qboolean bMode )
{
	g_bInactive = bMode;
}

DLL_EXPORT void SetPauseState( qboolean bPause )
{
	if (bPause)
	{
		// TODO: Implement
	}
	else
	{
		// TODO: Implement
	}
}

DLL_EXPORT int GetPauseState( void )
{
	// TODO: Implement
	return 0;
}

DLL_EXPORT void Keyboard_ReturnToGame( void )
{
	Con_DPrintf("KB Reset\n");
}

// This is a table of exported engine functions to server dll
// NOTE: New functions come imidieately after the last one
static enginefuncs_t g_engfuncsExportedToDlls =
{
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	AlertMessage,
	EngineFprintf,
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL,	// TODO: Implement
	NULL	// TODO: Implement
};

extensiondll_t		g_rgextdll[MAX_EXT_DLLS];
int					g_iextdllMac;

DLL_FUNCTIONS		gEntityInterface;

// Gets a dllexported function from the first DLL that exports it
// Returns functions to dispatch events to entities
DISPATCHFUNCTION GetDispatch( char* pname )
{
	int i;
	DISPATCHFUNCTION pDispatch;

	for (i = 0; i < g_iextdllMac; i++)
	{
		pDispatch = (DISPATCHFUNCTION)GetProcAddress((HMODULE)g_rgextdll[i].lDLLHandle, pname);
		if (pDispatch)
		{
			return pDispatch;
		}
	}

	return NULL;
}

const char* FindAddressInTable( extensiondll_t* pDll, uint32 function )
{
	int	i;

	for (i = 0; i < pDll->functionCount; i++)
	{
		if (pDll->functionTable[i].pFunction == function)
			return pDll->functionTable[i].pFunctionName;
	}

	return NULL;
}

uint32 FindNameInTable( extensiondll_t* pDll, const char* pName )
{
	int	i;

	for (i = 0; i < pDll->functionCount; i++)
	{
		if (!strcmp(pName, pDll->functionTable[i].pFunctionName))
			return pDll->functionTable[i].pFunction;
	}

	return 0;
}

// Gets the index of an exported function
uint32 FunctionFromName( const char* pName )
{
	int	i;
	uint32 function;

	for (i = 0; i < g_iextdllMac; i++)
	{
		function = FindNameInTable(&g_rgextdll[i], pName);
		if (function)
		{
			return function;
		}
	}

	Con_Printf("Can't find proc: %s\n", pName);
	return 0;
}

// Gets the name of an exported function
const char* NameForFunction( uint32 function )
{
	int i;
	const char* pName;

	for (i = 0; i < g_iextdllMac; i++)
	{
		pName = FindAddressInTable(&g_rgextdll[i], function);
		if (pName)
		{
			return pName;
		}
	}

	Con_Printf("Can't find address: %08lx\n", function);
	return NULL;
}

// Gets a dllexported function from the first DLL that exports it.
// Returns entity initialization functions, generated by LINK_ENTITY_TO_CLASS
ENTITYINIT GetEntityInit( char* pClassName )
{
	return (ENTITYINIT)GetDispatch(pClassName);
}

// Gets a dllexported function from the first DLL that exports it
FIELDIOFUNCTION GetIOFunction( char* pName )
{
	return (FIELDIOFUNCTION)GetDispatch(pName);
}

//
// Scan DLL directory, load all DLLs that conform to spec.
//
void LoadEntityDLLs( char* szBaseDir )
{
	char szDllWildcard[MAX_PATH];
	int i;

	APIFUNCTION pfnGetAPI;
	int interface_version;

	// vars for .gam file loading:
	char* pszInputStream;
	char* pStreamPos;
	char szDllListFile[MAX_PATH];
	int	 nFileSize;
	int  nBytesRead;
	int	 hLibListFile;
	char szGameDir[64];

	char szDllFilename[8192];

	g_iextdllMac = 0;
	memset(g_rgextdll, 0, sizeof(g_rgextdll));

	if (COM_CheckParm("-game") || COM_CheckParm("-usegamelist"))
	{
	//
	// -game <gamedir>
	// Adds basedir/gamedir as an override game
	//
		i = COM_CheckParm("-game");
		if (i && i < com_argc - 1)
		{
			strcpy(szGameDir, com_argv[i + 1]);
		}
		else
		{
			strcpy(szGameDir, "valve");
		}

		sprintf(szDllListFile					// Listing file for this game.
			, "%s\\%s\\%s"
			, szBaseDir
			, szGameDir
			, GAME_LIST_FILE);

		nFileSize = Sys_FileOpenRead(szDllListFile, &hLibListFile);
		if (nFileSize == -1)
			Sys_Error("Could not load game listing file [%s],make sure file is in the [%s] subdirectory.", GAME_LIST_FILE, szBaseDir);

		if (nFileSize == 0 ||
			nFileSize > 1024 * 256)				// 0 or 256K .gam file is probably bogus
			Sys_Error("Game listing file size is bogus [%s: size %i]", "liblist.gam", nFileSize);

		pszInputStream = (char*)malloc(nFileSize + 1);  // Leave room for terminating 0 just in case
		if (!pszInputStream)
			Sys_Error("Could not allocate space for game listing file of %i bytes", nFileSize + 1);

		nBytesRead = Sys_FileRead(
			hLibListFile,
			pszInputStream,
			nFileSize);

		if (nBytesRead != nFileSize)			// Check amound actually read
			Sys_Error("Error reading in game listing file, expected %i bytes, read %i", nFileSize, nBytesRead);

		pszInputStream[nFileSize] = '\0';		// Prevent overrun
		pStreamPos = pszInputStream;

		while (1)
		{
			// Skip the first two tokens:  game "Half-Life" which are used by the front end to determine the game type for this liblist.gam
			com_ignorecolons = TRUE;
			pStreamPos = COM_Parse(pStreamPos);
			com_ignorecolons = FALSE;
			if (!pStreamPos)  // Done
				break;

			strcpy(szDllFilename, com_token);
			if (!strstr(szDllFilename, ".dll"))
			{
				Con_Printf("Skipping non-dll:  %s\n", szDllFilename);
				continue;
			}

			Con_Printf("Adding:  %s\n", szDllFilename);
			LoadThisDll(szDllFilename);
		}

		free(pszInputStream);

		Sys_FileClose(hLibListFile);
	}
	else
	{
		// Load the game .dll
		sprintf(szDllWildcard, "%s\\%s\\*.dll", szBaseDir, "valve\\dlls");

		struct _finddata_t findData;
		intptr_t findfn = _findfirst(szDllWildcard, &findData);
		if (findfn != -1)
		{
			do
			{
				sprintf(szDllFilename, "%s\\%s\\%s", szBaseDir, "valve\\dlls", findData.name);
				LoadThisDll(szDllFilename);
			} while (_findnext(findfn, &findData) == 0);
		}
		_findclose(findfn);
	}

	// Check if it provides the original dll functions
	pfnGetAPI = (APIFUNCTION)GetDispatch("GetEntityAPI");
	if (!pfnGetAPI)
		Sys_Error("Can't get DLL API!");

	interface_version = INTERFACE_VERSION;
	if (!pfnGetAPI(&gEntityInterface, interface_version))
		Sys_Error("Invalid DLL version!");

	Con_Printf("----------------------\n");
	Con_Printf("Dlls loaded for game:\n%s\n", gEntityInterface.pfnGetGameDescription());
	Con_Printf("----------------------\n");
}

//
// Try to load a single DLL.  If it conforms to spec, keep it loaded, and add relevant
// info to the DLL directory.  If not, ignore it entirely.
//
void LoadThisDll( char* szDllFilename )
{
	HMODULE hDLL;
	extensiondll_t* pextdll;

	hDLL = LoadLibrary(szDllFilename);

	if (hDLL == NULL)
	{
		Con_Printf("LoadLibrary failed on %s (%d)\n", szDllFilename, GetLastError());
		return;
	}

	typedef void (DLLEXPORT* PFN_GiveFnptrsToDll)(enginefuncs_t*, globalvars_t*);
	PFN_GiveFnptrsToDll pfnGiveFnptrsToDll = (PFN_GiveFnptrsToDll)GetProcAddress(hDLL, "GiveFnptrsToDll");

	if (!pfnGiveFnptrsToDll)
	{
		Con_Printf("Couldn't get GiveFnptrsToDll in %s\n", szDllFilename);
		FreeLibrary(hDLL);
		return;
	}

	pfnGiveFnptrsToDll(&g_engfuncsExportedToDlls, &gGlobalVariables);

	if (g_iextdllMac == MAX_EXT_DLLS)
	{
		Con_Printf("Too many DLLs, ignoring remainder\n");
		FreeLibrary(hDLL);
		return;
	}

	pextdll = &g_rgextdll[g_iextdllMac++];

	memset(pextdll, 0, sizeof(*pextdll));
	pextdll->lDLLHandle = hDLL;

	BuildExportTable(pextdll, szDllFilename);
}


//
// Release all entity dlls
//
void ReleaseEntityDlls( void )
{
	extensiondll_t* pextdll = g_rgextdll;
	extensiondll_t* pextdllMac = &g_rgextdll[g_iextdllMac];

	while (pextdll < pextdllMac)
	{
		FreeLibrary(pextdll->lDLLHandle);
		pextdll->lDLLHandle = NULL;

		pextdll++;
	}
}

void EngineFprintf( void* pFile, char* szFmt, ... )
{
	va_list		argptr;
	static char szOut[1024];

	va_start(argptr, szFmt);
	vsprintf(szOut, szFmt, argptr);
	va_end(argptr);

	fprintf(pFile, szOut);
}

// Outputs a message to the server console
// If aType is at_logged and this is a multiplayer game, logs the message to the log file
void AlertMessage( ALERT_TYPE atype, char* szFmt, ... )
{
	va_list		argptr;
	static char szOut[1024];

	if (!developer.value)
		return;

	va_start(argptr, szFmt);
	vsprintf(szOut, szFmt, argptr);
	va_end(argptr);

	switch (atype)
	{
	case at_notice:
		Con_Printf("NOTE:  %s", szOut);
		break;
	case at_console:
		Con_Printf("%s", szOut);
		break;
	case at_aiconsole:
		if (developer.value < 2)
			return;
		Con_Printf("%s", szOut);
		break;
	case at_warning:
		Con_Printf("WARNING:  %s", szOut);
		break;
	case at_error:
		Con_Printf("ERROR:  %s", szOut);
		break;
	}
}