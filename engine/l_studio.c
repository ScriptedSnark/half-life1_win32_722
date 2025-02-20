// l_studio.c - studio model loading

#include "quakedef.h"
#include "studio.h"

int giTextureSize;

extern char loadname[32];

//=============================================================================


/*
=================
Mod_LoadStudioModel
=================
*/
void Mod_LoadStudioModel( model_t* mod, void* buffer )
{
	int					i;
	byte				*pin, *pout;
	studiohdr_t*		phdr;
	mstudiotexture_t*	ptexture;
	byte*				block;
	int					total;
	int					version;
	int					mark;
	char				name[256];
	byte*				pal;
	pin = (byte*)buffer;

	phdr = (studiohdr_t*)pin;

	mark = Hunk_LowMark();

	version = LittleLong(phdr->version);
	if (version != STUDIO_VERSION)
	{
		memset(phdr, 0, sizeof(*phdr));
		strcpy(phdr->name, "bogus");
		phdr->length = sizeof(*phdr);
		phdr->texturedataindex = sizeof(*phdr);
	}

	mod->type = mod_studio;
	mod->flags = phdr->flags;

	block = (byte*)Hunk_AllocName(phdr->length, loadname);
	memcpy(block, phdr, phdr->length);

	ptexture = (mstudiotexture_t*)((byte*)block + phdr->textureindex);
	for (i = 0; i < phdr->numtextures; i++, ptexture++)
	{
		pal = (byte*)block + ptexture->index + (ptexture->width * ptexture->height);
		strcpy(name, mod->name);
		strcat(name, ptexture->name);
		ptexture->index = GL_LoadTexture(name, GLT_STUDIO, ptexture->width, ptexture->height,
			(byte*)block + ptexture->index, FALSE, TEX_TYPE_NONE, pal);
	}

	total = phdr->texturedataindex - phdr->length - mark + Hunk_LowMark();
	Cache_Alloc(&mod->cache, total, mod->name);

	pout = (byte*)mod->cache.data;
	if (pout)
	{
		memcpy(pout, block, total);
		Hunk_FreeToLowMark(mark);
	}
}