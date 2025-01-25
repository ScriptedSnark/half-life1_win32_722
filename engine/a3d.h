// a3d.h -- Aureal3D Sound System

#ifndef A3D_H
#define A3D_H
#pragma once

#define A3D_FAILED		-1
#define A3D_FAKE		-5

int hA3D_Init( HWND hWnd, int nChannels, int nSamplesPerSec, int wBitsPerSample );

void hA3D_PrecacheSources( void );

#endif // A3D_H