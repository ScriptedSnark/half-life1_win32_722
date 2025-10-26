//=============================================
// HALF-LIFE NET TEST 1 LAUNCHER REIMPLEMENTATION
// No GUI version
//
// Author: ScriptedSnark
// File: exefuncs.c
//=============================================

#include "launcher.h"

exefuncs_t ef;
viddef_t g_pvid;
qboolean fWindowActive = FALSE;

void VID_LockBuffer(void)
{

}

void VID_UnlockBuffer(void)
{

}

void VID_Shutdown(void)
{
#ifdef _WIN32
	if (hDC)
	{
		ReleaseDC(hWnd, hDC);
		hDC = NULL;
	}

	if (hRC)
	{
		//wglDeleteContext(hRC);
		//hRC = NULL;
	}

	//DestroyWindow(hWnd);
	//UnregisterClassA("Half-Life", GetModuleHandle(NULL));
#else
	if (gWindow)
	{
		SDL_DestroyWindow(gWindow);
		gWindow = NULL;
	}
#endif
}

void VID_Update(vrect_t* rects)
{
#ifdef _WIN32 // why is it still there
	if (fWindowActive)
	{
		IN_ActivateMouse();
		IN_HideMouse();
		ShowCursor(FALSE);
	}
	else
	{
		IN_DeactivateMouse();
		IN_ShowMouse();
		ShowCursor(TRUE);
	}

	SwapBuffers(hDC);
#else
	SDL_GL_SwapWindow(gWindow);
#endif
}

int VID_ForceLockState(int lk)
{
    return 0;
}

int VID_ForceUnlockedAndReturnState(void)
{
    return 0;
}

void VID_SetDefaultMode(void)
{

}

char* VID_GetExtModeDescription(int mode)
{
    return "1024x768 windowed"; // TODO: return proper description
}

void VID_GetVID(viddef_t* pvid)
{
	if (pvid)
		memcpy(pvid, &g_pvid, sizeof(g_pvid));
}

void D_BeginDirectRect(int width, int height, byte* data, int pic_width, int pic_height)
{

}

void D_EndDirectRect(int width, int height, int pic_width, int pic_height)
{

}

void AppActivate(int fActive, int minimize)
{
	fWindowActive = fActive;

	if (fWindowActive)
	{
		IN_ActivateMouse();
		IN_HideMouse();
		SDL_ShowCursor(SDL_DISABLE);
	}
	else
	{
		IN_DeactivateMouse();
		IN_ShowMouse();
		SDL_ShowCursor(SDL_ENABLE);
	}
}

void CDAudio_Play(int track, int looping)
{

}

void CDAudio_Pause(void)
{

}

void CDAudio_Resume(void)
{

}

void CDAudio_Update(void)
{

}

void Launcher_InitCmds(void)
{

}

void ErrorMessage(int nLevel, const char* pszErrorMessage)
{
	Sys_Error("Engine error: %s", pszErrorMessage);
}

int D_SurfaceCacheForRes(int width, int height)
{
    return 0;
}

void Console_Printf(char* fmt, ...)
{
	
}

void Launcher_GetCDKey(char* pszCDKey, int* nLength, int* bDedicated)
{

}

void Launcher_GetClientID(void* pID)
{

}

void Launcher_GetUUID(void* pUUID, int* nLength, int* bDedicated)
{

}

void* Launcher_VerifyMessage(int nLength, byte* pKey, int nMsgLength, char* pMsg, int nSignLength, byte* pSign)
{
    return NULL;
}

int Launcher_GetCertificate(void* pBuffer, int* nLength)
{
    return 0;
}

int Launcher_RequestNewClientCertificate(void)
{
    return 0;
}

int Launcher_ValidateClientCertificate(void* pBuffer, int nLength)
{
    return 0;
}

