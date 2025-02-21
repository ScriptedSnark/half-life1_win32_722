// gl_model.c -- model loading and caching

// models are the only shared resource between a client and server running
// on the same machine.

#include "quakedef.h"
#include "decal.h"
#include "textures.h"
#include "gl_water.h"

model_t* loadmodel;
char loadname[32];	// for hunk tags
char* wadpath;

void Mod_LoadSpriteModel( model_t* mod, void* buffer );
void Mod_LoadBrushModel( model_t* mod, void* buffer );
void Mod_LoadAliasModel( model_t* mod, void* buffer );
void Mod_LoadStudioModel( model_t* mod, void* buffer );
model_t* Mod_LoadModel( model_t* mod, qboolean crash );

model_t mod_known[MAX_MODELS];
int		mod_numknown;

int gSpriteTextureFormat = SPR_NORMAL;

extern qboolean gSpriteMipMap;

/*
===============
Mod_Init

Caches the data if needed
===============
*/
void* Mod_Extradata( model_t* mod )
{
	void* r;

	r = Cache_Check(&mod->cache);
	if (r)
		return r;
	
	Mod_LoadModel(mod, TRUE);

	if (!mod->cache.data)
		Sys_Error("Mod_Extradata: caching failed");
	return mod->cache.data;
}

/*
===============
Mod_PointInLeaf
===============
*/
mleaf_t* Mod_PointInLeaf( vec_t* p, model_t* model )
{
	mnode_t* node;
	float		d;
	mplane_t* plane;

	if (!model || !model->nodes)
		Sys_Error("Mod_PointInLeaf: bad model");

	node = model->nodes;
	while (1)
	{
		if (node->contents < 0)
			return (mleaf_t*)node;
		plane = node->plane;
		d = DotProduct(p, plane->normal) - plane->dist;
		if (d > 0)
			node = node->children[0];
		else
			node = node->children[1];
	}

	return NULL;	// never reached
}

/*
===================
Mod_ClearAll
===================
*/
void Mod_ClearAll( void )
{
	int		i;
	model_t* mod;

	for (i = 0, mod = mod_known; i < mod_numknown; i++, mod++)
	{
		if (mod->type != mod_alias && mod->needload != (NL_NEEDS_LOADED | NL_UNREFERENCED))
		{
			mod->needload = NL_NEEDS_LOADED;
		}
	}
}

/*
==================
Mod_FindName

==================
*/
model_t* Mod_FindName( char* name )
{
	int		i;
	model_t	*mod;

	if (!name[0])
		Sys_Error("Mod_FindName: NULL name");

//
// search the currently loaded models
//
	for (i = 0, mod = mod_known; i < mod_numknown; i++, mod++)
	{
		if (!strcmp(mod->name, name))
			break;
	}

	if (i == mod_numknown)
	{
		if (mod_numknown == MAX_MODELS)
			Sys_Error("mod_numknown == MAX_MODELS");
		strcpy(mod->name, name);
		mod->needload = TRUE;
		mod_numknown++;
	}

	return mod;
}

/*
==================
Mod_TouchModel

==================
*/
void Mod_TouchModel( char* name )
{
	model_t* mod;

	mod = Mod_FindName(name);

	if (mod->needload == NL_PRESENT || mod->needload == (NL_NEEDS_LOADED | NL_UNREFERENCED))
	{
		if (mod->type == mod_alias || mod->type == mod_studio)
			Cache_Check(&mod->cache);
	}
}

/*
==================
Mod_LoadModel

Loads a model into the cache
==================
*/
model_t* Mod_LoadModel( model_t* mod, qboolean crash )
{
	unsigned* buf;
	byte	stackbuf[1024];		// avoid dirtying the cache heap

	if (mod->type == mod_alias || mod->type == mod_studio)
	{
		if (Cache_Check(&mod->cache))
		{
			mod->needload = NL_PRESENT;
			return mod;
		}
	}
	else
	{
		if (mod->needload == NL_PRESENT || mod->needload == (NL_NEEDS_LOADED | NL_UNREFERENCED))
			return mod;		// not cached at all
	}

//
// because the world is so huge, load it one piece at a time
//
	if (!crash)
	{

	}

//
// load the file
//
	buf = (unsigned*)COM_LoadStackFile(mod->name, stackbuf, sizeof(stackbuf));
	if (!buf)
	{
		if (crash)
			Sys_Error("Mod_NumForName: %s not found", mod->name);
		return NULL;
	}

	if (developer.value > 1.0)
		Con_DPrintf("loading %s\n", mod->name);

//
// allocate a new model
//
	COM_FileBase(mod->name, loadname);

	loadmodel = mod;

//
// fill it in
//

// call the apropriate loader
	mod->needload = NL_PRESENT;

	switch (LittleLong(*(unsigned*)buf))
	{
	case IDPOLYHEADER:
		Mod_LoadAliasModel(mod, buf);
		break;

	case IDSPRITEHEADER:
		Mod_LoadSpriteModel(mod, buf);
		break;

	case IDSTUDIOHEADER:
		Mod_LoadStudioModel(mod, buf);
		break;

	default:
		Mod_LoadBrushModel(mod, buf);
		break;
	}

	return mod;
}

/*
==================
Mod_ForName

Loads in a model for the given name
==================
*/
model_t* Mod_ForName( char* name, qboolean crash )
{
	model_t* mod;

	mod = Mod_FindName(name);

	return Mod_LoadModel(mod, crash);
}

void Mod_MarkClient( model_t* pModel )
{
	pModel->needload = (NL_NEEDS_LOADED | NL_UNREFERENCED);
}

/*
===============================================================================

					BRUSHMODEL LOADING

===============================================================================
*/

byte* mod_base;

#define MIPSCALE			(64 + 16 + 4 + 1)
#define PIXELS_SIZE			(MIPSCALE * (512 * 512) / 64)
#define PALETTE_SIZE		(256 * 3)
#define TEXTUREDATA_SIZE	(PIXELS_SIZE + PALETTE_SIZE + sizeof(word))

/*
===============
Mod_LoadTextures
===============
*/
void Mod_LoadTextures( lump_t* l )
{
	int				i, j, pixels, palette, num, max, altmax;
	miptex_t* mt;
	texture_t* tx, * tx2;
	texture_t* anims[10];
	texture_t* altanims[10];
	dmiptexlump_t* m;
	byte			dtexdata[TEXTUREDATA_SIZE + MIP_EXTRASIZE + sizeof(miptex_t)];
	unsigned char* pPal;
	qboolean		wads_parsed = FALSE;
	double			starttime;
	byte* rawtex;

	starttime = Sys_FloatTime();

	if (!l->filelen)
	{
		loadmodel->textures = NULL;
		return;
	}
	m = (dmiptexlump_t*)(mod_base + l->fileofs);

	m->nummiptex = LittleLong(m->nummiptex);

	loadmodel->numtextures = m->nummiptex;
	loadmodel->textures = (texture_t**)Hunk_AllocName(m->nummiptex * sizeof(*loadmodel->textures), loadname);

	for (i = 0; i < m->nummiptex; i++)
	{
		m->dataofs[i] = LittleLong(m->dataofs[i]);
		if (m->dataofs[i] == -1)
			continue;
		mt = (miptex_t*)((byte*)m + m->dataofs[i]);

		if (r_wadtextures.value || !LittleLong(mt->offsets[0]))
		{
			if (!wads_parsed)
			{
				TEX_InitFromWad(wadpath);
				TEX_AddAnimatingTextures();
				wads_parsed = TRUE;
			}

			if (!TEX_LoadLump(mt->name, dtexdata))
			{
				m->dataofs[i] = -1;
				continue;
			}

			mt = (miptex_t*)dtexdata;
		}

		mt->width = LittleLong(mt->width);
		mt->height = LittleLong(mt->height);
		for (j = 0; j < MIPLEVELS; j++)
			mt->offsets[j] = LittleLong(mt->offsets[j]);

		if ((mt->width & 15) || (mt->height & 15))
			Sys_Error("Texture %s is not 16 aligned", mt->name);

		// total amount of pixels and palette entires
		pixels = mt->width * mt->height / 64 * MIPSCALE;
		palette = *(word*)((byte*)mt + pixels + sizeof(miptex_t)) * 3;

		tx = (texture_t*)Hunk_AllocName(sizeof(texture_t) + palette, loadname);

		loadmodel->textures[i] = tx;

		// copy data
		memcpy(tx->name, mt->name, sizeof(tx->name));
		tx->width = mt->width;
		tx->height = mt->height;

		// palette is at the end of current texture field
		pPal = (byte*)mt + pixels + sizeof(miptex_t) + sizeof(word);
		tx->pPal = (byte*)(tx + 1);

		// store palette data
		memcpy(tx + 1, pPal, palette);

		rawtex = (byte*)(mt + 1);

		if (!Q_strncmp(mt->name, "sky", 3))
			R_InitSky();
		else
		{
			texture_mode = GL_LINEAR_MIPMAP_NEAREST; //_LINEAR;

			if (mt->name[0] == '{')
			{
				tx->gl_texturenum = GL_LoadTexture(mt->name, GLT_WORLD, tx->width, tx->height, rawtex, TRUE, TEX_TYPE_ALPHA, pPal);
			}
			else
			{
				tx->gl_texturenum = GL_LoadTexture(mt->name, GLT_WORLD, tx->width, tx->height, rawtex, TRUE, TEX_TYPE_NONE, pPal);
			}

			texture_mode = GL_LINEAR;
		}
	}

	if (wads_parsed)
	{
		TEX_CleanupWadInfo();
	}

//
// sequence the animations
//
	for (i = 0; i < m->nummiptex; i++)
	{
		tx = loadmodel->textures[i];
		if (!tx || (tx->name[0] != '+' && tx->name[0] != '-'))
			continue;
		if (tx->anim_next)
			continue; // allready sequenced

	// find the number of frames in the animation
		memset(anims, 0, sizeof(anims));
		memset(altanims, 0, sizeof(altanims));

		max = tx->name[1];
		altmax = 0;
		if (max >= 'a' && max <= 'z')
			max -= 'a' - 'A';
		if (max >= '0' && max <= '9')
		{
			max -= '0';
			altmax = 0;
			anims[max] = tx;
			max++;
		}
		else if (max >= 'A' && max <= 'J')
		{
			altmax = max - 'A';
			max = 0;
			altanims[altmax] = tx;
			altmax++;
		}
		else
			Sys_Error("Bad animating texture %s", tx->name);

		for (j = i + 1; j < m->nummiptex; j++)
		{
			tx2 = loadmodel->textures[j];
			if (!tx2 || (tx2->name[0] != '+' && tx2->name[0] != '-'))
				continue;
			if (strcmp(tx2->name + 2, tx->name + 2))
				continue;

			num = tx2->name[1];
			if (num >= 'a' && num <= 'z')
				num -= 'a' - 'A';
			if (num >= '0' && num <= '9')
			{
				num -= '0';
				anims[num] = tx2;
				if (num + 1 > max)
					max = num + 1;
			}
			else if (num >= 'A' && num <= 'J')
			{
				num = num - 'A';
				altanims[num] = tx2;
				if (num + 1 > altmax)
					altmax = num + 1;
			}
			else
				Sys_Error("Bad animating texture %s", tx->name);
		}

#define	ANIM_CYCLE	1
	// link them all together
		for (j = 0; j < max; j++)
		{
			tx2 = anims[j];
			if (!tx2)
				Sys_Error("Missing frame %i of %s", j, tx->name);
			tx2->anim_total = max * ANIM_CYCLE;
			tx2->anim_min = j * ANIM_CYCLE;
			tx2->anim_max = (j + 1) * ANIM_CYCLE;
			tx2->anim_next = anims[(j + 1) % max];
			if (altmax)
				tx2->alternate_anims = altanims[0];
		}
		for (j = 0; j < altmax; j++)
		{
			tx2 = altanims[j];
			if (!tx2)
				Sys_Error("Missing frame %i of %s", j, tx->name);
			tx2->anim_total = altmax * ANIM_CYCLE;
			tx2->anim_min = j * ANIM_CYCLE;
			tx2->anim_max = (j + 1) * ANIM_CYCLE;
			tx2->anim_next = altanims[(j + 1) % altmax];
			if (max)
				tx2->alternate_anims = anims[0];
		}
	}

	Con_DPrintf("Texture load: %6.1fms\n", (Sys_FloatTime() - starttime) * 1000.0);
}

/*
===============
Mod_LoadLighting
===============
*/
void Mod_LoadLighting( lump_t* l )
{
	if (!l->filelen)
	{
		loadmodel->lightdata = NULL;
		return;
	}
	loadmodel->lightdata = (color24*)Hunk_AllocName(l->filelen, loadname);
	memcpy(loadmodel->lightdata, mod_base + l->fileofs, l->filelen);
}


/*
===============
Mod_LoadVisibility
===============
*/
void Mod_LoadVisibility( lump_t* l )
{
	if (!l->filelen)
	{
		loadmodel->visdata = NULL;
		return;
	}
	loadmodel->visdata = (byte*)Hunk_AllocName(l->filelen, loadname);
	memcpy(loadmodel->visdata, mod_base + l->fileofs, l->filelen);
}


/*
=================
Mod_LoadEntities
=================
*/
void Mod_LoadEntities( lump_t* l )
{
	char* pszInputStream;

	if (!l->filelen)
	{
		loadmodel->entities = NULL;
		return;
	}
	loadmodel->entities = (char*)Hunk_AllocName(l->filelen, loadname);
	memcpy(loadmodel->entities, mod_base + l->fileofs, l->filelen);

	if (loadmodel->entities)
	{
		pszInputStream = COM_Parse(loadmodel->entities);
		while (*pszInputStream && com_token[0] != '}')
		{
			if (!strcmp(com_token, "wad"))
			{
				COM_Parse(pszInputStream);

				if (wadpath)
					free(wadpath);
				wadpath = _strdup(com_token);
				break;
			}
			pszInputStream = COM_Parse(pszInputStream);
		}
	}
}


/*
=================
Mod_LoadVertexes
=================
*/
void Mod_LoadVertexes( lump_t* l )
{
	dvertex_t* in;
	mvertex_t* out;
	int			i, count;

	in = (dvertex_t*)(mod_base + l->fileofs);
	if (l->filelen % sizeof(*in))
		Sys_Error("MOD_LoadBmodel: funny lump size in %s", loadmodel->name);
	count = l->filelen / sizeof(*in);
	out = (mvertex_t*)Hunk_AllocName(count * sizeof(*out), loadname);

	loadmodel->vertexes = out;
	loadmodel->numvertexes = count;

	for (i = 0; i < count; i++, in++, out++)
	{
		out->position[0] = LittleFloat(in->point[0]);
		out->position[1] = LittleFloat(in->point[1]);
		out->position[2] = LittleFloat(in->point[2]);
	}
}

/*
=================
Mod_LoadSubmodels
=================
*/
void Mod_LoadSubmodels( lump_t* l )
{
	dmodel_t* in;
	dmodel_t* out;
	int			i, j, count;

	in = (dmodel_t*)(mod_base + l->fileofs);
	if (l->filelen % sizeof(*in))
		Sys_Error("MOD_LoadBmodel: funny lump size in %s", loadmodel->name);
	count = l->filelen / sizeof(*in);
	out = (dmodel_t*)Hunk_AllocName(count * sizeof(*out), loadname);

	loadmodel->submodels = out;
	loadmodel->numsubmodels = count;

	for (i = 0; i < count; i++, in++, out++)
	{
		for (j = 0; j < 3; j++)
		{	// spread the mins / maxs by a pixel
			out->mins[j] = LittleFloat(in->mins[j]) - 1;
			out->maxs[j] = LittleFloat(in->maxs[j]) + 1;
			out->origin[j] = LittleFloat(in->origin[j]);
		}
		for (j = 0; j < MAX_MAP_HULLS; j++)
			out->headnode[j] = LittleLong(in->headnode[j]);
		out->visleafs = LittleLong(in->visleafs);
		out->firstface = LittleLong(in->firstface);
		out->numfaces = LittleLong(in->numfaces);
	}
}

/*
=================
Mod_LoadEdges
=================
*/
void Mod_LoadEdges( lump_t* l )
{
	dedge_t* in;
	medge_t* out;
	int 	i, count;

	in = (dedge_t*)(mod_base + l->fileofs);
	if (l->filelen % sizeof(*in))
		Sys_Error("MOD_LoadBmodel: funny lump size in %s", loadmodel->name);
	count = l->filelen / sizeof(*in);
	out = (medge_t*)Hunk_AllocName((count + 1) * sizeof(*out), loadname);

	loadmodel->edges = out;
	loadmodel->numedges = count;

	for (i = 0; i < count; i++, in++, out++)
	{
		out->v[0] = (unsigned short)LittleShort(in->v[0]);
		out->v[1] = (unsigned short)LittleShort(in->v[1]);
	}
}

/*
=================
Mod_LoadTexinfo
=================
*/
void Mod_LoadTexinfo( lump_t* l )
{
	texinfo_t* in;
	mtexinfo_t* out;
	int 	i, j, count;
	int		miptex;
	float	len1, len2;

	in = (texinfo_t*)(mod_base + l->fileofs);
	if (l->filelen % sizeof(*in))
		Sys_Error("MOD_LoadBmodel: funny lump size in %s", loadmodel->name);
	count = l->filelen / sizeof(*in);
	out = (mtexinfo_t*)Hunk_AllocName(count * sizeof(*out), loadname);

	loadmodel->texinfo = out;
	loadmodel->numtexinfo = count;

	for (i = 0; i < count; i++, in++, out++)
	{
		for (j = 0; j < 8; j++)
			out->vecs[0][j] = LittleFloat(in->vecs[0][j]);
		len1 = Length(out->vecs[0]);
		len2 = Length(out->vecs[1]);
		len1 = (len1 + len2) / 2;
		if (len1 < 0.32)
			out->mipadjust = 4;
		else if (len1 < 0.49)
			out->mipadjust = 3;
		else if (len1 < 0.99)
			out->mipadjust = 2;
		else
			out->mipadjust = 1;
#if 0
		if (len1 + len2 < 0.001)
			out->mipadjust = 1;		// don't crash
		else
			out->mipadjust = 1 / floor((len1 + len2) / 2 + 0.1);
#endif

		miptex = LittleLong(in->miptex);
		out->flags = LittleLong(in->flags);

		if (!loadmodel->textures)
		{
			out->texture = r_notexture_mip;	// checkerboard texture
			out->flags = 0;
		}
		else
		{
			if (miptex >= loadmodel->numtextures)
				Sys_Error("miptex >= loadmodel->numtextures");
			out->texture = loadmodel->textures[miptex];
			if (!out->texture)
			{
				out->texture = r_notexture_mip; // texture not found
				out->flags = 0;
			}
		}
	}
}

/*
===============
CalcSurfaceExtents

Fills in s->texturemins[] and s->extents[]
===============
*/
void CalcSurfaceExtents( msurface_t* s )
{
	float	mins[2], maxs[2], val;
	int		i, j, e;
	mvertex_t* v;
	mtexinfo_t* tex;
	int		bmins[2], bmaxs[2];

	mins[0] = mins[1] = 999999;
	maxs[0] = maxs[1] = -99999;

	tex = s->texinfo;

	for (i = 0; i < s->numedges; i++)
	{
		e = loadmodel->surfedges[s->firstedge + i];
		if (e >= 0)
			v = &loadmodel->vertexes[loadmodel->edges[e].v[0]];
		else
			v = &loadmodel->vertexes[loadmodel->edges[-e].v[1]];

		for (j = 0; j < 2; j++)
		{
			val = v->position[0] * (double)tex->vecs[j][0] +
				v->position[1] * (double)tex->vecs[j][1] +
				v->position[2] * (double)tex->vecs[j][2] +
				(double)tex->vecs[j][3];
			if (val < mins[j])
				mins[j] = val;
			if (val > maxs[j])
				maxs[j] = val;
		}
	}

	for (i = 0; i < 2; i++)
	{
		bmins[i] = floor(mins[i] / 16);
		bmaxs[i] = ceil(maxs[i] / 16);

		s->texturemins[i] = bmins[i] * 16;
		s->extents[i] = (bmaxs[i] - bmins[i]) * 16;
		if (!(tex->flags & TEX_SPECIAL) && s->extents[i] > 512)
			Sys_Error("Bad surface extents %d/%d", s->extents[0], s->extents[1]);
	}
}


/*
=================
Mod_LoadFaces
=================
*/
void Mod_LoadFaces( lump_t* l )
{
	dface_t* in;
	msurface_t* out;
	int			i, count, surfnum;
	int			planenum, side;

	in = (dface_t*)(mod_base + l->fileofs);
	if (l->filelen % sizeof(*in))
		Sys_Error("MOD_LoadBmodel: funny lump size in %s", loadmodel->name);
	count = l->filelen / sizeof(*in);
	out = (msurface_t*)Hunk_AllocName(count * sizeof(*out), loadname);
	
	loadmodel->surfaces = out;
	loadmodel->numsurfaces = count;

	for (surfnum = 0; surfnum < count; surfnum++, in++, out++)
	{
		out->firstedge = LittleLong(in->firstedge);
		out->numedges = LittleShort(in->numedges);
		out->flags = 0;
		out->pdecals = NULL; // the surface has no decals by default

		planenum = LittleShort(in->planenum);
		side = LittleShort(in->side);
		if (side)
			out->flags |= SURF_PLANEBACK;

		out->plane = loadmodel->planes + planenum;

		out->texinfo = loadmodel->texinfo + LittleShort(in->texinfo);
		
		CalcSurfaceExtents(out);

	// lighting info

		for (i = 0; i < MAXLIGHTMAPS; i++)
			out->styles[i] = in->styles[i];
		i = LittleLong(in->lightofs);
		if (i == -1)
			out->samples = NULL;
		else
			out->samples = (color24*)((byte*)loadmodel->lightdata + i);
		
	// set the drawing flags flag
		
		if (!Q_strncmp(out->texinfo->texture->name, "sky", 3))	// sky
		{
			out->flags |= (SURF_DRAWSKY | SURF_DRAWTILED);
			continue;
		}

		if (!Q_strncmp(out->texinfo->texture->name, "scroll", 6))	// scroll
		{
			out->flags |= SURF_DRAWTILED;
			continue;
		}

		if (out->texinfo->texture->name[0] == '!' ||
			!_strnicmp(out->texinfo->texture->name, "laser", 5) ||
			!_strnicmp(out->texinfo->texture->name, "water", 5))	// turbulent
		{
			out->flags |= SURF_DRAWTURB;
			GL_SubdivideSurface(out);	// cut up polygon for warps
			continue;
		}
		
		if (out->texinfo->flags & TEX_SPECIAL)
		{
			out->flags |= SURF_DRAWTILED;
			continue;
		}
	}
}


/*
=================
Mod_SetParent
=================
*/
void Mod_SetParent( mnode_t* node, mnode_t* parent )
{
	node->parent = parent;
	if (node->contents < 0)
		return;
	Mod_SetParent(node->children[0], node);
	Mod_SetParent(node->children[1], node);
}

/*
=================
Mod_LoadNodes
=================
*/
void Mod_LoadNodes( lump_t* l )
{
	int			i, j, count, p;
	dnode_t* in;
	mnode_t* out;

	in = (dnode_t*)(mod_base + l->fileofs);
	if (l->filelen % sizeof(*in))
		Sys_Error("MOD_LoadBmodel: funny lump size in %s", loadmodel->name);
	count = l->filelen / sizeof(*in);
	out = (mnode_t*)Hunk_AllocName(count * sizeof(*out), loadname);

	loadmodel->nodes = out;
	loadmodel->numnodes = count;

	for (i = 0; i < count; i++, in++, out++)
	{
		for (j = 0; j < 3; j++)
		{
			out->minmaxs[j] = LittleShort(in->mins[j]);
			out->minmaxs[3 + j] = LittleShort(in->maxs[j]);
		}

		p = LittleLong(in->planenum);
		out->plane = loadmodel->planes + p;

		out->firstsurface = LittleShort(in->firstface);
		out->numsurfaces = LittleShort(in->numfaces);

		for (j = 0; j < 2; j++)
		{
			p = LittleShort(in->children[j]);
			if (p >= 0)
				out->children[j] = loadmodel->nodes + p;
			else
				out->children[j] = (mnode_t*)(loadmodel->leafs + (-1 - p));
		}
	}

	Mod_SetParent(loadmodel->nodes, NULL);	// sets nodes and leafs
}

/*
=================
Mod_LoadLeafs
=================
*/
void Mod_LoadLeafs( lump_t* l )
{
	dleaf_t* in;
	mleaf_t* out;
	int			i, j, count, p;

	in = (dleaf_t*)(mod_base + l->fileofs);
	if (l->filelen % sizeof(*in))
		Sys_Error("MOD_LoadBmodel: funny lump size in %s", loadmodel->name);
	count = l->filelen / sizeof(*in);
	out = (mleaf_t*)Hunk_AllocName(count * sizeof(*out), loadname);

	loadmodel->leafs = out;
	loadmodel->numleafs = count;

	for (i = 0; i < count; i++, in++, out++)
	{
		for (j = 0; j < 3; j++)
		{
			out->minmaxs[j] = LittleShort(in->mins[j]);
			out->minmaxs[3 + j] = LittleShort(in->maxs[j]);
		}

		p = LittleLong(in->contents);
		out->contents = p;

		out->firstmarksurface = loadmodel->marksurfaces +
			in->firstmarksurface;
		out->nummarksurfaces = LittleShort(in->nummarksurfaces);

		p = LittleLong(in->visofs);
		if (p == -1)
			out->compressed_vis = NULL;
		else
			out->compressed_vis = loadmodel->visdata + p;
		out->efrags = NULL;

		for (j = 0; j < 4; j++)
			out->ambient_sound_level[j] = in->ambient_level[j];
	}
}

/*
=================
Mod_LoadClipnodes
=================
*/
void Mod_LoadClipnodes( lump_t* l )
{
	dclipnode_t* in, * out;
	int			i, count;
	hull_t* hull;

	in = (dclipnode_t*)(mod_base + l->fileofs);
	if (l->filelen % sizeof(*in))
		Sys_Error("MOD_LoadBmodel: funny lump size in %s", loadmodel->name);
	count = l->filelen / sizeof(*in);
	out = (dclipnode_t*)Hunk_AllocName(count * sizeof(*out), loadname);

	loadmodel->clipnodes = out;
	loadmodel->numclipnodes = count;

	hull = &loadmodel->hulls[1];
	hull->clipnodes = out;
	hull->firstclipnode = 0;
	hull->lastclipnode = count - 1;
	hull->planes = loadmodel->planes;
	hull->clip_mins[0] = -16;
	hull->clip_mins[1] = -16;
	hull->clip_mins[2] = -36;
	hull->clip_maxs[0] = 16;
	hull->clip_maxs[1] = 16;
	hull->clip_maxs[2] = 36;

	hull = &loadmodel->hulls[2];
	hull->clipnodes = out;
	hull->firstclipnode = 0;
	hull->lastclipnode = count - 1;
	hull->planes = loadmodel->planes;
	hull->clip_mins[0] = -32;
	hull->clip_mins[1] = -32;
	hull->clip_mins[2] = -32;
	hull->clip_maxs[0] = 32;
	hull->clip_maxs[1] = 32;
	hull->clip_maxs[2] = 32;

	hull = &loadmodel->hulls[3];
	hull->clipnodes = out;
	hull->firstclipnode = 0;
	hull->lastclipnode = count - 1;
	hull->planes = loadmodel->planes;
	hull->clip_mins[0] = -16;
	hull->clip_mins[1] = -16;
	hull->clip_mins[2] = -18;
	hull->clip_maxs[0] = 16;
	hull->clip_maxs[1] = 16;
	hull->clip_maxs[2] = 18;

	for (i = 0; i < count; i++, out++, in++)
	{
		out->planenum = LittleLong(in->planenum);
		out->children[0] = LittleShort(in->children[0]);
		out->children[1] = LittleShort(in->children[1]);
	}
}

/*
=================
Mod_MakeHull0

Deplicate the drawing hull structure as a clipping hull
=================
*/
void Mod_MakeHull0( void )
{
	mnode_t* in, * child;
	dclipnode_t* out;
	int			i, j, count;
	hull_t* hull;

	hull = &loadmodel->hulls[0];

	in = loadmodel->nodes;
	count = loadmodel->numnodes;
	out = (dclipnode_t*)Hunk_AllocName(count * sizeof(*out), loadname);

	hull->clipnodes = out;
	hull->firstclipnode = 0;
	hull->lastclipnode = count - 1;
	hull->planes = loadmodel->planes;

	for (i = 0; i < count; i++, out++, in++)
	{
		out->planenum = in->plane - loadmodel->planes;
		for (j = 0; j < 2; j++)
		{
			child = in->children[j];
			if (child->contents < 0)
				out->children[j] = child->contents;
			else
				out->children[j] = child - loadmodel->nodes;
		}
	}
}

/*
=================
Mod_LoadMarksurfaces
=================
*/
void Mod_LoadMarksurfaces( lump_t* l )
{
	int		i, j, count;
	short* in;
	msurface_t** out;

	in = (short*)(mod_base + l->fileofs);
	if (l->filelen % sizeof(*in))
		Sys_Error("MOD_LoadBmodel: funny lump size in %s", loadmodel->name);
	count = l->filelen / sizeof(*in);
	out = (msurface_t**)Hunk_AllocName(count * sizeof(*out), loadname);

	loadmodel->marksurfaces = out;
	loadmodel->nummarksurfaces = count;

	for (i = 0; i < count; i++)
	{
		j = LittleShort(in[i]);
		if (j >= loadmodel->numsurfaces)
			Sys_Error("Mod_ParseMarksurfaces: bad surface number");
		out[i] = loadmodel->surfaces + j;
	}
}

/*
=================
Mod_LoadSurfedges
=================
*/
void Mod_LoadSurfedges( lump_t* l )
{
	int		i, count;
	int* in, * out;

	in = (int*)(mod_base + l->fileofs);
	if (l->filelen % sizeof(*in))
		Sys_Error("MOD_LoadBmodel: funny lump size in %s", loadmodel->name);
	count = l->filelen / sizeof(*in);
	out = (int*)Hunk_AllocName(count * sizeof(*out), loadname);

	loadmodel->surfedges = out;
	loadmodel->numsurfedges = count;

	for (i = 0; i < count; i++)
		out[i] = LittleLong(in[i]);
}


/*
=================
Mod_LoadPlanes
=================
*/
void Mod_LoadPlanes( lump_t* l )
{
	int			i, j;
	mplane_t* out;
	dplane_t* in;
	int			count;
	int			bits;

	in = (dplane_t*)(mod_base + l->fileofs);
	if (l->filelen % sizeof(*in))
		Sys_Error("MOD_LoadBmodel: funny lump size in %s", loadmodel->name);
	count = l->filelen / sizeof(*in);
	out = (mplane_t*)Hunk_AllocName(count * 2 * sizeof(*out), loadname);

	loadmodel->planes = out;
	loadmodel->numplanes = count;

	for (i = 0; i < count; i++, in++, out++)
	{
		bits = 0;
		for (j = 0; j < 3; j++)
		{
			out->normal[j] = LittleFloat(in->normal[j]);
			if (out->normal[j] < 0)
				bits |= 1 << j;
		}

		out->dist = LittleFloat(in->dist);
		out->type = LittleLong(in->type);
		out->signbits = bits;
	}
}

/*
=================
RadiusFromBounds
=================
*/
float RadiusFromBounds( vec_t* mins, vec_t* maxs )
{
	int		i;
	vec3_t	corner;

	for (i = 0; i < 3; i++)
	{
		corner[i] = fabs(mins[i]) > fabs(maxs[i]) ? fabs(mins[i]) : fabs(maxs[i]);
	}

	return Length(corner);
}

/*
=================
Mod_LoadBrushModel
=================
*/
void Mod_LoadBrushModel( model_t* mod, void* buffer )
{
	int			i, j;
	dheader_t* header;
	dmodel_t* bm;

	loadmodel->type = mod_brush;

	header = (dheader_t*)buffer;

	i = LittleLong(header->version);
	if (i != Q1BSP_VERSION && i != BSPVERSION)
		Sys_Error("Mod_LoadBrushModel: %s has wrong version number (%i should be %i)", mod->name, i, BSPVERSION);

// swap all the lumps
	mod_base = (byte*)header;

	for (i = 0; i < sizeof(dheader_t) / 4; i++)
		((int*)header)[i] = LittleLong(((int*)header)[i]);

// load into heap

	Mod_LoadVertexes(&header->lumps[LUMP_VERTEXES]);
	Mod_LoadEdges(&header->lumps[LUMP_EDGES]);
	Mod_LoadSurfedges(&header->lumps[LUMP_SURFEDGES]);
	Mod_LoadEntities(&header->lumps[LUMP_ENTITIES]);
	Mod_LoadTextures(&header->lumps[LUMP_TEXTURES]);
	Mod_LoadLighting(&header->lumps[LUMP_LIGHTING]);
	Mod_LoadPlanes(&header->lumps[LUMP_PLANES]);
	Mod_LoadTexinfo(&header->lumps[LUMP_TEXINFO]);
	Mod_LoadFaces(&header->lumps[LUMP_FACES]);
	Mod_LoadMarksurfaces(&header->lumps[LUMP_MARKSURFACES]);
	Mod_LoadVisibility(&header->lumps[LUMP_VISIBILITY]);
	Mod_LoadLeafs(&header->lumps[LUMP_LEAFS]);
	Mod_LoadNodes(&header->lumps[LUMP_NODES]);
	Mod_LoadClipnodes(&header->lumps[LUMP_CLIPNODES]);
	Mod_LoadSubmodels(&header->lumps[LUMP_MODELS]);

	Mod_MakeHull0();

	mod->numframes = 2;		// regular and alternate animation
	mod->flags = 0;

//
// set up the submodels (FIXME: this is confusing)
//
	for (i = 0; i < mod->numsubmodels; i++)
	{
		bm = &mod->submodels[i];

		mod->hulls[0].firstclipnode = bm->headnode[0];
		for (j = 1; j < MAX_MAP_HULLS; j++)
		{
			mod->hulls[j].firstclipnode = bm->headnode[j];
			mod->hulls[j].lastclipnode = mod->numclipnodes - 1;
		}

		mod->firstmodelsurface = bm->firstface;
		mod->nummodelsurfaces = bm->numfaces;

		VectorCopy(bm->maxs, mod->maxs);
		VectorCopy(bm->mins, mod->mins);

		mod->radius = RadiusFromBounds(mod->mins, mod->maxs);
		mod->numleafs = bm->visleafs;

		if (i < mod->numsubmodels - 1)
		{	// duplicate the basic information
			char	name[10];

			sprintf(name, "*%i", i + 1);
			loadmodel = Mod_FindName(name);
			*loadmodel = *mod;
			strcpy(loadmodel->name, name);
			mod = loadmodel;
		}
	}
}

/*
==============================================================================

ALIAS MODELS

==============================================================================
*/

aliashdr_t* pheader;

stvert_t	stverts[MAXALIASVERTS];
mtriangle_t	triangles[MAXALIASTRIS];

// a pose is a single set of vertexes.  a frame may be
// an animating sequence of poses
trivertx_t* poseverts[MAXALIASFRAMES];
int			posenum;

byte		player_8bit_texels[320 * 200];

/*
=================
Mod_LoadAliasFrame
=================
*/
void* Mod_LoadAliasFrame( void* pin, maliasframedesc_t* frame )
{
	trivertx_t* pframe, * pinframe;
	int				i, j;
	daliasframe_t* pdaliasframe;

	pdaliasframe = (daliasframe_t*)pin;

	strcpy(frame->name, pdaliasframe->name);
	frame->firstpose = posenum;
	frame->numposes = 1;

	for (i = 0; i < 3; i++)
	{
	// these are byte values, so we don't have to worry about
	// endianness
		frame->bboxmin.v[i] = pdaliasframe->bboxmin.v[i];
		frame->bboxmax.v[i] = pdaliasframe->bboxmax.v[i];
	}

	pinframe = (trivertx_t*)(pdaliasframe + 1);

	poseverts[posenum] = pinframe;
	posenum++;

	pinframe += pheader->numverts;

	return (void*)pinframe;
}


/*
=================
Mod_LoadAliasGroup
=================
*/
void* Mod_LoadAliasGroup( void* pin, maliasframedesc_t* frame )
{
	daliasgroup_t* pingroup;
	int					i, numframes;
	daliasinterval_t* pin_intervals;
	void* ptemp;

	pingroup = (daliasgroup_t*)pin;

	numframes = LittleLong(pingroup->numframes);

	frame->firstpose = posenum;
	frame->numposes = numframes;

	for (i = 0; i < 3; i++)
	{
	// these are byte values, so we don't have to worry about endianness
		frame->bboxmin.v[i] = pingroup->bboxmin.v[i];
		frame->bboxmax.v[i] = pingroup->bboxmax.v[i];
	}

	pin_intervals = (daliasinterval_t*)(pingroup + 1);

	frame->interval = LittleFloat(pin_intervals->interval);

	pin_intervals += numframes;

	ptemp = (void*)pin_intervals;

	for (i = 0; i < numframes; i++)
	{
		poseverts[posenum] = (trivertx_t*)((daliasframe_t*)ptemp + 1);
		posenum++;

		ptemp = (trivertx_t*)((daliasframe_t*)ptemp + 1) + pheader->numverts;
	}

	return ptemp;
}

//=========================================================


// TODO: Implement



//=========================================================================

/*
=================
Mod_LoadAliasModel
=================
*/
void Mod_LoadAliasModel( model_t* mod, void* buffer )
{
	// TODO: Implement
}

//=============================================================================

byte* pspritepal;

/*
===============
Mod_LoadSpriteFrame
===============
*/
void* Mod_LoadSpriteFrame( void* pin, mspriteframe_t** ppframe, int framenum )
{
	dspriteframe_t* pinframe;
	mspriteframe_t* pspriteframe;
	int					width, height, size, origin[2], textureType;
	byte* pdata, * ppal;
	char				name[MAX_QPATH];
	byte				bPal[768];
	
	memcpy(bPal, pspritepal, sizeof(bPal));

	pinframe = (dspriteframe_t*)pin;

	width = LittleLong(pinframe->width);
	height = LittleLong(pinframe->height);
	size = width * height;

	pspriteframe = (mspriteframe_t*)Hunk_AllocName(sizeof(mspriteframe_t), loadname);
	Q_memset(pspriteframe, 0, sizeof(mspriteframe_t));

	*ppframe = pspriteframe;

	pspriteframe->width = width;
	pspriteframe->height = height;
	origin[0] = LittleLong(pinframe->origin[0]);
	origin[1] = LittleLong(pinframe->origin[1]);

	pspriteframe->up = origin[1];
	pspriteframe->down = origin[1] - height;
	pspriteframe->left = origin[0];
	pspriteframe->right = origin[0] + width;

	sprintf(name, "%s_%i", loadmodel->name, framenum);
	pdata = (byte*)(pinframe + 1);
	ppal = bPal;

	// Get the sprite texture type
	switch (gSpriteTextureFormat)
	{
		case SPR_NORMAL:
		case SPR_ADDITIVE:
			textureType = TEX_TYPE_NONE;
			break;
		case SPR_INDEXALPHA:
			textureType = TEX_TYPE_ALPHA_GRADIENT;
			break;
		case SPR_ALPHTEST:
			textureType = TEX_TYPE_ALPHA;
			break;
		default:
			textureType = TEX_TYPE_ALPHA_GRADIENT;
			break;
	}

	if (gSpriteMipMap)
		pspriteframe->gl_texturenum = GL_LoadTexture(name, GLT_SPRITE, width, height, pdata, TRUE, textureType, ppal);
	else
		pspriteframe->gl_texturenum = GL_LoadTexture(name, GLT_HUDSPRITE, width, height, pdata, FALSE, textureType, ppal);

	return (void*)((byte*)pinframe + sizeof(dspriteframe_t) + size);
}

/*
===============
Mod_LoadSpriteGroup
===============
*/
void* Mod_LoadSpriteGroup( void* pin, mspriteframe_t** ppframe, int framenum )
{
	dspritegroup_t* pingroup;
	mspritegroup_t* pspritegroup;
	int					i, numframes;
	dspriteinterval_t* pin_intervals;
	float* poutintervals;
	void* ptemp;

	pingroup = (dspritegroup_t*)pin;

	numframes = LittleLong(pingroup->numframes);

	pspritegroup = (mspritegroup_t*)Hunk_AllocName(sizeof(mspritegroup_t) +
		(numframes - 1) * sizeof(pspritegroup->frames[0]), loadname);

	pspritegroup->numframes = numframes;

	*ppframe = (mspriteframe_t*)pspritegroup;

	pin_intervals = (dspriteinterval_t*)(pingroup + 1);

	poutintervals = (float*)Hunk_AllocName(numframes * sizeof(float), loadname);

	pspritegroup->intervals = poutintervals;

	for (i = 0; i < numframes; i++)
	{
		*poutintervals = LittleFloat(pin_intervals->interval);
		if (*poutintervals <= 0.0)
			Sys_Error("Mod_LoadSpriteGroup: interval<=0");

		poutintervals++;
		pin_intervals++;
	}

	ptemp = (void*)pin_intervals;

	for (i = 0; i < numframes; i++)
	{
		ptemp = Mod_LoadSpriteFrame(ptemp, &pspritegroup->frames[i], framenum * 100 + i);
	}

	return ptemp;
}


/*
===============
Mod_LoadSpriteModel
===============
*/
void Mod_LoadSpriteModel( model_t* mod, void* buffer )
{
	int					i;
	int					version;
	dsprite_t* pin;
	msprite_t* psprite;
	int					numframes;
	int					size;
	int					palsize;
	dspriteframetype_t* pframetype;

	pin = (dsprite_t*)buffer;

	version = LittleLong(pin->version);
	if (version != SPRITE_VERSION)
		Sys_Error("%s has wrong version number (%i should be %i)",
			mod->name, version, SPRITE_VERSION);

	numframes = LittleLong(pin->numframes);
	// Skip header and read palette size in 16bit mode
	palsize = (*(word*)((byte*)buffer + sizeof(dsprite_t))) * 3 + 2;
	size = sizeof(msprite_t) + (numframes - 1) * sizeof(psprite->frames);
	psprite = (msprite_t*)Hunk_AllocName(size + palsize, loadname);
	mod->cache.data = psprite;

	psprite->type = LittleLong(pin->type);

	psprite->texFormat = LittleLong(pin->texFormat);
	gSpriteTextureFormat = psprite->texFormat;

	psprite->maxwidth = LittleLong(pin->width);
	psprite->maxheight = LittleLong(pin->height);
	psprite->beamlength = LittleFloat(pin->beamlength);

	mod->synctype = (synctype_t)LittleLong(pin->synctype);

	psprite->numframes = numframes;

	mod->mins[0] = mod->mins[1] = -psprite->maxwidth / 2;
	mod->maxs[0] = mod->maxs[1] = psprite->maxwidth / 2;
	mod->mins[2] = -psprite->maxheight / 2;
	mod->maxs[2] = psprite->maxheight / 2;

	psprite->paloffset = numframes * sizeof(psprite->frames) + 30;
	pspritepal = (byte*)(psprite->frames + numframes);

	memcpy(psprite->frames + numframes, (byte*)(pin + 1) + 2, palsize);

//
// load the frames
//
	if (numframes < 1)
		Sys_Error("Mod_LoadSpriteModel: Invalid # of frames: %d\n", numframes);

	mod->numframes = numframes;
	mod->flags = 0;

	pframetype = (dspriteframetype_t*)((byte*)(pin + 1) + palsize);

	for (i = 0; i < numframes; i++)
	{
		spriteframetype_t	frametype;

		frametype = (spriteframetype_t)LittleLong(pframetype->type);
		psprite->frames[i].type = frametype;

		if (frametype == SPR_SINGLE)
		{
			pframetype = (dspriteframetype_t*)
				Mod_LoadSpriteFrame(pframetype + 1,
					&psprite->frames[i].frameptr, i);
		}
		else
		{
			pframetype = (dspriteframetype_t*)
				Mod_LoadSpriteGroup(pframetype + 1,
					&psprite->frames[i].frameptr, i);
		}
	}

	mod->type = mod_sprite;
}

// TODO: Implement