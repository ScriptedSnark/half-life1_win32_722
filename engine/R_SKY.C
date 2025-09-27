// r_sky.c

#include "quakedef.h"
#include "pr_cmds.h"

void R_InitFakeSkyModel( void );

/*
=========================================================

TARGA LOADING

=========================================================
*/

typedef struct _TargaHeader
{
	unsigned char 	id_length, colormap_type, image_type;
	unsigned short	colormap_index, colormap_length;
	unsigned char	colormap_size;
	unsigned short	x_origin, y_origin, width, height;
	unsigned char	pixel_size, attributes;
} TargaHeader;


int fgetLittleShort( FILE* pFile )
{
	byte	b1, b2;

	b1 = fgetc(pFile);
	b2 = fgetc(pFile);

	return (short)(b1 + (b2 << 8));
}

int fgetLittleLong( FILE* pFile )
{
	byte	b1, b2, b3, b4;

	b1 = fgetc(pFile);
	b2 = fgetc(pFile);
	b3 = fgetc(pFile);
	b4 = fgetc(pFile);

	return b1 + (b2 << 8) + (b3 << 16) + (b4 << 24);
}

short PutDitheredRGB( colorVec* pcv )
{
	int r, g, b;

	r = pcv->r + RandomLong(0, 3);
	if (r > 255)
		r = 255;
	g = pcv->g + RandomLong(0, 3);
	if (g > 255)
		g = 255;
	b = pcv->b + RandomLong(0, 3);
	if (b > 255)
		b = 255;

	if (is15bit)
	{
		return PACKEDRGB555(r, g, b);
	}
	else
	{
		return PACKEDRGB565(r, g, b);
	}
}

/*
=============
LoadTGA
=============
*/
byte* LoadTGA( FILE* fin )
{
	int				columns, rows;
	byte* pixbuf;
	byte* targa_rgba;
	int				row, column;
	TargaHeader		targa_header;

	targa_header.id_length = fgetc(fin);
	targa_header.colormap_type = fgetc(fin);
	targa_header.image_type = fgetc(fin);
	fgetLittleShort(fin);
	fgetLittleShort(fin);
	fgetc(fin);
	fgetLittleShort(fin);
	fgetLittleShort(fin);
	targa_header.width = fgetLittleShort(fin);
	targa_header.height = fgetLittleShort(fin);
	targa_header.pixel_size = fgetc(fin);
	fgetc(fin);

	if (targa_header.image_type != 2 &&
		targa_header.image_type != 10)
		Sys_Error("LoadTGA: Only type 2 and 10 targa RGB images supported\n");

	if (targa_header.colormap_type != 0
		|| (targa_header.pixel_size != 32 && targa_header.pixel_size != 24))
		Sys_Error("Texture_LoadTGA: Only 32 or 24 bit images supported (no colormaps)\n");

	columns = targa_header.width;
	rows = targa_header.height;

	targa_rgba = (byte*)Hunk_AllocName(targa_header.width * targa_header.height * 2, "SKYBOX");

	if (targa_header.id_length != 0)
		fseek(fin, targa_header.id_length, SEEK_CUR);  // skip TARGA image comment

	if (targa_header.image_type == 2)
	{	// Uncompressed, RGB images
		for (row = rows - 1; row >= 0; row--)
		{
			pixbuf = targa_rgba + row * columns * 2;
			for (column = 0; column < columns; column++)
			{
				colorVec color;
				switch (targa_header.pixel_size)
				{
				case 24:
					color.b = getc(fin);
					color.g = getc(fin);
					color.r = getc(fin);
					break;
				case 32:
					color.b = getc(fin);
					color.g = getc(fin);
					color.r = getc(fin);
					getc(fin);
					break;
				}
				*(unsigned short*)pixbuf = PutDitheredRGB(&color);
				pixbuf += 2;
			}
		}
	}
	else if (targa_header.image_type == 10)
	{	// Runlength encoded RGB images
		unsigned char packetHeader, packetSize, j;
		colorVec color;
		for (row = rows - 1; row >= 0; row--)
		{
			pixbuf = targa_rgba + row * columns * 2;
			for (column = 0; column < columns; )
			{
				packetHeader = getc(fin);

				packetSize = 1 + (packetHeader & 0x7f);
				if (packetHeader & 0x80)	// run-length packet
				{	// run-length packet
					switch (targa_header.pixel_size)
					{
					case 24:
						color.b = getc(fin);
						color.g = getc(fin);
						color.r = getc(fin);
						break;
					case 32:
						color.b = getc(fin);
						color.g = getc(fin);
						color.r = getc(fin);
						getc(fin);
						break;
					}

					for (j = 0; j < packetSize; j++)
					{
						*(unsigned short*)pixbuf = PutDitheredRGB(&color);
						pixbuf += 2;

						column++;
						if (column == columns)
						{	// run spans across rows
							column = 0;
							if (row > 0)
								row--;
							else
								goto breakOut;
							pixbuf = targa_rgba + row * columns * 2;
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
							color.b = getc(fin);
							color.g = getc(fin);
							color.r = getc(fin);
							break;
						case 32:
							color.b = getc(fin);
							color.g = getc(fin);
							color.r = getc(fin);
							getc(fin);
							break;
						}
						*(unsigned short*)pixbuf = PutDitheredRGB(&color);
						pixbuf += 2;

						column++;
						if (column == columns)
						{	// pixel packet run spans across rows
							column = 0;
							if (row > 0)
								row--;
							else
								goto breakOut;
							pixbuf = targa_rgba + row * columns * 2;
						}
					}
				}
			}
		breakOut:;
		}
	}

	fclose(fin);

	return targa_rgba;
}

/*
==================
R_LoadSkys
==================
*/
const char* suf[6] = { "rt", "bk", "lf", "ft", "up", "dn" };
pixel_t* gSkies[6];
const int	skytexorder[6] = { 0, 2, 1, 3, 4, 5 };
const int	gFakePlaneType[6] = { 1, -1, -2, 2, 3, -3 };
msurface_t	gFakeSurface[6];
texture_t	gFakeTex;
mtexinfo_t	gFakeTexInfo[6];
mplane_t	gFakePlanes[6];
bedge_t		gFakeEdges[4];
mvertex_t	gFakeVerts[4];
int gLoadSky = FALSE, gDrawSky = FALSE;
cl_entity_t	gFakeEntity;
float	skymins[2][6], skymaxs[2][6];

void R_LoadSkys( void )
{
	int		i, skipBMP;
	char	name[MAX_QPATH];
	FILE* hFile;

	if (!gLoadSky)
		return;

retry:
	skipBMP = FALSE;

	for (i = 0; i < 6; i++)
	{
		sprintf(name, "gfx/env/%s%s.tga", cl_skyname.string, suf[i]);

		if (!skipBMP && COM_FOpenFile(name, &hFile) > 0)
		{
			gSkies[i] = LoadTGA(hFile);
			if (gSkies[i])
			{
				Con_DPrintf("SKY: %s\n", name);
			}
			else
			{
				Con_Printf("Couldn't load %s\n", name);
				gSkies[i] = 0;

				if ((i * 4) == 0 && strcmp(cl_skyname.string, "desert"))
				{
					Cvar_Set("cl_skyname", "desert");
					goto retry;
				}
			}
		}
		else
		{
			skipBMP = TRUE;
			sprintf(name, "gfx/env/%s%s.bmp", cl_skyname.string, suf[i]);

			if (COM_FOpenFile(name, &hFile) > 0)
			{
				gSkies[i] = LoadBMP16(hFile, is15bit);
				if (gSkies[i])
				{
					Con_DPrintf("SKY: %s\n", name);
				}
				else
				{
					Con_Printf("Couldn't load %s\n", name);
					gSkies[i] = 0;

					if ((i * 4) == 0 && strcmp(cl_skyname.string, "desert"))
					{
						Cvar_Set("cl_skyname", "desert");
						goto retry;
					}
				}
			}
		}
	}

	R_InitFakeSkyModel();

	gLoadSky = FALSE;
}

vec3_t skyclip[6] = {
	{1,1,0},
	{1,-1,0},
	{0,-1,1},
	{0,1,1},
	{1,0,1},
	{-1,0,1}
};

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

// REVISIT: Do we want to spend this much CPU to save fill rate?
void DrawSkyPolygon( int nump, vec_t* vecs )
{
	int		i, j;
	vec3_t	v, av;
	float	s, t, dv;
	int		axis;
	float* vp;

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
void R_DrawSkyChain( msurface_t* psurf )
{
	vec3_t verts[MAX_CLIP_VERTS];
	int			i, lindex;
	medge_t* pedge, * pedges;

	gDrawSky = TRUE;

	pedges = currententity->model->edges;
	if (psurf->numedges > 0)
	{
		for (i = 0; i < psurf->numedges; i++)
		{
			lindex = currententity->model->surfedges[psurf->firstedge + i];

			if (lindex > 0)
			{
				pedge = &pedges[lindex];
				VectorSubtract(r_pcurrentvertbase[pedge->v[0]].position, r_origin, verts[i]);
			}
			else
			{
				lindex = -lindex;
				pedge = &pedges[lindex];
				VectorSubtract(r_pcurrentvertbase[pedge->v[1]].position, r_origin, verts[i]);
			}
		}

		ClipSkyPolygon(psurf->numedges, verts[0], 0);
	}
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

	gDrawSky = FALSE;
}

void MakeSkyVec( float s, float t, int axis, vec_t* output )
{
	vec3_t		b;
	int			j, k;

	if (s < -1)
		s = -1;
	else if (s > 1)
		s = 1;
	if (t < -1)
		t = -1;
	else if (t > 1)
		t = 1;

	b[0] = s * 2048.0;
	b[1] = t * 2048.0;
	b[2] = 2048.0;

	for (j = 0; j < 3; j++)
	{
		k = st_to_vec[axis][j];
		if (k < 0)
			output[j] = -b[-k - 1];
		else
			output[j] = b[k - 1];
	}
}

/*
=================
R_InitFakeSkyModel
=================
*/
void R_InitFakeSkyModel( void )
{
	int	i;

	gFakeEdges[0].v[0] = &gFakeVerts[0];
	gFakeEdges[0].v[1] = &gFakeVerts[1];
	gFakeEdges[1].v[0] = &gFakeVerts[1];
	gFakeEdges[1].v[1] = &gFakeVerts[2];
	gFakeEdges[2].v[0] = &gFakeVerts[2];
	gFakeEdges[2].v[1] = &gFakeVerts[3];
	gFakeEdges[3].v[0] = &gFakeVerts[3];
	gFakeEdges[3].v[1] = &gFakeVerts[0];

	gFakeEdges[0].pnext = &gFakeEdges[1];
	gFakeEdges[1].pnext = &gFakeEdges[2];
	gFakeEdges[2].pnext = &gFakeEdges[3];
	gFakeEdges[3].pnext = NULL;

	gFakeTex.width = 256;
	gFakeTex.height = 256;

	memset(gFakeSurface, 0, sizeof(gFakeSurface));
	memset(gFakeTexInfo, 0, sizeof(gFakeTexInfo));

	for (i = 0; i < 6; i++)
	{
		gFakeSurface[i].texturemins[0] = 0;
		gFakeSurface[i].texturemins[1] = 0;
		gFakeSurface[i].extents[0] = 256;
		gFakeSurface[i].extents[1] = 256;
		gFakeSurface[i].texinfo = &gFakeTexInfo[i];
		gFakeSurface[i].samples = (color24*)gSkies[skytexorder[i]];
		gFakeSurface[i].plane = &gFakePlanes[i];
		gFakeSurface[i].flags = SURF_DRAWSKY;

		VectorCopy(vec3_origin, gFakePlanes[i].normal);
		VectorCopy(vec3_origin, gFakeTexInfo[i].vecs[0]);
		VectorCopy(vec3_origin, gFakeTexInfo[i].vecs[1]);

		gFakeTexInfo[i].texture = &gFakeTex;

		switch (gFakePlaneType[i])
		{
		case 1:
			gFakeTexInfo[i].vecs[0][1] = -0.0625;
			gFakeTexInfo[i].vecs[1][2] = -0.0625;
			gFakePlanes[i].normal[0] = 1;
			gFakePlanes[i].type = PLANE_X;
			gFakePlanes[i].dist = 2048;
			break;
		case -1:
			gFakeTexInfo[i].vecs[0][1] = 0.0625;
			gFakeTexInfo[i].vecs[1][2] = -0.0625;
			gFakePlanes[i].normal[0] = 1;
			gFakePlanes[i].type = PLANE_X;
			gFakePlanes[i].dist = -2048;
			break;
		case 2:
			gFakeTexInfo[i].vecs[0][0] = 0.0625;
			gFakeTexInfo[i].vecs[1][2] = 0.0625;
			gFakePlanes[i].normal[1] = 1;
			gFakePlanes[i].type = PLANE_Y;
			gFakePlanes[i].dist = 2048;
			break;
		case -2:
			gFakeTexInfo[i].vecs[0][0] = -0.0625;
			gFakeTexInfo[i].vecs[1][2] = 0.0625;
			gFakePlanes[i].normal[1] = 1;
			gFakePlanes[i].type = PLANE_Y;
			gFakePlanes[i].dist = -2048;
			break;
		case 3:
			gFakeTexInfo[i].vecs[1][0] = 0.0625;
			gFakeTexInfo[i].vecs[0][1] = -0.0625;
			gFakePlanes[i].normal[2] = 1;
			gFakePlanes[i].type = PLANE_Z;
			gFakePlanes[i].dist = 2048;
			break;
		case -3:
			gFakeTexInfo[i].vecs[1][0] = -0.0625;
			gFakeTexInfo[i].vecs[0][1] = -0.0625;
			gFakePlanes[i].normal[2] = 1;
			gFakePlanes[i].type = PLANE_Z;
			gFakePlanes[i].dist = -2048;
			break;
		}

		gFakeTexInfo[i].mipadjust = 1.0;
		gFakeTexInfo[i].vecs[0][3] = 128;
		gFakeTexInfo[i].vecs[1][3] = 128;
	}
}

/*
=================
R_DrawSkyBox
=================
*/
void R_DrawSkyBox( int sortKey )
{
	int		i;
	vec3_t	oldmodelorg;

	if (!gDrawSky)
		return;

	if (r_draworder.value)
		r_currentbkey = 1;
	else
		r_currentbkey = sortKey + 1;

	VectorCopy(modelorg, oldmodelorg);
	VectorCopy(r_origin, r_entorigin);
	VectorCopy(r_entorigin, gFakeEntity.origin);

	VectorClear(modelorg);
	VectorClear(r_worldmodelorg);

	r_clipflags = 15;

	VectorCopy(vec3_origin, gFakeEntity.angles);
	currententity = &gFakeEntity;

	R_RotateBmodel();

	insubmodel = TRUE;

	for (i = 0; i < 6; i++)
	{
		if (skymins[0][i] >= skymaxs[0][i] || skymins[1][i] >= skymaxs[1][i])
			continue;

		MakeSkyVec(skymins[0][i], skymins[1][i], i, gFakeVerts[0].position);
		MakeSkyVec(skymins[0][i], skymaxs[1][i], i, gFakeVerts[1].position);
		MakeSkyVec(skymaxs[0][i], skymaxs[1][i], i, gFakeVerts[2].position);
		MakeSkyVec(skymaxs[0][i], skymins[1][i], i, gFakeVerts[3].position);

		R_RenderBmodelFace(gFakeEdges, &gFakeSurface[i]);
	}

	VectorCopy(oldmodelorg, modelorg);
	VectorCopy(base_vpn, vpn);
	VectorCopy(base_vright, vright);
	VectorCopy(base_vup, vup);

	R_TransformFrustum();
}

/*
==================
R_InitSky
==================
*/
void R_InitSky( void )
{
	gLoadSky = TRUE;
}