#include "quakedef.h"
#include "winquake.h"

#if defined (__USEA3D)
#include "a3d.h"
//#include "../a3dwrapper/a3dwrapperDP.h"
#endif

#define iDirectSoundCreate(a,b,c)	pDirectSoundCreate(a,b,c)

typedef HRESULT(WINAPI* LPDIRECTSOUNDCREATE)(GUID FAR* lpGUID, LPDIRECTSOUND FAR* lplpDS, IUnknown FAR* pUnkOuter);
LPDIRECTSOUNDCREATE pDirectSoundCreate;

// 64K is > 1 second at 16-bit, 22050 Hz
#define	WAV_BUFFERS             64
#define	WAV_MASK				(WAV_BUFFERS - 1)
#define	WAV_BUFFER_SIZE			0x0400
#define SECONDARY_BUFFER_SIZE	0x10000

#if defined (__USEA3D)
typedef enum { SIS_SUCCESS, SIS_FAILURE, SIS_NOTAVAIL, SIS_FAKEA3D } sndinitstat;
#else
typedef enum { SIS_SUCCESS, SIS_FAILURE, SIS_NOTAVAIL } sndinitstat;
#endif

static qboolean wavonly;
static qboolean dsound_init;
static qboolean wav_init;
static qboolean snd_firsttime = TRUE, snd_isdirect, snd_iswave;
static qboolean primary_format_set;

static int snd_buffer_count;

static int sample16;
static int snd_sent, snd_completed;


/*
*	Global variables. Must be visible to window-procedure function
*	so it can unlock and free the data block after it has been played.
*/

HANDLE		hData;
HPSTR		lpData, lpData2;

HGLOBAL		hWaveHdr;
LPWAVEHDR	lpWaveHdr;

HWAVEOUT    hWaveOut;

WAVEOUTCAPS	wavecaps;

DWORD	gSndBufSize;

MMTIME		mmstarttime;

LPDIRECTSOUND pDS;
LPDIRECTSOUNDBUFFER pDSBuf, pDSPBuf;

HINSTANCE hInstDS;

sndinitstat SNDDMA_InitDirect( void );
qboolean SNDDMA_InitWav( void );

#if defined (__USEA3D)
sndinitstat SNDDMA_InitA3D( void );
#endif

extern int snd_blocked;

/*
==================
S_GetWAVPointer

Returns a pointer to pDS, if it exists, NULL otherwise
==================
*/
void* S_GetWAVPointer( void )
{
	if (hWaveOut)
		return (void*)&hWaveOut;

	return NULL;
}

/*
==================
LPDIRECTSOUND S_GetDSPointer

Returns a pointer to pDS, if it exists, NULL otherwise
==================
*/
void S_GetDSPointer( LPDIRECTSOUND* lpDS, LPDIRECTSOUNDBUFFER* lpDSBuf )
{
#if defined (__USEA3D)
	if (snd_isa3d)
		OutputDebugString("error in S_GetDSPointer - shouldn't be called with A3D.\n");
#endif

	*lpDS = pDS;
	*lpDSBuf = pDSBuf;
}

/*
==================
S_BlockSound
==================
*/
void S_BlockSound( void )
{
#if defined (__USEA3D)
	if (snd_isa3d)
	{
		// TODO: Implement
		snd_blocked = 1;
		return;
	}
#endif

// DirectSound takes care of blocking itself
	if (snd_iswave)
	{
		snd_blocked++;

		if (snd_blocked == 1)
			waveOutReset(hWaveOut);
	}

	if (pDSBuf)
		pDSBuf->lpVtbl->Stop(pDSBuf);
}

/*
==================
S_UnblockSound
==================
*/
void S_UnblockSound( void )
{
#if defined (__USEA3D)
	if (snd_isa3d)
		snd_blocked = 0;
#endif

// DirectSound takes care of blocking itself
	if (snd_iswave)
	{
		snd_blocked--;
	}

	if (pDSBuf)
		pDSBuf->lpVtbl->Play(pDSBuf, 0, 0, DSBPLAY_LOOPING);
}

/*
==================
FreeSound
==================
*/
void FreeSound( void )
{
	int	i;

#if defined (__USEA3D)
	if (snd_isa3d)
	{
		// TODO: Implement
	}
#endif

	if (pDSBuf)
	{
		pDSBuf->lpVtbl->Stop(pDSBuf);
		pDSBuf->lpVtbl->Release(pDSBuf);
	}

// only release primary buffer if it's not also the mixing buffer we just released
	if (pDSPBuf && (pDSBuf != pDSPBuf))
	{
		pDSPBuf->lpVtbl->Release(pDSPBuf);
	}

	if (pDS)
	{
		pDS->lpVtbl->SetCooperativeLevel(pDS, *pmainwindow, DSSCL_NORMAL);
		pDS->lpVtbl->Release(pDS);
	}

	if (hWaveOut)
	{
		waveOutReset(hWaveOut);

		if (lpWaveHdr)
		{
			for (i = 0; i < WAV_BUFFERS; i++)
				waveOutUnprepareHeader(hWaveOut, &lpWaveHdr[i], sizeof(WAVEHDR));
		}

		waveOutClose(hWaveOut);

		if (hWaveHdr)
		{
			GlobalUnlock(hWaveHdr);
			GlobalFree(hWaveHdr);
		}

		if (hData)
		{
			GlobalUnlock(hData);
			GlobalFree(hData);
		}
	}

	pDS = NULL;
	pDSBuf = NULL;
	pDSPBuf = NULL;
	hWaveOut = 0;
	hData = 0;
	hWaveHdr = 0;
	lpData = NULL;
	lpWaveHdr = NULL;
	dsound_init = FALSE;
	wav_init = FALSE;
}

void Snd_ReleaseBuffer( void )
{
#if defined (__USEA3D)
	if (snd_isa3d)
	{
		S_Shutdown();
		return;
	}
#endif

	if (snd_isdirect)
	{
		snd_buffer_count--;

		if (!snd_buffer_count)
		{
			S_ClearBuffer();
			S_Shutdown();
		}
	}
}

void Snd_AcquireBuffer( void )
{
#if defined (__USEA3D)
	if (snd_isa3d)
	{
		S_Startup();
		return;
	}
#endif

	if (snd_isdirect)
	{
		snd_buffer_count++;

		if (snd_buffer_count == 1)
			S_Startup();
	}
}

/*
==================
SNDDMA_InitDirect

Direct-Sound support
==================
*/
sndinitstat SNDDMA_InitDirect( void )
{
	DSBCAPS			dsbcaps;
	DWORD			dwSize, dwWrite;
	DSBUFFERDESC	dsbuf;
	DSCAPS			dscaps;
	WAVEFORMATEX	format, pformat;
	HRESULT			hresult;
	int				reps;

	memset((void*)&sn, 0, sizeof(sn));

	shm = &sn;

	shm->channels = 2;
	shm->samplebits = 16;
	shm->speed = 11025;
	shm->dmaspeed = SOUND_DMA_SPEED;

	memset(&format, 0, sizeof(format));
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = shm->channels;
	format.wBitsPerSample = shm->samplebits;
	format.nSamplesPerSec = shm->dmaspeed;
	format.nBlockAlign = format.nChannels * format.wBitsPerSample / 8;
	format.cbSize = 0;
	format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;

	if (!hInstDS)
	{
		hInstDS = (HINSTANCE)LoadLibrary("dsound.dll");

		if (!hInstDS)
		{
			Con_SafePrintf("Couldn't load dsound.dll\n");
			return SIS_FAILURE;
		}

		pDirectSoundCreate = (LPDIRECTSOUNDCREATE)GetProcAddress(hInstDS, "DirectSoundCreate");

		if (!pDirectSoundCreate)
		{
			Con_SafePrintf("Couldn't get DS proc addr\n");
			return SIS_FAILURE;
		}
	}

	while ((hresult = iDirectSoundCreate(NULL, &pDS, NULL)) != DS_OK)
	{
		if (hresult != DSERR_ALLOCATED)
		{
			Con_DPrintf("DirectSound create failed\n");
			return SIS_FAILURE;
		}

		if (MessageBox(NULL,
			"The sound hardware is in use by another app.\n\n"
			"Select Retry to try to start sound again or Cancel to run Half-Life with no sound.",
			"Sound not available",
			MB_RETRYCANCEL | MB_SETFOREGROUND | MB_ICONEXCLAMATION) != IDRETRY)
		{
			Con_SafePrintf("DirectSoundCreate failure\n  hardware already in use\n");
			return SIS_NOTAVAIL;
		}
	}

	dscaps.dwSize = sizeof(dscaps);

	if (DS_OK != pDS->lpVtbl->GetCaps(pDS, &dscaps))
	{
		Con_SafePrintf("Couldn't get DS caps\n");
	}

	if (dscaps.dwFlags & DSCAPS_EMULDRIVER)
	{
		Con_SafePrintf("No DirectSound driver installed\n");
		FreeSound();
		return SIS_FAILURE;
	}

	if (DS_OK != pDS->lpVtbl->SetCooperativeLevel(pDS, *pmainwindow, DSSCL_EXCLUSIVE))
	{
		Con_SafePrintf("Set coop level failed\n");
		FreeSound();
		return SIS_FAILURE;
	}

// get access to the primary buffer, if possible, so we can set the
// sound hardware format
	memset(&dsbuf, 0, sizeof(dsbuf));
	dsbuf.dwSize = sizeof(DSBUFFERDESC);
	dsbuf.dwFlags = DSBCAPS_PRIMARYBUFFER;
	dsbuf.dwBufferBytes = 0;
	dsbuf.lpwfxFormat = NULL;

	memset(&dsbcaps, 0, sizeof(dsbcaps));
	dsbcaps.dwSize = sizeof(dsbcaps);
	primary_format_set = FALSE;

	if (!COM_CheckParm("-snoforceformat"))
	{
		if (DS_OK == pDS->lpVtbl->CreateSoundBuffer(pDS, &dsbuf, &pDSPBuf, NULL))
		{
			pformat = format;

			if (DS_OK != pDSPBuf->lpVtbl->SetFormat(pDSPBuf, &pformat))
			{
				if (snd_firsttime)
					Con_DPrintf("Set primary sound buffer format: no\n");
			}
			else
			{
				if (snd_firsttime)
					Con_DPrintf("Set primary sound buffer format: yes\n");

				primary_format_set = TRUE;
			}
		}
	}

	if (!primary_format_set || !COM_CheckParm("-primarysound"))
	{
	// create the secondary buffer we'll actually work with
		memset(&dsbuf, 0, sizeof(dsbuf));
		dsbuf.dwSize = sizeof(DSBUFFERDESC);
		dsbuf.dwFlags = DSBCAPS_CTRLFREQUENCY | DSBCAPS_LOCSOFTWARE;
		dsbuf.dwBufferBytes = SECONDARY_BUFFER_SIZE;
		dsbuf.lpwfxFormat = &format;

		memset(&dsbcaps, 0, sizeof(dsbcaps));
		dsbcaps.dwSize = sizeof(dsbcaps);

		if (DS_OK != pDS->lpVtbl->CreateSoundBuffer(pDS, &dsbuf, &pDSBuf, NULL))
		{
			Con_SafePrintf("DS:CreateSoundBuffer Failed");
			FreeSound();
			return SIS_FAILURE;
		}

		shm->channels = format.nChannels;
		shm->samplebits = format.wBitsPerSample;
		shm->dmaspeed = format.nSamplesPerSec;

		if (DS_OK != pDSBuf->lpVtbl->GetCaps(pDSBuf, &dsbcaps))
		{
			Con_SafePrintf("DS:GetCaps failed\n");
			FreeSound();
			return SIS_FAILURE;
		}

		if (snd_firsttime)
			Con_SafePrintf("Using secondary sound buffer\n");
	}
	else
	{
		if (DS_OK != pDS->lpVtbl->SetCooperativeLevel(pDS, *pmainwindow, DSSCL_WRITEPRIMARY))
		{
			Con_SafePrintf("Set coop level failed\n");
			FreeSound();
			return SIS_FAILURE;
		}

		if (DS_OK != pDSPBuf->lpVtbl->GetCaps(pDSPBuf, &dsbcaps))
		{
			Con_Printf("DS:GetCaps failed\n");
			return SIS_FAILURE;
		}

		pDSBuf = pDSPBuf;
		Con_SafePrintf("Using primary sound buffer\n");
	}

	// Make sure mixer is active
	pDSBuf->lpVtbl->Play(pDSBuf, 0, 0, DSBPLAY_LOOPING);

	if (snd_firsttime)
		Con_SafePrintf("   %d channel(s)\n"
			"   %d bits/sample\n"
			"   %d bytes/sec\n",
			shm->channels, shm->samplebits, shm->speed);

	gSndBufSize = dsbcaps.dwBufferBytes;

// initialize the buffer
	reps = 0;

	while ((hresult = pDSBuf->lpVtbl->Lock(pDSBuf, 0, gSndBufSize, (LPVOID*)&lpData, &dwSize, NULL, NULL, 0)) != DS_OK)
	{
		if (hresult != DSERR_BUFFERLOST)
		{
			Con_SafePrintf("SNDDMA_InitDirect: DS::Lock Sound Buffer Failed\n");
			FreeSound();
			return SIS_FAILURE;
		}

		if (++reps > 10000)
		{
			Con_SafePrintf("SNDDMA_InitDirect: DS: couldn't restore buffer\n");
			FreeSound();
			return SIS_FAILURE;
		}
	}

	memset(lpData, 0, dwSize);
//		lpData[4] = lpData[5] = 0x7f;	// force a pop for debugging

	pDSBuf->lpVtbl->Unlock(pDSBuf, lpData, dwSize, NULL, 0);

	/* we don't want anyone to access the buffer directly w/o locking it first. */
	lpData = NULL;

	pDSBuf->lpVtbl->Stop(pDSBuf);
	pDSBuf->lpVtbl->GetCurrentPosition(pDSBuf, &mmstarttime.u.sample, &dwWrite);
	pDSBuf->lpVtbl->Play(pDSBuf, 0, 0, DSBPLAY_LOOPING);

	shm->soundalive = TRUE;
	shm->splitbuffer = FALSE;
	shm->samples = gSndBufSize / (shm->samplebits / 8);
	shm->samplepos = 0;
	shm->submission_chunk = 1;
	shm->buffer = (unsigned char*)lpData;
	sample16 = (shm->samplebits / 8) - 1;

	dsound_init = TRUE;

	return SIS_SUCCESS;
}

/*
==================
SNDDM_InitWav

Crappy windows multimedia base
==================
*/
qboolean SNDDMA_InitWav( void )
{
	WAVEFORMATEX  format;
	int				i;
	HRESULT			hr;

	snd_sent = 0;
	snd_completed = 0;

	shm = &sn;

	shm->channels = 2;
	shm->samplebits = 16;
	shm->speed = 11025;
	shm->dmaspeed = SOUND_DMA_SPEED;

	memset(&format, 0, sizeof(format));
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = shm->channels;
	format.wBitsPerSample = shm->samplebits;
	format.nSamplesPerSec = shm->dmaspeed;
	format.nBlockAlign = format.nChannels * format.wBitsPerSample / 8;
	format.cbSize = 0;
	format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;

	/* Open a waveform device for output using window callback. */
	while (hr = waveOutOpen(&hWaveOut, WAVE_MAPPER, &format, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR)
	{
		if (hr != MMSYSERR_ALLOCATED)
		{
			Con_DPrintf("waveOutOpen failed\n");
			return FALSE;
		}

		if (MessageBox(NULL,
			"The sound hardware is in use by another app.\n\n"
			"Select Retry to try to start sound again or Cancel to run Half-Life with no sound.",
			"Sound not available",
			MB_RETRYCANCEL | MB_SETFOREGROUND | MB_ICONEXCLAMATION) != IDRETRY)
		{
			Con_SafePrintf("waveOutOpen failure;\n  hardware already in use\n");
			return FALSE;
		}		
	}

	/*
	*	Allocate and lock memory for the waveform data. The memory
	*	for waveform data must be globally allocated with
	*	GMEM_MOVEABLE and GMEM_SHARE flags.
	*/
	gSndBufSize = WAV_BUFFERS * WAV_BUFFER_SIZE;
	hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, gSndBufSize);	
	if (!hData)
	{
		Con_SafePrintf("Sound: Out of memory.\n");
		FreeSound();
		return FALSE;
	}
	lpData = (HPSTR)GlobalLock(hData);
	if (!lpData)
	{
		Con_SafePrintf("Sound: Failed to lock.\n");
		FreeSound();
		return FALSE;
	}
	memset(lpData, 0, gSndBufSize);

	/*
	*	Allocate and lock memory for the header. This memory must
	*	also be globally allocated with GMEM_MOVEABLE and
	*	GMEM_SHARE flags.
	*/
	hWaveHdr = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE,
		(DWORD)sizeof(WAVEHDR) * WAV_BUFFERS);

	if (hWaveHdr == NULL)
	{
		Con_SafePrintf("Sound: Failed to Alloc header.\n");
		FreeSound();
		return FALSE;
	}

	lpWaveHdr = (LPWAVEHDR)GlobalLock(hWaveHdr);

	if (lpWaveHdr == NULL)
	{
		Con_SafePrintf("Sound: Failed to lock header.\n");
		FreeSound();
		return FALSE;
	}

	memset(lpWaveHdr, 0, sizeof(WAVEHDR) * WAV_BUFFERS);

	/* After allocation, set up and prepare headers. */
	for (i = 0; i < WAV_BUFFERS; i++)
	{
		lpWaveHdr[i].dwBufferLength = WAV_BUFFER_SIZE;
		lpWaveHdr[i].lpData = lpData + i * WAV_BUFFER_SIZE;

		if (waveOutPrepareHeader(hWaveOut, lpWaveHdr + i, sizeof(WAVEHDR)) !=
			MMSYSERR_NOERROR)
		{
			Con_SafePrintf("Sound: failed to prepare wave headers\n");
			FreeSound();
			return FALSE;
		}
	}

	shm->soundalive = TRUE;
	shm->splitbuffer = FALSE;
	shm->samples = gSndBufSize / (shm->samplebits / 8);
	shm->samplepos = 0;
	shm->submission_chunk = 1;
	shm->buffer = (unsigned char*)lpData;
	sample16 = (shm->samplebits / 8) - 1;

	wav_init = TRUE;

	return TRUE;
}




/*
==============
SNDDMA_Shutdown

Reset the sound device for exiting
===============
*/
void SNDDMA_Shutdown( void )
{
	// TODO: Implement
}