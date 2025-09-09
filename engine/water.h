// water.h
#ifndef WATER_H
#define WATER_H
#ifdef _WIN32
#pragma once
#endif

extern word* gWaterLastPalette;

void D_SetScreenFade( int r, int g, int b, int alpha, int type );

void WaterTextureUpdate( unsigned short* pPalette, float dropTime, texture_t* texture );

#endif // WATER_H