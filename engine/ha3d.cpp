/*
 * Copyright (c) 1999 Aureal Semiconductor, Inc. - All rights reserved.
 *
 * This code may be used, copied or distributed in accordance with the terms
 * described in the A3D2.0 SDK License Agreement.
 *
*/

#include "quakedef.h"
#include "snd_a3d.h"

/* -------------------------------------------------------------------------- */

float hA3D_CalcNormalizedSum( int a, int b )
{
	float sum = a + b;
	return (float)(sum / 510.0);
}

int hA3D_Init( HWND hWnd, int nChannels, int nSamplesPerSec, int wBitsPerSample )
{
	// TODO: Implement
	return -1;
}

void hA3D_PrecacheSources( void )
{
	// TODO: Implement
}