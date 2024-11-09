//			
//  hud.h
//
// class CHud declaration
//
// CHud handles the message, calculation, and drawing the HUD
//


#define RGB_YELLOWISH 0x00FFA000 //255,160,0
#define RGB_REDISH 0x00FF1010 //255,160,0
#define RGB_GREENISH 0x0000A000 //0,160,0

typedef struct rect_s
{
	int				left, right, top, bottom;
} wrect_t;

#include "cl_dll.h"
#include "ammo.h"

#define DHN_DRAWZERO 1
#define DHN_2DIGITS  2
#define DHN_3DIGITS  4
#define MIN_ALPHA	 100	

#define		HUDELEM_ACTIVE	1

typedef struct {
	int x, y;
} POSITION;

typedef struct {
	unsigned char r,g,b,a;
} RGBA;


#define HUD_ACTIVE	1
#define HUD_INTERMISSION 2

#define MAX_PLAYER_NAME_LENGTH		32

//
//-----------------------------------------------------
//
class CHudBase
{
public:
	POSITION  m_pos;
	int   m_type;
	int	  m_iFlags; // active, moving, 
	virtual int Init( void ) {return 0;}
	virtual int VidInit( void ) {return 0;}
	virtual int Draw(float flTime) {return 0;}
	virtual void Think(void) {return;}
	virtual void Reset(void) {return;}
	virtual void InitHUDData( void ) {}		// called every time a server is connected to

};

struct HUDLIST {
	CHudBase	*p;
	HUDLIST		*pNext;
};


//
//-----------------------------------------------------
//
class CHudAmmo: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	void Think(void);
	void Reset(void);
	int DrawWList(float flTime);
	int MsgFunc_CurWeapon(const char* pszName, int iSize, void *pbuf);
	int MsgFunc_WeaponList(const char* pszName, int iSize, void *pbuf);
	int MsgFunc_AmmoX(const char* pszName, int iSize, void *pbuf);
	int MsgFunc_AmmoPickup( const char* pszName, int iSize, void *pbuf );
	int MsgFunc_WeapPickup( const char* pszName, int iSize, void *pbuf );
	int MsgFunc_ItemPickup( const char* pszName, int iSize, void *pbuf );
	int MsgFunc_HideWeapon( const char* pszName, int iSize, void *pbuf );

	void _cdecl UserCmd_Slot1( void );
	void _cdecl UserCmd_Slot2( void );
	void _cdecl UserCmd_Slot3( void );
	void _cdecl UserCmd_Slot4( void );
	void _cdecl UserCmd_Slot5( void );
	void _cdecl UserCmd_Close( void );
	void _cdecl UserCmd_NextWeapon( void );
	void _cdecl UserCmd_PrevWeapon( void );

private:
	float m_fFade;
	RGBA  m_rgba;
	WEAPON *m_pWeapon;
};


#include "health.h"


#define FADE_TIME 100


//
//-----------------------------------------------------
//
class CHudGeiger: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	int MsgFunc_Geiger(const char* pszName, int iSize, void *pbuf);
	
private:
	int m_iGeigerRange;

};

//
//-----------------------------------------------------
//
class CHudTrain: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	int MsgFunc_Train(const char* pszName, int iSize, void *pbuf);

private:
	HSPRITE_t m_hSprite;
	int m_iPos;

};

//
//-----------------------------------------------------
//
class CHudMOTD : public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw( float flTime );
	void Reset( void );

	int MsgFunc_MOTD( const char *pszName, int iSize, void *pbuf );

protected:
	enum { MAX_MOTD_LENGTH = 241, };
	static int MOTD_DISPLAY_TIME;
	char m_szMOTD[ MAX_MOTD_LENGTH ];
	float m_flActiveTill;
	int m_iLines;
};

//
//-----------------------------------------------------
//
class CHudScoreboard: public CHudBase
{
public:
	int Init( void );
	void InitHUDData( void );
	int VidInit( void );
	int Draw( float flTime );
	void UserCmd_ShowScores( void );
	void UserCmd_HideScores( void );
	int MsgFunc_ScoreInfo( const char *pszName, int iSize, void *pbuf );
	void DeathMsg( int killer, int victim );

	struct extra_player_info_t {
		short frags;
		short deaths;
	};

	enum { 
		MAX_PLAYERS = 64,
	};

	hud_player_info_t m_PlayerInfoList[MAX_PLAYERS+1];	   // player info from the engine
	extra_player_info_t m_PlayerExtraInfo[MAX_PLAYERS+1];  // additional player info sent directly to the client dll

	int m_iLastKilledBy;
	int m_fLastKillTime;
	int m_iPlayerNum;
	int m_iShowscoresHeld;

	void GetAllPlayersInfo( void );
};

//
//-----------------------------------------------------
//
class CHudDeathNotice : public CHudBase
{
public:
	int Init( void );
	void InitHUDData( void );
	int VidInit( void );
	int Draw( float flTime );
	int MsgFunc_DeathMsg( const char *pszName, int iSize, void *pbuf );
};

//
//-----------------------------------------------------
//
class CHudSayText : public CHudBase
{
public:
	int Init( void );
	void InitHUDData( void );
	int VidInit( void );
	int Draw( float flTime );
	int MsgFunc_SayText( const char *pszName, int iSize, void *pbuf );
	void EnsureTextFitsInOneLineAndWrapIfHaveTo( int line );
};

//
//-----------------------------------------------------
//
class CHudBattery: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	int MsgFunc_Battery(const char *pszName,  int iSize, void *pbuf );
	
private:
	HSPRITE_t m_hSprite1;
	HSPRITE_t m_hSprite2;
	wrect_t *m_prc1;
	wrect_t *m_prc2;
	int	  m_iBat;	
	float m_fFade;
	int	  m_iHeight;		// width of the battery innards
};


//
//-----------------------------------------------------
//
class CHudFlashlight: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	void Reset( void );
	int MsgFunc_Flashlight(const char *pszName,  int iSize, void *pbuf );
	int MsgFunc_FlashBat(const char *pszName,  int iSize, void *pbuf );
	
private:
	HSPRITE_t m_hSprite1;
	HSPRITE_t m_hSprite2;
	wrect_t *m_prc1;
	wrect_t *m_prc2;
	float m_flBat;	
	int	  m_iBat;	
	int	  m_fOn;
	float m_fFade;
	int	  m_iWidth;		// width of the battery innards
};

//
//-----------------------------------------------------
//
const int maxHUDMessages = 16;
struct message_parms_t
{
	client_textmessage_t	*pMessage;
	float	time;
	int x, y;
	int	totalWidth, totalHeight;
	int width;
	int lines;
	int lineLength;
	int length;
	int r, g, b;
	int text;
	int fadeBlend;
	float charTime;
	float fadeTime;
};

//
//-----------------------------------------------------
//

class CHudMessage: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw(float flTime);
	int MsgFunc_HudText(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_GameTitle(const char *pszName, int iSize, void *pbuf);

	float FadeBlend( float fadein, float fadeout, float hold, float localTime );
	int	XPosition( float x, int width, int lineWidth );
	int YPosition( float y, int height );

	void MessageAdd( char *pName, float time );
	void MessageDrawScan( client_textmessage_t *pMessage, float time );
	void MessageScanStart( void );
	void MessageScanNextChar( void );
	void Reset( void );

private:
	client_textmessage_t		*m_pMessages[maxHUDMessages];
	float						m_startTime[maxHUDMessages];
	message_parms_t				m_parms;
	float						m_gameTitleTime;
	client_textmessage_t		*m_pGameTitle;
};

//
//-----------------------------------------------------
//
enum
{
	HUD_selection = 0,
	HUD_bucket0 = 1,
	HUD_bucket1 = 2,
	HUD_bucket2 = 3,
	HUD_bucket3 = 4,
	HUD_bucket4 = 5,
	HUD_bucket5 = 6,
	HUD_dmg_bio = 7,
	HUD_dmg_chem = 8,
	HUD_dmg_cold = 9,
	HUD_dmg_drown = 10,
	HUD_dmg_heat = 11,
	HUD_dmg_gas = 12,
	HUD_dmg_rad = 13,
	HUD_dmg_shock = 14,
	HUD_number_0 = 15,
	HUD_number_1 = 16,
	HUD_number_2 = 17,
	HUD_number_3 = 18,
	HUD_number_4 = 19,
	HUD_number_5 = 20,
	HUD_number_6 = 21,
	HUD_number_7 = 22,
	HUD_number_8 = 23,
	HUD_number_9 = 24,
	HUD_divider = 25,
	HUD_cross = 26,
	HUD_suit_full = 27,
	HUD_suit_empty = 28,
	HUD_flash_full = 29,
	HUD_flash_empty = 30,
	HUD_flash_beam = 31,
	HUD_title_half = 32,
	HUD_title_life = 33,
	HUD_item_healthkit = 34,
	HUD_item_battery = 35,
	HUD_item_longjump = 36,
	HUD_d_skull = 37,
	HUD_d_crowbar = 38,
	HUD_d_9mmhandgun = 39,
	HUD_d_357 = 40,
	HUD_d_9mmAR = 41,
	HUD_d_shotgun = 42,
	HUD_d_bolt = 43,
	HUD_d_crossbow = 44,
	HUD_d_rpg_rocket = 45,
	HUD_d_taucannon = 46,
	HUD_d_gluongun = 47,
	HUD_d_hornet = 48,
	HUD_d_satchel = 49,
	HUD_d_tripmine = 50,
	HUD_d_snark = 51,
	HUD_d_grenade = 52,
	HUD_d_tracktrain = 53,
	HUD_SPRITE_COUNT = 54
};

class CHud
{
private:
	HUDLIST						*m_pHudList;
	HSPRITE_t						m_hsprLogo;
	int							m_iLogo;
	client_sprite_t				*m_pSpriteList;
	int							m_iSpriteCount;
	float						m_flOldSensitivity;

public:

	float m_flTime;	   // the current client time
	float m_fOldTime;  // the time at which the HUD was last redrawn
	double m_flTimeDelta; // the difference between flTime and fOldTime
	Vector	m_vecOrigin;
	Vector	m_vecAngles;
	int		m_iKeyBits;
	int		m_iHideHUDDisplay;
	int		m_iFOV;

	int m_iFontHeight;
	int DrawHudNumber(int x, int y, int iFlags, int iNumber, int r, int g, int b );
	int DrawHudString(int x, int y, int iMaxX, char *szString, int r, int g, int b );
	int DrawHudStringReverse( int xpos, int ypos, int iMinX, char *szString, int r, int g, int b );
	int DrawHudNumberString( int xpos, int ypos, int iMinX, int iNumber, int r, int g, int b );

	HSPRITE_t m_rghSprites[HUD_SPRITE_COUNT]; // the sprites loaded from hud.txt
	wrect_t m_rgrcRects[HUD_SPRITE_COUNT];
	
	CHudAmmo	m_Ammo;
	CHudHealth	m_Health;
	CHudGeiger	m_Geiger;
	CHudBattery	m_Battery;
	CHudTrain	m_Train;
	CHudFlashlight m_Flash;
	CHudMessage m_Message;
	CHudScoreboard m_Scoreboard;
	CHudMOTD    m_MOTD;
	CHudDeathNotice m_DeathNotice;
	CHudSayText m_SayText;

	void Init( void );
	void VidInit( void );
	void Think(void);
	int Redraw( float flTime, int intermission );
	int UpdateClientData( client_data_t *cdata, float time );

	CHud() : m_iSpriteCount(0), m_pHudList(NULL) {}  

	// user messages
	int _cdecl MsgFunc_Logo(const char* pszName,  int iSize, void *pbuf);
	int _cdecl MsgFunc_ResetHUD(const char* pszName,  int iSize, void *pbuf);
	void _cdecl MsgFunc_InitHUD( const char* pszName, int iSize, void *pbuf );
	int _cdecl MsgFunc_SetFOV(const char* pszName,  int iSize, void *pbuf);

	// Screen information
	SCREENINFO	m_scrinfo;

	int	m_iWeaponBits;
	int	m_fPlayerDead;
	int m_iIntermission;

	void AddHudElem(CHudBase *p);

};

extern CHud gHUD;
