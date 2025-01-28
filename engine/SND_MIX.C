// snd_mix.c -- portable code to mix sounds for snd_dma.c

#include "quakedef.h"
#include "winquake.h"
#include "pr_cmds.h"

extern LPDIRECTSOUND pDS;
extern LPDIRECTSOUNDBUFFER pDSBuf, pDSPBuf;

extern DWORD gSndBufSize;

extern void Snd_WriteLinearBlastStereo16( void );
extern void SND_PaintChannelFrom8( channel_t* ch, sfxcache_t* sc, int count );
extern void SND_PaintChannelFrom8toDry( channel_t* ch, sfxcache_t* sc, int count );

portable_samplepair_t paintbuffer[(PAINTBUFFER_SIZE + 1) * 2];

#if defined (__USEA3D)
#include "a3d.h"
//#include "../a3dwrapper/a3dwrapperDP.h"
#endif

#if defined (__USEA3D) || defined (_ADD_EAX_)
portable_samplepair_t drybuffer[(PAINTBUFFER_SIZE + 1) * 2];
#endif

int			snd_scaletable[32][256];
int* snd_p, snd_linear_count, snd_vol;
short* snd_out;

#if defined (__USEA3D)
void S_A3DFinishChannel( int iChannel );

int PaintToA3D( int iChannel, channel_t* ch, sfxcache_t* sc, int count, int feedStart, float flPitch );
int hA3D_FeedBuffer( void* pA3D, int iChannel, channel_t* ch, sfxcache_t* sc, int count, int feedStart, float flPitch );
#endif

#if	!id386
void Snd_WriteLinearBlastStereo16( void )
{
	int		i;
	int		val;

	for (i = 0; i < snd_linear_count; i += 2)
	{
		val = (snd_p[i] * snd_vol) >> 8;
		if (val > 0x7fff)
			snd_out[i] = 0x7fff;
		else if (val < (short)0x8000)
			snd_out[i] = (short)0x8000;
		else
			snd_out[i] = val;

		val = (snd_p[i + 1] * snd_vol) >> 8;
		if (val > 0x7fff)
			snd_out[i + 1] = 0x7fff;
		else if (val < (short)0x8000)
			snd_out[i + 1] = (short)0x8000;
		else
			snd_out[i + 1] = val;
	}
}
#endif

// Transfer paintbuffer into dma buffer

void S_TransferStereo16( int end )
{
	int		lpos;
	int		lpaintedtime;
	LPVOID	pbuf;
	int		endtime;
#ifdef _WIN32
	int		reps;
	DWORD	dwSize, dwSize2;
	LPVOID	pbuf2;
	HRESULT	hresult;
#endif

	snd_vol = volume.value * 256;

	snd_p = (int*)paintbuffer;
	lpaintedtime = paintedtime << 1;
	endtime = end << 1;

#if defined (__USEA3D)
	dwSize = 0;
	if (FALSE) // TODO: Implement
	{
		// TODO: Implement
	}
	else
#endif

#ifdef _WIN32
	if (pDSBuf)
	{
		reps = 0;

		while (DS_OK != (hresult = pDSBuf->lpVtbl->Lock(pDSBuf, 0, gSndBufSize, &pbuf, &dwSize,
			&pbuf2, &dwSize2, 0)))
		{
			if (hresult != DSERR_BUFFERLOST)
			{
				Con_Printf("S_TransferStereo16: DS::Lock Sound Buffer Failed\n");
				S_Shutdown();
				S_Startup();
				return;
			}

			if (++reps > 10000)
			{
				Con_Printf("S_TransferStereo16: DS: couldn't restore buffer\n");
				S_Shutdown();
				S_Startup();
				return;
			}
		}
	}
	else
#endif
	{
		pbuf = (DWORD*)shm->buffer;
	}

	while (lpaintedtime < endtime)
	{
															// pbuf can hold 16384, 16 bit L/R samplepairs.
		// handle recirculating buffer issues				// lpaintedtime - where to start painting into dma buffer. 
															//		(modulo size of dma buffer for current position).
		lpos = lpaintedtime & ((shm->samples >> 1) - 1);	// lpos - samplepair index into dma buffer. First samplepair from paintbuffer to be xfered here.

		snd_out = (short*)pbuf + (lpos << 1);				// snd_out - L/R sample index into dma buffer.  First L sample from paintbuffer goes here.
		
		snd_linear_count = ((shm->samples >> 1) - lpos);	// snd_linear_count - number of samplepairs between end of dma buffer and xfer start index.
		if (lpaintedtime + snd_linear_count > endtime)		// make sure we write at most snd_linear_count, and at least as many samplepairs as we've premixed
			snd_linear_count = endtime - lpaintedtime;		// endtime - lpaintedtime = number of premixed sample pairs ready for xfer.

		snd_linear_count <<= 1;								// snd_linear_count is now number of 16 bit samples (L or R) to xfer.

	// write a linear blast of samples
		Snd_WriteLinearBlastStereo16();						// transfer 16bit samples from snd_p into snd_out, multiplying each sample by volume.

		snd_p += snd_linear_count;							// advance paintbuffer pointer
		lpaintedtime += (snd_linear_count >> 1);			// advance lpaintedtime by number of samplepairs just xfered.
	}

#if defined (__USEA3D)
	if (snd_isa3d)
	{
		// TODO: Implement
		return;
	}
#endif

#ifdef _WIN32
	if (pDSBuf)
		pDSBuf->lpVtbl->Unlock(pDSBuf, pbuf, dwSize, NULL, 0);
#endif
}

void S_TransferPaintBuffer( int endtime )
{
	int 	out_idx;
	int 	count;
	int 	out_mask;
	int* p;
	int 	step;
	int		val;
	int		snd_vol;
	LPVOID pbuf;
#ifdef _WIN32
	int		reps;
	DWORD	dwSize, dwSize2;
	LPVOID	pbuf2;
	HRESULT	hresult;
#endif

	if (shm->samplebits == 16 && shm->channels == 2)
	{
		S_TransferStereo16(endtime);
		return;
	}

	p = (int*)paintbuffer;
	count = (endtime - paintedtime) * shm->channels * 2;
	out_mask = shm->samples - 1;
	out_idx = ((paintedtime << 1) * shm->channels) & out_mask;
	step = 3 - shm->channels;
	snd_vol = volume.value * 256;

#if defined (__USEA3D)
	dwSize = 0;
	if (FALSE) // TODO: Implement
	{
		// TODO: Implement
	}
	else
#endif

#ifdef _WIN32
	if (pDSBuf)
	{
		reps = 0;

		while ((hresult = pDSBuf->lpVtbl->Lock(pDSBuf, 0, gSndBufSize, &pbuf, &dwSize,
			&pbuf2, &dwSize2, 0)) != DS_OK)
		{
			if (hresult != DSERR_BUFFERLOST)
			{
				Con_Printf("S_TransferPaintBuffer: DS::Lock Sound Buffer Failed\n");
				S_Shutdown();
				S_Startup();
				return;
			}

			if (++reps > 10000)
			{
				Con_Printf("S_TransferPaintBuffer: DS: couldn't restore buffer\n");
				S_Shutdown();
				S_Startup();
				return;
			}
		}
	}
	else
#endif
	{
		pbuf = (DWORD*)shm->buffer;
	}

	if (shm->samplebits == 16)
	{
		short* out = (short*)pbuf;
		while (count--)
		{
			val = (*p * snd_vol) >> 8;
			p += step;
			if (val > 0x7fff)
				val = 0x7fff;
			else if (val < (short)0x8000)
				val = (short)0x8000;
			out[out_idx] = val;
			out_idx = (out_idx + 1) & out_mask;
		}
	}
	else if (shm->samplebits == 8)
	{
		unsigned char* out = (unsigned char*)pbuf;
		while (count--)
		{
			val = (*p * snd_vol) >> 8;
			p += step;
			if (val > 0x7fff)
				val = 0x7fff;
			else if (val < (short)0x8000)
				val = (short)0x8000;
			out[out_idx] = (val >> 8) + 128;
			out_idx = (out_idx + 1) & out_mask;
		}
	}

#if defined (__USEA3D)
	if (snd_isa3d)
	{
		// TODO: Implement
		return;
	}
#endif

#ifdef _WIN32
	if (pDSBuf) {
		DWORD dwNewpos, dwWrite;
		int il = paintedtime;
		int ir = endtime - paintedtime;

		ir += il;

		pDSBuf->lpVtbl->Unlock(pDSBuf, pbuf, dwSize, NULL, 0);

		pDSBuf->lpVtbl->GetCurrentPosition(pDSBuf, &dwNewpos, &dwWrite);

//		if ((dwNewpos >= il) && (dwNewpos <= ir))
//			Con_Printf("%d-%d p %d c\n", il, ir, dwNewpos);
	}
#endif
}




// TODO: Implement


/*
===============================================================================

CHANNEL MIXING

===============================================================================
*/

// free channel so that it may be allocated by the
// next request to play a sound.  If sound is a 
// word in a sentence, release the sentence.
// Works for static, dynamic, sentence and stream sounds

void S_FreeChannel( channel_t* ch )
{
	// TODO: Implement
}


// TODO: Implement


void S_PaintChannels( int endtime )
{
	// TODO: Implement
}


// TODO: Implement


void SND_InitScaletable( void )
{
	int i, j;

	for (i = 0; i < 32; i++)
		for (j = 0; j < 256; j++)
			snd_scaletable[i][j] = ((signed char)j) * i * 8;
}


// TODO: Implement


//=====================================================================
// Init/release all structures for sound effects
//=====================================================================

void SX_Init( void )
{
	// TODO: Implement
}


// TODO: Implement


// main routine for processing room sound fx
// if fFilter is TRUE, then run in-line filter (for underwater fx)
// if fTimefx is TRUE, then run reverb and delay fx
// NOTE: only processes preset room_types from 0-29 (CSXROOM)

void SX_RoomFX( int count, int fFilter, int fTimefx )
{
	// TODO: Implement
}


// TODO: Implement


//===============================================================================
//
// WAVE Streams
// 
// Routines for CHAN_STREAM sound channel (from static/dynamic areas)
//===============================================================================

// Initialize wavestreams
qboolean Wavstream_Init( void )
{
	// TODO: Implement
	return TRUE;
}


// TODO: Implement


void SND_InitMouth( int entnum, int entchannel )
{
	// TODO: Implement
}


// TODO: Implement


void VOX_Init( void )
{
	// TODO: Implement
}


// TODO: Implement


// set channel volume based on volume of current word

void VOX_SetChanVol( channel_t* ch )
{
	// TODO: Implement
}


// TODO: Implement


void VOX_MakeSingleWordSentence( channel_t* ch, int pitch )
{
	// TODO: Implement
}

// link all sounds in sentence, start playing first word.

sfxcache_t* VOX_LoadSound( channel_t* pchan, char* pszin )
{
	// TODO: Implement
	return NULL;
}


// TODO: Implement


char* rgpszrawsentence[CVOXFILESENTENCEMAX];
int cszrawsentences;


// TODO: Implement


// trim the start and end times of a voice channel's audio data
// based on specified start and end points

void VOX_TrimStartEndTimes( channel_t* ch, sfxcache_t* sc )
{
	// TODO: Implement
}


// TODO: Implement