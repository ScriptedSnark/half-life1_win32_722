/*
 * a3dwrapper.h
 *
 *
 * Copyright (c) 1999 Aureal Semiconductor, Inc. - All rights reserved.
 *
 * This code may be used, copied or distributed in accordance with the terms
 * described in the A3D2.0 SDK License Agreement.
 *
*/

#ifndef A3DWRAPPERDP_H
#define A3DWRAPPERDP_H
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// DEBUGGING
void debugSetName( const char* debugString );
void debugOutputString( void );

// A3D Wrapper
int A3D_SetListenerPosition( float* origin );
int A3D_SetListenerOrientation( float* forward, float* up );

int A3D_Flush( void );
int A3D_Clear( void );

int A3D_SetMainVolume( float volume );

int A3D_PlayLoopedSourceAt( void* pA3D, int pos );
int A3D_StopPlayingSource( void* pA3D );
int A3D_InitializeSource( void* pA3D );
int A3D_SetSourceListenerPosition3fv( void* pA3D, float* pos );
int A3D_SetSourceVolume( void* pA3D, int volume );
int A3D_SetSourcePitch( void* pA3D, int pitch );
int A3D_EnableSourceOcclusion( void* pA3D, int enabled );
int A3D_EnableSourceReflection( void* pA3D, int enabled );
int A3D_IsSourcePlaying( void* pA3D );

int A3D_StartLoopingPlayback( void );

int A3D_SetMixBufferVolume( float fVolume );

int A3D_LockMixBuffer( LPVOID* lpAudioPtr1, LPDWORD lpdwAudioBytes1 );
int A3D_UnlockMixBuffer( LPVOID lpAudioPtr1, DWORD dwAudioBytes1 );
int A3D_GetMixBufferPos( LPDWORD lpdwCapturePosition, LPDWORD lpdwReadPosition );
int A3D_IsMixBuffer( void );

int A3D_UnlockSource( void* pA3D, void* pvAudioPtr1, WORD audioBytes1, void* pvAudioPtr2, WORD audioBytes2 );
int A3D_SetSourceCursorPositionEx( void* pA3D, int bytes, void** pvAudioPtr1, WORD* audioBytes1, void** pvAudioPtr2, WORD* audioBytes2, int dwWriteCursor );

int A3D_GetSourceSampleBytes( void* pA3D );

int A3D_RegisterSourceAbsolutePosUpdate( void* pA3D );
int A3D_CheckForSourceCumulativeCounterOverflow( void* pA3D, int* overflowed );

int A3D_FeedSourceData( void* pA3D, byte* data, int bytes, int feedStart );

int A3D_StartSourcePlayback( void* pA3D );

int A3D_GetSourcePosition( void* pA3D );
int A3D_SetSourcePosition( void* pA3D, int pos );

#ifdef __cplusplus
}
#endif

#endif // A3DWRAPPERDP_H