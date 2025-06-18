/*
 * a3dwrapper.cpp
 *
 *
 * Copyright (c) 1999 Aureal Semiconductor, Inc. - All rights reserved.
 *
 * This code may be used, copied or distributed in accordance with the terms
 * described in the A3D2.0 SDK License Agreement.
 *
*/

#define INITGUID

#include <objbase.h>
#include <stdlib.h>
#include <cguid.h>

/* need to include windows.h */
#include <windows.h>

#include "a3dwrapper.h"


/* -------------------------------------------------------------------------- */

// DEBUGGING

const char* pA3DDebugString;

void debugSetName( const char* debugString )
{
	pA3DDebugString = debugString;
}

void debugOutputString( void )
{
	if (pA3DDebugString)
		OutputDebugString(pA3DDebugString);
	else
		OutputDebugString("NULL");
}

// A3D Wrapper

int A3D_Flush( void )
{
	// TODO: Implement
	return 0;
}

int A3D_Clear( void )
{
	// TODO: Implement
	return 0;
}

int A3D_StartLoopingPlayback( void )
{
	// TODO: Implement
	return 0;
}

int A3D_SetMixBufferVolume( float fVolume )
{
	// TODO: Implement
	return 0;
}

int A3D_LockMixBuffer( LPVOID* lpAudioPtr1, LPDWORD lpdwAudioBytes1 )
{
	// TODO: Implement
	return 0;
}

int A3D_UnlockMixBuffer( LPVOID lpAudioPtr1, DWORD dwAudioBytes1 )
{
	// TODO: Implement
	return 0;
}

int A3D_GetMixBufferPos( LPDWORD lpdwCapturePosition, LPDWORD lpdwReadPosition )
{
	// TODO: Implement
	return 0;
}

int A3D_IsMixBuffer( void )
{
	// TODO: Implement
	return 0;
}