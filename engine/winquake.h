// winquake.h: Win32-specific Quake header file

#pragma warning( disable : 4229 )  // mgraph gets this

#include <windows.h>




int (*VID_ForceUnlockedAndReturnState)( void );
int (*VID_ForceLockState)( int lk );
