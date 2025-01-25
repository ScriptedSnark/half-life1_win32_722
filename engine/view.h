// view.h
#if !defined( VIEW_H )
#define VIEW_H
#ifdef _WIN32
#pragma once
#endif

extern vec3_t 		r_soundOrigin;
extern vec3_t 		r_playerViewportAngles;

extern byte			texgammatable[256];
extern int 			lightgammatable[1024];
extern int			lineargammatable[1024];
extern int			screengammatable[1024];

void V_Init( void );
void V_InitLevel( void );
void V_RenderView( void );

int V_FadeAlpha( void );
void V_CalcBlend( void );

void V_UpdatePalette( void );

#endif // VIEW_H