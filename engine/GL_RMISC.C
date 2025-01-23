// r_misc.c

#include "quakedef.h"

cvar_t	r_norefresh = { "r_norefresh", "0" };


cvar_t	r_speeds = { "r_speeds", "0" };




cvar_t	r_mirroralpha = { "r_mirroralpha", "1" };



cvar_t	r_wadtextures = { "r_wadtextures", "0" };

cvar_t	gl_clear = { "gl_clear", "0" };


cvar_t	gl_wateramp = { "gl_wateramp", "0.3" };
cvar_t	gl_dither = { "gl_dither", "1", TRUE };
cvar_t	gl_spriteblend = { "gl_spriteblend", "1" };

/*
====================
R_InitTextures
====================
*/
void R_InitTextures( void )
{
	// TODO: Implement
}

/*
====================
R_Init

Initialize the renderer
====================
*/
void R_Init( void )
{
	// TODO: Implement

	Cvar_RegisterVariable(&r_norefresh);

	// TODO: Implement

	Cvar_RegisterVariable(&r_mirroralpha);

	// TODO: Implement

	Cvar_RegisterVariable(&r_speeds);
	Cvar_RegisterVariable(&r_wadtextures);

	// TODO: Implement

	Cvar_RegisterVariable(&gl_clear);

	// TODO: Implement

	Cvar_RegisterVariable(&gl_dither);
	Cvar_RegisterVariable(&gl_spriteblend);
	
	// TODO: Implement

	Cvar_RegisterVariable(&gl_wateramp);

	// TODO: Implement
}

/*
===============
R_NewMap
===============
*/
void R_NewMap( void )
{
	int		i;

	for (i = 0; i < 256; i++)
		d_lightstylevalue[i] = 264;		// normal light value

	memset(&r_worldentity, 0, sizeof(r_worldentity));
	r_worldentity.model = cl.worldmodel;

// clear out efrags in case the level hasn't been reloaded
// FIXME: is this one short?
	for (i = 0; i < cl.worldmodel->numleafs; i++)
		cl.worldmodel->leafs[i].efrags = NULL;

	r_viewleaf = NULL;
	R_ClearParticles();

	R_DecalInit();
	V_InitLevel();
	GL_BuildLightmaps();

	// identify sky texture
	skytexturenum = -1;
	mirrortexturenum = -1;
	for (i = 0; i < cl.worldmodel->numtextures; i++)
	{
		if (!cl.worldmodel->textures[i])
			continue;
		if (!Q_strncmp(cl.worldmodel->textures[i]->name, "sky", 3))
			skytexturenum = i;
		if (!Q_strncmp(cl.worldmodel->textures[i]->name, "window02_1", 10))
			mirrortexturenum = i;
		cl.worldmodel->textures[i]->texturechain = NULL;
	}
	R_LoadSkys();
	cl_entities->scale = gl_wateramp.value;

	// Unload textures from the previous map
	GL_UnloadTextures();
}