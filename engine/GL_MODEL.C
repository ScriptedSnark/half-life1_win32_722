// gl_model.c -- model loading and caching

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

// TODO: Implement

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
==============================================================================

ALIAS MODELS

==============================================================================
*/



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

// TODO: Implement

/*
===============
Mod_LoadSpriteModel
===============
*/
void Mod_LoadSpriteModel( model_t* mod, void* buffer )
{
	// TODO: Implement
}

// TODO: Implement