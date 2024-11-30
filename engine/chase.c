// chase.c -- chase camera code

#include "quakedef.h"

cvar_t chase_back = { "chase_back", "100" };
cvar_t chase_up = { "chase_up", "16" };
cvar_t chase_right = { "chase_right", "0" };
cvar_t chase_active = { "chase_active", "0" };

vec3_t chase_pos;
vec3_t chase_angles;

vec3_t chase_dest;
vec3_t chase_dest_angles;


void Chase_Init( void )
{
	Cvar_RegisterVariable(&chase_back);
	Cvar_RegisterVariable(&chase_up);
	Cvar_RegisterVariable(&chase_right);
	Cvar_RegisterVariable(&chase_active);
}


void Chase_Reset( void )
{
	// for respawning and teleporting
//	start position 12 units behind head
}

// TODO: Implement