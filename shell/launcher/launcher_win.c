//=============================================
// HALF-LIFE NET TEST 1 LAUNCHER REIMPLEMENTATION
// No GUI version
//
// Author: ScriptedSnark
// File: launcher.c
//=============================================

#include "launcher.h"

#define DEFAULT_MEM_MB 32
#define DEFAULT_MEM_SIZE (DEFAULT_MEM_MB * 1024 * 1024)

#define LOAD_IFACE_FUNC(func, hModule, pszName)							\
	func = DECLTYPE(func)(Sys_GetProcAddress(hModule, pszName));			\
	if (!func)												\
		Sys_Error("could not link engine function " pszName "\n")

VidTypes g_vidType = VT_None;
int gDLLState, gDLLStateInfo;
qboolean gfInLoad;
HGLOBAL gpMemBase;
size_t giMemSize;

int tStart = 0;

HWND hWnd = NULL;
HDC hDC = NULL;
HGLRC hRC = NULL;

qboolean in_mode_set = FALSE;

int g_iScreenWidth, g_iScreenHeight;

HWND Launcher_CreateWindow();

qboolean IsHardwareEngine(void)
{
	return (g_vidType == VT_OpenGL || g_vidType == VT_Direct3D);
}

qboolean AllocGameMem(void)
{
	char* pszValue = NULL;

	giMemSize = DEFAULT_MEM_SIZE;
	// I don't have CheckParm yet	
	//if (CheckParm("-heapsize", &pszValue) && pszValue)
	//	giMemSize = atoi(pszValue);

	gpMemBase = GlobalAlloc(0, giMemSize);

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

void Eng_Frame(qboolean forceUpdate)
{
    static qboolean fDeferedPause = FALSE;
    static int iWait = 0;
    static double oldtime = 0.0;
	HWND hWC;

	if (gDLLState != DLL_INACTIVE || forceUpdate)
	{
		if (gDLLState != DLL_INACTIVE)
		{
			/*
			while (TRUE)
			{
				double curtime = Sys_FloatTime();
				double delta = curtime - oldtime;

				if (delta >= sys_ticrate)
					break;

				if (((sys_ticrate - delta) * 1000) > 2)
					Sys_Sleep(2);
			}
			*/

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
#ifdef PLATFORM_WINDOWS
				case STATE_WORLDCRAFT:
					hWC = FindWindowA("VALVEWORLDCRAFT", NULL);
					if (hWC)
					{
						SetForegroundWindow(hWC);
						SetFocus(hWC);
					}
					break;
#endif
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

    //tStart = clock();
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
	// These aren't available - ScriptedSnark
    //LOAD_IFACE_FUNC(S_GetWAVPointer, hModule, "S_GetWAVPointer");
    //LOAD_IFACE_FUNC(S_GetDSPointer, hModule, "S_GetDSPointer");
    LOAD_IFACE_FUNC(DeinitConProc, hModule, "DeinitConProc");
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
    LOAD_IFACE_FUNC(qwglSwapBuffers, hModule, "qwglSwapBuffers");
    LOAD_IFACE_FUNC(GL_Init, hModule, "GL_Init");
    LOAD_IFACE_FUNC(GL_SetMode, hModule, "GL_SetMode");
    LOAD_IFACE_FUNC(GL_Shutdown, hModule, "GL_Shutdown");
    LOAD_IFACE_FUNC(GetGameInfo, hModule, "GetGameInfo");
    LOAD_IFACE_FUNC(QGL_D3DShared, hModule, "QGL_D3DShared");
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

    CommandLine = GetCommandLine();
    if ( !CommandLine )
        CommandLine = "empty";

	hWnd = Launcher_CreateWindow();

	Snd_AcquireBuffer();

	RECT rect = { 0, 0, 1024, 768 };
	VID_UpdateWindowVars(&rect, 1024 / 2, 768 / 2);

	if (g_vidType == VT_OpenGL || g_vidType )
	if (GL_SetMode(hWnd, &hDC, &hRC, 0, NULL))
		GL_Init();

	in_mode_set = FALSE;

	IN_ActivateMouse();
	IN_HideMouse();

	GameSetBackground(FALSE);

    // gaming
    if ( IsWindow(hWnd) && GameInit(CommandLine, gpMemBase, giMemSize, &ef, &hWnd, "Player", 0) )
    {
        SetStartupMode(TRUE);

		ForceReloadProfile("Player");

        if ( Cbuf_AddText )
            Eng_Frame(TRUE);
        
        gfInLoad = FALSE;

        SetStartupMode(FALSE);

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
        fLoad = Eng_Load("hw.dll", subState);
    else
        fLoad = Eng_Load("sw.dll", subState);
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
int MapKey(int key)
{
	key = (key >> 16) & 255;
	if (key > 127)
		return 0;

	return scantokey[key];
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

LONG WINAPI MainWndProc(
	HWND    hWnd,
	UINT    uMsg,
	WPARAM  wParam,
	LPARAM  lParam)
{
	LONG			lRet = 0;
	int				fActive, fMinimized, temp;
	PAINTSTRUCT		ps;
	static unsigned int uiWheelMessage = 0;
	static int		recursiveflag;

	if (uiWheelMessage == 0)
		uiWheelMessage = RegisterWindowMessage("MSWHEEL_ROLLMSG");

	if (uMsg == uiWheelMessage) {
		uMsg = WM_MOUSEWHEEL;
		wParam <<= 16;
	}

	switch (uMsg)
	{
	case WM_CREATE:
		break;
	case WM_SYSCHAR:
		// keep Alt-Space from happening
		break;

	case WM_ACTIVATE:
		fActive = LOWORD(wParam);
		fMinimized = (BOOL)HIWORD(wParam);
		AppActivate(!(fActive == WA_INACTIVE), fMinimized);

		// fix the leftover Alt from any Alt-Tab or the like that switched us away
		ClearAllStates();

		break;

	case WM_PAINT:
		BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (!in_mode_set)
			Key_Event(MapKey(lParam), TRUE);
		break;

	case WM_KEYUP:
	case WM_SYSKEYUP:
		if (!in_mode_set)
			Key_Event(MapKey(lParam), FALSE);
		break;

		// this is complicated because Win32 seems to pack multiple mouse events into
		// one update sometimes, so we always check all states and look for events
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEMOVE:
		if (!in_mode_set)
		{
			temp = 0;

			if (wParam & MK_LBUTTON)
				temp |= 1;

			if (wParam & MK_RBUTTON)
				temp |= 2;

			if (wParam & MK_MBUTTON)
				temp |= 4;

			IN_MouseEvent(temp);
		}
		break;
		// JACK: This is the mouse wheel with the Intellimouse
		// Its delta is either positive or neg, and we generate the proper
		// Event.
	case WM_MOUSEWHEEL:
		if ((short)HIWORD(wParam) > 0) {
			Key_Event(K_MWHEELUP, TRUE);
			Key_Event(K_MWHEELUP, FALSE);
		}
		else {
			Key_Event(K_MWHEELDOWN, TRUE);
			Key_Event(K_MWHEELDOWN, FALSE);
		}
		break;
		// KJB: Added these new palette functions
	case WM_PALETTECHANGED:
		if ((HWND)wParam == hWnd)
			break;
	default:
		/* pass all unhandled messages to DefWindowProc */
		lRet = DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	}

	/* return 0 if handled message, 1 if not */
	return lRet;
}

void Launcher_CenterWindow(HWND hwnd)
{
	int screenWidth, screenHeight;
	int windowWidth, windowHeight;
	int posX, posY;
	RECT rcScreen, rcWnd;

	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcScreen, 0);

	screenWidth = rcScreen.right - rcScreen.left;
	screenHeight = rcScreen.bottom - rcScreen.top;

	GetWindowRect(hwnd, &rcWnd);

	windowWidth = rcWnd.right - rcWnd.left;
	windowHeight = rcWnd.bottom - rcWnd.top;

	posX = rcScreen.left + (screenWidth - windowWidth) / 2;
	posY = rcScreen.top + (screenHeight - windowHeight) / 2;

	SetWindowPos(hwnd, NULL, posX, posY, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

HWND Launcher_CreateWindow()
{
	WNDCLASSEX wcex;
	HINSTANCE hInstance = GetModuleHandle(NULL);
	RECT client_rect = { 0, 0, 1024, 768 }; // TODO: 1024x768 is hardcoded!
	DWORD dwStyle = (WS_OVERLAPPEDWINDOW | WS_MINIMIZEBOX | WS_SYSMENU | WS_VISIBLE);
	if (hWnd)
	{
		//if (hdc)
		//	ReleaseHDC(hdc);

		DestroyWindow(hWnd);
		UnregisterClassA("Half-Life", hInstance);

		hWnd = NULL;
	}

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_OWNDC;
	wcex.lpfnWndProc = MainWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = NULL; 
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "Half-Life";
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex)) {
		MessageBox(NULL, "Failed to create engine class!", "Error", 0);
		return NULL;
	}

	AdjustWindowRectEx(&client_rect, dwStyle, FALSE, 0);

	hWnd = CreateWindow(wcex.lpszClassName, "Half-Life", dwStyle,
		0, 0, client_rect.right - client_rect.left, client_rect.bottom - client_rect.top, NULL, NULL, hInstance, NULL);

	if (!hWnd) {
		Sys_Error("Failed to create engine window!");
		return NULL;
	}

	Launcher_CenterWindow(hWnd);
	return hWnd;
}

void Launcher_Shutdown(void)
{
	if (hWnd)
	{
		DestroyWindow(hWnd);
		hWnd = NULL;
	}

	HINSTANCE hInstance = GetModuleHandle(NULL);
	UnregisterClassA("Half-Life", hInstance);

	GlobalFree(gpMemBase);
}

void Launcher_Main(void)
{
	Launcher_SetupExefuncs();

    if (!AllocGameMem())
        return;

    InvokeEngine(0);

	if (!hWnd)
		return;

	while (gDLLState != DLL_CLOSE)
		Eng_Frame(1);

	Launcher_Shutdown();
}

#ifdef _WIN32

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	AllocConsole();
	FILE* in, * out;
	freopen_s(&in, "conin$", "r", stdin);
	freopen_s(&out, "conout$", "w+", stdout);

    Launcher_Main();
    return 0;
}
#else
int main(int argc, char* argv[])
{
    Launcher_Main();
    return 0;
}
#endif