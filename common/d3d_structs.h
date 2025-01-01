#ifndef D3D_STRUCTS_H
#define D3D_STRUCTS_H
#pragma once

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct LPDIRECT3D3;

typedef struct tagD3DGlobals
{
	BOOL bCalled;
	BOOL bFullscreen;
	BOOL bSecondary;
	LPDIRECTDRAW4           lpDD4;          // DirectDraw object
	LPDIRECTDRAWSURFACE     lpDDSPrimary;   // DirectDraw primary surface
	LPDIRECTDRAWSURFACE     lpDDSBack;      // DirectDraw back surface
	LPDIRECTDRAWSURFACE     lpDDSMem;		// DirectDraw mem surface
	LPDIRECT3D3				lpD3D;
}D3DGLOBALS;

//D3DGLOBALS d3dG;
//

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // D3D_STRUCTS_H