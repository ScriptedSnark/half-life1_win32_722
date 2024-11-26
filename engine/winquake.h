// winquake.h: Win32-specific Quake header file

#pragma warning( disable : 4229 )  // mgraph gets this
#pragma warning( disable : 4996 )  // GetVersionExA

#ifndef KEEP_XWHITEYS_SHIT_OUT_OF_HERE
#include "dummyfile.h"
#endif //!KEEP_XWHITEYS_SHIT_OUT_OF_HERE

#include <windows.h>
#include <direct.h>

#define CINTERFACE

#include <ddraw.h>
#include <mmeapi.h>
#include <dsound.h>


extern HWND* pmainwindow;
extern qboolean	Win32AtLeastV4;
extern int gHasMMXTechnology;

extern int (*VID_ForceUnlockedAndReturnState)( void );
extern int (*VID_ForceLockState)( int lk );

extern int		window_center_x, window_center_y;

DLL_EXPORT void S_BlockSound( void );
DLL_EXPORT void S_UnblockSound( void );