// screen.h
#if !defined( SCREEN_H )
#define SCREEN_H
#ifdef _WIN32
#pragma once
#endif

void SCR_Init( void );

void SCR_UpdateScreen( void );


void SCR_EndLoadingPlaque( void );

extern	float		scr_con_current;




extern	int			clearnotify;	// set to 0 whenever notify text is drawn
extern	qboolean	scr_disabled_for_loading;




// only the refresh window will be updated unless these variables are flagged 
extern	int			scr_copytop;
extern	int			scr_copyeverything;

extern	qboolean	scr_skipupdate;

extern	int			clearconsole;
extern	int			clearnotify;

#endif // SCREEN_H