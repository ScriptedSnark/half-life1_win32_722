// gl_vidnt.c -- NT GL vid component

#include "quakedef.h"
#include "winquake.h"

qboolean		scr_skipupdate;


// TODO: Implement

viddef_t	vid;				// global video state


int			window_center_x, window_center_y;
RECT		window_rect;


/*
=================
GL_Init

=================
*/
DLL_EXPORT void GL_Init( void )
{
	// TODO: Implement
}


DLL_EXPORT int GL_SetMode( HWND mainwindow, HDC* pmaindc, HGLRC* pbaseRC, int fD3D, char* pszDriver )
{
	// TODO: Implement
	return 1;
}

DLL_EXPORT void GL_Shutdown( HWND hwnd, HDC hdc, HGLRC hglrc )
{
	// TODO: Implement
}




DLL_EXPORT void VID_UpdateWindowVars( void* prc, int x, int y )
{
	RECT* rect = (RECT*)prc;
	window_rect.left = rect->left;
	window_rect.top = rect->top;
	window_rect.right = rect->right;
	window_rect.bottom = rect->bottom;
	window_center_x = x;
	window_center_y = y;
}

DLL_EXPORT void VID_UpdateVID( viddef_t* pvid )
{
	vid = *pvid;
}

DLL_EXPORT int VID_AllocBuffers( void )
{
	return TRUE;
}