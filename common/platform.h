#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef _WIN32
#define PLATFORM_WINDOWS
#endif

// DLL Export stuff
#if defined( PLATFORM_WINDOWS )
	#if __GNUC__
		#define EXPORT __attribute__ ((dllexport))
	#else
		#define EXPORT __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
	#endif
#else
	#if __GNUC__ >= 4
		#define EXPORT __attribute__ ((visibility ("default")))
	#else
		#define EXPORT
	#endif
#endif

#define DLLEXPORT EXPORT
#define _DLLEXPORT EXPORT
#define DLL_EXPORT EXPORT

#if !defined(ARRAYSIZE)
#define ARRAYSIZE(p)		( sizeof(p) /sizeof(p[0]) )
#endif

#ifndef V_min
#define V_min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef V_max
#define V_max(a, b) (((a) > (b)) ? (a) : (b))
#endif

// Platform stuff
#if defined( PLATFORM_WINDOWS )

#include <windows.h>

typedef __int16 int16;
typedef unsigned __int16 uint16;
typedef __int32 int32;
typedef unsigned __int32 uint32;
typedef __int64 int64;
typedef unsigned __int64 uint64;

#define DLL_FORMAT ".dll"

#else // _WIN32

#include <sys/stat.h> // for stat()
#include <unistd.h>   // for close()
#include <fcntl.h>    // for open()
#include <string.h>
#include <ctype.h>
#include <sys/ioctl.h>

#define DLL_FORMAT ".so"

typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;
typedef unsigned char uint8;
typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned long ULONG;

typedef void* LPVOID;
typedef void* HANDLE;
typedef char* HPSTR;

typedef time_t FILETIME;
#define MAX_PATH 260

#define _cdecl
#define WINAPI

typedef void* HINSTANCE;
typedef void* HMODULE;

// RECT for Linux
typedef struct tagRECT
{
	int left;
	int top;
	int right;
	int bottom;
} RECT;

// POINT for Linux
typedef struct tagPOINT
{
	int x;
	int y;
} POINT;

typedef unsigned short WORD;
typedef unsigned int DWORD;

#if !PLATFORM_WINDOWS
static unsigned _rotr( unsigned val, int shift )
{
	// Any modern compiler will generate one single ror instruction for x86, arm and mips here.
	return ( val >> shift ) | ( val << ( 32 - shift ));
}
#endif

#endif // else _WIN32


// C specific stuff

#ifndef GLQUAKE

#ifndef __cplusplus
typedef enum {FALSE, TRUE}  qboolean;
#else 
#define FALSE 0
#define TRUE 1
typedef int qboolean;
#endif

#else
#define FALSE 0
#define TRUE 1
#endif

#endif /* PLATFORM_H */