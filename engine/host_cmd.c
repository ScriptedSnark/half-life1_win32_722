#include "quakedef.h"
#include "winquake.h"

int	gHostSpawnCount = 0;
qboolean noclip_anglehack;

DLL_EXPORT BOOL SaveGame( char* pszSlot, char* pszComment )
{
	// TODO: Implement
	return 0;
}

DLL_EXPORT int LoadGame( char* pName )
{
	// TODO: Implement
	return 0;
}



void Profile_Init( void )
{
	// TODO: Implement
}


/*
==================
Host_Version_f

Print current protocol version and build date
==================
*/
void Host_Version_f( void )
{
	Con_Printf("Build %d\n", build_number());
	Con_Printf("Exe: " __TIME__ " " __DATE__ "\n");
}





//=============================================================================

/*
==================
Host_InitCommands
==================
*/
void Host_InitCommands( void )
{
	// TODO: Implement

	Cmd_AddCommand("version", Host_Version_f);

	// TODO: Implement
}