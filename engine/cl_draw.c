//
// cl_draw.c
//

#include "quakedef.h"
#include "cl_draw.h"
#if !defined ( GLQUAKE )
#include "r_local.h"
#endif

typedef struct spritelist_s
{
	struct model_s* pSprite;
	char* pName;
	int frameCount;
} SPRITELIST;

SPRITELIST* gSpriteList;
int gSpriteCount;

msprite_t* gpSprite;

#define SPR_MAX_SPRITES		128
unsigned short gSpritePalette[256];

// Crosshair sprite and colors
HSPRITE_t ghCrosshair;
wrect_t gCrosshairRc;
int gCrosshairR;
int gCrosshairG;
int gCrosshairB;

void SetCrosshair( HSPRITE_t hspr, wrect_t rc, int r, int g, int b )
{
	ghCrosshair = hspr;
	gCrosshairRc = rc;
	gCrosshairR = r;
	gCrosshairG = g;
	gCrosshairB = b;
}

// Draw the crosshair
void DrawCrosshair( int x, int y )
{
	if (!ghCrosshair)
		return;

	SPR_Set(ghCrosshair, gCrosshairR, gCrosshairG, gCrosshairB);
	SPR_DrawHoles(0, x + (gCrosshairRc.left - gCrosshairRc.right) / 2, y + (gCrosshairRc.top - gCrosshairRc.bottom) / 2, &gCrosshairRc);
}

qboolean gSpriteMipMap = TRUE;

// Loads a sprite by name
// A maximum of 256 HUD sprites can be loaded at the same time
HSPRITE_t SPR_Load( const char* pTextureName )
{
	int i;
	model_t* pSprite;
	model_t* pModel;

	if (!pTextureName)
		return 0;

	if (!gSpriteList || gSpriteCount <= 0)
		return 0;

	//
	// Find a free model slot spot

	for (i = 0; i < gSpriteCount; i++)
	{
		if (!gSpriteList[i].pSprite)
			break; // Found one

		if (!_stricmp(pTextureName, gSpriteList[i].pName))
			return i + 1;
	}

	if (i >= gSpriteCount)
	{
		Sys_Error("cannot allocate more than %d HUD sprites\n", SPR_MAX_SPRITES);
		return 0;
	}

	// Now allocate the memory and take the slot
	gSpriteList[i].pSprite = (model_t*)Hunk_Alloc(sizeof(model_t));
	gSpriteList[i].pName = (char*)Hunk_Alloc(strlen(pTextureName));
	strcpy(gSpriteList[i].pName, pTextureName);

	pSprite = gSpriteList[i].pSprite;
	strcpy(pSprite->name, pTextureName);

	gSpriteMipMap = FALSE;

	pSprite->needload = TRUE;
	pModel = Mod_LoadModel(pSprite, FALSE);

	gSpriteMipMap = TRUE;

	gSpriteList[i].pSprite = pModel;
	if (gSpriteList[i].pSprite)
	{
		gSpriteList[i].frameCount = ModelFrameCount(gSpriteList[i].pSprite);
		return i + 1;
	}

	return 0;
}

SPRITELIST* SPR_Get( HSPRITE_t hSprite )
{
	int spriteIndex;

	spriteIndex = hSprite - 1;

	if (spriteIndex < 0 || spriteIndex >= gSpriteCount)
		return NULL;

	return &gSpriteList[spriteIndex];
}

msprite_t* SPR_Pointer( SPRITELIST* pList )
{
	return (msprite_t*)pList->pSprite->cache.data;
}

void SPR_Init( void )
{
	int listSize;

	ghCrosshair = 0;

	gSpriteCount = SPR_MAX_SPRITES;

	listSize = SPR_MAX_SPRITES * sizeof(SPRITELIST);
	gSpriteList = (SPRITELIST *)Hunk_Alloc(listSize);
	memset(gSpriteList, 0, listSize);
}

// Gets the number of frames in the sprite
int SPR_Frames( HSPRITE_t hSprite )
{
	SPRITELIST* pList;

	pList = SPR_Get(hSprite);
	if (pList)
		return pList->frameCount;
	return 0;
}

// Gets the width of a given sprite frame
int SPR_Width( HSPRITE_t hSprite, int frame )
{
	mspriteframe_t* pFrame;
	SPRITELIST* pList;
	msprite_t* pSprite;

	pList = SPR_Get(hSprite);
	if (pList)
	{
		pSprite = SPR_Pointer(pList);
		pFrame = R_GetSpriteFrame(pSprite, frame);
		if (pFrame)
			return pFrame->width;
	}

	return 0;
}

// Gets the height of a given sprite frame
int SPR_Height( HSPRITE_t hSprite, int frame )
{
	mspriteframe_t *pFrame;
	SPRITELIST *pList;
	msprite_t* pSprite;

	pList = SPR_Get(hSprite);
	if (pList)
	{
		pSprite = SPR_Pointer(pList);
		pFrame = R_GetSpriteFrame(pSprite, frame);
		if (pFrame)
			return pFrame->height;
	}

	return 0;
}

#if defined( GLQUAKE )
void UnpackPalette( word* pDest, word* pSource, int r, int g, int b )
{
}
#else
void UnpackPalette( word* pDest, word* pSource, int r, int g, int b )
{
	word* red, * green, * blue;
	int i;

	red = red_64klut + ((r * 192) & 0xFF00);
	green = green_64klut + ((g * 192) & 0xFF00);
	blue = blue_64klut + ((b * 192) & 0xFF00);

	for (i = 0; i < 256; i++)
	{
		pDest[i] = (int)(red[pSource[i * 4 + 0]] | green[pSource[i * 4 + 1]] | blue[pSource[i * 4 + 2]]);
	}
}
#endif

// Sets the sprite to draw, and its color
void SPR_Set( HSPRITE_t hSprite, int r, int g, int b )
{
	SPRITELIST* pList;

	pList = SPR_Get(hSprite);
	if (pList)
	{
		gpSprite = SPR_Pointer(pList);
		if (gpSprite)
		{
#if defined ( GLQUAKE )
			qglColor4f((GLfloat)r / 255.0, (GLfloat)g / 255.0, (GLfloat)b / 255.0, 1.0);
#else
			UnpackPalette(gSpritePalette, (unsigned short*)((byte*)gpSprite + gpSprite->paloffset), r, g, b);
#endif
		}
	}
}

// Precondition: SPR_Set has already been called for a sprite
// Draws the currently active sprite to the screen,
// at position(x, y), where(0, 0) is the top left - hand corner of the screen
void SPR_Draw( int frame, int x, int y, const wrect_t* prc )
{
	mspriteframe_t* pFrame;

	if (gpSprite == NULL)
		return;

	if (x >= (int)vid.width || y >= (int)vid.height)
		return;
	
	pFrame = R_GetSpriteFrame(gpSprite, frame);
	if (pFrame == NULL)
		Sys_Error("SPR_Draw error:  invalid frame");

	Draw_SpriteFrame(pFrame, gSpritePalette, x, y, prc);
}

// Precondition: SPR_Set has already been called for a sprite
// Draws the currently active sprite to the screen
// Color index #255 is treated as transparent
void SPR_DrawHoles( int frame, int x, int y, const wrect_t* prc )
{
	mspriteframe_t* pFrame;

	if (gpSprite == NULL)
		return;

	if (x >= (int)vid.width || y >= (int)vid.height)
		return;

	pFrame = R_GetSpriteFrame(gpSprite, frame);

	Draw_SpriteFrameHoles(pFrame, gSpritePalette, x, y, prc);
}

// Precondition: SPR_Set has already been called for a sprite
// Draws the currently active sprite to the screen, adding it's color values to the background
void SPR_DrawAdditive( int frame, int x, int y, const wrect_t* prcSubRect )
{
	mspriteframe_t* pFrame;

	if (gpSprite == NULL)
		return;

	if (x >= (int)vid.width || y >= (int)vid.height)
		return;

	pFrame = R_GetSpriteFrame(gpSprite, frame);

	Draw_SpriteFrameAdditive(pFrame, gSpritePalette, x, y, prcSubRect);
}

// Creates a clipping rectangle
// No pixels will be drawn outside the specified area
// Will stay in effect until either the next frame, or SPR_DisableScissor is called
void SPR_EnableScissor( int x, int y, int width, int height )
{
	EnableScissorTest(x, y, width, height);
}

// Disables the effect of an SPR_EnableScissor call
void SPR_DisableScissor( void )
{
	DisableScissorTest();
}