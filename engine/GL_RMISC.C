// r_misc.c

#include "quakedef.h"

cvar_t	r_norefresh = { "r_norefresh", "0" };


cvar_t	r_speeds = { "r_speeds", "0" };




cvar_t	r_novis = { "r_novis", "0" };




cvar_t	r_mirroralpha = { "r_mirroralpha", "1" };



cvar_t	r_wadtextures = { "r_wadtextures", "0" };

cvar_t	gl_clear = { "gl_clear", "0" };
cvar_t	gl_cull = { "gl_cull", "1" };
cvar_t	gl_texsort = { "gl_texsort", "1" };




cvar_t	gl_wateramp = { "gl_wateramp", "0.3" };
cvar_t	gl_dither = { "gl_dither", "1", TRUE };
cvar_t	gl_spriteblend = { "gl_spriteblend", "1" };


cvar_t	gl_zmax = { "gl_zmax", "4096" };



cvar_t	gl_envmapsize = { "gl_envmapsize", "256" };
cvar_t	gl_flipmatrix = { "gl_flipmatrix", "0", TRUE };




/*
====================
R_InitTextures
====================
*/
void R_InitTextures( void )
{
	int		x, y, m;
	byte* dest;

	// create a simple checkerboard texture for the default
	r_notexture_mip = (texture_t*)Hunk_AllocName(sizeof(texture_t) + 16 * 16 + 8 * 8 + 4 * 4 + 2 * 2, "notexture");

	r_notexture_mip->width = r_notexture_mip->height = 16;
	r_notexture_mip->offsets[0] = sizeof(texture_t);
	r_notexture_mip->offsets[1] = r_notexture_mip->offsets[0] + 16 * 16;
	r_notexture_mip->offsets[2] = r_notexture_mip->offsets[1] + 8 * 8;
	r_notexture_mip->offsets[3] = r_notexture_mip->offsets[2] + 4 * 4;

	for (m = 0; m < 4; m++)
	{
		dest = (byte*)r_notexture_mip + r_notexture_mip->offsets[m];
		for (y = 0; y < (16 >> m); y++)
		{
			for (x = 0; x < (16 >> m); x++)
				if ((y < (8 >> m)) ^ (x < (8 >> m)))
					*dest++ = 0;
				else
					*dest++ = 0xFF;
		}
	}
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

	Cvar_RegisterVariable(&r_novis);
	Cvar_RegisterVariable(&r_speeds);
	Cvar_RegisterVariable(&r_wadtextures);

	// TODO: Implement

	Cvar_RegisterVariable(&gl_clear);
	Cvar_RegisterVariable(&gl_texsort);
	Cvar_RegisterVariable(&gl_cull);

	// TODO: Implement

	Cvar_RegisterVariable(&gl_dither);
	Cvar_RegisterVariable(&gl_spriteblend);
	
	// TODO: Implement

	Cvar_RegisterVariable(&gl_wateramp);
	// TODO: Implement
	Cvar_RegisterVariable(&gl_zmax);
	// TODO: Implement
	Cvar_RegisterVariable(&gl_flipmatrix);

	// TODO: Implement

	if (gl_mtexable)
		Cvar_SetValue("gl_texsort", 0.0);

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