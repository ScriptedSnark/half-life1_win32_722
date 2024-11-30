//-----------------------------------------------------------------------------
// Quake GL DirectX wrapper
//-----------------------------------------------------------------------------

#ifndef _OPENGL2D3D_H_
#define _OPENGL2D3D_H_

#ifndef GLAPIENTRY
#  define GLAPI       __declspec(dllexport) 
#  define GLAPIENTRY  __stdcall
#endif


DLL_EXPORT void Download4444( void );

DLL_EXPORT void QGL_D3DShared( void* d3dGShared );
DLL_EXPORT HINSTANCE QGL_D3DInit( void );

#endif // _OPENGL2D3D_H_