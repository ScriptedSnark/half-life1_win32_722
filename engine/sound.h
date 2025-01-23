// sound.h -- client sound i/o functions
#ifndef SOUND_H
#define SOUND_H
#ifdef _WIN32
#pragma once
#endif

void S_Init( void );
void S_Shutdown( void );
void S_StopAllSounds( qboolean clear );
DLL_EXPORT void S_ClearBuffer( void );
void S_Update( vec_t* origin, vec_t* forward, vec_t* right, vec_t* up );
void S_ExtraUpdate( void );


// shutdown the DMA xfer.
DLL_EXPORT void SNDDMA_Shutdown( void );



DLL_EXPORT void Snd_ReleaseBuffer( void );
DLL_EXPORT void Snd_AcquireBuffer( void );



void S_LocalSound( char* s );

#endif // SOUND_H