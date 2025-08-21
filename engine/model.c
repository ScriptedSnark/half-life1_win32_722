// model.c -- model loading and caching

// models are the only shared resource between a client and server running
// on the same machine.

#include "quakedef.h"
#include "decal.h"
#include "textures.h"

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

void SW_Mod_Init( void )
{
}

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

	if (mod->type == mod_brush)
		Sys_Error("Mod_ExtraData called with mod_brush!\n");

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

		if (plane->type < 3)
			d = p[plane->type] - plane->dist;
		else
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
		if (mod->needload != (NL_NEEDS_LOADED | NL_UNREFERENCED))
			mod->needload = NL_UNREFERENCED;
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
	model_t* mod;
	model_t* avail;

	avail = NULL;

	if (!name[0])
		Sys_Error("Mod_ForName: NULL name");

//
// search the currently loaded models
//
	for (i = 0, mod = mod_known; i < mod_numknown; i++, mod++)
	{
		if (!strcmp(mod->name, name))
			break;

		if (mod->needload == NL_UNREFERENCED)
		{
			if (!avail || mod->type != mod_alias && mod->type != mod_studio)
				avail = mod;
		}
	}

	if (i == mod_numknown)
	{
		if (mod_numknown == MAX_MODELS)
		{
			if (avail)
			{
				mod = avail;
				Sys_Error("mod_numknown == MAX_MODELS");
			}
			else
				return NULL;
		}
		else
			mod_numknown++;
		strcpy(mod->name, name);
		if (mod->needload != (NL_NEEDS_LOADED | NL_UNREFERENCED))
			mod->needload = NL_NEEDS_LOADED;
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

	if (mod->needload == NL_PRESENT)
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

	if (developer.value > 1)
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

void Mod_MarkClient( model_t* pModel )
{
	pModel->needload = NL_NEEDS_LOADED | NL_UNREFERENCED;
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
	if (!mod)
		return NULL;

	return Mod_LoadModel(mod, crash);
}

/*
===============================================================================

					BRUSHMODEL LOADING

===============================================================================
*/

byte* mod_base;

#define MIPSCALE			(64 + 16 + 4 + 1)
#define PIXELS_SIZE			(MIPSCALE * (512 * 512) / 64)
#define PALETTE_SIZE		(256 * 3) + 2
#define TEXTUREDATA_SIZE	(PIXELS_SIZE + PALETTE_SIZE + MIP_EXTRASIZE + sizeof(miptex_t))

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
	dmiptexlump_t*	m;
	byte			dtexdata[TEXTUREDATA_SIZE];
	unsigned char* pPal;
	qboolean		wads_parsed = FALSE;
	double			starttime;
	unsigned short* rawtex;

	starttime = Sys_FloatTime();

	if (!l->filelen)
	{
		loadmodel->textures = NULL;
		return;
	}
	m = (dmiptexlump_t*)(mod_base + l->fileofs);

	m->nummiptex = LittleLong(m->nummiptex);

	loadmodel->numtextures = m->nummiptex;
	loadmodel->textures = (texture_t**)Hunk_AllocName(m->nummiptex * sizeof(texture_t**), loadname);

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
				continue;

			mt = (miptex_t*)dtexdata;
		}
		mt->width = LittleLong(mt->width);
		mt->height = LittleLong(mt->height);
		for (j = 0; j < MIPLEVELS; j++)
			mt->offsets[j] = LittleLong(mt->offsets[j]);

		if ((mt->width & 15) || (mt->height & 15))
			Sys_Error("Texture %s is not 16 aligned", mt->name);

		// total amout of pixels and palette entires
		pixels = mt->height * mt->width / 64 * MIPSCALE;
		palette = *(word*)((byte*)mt + sizeof(miptex_t) + pixels);

		tx = (texture_t*)Hunk_AllocName(8 * palette + 2 + pixels + sizeof(texture_t), loadname);
		loadmodel->textures[i] = tx;

		// copy data
		memcpy(tx->name, mt->name, sizeof(tx->name));

		if (strchr(tx->name, '~'))
			tx->name[2] = ' ';

		tx->width = mt->width;
		tx->height = mt->height;
		for (j = 0; j < MIPLEVELS; j++)
			tx->offsets[j] = mt->offsets[j] + sizeof(texture_t) - sizeof(miptex_t);

		tx->paloffset = sizeof(texture_t) + pixels + 2;

		// the pixels immediately follow the structures
		memcpy(tx + 1, mt + 1, pixels + 2);

		if (!Q_strncmp(mt->name, "sky", 3))
			R_InitSky();

		pPal = (byte*)&mt[1] + pixels + 2;
		rawtex = (unsigned short*)((char*)&tx[1] + pixels + 2);

		for (j = 0; j < palette; j++, pPal += 3, rawtex += 4)
		{
			rawtex[0] = texgammatable[pPal[2]];
			rawtex[1] = texgammatable[pPal[1]];
			rawtex[2] = texgammatable[pPal[0]];
			rawtex[3] = 0;
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
			if (Q_strcmp(tx2->name + 2, tx->name + 2))
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
=================
Mod_LoadVisibility
=================
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
Mod_LoadBrushModel
=================
*/
void Mod_LoadBrushModel( model_t* mod, void* buffer )
{
	// TODO: Implement
}

/*
=================
Mod_LoadAliasModel
=================
*/
void Mod_LoadAliasModel( model_t* mod, void* buffer )
{
	// TODO: Implement
}

/*
===============
Mod_LoadSpriteModel
===============
*/
void Mod_LoadSpriteModel( model_t* mod, void* buffer )
{
	// TODO: Implement
}

//=============================================================================

/*
================
Mod_Print
================
*/
void Mod_Print( void )
{
	// TODO: Implement
}