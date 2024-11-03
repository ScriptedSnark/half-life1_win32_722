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
you can add ,FCVAR_* flags for variables that you want saved to the configuration
file when the game is quit:

cvar_t	r_draworder = {"r_draworder","1"};
cvar_t	scr_screensize = {"screensize","1",FCVAR_ARCHIVE};

Cvars must be registered before use, or they will have a 0 value instead of the float interpretation of the string.  Generally, all cvar_t declarations should be registered in the apropriate init function before any console commands are executed:
Cvar_RegisterVariable(&host_framerate);


C code usually just references a cvar in place:
if (r_draworder.value)

It could optionally ask for the value to be looked up for a string name:
if (Cvar_VariableValue("r_draworder"))

The user can access cvars from the console in two ways:
r_draworder			prints the current value
r_draworder 0		sets the current value to 0
Cvars are restricted from having the same names as commands to keep this
interface from being ambiguous.
*/










extern cvar_t* cvar_vars;

#endif // CVAR_H