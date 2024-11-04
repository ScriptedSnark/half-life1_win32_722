// cvar.h -- dynamic variable tracking

#if !defined( CVAR_H )
#define CVAR_H
#ifdef _WIN32
#pragma once
#endif

#include "cvardef.h"

/*

cvar_t variables are used to hold scalar or string variables that can be changed or displayed at the console or prog code as well as accessed directly
in C code.

it is sufficient to initialize a cvar_t with just the first two fields, or
you can add a, true flag for variables that you want saved to the configuration
file when the game is quit:

cvar_t	r_draworder = { "r_draworder", "1" };
cvar_t	scr_screensize = { "screensize", "1", TRUE };

Cvars must be registered before use, or they will have a 0 value instead of the float interpretation of the string.  Generally, all cvar_t declarations should be registered in the appropriate init function before any console commands are executed:
Cvar_RegisterVariable (&host_framerate);


C code usually just references a cvar in place:
if ( r_draworder.value )

It could optionally ask for the value to be looked up for a string name:
if (Cvar_VariableValue ("r_draworder"))

Interpreted prog code can access cvars with the cvar(name) or
cvar_set (name, value) internal functions:
teamplay = cvar("teamplay");
cvar_set ("registered", "1");

The user can access cvars from the console in two ways:
r_draworder			prints the current value
r_draworder 0		sets the current value to 0
Cvars are restricted from having the same names as commands to keep this
interface from being ambiguous.
*/

void 	Cvar_RegisterVariable( cvar_t* variable );
// registers a cvar that allready has the name, string, and optionally the
// archive elements set.

DLL_EXPORT void		Cvar_Set( char* var_name, char* value );
// equivelant to "<name> <variable>" typed at the console

DLL_EXPORT void		Cvar_SetValue( char* var_name, float value );
// expands value to a string and calls Cvar_Set

DLL_EXPORT float	Cvar_VariableValue( char* var_name );
// returns 0 if not defined or non numeric

DLL_EXPORT int		Cvar_VariableInt( char* var_name );

DLL_EXPORT char*	Cvar_VariableString( char* var_name );
// returns an empty string if not defined

DLL_EXPORT char*	Cvar_CompleteVariable( char* partial );
// attempts to match a partial variable name for command line completion
// returns NULL if nothing fits

qboolean Cvar_Command( void );
// called by Cmd_ExecuteString when Cmd_Argv(0) doesn't match a known
// command.  Returns true if the command was a variable reference that
// was handled. (print or change)

void 	Cvar_WriteVariables( FILE* f );
// Writes lines containing "set variable value" for all variables
// with the archive flag set to true.

void	Cvar_WriteProfileVariables( FILE* f );
// Writes lines containing "set variable value" for all variables
// with the profile flag set to true.

int		Cvar_CountServerVariables( void );

void	Cvar_CmdInit( void );

cvar_t* Cvar_FindVar( char* var_name );

extern cvar_t* cvar_vars;

#endif // CVAR_H