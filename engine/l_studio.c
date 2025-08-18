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
	byte* pin, * pout;
#if !defined( GLQUAKE )
	byte* pindata, * poutdata;
#endif
	studiohdr_t* phdr;
	mstudiotexture_t* ptexture;
	int					start, end, total;
	int					version;

	pin = (byte*)buffer;

	phdr = (studiohdr_t*)pin;

	start = Hunk_LowMark();

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

#if defined( GLQUAKE )
	pout = (byte*)Hunk_AllocName(phdr->length, loadname);
	ptexture = (mstudiotexture_t*)((byte*)pout + phdr->textureindex);
	memcpy(pout, pin, phdr->length);
#else
	pout = (byte*)Hunk_AllocName(1280 * phdr->numtextures + phdr->length, loadname);
	ptexture = (mstudiotexture_t*)((byte*)pout + phdr->textureindex);
	memcpy(pout, pin, phdr->texturedataindex);

	poutdata = pout + phdr->texturedataindex;
	pindata = pin + phdr->texturedataindex;
#endif
	
	for (i = 0; i < phdr->numtextures; i++, ptexture++)
	{
#if defined( GLQUAKE )
		char name[256];
		byte* pal;

		pal = (byte*)pout + ptexture->index + (ptexture->width * ptexture->height);
		strcpy(name, mod->name);
		strcat(name, ptexture->name);
		ptexture->index = GL_LoadTexture(name, GLT_STUDIO, ptexture->width, ptexture->height,
			(byte*)pout + ptexture->index, FALSE, TEX_TYPE_NONE, pal);
#else
		int j, size;

		ptexture->index = poutdata - pout;
		size = ptexture->width * ptexture->height;
		memcpy(poutdata, pindata, size);
		poutdata += size;
		pindata += size;

		for (j = 0; j < 256; j++, pindata += 3, poutdata += 8)
		{
			((unsigned short*)poutdata)[0] = texgammatable[pindata[0]];
			((unsigned short*)poutdata)[1] = texgammatable[pindata[1]];
			((unsigned short*)poutdata)[2] = texgammatable[pindata[2]];
			((unsigned short*)poutdata)[3] = 0;
		}
#endif
	}

//
// move the complete, relocatable alias model to the cache
//	
	end = Hunk_LowMark();
#if defined( GLQUAKE )
	total = phdr->texturedataindex - phdr->length + end - start;
#else
	total = end - start;
#endif
	Cache_Alloc(&mod->cache, total, mod->name);
	if (!mod->cache.data)
		return;
	memcpy(mod->cache.data, pout, total);

	Hunk_FreeToLowMark(start);
}