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

// KVS: Will be implemented later

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

int A3D_SetListenerPosition( float* origin )
{
	// TODO: Implement
	return 0;
}

int A3D_SetListenerOrientation( float* forward, float* up )
{
	// TODO: Implement
	return 0;
}

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

int A3D_SetMainVolume( float volume )
{
	// TODO: Implement
	return 0;
}

int A3D_PlayLoopedSourceAt( void* pA3D, int pos )
{
	// TODO: Implement
	return 0;
}

int A3D_StopPlayingSource( void* pA3D )
{
	// TODO: Implement
	return 0;
}

int A3D_InitializeSource( void* pA3D )
{
	// TODO: Implement
	return 0;
}

int A3D_SetSourceListenerPosition3fv( void* pA3D, float* pos )
{
	// TODO: Implement
	return 0;
}

int A3D_SetSourceVolume( void* pA3D, int volume )
{
	// TODO: Implement
	return 0;
}

int A3D_SetSourcePitch( void* pA3D, int pitch )
{
	// TODO: Implement
	return 0;
}

int A3D_EnableSourceOcclusion( void* pA3D, int enabled )
{
	// TODO: Implement
	return 0;
}

int A3D_EnableSourceReflection( void* pA3D, int enabled )
{
	// TODO: Implement
	return 0;
}

int A3D_IsSourcePlaying( void* pA3D )
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

int A3D_UnlockSource( void* pA3D, void* pvAudioPtr1, WORD audioBytes1, void* pvAudioPtr2, WORD audioBytes2 )
{
	// TODO: Implement
	return 0;
}

int A3D_SetSourceCursorPositionEx( void* pA3D, int bytes, void** pvAudioPtr1, WORD* audioBytes1, void** pvAudioPtr2, WORD* audioBytes2, int dwWriteCursor )
{
	// TODO: Implement
	return 0;
}

int A3D_GetSourceSampleBytes( void* pA3D )
{
	// TODO: Implement
	return 0;
}

int A3D_RegisterSourceAbsolutePosUpdate( void* pA3D )
{
	// TODO: Implement
	return 0;
}

int A3D_CheckForSourceCumulativeCounterOverflow( void* pA3D, int* overflowed )
{
	// TODO: Implement
	return 0;
}

int A3D_FeedSourceData( void* pA3D, byte* data, int bytes, int feedStart )
{
	// TODO: Implement
	return 0;
}

int A3D_StartSourcePlayback( void* pA3D )
{
	// TODO: Implement
	return 0;
}

int A3D_GetSourcePosition( void* pA3D )
{
	// TODO: Implement
	return 0;
}

int A3D_SetSourcePosition( void* pA3D, int pos )
{
	// TODO: Implement
	return 0;
}