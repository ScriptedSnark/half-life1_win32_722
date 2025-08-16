// draw.c -- this is the only file outside the refresh that touches the
// vid buffer

#include "quakedef.h"
#include "winquake.h"
#include "decal.h"
#include "pr_cmds.h"
#include "opengl2d3d.h"

cvar_t		gl_nobind = { "gl_nobind", "0" };
cvar_t		gl_max_size = { "gl_max_size", "256" };
cvar_t		gl_round_down = { "gl_round_down", "3" };
cvar_t		gl_picmip = { "gl_picmip", "0" };
cvar_t		gl_palette_tex = { "gl_palette_tex", "1" };

int		font_texture;
qfont_t* draw_creditsfont;
qfont_t* draw_chars;
qpic_t* draw_disc;
qpic_t* draw_backtile;

int			translate_texture;
int			char_texture;

typedef struct
{
	int		texnum;
	float	sl, tl, sh, th;
} glpic_t;

byte		conback_buffer[sizeof(qpic_t) + sizeof(glpic_t)];
qpic_t* conback = (qpic_t*)&conback_buffer;

int		gl_lightmap_format = GL_RGBA;
//int		gl_solid_format = 3;
//int		gl_alpha_format = 4;

int		gl_filter_min = GL_LINEAR_MIPMAP_NEAREST;
int		gl_filter_max = GL_LINEAR;


int		texels;

typedef struct
{
	int		tag;
	byte	colors[768];
} GL_PALETTE;

GL_PALETTE gGLPalette[350];

typedef struct
{
	char name[64];
	qpic_t pic;
	byte padding[32];
} cachepic_t;

#define	MAX_CACHED_PICS		16
cachepic_t	menu_cachepics[MAX_CACHED_PICS];
int			menu_numcachepics;

byte		menuplyr_pixels[4096];

int		pic_texels;
int		pic_count;

cachewad_t	decal_wad;
cachewad_t	custom_wad;
cachewad_t	menu_wad;

typedef struct
{
	int			texnum;
	short		servercount;
	short		paletteIndex;
	int			width, height;
	qboolean	mipmap;
	char		identifier[MAX_QPATH];
} gltexture_t;

#define	MAX_GLTEXTURES	1500
gltexture_t gltextures[MAX_GLTEXTURES];
int			numgltextures;

float		chars_xsize, chars_ysize;
float		creditsfont_ysize;

char		decal_names[MAX_BASE_DECALS][16];

void	GL_PaletteInit( void );
void	GL_PaletteSelect( int paletteIndex );

qpic_t* LoadTransBMP( char* pszName );
qpic_t* LoadTransPic( char* pszName, qpic_t* ppic );

void GL_Bind( int texnum )
{
	int paletteIndex = (texnum >> 16) - 1;

	texnum = (texnum & 0xFFFF);
	if (gl_nobind.value)
		texnum = char_texture;
	if (currenttexture == texnum)
		return;
	currenttexture = texnum;
	qglBindTexture(GL_TEXTURE_2D, texnum);

	if (paletteIndex >= 0)
		GL_PaletteSelect(paletteIndex);
}

void GL_Texels_f( void )
{
	Con_Printf("Current uploaded texels: %i\n", texels);
}

/****************************************/

static GLenum oldtarget = TEXTURE0_SGIS;

void GL_SelectTexture( GLenum target )
{
	if (!gl_mtexable)
		return;
	qglSelectTextureSGIS(target);
	if (target == oldtarget)
		return;
	cnttextures[oldtarget - TEXTURE0_SGIS] = currenttexture;
	currenttexture = cnttextures[target - TEXTURE0_SGIS];
	oldtarget = target;
}

//=============================================================================
/* Support Routines */

qpic_t* Draw_PicFromWad( char* name )
{
	qpic_t* p;
	glpic_t* gl;

	p = (qpic_t*)W_GetLumpName(name);
	gl = (glpic_t*)p->data;

	gl->texnum = GL_LoadPicTexture(p, name);
	gl->sl = 0;
	gl->sh = 1;
	gl->tl = 0;
	gl->th = 1;

	return p;
}

qpic_t* Draw_CachePic( char* path )
{
	qpic_t* ret;
	int idx;

	idx = Draw_CacheIndex(&menu_wad, path);
	ret = (qpic_t*)Draw_CacheGet(&menu_wad, idx);
	return ret;
}

/*
===============
Draw_StringLen
===============
*/
int Draw_StringLen( char* psz )
{
	int totalWidth = 0;

	while (psz && *psz)
	{
		totalWidth += draw_chars->fontinfo[*psz].charwidth;
		psz++;
	}
	return totalWidth;
}

int Draw_MessageFontInfo( short* pWidth )
{
	int i;

	if (!draw_creditsfont)
		return 0;

	if (pWidth)
	{
		for (i = 0; i < 256; i++)
			*pWidth++ = draw_creditsfont->fontinfo[i].charwidth;
	}

	return draw_creditsfont->rowheight;
}

int Draw_MessageCharacterAdd( int x, int y, int num, int rr, int gg, int bb )
{
	int				row, col;
	int				rowheight, charwidth;
	float			frow, fcol, xsize, ysize;

	num &= 255;

	rowheight = draw_creditsfont->rowheight;
	if (y <= -rowheight)
		return 0;			// totally off screen

	charwidth = draw_creditsfont->fontinfo[num].charwidth;

	col = draw_creditsfont->fontinfo[num].startoffset & 255;
	fcol = col * chars_xsize;
	row = (draw_creditsfont->fontinfo[num].startoffset & ~255) >> 8;
	frow = row * creditsfont_ysize;

	xsize = charwidth * chars_xsize;
	ysize = rowheight * creditsfont_ysize;
	
	qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	qglEnable(GL_ALPHA_TEST);
	qglEnable(GL_BLEND);
	qglBlendFunc(GL_ONE, GL_ONE);
	qglColor4ub(rr, gg, bb, 255);

	GL_DisableMultitexture();
	GL_Bind(font_texture);

	qglBegin(GL_QUADS);
	qglTexCoord2f(fcol, frow);
	qglVertex2f(x, y);
	qglTexCoord2f(fcol + xsize, frow);
	qglVertex2f(x + charwidth, y);
	qglTexCoord2f(fcol + xsize, frow + ysize);
	qglVertex2f(x + charwidth, y + rowheight);
	qglTexCoord2f(fcol, frow + ysize);
	qglVertex2f(x, y + rowheight);
	qglEnd();

	qglDisable(GL_BLEND);

	return charwidth;
}

void Draw_CharToConback( int num, byte* dest )
{
}

typedef struct
{
	char* name;
	int	minimize, maximize;
} quake_mode_t;

quake_mode_t modes[] = {
	{ "GL_NEAREST", GL_NEAREST, GL_NEAREST },
	{ "GL_LINEAR", GL_LINEAR, GL_LINEAR },
	{ "GL_NEAREST_MIPMAP_NEAREST", GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST },
	{ "GL_LINEAR_MIPMAP_NEAREST", GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR },
	{ "GL_NEAREST_MIPMAP_LINEAR", GL_NEAREST_MIPMAP_LINEAR, GL_NEAREST },
	{ "GL_LINEAR_MIPMAP_LINEAR", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR }
};

/*
===============
Draw_TextureMode_f
===============
*/
void Draw_TextureMode_f( void )
{
	int		i;
	gltexture_t* glt;

	if (Cmd_Argc() == 1)
	{
		for (i = 0; i < 6; i++)
		{
			if (gl_filter_min == modes[i].minimize)
			{
				Con_Printf("%s\n", modes[i].name);
				return;
			}
		}
		Con_Printf("current filter is unknown???\n");
		return;
	}

	for (i = 0; i < 6; i++)
	{
		if (!Q_strcasecmp(modes[i].name, Cmd_Argv(1)))
			break;
	}
	if (i == 6)
	{
		Con_Printf("bad filter name\n");
		return;
	}

	gl_filter_min = modes[i].minimize;
	gl_filter_max = modes[i].maximize;

	// change all the existing mipmap texture objects
	for (i = 0, glt = gltextures; i < numgltextures; i++, glt++)
	{
		if (glt->mipmap)
		{
			GL_Bind(glt->texnum);
			qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter_min);
			qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter_max);
		}
	}
}

// This is called to reset all loaded decals
// called from cl_parse.c and host.c
void Decal_Init( void )
{
	int i;

	Draw_CacheWadInit("decals.wad", MAX_BASE_DECALS, &decal_wad);

	sv_decalnamecount = Draw_DecalCount();
	if (sv_decalnamecount > MAX_BASE_DECALS)
		Sys_Error("Too many decals: %d / %d\n", sv_decalnamecount, MAX_BASE_DECALS);

	for (i = 0; i < sv_decalnamecount; i++)
	{
		memset(&sv_decalnames[i], 0, sizeof(decalname_t));
		strncpy(sv_decalnames[i].name, Draw_DecalName(i), sizeof(sv_decalnames[i].name) - 1);
	}
}

/*
===============
Draw_Init
===============
*/
void Draw_Init( void )
{
	int				i;
	qpic_t*			cb;
	glpic_t*		gl;
	unsigned char*	pPal;
	float			prev;

	Draw_CacheWadInit("cached.wad", 16, &menu_wad);
	menu_wad.tempWad = TRUE;

	Draw_CacheWadHandler(&decal_wad, Draw_MiptexTexture, MIP_EXTRASIZE);
	Draw_CacheWadHandler(&custom_wad, Draw_MiptexTexture, MIP_EXTRASIZE);

	Cvar_RegisterVariable(&gl_nobind);
	Cvar_RegisterVariable(&gl_max_size);
	Cvar_RegisterVariable(&gl_round_down);
	Cvar_RegisterVariable(&gl_picmip);
	Cvar_RegisterVariable(&gl_palette_tex);

	GL_PaletteInit();

	memset(decal_names, 0, sizeof(decal_names));

	Cmd_AddCommand("gl_texturemode", Draw_TextureMode_f);
	Cmd_AddCommand("gl_texels", GL_Texels_f);

	for (i = 0; i < 256; i++)
		texgammatable[i] = i;

	draw_chars = (qfont_t*)W_GetLumpName("conchars");
	draw_creditsfont = (qfont_t*)W_GetLumpName("creditsfont");

	if (con_loading)
		cb = (qpic_t*)Draw_CachePic("gfx/loading.lmp");
	else
		cb = (qpic_t*)Draw_CachePic("gfx/conback.lmp");
	if (!cb)
		Sys_Error("Couldn't load conback.lmp");

	SwapPic(cb);

	conback->width = cb->width;
	conback->height = cb->height;

	gl = (glpic_t*)conback->data;
	pPal = &cb->data[cb->width * cb->height + 2];
	gl->texnum = GL_LoadTexture("conback", GLT_SYSTEM, cb->width, cb->height, cb->data, FALSE, TEX_TYPE_NONE, pPal);
	gl->sl = 0;
	gl->sh = 1;
	gl->tl = 0;
	gl->th = 1;

	prev = gl_round_down.value;
	gl_round_down.value = 0.0;

	// now turn them into textures
	pPal = &draw_chars->data[256 * draw_chars->height + 2];
	char_texture = GL_LoadTexture("conchars", GLT_SYSTEM, 256, draw_chars->height, draw_chars->data, FALSE, TEX_TYPE_ALPHA, pPal);
	pPal = &draw_creditsfont->data[256 * draw_creditsfont->height + 2];
	font_texture = GL_LoadTexture("creditsfont", GLT_SYSTEM, 256, draw_creditsfont->height, draw_creditsfont->data, FALSE, TEX_TYPE_NONE, pPal);

	gl_round_down.value = prev;

	chars_xsize = 1.0 / 256;
	chars_ysize = 1.0 / draw_chars->height;

	creditsfont_ysize = 1.0 / draw_creditsfont->height;

	// save a texture slot for translated picture
	translate_texture = texture_extension_number++;

	draw_disc = (qpic_t*)LoadTransBMP("lambda");
}

/*
================
Draw_Character

Draws a single character
================
*/
int Draw_Character( int x, int y, int num )
{
	int				row, col;
	int				rowheight, charwidth;
	float			frow, fcol, xsize, ysize;

	num &= 255;

	rowheight = draw_chars->rowheight;
	if (y <= -rowheight)
		return 0;			// totally off screen

	charwidth = draw_chars->fontinfo[num].charwidth;
	if (y < 0 || num == 32)
		return charwidth;		// space

	col = draw_chars->fontinfo[num].startoffset & 255;
	fcol = col * chars_xsize;
	row = (draw_chars->fontinfo[num].startoffset & ~255) >> 8;
	frow = row * chars_ysize;

	xsize = charwidth * chars_xsize;
	ysize = rowheight * chars_ysize;
	
	qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	qglEnable(GL_ALPHA_TEST);
	qglColor4f(1, 1, 1, 1);

	GL_DisableMultitexture();
	GL_Bind(char_texture);

	qglBegin(GL_QUADS);
	qglTexCoord2f(fcol, frow);
	qglVertex2f(x, y);
	qglTexCoord2f(fcol + xsize, frow);
	qglVertex2f(x + charwidth, y);
	qglTexCoord2f(fcol + xsize, frow + ysize);
	qglVertex2f(x + charwidth, y + rowheight);
	qglTexCoord2f(fcol, frow + ysize);
	qglVertex2f(x, y + rowheight);
	qglEnd();

	qglDisable(GL_BLEND);

	return charwidth;
}

/*
================
Draw_String
================
*/
int Draw_String( int x, int y, char* str )
{
	while (*str)
	{
		x += Draw_Character(x, y, *str);
		str++;
	}
	return x;
}

/*
================
Draw_DebugChar

Draws a single character directly to the upper right corner of the screen.
This is for debugging lockups by drawing different chars in different parts
of the code.
================
*/
void Draw_DebugChar( char num )
{
}

/*
=============
Draw_Pic
=============
*/
void Draw_Pic( int x, int y, qpic_t *pic )
{
	glpic_t* gl;

	if (!pic)
		return;

	qglEnable(GL_TEXTURE_2D);
	qglDisable(GL_BLEND);
	qglDisable(GL_DEPTH_TEST);
	qglEnable(GL_ALPHA_TEST);
	qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	gl = (glpic_t*)pic->data;
	qglColor4f(1, 1, 1, 1);
	GL_Bind(gl->texnum);
	qglBegin(GL_QUADS);
	qglTexCoord2f(gl->sl, gl->tl);
	qglVertex2f(x, y);
	qglTexCoord2f(gl->sh, gl->tl);
	qglVertex2f(x + pic->width, y);
	qglTexCoord2f(gl->sh, gl->th);
	qglVertex2f(x + pic->width, y + pic->height);
	qglTexCoord2f(gl->sl, gl->th);
	qglVertex2f(x, y + pic->height);
	qglEnd();
}

/*
=============
Draw_AlphaSubPic
=============
*/
void Draw_AlphaSubPic( int xDest, int yDest, int xSrc, int ySrc, int iWidth, int iHeight, qpic_t* pPic, colorVec* pc, int iAlpha )
{
	glpic_t* gl;
	float flX, flY, flHeight, flWidth, alpha;

	if (!pPic)
		return;

	qglBlendFunc(GL_ONE, GL_ONE);
	qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	qglEnable(GL_BLEND);
	qglEnable(GL_ALPHA_TEST);

	flWidth = (float)iWidth / (float)pPic->width;
	flHeight = (float)iHeight / (float)pPic->height;

	flX = (float)xSrc / (float)pPic->width;
	flY = (float)ySrc / (float)pPic->height;

	alpha = iAlpha / 255.0;

	gl = (glpic_t*)pPic->data;
	qglColor4f((pc->r * alpha) / 256.0, (pc->g * alpha) / 256.0, (pc->b * alpha) / 256.0, 1);
	GL_Bind(gl->texnum);
	qglBegin(GL_QUADS);
	qglTexCoord2f(flX, flY);
	qglVertex2f(xDest, yDest);
	qglTexCoord2f(flX + flWidth, flY);
	qglVertex2f(iWidth + xDest, yDest);
	qglTexCoord2f(flX + flWidth, flY + flHeight);
	qglVertex2f(iWidth + xDest, yDest + iHeight);
	qglTexCoord2f(flX, flY + flHeight);
	qglVertex2f(xDest, yDest + iHeight);
	qglEnd();

	qglDisable(GL_BLEND);
}

/*
=============
Draw_AlphaPic
=============
*/
void Draw_AlphaPic( int x, int y, qpic_t* pic, colorVec* pc, int iAlpha )
{
	glpic_t* gl;
	float alpha;

	if (!pic)
		return;

	qglBlendFunc(GL_ONE, GL_ONE);
	qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	qglEnable(GL_BLEND);
	qglEnable(GL_ALPHA_TEST);

	alpha = iAlpha / 255.0;
	if (pc)
	{
		qglColor3f((pc->r * alpha) / 256.0, (pc->g * alpha) / 256.0, (pc->b * alpha) / 256.0);
	}
	else
	{
		qglColor3f(alpha, alpha, alpha);
	}

	gl = (glpic_t*)pic->data;
	GL_Bind(gl->texnum);
	qglBegin(GL_QUADS);
	qglTexCoord2f(gl->sl, gl->tl);
	qglVertex2f(x, y);
	qglTexCoord2f(gl->sh, gl->tl);
	qglVertex2f(x + pic->width, y);
	qglTexCoord2f(gl->sh, gl->th);
	qglVertex2f(x + pic->width, y + pic->height);
	qglTexCoord2f(gl->sl, gl->th);
	qglVertex2f(x, y + pic->height);
	qglEnd();

	qglDisable(GL_BLEND);
}

/*
=============
Draw_AlphaAddPic
=============
*/
void Draw_AlphaAddPic( int x, int y, qpic_t* pic, colorVec* pc, int iAlpha )
{
	glpic_t* gl;

	if (!pic)
		return;

	qglEnable(GL_TEXTURE_2D);
	qglBlendFunc(GL_SRC_ALPHA, GL_ONE);
	qglEnable(GL_BLEND);
	qglEnable(GL_ALPHA_TEST);
	qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	if (pc)
	{
		qglColor4f(pc->r / 255.0, pc->g / 255.0, pc->b / 255.0, iAlpha / 255.0);
	}
	else
	{
		qglColor4f(1, 1, 1, iAlpha / 255.0);
	}

	gl = (glpic_t*)pic->data;
	GL_Bind(gl->texnum);
	qglBegin(GL_QUADS);
	qglTexCoord2f(gl->sl, gl->tl);
	qglVertex2f(x, y);
	qglTexCoord2f(gl->sh, gl->tl);
	qglVertex2f(x + pic->width, y);
	qglTexCoord2f(gl->sh, gl->th);
	qglVertex2f(x + pic->width, y + pic->height);
	qglTexCoord2f(gl->sl, gl->th);
	qglVertex2f(x, y + pic->height);
	qglEnd();

	qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter_max);
}

/*
=============
Draw_Pic2
=============
*/
void Draw_Pic2( int x, int y, int w, int h, qpic_t* pic )
{
	glpic_t* gl;

	if (!pic)
		return;

	qglEnable(GL_TEXTURE_2D);
	qglDisable(GL_BLEND);
	qglEnable(GL_ALPHA_TEST);
	qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter_max);

	gl = (glpic_t*)pic->data;
	GL_Bind(gl->texnum);
	qglBegin(GL_QUADS);
	qglTexCoord2f(gl->sl, gl->tl);
	qglVertex2f(x, y);
	qglTexCoord2f(gl->sh, gl->tl);
	qglVertex2f(x + w, y);
	qglTexCoord2f(gl->sh, gl->th);
	qglVertex2f(x + w, h - 1 + y);
	qglTexCoord2f(gl->sl, gl->th);
	qglVertex2f(x, h - 1 + y);
	qglEnd();
}

/*
=============
Draw_TransPic
=============
*/
void Draw_TransPic( int x, int y, qpic_t* pic )
{
	if (!pic)
		return;

	if (x < 0 || (unsigned)(x + pic->width) > vid.width || y < 0 ||
		(unsigned)(y + pic->height) > vid.height)
	{
		Sys_Error("Draw_TransPic: bad coordinates");
	}

	Draw_Pic(x, y, pic);
}


/*
=============
Draw_TransPicTranslate

Only used for the player color selection menu
=============
*/
void Draw_TransPicTranslate( int x, int y, qpic_t* pic, unsigned char* translation )
{
	int				v, u, c;
	unsigned		trans[64 * 64], * dest;
	byte* src;
	int				p;

	if (!pic)
		return;

	GL_Bind(translate_texture);

	c = pic->width * pic->height;

	dest = trans;
	for (v = 0; v < 64; v++, dest += 64)
	{
		src = &menuplyr_pixels[((v * pic->height) >> 6) * pic->width];
		for (u = 0; u < 64; u++)
		{
			p = src[(u * pic->width) >> 6];
			if (p == 255)
				dest[u] = p;
			else
				dest[u] = 0xFF0000FF;
		}
	}

	qglTexImage2D(GL_TEXTURE_2D, 0, 4, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, trans);

	qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	qglColor3f(1, 1, 1);
	qglBegin(GL_QUADS);
	qglTexCoord2f(0, 0);
	qglVertex2f(x, y);
	qglTexCoord2f(1, 0);
	qglVertex2f(x + pic->width, y);
	qglTexCoord2f(1, 1);
	qglVertex2f(x + pic->width, y + pic->height);
	qglTexCoord2f(0, 1);
	qglVertex2f(x, y + pic->height);
	qglEnd();
}

// Sprites are clipped to this rectangle (x,y,width,height) if ScissorTest is enabled
int scissor_x = 0, scissor_y = 0, scissor_width = 0, scissor_height = 0;
qboolean giScissorTest = FALSE;

/*
===============
EnableScissorTest

Set the scissor
 the coordinate system for gl is upsidedown (inverted-y) as compared to software, so the
 specified clipping rect must be flipped
===============
*/
void EnableScissorTest( int x, int y, int width, int height )
{
	// Added casts to int because these warnings are so annoying
	x = clamp(x, 0, (int)vid.width);
	y = clamp(y, 0, (int)vid.height);
	width = clamp(width, 0, (int)vid.width - x);
	height = clamp(height, 0, (int)vid.height - y);

	scissor_x = x;
	scissor_y = y;
	scissor_width = width;
	scissor_height = height;

	giScissorTest = TRUE;
}

/*
===============
DisableScissorTest
===============
*/
void DisableScissorTest( void )
{
	scissor_x = 0;
	scissor_y = 0;
	scissor_width = 0;
	scissor_height = 0;

	giScissorTest = FALSE;
}

/*
===============
ValidateWRect

Verify that this is a valid, properly ordered rectangle.
===============
*/
int ValidateWRect( const wrect_t* prc )
{
	if (!prc)
		return FALSE;

	if ((prc->left >= prc->right) || (prc->top >= prc->bottom))
	{
		//!!!UNDONE Dev only warning msg
		return FALSE;
	}

	return TRUE;
}

/*
===============
IntersectWRect

classic interview question
===============
*/
int IntersectWRect( const wrect_t* prc1, const wrect_t* prc2, wrect_t* prc )
{
	wrect_t rc;

	if (!prc)
		prc = &rc;

	prc->left = max(prc1->left, prc2->left);
	prc->right = min(prc1->right, prc2->right);

	if (prc->left < prc->right)
	{
		prc->top = max(prc1->top, prc2->top);
		prc->bottom = min(prc1->bottom, prc2->bottom);

		if (prc->top < prc->bottom)
			return TRUE;
	}

	return FALSE;
}

/*
===============
AdjustSubRect
===============
*/
void AdjustSubRect( mspriteframe_t* pFrame, float* pfLeft, float* pfRight, float* pfTop, float* pfBottom, int* pw, int* ph, const wrect_t* prcSubRect )
{
	wrect_t rc;

	if (!ValidateWRect(prcSubRect))
		return;

	// clip sub rect to sprite

	rc.top = rc.left = 0;
	rc.right = *pw;
	rc.bottom = *ph;

	if (!IntersectWRect(prcSubRect, &rc, &rc))
		return;

	*pw = rc.right - rc.left;
	*ph = rc.bottom - rc.top;

	*pfLeft = rc.left / (float)pFrame->width;
	*pfRight = rc.right / (float)pFrame->width;
	*pfTop = rc.top / (float)pFrame->height;
	*pfBottom = rc.bottom / (float)pFrame->height;
}

/*
===============
Draw_Frame
===============
*/
void Draw_Frame( mspriteframe_t* pFrame, int x, int y, const wrect_t* prcSubRect )
{
	float	fLeft = 0;
	float	fRight = 1;
	float	fTop = 0;
	float	fBottom = 1;
	int		iWidth;
	int		iHeight;

	iWidth = pFrame->width;
	iHeight = pFrame->height;

	if (giScissorTest)
	{
		qglScissor(scissor_x, scissor_y, scissor_width, scissor_height);
		qglEnable(GL_SCISSOR_TEST);
	}

	if (prcSubRect)
	{
		AdjustSubRect(pFrame, &fLeft, &fRight, &fTop, &fBottom, &iWidth, &iHeight, prcSubRect);
	}

	qglDepthMask(GL_FALSE);

	GL_Bind(pFrame->gl_texturenum);
	qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	qglBegin(GL_QUADS);
	qglTexCoord2f(fLeft, fTop);
	qglVertex2f(x, y);
	qglTexCoord2f(fRight, fTop);
	qglVertex2f(x + iWidth, y);
	qglTexCoord2f(fRight, fBottom);
	qglVertex2f(x + iWidth, y + iHeight);
	qglTexCoord2f(fLeft, fBottom);
	qglVertex2f(x, y + iHeight);
	qglEnd();

	qglDepthMask(GL_TRUE);
	qglDisable(GL_SCISSOR_TEST);
}

void Draw_SpriteFrame( mspriteframe_t* pFrame, unsigned short* pPalette, int x, int y, const wrect_t* prcSubRect )
{
	Draw_Frame(pFrame, x, y, prcSubRect);
}

void Draw_SpriteFrameHoles( mspriteframe_t* pFrame, unsigned short* pPalette, int x, int y, const wrect_t* prcSubRect )
{
	qglEnable(GL_ALPHA_TEST);

	if (gl_spriteblend.value)
	{
		qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		qglEnable(GL_BLEND);
	}

	Draw_Frame(pFrame, x, y, prcSubRect);

	qglDisable(GL_ALPHA_TEST);
	qglDisable(GL_BLEND);
}

void Draw_SpriteFrameAdditive( mspriteframe_t* pFrame, unsigned short* pPalette, int x, int y, const wrect_t* prcSubRect )
{
	qglEnable(GL_BLEND);
	qglBlendFunc(GL_ONE, GL_ONE);

	Draw_Frame(pFrame, x, y, prcSubRect);

	qglDisable(GL_BLEND);
}

/*
================
Draw_ConsoleBackground

================
*/
void Draw_ConsoleBackground( int lines )
{
	char ver[100];
	int x;

	Draw_Pic2(0, lines - glheight, glwidth, glheight + 1, conback);

	sprintf(ver, "Half-Life 1.0 (build %d)", build_number());

	x = vid.conwidth - Draw_StringLen(ver);
	if (!con_loading && !(giSubState & 4))
	{
		Draw_String(x, 0, ver);
	}
}

/*
===============
Draw_FillRGBA

Fills the given rectangle with a given color
===============
*/
void Draw_FillRGBA( int x, int y, int w, int h, int r, int g, int b, int a )
{
	qglDisable(GL_TEXTURE_2D);
	qglEnable(GL_BLEND);

	qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	qglBlendFunc(GL_SRC_ALPHA, GL_ONE);

	qglColor4f(r / 255.0, g / 255.0, b / 255.0, a / 255.0);

	qglBegin(GL_QUADS);
	qglVertex2f(x, y);
	qglVertex2f(x + w, y);
	qglVertex2f(x + w, y + h);
	qglVertex2f(x, y + h);
	qglEnd();

	qglColor3f(1, 1, 1);

	qglEnable(GL_TEXTURE_2D);
	qglDisable(GL_BLEND);
}

/*
=============
Draw_TileClear

This repeats a 64*64 tile graphic to fill the screen around a sized down
refresh window.
=============
*/
void Draw_TileClear( int x, int y, int w, int h )
{
	Draw_FillRGBA(x, y, w, h, 0, 0, 0, 255);
}

/*
=============
Draw_Fill

Fills a box of pixels with a single color
=============
*/
void Draw_Fill( int x, int y, int w, int h, int c )
{
	Draw_FillRGBA(x, y, w, h,
		host_basepal[c * 4],
		host_basepal[c * 4 + 1],
		host_basepal[c * 4 + 2],
		255);
}
//=============================================================================

/*
================
Draw_FadeScreen
================
*/
void Draw_FadeScreen( void )
{
	qglEnable(GL_BLEND);
	qglDisable(GL_TEXTURE_2D);
	qglColor4f(0, 0, 0, 0.8);
	qglBegin(GL_QUADS);

	qglVertex2f(0, 0);
	qglVertex2f(glwidth, 0);
	qglVertex2f(glwidth, glheight);
	qglVertex2f(0, glheight);

	qglEnd();
	qglColor4f(1, 1, 1, 1);
	qglEnable(GL_TEXTURE_2D);
	qglDisable(GL_BLEND);
}

//=============================================================================

/*
================
Draw_BeginDisc

Draws the little blue disc in the corner of the screen.
Call before beginning any disc IO.
================
*/
void Draw_BeginDisc( void )
{
	if (!draw_disc)
		return;

	Draw_CenterPic(draw_disc);
}

/*
================
Draw_EndDisc

Erases the disc icon.
Call after completing any disc IO
================
*/
void Draw_EndDisc( void )
{
}

void ComputeScaledSize( int* wscale, int* hscale, int width, int height )
{
	int scaled_width, scaled_height;

	for (scaled_width = 1; scaled_width < width; scaled_width <<= 1)
		;

	if (gl_round_down.value > 0 && 
		width < scaled_width && 
		(gl_round_down.value == 1 || (scaled_width - width) > (scaled_width >> (int)gl_round_down.value)))
		scaled_width >>= 1;

	for (scaled_height = 1; scaled_height < height; scaled_height <<= 1)
		;

	if (gl_round_down.value > 0 && 
		height < scaled_height && 
		(gl_round_down.value == 1 || (scaled_height - height) > (scaled_height >> (int)gl_round_down.value)))
		scaled_height >>= 1;

	if (wscale)
		*wscale = min(scaled_width >> (int)gl_picmip.value, (int)gl_max_size.value);
	if (hscale)
		*hscale = min(scaled_height >> (int)gl_picmip.value, (int)gl_max_size.value);
}

//====================================================================

/*
================
GL_FindTexture
================
*/
int GL_FindTexture( char* identifier )
{
	int		i;
	gltexture_t* glt;

	for (i = 0, glt = gltextures; i < numgltextures; i++, glt++)
	{
		if (!strcmp(identifier, glt->identifier))
			return glt->texnum;
	}

	return -1;
}

/*
================
GL_ResampleTexture
================
*/
void GL_ResampleTexture( unsigned int* in, int inwidth, int inheight, unsigned int* out, int outwidth, int outheight )
{
	int		i, j;
	unsigned* inrow, * inrow2;
	unsigned	frac, fracstep;
	unsigned	p1[1024], p2[1024];
	byte* pix1, * pix2, * pix3, * pix4;

	fracstep = inwidth * 0x10000 / outwidth;

	frac = fracstep >> 2;
	for (i = 0; i < outwidth; i++)
	{
		p1[i] = 4 * (frac >> 16);
		frac += fracstep;
	}
	frac = 3 * (fracstep >> 2);
	for (i = 0; i < outwidth; i++)
	{
		p2[i] = 4 * (frac >> 16);
		frac += fracstep;
	}

	for (i = 0; i < outheight; i++, out += outwidth)
	{
		inrow = in + inwidth * (int)((i + 0.25) * inheight / outheight);
		inrow2 = in + inwidth * (int)((i + 0.75) * inheight / outheight);

		for (j = 0; j < outwidth; j++)
		{
			pix1 = (byte*)inrow + p1[j];
			pix2 = (byte*)inrow + p2[j];
			pix3 = (byte*)inrow2 + p1[j];
			pix4 = (byte*)inrow2 + p2[j];
			((byte*)(out + j))[0] = (pix1[0] + pix2[0] + pix3[0] + pix4[0]) >> 2;
			((byte*)(out + j))[1] = (pix1[1] + pix2[1] + pix3[1] + pix4[1]) >> 2;
			((byte*)(out + j))[2] = (pix1[2] + pix2[2] + pix3[2] + pix4[2]) >> 2;
			((byte*)(out + j))[3] = (pix1[3] + pix2[3] + pix3[3] + pix4[3]) >> 2;
		}
	}
}

/*
================
GL_ResampleAlphaTexture
================
*/
void GL_ResampleAlphaTexture( byte* in, int inwidth, int inheight, byte* out, int outwidth, int outheight )
{
	int		i, j;
	byte* inrow, * inrow2;
	unsigned	frac, fracstep;
	byte	p1[1024], p2[1024];
	byte* pix1, * pix2, * pix3, * pix4;

	fracstep = inwidth * 0x10000 / outwidth;

	frac = fracstep >> 2;
	for (i = 0; i < outwidth; i++)
	{
		p1[i] = frac >> 16;
		frac += fracstep;
	}
	frac = 3 * (fracstep >> 2);
	for (i = 0; i < outwidth; i++)
	{
		p2[i] = frac >> 16;
		frac += fracstep;
	}

	for (i = 0; i < outheight; i++, out += outwidth)
	{
		inrow = in + inwidth * (int)((i + 0.25) * inheight / outheight);
		inrow2 = in + inwidth * (int)((i + 0.75) * inheight / outheight);

		for (j = 0; j < outwidth; j++)
		{
			pix1 = (byte*)inrow + p1[j];
			pix2 = (byte*)inrow + p2[j];
			pix3 = (byte*)inrow2 + p1[j];
			pix4 = (byte*)inrow2 + p2[j];
			((byte*)(out + j))[0] = (pix1[0] + pix2[0] + pix3[0] + pix4[0]) >> 2;
			((byte*)(out + j))[1] = (pix1[1] + pix2[1] + pix3[1] + pix4[1]) >> 2;
			((byte*)(out + j))[2] = (pix1[2] + pix2[2] + pix3[2] + pix4[2]) >> 2;
			((byte*)(out + j))[3] = (pix1[3] + pix2[3] + pix3[3] + pix4[3]) >> 2;
		}
	}
}

void GL_ResampleTexturePoint( byte* in, int inwidth, int inheight, byte* out, int outwidth, int outheight )
{
	int i, j;
	unsigned ufrac, vfrac;
	unsigned ufracstep, vfracstep;
	byte* src, * dest;

	src = in;
	dest = out;
	ufracstep = inwidth * 0x10000 / outwidth;
	vfracstep = inheight * 0x10000 / outheight;

	vfrac = vfracstep >> 2;

	for (i = 0; i < outheight; i++, out += outwidth)
	{
		ufrac = ufracstep >> 2;

		for (j = 0; j < outwidth; j++)
		{
			*dest = src[ufrac >> 16];
			ufrac += ufracstep;
			dest++;
		}

		vfrac += vfracstep;
		src += inwidth * (vfrac >> 16);
		vfrac = vfrac & 0xFFFF;
	}
}

/*
================
GL_MipMap

Operates in place, quartering the size of the texture
================
*/
void GL_MipMap( byte* in, int width, int height )
{
	int		i, j;
	byte* out;

	width <<= 2;
	height >>= 1;
	out = in;
	for (i = 0; i < height; i++, in += width)
	{
		for (j = 0; j < width; j += 8, out += 4, in += 8)
		{
			out[0] = (in[0] + in[4] + in[width + 0] + in[width + 4]) >> 2;
			out[1] = (in[1] + in[5] + in[width + 1] + in[width + 5]) >> 2;
			out[2] = (in[2] + in[6] + in[width + 2] + in[width + 6]) >> 2;
			out[3] = (in[3] + in[7] + in[width + 3] + in[width + 7]) >> 2;
		}
	}
}

void BoxFilter3x3( byte* out, byte* in, int w, int h, int x, int y )
{
	int		i, j;
	int		a = 0, r = 0, g = 0, b = 0;
	int		count = 0, acount = 0;
	int		u, v;
	byte* pixel;

	for (i = 0; i < 3; i++)
	{
		u = (i - 1) + x;

		for (j = 0; j < 3; j++)
		{
			v = (j - 1) + y;

			if (u >= 0 && u < w && v >= 0 && v < h)
			{
				pixel = &in[(u + v * w) * 4];

				if (pixel[3] != 0)
				{
					r += pixel[0];
					g += pixel[1];
					b += pixel[2];
					a += pixel[3];
					acount++;
				}
			}
		}
	}

	if (acount == 0)
		acount = 1;

	out[0] = r / acount;
	out[1] = g / acount;
	out[2] = b / acount;
	out[3] = 0;
}

int giTotalTextures;
int giTotalTexBytes;

void GL_Upload32( unsigned int* data, int width, int height, qboolean mipmap, int iType )
{
	int			iComponent = 0, iFormat = 0;
	int			i;

	static	unsigned	scaled[1024 * 512];	// [512*256];
	int			scaled_width, scaled_height;
	qboolean	f4444 = FALSE;

	if (vid_d3d.value && TEX_IS_ALPHA(iType))
		f4444 = TRUE;

	giTotalTexBytes += height * width;

	if (iType != TEX_TYPE_LUM)
		giTotalTexBytes += width * height * 2;

	giTotalTextures++;

	if (gl_spriteblend.value && TEX_IS_ALPHA(iType))
	{
		for (i = 0; i < width * height; i++)
		{
			if (!data[i])
				BoxFilter3x3((byte*)&data[i], (byte*)data, width, height, i % width, i / width);
		}
	}

	ComputeScaledSize(&scaled_width, &scaled_height, width, height);

	if (scaled_width * scaled_height > sizeof(scaled) / 4)
		Sys_Error("GL_LoadTexture: too big");

	switch (iType)
	{
		case TEX_TYPE_NONE:
			iFormat = GL_RGBA;
			iComponent = 3;
			break;
		case TEX_TYPE_ALPHA:
		case TEX_TYPE_ALPHA_GRADIENT:
			iFormat = GL_RGBA;
			iComponent = 4;
			break;
		case TEX_TYPE_LUM:
			iFormat = GL_LUMINANCE;
			iComponent = 1;
			break;
		case TEX_TYPE_RGBA:
			iFormat = GL_RGBA;
			iComponent = 4;
			break;
	}

	if (scaled_width == width && scaled_height == height)
	{
		if (!mipmap)
		{
			qglTexImage2D(GL_TEXTURE_2D, GL_ZERO, iComponent, scaled_width, scaled_height, GL_ZERO, iFormat, GL_UNSIGNED_BYTE, data);
			goto done;
		}

		if (iType == TEX_TYPE_LUM)
			memcpy(scaled, data, width * height);
		else
			memcpy(scaled, data, width * height * 4);			
	}
	else
	{
		if (iType == TEX_TYPE_LUM)
			GL_ResampleAlphaTexture((byte*)data, width, height, (byte*)scaled, scaled_width, scaled_height);
		else
			GL_ResampleTexture(data, width, height, scaled, scaled_width, scaled_height);
	}

	texels += scaled_width * scaled_height;

	if (f4444)
	{
		Download4444();
	}

	qglTexImage2D(GL_TEXTURE_2D, GL_ZERO, iComponent, scaled_width, scaled_height, GL_ZERO, iFormat, GL_UNSIGNED_BYTE, scaled);
	
	if (mipmap)
	{
		int		miplevel;

		miplevel = 0;
		while (scaled_width > 1 || scaled_height > 1)
		{
			GL_MipMap((byte*)scaled, scaled_width, scaled_height);
			scaled_width >>= 1;
			scaled_height >>= 1;
			if (scaled_width < 1)
				scaled_width = 1;
			if (scaled_height < 1)
				scaled_height = 1;

			texels += scaled_width * scaled_height;

			if (f4444)
			{
				Download4444();
			}

			miplevel++;
			qglTexImage2D(GL_TEXTURE_2D, miplevel, iComponent, scaled_width, scaled_height, GL_ZERO, iFormat, GL_UNSIGNED_BYTE, scaled);
		}
	}

done:
	if (mipmap)
	{
		qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter_min);
		qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter_max);
	}
	else
	{
		qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter_max);
		qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter_max);
	}
}

/*
===============
GL_Upload16
===============
*/
void GL_Upload16( unsigned char* data, int width, int height, qboolean mipmap, int iType, unsigned char* pPal )
{
	static	unsigned	trans[640 * 480];		// FIXME, temporary
	int			i, s;
	qboolean noalpha = TRUE;
	int			p;
	unsigned char* pb;

	s = width * height;

	if (iType != TEX_TYPE_LUM)
	{
		if (!pPal)
			return;

		for (i = 0; i < 768; i++)
			pPal[i] = texgammatable[pPal[i]];
	}

	// Alpha textures
	if (TEX_IS_ALPHA(iType))
	{
		if (iType == TEX_TYPE_ALPHA_GRADIENT)
		{
			for (i = 0; i < s; i++)
			{
				p = data[i];
				pb = (byte*)&trans[i];
				pb[0] = pPal[765];
				pb[1] = pPal[766];
				pb[2] = pPal[767];
				pb[3] = p;
				noalpha = FALSE;
			}
		}
		else if (iType == TEX_TYPE_RGBA)
		{
			for (i = 0; i < s; i++)
			{
				p = data[i];
				pb = (byte*)&trans[i];
				pb[0] = pPal[p * 3];
				pb[1] = pPal[p * 3 + 1];
				pb[2] = pPal[p * 3 + 2];
				pb[3] = p;
				noalpha = FALSE;
			}
		}
		else if (iType == TEX_TYPE_ALPHA)
		{
			for (i = 0; i < s; i++)
			{
				p = data[i];
				pb = (byte*)&trans[i];

				if (p == 255)
				{
					pb[0] = 0;
					pb[1] = 0;
					pb[2] = 0;
					pb[3] = 0;
					noalpha = FALSE;
				}
				else
				{
					pb[0] = pPal[p * 3];
					pb[1] = pPal[p * 3 + 1];
					pb[2] = pPal[p * 3 + 2];
					pb[3] = 255;
				}
			}
		}

		if (noalpha)
			iType = TEX_TYPE_NONE;
	}
	else if (iType == TEX_TYPE_NONE)
	{
		unsigned char r, g, b, * ppix;

		if (s & 3)
			Sys_Error("GL_Upload16: s&3");

		if (gl_dither.value)
		{
			for (i = 0; i < s; i++)
			{
				p = data[i];
				pb = (byte*)&trans[i];
				ppix = &pPal[p * 3];
				r = ppix[0];
				g = ppix[1];
				b = ppix[2];

				if (r < 252)
					r += RandomLong(0, 3);
				if (g < 252)
					g += RandomLong(0, 3);
				if (b < 252)
					b += RandomLong(0, 3);

				pb[0] = r;
				pb[1] = g;
				pb[2] = b;
				pb[3] = 255;
			}
		}
		else
		{
			for (i = 0; i < s; i++)
			{
				p = data[i];
				pb = (byte*)&trans[i];
				ppix = &pPal[p * 3];
				r = ppix[0] |= (ppix[0] >> 6);
				g = ppix[1] |= (ppix[1] >> 6);
				b = ppix[2] |= (ppix[2] >> 6);

				pb[0] = r;
				pb[1] = g;
				pb[2] = b;
				pb[3] = 255;
			}
		}
	}
	else if (iType == TEX_TYPE_LUM)
	{
		memcpy(trans, data, s);
	}
	else
	{
		Con_Printf("Upload16:Bogus texture type!/n");
	}

	GL_Upload32(trans, width, height, mipmap, iType);
}

/*
================
GL_UnloadTextures

Unload all loaded textures
We do this every time we load the map
================
*/
void GL_UnloadTextures( void )
{
	int i, texnum;
	gltexture_t* glt;

	for (i = 0, glt = gltextures; i < numgltextures; i++, glt++)
	{
		if (glt->servercount > 0 && glt->servercount != gHostSpawnCount)
		{
			texnum = glt->texnum;
			qglDeleteTextures(1, (const GLuint*)glt);

			memset(glt, 0, sizeof(gltexture_t));
			glt->servercount = -1;
			glt->texnum = texnum;
		}
	}
}

void GL_PaletteInit( void )
{
	int i;

	for (i = 0; i < 350; i++)
	{
		gGLPalette[i].tag = -1;
	}
}

int GL_PaletteTag( byte* pPal )
{
	int tag;
	int i;

	tag = *pPal;

	for (i = 0; i < 768; i++)
	{
		tag = (tag + pPal[1]) ^ *pPal;
		pPal++;
	}

	if (tag < 0)
		tag = -tag;

	return tag;
}

int GL_PaletteEqual( byte* pPal1, int tag1, byte* pPal2, int tag2 )
{
	int i;

	if (tag1 != tag2)
		return FALSE;

	for (i = 0; i < 768; i++)
	{
		if (pPal1[i] != pPal2[i])
			return FALSE;
	}

	return TRUE;
}

void GL_PaletteClearSky( void )
{
	int i;

	for (i = 344; i < 350; i++)
	{
		gGLPalette[i].tag = -1;
	}
}

/*
===============
GL_PaletteAdd
===============
*/
short GL_PaletteAdd( unsigned char* pPal, qboolean isSky )
{
	int i, tag;
	int limit;

	if (!qglColorTableEXT)
		return -1;

	i = 0;
	limit = 350;

	tag = GL_PaletteTag(pPal);

	if (isSky)
		i = 344;
	else
		limit = 344;

	for (; i < limit; i++)
	{
		if (gGLPalette[i].tag < 0)
		{
			memcpy(gGLPalette[i].colors, pPal, sizeof(gGLPalette[i].colors));
			gGLPalette[i].tag = tag;
			return i;
		}

		if (GL_PaletteEqual(pPal, tag, gGLPalette[i].colors, gGLPalette[i].tag))
		{
			return i;
		}
	}
	
	return -1;
}

int g_currentpalette = -1;

void GL_PaletteSelect( int paletteIndex )
{
	if (g_currentpalette == paletteIndex)
		return;

	if (qglColorTableEXT)
	{
		g_currentpalette = paletteIndex;
		qglColorTableEXT(GL_SHARED_TEXTURE_PALETTE_EXT, GL_RGB, 256, GL_RGB, GL_UNSIGNED_BYTE, gGLPalette[paletteIndex].colors);
	}
}

/*
================
GL_LoadTexture
================
*/
int GL_LoadTexture( char* identifier, GL_TEXTURETYPE textureType, int width, int height, unsigned char* data, int mipmap, int iType, unsigned char* pPal )
{
	int			i;
	int			scaled_width, scaled_height;
	gltexture_t* glt, * slot;
	BOOL		mustRescale;

	glt = NULL;

tryagain:
	// see if the texture is allready present
	if (identifier[0])
	{
		for (i = 0; i < numgltextures; i++)
		{
			slot = &gltextures[i];
			if (slot->servercount < 0)
			{
				if (!glt)
				{
					glt = slot;
				}
				continue;
			}

			if (!strcmp(identifier, slot->identifier))
			{
				if ((slot->width == width) && (slot->height == height))
				{
					if (slot->servercount > 0)
						slot->servercount = gHostSpawnCount;

					if (slot->paletteIndex >= 0)
						return slot->texnum | ((slot->paletteIndex + 1) << 16);
					else
						return slot->texnum;
				}

				identifier[3]++;
				goto tryagain;	// check again
			}
		}
	}
	else
	{
		Con_DPrintf("NULL Texture\n");
	}

	if (!glt)
	{
		glt = &gltextures[numgltextures];
		numgltextures++;

		if (numgltextures >= MAX_GLTEXTURES)
			Sys_Error("Texture Overflow: MAX_GLTEXTURES");

		glt->texnum = texture_extension_number;
		texture_extension_number++;
	}

	strcpy(glt->identifier, identifier);
	glt->width = width;
	glt->height = height;
	glt->mipmap = mipmap;

	if (textureType == GLT_WORLD)
		glt->servercount = gHostSpawnCount;
	else
		glt->servercount = 0;

	glt->paletteIndex = -1;

	GL_Bind(glt->texnum);

	ComputeScaledSize(&scaled_width, &scaled_height, width, height);

	if (scaled_width == width && scaled_height == height)
		mustRescale = FALSE;
	else
		mustRescale = TRUE;

	if (!mipmap)
	{
		unsigned char* pTexture = NULL;
		byte scaled[128 * 128];

		if (mustRescale)
		{
			if (scaled_width <= 128 && scaled_height <= 128)
			{
				GL_ResampleTexturePoint(data, width, height, scaled, scaled_width, scaled_height);
				pTexture = scaled;
			}
		}
		else
		{
			pTexture = data;
		}

		if (pTexture && qglColorTableEXT)
		{
			if (gl_palette_tex.value && (iType == TEX_TYPE_NONE))
			{
				glt->paletteIndex = GL_PaletteAdd(pPal, FALSE);
				if (glt->paletteIndex >= 0)
				{
					qglTexImage2D(GL_TEXTURE_2D, 0, GL_COLOR_INDEX8_EXT, scaled_width, scaled_height, GL_FALSE, GL_COLOR_INDEX, GL_UNSIGNED_BYTE, pTexture);

					qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter_max);
					qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter_max);

					if (glt->paletteIndex < 0)
						return glt->texnum;
					else
						return glt->texnum | ((glt->paletteIndex + 1) << 16);
				}
			}
		}
	}

	GL_Upload16(data, width, height, mipmap, iType, pPal);

	if (glt->paletteIndex < 0)
		return glt->texnum;
	else
		return glt->texnum | ((glt->paletteIndex + 1) << 16);
}

/*
================
GL_LoadPicTexture
================
*/
int GL_LoadPicTexture( qpic_t* pic, char* pszName )
{
	unsigned char* pPal;

	pPal = &pic->data[pic->width * pic->height + 2];

	return GL_LoadTexture(pszName, GLT_SYSTEM, pic->width, pic->height, pic->data, FALSE, TEX_TYPE_ALPHA, pPal);
}

qpic_t* LoadTransBMP( char* pszName )
{
	return LoadTransPic(pszName, (qpic_t*)W_GetLumpName(pszName));
}

qpic_t* LoadTransPic( char* pszName, qpic_t* ppic )
{
	int		i, width, height;
	gltexture_t* glt;
	int* pbuf;
	byte* pPal;
	glpic_t* gl;
	qpic_t* ppicNew;

	if (!ppic)
		return NULL;

	ppicNew = (qpic_t*)malloc(sizeof(qpic_t) + sizeof(glpic_t));
	gl = (glpic_t*)ppicNew->data;

	ppicNew->width = ppic->width;
	ppicNew->height = ppic->height;

tryagain:
	// see if the texture is allready present
	if (pszName[0])
	{
		for (i = 0, glt = gltextures; i < numgltextures; i++, glt++)
		{
			if (!strcmp(pszName, glt->identifier))
			{
				if (glt->width == ppic->width && glt->height == ppic->height)
					return ppic;

				pszName[3]++;
				goto tryagain;	// check again
			}
		}
	}
	else
	{
		glt = &gltextures[numgltextures];
	}

	numgltextures++;

	strcpy(glt->identifier, pszName);
	glt->texnum = texture_extension_number;
	glt->width = ppic->width;
	glt->height = ppic->height;
	glt->mipmap = FALSE;

	GL_Bind(glt->texnum);

	pbuf = (int*)malloc(ppic->width * ppic->height * sizeof(int));

	pPal = &ppic->data[ppic->width * ppic->height + 2];

	for (i = 0; i < 768; i++)
	{
		pPal[i] = texgammatable[pPal[i]];
	}

	width = ppic->width;
	height = ppic->height;

	for (i = 0; i < width * height; i++)
	{
		byte b = ppic->data[i];
		pbuf[i] = *(uint32*)&pPal[b * 3] & 0xFFFFFF;
		
		if (b != 0xFF)
		{
			pbuf[i] |= 0xFF000000;
		}
	}

	GL_Upload32((unsigned int*)pbuf, width, height, FALSE, TEX_TYPE_ALPHA);

	gl->texnum = texture_extension_number;
	texture_extension_number++;
	gl->sl = 0;
	gl->sh = 1;
	gl->tl = 0;
	gl->th = 1;

	free(pbuf);

	return ppicNew;
}

/*
===============
Draw_MiptexTexture

===============
*/
void Draw_MiptexTexture( cachewad_t* wad, byte* data )
{	
	texture_t* tex;
	miptex_t* mip, tmp;
	int			i, pix, paloffset, palettesize;
	byte* pal, * bitmap;

	if (wad->cacheExtra != MIP_EXTRASIZE)
		Sys_Error("Draw_MiptexTexture: Bad cached wad %s\n", wad->name);

	tex = (texture_t*)data;
	mip = (miptex_t*)(data + wad->cacheExtra);
	tmp = *mip;
	strcpy(tex->name, tmp.name);

	tex->width = LittleLong(tmp.width);
	tex->height = LittleLong(tmp.height);
	tex->anim_min = 0;
	tex->anim_max = 0;
	tex->anim_total = 0;
	tex->alternate_anims = NULL;
	tex->anim_next = NULL;

	for (i = 0; i < MIPLEVELS; i++)
		tex->offsets[i] = LittleLong(tmp.offsets[i]) + wad->cacheExtra;

	pix = tex->width * tex->height;
	palettesize = tex->offsets[0];
	paloffset = palettesize + pix + (pix >> 2) + (pix >> 4) + (pix >> 6) + 2;
	pal = (byte*)tex + paloffset;
	bitmap = (byte*)tex + palettesize;

	if (pal[765] != 0 || pal[766] != 0 || pal[767] != 255)
	{
		tex->name[0] = '}';
		tex->gl_texturenum = GL_LoadTexture(tex->name, GLT_DECAL, tex->width, tex->height, bitmap, TRUE, TEX_TYPE_ALPHA_GRADIENT, pal);
	}
	else
	{
		tex->name[0] = '{';
		tex->gl_texturenum = GL_LoadTexture(tex->name, GLT_DECAL, tex->width, tex->height, bitmap, TRUE, TEX_TYPE_ALPHA, pal);
	}
}

void Draw_CacheWadInit( char* name, int cacheMax, cachewad_t* wad )
{
	int		h[3];
	int		nFileSize;
	lumpinfo_t* lump_p;
	wadinfo_t header;
	int		i;

	nFileSize = COM_OpenFile(name, h);
	if (h[2] == -1)
		Sys_Error("Draw_LoadWad: Couldn't open %s\n", name);

	Sys_FileRead(h[2], &header, sizeof(header));

	if (header.identification[0] != 'W'
	  || header.identification[1] != 'A'
	  || header.identification[2] != 'D'
	  || header.identification[3] != '3')
	{
		Sys_Error("Wad file %s doesn't have WAD3 id\n", name);
	}

	wad->lumps = (lumpinfo_t*)malloc(nFileSize - header.infotableofs);

	COM_FileSeek(h[0], h[1], h[2], header.infotableofs);
	Sys_FileRead(h[2], wad->lumps, nFileSize - header.infotableofs);
	COM_CloseFile(h[0], h[1], h[2]);

	for (i = 0, lump_p = wad->lumps; i < header.numlumps; i++, lump_p++)
	{
		W_CleanupName(lump_p->name, lump_p->name);
	}

	wad->name = name;
	wad->lumpCount = header.numlumps;
	wad->cacheCount = 0;
	wad->cacheMax = cacheMax;
	wad->cache = (cacheentry_t*)malloc(sizeof(cacheentry_t) * cacheMax);
	memset(wad->cache, 0, sizeof(cacheentry_t) * cacheMax);
	wad->cacheExtra = 0;
	wad->pfnCacheBuild = NULL;

	wad->tempWad = FALSE;
}

void Draw_CacheWadHandler( cachewad_t* wad, PFNCACHE fn, int extraDataSize )
{
	wad->cacheExtra = extraDataSize;
	wad->pfnCacheBuild = fn;
}

void Draw_DecalSetName( int decal, char* name )
{
	if (decal >= MAX_BASE_DECALS)
		return;

	strncpy(decal_names[decal], name, sizeof(decal_names[0]) - 1);
	decal_names[decal][sizeof(decal_names[0]) - 1] = 0;
}

int Draw_DecalIndex( int id )
{
	char* pName;

	pName = decal_names[id];
	if (!pName[0])
		Sys_Error("Used decal #%d without no name\n", id);

	return Draw_CacheIndex(&decal_wad, pName);
}

int Draw_CacheIndex( cachewad_t* wad, char* path )
{
	cacheentry_t* pic;
	int i;

	for (i = 0, pic = wad->cache; i < wad->cacheCount; i++, pic++)
	{
		if (!strcmp(path, pic->name))
			break;
	}

	if (i == wad->cacheCount)
	{
		if (wad->cacheCount == wad->cacheMax)
			Sys_Error("Cache wad (%s) out of %d entries", wad->name, wad->cacheMax);
		wad->cacheCount++;
		strcpy(pic->name, path);
	}
	return i;
}

int Draw_DecalCount( void )
{
	return decal_wad.lumpCount;
}

int Draw_DecalSize( int number )
{
	if (number >= decal_wad.lumpCount)
		return 0;

	return decal_wad.lumps[number].size;
}

char* Draw_DecalName( int number )
{
	if (number >= decal_wad.lumpCount)
		return 0;
	
	return decal_wad.lumps[number].name;
}

texture_t* Draw_DecalTexture( int index )
{
	int		playernum;
	customization_t* pCust;

	// Just a regular decal
	if (index >= 0)
		return (texture_t*)Draw_CacheGet(&decal_wad, index);

	// Player decal
	playernum = ~index;
	pCust = cl.players[playernum].customdata.pNext;
	if (pCust && pCust->bInUse)
	{
		cachewad_t* pWad;

		pWad = (cachewad_t*)pCust->pInfo;
		if (pWad && pCust->pBuffer)
			return (texture_t*)Draw_CustomCacheGet(pWad, pCust->pBuffer, pCust->nUserData1);
	}

	Sys_Error("Failed to load custom decal for player #%i:%s using default decal 0.\n", playernum, cl.players[playernum].name);
	return NULL;
}

// called from cl_parse.c
// find the server side decal id given it's name.
// used for save/restore
int Draw_DecalIndexFromName( char* name )
{
	char tmpName[16];
	int i;

	strcpy(tmpName, name);

	if (tmpName[0] == '}')
		tmpName[0] = '{';

	for (i = 0; i < MAX_BASE_DECALS; i++)
	{
		if (decal_names[i][0] && !strcmp(tmpName, decal_names[i]))
			return i;
	}

	return 0;
}

qboolean Draw_CacheReload( cachewad_t* wad, lumpinfo_t* pLump, cacheentry_t* pic, char* clean, char* path )
{
	byte* buf;
	int		h[3];

	COM_OpenFile(wad->name, h);
	if (h[2] == -1)
		return FALSE;

	if (wad->tempWad)
	{
		buf = (byte*)Hunk_TempAlloc(pLump->size + wad->cacheExtra + 1);
		pic->cache.data = buf;
	}
	else
	{
		buf = (byte*)Cache_Alloc(&pic->cache, pLump->size + wad->cacheExtra + 1, clean);
	}

	if (!buf)
		Sys_Error("Draw_CacheGet: not enough space for %s in %s", path, wad->name);

	buf[pLump->size + wad->cacheExtra] = 0;

	COM_FileSeek(h[0], h[1], h[2], pLump->filepos);
	Sys_FileRead(h[2], &buf[wad->cacheExtra], pLump->size);
	COM_CloseFile(h[0], h[1], h[2]);

	if (wad->pfnCacheBuild)
		wad->pfnCacheBuild(wad, buf);

	return TRUE;
}

qboolean Draw_CacheLoadFromCustom( char* clean, cachewad_t* wad, void* raw, cacheentry_t* pic )
{
	int		idx;
	byte* buf;
	lumpinfo_t* pLump;

	idx = atoi(clean);
	if (idx < 0 || idx >= wad->lumpCount)
		return FALSE;

	pLump = &wad->lumps[idx];
	buf = (byte*)Cache_Alloc(&pic->cache, wad->cacheExtra + pLump->size + 1, clean);
	if (!buf)
		Sys_Error("Draw_CacheGet: not enough space for %s in %s", clean, wad->name);

	buf[pLump->size + wad->cacheExtra] = 0;

	memcpy(&buf[wad->cacheExtra], (char*)raw + pLump->filepos, pLump->size);

	if (wad->pfnCacheBuild)
		wad->pfnCacheBuild(wad, buf);

	return TRUE;
}

void* Draw_CacheGet( cachewad_t* wad, int index )
{
	cacheentry_t* pic;
	int i;
	void* dat = NULL;

	if (index >= wad->cacheCount)
		Sys_Error("Cache wad indexed before load %s: %d", wad->name, index);

	pic = &wad->cache[index];
	if (wad->tempWad || (dat = Cache_Check(&pic->cache)) == NULL)
	{
		char name[16];
		char clean[16];
		lumpinfo_t* pLump;

		COM_FileBase(pic->name, name);
		W_CleanupName(name, clean);

		for (i = 0, pLump = wad->lumps; i < wad->lumpCount; i++, pLump++)
		{
			if (!strcmp(clean, pLump->name))
				break;
		}

		if (i >= wad->lumpCount)
			return NULL;

		if (!Draw_CacheReload(wad, pLump, pic, clean, pic->name))
			return NULL;

		dat = pic->cache.data;
		if (!dat)
			Sys_Error("Draw_CacheGet: failed to load %s", pic->name);
	}

	return dat;
}

void* Draw_CustomCacheGet( cachewad_t* wad, void* raw, int index )
{
	cacheentry_t* pic;
	void* dat;

	if (index >= wad->cacheCount)
		Sys_Error("Cache wad indexed before load %s: %d", wad->name, index);

	pic = &wad->cache[index];
	dat = Cache_Check(&pic->cache);
	if (dat == NULL)
	{
		char name[16];
		char clean[16];
		COM_FileBase(pic->name, name);
		W_CleanupName(name, clean);

		if (!Draw_CacheLoadFromCustom(clean, wad, raw, pic))
			return NULL;

		dat = pic->cache.data;
		if (!dat)
			Sys_Error("Draw_CacheGet: failed to load %s", pic->name);
	}

	return dat;
}

void CustomDecal_Init( cachewad_t* wad, void* raw, int nFileSize )
{
	int i;

	Draw_CustomCacheWadInit(16, wad, raw, nFileSize);
	Draw_CacheWadHandler(wad, Draw_MiptexTexture, MIP_EXTRASIZE);

	for (i = 0; i < wad->lumpCount; i++)
	{
		Draw_CacheByIndex(wad, i);
	}
}

void Draw_CustomCacheWadInit( int cacheMax, cachewad_t* wad, void* raw, int nFileSize )
{
	lumpinfo_t* lump_p;
	wadinfo_t header;
	int		i;

	header = *(wadinfo_t*)raw;

	if (header.identification[0] != 'W'
	  || header.identification[1] != 'A'
	  || header.identification[2] != 'D'
	  || header.identification[3] != '3')
	{
		Sys_Error("Custom file doesn't have WAD3 id\n");
	}

	wad->lumps = (lumpinfo_t*)malloc(nFileSize - header.infotableofs);
	memcpy(wad->lumps, (char*)raw + header.infotableofs, nFileSize - header.infotableofs);

	for (i = 0, lump_p = wad->lumps; i < header.numlumps; i++, lump_p++)
	{
		W_CleanupName(lump_p->name, lump_p->name);
	}

	wad->name = "pldecal.wad";
	wad->lumpCount = header.numlumps;
	wad->cacheCount = 0;
	wad->cacheMax = cacheMax;
	wad->cache = (cacheentry_t*)malloc(sizeof(cacheentry_t) * cacheMax);
	memset(wad->cache, 0, sizeof(cacheentry_t) * cacheMax);
	wad->pfnCacheBuild = NULL;
	wad->cacheExtra = 0;
}

int Draw_CacheByIndex( cachewad_t* wad, int nIndex )
{
	cacheentry_t* pic;
	int i;

	for (i = 0, pic = wad->cache; i < wad->cacheCount; i++, pic++)
	{
		if (atoi(pic->name) == nIndex)
			break;
	}

	if (i == wad->cacheCount)
	{
		if (wad->cacheCount == wad->cacheMax)
			Sys_Error("Cache wad (%s) out of %d entries", wad->name, wad->cacheMax);

		wad->cacheCount++;
		sprintf(pic->name, "%i", nIndex);
	}

	return i;
}