//
// flashlight.cpp
//
// implementation of CHudFlashlight class
//

#include "hud.h"
#include "util.h"
#include "parsemsg.h"

#include <string.h>
#include <stdio.h>



DECLARE_MESSAGE(m_Flash, FlashBat)
DECLARE_MESSAGE(m_Flash, Flashlight)

int CHudFlashlight::Init(void)
{
	m_fFade = 0;
	m_fOn = 0;

	HOOK_MESSAGE(Flashlight);
	HOOK_MESSAGE(FlashBat);

	m_iFlags |= HUD_ACTIVE;

	gHUD.AddHudElem(this);

	return 1;
};

void CHudFlashlight::Reset(void)
{
	m_fFade = 0;
	m_fOn = 0;
}

int CHudFlashlight::VidInit(void)
{
	m_hSprite1 = gHUD.m_rghSprites[HUD_flash_empty];
	m_hSprite2 = gHUD.m_rghSprites[HUD_flash_full];
	m_prc1 = &gHUD.m_rgrcRects[HUD_flash_empty];
	m_prc2 = &gHUD.m_rgrcRects[HUD_flash_full];
	m_iWidth = m_prc2->right - m_prc2->left;

	return 1;
};

int CHudFlashlight:: MsgFunc_FlashBat(const char *pszName,  int iSize, void *pbuf )
{

	
	BEGIN_READ( pbuf, iSize );
	int x = READ_BYTE();
	m_iBat = x;
	m_flBat = ((float)x)/100.0;

	return 1;
}

int CHudFlashlight:: MsgFunc_Flashlight(const char *pszName,  int iSize, void *pbuf )
{

	BEGIN_READ( pbuf, iSize );
	m_fOn = READ_BYTE();
	int x = READ_BYTE();
	m_iBat = x;
	m_flBat = ((float)x)/100.0;

	return 1;
}

int CHudFlashlight::Draw(float flTime)
{
	if ( gHUD.m_iHideHUDDisplay & ( HIDEHUD_FLASHLIGHT | HIDEHUD_ALL ) )
		return 1;

	int r, g, b, x, y, a;
	wrect_t rc;

	if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT)) ))
		return 1;

	if (m_fOn)
		a = 225;
	else
		a = MIN_ALPHA;

	if (m_flBat < 0.20)
		UnpackRGB(r,g,b, RGB_REDISH);
	else
		UnpackRGB(r,g,b, RGB_YELLOWISH);

	ScaleColors(r, g, b, a);

	y = (m_prc1->bottom - m_prc2->top)/2;
	x = ScreenWidth - m_iWidth - m_iWidth/2 ;

	// Draw the flashlight casing
	SPR_Set(m_hSprite1, r, g, b );
	SPR_DrawAdditive( 0,  x, y, m_prc1);

	if (m_fOn)
	{ // draw the flashlight beam
		x = ScreenWidth - m_iWidth/2;

		SPR_Set( gHUD.m_rghSprites[ HUD_flash_beam ], r, g, b );
		SPR_DrawAdditive( 0, x, y, &gHUD.m_rgrcRects[ gHUD.m_rghSprites[HUD_flash_beam] ] );
	}

	// draw the flashlight energy level
	x = ScreenWidth - m_iWidth - m_iWidth/2 ;
	int iOffset = m_iWidth * (1.0 - m_flBat);
	if (iOffset < m_iWidth)
	{
		rc = *m_prc2;
		rc.left += iOffset;

		SPR_Set(m_hSprite2, r, g, b );
		SPR_DrawAdditive( 0, x + iOffset, y, &rc);
	}


	return 1;
}