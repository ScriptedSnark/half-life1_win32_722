// winquake.h: Win32-specific Quake header file

#ifdef _WIN32

#pragma warning( disable : 4229 )  // mgraph gets this
#pragma warning( disable : 4996 )  // GetVersionExA

#include <windows.h>
#include <direct.h>

#define CINTERFACE

#include <ddraw.h>
// ScriptedSnark: MSVC++ 4.2 doesn't have MMEAPI
#if (_MSC_VER != 1020)
#include <mmeapi.h>
#endif
#include <dsound.h>

extern LPDIRECTSOUND pDS;
extern LPDIRECTSOUNDBUFFER pDSBuf, pDSPBuf;

extern DWORD gSndBufSize;
//#define SNDBUFSIZE 65536

void	(*VID_LockBuffer)( void );
void	(*VID_UnlockBuffer)( void );

extern HWND* pmainwindow;
extern qboolean	Win32AtLeastV4;
extern int gHasMMXTechnology;

int (*VID_ForceUnlockedAndReturnState)( void );
int (*VID_ForceLockState)( int lk );

DLL_EXPORT void IN_ShowMouse( void );
DLL_EXPORT void IN_DeactivateMouse( void );
DLL_EXPORT void IN_HideMouse( void );
DLL_EXPORT void IN_ActivateMouse( void );
void IN_SetQuakeMouseState( void );
DLL_EXPORT void IN_MouseEvent( int mstate );

extern int		window_center_x, window_center_y;
extern RECT		window_rect;

DLL_EXPORT void IN_UpdateClipCursor( void );

DLL_EXPORT void S_BlockSound( void );
DLL_EXPORT void S_UnblockSound( void );

#else // Linux version

#include <SDL2/SDL.h>

extern void	(*VID_LockBuffer)( void );
extern void	(*VID_UnlockBuffer)( void );

extern SDL_Window* pmainwindow;
extern qboolean	Win32AtLeastV4;
extern int gHasMMXTechnology;

extern int (*VID_ForceUnlockedAndReturnState)( void );
extern int (*VID_ForceLockState)( int lk );

DLL_EXPORT void IN_ShowMouse( void );
DLL_EXPORT void IN_DeactivateMouse( void );
DLL_EXPORT void IN_HideMouse( void );
DLL_EXPORT void IN_ActivateMouse( void );
void IN_SetQuakeMouseState( void );
DLL_EXPORT void IN_MouseEvent( int mstate );

extern int		window_center_x, window_center_y;
extern RECT		window_rect;

DLL_EXPORT void IN_UpdateClipCursor( void );

DLL_EXPORT void S_BlockSound( void );
DLL_EXPORT void S_UnblockSound( void );

#endif