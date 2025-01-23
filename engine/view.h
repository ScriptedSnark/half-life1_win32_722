// view.h
#if !defined( VIEW_H )
#define VIEW_H
#ifdef _WIN32
#pragma once
#endif

extern byte			texgammatable[256];

extern vec3_t 		r_soundOrigin;
extern vec3_t 		r_playerViewportAngles;



void V_Init( void );
void V_InitLevel( void );
void V_RenderView( void );

void V_CalcBlend( void );

void V_UpdatePalette( void );

#endif // VIEW_H