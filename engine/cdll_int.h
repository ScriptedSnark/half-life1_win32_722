//
//  cdll_int.h
//
// 4-23-98  
// JOHN:  client dll interface declarations
//

#ifndef CDLL_INT_H
#define CDLL_INT_H

// this file is included by both the engine and the client-dll,
// so make sure engine declarations aren't done twice

typedef int HSPRITE_t;	// handle to a graphic

#define SCRINFO_SCREENFLASH 1
#define SCRINFO_STRETCHED	2

typedef struct
{
	int		iSize;
	int		iWidth;
	int		iHeight;
	int		iFlags;
	int		iCharHeight;
	short	charWidths[256];
} SCREENINFO;


typedef struct client_data_s
{
	// fields that cannot be modified  (ie. have no effect if changed)
	vec3_t origin;

	// fields that can be changed by the cldll
	float viewheight;
	float maxspeed;
	vec3_t viewangles;
	vec3_t punchangle;
	int		iKeyBits; // Keyboard bits
	int		iWeaponBits;
	float	fov;	// field of view
} client_data_t;

typedef struct client_sprite_s
{
	char szName[64];
	char szSprite[64];
	int hspr;
	int iRes;
	wrect_t rc;
} client_sprite_t;

typedef struct
{
	int		effect;
	byte	r1, g1, b1, a1;		// 2 colors for effects
	byte	r2, g2, b2, a2;
	float	x;
	float	y;
	float	fadein;
	float	fadeout;
	float	holdtime;
	float	fxtime;
	char* pName;
	char* pMessage;
} client_textmessage_t;

typedef struct
{
	char* name;
	short ping;
	byte thisplayer;  // TRUE if this is the calling player

	// stuff that's unused at the moment,  but should be done
	byte spectator;
	byte packetloss;
} hud_player_info_t;


// this is by no means complete,  or even accurate
typedef struct cl_enginefuncs_s
{
	// sprite handlers
	HSPRITE_t (*pfnSPR_Load)			(const char* szPicName);
	int		(*pfnSPR_Frames)		(HSPRITE_t hPic);
	int		(*pfnSPR_Height)		(HSPRITE_t hPic, int frame);
	int		(*pfnSPR_Width)			(HSPRITE_t hPic, int frame);
	void	(*pfnSPR_Set)			(HSPRITE_t hPic, int r, int g, int b);
	void	(*pfnSPR_Draw)			(int frame, int x, int y, const wrect_t* prc);
	void	(*pfnSPR_DrawHoles)		(int frame, int x, int y, const wrect_t* prc);
	void	(*pfnSPR_DrawAdditive)	(int frame, int x, int y, const wrect_t* prc);
	void	(*pfnSPR_EnableScissor)	(int x, int y, int width, int height);
	void	(*pfnSPR_DisableScissor)(void);
	client_sprite_t* (*pfnSPR_GetList)		(char* psz, int* piCount);

	// screen handlers
	void	(*pfnFillRGBA)			(int x, int y, int width, int height, int r, int g, int b, int a);
	int		(*pfnGetScreenInfo)		(SCREENINFO* pscrinfo);
	void	(*pfnSetCrosshair)		(HSPRITE_t hspr, wrect_t rc, int r, int g, int b);

	// cvar handlers
	int		(*pfnRegisterVariable)  (char* szName, char* szValue, int flags);
	float	(*pfnGetCvarFloat)		(char* szName);
	char* (*pfnGetCvarString)		(char* szName);

	// command handlers
	int		(*pfnAddCommand)		(char* cmd_name, void (*function)(void));
	int		(*pfnHookUserMsg)		(char* szMsgName, pfnUserMsgHook pfn);
	int		(*pfnServerCmd)			(char* szCmdString);
	int		(*pfnClientCmd)			(char* szCmdString);

	void	(*pfnGetPlayerInfo)		(int ent_num, hud_player_info_t* pinfo);

	// sound handlers
	void	(*pfnPlaySoundByName)	(char* szSound, float volume);
	void	(*pfnPlaySoundByIndex)	(int iSound, float volume);

	// vector helpers
	void  (*pfnAngleVectors)		(const float* vecAngles, float* forward, float* right, float* up);

	// text message system
	client_textmessage_t	*(*pfnTextMessageGet)	( const char *pName );
	int	  (*pfnDrawCharacter)		( int x, int y, int number, int r, int g, int b );
	int   (*pfnDrawConsoleString)	( int x, int y, char *string );
	void  (*pfnDrawConsoleStringLen) (  const char *string, int *length, int *height );
	void  (*pfnConsolePrint) ( const char *string );
	
} cl_enginefunc_t;


// ! duplicate macro's!  really bad coding practice
// buttons
#define IN_ATTACK	(1 << 0)
#define IN_JUMP		(1 << 1)
#define IN_DUCK		(1 << 2)
#define IN_FORWARD	(1 << 3)
#define IN_BACK		(1 << 4)
#define IN_USE		(1 << 5)
#define IN_CANCEL	(1 << 6)
#define IN_LEFT		(1 << 7)
#define IN_RIGHT	(1 << 8)
#define IN_MOVELEFT	(1 << 9)
#define IN_MOVERIGHT (1 << 10)
#define IN_ATTACK2	(1 << 11)
#define IN_RUN      (1 << 12)
#define IN_RELOAD	(1 << 13)
#define IN_ALT1		(1 << 14)
#define IN_ALT2		(1 << 15)


#define CLDLL_INTERFACE_VERSION		3

extern void ClientDLL_Init(void); // from cdll_int.c
extern void ClientDLL_Shutdown(void);
extern void ClientDLL_HudInit(void);
extern void ClientDLL_HudVidInit(void);
extern void	ClientDLL_UpdateClientData(void);
extern void ClientDLL_HudRedraw(int intermission);


#endif // CDLL_INT_H
