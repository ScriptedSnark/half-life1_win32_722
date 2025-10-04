/*
 * verifya3d.h
 *
 *
 * Copyright (c) 1999 Aureal Semiconductor, Inc. - All rights reserved.
 *
 * This code may be used, copied or distributed in accordance with the terms
 * described in the A3D2.0 SDK License Agreement.
 *
 * A3D Verification Header File
 * 
*/

/* need to include windows.h */
#include <windows.h>

#include "../engine/snd_a3d.h"

#ifndef VERIFYA3D_H
#define VERIFYA3D_H
#pragma once

int WINAPI CheckA3DDllFileVersion( LPCSTR lptstrFilename, DWORD dwLen, LPVOID lpData );
HRESULT WINAPI VerifyAurealA3D( void );
HRESULT WINAPI A3dInitialize( void );
void WINAPI A3dUninitialize( void );
HRESULT WINAPI A3dCreate( LPGUID lpGUID, void** ppA3d, IUnknown FAR* pUnkOuter );

#endif // VERIFYA3D_H