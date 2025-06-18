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

int A3D_EnableSourceReflection( void* pA3D, int enabled );

int A3D_StartLoopingPlayback( void );

int A3D_SetMixBufferVolume( float fVolume );

int A3D_LockMixBuffer( LPVOID* lpAudioPtr1, LPDWORD lpdwAudioBytes1 );
int A3D_UnlockMixBuffer( LPVOID lpAudioPtr1, DWORD dwAudioBytes1 );
int A3D_GetMixBufferPos( LPDWORD lpdwCapturePosition, LPDWORD lpdwReadPosition );
int A3D_IsMixBuffer( void );
#ifdef __cplusplus
}
#endif

#endif // A3DWRAPPERDP_H