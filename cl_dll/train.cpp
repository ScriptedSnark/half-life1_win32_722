//
// Train.cpp
//
// implementation of CHudAmmo class
//

#include "hud.h"
#include "util.h"
#include <string.h>
#include <stdio.h>
#include "parsemsg.h"

DECLARE_MESSAGE(m_Train, Train )


int CHudTrain::Init(void)
{
	HOOK_MESSAGE( Train );

	m_iPos = 0;
	m_iFlags = 0;
	gHUD.AddHudElem(this);

	return 1;
};

int CHudTrain::VidInit(void)
{
	m_hSprite = LoadSprite("sprites/%d_train.spr");

	return 1;
};

int CHudTrain::Draw(float fTime)
{
	if (m_iPos)
	{
		int r, g, b, x, y;

		UnpackRGB(r,g,b, RGB_YELLOWISH);
		SPR_Set(m_hSprite, r, g, b );

		// This should show up to the right and part way up the armor number
		y = ScreenHeight - SPR_Height(m_hSprite,0) - gHUD.m_iFontHeight;
		x = ScreenWidth/3 + SPR_Width(m_hSprite,0)/4;

		SPR_DrawAdditive( m_iPos - 1,  x, y, NULL);

	}

	return 1;
}


int CHudTrain::MsgFunc_Train(const char *pszName,  int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );

	// update Train data
	m_iPos = READ_BYTE();

	if (m_iPos)
		m_iFlags |= HUD_ACTIVE;
	else
		m_iFlags &= ~HUD_ACTIVE;

	return 1;
}
