// l_studio.c - studio model loading

#include "quakedef.h"
#include "studio.h"

int giTextureSize;

char loadname[32];

//=============================================================================


/*
=================
Mod_LoadStudioModel
=================
*/
void Mod_LoadStudioModel( model_t* mod, void* buffer )
{
	byte*				pin;
	studiohdr_t*		phdr;
	int					version;
	void*				cache;
	int					i;
	mstudiotexture_t*	ptexture;
	char				texname[256];
	unsigned char*		pPal;
	int					total;
	int					size;

	pin = (byte *)buffer;

	phdr = (studiohdr_t *)pin;

	total = Hunk_LowMark();

	version = LittleLong(phdr->version);
	if (version != STUDIO_VERSION)
	{
		memset(phdr, 0, sizeof(studiohdr_t));

		strcpy(phdr->name, "bogus");

		phdr->length = sizeof(studiohdr_t);
		phdr->texturedataindex = sizeof(studiohdr_t);
	}

	mod->flags = phdr->flags;
	mod->type = mod_studio;

	cache = Hunk_AllocName(phdr->length, loadname);
	ptexture = (mstudiotexture_t *)(((byte *)phdr) + phdr->textureindex);
	memcpy(cache, phdr, phdr->length);

	if (phdr->numtextures > 0)
	{
		for (i = 0; i < phdr->numtextures; i++)
		{
			pPal = cache + ptexture->index + ptexture->width + ptexture->height;
			strcpy(texname, mod->name);
			strcat(texname, ptexture->name);
			ptexture->index = GL_LoadTexture(texname, GLT_STUDIO, ptexture->width, ptexture->height, cache + ptexture->index, FALSE, TEX_TYPE_NONE, pPal);
		}
	}
	size = phdr->texturedataindex - phdr->length - total + Hunk_LowMark();
	Cache_Alloc(&mod->cache, size, mod->name);
	if (mod->cache.data)
	{
		memcpy(mod->cache.data, cache, size);
		Hunk_FreeToLowMark(total);
	}
}