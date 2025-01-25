// gl_warp.c -- sky and water polygons

#include "quakedef.h"
#include "pr_cmds.h"
#include "gl_water.h"

extern model_t* loadmodel;

cshift_t	cshift_water = { 130, 80, 50, 128 };

int		skytexturenum;

int		solidskytexture;
int		alphaskytexture;
float	speedscale;		// for top sky and bottom sky
colorVec gWaterColor;

msurface_t* warpface;

#define	BLOCK_WIDTH		128
#define	BLOCK_HEIGHT	128

#define SUBDIVIDE_SIZE	64.0

void R_DrawSkyBox( void );

void BoundPoly( int numverts, float* verts, vec_t* mins, vec_t* maxs )
{
	int		i, j;
	float* v;

	mins[0] = mins[1] = mins[2] = 9999;
	maxs[0] = maxs[1] = maxs[2] = -9999;
	v = verts;
	for (i = 0; i < numverts; i++)
		for (j = 0; j < 3; j++, v++)
		{
			if (*v < mins[j])
				mins[j] = *v;
			if (*v > maxs[j])
				maxs[j] = *v;
		}
}

void SubdividePolygon( int numverts, float* verts )
{
	int		i, j, k;
	vec3_t	mins, maxs;
	float	m;
	float* v;
	vec3_t	front[64], back[64];
	int		f, b;
	float	dist[64];
	float	frac;
	glpoly_t* poly;
	float	s, t;

	if (numverts > 60)
		Sys_Error("numverts = %i", numverts);

	BoundPoly(numverts, verts, mins, maxs);

	for (i = 0; i < 3; i++)
	{
		m = (mins[i] + maxs[i]) * 0.5;
		m = SUBDIVIDE_SIZE * floor(m / SUBDIVIDE_SIZE + 0.5);
		if (maxs[i] - m < 8)
			continue;
		if (m - mins[i] < 8)
			continue;

		// cut it
		v = verts + i;
		for (j = 0; j < numverts; j++, v += 3)
			dist[j] = *v - m;

		// wrap cases
		dist[j] = dist[0];
		v -= i;
		VectorCopy(verts, v);

		f = b = 0;
		v = verts;
		for (j = 0; j < numverts; j++, v += 3)
		{
			if (dist[j] >= 0)
			{
				VectorCopy(v, front[f]);
				f++;
			}
			if (dist[j] <= 0)
			{
				VectorCopy(v, back[b]);
				b++;
			}
			if (dist[j] == 0 || dist[j + 1] == 0)
				continue;
			if ((dist[j] > 0) != (dist[j + 1] > 0))
			{
				// clip point
				frac = dist[j] / (dist[j] - dist[j + 1]);
				for (k = 0; k < 3; k++)
					front[f][k] = back[b][k] = v[k] + frac * (v[3 + k] - v[k]);
				f++;
				b++;
			}
		}

		SubdividePolygon(f, front[0]);
		SubdividePolygon(b, back[0]);
		return;
	}

	poly = (glpoly_t*)Hunk_Alloc(sizeof(glpoly_t) + (numverts - 4) * VERTEXSIZE * sizeof(float));
	poly->next = warpface->polys;
	poly->flags = warpface->flags;
	warpface->polys = poly;
	poly->numverts = numverts;
	for (i = 0; i < numverts; i++, verts += 3)
	{
		VectorCopy(verts, poly->verts[i]);
		s = DotProduct(verts, warpface->texinfo->vecs[0]);
		t = DotProduct(verts, warpface->texinfo->vecs[1]);
		poly->verts[i][3] = s;
		poly->verts[i][4] = t;

		//
		// lightmap texture coordinates
		//
		s = DotProduct(verts, warpface->texinfo->vecs[0]) + warpface->texinfo->vecs[0][3];
		s -= warpface->texturemins[0];
		s += (float)(warpface->light_s * (BLOCK_WIDTH * 16));
		s += 8;
		s /= BLOCK_WIDTH * 16; //fa->texinfo->texture->width;

		t = DotProduct(verts, warpface->texinfo->vecs[1]) + warpface->texinfo->vecs[1][3];
		t -= warpface->texturemins[1];
		t += (float)(warpface->light_t * (BLOCK_HEIGHT * 16));
		t += 8;
		t /= BLOCK_HEIGHT * 16; //fa->texinfo->texture->height;

		poly->verts[i][5] = s;
		poly->verts[i][6] = t;
	}
}

/*
================
GL_SubdivideSurface

Breaks a polygon up along axial 64 unit
boundaries so that turbulent and sky warps
can be done reasonably.
================
*/
void GL_SubdivideSurface( msurface_t* fa )
{
	vec3_t		verts[64];
	int			numverts;
	int			i;
	int			lindex;
	float* vec;

	warpface = fa;

	//
	// convert edges back to a normal polygon
	//
	numverts = 0;
	for (i = 0; i < fa->numedges; i++)
	{
		lindex = loadmodel->surfedges[fa->firstedge + i];

		if (lindex > 0)
			vec = loadmodel->vertexes[loadmodel->edges[lindex].v[0]].position;
		else
			vec = loadmodel->vertexes[loadmodel->edges[-lindex].v[1]].position;
		VectorCopy(vec, verts[numverts]);
		numverts++;
	}

	SubdividePolygon(numverts, verts[0]);
}

//=========================================================



// speed up sin calculations - Ed
float	turbsin[] =
{
	#include "gl_warp_sin.h"
};
#define TURBSCALE (256.0 / (2 * M_PI))

/*
=============
D_SetFadeColor

Set the color and fog parameters for a water surface
=============
*/
void D_SetFadeColor( int r, int g, int b, int fog )
{
	gWaterColor.r = r;
	gWaterColor.g = g;
	gWaterColor.b = b;

	cshift_water.destcolor[0] = r;
	cshift_water.destcolor[1] = g;
	cshift_water.destcolor[2] = b;
	cshift_water.percent = fog;
}

void EmitWaterPolys( msurface_t* fa, int direction )
{
	glpoly_t* p;
	float* v;
	int			i;
	float		s, t, os, ot, scale;
	float		tempVert[3];
	byte* pSourcePalette;

	pSourcePalette = fa->texinfo->texture->pPal;
	D_SetFadeColor(pSourcePalette[9], pSourcePalette[10], pSourcePalette[11], pSourcePalette[12]);

	if (fa->polys->verts[0][2] >= r_refdef.vieworg[2])
		scale = -currententity->scale;
	else
		scale = currententity->scale;

	for (p = fa->polys; p; p = p->next)
	{
		qglBegin(GL_POLYGON);
		if (direction)
			v = p->verts[p->numverts - 1];
		else
			v = p->verts[0];

		for (i = 0; i < p->numverts; i++)
		{
			os = v[3];
			ot = v[4];

			VectorCopy(v, tempVert);
			s = turbsin[(int)(cl.time * 160.0 + v[0] + v[1]) & 255] + 8.0;
			s += (turbsin[(int)(cl.time * 171.0 + v[0] * 5.0 - v[1]) & 255] + 8.0) * 0.8;
			tempVert[2] += s * scale;

			s = os + turbsin[(int)((ot * 0.125 + cl.time) * TURBSCALE) & 255];
			s *= (1.0 / 64);
			t = ot + turbsin[(int)((os * 0.125 + cl.time) * TURBSCALE) & 255];
			t *= (1.0 / 64);

			qglTexCoord2f(s, t);
			qglVertex3fv(tempVert);

			if (direction)
				v -= VERTEXSIZE;
			else
				v += VERTEXSIZE;
		}
		qglEnd();
	}
}

#if 0
/*
===============
EmitBothSkyLayers

Does a sky warp on the pre-fragmented glpoly_t chain
This will be called for brushmodels, the world
will have them chained together.
===============
*/
void EmitBothSkyLayers( msurface_t* fa )
{
	GL_DisableMultitexture();

	GL_Bind(solidskytexture);
	speedscale = realtime * 8;
	speedscale -= (int)speedscale & ~127;

	EmitSkyPolys(fa);

	glEnable(GL_BLEND);
	GL_Bind(alphaskytexture);
	speedscale = realtime * 16;
	speedscale -= (int)speedscale & ~127;

	EmitSkyPolys(fa);

	glDisable(GL_BLEND);
}


#define	SKY_TEX		2000

/*
=================================================================

  PCX Loading

=================================================================
*/

typedef struct
{
	char	manufacturer;
	char	version;
	char	encoding;
	char	bits_per_pixel;
	unsigned short	xmin, ymin, xmax, ymax;
	unsigned short	hres, vres;
	unsigned char	palette[48];
	char	reserved;
	char	color_planes;
	unsigned short	bytes_per_line;
	unsigned short	palette_type;
	char	filler[58];
	unsigned 	data;			// unbounded
} pcx_t;

byte* pcx_rgb;

/*
============
LoadPCX
============
*/
void LoadPCX( FILE* f )
{
	pcx_t* pcx, pcxbuf;
	byte	palette[768];
	byte* pix;
	int		x, y;
	int		dataByte, runLength;
	int		count;

//
// parse the PCX file
//
	fread(&pcxbuf, 1, sizeof(pcxbuf), f);

	pcx = &pcxbuf;

	if (pcx->manufacturer != 0x0a
		|| pcx->version != 5
		|| pcx->encoding != 1
		|| pcx->bits_per_pixel != 8
		|| pcx->xmax >= 320
		|| pcx->ymax >= 256)
	{
		Con_Printf("Bad pcx file\n");
		return;
	}

	// seek to palette
	fseek(f, -768, SEEK_END);
	fread(palette, 1, 768, f);

	fseek(f, sizeof(pcxbuf) - 4, SEEK_SET);

	count = (pcx->xmax + 1) * (pcx->ymax + 1);
	pcx_rgb = malloc(count * 4);

	for (y = 0; y <= pcx->ymax; y++)
	{
		pix = pcx_rgb + 4 * y * (pcx->xmax + 1);
		for (x = 0; x <= pcx->ymax; )
		{
			dataByte = fgetc(f);

			if ((dataByte & 0xC0) == 0xC0)
			{
				runLength = dataByte & 0x3F;
				dataByte = fgetc(f);
			}
			else
				runLength = 1;

			while (runLength-- > 0)
			{
				pix[0] = palette[dataByte * 3];
				pix[1] = palette[dataByte * 3 + 1];
				pix[2] = palette[dataByte * 3 + 2];
				pix[3] = 255;
				pix += 4;
				x++;
			}
		}
	}
}

#endif

/*
=========================================================

TARGA LOADING

=========================================================
*/

typedef struct _TargaHeader {
	unsigned char 	id_length, colormap_type, image_type;
	unsigned short	colormap_index, colormap_length;
	unsigned char	colormap_size;
	unsigned short	x_origin, y_origin, width, height;
	unsigned char	pixel_size, attributes;
} TargaHeader;


TargaHeader		targa_header;
//byte*			targa_rgba;

int fgetLittleShort( FILE* f )
{
	byte	b1, b2;

	b1 = fgetc(f);
	b2 = fgetc(f);

	return (short)(b1 + (b2 << 8));
}

int fgetLittleLong( FILE* f )
{
	byte	b1, b2, b3, b4;

	b1 = fgetc(f);
	b2 = fgetc(f);
	b3 = fgetc(f);
	b4 = fgetc(f);

	return b1 + (b2 << 8) + (b3 << 16) + (b4 << 24);
}


/*
=============
LoadTGA
=============
*/
void LoadTGA( FILE* fin, byte* buffer )
{
	int				columns, rows, numPixels;
	byte* pixbuf;
	byte* targa_rgba;
	int				row, column;

	targa_header.id_length = fgetc(fin);
	targa_header.colormap_type = fgetc(fin);
	targa_header.image_type = fgetc(fin);

	targa_header.colormap_index = fgetLittleShort(fin);
	targa_header.colormap_length = fgetLittleShort(fin);
	targa_header.colormap_size = fgetc(fin);
	targa_header.x_origin = fgetLittleShort(fin);
	targa_header.y_origin = fgetLittleShort(fin);
	targa_header.width = fgetLittleShort(fin);
	targa_header.height = fgetLittleShort(fin);
	targa_header.pixel_size = fgetc(fin);
	targa_header.attributes = fgetc(fin);

	if (targa_header.image_type != 2 &&
		targa_header.image_type != 10)
		Sys_Error("LoadTGA: Only type 2 and 10 targa RGB images supported\n");

	if (targa_header.colormap_type
		|| (targa_header.pixel_size != 32 && targa_header.pixel_size != 24))
		Sys_Error("Texture_LoadTGA: Only 32 or 24 bit images supported (no colormaps)\n");

	columns = targa_header.width;
	rows = targa_header.height;
	numPixels = columns * rows;

	if (numPixels != (256 * 256))
		Sys_Error("Sky map is the wrong size!\n");

	targa_rgba = buffer;

	if (targa_header.id_length)
		fseek(fin, targa_header.id_length, SEEK_CUR);  // skip TARGA image comment

	if (targa_header.image_type == 2)
	{	// Uncompressed, RGB images
		for (row = rows - 1; row >= 0; row--)
		{
			pixbuf = targa_rgba + row * columns * 4;
			for (column = 0; column < columns; column++)
			{
				unsigned char red, green, blue, alphabyte;
				switch (targa_header.pixel_size)
				{
					case 24:

						blue = getc(fin);
						green = getc(fin);
						red = getc(fin);
						*pixbuf++ = red;
						*pixbuf++ = green;
						*pixbuf++ = blue;
						*pixbuf++ = 255;
						break;
					case 32:
						blue = getc(fin);
						green = getc(fin);
						red = getc(fin);
						alphabyte = getc(fin);
						*pixbuf++ = red;
						*pixbuf++ = green;
						*pixbuf++ = blue;
						*pixbuf++ = alphabyte;
						break;
				}
			}
		}
	}
	else if (targa_header.image_type == 10)
	{	// Runlength encoded RGB images
		unsigned char red, green, blue, alphabyte, packetHeader, packetSize, j;
		for (row = rows - 1; row >= 0; row--)
		{
			pixbuf = targa_rgba + row * columns * 4;
			for (column = 0; column < columns; )
			{
				packetHeader = getc(fin);
				packetSize = 1 + (packetHeader & 0x7f);
				if (packetHeader & 0x80)
				{	// run-length packet
					switch (targa_header.pixel_size)
					{
						case 24:
							blue = getc(fin);
							green = getc(fin);
							red = getc(fin);
							alphabyte = 255;
							break;
						case 32:
							blue = getc(fin);
							green = getc(fin);
							red = getc(fin);
							alphabyte = getc(fin);
							break;
					}

					for (j = 0; j < packetSize; j++)
					{
						*pixbuf++ = red;
						*pixbuf++ = green;
						*pixbuf++ = blue;
						*pixbuf++ = alphabyte;
						column++;
						if (column == columns)
						{	// run spans across rows
							column = 0;
							if (row > 0)
								row--;
							else
								goto breakOut;
							pixbuf = targa_rgba + row * columns * 4;
						}
					}
				}
				else
				{	// non run-length packet
					for (j = 0; j < packetSize; j++)
					{
						switch (targa_header.pixel_size)
						{
							case 24:
								blue = getc(fin);
								green = getc(fin);
								red = getc(fin);
								*pixbuf++ = red;
								*pixbuf++ = green;
								*pixbuf++ = blue;
								*pixbuf++ = 255;
								break;
							case 32:
								blue = getc(fin);
								green = getc(fin);
								red = getc(fin);
								alphabyte = getc(fin);
								*pixbuf++ = red;
								*pixbuf++ = green;
								*pixbuf++ = blue;
								*pixbuf++ = alphabyte;
								break;
						}
						column++;
						if (column == columns)
						{	// pixel packet run spans across rows
							column = 0;
							if (row > 0)
								row--;
							else
								goto breakOut;
							pixbuf = targa_rgba + row * columns * 4;
						}
					}
				}
			}
		breakOut:;
		}
	}

	fclose(fin);
}

/*
==================
R_LoadSkys
==================
*/
char* suf[6] = { "rt", "bk", "lf", "ft", "up", "dn" };

int gLoadSky = FALSE, gSkyTexNumber[6];
void R_LoadSkys( void )
{
	int			i, j, skipBMP;
	char		name[MAX_QPATH];
	byte* buffer;

	if (!gLoadSky)
	{
		int skytexture;
		for (i = 0; i < 6; i++)
		{
			skytexture = 2000 + i;
			qglDeleteTextures(1, &skytexture);
		}
		return;
	}

	if (qglColorTableEXT && gl_palette_tex.value)
	{
		skipBMP = FALSE;
		GL_PaletteClearSky();
	}
	else
	{
		skipBMP = TRUE;
	}

	buffer = (byte*)malloc((256 * 256) * 4);

retry:
	for (i = 0; i < 6; i++)
	{
		int loadedBMP = FALSE, palCount, paletteIndex;
		byte* packPalette, * pImage;
		int hFile[3];
		FILE* phFile[2];

		sprintf(name, "gfx/env/%s%s.bmp", cl_skyname.string, suf[i]);

		if (!skipBMP && COM_OpenFile(name, hFile) > 0)
		{
			LoadBMP8(hFile, &packPalette, &palCount, &pImage);
			if (phFile[1])
			{
				paletteIndex = GL_PaletteAdd(packPalette, TRUE);
				if (paletteIndex >= 0)
				{
					GL_Bind(2000 + i);
					loadedBMP = TRUE;
					qglTexImage2D(GL_TEXTURE_2D, 0, GL_COLOR_INDEX8_EXT, 256, 256, 0, GL_COLOR_INDEX, GL_UNSIGNED_BYTE, pImage);
					gSkyTexNumber[i] = 2000 + i + 65536 + (paletteIndex << 16);
				}

				free(pImage);
				free(packPalette);
			}
		}

		if (!loadedBMP)
		{
			skipBMP = TRUE;
			sprintf(name, "gfx/env/%s%s.tga", cl_skyname.string, suf[i]);

			COM_FOpenFile(name, phFile);
			if (!phFile[0])
			{
				Con_Printf("Couldn't load %s\n", name);

				if ((i * 4) == 0 && strcmp(cl_skyname.string, "desert"))
				{
					Cvar_Set("cl_skyname", "desert");
					goto retry;
				}
				continue;
			}

			LoadTGA(phFile[0], buffer);

			if (gl_dither.value)
			{
				for (j = 0; j < (256 * 256) * 4; j += 4)
				{
					if (buffer[j + 0] < 252)
						buffer[j + 0] += RandomLong(0, 3);
					if (buffer[j + 1] < 252)
						buffer[j + 1] += RandomLong(0, 3);
					if (buffer[j + 2] < 252)
						buffer[j + 2] += RandomLong(0, 3);
				}
			}

			GL_Bind(2000 + i);
			qglTexImage2D(GL_TEXTURE_2D, 0, 3, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
			gSkyTexNumber[i] = 2000 + i;
		}

		qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (loadedBMP)
		{
			Con_DPrintf("Loading paletted sky %s\n", name);
		}
		else
		{
			Con_DPrintf("SKY: %s\n", name);
		}
	}

	free(buffer);

	gLoadSky = FALSE; // mark as loaded
}


vec3_t	skyclip[6] = {
	{1,1,0},
	{1,-1,0},
	{0,-1,1},
	{0,1,1},
	{1,0,1},
	{-1,0,1}
};
int	c_sky;

// 1 = s, 2 = t, 3 = 2048
int	st_to_vec[6][3] =
{
	{3,-1,2},
	{-3,1,2},

	{1,3,2},
	{-1,-3,2},

	{-2,-1,3},		// 0 degrees yaw, look straight up
	{2,-1,-3}		// look straight down

//	{-1,2,3},
//	{1,2,-3}
};

// s = [0]/[2], t = [1]/[2]
int	vec_to_st[6][3] =
{
	{-2,3,1},
	{2,3,-1},

	{1,3,2},
	{-1,3,-2},

	{-2,-1,3},
	{-2,1,-3}

//	{-1,2,3},
//	{1,2,-3}
};

float	skymins[2][6], skymaxs[2][6];

void DrawSkyPolygon( int nump, vec_t* vecs )
{
	int		i, j;
	vec3_t	v, av;
	float	s, t, dv;
	int		axis;
	float* vp;

	c_sky++;
#if 0
	qglBegin(GL_POLYGON);
	for (i = 0; i < nump; i++, vecs += 3)
	{
		VectorAdd(vecs, r_origin, v);
		qglVertex3fv(v);
	}
	qglEnd();
	return;
#endif
	// decide which face it maps to
	VectorCopy(vec3_origin, v);
	for (i = 0, vp = vecs; i < nump; i++, vp += 3)
	{
		VectorAdd(vp, v, v);
	}
	av[0] = fabs(v[0]);
	av[1] = fabs(v[1]);
	av[2] = fabs(v[2]);
	if (av[0] > av[1] && av[0] > av[2])
	{
		if (v[0] < 0)
			axis = 1;
		else
			axis = 0;
	}
	else if (av[1] > av[2] && av[1] > av[0])
	{
		if (v[1] < 0)
			axis = 3;
		else
			axis = 2;
	}
	else
	{
		if (v[2] < 0)
			axis = 5;
		else
			axis = 4;
	}

	// project new texture coords
	for (i = 0; i < nump; i++, vecs += 3)
	{
		j = vec_to_st[axis][2];
		if (j > 0)
			dv = vecs[j - 1];
		else
			dv = -vecs[-j - 1];

		j = vec_to_st[axis][0];
		if (j < 0)
			s = -vecs[-j - 1] / dv;
		else
			s = vecs[j - 1] / dv;
		j = vec_to_st[axis][1];
		if (j < 0)
			t = -vecs[-j - 1] / dv;
		else
			t = vecs[j - 1] / dv;

		if (s < skymins[0][axis])
			skymins[0][axis] = s;
		if (t < skymins[1][axis])
			skymins[1][axis] = t;
		if (s > skymaxs[0][axis])
			skymaxs[0][axis] = s;
		if (t > skymaxs[1][axis])
			skymaxs[1][axis] = t;
	}
}

#define	MAX_CLIP_VERTS	64
void ClipSkyPolygon( int nump, vec_t* vecs, int stage )
{
	float* norm;
	float* v;
	qboolean	front, back;
	float	d, e;
	float	dists[MAX_CLIP_VERTS];
	int		sides[MAX_CLIP_VERTS];
	vec3_t	newv[2][MAX_CLIP_VERTS];
	int		newc[2];
	int		i, j;

	if (nump > MAX_CLIP_VERTS - 2)
		Sys_Error("ClipSkyPolygon: MAX_CLIP_VERTS");
	if (stage == 6)
	{	// fully clipped, so draw it
		DrawSkyPolygon(nump, vecs);
		return;
	}

	front = back = FALSE;
	norm = skyclip[stage];
	for (i = 0, v = vecs; i < nump; i++, v += 3)
	{
		d = DotProduct(v, norm);
		if (d > ON_EPSILON)
		{
			front = TRUE;
			sides[i] = SIDE_FRONT;
		}
		else if (d < ON_EPSILON)
		{
			back = TRUE;
			sides[i] = SIDE_BACK;
		}
		else
			sides[i] = SIDE_ON;
		dists[i] = d;
	}

	if (!front || !back)
	{	// not clipped
		ClipSkyPolygon(nump, vecs, stage + 1);
		return;
	}

	// clip it
	sides[i] = sides[0];
	dists[i] = dists[0];
	VectorCopy(vecs, (vecs + (i * 3)));
	newc[0] = newc[1] = 0;

	for (i = 0, v = vecs; i < nump; i++, v += 3)
	{
		switch (sides[i])
		{
			case SIDE_FRONT:
				VectorCopy(v, newv[0][newc[0]]);
				newc[0]++;
				break;
			case SIDE_BACK:
				VectorCopy(v, newv[1][newc[1]]);
				newc[1]++;
				break;
			case SIDE_ON:
				VectorCopy(v, newv[0][newc[0]]);
				newc[0]++;
				VectorCopy(v, newv[1][newc[1]]);
				newc[1]++;
				break;
		}

		if (sides[i] == SIDE_ON || sides[i + 1] == SIDE_ON || sides[i + 1] == sides[i])
			continue;

		d = dists[i] / (dists[i] - dists[i + 1]);
		for (j = 0; j < 3; j++)
		{
			e = v[j] + d * (v[j + 3] - v[j]);
			newv[0][newc[0]][j] = e;
			newv[1][newc[1]][j] = e;
		}
		newc[0]++;
		newc[1]++;
	}

	// continue
	ClipSkyPolygon(newc[0], newv[0][0], stage + 1);
	ClipSkyPolygon(newc[1], newv[1][0], stage + 1);
}

/*
=================
R_DrawSkyChain
=================
*/
void R_DrawSkyChain( msurface_t* s )
{
	msurface_t* fa;

	int		i;
	vec3_t	verts[MAX_CLIP_VERTS];
	glpoly_t* p;

	c_sky = 0;

	// calculate vertex values for sky box

	for (fa = s; fa; fa = fa->texturechain)
	{
		for (p = fa->polys; p; p = p->next)
		{
			for (i = 0; i < p->numverts; i++)
			{
				VectorSubtract(p->verts[i], r_origin, verts[i]);
			}

			ClipSkyPolygon(p->numverts, verts[0], 0);
		}
	}

	R_DrawSkyBox();
}


/*
==============
R_ClearSkyBox
==============
*/
void R_ClearSkyBox( void )
{
	int		i;

	for (i = 0; i < 6; i++)
	{
		skymins[0][i] = skymins[1][i] = 9999;
		skymaxs[0][i] = skymaxs[1][i] = -9999;
	}
}


#define SQRT3INV		(0.57735f)		// a little less than 1 / sqrt(3)

void MakeSkyVec( float s, float t, int axis )
{
	vec3_t		v, b;
	int			j, k;
	float		width;

	width = gl_zmax.value * SQRT3INV;

	if (s < -1)
		s = -1;
	else if (s > 1)
		s = 1;
	if (t < -1)
		t = -1;
	else if (t > 1)
		t = 1;

	b[0] = s * width;
	b[1] = t * width;
	b[2] = width;

	for (j = 0; j < 3; j++)
	{
		k = st_to_vec[axis][j];
		if (k < 0)
			v[j] = -b[-k - 1];
		else
			v[j] = b[k - 1];
		v[j] += r_origin[j];
	}

	// avoid bilerp seam
	s = (s + 1) * 0.5;
	t = (t + 1) * 0.5;

	if (s < 1.0 / 512)
		s = 1.0 / 512;
	else if (s > 511.0 / 512)
		s = 511.0 / 512;
	if (t < 1.0 / 512)
		t = 1.0 / 512;
	else if (t > 511.0 / 512)
		t = 511.0 / 512;

	t = 1.0 - t;
	qglTexCoord2f(s, t);
	qglVertex3fv(v);
}

/*
=================
R_DrawSkyBox
=================
*/
int skytexorder[6] = { 0, 2, 1, 3, 4, 5 };
#define SIGN(d)				((d)<0?-1:1)
static int	gFakePlaneType[6] = { 1, -1, 2, -2, 3, -3 };
void R_DrawSkyBox( void )
{
	int		i;
	vec3_t	normal;
	float	r = 1.0;
	float	g = 1.0;
	float	b = 1.0;

	GL_DisableMultitexture();

	for (i = 0; i < 6; i++)
	{
		if (skymins[0][i] >= skymaxs[0][i] || skymins[1][i] >= skymaxs[1][i])
			continue;

		VectorCopy(vec3_origin, normal);
		switch (gFakePlaneType[i])
		{
		case 1:
			normal[0] = 1;
			break;

		case -1:
			normal[0] = -1;
			break;

		case 2:
			normal[1] = 1;
			break;

		case -2:
			normal[1] = -1;
			break;

		case 3:
			normal[2] = 1;
			break;

		case -3:
			normal[2] = -1;
			break;
		}
		// Hack, try to find backfacing surfaces on the inside of the cube to avoid binding their texture
		if (DotProduct(vpn, normal) < (-1 + 0.70710678))
			continue;

		GL_Bind(gSkyTexNumber[skytexorder[i]]);

		qglColor3f(1, 1, 1);

		qglBegin(GL_QUADS);
		MakeSkyVec(skymins[0][i], skymins[1][i], i);
		MakeSkyVec(skymins[0][i], skymaxs[1][i], i);
		MakeSkyVec(skymaxs[0][i], skymaxs[1][i], i);
		MakeSkyVec(skymaxs[0][i], skymins[1][i], i);
		qglEnd();
	}
}

//===============================================================

/*
==================
R_InitSky
==================
*/
void R_InitSky( void )
{
	gLoadSky = TRUE;
}