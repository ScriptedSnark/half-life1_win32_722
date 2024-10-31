// sys_win.c -- Win32 system interface code

#include "quakedef.h"
#include "winquake.h"

// 0 = not active, 1 = active, 2 = pause
int					giActive = DLL_INACTIVE;
int					giStateInfo;

static double		pfreq;
static double		curtime = 0.0;
static double		lastcurtime = 0.0;
static int			lowshift;
qboolean			isDedicated;



void MaskExceptions( void );
void Sys_InitFloatTime( void );
void Sys_PushFPCW_SetHigh( void );
void Sys_PopFPCW( void );



int		(*VID_ForceUnlockedAndReturnState)( void );
int		(*VID_ForceLockState)( int lk );
void	(*Launcher_ConsolePrintf)( char* fmt, ... );

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
	// TODO: Implement
}

void Sys_Warning( char* warning, ... )
{
	// TODO: Implement
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

		if (Launcher_ConsolePrintf)
		{
			Launcher_ConsolePrintf(text);
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
double Sys_FloatTime( void )
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
	// TODO: Implement
}

// TODO: Implement

// Required DLL entry point
BOOL WINAPI DllMain(
	HINSTANCE hinstDLL,
	DWORD fdwReason,
	LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		// TODO: Implement
	}
	else if (fdwReason == DLL_PROCESS_DETACH)
	{
		// TODO: Implement
	}
	return TRUE;
}




// TODO: Implement
