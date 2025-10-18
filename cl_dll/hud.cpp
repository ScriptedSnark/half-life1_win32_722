//
// hud.cpp
//
// implementation of CHud class
//

#include "hud.h"
#include "util.h"
#include <string.h>
#include <stdio.h>
#include "parsemsg.h"

const char* g_rgszSpriteNames[] =
{
	"selection",
	"bucket1",
	"bucket2",
	"bucket3",
	"bucket4",
	"bucket5",
	"bucket0",
	"dmg_bio",
	"dmg_chem",
	"dmg_cold",
	"dmg_drown",
	"dmg_heat",
	"dmg_gas",
	"dmg_rad",
	"dmg_shock",
	"number_0",
	"number_1",
	"number_2",
	"number_3",
	"number_4",
	"number_5",
	"number_6",
	"number_7",
	"number_8",
	"number_9",
	"divider",
	"cross",
	"suit_full",
	"suit_empty",
	"flash_full",
	"flash_empty",
	"flash_beam",
	"title_half",
	"title_life",
	"item_longjump",
	"item_battery",
	"item_healthkit",
	"d_skull",
	"d_crowbar",
	"d_9mmhandgun",
	"d_357",
	"d_9mmAR",
	"d_shotgun",
	"d_bolt",
	"d_crossbow",
	"d_rpg_rocket",
	"d_gauss",
	"d_egon",
	"d_hornet",
	"d_satchel",
	"d_tripmine",
	"d_snark",
	"d_grenade",
	"d_tracktrain"
};

extern client_sprite_t *GetSpriteList(client_sprite_t *pList, const char *psz, int iRes, int iCount);

//DECLARE_MESSAGE(m_Logo, Logo)
int __MsgFunc_Logo(const char *pszName, int iSize, void *pbuf)
{
	return gHUD.MsgFunc_Logo(pszName, iSize, pbuf );
}

//DECLARE_MESSAGE(m_Logo, Logo)
int __MsgFunc_ResetHUD(const char *pszName, int iSize, void *pbuf)
{
	return gHUD.MsgFunc_ResetHUD(pszName, iSize, pbuf );
}

int __MsgFunc_InitHUD(const char *pszName, int iSize, void *pbuf)
{
	gHUD.MsgFunc_InitHUD( pszName, iSize, pbuf );
	return 1;
}

int __MsgFunc_SetFOV(const char *pszName, int iSize, void *pbuf)
{
	return gHUD.MsgFunc_SetFOV( pszName, iSize, pbuf );
}


// This is called every time the DLL is loaded
void CHud :: Init( void )
{
	HOOK_MESSAGE( Logo );
	HOOK_MESSAGE( ResetHUD );
	HOOK_MESSAGE( InitHUD );
	HOOK_MESSAGE( SetFOV );

	m_iLogo = 0;
	m_iFOV = 0;
	m_flOldSensitivity = 0;

	CVAR_CREATE( "zoom_sensitivity_ratio", "1.2" );
	CVAR_CREATE( "default_fov", "90" );

	m_pSpriteList = NULL;

	// Clear any old HUD list
	if ( m_pHudList )
	{
		HUDLIST *pList;
		while ( m_pHudList )
		{
			pList = m_pHudList;
			m_pHudList = m_pHudList->pNext;
			free( pList );
		}
		m_pHudList = NULL;
	}

	// In case we get messages before the first update -- time will be valid
	m_flTime = 1.0;

	m_Ammo.Init();
	m_Health.Init();
	m_Geiger.Init();
	m_Train.Init();
	m_Battery.Init();
	m_Flash.Init();
	m_Message.Init();
	m_Scoreboard.Init();
	m_MOTD.Init();
	m_DeathNotice.Init();

	m_SayText.Init();

	MsgFunc_ResetHUD(0, 0, NULL );
}

void CHud :: VidInit( void )
{
	m_scrinfo.iSize = sizeof(m_scrinfo);
	GetScreenInfo(&m_scrinfo);

	// ----------
	// Load Sprites
	// ---------
//	m_hsprFont = LoadSprite("sprites/%d_font.spr");
	
	m_hsprLogo = 0;	

	int iRes;
	if (ScreenWidth < 640)
		iRes = 320;
	else
		iRes = 640;

	m_pSpriteList = gEngfuncs.pfnSPR_GetList("sprites/hud.txt", &m_iSpriteCount);

	if (m_pSpriteList)
	{
		for (int i = 0; i < HUD_SPRITE_COUNT; i++)
		{
			client_sprite_s* p = GetSpriteList(m_pSpriteList, g_rgszSpriteNames[i], iRes, m_iSpriteCount);

			if (p)
			{
				char buffer[256];
				sprintf(buffer, "sprites/%s.spr", p->szSprite);

				m_rghSprites[i] = SPR_Load(buffer);
				m_rgrcRects[i] = p->rc;
			}
			else
			{
				m_rghSprites[i] = NULL;
			}
		}
	}

	m_iFontHeight = m_rgrcRects[HUD_number_0].bottom - m_rgrcRects[HUD_number_0].top;

	m_Ammo.VidInit();
	m_Health.VidInit();
	m_Geiger.VidInit();
	m_Train.VidInit();
	m_Battery.VidInit();
	m_Flash.VidInit();
	m_Message.VidInit();
	m_Scoreboard.VidInit();
	m_MOTD.VidInit();
	m_DeathNotice.VidInit();
	m_SayText.VidInit();
}

int CHud::MsgFunc_Logo(const char *pszName,  int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );

	// update Train data
	m_iLogo = READ_BYTE();

	return 1;
}

int CHud::MsgFunc_SetFOV(const char *pszName,  int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );

	int newfov = READ_BYTE();
	int def_fov = CVAR_GET_FLOAT( "default_fov" );
	float flSensitivity;

	// the clients fov is actually set in the client data update section of the hud
	m_iFOV = newfov;

	if ( newfov == 0 )
	{
		m_iFOV = def_fov;
		// Set a new sensitivity
		flSensitivity = m_flOldSensitivity;
	}
	else
	{
		m_flOldSensitivity = CVAR_GET_FLOAT( "sensitivity" );
		// set a new sensitivity that is proportional to the change from the FOV default
		flSensitivity = m_flOldSensitivity * ((float)newfov / (float)m_iFOV) * CVAR_GET_FLOAT("zoom_sensitivity_ratio");
	}

	if ( flSensitivity != 0 )
	{
		char buffer[32];
		sprintf(buffer, "sensitivity %.2f\n", flSensitivity);
		ClientCmd(buffer);
	}

	return 1;
}


void CHud::AddHudElem(CHudBase *phudelem)
{
	HUDLIST *pdl, *ptemp;

//phudelem->Think();

	if (!phudelem)
		return;

	pdl = (HUDLIST *)malloc(sizeof(HUDLIST));
	if (!pdl)
		return;

	memset(pdl, 0, sizeof(HUDLIST));
	pdl->p = phudelem;

	if (!m_pHudList)
	{
		m_pHudList = pdl;
		return;
	}

	ptemp = m_pHudList;

	while (ptemp->pNext)
		ptemp = ptemp->pNext;

	ptemp->pNext = pdl;
}


