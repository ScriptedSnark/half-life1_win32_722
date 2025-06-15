// snd_a3d.h -- Aureal3D Sound System

#ifndef A3D_H
#define A3D_H
#pragma once

#define A3D_FAILED		-1
#define A3D_FAKE		-5

#define S_VERBWET_EPS	0.001

#ifdef __cplusplus
extern "C" {
#endif
float hA3D_CalcNormalizedSum( int a, int b );



int hA3D_Init( HWND hWnd, int nChannels, int nSamplesPerSec, int wBitsPerSample );

void hA3D_PrecacheSources( void );
#ifdef __cplusplus
}
#endif

#endif // A3D_H