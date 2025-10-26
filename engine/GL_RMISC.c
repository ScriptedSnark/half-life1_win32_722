// r_misc.c

#include "quakedef.h"

cvar_t	r_cachestudio = { "r_cachestudio", "1" };
cvar_t	r_norefresh = { "r_norefresh", "0" };
cvar_t	r_drawentities = { "r_drawentities", "1" };
cvar_t	r_drawviewmodel = { "r_drawviewmodel", "1" };
cvar_t	r_speeds = { "r_speeds", "0" };
cvar_t	r_fullbright = { "r_fullbright", "0" };
cvar_t	r_decals = { "r_decals", "1" };
cvar_t	r_lightmap = { "r_lightmap", "0" };
cvar_t	r_shadows = { "r_shadows", "0" };
cvar_t	r_mirroralpha = { "r_mirroralpha", "1" };
cvar_t	r_wateralpha = { "r_wateralpha", "1" };
cvar_t	r_dynamic = { "r_dynamic", "1" };
cvar_t	r_novis = { "r_novis", "0" };
cvar_t	r_mmx = { "r_mmx", "0" };
cvar_t	r_traceglow = { "r_traceglow", "0" };
cvar_t	r_wadtextures = { "r_wadtextures", "0" };

cvar_t	gl_clear = { "gl_clear", "0" };
cvar_t	gl_cull = { "gl_cull", "1" };
cvar_t	gl_texsort = { "gl_texsort", "1" };
cvar_t	gl_smoothmodels = { "gl_smoothmodels", "1" };
cvar_t	gl_affinemodels = { "gl_affinemodels", "0" };
cvar_t	gl_flashblend = { "gl_flashblend", "0" };
cvar_t	gl_playermip = { "gl_playermip", "0" };
cvar_t	gl_nocolors = { "gl_nocolors", "0" };
cvar_t	gl_keeptjunctions = { "gl_keeptjunctions", "1" };
cvar_t	gl_reporttjunctions = { "gl_reporttjunctions", "0" };
cvar_t	gl_wateramp = { "gl_wateramp", "0.3" };
cvar_t	gl_dither = { "gl_dither", "1", TRUE };
cvar_t	gl_spriteblend = { "gl_spriteblend", "1" };
cvar_t	gl_polyoffset = { "gl_polyoffset", "4", TRUE };
cvar_t	gl_lightholes = { "gl_lightholes", "1" };
cvar_t	gl_zmax = { "gl_zmax", "4096" };
cvar_t	gl_alphamin = { "gl_alphamin", "0.25" };
cvar_t	gl_overdraw = { "gl_overdraw", "0" };
cvar_t	gl_watersides = { "gl_watersides", "0" };
cvar_t	gl_overbright = { "gl_overbright", "1", TRUE };
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

void R_UploadEmptyTex( void )
{
	byte pPal[768];
	memset(pPal, 0, sizeof(pPal));
	pPal[765] = 255;	// r
	pPal[766] = 0;		// g
	pPal[767] = 255;	// b

	r_notexture_mip->gl_texturenum = GL_LoadTexture("**empty**", GLT_SYSTEM, r_notexture_mip->width, r_notexture_mip->height, (byte*)(r_notexture_mip + 1), TRUE, TEX_TYPE_NONE, pPal);
}

byte	dottexture[8][8] =
{
	{0,1,1,0,0,0,0,0},
	{1,1,1,1,0,0,0,0},
	{1,1,1,1,0,0,0,0},
	{0,1,1,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
};
void R_InitParticleTexture( void )
{
	int		x, y;
	byte	data[8][8][4];

	//
	// particle texture
	//
	particletexture = texture_extension_number++;
	GL_Bind(particletexture);

	for (x = 0; x < 8; x++)
	{
		for (y = 0; y < 8; y++)
		{
			data[y][x][0] = 255;
			data[y][x][1] = 255;
			data[y][x][2] = 255;
			data[y][x][3] = dottexture[x][y] * 255;
		}
	}
	qglTexImage2D(GL_TEXTURE_2D, 0, 4, 8, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

/*
===============
R_Envmap_f

Grab six views for environment mapping tests
===============
*/
void R_Envmap_f( void )
{
	char	name[1024];
	vrect_t saveVrect;
	char	base[64];

	// Extract the base name
	if (cl.num_entities && cl_entities->model)
		COM_FileBase(cl_entities->model->name, base);
	else
		strcpy(base, "Env");

	qglDrawBuffer(GL_FRONT);
	qglReadBuffer(GL_FRONT);
	envmap = TRUE;

	memcpy(&saveVrect, &r_refdef.vrect, sizeof(vrect_t));
	r_refdef.vrect.x = 0;
	r_refdef.vrect.y = 0;
	r_refdef.vrect.width = gl_envmapsize.value;
	r_refdef.vrect.height = gl_envmapsize.value;

	r_refdef.viewangles[0] = 0;
	r_refdef.viewangles[1] = 0;
	r_refdef.viewangles[2] = 0;
	GL_BeginRendering(&glx, &gly, &glwidth, &glheight);
	R_RenderView();
	sprintf(name, "%srt.bmp", base);
	VID_TakeSnapshotRect(name, 0, 0, gl_envmapsize.value, gl_envmapsize.value);

	r_refdef.viewangles[1] = 90;
	GL_BeginRendering(&glx, &gly, &glwidth, &glheight);
	R_RenderView();
	sprintf(name, "%sbk.bmp", base);
	VID_TakeSnapshotRect(name, 0, 0, gl_envmapsize.value, gl_envmapsize.value);

	r_refdef.viewangles[1] = 180;
	GL_BeginRendering(&glx, &gly, &glwidth, &glheight);
	R_RenderView();
	sprintf(name, "%slf.bmp", base);
	VID_TakeSnapshotRect(name, 0, 0, gl_envmapsize.value, gl_envmapsize.value);

	r_refdef.viewangles[1] = 270;
	GL_BeginRendering(&glx, &gly, &glwidth, &glheight);
	R_RenderView();
	sprintf(name, "%sft.bmp", base);
	VID_TakeSnapshotRect(name, 0, 0, gl_envmapsize.value, gl_envmapsize.value);

	r_refdef.viewangles[1] = 0;
	r_refdef.viewangles[0] = -90;
	GL_BeginRendering(&glx, &gly, &glwidth, &glheight);
	R_RenderView();
	sprintf(name, "%sup.bmp", base);
	VID_TakeSnapshotRect(name, 0, 0, gl_envmapsize.value, gl_envmapsize.value);

	r_refdef.viewangles[1] = 0;
	r_refdef.viewangles[0] = 90;
	GL_BeginRendering(&glx, &gly, &glwidth, &glheight);
	R_RenderView();
	sprintf(name, "%sdn.bmp", base);
	VID_TakeSnapshotRect(name, 0, 0, gl_envmapsize.value, gl_envmapsize.value);

	memcpy(&r_refdef.vrect, &saveVrect, sizeof(vrect_t));
	envmap = FALSE;
	qglDrawBuffer(GL_BACK);
	qglReadBuffer(GL_BACK);
	GL_EndRendering();
}

/*
====================
R_Init

Initialize the renderer
====================
*/
void R_Init( void )
{
	Cmd_AddCommand("timerefresh", R_TimeRefresh_f);
	Cmd_AddCommand("envmap", R_Envmap_f);
	Cmd_AddCommand("pointfile", R_ReadPointFile_f);

	Cvar_RegisterVariable(&r_norefresh);
	Cvar_RegisterVariable(&r_lightmap);
	Cvar_RegisterVariable(&r_fullbright);
	Cvar_RegisterVariable(&r_decals);
	Cvar_RegisterVariable(&r_drawentities);
	Cvar_RegisterVariable(&r_drawviewmodel);
	Cvar_RegisterVariable(&r_mirroralpha);
	Cvar_RegisterVariable(&r_wateralpha);
	Cvar_RegisterVariable(&r_dynamic);
	Cvar_RegisterVariable(&r_novis);
	Cvar_RegisterVariable(&r_speeds);
	Cvar_RegisterVariable(&r_wadtextures);
	Cvar_RegisterVariable(&r_shadows);
	Cvar_RegisterVariable(&r_mmx);
	Cvar_RegisterVariable(&r_traceglow);

	Cvar_RegisterVariable(&gl_clear);
	Cvar_RegisterVariable(&gl_texsort);
	Cvar_RegisterVariable(&gl_cull);
	Cvar_RegisterVariable(&gl_smoothmodels);
	Cvar_RegisterVariable(&gl_affinemodels);
	Cvar_RegisterVariable(&gl_playermip);
	Cvar_RegisterVariable(&gl_nocolors);
	Cvar_RegisterVariable(&gl_dither);
	Cvar_RegisterVariable(&gl_spriteblend);
	Cvar_RegisterVariable(&gl_polyoffset);
	Cvar_RegisterVariable(&gl_lightholes);
	Cvar_RegisterVariable(&gl_keeptjunctions);
	Cvar_RegisterVariable(&gl_reporttjunctions);
	Cvar_RegisterVariable(&gl_wateramp);
	Cvar_RegisterVariable(&gl_overbright);
	Cvar_RegisterVariable(&gl_zmax);
	Cvar_RegisterVariable(&gl_alphamin);
	Cvar_RegisterVariable(&gl_flipmatrix);

	if (gl_mtexable)
		Cvar_SetValue("gl_texsort", 0.0);

	R_InitParticles();
	R_InitParticleTexture();
	R_UploadEmptyTex();

	playertextures = texture_extension_number;
	texture_extension_number += 16;
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

/*
====================
R_TimeRefresh_f

For program optimization
====================
*/
void R_TimeRefresh_f( void )
{
	int			i;
	float		start, stop, time;

	qglDrawBuffer(GL_FRONT);
	qglFinish();

	start = Sys_FloatTime();
	for (i = 0; i < 128; i++)
	{
		r_refdef.viewangles[1] = i / 128.0 * 360.0;
		R_RenderView();
	}

	qglFinish();
	stop = Sys_FloatTime();
	time = stop - start;
	Con_Printf("%f seconds (%f fps)\n", time, 128 / time);

	qglDrawBuffer(GL_BACK);
	GL_EndRendering();
}

void D_FlushCaches( void )
{
}