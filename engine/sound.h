// sound.h -- client sound i/o functions

void S_Init( void );

DLL_EXPORT void S_ClearBuffer( void );


// shutdown the DMA xfer.
DLL_EXPORT void SNDDMA_Shutdown( void );



DLL_EXPORT void Snd_ReleaseBuffer( void );
DLL_EXPORT void Snd_AcquireBuffer( void );



void S_LocalSound( char* s );