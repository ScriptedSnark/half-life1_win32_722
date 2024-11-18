// cl_main.c  -- client main loop

#include "quakedef.h"
#include "winquake.h"

cvar_t	lookspring = { "lookspring","0", TRUE };
// TODO: Implement
cvar_t	sensitivity = { "sensitivity","3", TRUE };

client_static_t	cls;
client_state_t cl;