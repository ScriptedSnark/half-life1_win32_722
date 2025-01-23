#include "quakedef.h"

/*
===============
R_ClearParticles
===============
*/
void R_ClearParticles( void )
{
	// TODO: Implement
}

/*
===============
R_ParseParticleEffect

Parse an effect out of the server message
===============
*/
void R_ParseParticleEffect( void )
{
	vec3_t		org, dir;
	int			i, count, msgcount, color;

	for (i = 0; i < 3; i++)
		org[i] = MSG_ReadCoord();
	for (i = 0; i < 3; i++)
		dir[i] = MSG_ReadChar() * (1.0 / 16);
	msgcount = MSG_ReadByte();
	color = MSG_ReadByte();

	if (msgcount == 255)
		count = 1024;
	else
		count = msgcount;

	// TODO: Implement
}