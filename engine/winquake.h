// winquake.h: Win32-specific Quake header file

#pragma warning( disable : 4229 )  // mgraph gets this

#include <windows.h>
#include <direct.h>

#ifdef __cplusplus
#define C_EXTERN extern "C"
#else
#define C_EXTERN extern
#endif

#ifdef __cplusplus
#define DECLTYPE(func) (decltype(func))
#else
#define DECLTYPE(func) (void*)
#endif

#if defined( _WIN32 )

// Used for dll exporting and importing
#define DLL_EXPORT				C_EXTERN __declspec( dllexport )
#define DLL_IMPORT				C_EXTERN __declspec( dllimport )

#endif





int (*VID_ForceUnlockedAndReturnState)( void );
int (*VID_ForceLockState)( int lk );
