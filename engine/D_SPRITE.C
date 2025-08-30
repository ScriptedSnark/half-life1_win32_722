// d_sprite.c: software top-level rasterization driver module for drawing
// sprites

#include "quakedef.h"
#include "d_local.h"

void (*spritedraw)(sspan_t* pspan) = D_SpriteDrawSpans;

/*
=====================
D_SpriteDrawSpans
=====================
*/
void D_SpriteDrawSpans( sspan_t* pspan )
{
	// TODO: Implement
}