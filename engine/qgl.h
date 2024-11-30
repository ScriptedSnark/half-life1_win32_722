#ifndef QGL_H
#define QGL_H
#ifdef _WIN32
#pragma once
#endif

#if !defined ( GLQUAKE )
#error This should not be called by sw/swds.
#endif

#include "quakedef.h"

#ifdef WIN32
#include <Windows.h>
#else
typedef void* HDC;
typedef void* HGLRC;
#endif







HINSTANCE QGL_Init( char* pdllname );
void QGL_Shutdown( void );

void GLimp_EnableLogging( void );
void GLimp_LogNewFrame( void );

void GL_Config( void );
DLL_EXPORT void GL_Init( void );




#endif // QGL_H