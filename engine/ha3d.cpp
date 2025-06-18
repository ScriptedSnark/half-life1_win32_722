/*
 * Copyright (c) 1999 Aureal Semiconductor, Inc. - All rights reserved.
 *
 * This code may be used, copied or distributed in accordance with the terms
 * described in the A3D2.0 SDK License Agreement.
 *
*/

#include "quakedef.h"
#include "snd_a3d.h"

int bA3dReloadSettings = TRUE;

/* -------------------------------------------------------------------------- */

float hA3D_CalcNormalizedSum( int a, int b )
{
	float sum = a + b;
	return (float)(sum / 510.0);
}

void* hA3D_GetDynamicSource3D( int channel )
{
	// TODO: Implement
	return NULL;
}

int hA3D_GetSourceStatus( int sourceID )
{
	// TODO: Implement
	return 0;
}

int hA3D_SetSourceStatus( int sourceID, int status )
{
	// TODO: Implement
	return 0;
}

void hA3D_SetMixBufferSize( int dwSize )
{
	// TODO: Implement
}

int hA3D_Init( HWND hWnd, int nChannels, int nSamplesPerSec, int wBitsPerSample )
{
	// TODO: Implement
	return -1;
}

int	hA3D_Shutdown( void )
{
	// TODO: Implement
	return 0;
}

void hA3D_StopAllSounds( void )
{
	// TODO: Implement
}

int	hA3D_StopMixBuffer( void )
{
	// TODO: Implement
	return 0;
}

int	hA3D_StartMixBuffer( void )
{
	// TODO: Implement
	return 0;
}

void hA3D_SetSecondaryBufferSize( int size )
{
	// TODO: Implement
}

void hA3D_SetAutoMinMax( float fMin, float fMax )
{
	// TODO: Implement
}

void hA3D_SetMinMax( int fMin, int fMax )
{
	// TODO: Implement
}

void hA3D_SetGlobals( float fDoppler, int iDistance, float fRollover )
{
	// TODO: Implement
}

void hA3D_ChangeBlipOrientation( int blipOrientation )
{
	// TODO: Implement
}

void hA3D_UpdateListenerOrigins( void )
{
	// TODO: Implement
}

void hA3D_PrecacheSources( void )
{
	// TODO: Implement
}