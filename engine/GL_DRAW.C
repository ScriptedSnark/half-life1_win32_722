// draw.c -- this is the only file outside the refresh that touches the
// vid buffer

#include "quakedef.h"
#include "winquake.h"
#include "decal.h"
#include "opengl2d3d.h"


cvar_t		gl_nobind = { "gl_nobind", "0" };
cvar_t		gl_max_size = { "gl_max_size", "256" };
cvar_t		gl_round_down = { "gl_round_down", "3" };
cvar_t		gl_picmip = { "gl_picmip", "0" };
cvar_t		gl_palette_tex = { "gl_palette_tex", "1" };

qfont_t* draw_chars;
qfont_t* draw_creditsfont;
qpic_t* draw_disc;

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

//	if (paletteIndex >= 0) TODO: Implement
//		GL_PaletteSelect(paletteIndex);
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
	// TODO: Implement
	return NULL;
}

qpic_t* Draw_CachePic( char* path )
{
	qpic_t* ret;
	int idx;

	idx = Draw_CacheIndex(&menu_wad, path);
	ret = (qpic_t*)Draw_CacheGet(&menu_wad, idx);

	return ret;
}

int __cdecl Draw_StringLen( char *psz )
{
	// TODO: Refactor

	int result; // eax
	char *v2; // esi
	char v3; // cl

	result = 0;
	v2 = psz;
	if (psz && *psz)
	{
		do
		{
			v3 = v2[1];
			result += draw_chars->fontinfo[*v2++].charwidth;
		} while (v3);
	}
	return result;
}











// This is called to reset all loaded decals
// called from cl_parse.c and host.c
void Decal_Init( void )
{
	// TODO: Implement
}

/*
===============
Draw_Init
===============
*/
void Draw_Init( void )
{
	int i;
	qpic_t* cb;
	glpic_t* gl;

	Draw_CacheWadInit("cached.wad", 16, &menu_wad);
	menu_wad.tempWad = TRUE;

	// TODO: Implement

	Cvar_RegisterVariable(&gl_nobind);
	Cvar_RegisterVariable(&gl_max_size);
	Cvar_RegisterVariable(&gl_round_down);
	Cvar_RegisterVariable(&gl_picmip);
	Cvar_RegisterVariable(&gl_palette_tex);

	// TODO: Implement

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
	gl->texnum = GL_LoadTexture("conback", GLT_SYSTEM, cb->width, cb->height, cb->data, FALSE, TEX_TYPE_NONE, &cb->data[cb->width * cb->height + 2]);
	gl->sl = 0;
	gl->sh = 1;
	gl->tl = 0;
	gl->th = 1;

	// TODO: Implement

	float prev = gl_round_down.value;
	gl_round_down.value = 0.0;

	// now turn them into textures
	char_texture = GL_LoadTexture("conchars", GLT_SYSTEM, 256, draw_chars->height, draw_chars->data, FALSE, 1, &draw_chars->data[draw_chars->height * 256 + 2]);
	
	// TODO: Implement

	gl_round_down.value = prev;

	chars_xsize = 1.0 / 256;
	chars_ysize = 1.0 / draw_chars->height;

	// TODO: Implement

	// save a texture slot for translated picture
	translate_texture = texture_extension_number++;

	// TODO: Implement
}



/*
================
Draw_Character

Draws a single character
================
*/
int Draw_Character( int x, int y, int num )
{
	int rowheight; // ebx
	int width; // esi
	GLfloat v6; // ecx
	GLfloat startoffset; // [esp+5Ch] [ebp-20h]
	GLfloat v8; // [esp+5Ch] [ebp-20h]
	GLfloat v9; // [esp+5Ch] [ebp-20h]
	GLfloat v10; // [esp+5Ch] [ebp-20h]
	GLfloat v11; // [esp+5Ch] [ebp-20h]
	float v12; // [esp+5Ch] [ebp-20h]
	float v13; // [esp+5Ch] [ebp-20h]
	float v14; // [esp+5Ch] [ebp-20h]
	GLfloat xsize; // [esp+60h] [ebp-1Ch]
	GLfloat wtf; // [esp+60h] [ebp-1Ch]
	float v17; // [esp+64h] [ebp-18h]
	GLfloat v18; // [esp+64h] [ebp-18h]
	GLfloat frow; // [esp+68h] [ebp-14h]
	GLfloat fcol; // [esp+6Ch] [ebp-10h]
	GLfloat v21; // [esp+70h] [ebp-Ch]
	GLfloat v22; // [esp+74h] [ebp-8h]
	GLfloat v23; // [esp+78h] [ebp-4h]

	rowheight = draw_chars->rowheight;
	if (-rowheight >= y)
		return 0;

	width = draw_chars->fontinfo[num].charwidth;  // Do before that num &= 255
	if (y < 0 || num == 32)
		return draw_chars->fontinfo[num].charwidth;

	startoffset = (float)(unsigned __int8)draw_chars->fontinfo[num].startoffset;
	fcol = startoffset * chars_xsize;
	v8 = (float)((int)(draw_chars->fontinfo[num].startoffset & ~0xFFu) >> 8);
	frow = v8 * chars_ysize;
	v9 = (float)rowheight;
	v17 = v9 * chars_ysize;
	v10 = (float)width;
	xsize = v10 * chars_xsize;

	qglTexEnvf( 0x2300u, 0x2200u, 8448.0 );
	qglEnable( 0xBC0u );
	qglColor4f( 1.0, 1.0, 1.0, 1.0 );
	GL_DisableMultitexture();

	GL_Bind( char_texture );

	qglBegin( 7 );
	qglTexCoord2f( fcol, frow );
	v11 = (float)y;
	v6 = v11;
	v12 = (float)x;
	v21 = v6;
	v23 = v12;
	qglVertex2f( v12, v6 );

	wtf = xsize + fcol;
	qglTexCoord2f( wtf, frow );
	v13 = (float)(width + x);
	v22 = v13;
	qglVertex2f( v13, v21 );

	v18 = v17 + frow;
	qglTexCoord2f( wtf, v18 );
	v14 = (float)(rowheight + y);
	qglVertex2f( v22, v14 );

	qglTexCoord2f( fcol, v18 );
	qglVertex2f( v23, v14 );

	qglEnd();

	qglDisable( 0xBE2 );

	return width;
}

/*
================
Draw_String
================
*/
int Draw_String( int x, int y, char* str )
{
	unsigned __int8 *v3; // ebx
	int v4; // esi
	unsigned __int8 v6; // [esp-4h] [ebp-10h]

	v3 = (unsigned __int8 *)str;
	v4 = x;
	if (!*str)
		return x;
	do
	{
		v6 = *v3++;
		v4 += Draw_Character( v4, y, v6 );
	} while (*v3);
	return v4;
}




/*
=============
Draw_Pic
=============
*/
void Draw_Pic( int x, int y, qpic_t *pic )
{
	// TODO: Implement
}




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






void Draw_ConsoleBackground( int lines )
{
	// TODO: Refactor

	int v1; // eax
	int v2; // esi
	int v3; // esi
	char ver[100]; // [esp+4h] [ebp-64h] BYREF

	Draw_Pic2( 0, lines - glheight, glwidth, glheight + 1, conback );
	v1 = build_number();
	sprintf( ver, "Half-Life 1.0 (build %d)", v1 );
	v2 = vid.conwidth;
	v3 = v2 - Draw_StringLen( ver );
	if (con_loading == FALSE && (giSubState & 4) == 0)
		Draw_String( v3, 0, ver );
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
	// TODO: Implement
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

	qglColor4f(0, 0, 0, 0.8f);

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


// TODO: Implement





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
	// TODO: Copy-pasted from 8684

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
	// TODO: Copy-pasted from 8684

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
	// TODO: Copy-pasted from 8684

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
			if (f4444)
			{
				Download4444();
			}

			{
				// JAY: No paletted textures for now
				qglTexImage2D(GL_TEXTURE_2D, GL_ZERO, iComponent, scaled_width, scaled_height, GL_ZERO, iFormat, GL_UNSIGNED_BYTE, data);
			}
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
		int	miplevel = 0;

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

Alpha textures require additional methods
===============
*/
void GL_Upload16( unsigned char* data, int width, int height, qboolean mipmap, int iType, unsigned char* pPal )
{
	// TODO: Copy-pasted from 8684

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

				pb[0] = pPal[p * 3 + 0];
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
					pb[0] = pPal[p * 3 + 0];
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
		if (s & 3)
			Sys_Error("GL_Upload16: s&3");

		if (gl_dither.value)
		{
			for (i = 0; i < s; i++)
			{
				unsigned char r, g, b;
				unsigned char* ppix;

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
		else
		{
			for (i = 0; i < s; i += 4)
			{
				trans[i + 0] = *(unsigned int*)&pPal[3 * data[i + 0]] | 0xFF000000;
				trans[i + 1] = *(unsigned int*)&pPal[3 * data[i + 1]] | 0xFF000000;
				trans[i + 2] = *(unsigned int*)&pPal[3 * data[i + 2]] | 0xFF000000;
				trans[i + 3] = *(unsigned int*)&pPal[3 * data[i + 3]] | 0xFF000000;
			}
		}
	}
	else if (iType == TEX_TYPE_LUM)
	{
		Q_memcpy(trans, data, s);
	}
	else
	{
		Con_Printf("Upload16:Bogus texture type!/n");
	}

	GL_Upload32(trans, width, height, mipmap, iType);
}









// TODO: Implement


/*
===============
GL_PaletteAdd
===============
*/
short GL_PaletteAdd( unsigned char* pPal, qboolean isSky )
{
	// TODO: Implement
	return 0;
}


int GL_LoadTexture( char* identifier, GL_TEXTURETYPE textureType, int width, int height, unsigned char* data, int mipmap, int iType, unsigned char* pPal )
{
	int			i;
	int			scaled_width, scaled_height;
	gltexture_t* glt, * slot;
	BOOL		mustRescale;

	glt = NULL;

tryagain:
	if (identifier[0] != 0)
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

		glt->texnum = texture_extension_number++;
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
		byte scaled[16384];

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







