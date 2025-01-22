// cl_draw.h
#if !defined( CL_DRAW_H )
#define CL_DRAW_H
#ifdef _WIN32
#pragma once
#endif

// Sprites
void	SPR_Init( void );

void	SetCrosshair( HSPRITE_t hspr, wrect_t rc, int r, int g, int b );
void	DrawCrosshair( int x, int y );

HSPRITE_t SPR_Load( const char* pTextureName );
int		SPR_Frames( HSPRITE_t hSprite );
int		SPR_Width( HSPRITE_t hSprite, int frame );
int		SPR_Height( HSPRITE_t hSprite, int frame );
void	SPR_Set( HSPRITE_t hSprite, int r, int g, int b );
void	SPR_Draw( int frame, int x, int y, const wrect_t* prc );
void	SPR_DrawHoles( int frame, int x, int y, const wrect_t* prc );
void	SPR_DrawAdditive( int frame, int x, int y, const wrect_t* prcSubRect );

// Scissor test
void	SPR_EnableScissor( int x, int y, int width, int height );
void	SPR_DisableScissor( void );

#endif // CL_DRAW_H