// snd_a3d.h -- Aureal3D Sound System

#ifndef A3D_H
#define A3D_H
#pragma once

#define A3D_FAILED		-1
#define A3D_FAKE		-5

#define S_VERBWET_EPS	0.001

// Source status
#define A3D_STATUS_OFF					0
#define A3D_STATUS_NORMAL				1
#define A3D_STATUS_LOOPED				2
#define A3D_STATUS_START				3
#define A3D_STATUS_STOP					4
#define A3D_STATUS_PROCESSING_NORMAL	5
#define A3D_STATUS_PROCESSING_LOOPED	6
#define A3D_STATUS_PROCESSING_START		7

// Blip Orientation
#define A3D_BLIP_ORIENTATION_OFF		0
#define A3D_BLIP_ORIENTATION_FRONT		1
#define A3D_BLIP_ORIENTATION_BACK		2
#define A3D_BLIP_ORIENTATION_RIGHT		3
#define A3D_BLIP_ORIENTATION_LEFT		4
#define A3D_BLIP_ORIENTATION_UP			5
#define A3D_BLIP_ORIENTATION_DOWN		6

#ifdef __cplusplus
extern "C" {
#endif

extern int bA3dReloadSettings;

float hA3D_CalcNormalizedSum( int a, int b );

void* hA3D_GetDynamicSource3D( int channel );

int hA3D_GetSourceStatus( int sourceID );
int hA3D_SetSourceStatus( int sourceID, int status );

void hA3D_SetMixBufferSize( int dwSize );

int hA3D_Init( HWND hWnd, int nChannels, int nSamplesPerSec, int wBitsPerSample );
int	hA3D_Shutdown( void );

void hA3D_StopAllSounds( void );
int	hA3D_StopMixBuffer( void );
int	hA3D_StartMixBuffer( void );

void hA3D_SetSecondaryBufferSize( int size );
void hA3D_SetAutoMinMax( float fMin, float fMax );
void hA3D_SetMinMax( int fMin, int fMax );
void hA3D_SetGlobals( float fDoppler, int iDistance, float fRollover );
void hA3D_ChangeBlipOrientation( int blipOrientation );

void hA3D_UpdateListenerOrigins( void );

void hA3D_PrecacheSources( void );

// A3D Geometry

void hA3Dg_SetListenerOrigin( float* origin );
void hA3Dg_RenderGeometry( void );
void hA3Dg_AdjustLeavesRendered( int leafnum );
void hA3Dg_AdjustNumPolys( int numpolys );
void hA3Dg_AdjustPolySize( float polysize );
void hA3Dg_AdjustPolyKeep( float polykeepsize );
void hA3Dg_AdjustRefDelay( int refdelay );
void hA3Dg_AdjustRefGain( float refgain );
void hA3Dg_AdjustShowTossed( int showtossed );
void hA3Dg_AdjustUsePVS( int usepvs );
void hA3Dg_AdjustBloatFactor( float bloatfactor );
void hA3Dg_AdjustSetOcclusion( int occlusion );
void hA3Dg_AdjustSetReflection( int reflection );
void hA3Dg_AdjustTransmittance( float transmittance );
void hA3Dg_AdjustOcclusionEpsilon( float occ_epsilon );
#ifdef __cplusplus
}
#endif

#endif // A3D_H