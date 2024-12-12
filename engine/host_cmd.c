#include "quakedef.h"
#include "winquake.h"
#include "cl_demo.h"



int	gHostSpawnCount = 0;
qboolean noclip_anglehack;




/*
=====================
Host_Connect_f

User command to connect to server
=====================
*/
void Host_Connect_f( void )
{
	char	name[MAX_QPATH];

	if (cls.demoplayback)
	{
		CL_StopPlayback();
		CL_Disconnect();
	}

	if (Cmd_Argc() < 2 || Cmd_Args() == 0)
	{
		Con_Printf("Usage:  connect <server>\n");
		return;
	}

	strcpy(name, Cmd_Args());
	strncpy(cls.servername, name, sizeof(cls.servername) - 1);

	CL_Connect_f();
}



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


	Cmd_AddCommand("connect", Host_Connect_f);


	// TODO: Implement


	Cmd_AddCommand("version", Host_Version_f);


	// TODO: Implement
}