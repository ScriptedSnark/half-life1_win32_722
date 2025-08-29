// screen.h
#if !defined( SCREEN_H )
#define SCREEN_H
#ifdef _WIN32
#pragma once
#endif

void SCR_Init( void );

void SCR_UpdateScreen( void );

void SCR_BringDownConsole( void );
void SCR_CenterPrint( char* str );

void SCR_BeginLoadingPlaque( void );
void SCR_EndLoadingPlaque( void );

int SCR_ModalMessage( char* text );

void SCR_DrawDownloadInfo( void );
void SCR_DrawDownloadProgress( void );

void Draw_CenterPic( qpic_t* pPic );

void D_FillRect( vrect_t* r, byte* color );

extern vrect_t		scr_vrect;

extern	float		scr_con_current;
extern	float		scr_conlines;		// lines of console to display
extern	qboolean	scr_drawloading;

extern	int			clearnotify;	// set to 0 whenever notify text is drawn
extern	qboolean	scr_disabled_for_loading;

// only the refresh window will be updated unless these variables are flagged 
extern	int			scr_copytop;
extern	int			scr_copyeverything;

extern	qboolean	scr_skipupdate;

extern	int			sb_lines;

extern	int			clearconsole;
extern	int			clearnotify;

extern	cvar_t		scr_viewsize;
extern	float		scr_fov_value;
extern	cvar_t		scr_graphheight;
extern	cvar_t		scr_graphhigh;
extern	cvar_t		scr_graphmean;
extern	cvar_t		scr_netusage;
extern	cvar_t		scr_graphmedian;
extern	cvar_t		scr_downloading;

#endif // SCREEN_H