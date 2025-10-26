//=============================================
// HALF-LIFE NET TEST 1 LAUNCHER REIMPLEMENTATION
// No GUI version
//
// Author: ScriptedSnark
// File: launcher.c
//=============================================

#include "launcher.h"

#define HARDWARE_ENGINE "hw.so"
#define SOFTWARE_ENGINE "sw.so"

#define DEFAULT_MEM_MB 32
#define DEFAULT_MEM_SIZE (DEFAULT_MEM_MB * 1024 * 1024)

#define LOAD_IFACE_FUNC(func, hModule, pszName)							\
	func = DECLTYPE(func)(Sys_GetProcAddress(hModule, pszName));			\
	if (!func)												\
		Sys_Error("could not link engine function " pszName "\n")

char szBaseDir[4096];

void  (*Cbuf_AddText)( char* text );
void  (*Cbuf_InsertText)( char* text );
void  (*Cmd_AddCommand)( char* cmd_name, xcommand_t function );
int   (*Cmd_Argc)( void );
char* (*Cmd_Argv)( int arg );
char* (*Cmd_Args)( void );
int   (*COM_CheckParmEx)( char* parm, char** argv );
void  (*DeinitConProc)( void );
int   (*Con_GetInput)( char* text, int linepos );
void  (*Con_Printf)( char* fmt, ... );
void  (*Con_DPrintf)( char* fmt, ... );
void  (*Con_SafePrintf)( char* fmt, ... );
void  (*Cvar_Set)( char* var_name, char* value );
void  (*Cvar_SetValue)( char* var_name, float value );
float (*Cvar_VariableValue)( char* var_name );
int   (*Cvar_VariableInt)( char* var_name );
char* (*Cvar_VariableString)( char* var_name );
char* (*Cvar_CompleteVariable)( char* partial );
int   (*GL_SetMode)( SDL_Window* mainwindow );
void  (*GL_Shutdown)( SDL_Window* window );
void  (*VID_UpdateWindowVars)( void* prc, int x, int y );
void  (*VID_UpdateVID)( viddef_t* pvid );
int   (*VID_AllocBuffers)( void );
void  (*Host_GetHostInfo)( float* fps, int* nActive, int* nBots, int* nMaxPlayers, char* pszMap );
int   (*Host_Frame)( float time, int iState, int* stateInfo );
BOOL  (*SaveGame)( char* pszSlot, char* pszComment );
int   (*LoadGame)( char* pName );
void  (*IN_ClearStates)( void );
void  (*Key_UpLine)( void );
void  (*Key_Event)( int key, qboolean down );
void  (*Key_ClearStates)( void );
void  (*Download4444)( void );
void  (*QGL_D3DShared)( void* d3dGShared );
void  (*GL_Init)( void );
BOOL (  *qwglSwapBuffers )( void* );
void  (*Host_Shutdown)( void );
void  (*S_ClearBuffer)( void );
void  (*SNDDMA_Shutdown)( void );
void  (*Snd_ReleaseBuffer)( void );
void  (*Snd_AcquireBuffer)( void );
double (*Sys_FloatTime)( void );
void  (*S_GetDSPointer)( void* lpDS, void* lpDSBuf );
void* (*S_GetWAVPointer)( void );
double (*Sys_FloatTime)( void );
int   (*GetEngineState)( void );
void  (*GameSetSubState)( int iSubState );
void  (*GameSetState)( int iState );
void  (*GameSetBackground)( qboolean bNewSetting );
int   (*GameInit)( char* lpCmdLine, unsigned char* pMem, int iSize, exefuncs_t* pef, void* pwnd, char* profile, int bIsDedicated, char* szBaseDir  );
int   (*GetGameInfo)( GameInfo_t* pGI, char* pszChannel );
void  (*ForceReloadProfile)( char* pszName );
void  (*SetStartupMode)( qboolean bMode );
void  (*SetMessagePumpDisableMode)( qboolean bMode );
void  (*SetPauseState)( qboolean bPause );
int   (*GetPauseState)( void );
void  (*Keyboard_ReturnToGame)( void );
void  (*IN_ShowMouse)( void );
void  (*IN_DeactivateMouse)( void );
void  (*IN_HideMouse)( void );
void  (*IN_ActivateMouse)( void );
void  (*IN_MouseEvent)( int mstate );
void  (*IN_UpdateClipCursor)( void );
void  (*S_BlockSound)( void );
void  (*S_UnblockSound)( void );

SDL_Window* Launcher_CreateWindow();

VidTypes g_vidType = VT_None;
int gDLLState, gDLLStateInfo;
qboolean gfInLoad;
void* gpMemBase;
size_t giMemSize;

char sys_cmdline[2048];

SDL_Window* gWindow = NULL;
void* hDC = NULL;
void* hRC = NULL;
qboolean in_mode_set = FALSE;
int g_iScreenWidth, g_iScreenHeight;

qboolean IsHardwareEngine(void)
{
	return (g_vidType == VT_OpenGL || g_vidType == VT_Direct3D);
}

qboolean AllocGameMem(void)
{
	char* pszValue = NULL;

	giMemSize = DEFAULT_MEM_SIZE;

	if (CheckParm("-heapsize", &pszValue) && pszValue)
		giMemSize = atoi(pszValue);

	gpMemBase = malloc(giMemSize);

	return (gpMemBase != NULL);
}

void Eng_SetGameState(int state)
{
    gDLLState = state;
    if ( GameSetState )
        GameSetState(state);
}

void Eng_SetSubGameState(int state)
{
    gDLLStateInfo = state;
    if ( GameSetSubState )
        GameSetSubState(state);
}

// thanks, hlds_l (1999)
void Eng_Frame(qboolean forceUpdate)
{
    static qboolean fDeferedPause = FALSE;
    static int iWait = 0;
    static double oldtime = 0.0;

	if (gDLLState != DLL_INACTIVE || forceUpdate)
	{
		if (gDLLState != DLL_INACTIVE)
		{
			double dt = Sys_FloatTime();
			double frametime = dt - oldtime;
			if (frametime < 0.0) frametime = 0.02;

			gDLLStateInfo = 0;
			int iDLLState = Host_Frame(frametime, gDLLState, &gDLLStateInfo);

			if (gDLLStateInfo)
			{
				switch (gDLLStateInfo)
				{
				default:
				case 0:
					break;
				case STATE_TRAINING:
					break;
				case STATE_ENDLOGO:
					break;
				case STATE_ENDDEMO:
					break;
				}
			}

			if (gDLLStateInfo == DLL_QUIT)
			{
				Eng_Unload();
			}

			if (iWait > 0)
			{
				iWait--;

				if (iDLLState == DLL_PAUSED)
				{
					fDeferedPause = TRUE;
					Eng_SetGameState(DLL_ACTIVE);
					iDLLState = DLL_ACTIVE;
				}
				if (iDLLState == DLL_TRANS)
				{
					iWait = 5;
					iDLLState = DLL_ACTIVE;
					Eng_SetGameState(DLL_ACTIVE);
				}

				if (iWait == 0 && fDeferedPause)
				{
					iDLLState = DLL_PAUSED;
					gDLLState = DLL_ACTIVE;
					fDeferedPause = FALSE;
				}
			}

			if (iDLLState == DLL_RESTART)
			{
				iDLLState = DLL_ACTIVE;
				iWait = 5;
				Eng_SetGameState(DLL_ACTIVE);
			}

			if (iDLLState != gDLLState)
			{
				Eng_SetGameState(iDLLState);
			}

			oldtime = dt;
		}

		if (gDLLState == DLL_CLOSE)
		{
			Eng_Unload();
		}
	}
}

qboolean Eng_Load(const char *szDllName, int subState)
{
    char *CommandLine;
    dllhandle_t hModule;
	char *width, *height;
	
	if (CheckParm("-w", &width) || CheckParm("-width", &width))
		g_iScreenWidth = atoi(width);
	else
		g_iScreenWidth = 640;

	if (CheckParm("-h", &height) || CheckParm("-height", &height))
		g_iScreenHeight = atoi(height);
	else
		g_iScreenHeight = 480;
	
	in_mode_set = TRUE;

    // load engine funcs
    hModule = Sys_LoadLibrary(szDllName);
	if (!hModule)
		Sys_Error("could not load library %s", szDllName);

    LOAD_IFACE_FUNC(LoadGame, hModule, "LoadGame");
    LOAD_IFACE_FUNC(SaveGame, hModule, "SaveGame");
    LOAD_IFACE_FUNC(GameInit, hModule, "GameInit");
    LOAD_IFACE_FUNC(GameSetState, hModule, "GameSetState");
    LOAD_IFACE_FUNC(GameSetBackground, hModule, "GameSetBackground");
    LOAD_IFACE_FUNC(GameSetSubState, hModule, "GameSetSubState");
    LOAD_IFACE_FUNC(Host_Frame, hModule, "Host_Frame");
    LOAD_IFACE_FUNC(Host_Shutdown, hModule, "Host_Shutdown");
    LOAD_IFACE_FUNC(Key_Event, hModule, "Key_Event");
    LOAD_IFACE_FUNC(Key_ClearStates, hModule, "Key_ClearStates");
    LOAD_IFACE_FUNC(Sys_FloatTime, hModule, "Sys_FloatTime");
    LOAD_IFACE_FUNC(IN_ClearStates, hModule, "IN_ClearStates");
    LOAD_IFACE_FUNC(IN_MouseEvent, hModule, "IN_MouseEvent");
    LOAD_IFACE_FUNC(IN_HideMouse, hModule, "IN_HideMouse");
    LOAD_IFACE_FUNC(IN_ActivateMouse, hModule, "IN_ActivateMouse");
    LOAD_IFACE_FUNC(IN_ShowMouse, hModule, "IN_ShowMouse");
    LOAD_IFACE_FUNC(IN_DeactivateMouse, hModule, "IN_DeactivateMouse");
    LOAD_IFACE_FUNC(IN_UpdateClipCursor, hModule, "IN_UpdateClipCursor");
    LOAD_IFACE_FUNC(Snd_AcquireBuffer, hModule, "Snd_AcquireBuffer");
    LOAD_IFACE_FUNC(Snd_ReleaseBuffer, hModule, "Snd_ReleaseBuffer");
    LOAD_IFACE_FUNC(S_UnblockSound, hModule, "S_UnblockSound");
    LOAD_IFACE_FUNC(S_BlockSound, hModule, "S_BlockSound");
    LOAD_IFACE_FUNC(S_ClearBuffer, hModule, "S_ClearBuffer");
    LOAD_IFACE_FUNC(SNDDMA_Shutdown, hModule, "SNDDMA_Shutdown");
    //LOAD_IFACE_FUNC(DeinitConProc, hModule, "DeinitConProc");
    LOAD_IFACE_FUNC(Con_Printf, hModule, "Con_Printf");
    LOAD_IFACE_FUNC(Con_SafePrintf, hModule, "Con_SafePrintf");
    LOAD_IFACE_FUNC(VID_AllocBuffers, hModule, "VID_AllocBuffers");
    LOAD_IFACE_FUNC(VID_UpdateVID, hModule, "VID_UpdateVID");
    LOAD_IFACE_FUNC(VID_UpdateWindowVars, hModule, "VID_UpdateWindowVars");
    LOAD_IFACE_FUNC(Cvar_Set, hModule, "Cvar_Set");
    LOAD_IFACE_FUNC(Cvar_SetValue, hModule, "Cvar_SetValue");
    LOAD_IFACE_FUNC(Cvar_VariableValue, hModule, "Cvar_VariableValue");
    LOAD_IFACE_FUNC(Cvar_VariableInt, hModule, "Cvar_VariableInt");
    LOAD_IFACE_FUNC(Cvar_VariableString, hModule, "Cvar_VariableString");
    LOAD_IFACE_FUNC(Cvar_CompleteVariable, hModule, "Cvar_CompleteVariable");
    LOAD_IFACE_FUNC(COM_CheckParmEx, hModule, "COM_CheckParmEx");
    LOAD_IFACE_FUNC(GetEngineState, hModule, "GetEngineState");
    LOAD_IFACE_FUNC(Cbuf_AddText, hModule, "Cbuf_AddText");
    LOAD_IFACE_FUNC(Cbuf_InsertText, hModule, "Cbuf_InsertText");
    LOAD_IFACE_FUNC(Cmd_AddCommand, hModule, "Cmd_AddCommand");
    LOAD_IFACE_FUNC(Cmd_Argc, hModule, "Cmd_Argc");
    LOAD_IFACE_FUNC(Cmd_Argv, hModule, "Cmd_Argv");
    LOAD_IFACE_FUNC(Cmd_Args, hModule, "Cmd_Args");
    //LOAD_IFACE_FUNC(qwglSwapBuffers, hModule, "qwglSwapBuffers");
    LOAD_IFACE_FUNC(GL_Init, hModule, "GL_Init");
    LOAD_IFACE_FUNC(GL_SetMode, hModule, "GL_SetMode");
    LOAD_IFACE_FUNC(GL_Shutdown, hModule, "GL_Shutdown");
    LOAD_IFACE_FUNC(GetGameInfo, hModule, "GetGameInfo");
    //LOAD_IFACE_FUNC(QGL_D3DShared, hModule, "QGL_D3DShared");
    LOAD_IFACE_FUNC(ForceReloadProfile, hModule, "ForceReloadProfile");
    LOAD_IFACE_FUNC(SetStartupMode, hModule, "SetStartupMode");
    LOAD_IFACE_FUNC(SetPauseState, hModule, "SetPauseState");
    LOAD_IFACE_FUNC(GetPauseState, hModule, "GetPauseState");
    LOAD_IFACE_FUNC(Keyboard_ReturnToGame, hModule, "Keyboard_ReturnToGame");
    LOAD_IFACE_FUNC(SetMessagePumpDisableMode, hModule, "SetMessagePumpDisableMode");

    // set proper states
    Eng_SetGameState(DLL_ACTIVE);
    Eng_SetSubGameState(subState);

    gfInLoad = TRUE;

    CommandLine = Sys_GetCommandLine();
    if ( !CommandLine )
        CommandLine = "empty";

	gWindow = Launcher_CreateWindow();

	Snd_AcquireBuffer();

	RECT rect = { 0, 0, g_iScreenWidth, g_iScreenHeight };
	VID_UpdateWindowVars(&rect, g_iScreenWidth / 2, g_iScreenHeight / 2);

	if (g_vidType == VT_OpenGL || g_vidType == VT_Direct3D)
    {
        if (GL_SetMode(gWindow))
            GL_Init();
    }

	in_mode_set = FALSE;

	IN_ActivateMouse();
	IN_HideMouse();

	GameSetBackground(FALSE);

    // gaming
    if ( gWindow && GameInit(CommandLine, gpMemBase, giMemSize, &ef, &gWindow, "Player", 0, szBaseDir) )
    {
        SetStartupMode(TRUE);

		ForceReloadProfile("Player");

        if ( Cbuf_AddText )
            Eng_Frame(TRUE);
        
        gfInLoad = FALSE;

        SetStartupMode(FALSE);
		S_UnblockSound();
		
        return 1;
    }

    return 0;
}

void Eng_Unload(void)
{
	gDLLState = DLL_CLOSE;
	gDLLStateInfo = 0;
}

void InvokeEngine(int subState)
{
	qboolean fLoad = FALSE;

	g_vidType = VT_OpenGL;

    if (g_vidType == VT_OpenGL || g_vidType == VT_Direct3D)
        fLoad = Eng_Load(HARDWARE_ENGINE, subState);
    else
        fLoad = Eng_Load(SOFTWARE_ENGINE, subState);
}

void Launcher_SetupExefuncs(void)
{
    ef.VID_LockBuffer                   = VID_LockBuffer;
    ef.VID_UnlockBuffer                 = VID_UnlockBuffer;
    ef.VID_Shutdown                     = VID_Shutdown;
    ef.VID_Update                       = VID_Update;
    ef.VID_ForceLockState               = VID_ForceLockState;
    ef.VID_ForceUnlockedAndReturnState  = VID_ForceUnlockedAndReturnState;
    ef.VID_SetDefaultMode               = VID_SetDefaultMode;
    ef.VID_GetExtModeDescription        = VID_GetExtModeDescription;
    ef.VID_GetVID                       = VID_GetVID;

    ef.D_BeginDirectRect                = D_BeginDirectRect;
    ef.D_EndDirectRect                  = D_EndDirectRect;

    ef.AppActivate                      = AppActivate;

    ef.CDAudio_Play                     = CDAudio_Play;
    ef.CDAudio_Pause                    = CDAudio_Pause;
    ef.CDAudio_Resume                   = CDAudio_Resume;
    ef.CDAudio_Update                   = CDAudio_Update;

    ef.Launcher_InitCmds                = Launcher_InitCmds;
    ef.ErrorMessage                     = ErrorMessage;
    ef.D_SurfaceCacheForRes             = D_SurfaceCacheForRes;
    ef.Console_Printf                   = Console_Printf;

    ef.Launcher_GetCDKey                = Launcher_GetCDKey;
    ef.Launcher_GetClientID             = Launcher_GetClientID;
    ef.Launcher_GetUUID                 = Launcher_GetUUID;
    ef.Launcher_VerifyMessage           = Launcher_VerifyMessage;

    ef.Launcher_GetCertificate          = Launcher_GetCertificate;
    ef.Launcher_RequestNewClientCertificate = Launcher_RequestNewClientCertificate;
    ef.Launcher_ValidateClientCertificate = Launcher_ValidateClientCertificate;
}


//==========================================================================

byte        scantokey[128] =
{
	//  0           1       2       3       4       5       6       7 
	//  8           9       A       B       C       D       E       F 
		0  ,    27,     '1',    '2',    '3',    '4',    '5',    '6',
		'7',    '8',    '9',    '0',    '-',    '=',    K_BACKSPACE, 9, // 0 
		'q',    'w',    'e',    'r',    't',    'y',    'u',    'i',
		'o',    'p',    '[',    ']',    13 ,    K_CTRL,'a',  's',      // 1 
		'd',    'f',    'g',    'h',    'j',    'k',    'l',    ';',
		'\'' ,    '`',    K_SHIFT,'\\',  'z',    'x',    'c',    'v',      // 2 
		'b',    'n',    'm',    ',',    '.',    '/',    K_SHIFT,'*',
		K_ALT,' ',   0  ,    K_F1, K_F2, K_F3, K_F4, K_F5,   // 3 
		K_F6, K_F7, K_F8, K_F9, K_F10,  K_PAUSE,    0  , K_HOME,
		K_UPARROW,K_PGUP,'-',K_LEFTARROW,'5',K_RIGHTARROW,'+',K_END, //4 
		K_DOWNARROW,K_PGDN,K_INS,K_DEL,0,0,             0,              K_F11,
		K_F12,0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0,        // 5
		0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0,
		0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0,        // 6 
		0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0,
		0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0  ,    0         // 7 
};

/*
=======
MapKey

Map from windows to quake keynums
=======
*/
int MapKey(SDL_Keysym keysym)
{
	switch (keysym.sym)
	{
		case SDLK_BACKSPACE: return K_BACKSPACE;
		case SDLK_TAB:       return K_TAB;
		case SDLK_RETURN:    return K_ENTER;
		case SDLK_ESCAPE:    return K_ESCAPE;

		case SDLK_SPACE:     return ' ';
		case SDLK_QUOTE:     return '\'';
		case SDLK_COMMA:     return ',';
		case SDLK_MINUS:     return '-';
		case SDLK_PERIOD:    return '.';
		case SDLK_SLASH:     return '/';
		case SDLK_0:         return '0';
		case SDLK_1:         return '1';
		case SDLK_2:         return '2';
		case SDLK_3:         return '3';
		case SDLK_4:         return '4';
		case SDLK_5:         return '5';
		case SDLK_6:         return '6';
		case SDLK_7:         return '7';
		case SDLK_8:         return '8';
		case SDLK_9:         return '9';
		case SDLK_SEMICOLON: return ';';
		case SDLK_EQUALS:    return '=';
		case SDLK_LEFTBRACKET: return '[';
		case SDLK_BACKSLASH: return '\\';
		case SDLK_RIGHTBRACKET: return ']';
		case SDLK_BACKQUOTE: return '`';  // You can also map this to K_CONSOLE if defined

		case SDLK_a: return 'a';
		case SDLK_b: return 'b';
		case SDLK_c: return 'c';
		case SDLK_d: return 'd';
		case SDLK_e: return 'e';
		case SDLK_f: return 'f';
		case SDLK_g: return 'g';
		case SDLK_h: return 'h';
		case SDLK_i: return 'i';
		case SDLK_j: return 'j';
		case SDLK_k: return 'k';
		case SDLK_l: return 'l';
		case SDLK_m: return 'm';
		case SDLK_n: return 'n';
		case SDLK_o: return 'o';
		case SDLK_p: return 'p';
		case SDLK_q: return 'q';
		case SDLK_r: return 'r';
		case SDLK_s: return 's';
		case SDLK_t: return 't';
		case SDLK_u: return 'u';
		case SDLK_v: return 'v';
		case SDLK_w: return 'w';
		case SDLK_x: return 'x';
		case SDLK_y: return 'y';
		case SDLK_z: return 'z';

		case SDLK_F1:  return K_F1;
		case SDLK_F2:  return K_F2;
		case SDLK_F3:  return K_F3;
		case SDLK_F4:  return K_F4;
		case SDLK_F5:  return K_F5;
		case SDLK_F6:  return K_F6;
		case SDLK_F7:  return K_F7;
		case SDLK_F8:  return K_F8;
		case SDLK_F9:  return K_F9;
		case SDLK_F10: return K_F10;
		case SDLK_F11: return K_F11;
		case SDLK_F12: return K_F12;

		case SDLK_LSHIFT:
		case SDLK_RSHIFT:
			return K_SHIFT;

		case SDLK_LCTRL:
		case SDLK_RCTRL:
			return K_CTRL;

		case SDLK_LALT:
		case SDLK_RALT:
			return K_ALT;

		case SDLK_INSERT:    return K_INS;
		case SDLK_DELETE:    return K_DEL;
		case SDLK_HOME:      return K_HOME;
		case SDLK_END:       return K_END;
		case SDLK_PAGEUP:    return K_PGUP;
		case SDLK_PAGEDOWN:  return K_PGDN;
		case SDLK_LEFT:      return K_LEFTARROW;
		case SDLK_RIGHT:     return K_RIGHTARROW;
		case SDLK_UP:        return K_UPARROW;
		case SDLK_DOWN:      return K_DOWNARROW;

		case SDLK_KP_DIVIDE:    return '/';
		case SDLK_KP_MULTIPLY:  return '*';
		case SDLK_KP_MINUS:     return '-';
		case SDLK_KP_PLUS:      return '+';
		case SDLK_KP_ENTER:     return K_ENTER;
		case SDLK_KP_1: return '1';
		case SDLK_KP_2: return '2';
		case SDLK_KP_3: return '3';
		case SDLK_KP_4: return '4';
		case SDLK_KP_5: return '5';
		case SDLK_KP_6: return '6';
		case SDLK_KP_7: return '7';
		case SDLK_KP_8: return '8';
		case SDLK_KP_9: return '9';
		case SDLK_KP_0: return '0';
		case SDLK_KP_PERIOD: return '.';

		default:
			return 0;
	}
}


/*
================
ClearAllStates
================
*/
void ClearAllStates(void)
{
	int		i;

	// send an up event for each key, to make sure the server clears them all
	for (i = 0; i < 256; i++)
	{
		Key_Event(i, FALSE);
	}

	Key_ClearStates();
	IN_ClearStates();
}

void Launcher_PollEvents(void)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		// --- Window / Quit events ---
		case SDL_QUIT:
			gDLLState = DLL_CLOSE;
			break;

		case SDL_WINDOWEVENT:
			switch (event.window.event)
			{
			case SDL_WINDOWEVENT_FOCUS_GAINED:
				AppActivate(TRUE, FALSE);
				ClearAllStates(); // fix leftover Alt or stuck keys
				break;

			case SDL_WINDOWEVENT_FOCUS_LOST:
				AppActivate(FALSE, FALSE);
				break;

			case SDL_WINDOWEVENT_MINIMIZED:
				AppActivate(FALSE, TRUE);
				break;

			case SDL_WINDOWEVENT_RESTORED:
				AppActivate(TRUE, FALSE);
				break;

			default:
				break;
			}
			break;

		// --- Keyboard events ---
		case SDL_KEYDOWN:
			if (!in_mode_set)
				Key_Event(MapKey(event.key.keysym), TRUE);
			break;

		case SDL_KEYUP:
			if (!in_mode_set)
				Key_Event(MapKey(event.key.keysym), FALSE);
			break;

		// --- Mouse buttons & movement ---
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEMOTION:
			if (!in_mode_set)
			{
				int temp = 0;
				Uint32 state = SDL_GetMouseState(NULL, NULL);

				if (state & SDL_BUTTON(SDL_BUTTON_LEFT))
					temp |= 1;
				if (state & SDL_BUTTON(SDL_BUTTON_RIGHT))
					temp |= 2;
				if (state & SDL_BUTTON(SDL_BUTTON_MIDDLE))
					temp |= 4;

				IN_MouseEvent(temp);
			}
			break;

		// --- Mouse wheel ---
		case SDL_MOUSEWHEEL:
			if (event.wheel.y > 0)
			{
				Key_Event(K_MWHEELUP, TRUE);
				Key_Event(K_MWHEELUP, FALSE);
			}
			else if (event.wheel.y < 0)
			{
				Key_Event(K_MWHEELDOWN, TRUE);
				Key_Event(K_MWHEELDOWN, FALSE);
			}
			break;

		default:
			break;
		}
	}
}


SDL_Window* Launcher_CreateWindow()
{
    Uint32 windowFlags;
    
	if (gWindow)
    {
		SDL_DestroyWindow(gWindow);
		gWindow = NULL;
	}

	// Set OpenGL flags (should 2.1 gl ver be enough?)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    if (CheckParmEx("-windowed") || CheckParmEx("-window"))
    {
        // Windowed mode
        windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
    }
    else
    {
        // Fullscreen
        windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN;
    }

	gWindow = SDL_CreateWindow(
		"Half-Life",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		g_iScreenWidth, g_iScreenHeight,
		windowFlags
	);

	if (!gWindow)
    {
        if (windowFlags & SDL_WINDOW_FULLSCREEN)
        {
            windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
            gWindow = SDL_CreateWindow(
                "Half-Life",
                SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                g_iScreenWidth, g_iScreenHeight,
                windowFlags
            );
        }
        
        if (!gWindow)
        {
            Sys_Error("Failed to create SDL window: %s", SDL_GetError());
            return NULL;
        }
	}

	return gWindow;
}

void Launcher_Shutdown(void)
{
	if (gWindow)
	{
		SDL_DestroyWindow(gWindow);
		gWindow = NULL;
	}

	SDL_Quit();

	free(gpMemBase);
}

void Launcher_Main(void)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
	{
		Sys_Error("SDL_Init failed: %s", SDL_GetError());
		return;
	}

	Launcher_SetupExefuncs();

    if (!AllocGameMem())
        return;

    InvokeEngine(0);

	if (!gWindow)
		return;

	while (gDLLState != DLL_CLOSE)
    {
		Launcher_PollEvents();
		Eng_Frame(1);
	}

	Launcher_Shutdown();
}

int main(int argc, char* argv[])
{
    sys_cmdline[0] = 0;
    for (int i = 0; i < argc; i++)
	{
        if (i > 0)
			strcat(sys_cmdline, " ");
			
        strcat(sys_cmdline, argv[i]);
    }

	snprintf(szBaseDir, sizeof(szBaseDir), "%s", argv[0]);
    char* szFirstSep = strrchr(szBaseDir, '/');
    if (szFirstSep)
    {
        szFirstSep[0] = 0;
    }
	
    Launcher_Main();
    return 0;
}