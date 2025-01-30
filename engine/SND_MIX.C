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

// Transfer contents of main paintbuffer out to 
// device.  Perform volume multiply on each sample.

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
	if (ch->entchannel == CHAN_STREAM)
	{
		int i;

		i = ch - channels;

		Wavstream_Close(i);

		// Release the cache
		Cache_Free(&ch->sfx->cache);
	}

	if (ch->isentence >= 0)
	{
		rgrgvoxword[ch->isentence][0].sfx = NULL;
	}

	ch->isentence = -1;
//	Con_Printf("End sound %s\n", ch->sfx->name);

	ch->sfx = NULL;

	SND_CloseMouth(ch);
}

// S_CheckWavEnd() - check the status of a sound channel.
// Determine if we need to update the positions
// for stream sounds, release the VOX sfx cache
// if needed and reload the sentence sound itself

qboolean S_CheckWavEnd( channel_t* ch, sfxcache_t** psc, int ltime, int ichan )
{
	sfxcache_t* sc;

	sc = (*psc);

	// Check if the sound is looped and set the position and end accordingly
	if (sc->loopstart >= 0)
	{
		ch->pos = sc->loopstart;
		ch->end = sc->length + ltime - ch->pos;

		if (ch->isentence >= 0)
		{
			rgrgvoxword[ch->isentence][ch->iword].samplefrac = 0;
		}

		return FALSE;
	}

	if (ch->entchannel == CHAN_STREAM)
	{
		if (wavstreams[ichan].csamplesplayed < wavstreams[ichan].info.samples)
		{
			Wavstream_GetNextChunk(ch, ch->sfx);
			ch->pos = 0;
			ch->end = sc->length + ltime;
			return FALSE;
		}

		S_FreeChannel(ch);

#if defined (__USEA3D)
		if (snd_isa3d)
		{
			// TODO: Implement
		}
#endif
		return TRUE;
	}

	if (ch->isentence >= 0)
	{
		sfx_t* sfx;

		sfx = rgrgvoxword[ch->isentence][ch->iword].sfx;
		if (!rgrgvoxword[ch->isentence][ch->iword].fKeepCached)
		{
			Cache_Free(&sfx->cache);
		}

		ch->sfx = rgrgvoxword[ch->isentence][ch->iword + 1].sfx;
		if (ch->sfx)
		{
			sc = S_LoadSound(ch->sfx, ch);
			(*psc) = sc;

			if (sc && ch->sfx)
			{
#if defined (__USEA3D)
				if (snd_isa3d)
				{
					// TODO: Implement
				}
#endif
				ch->pos = 0;
				ch->end = ltime + sc->length;
				ch->iword++;
				VOX_TrimStartEndTimes(ch, sc);
				return FALSE;
			}
		}
	}

	S_FreeChannel(ch);
	return TRUE;
}

// Mix all channels into active paintbuffers until paintbuffer is full or 'end' is reached.
// end: time in 22khz samples to mix
// fPaintHiresSounds: set to true if we are operating with highres sounds
// voiceOnly: true if we are doing voice processings, this sets in S_PaintChannels
//		if the game isn't paused
void S_MixChannelsToPaintbuffer( int end, int fPaintHiresSounds )
{
	int			i;
	channel_t* ch;
	sfxcache_t* sc = NULL;
	int			ltime, count;
	int			pitch;
	int			timecompress;
	int			fhitend = FALSE;
	int			hires = FALSE;
	int			offset;
	int			chend;
	portable_samplepair_t* pout;

	// mix each channel into paintbuffer
	ch = channels;

	hires = fPaintHiresSounds != 0 ? TRUE : FALSE;

	for (i = 0; i < total_channels; i++, ch++)
	{
#if defined (__USEA3D)
		int A3D_good = FALSE;
		static int A3D_painted;

		if (snd_isa3d)
		{		
			A3D_painted = FALSE;
			pitch = -PITCH_NORM;

			if (ch->sfx)
			{
				sc = S_LoadSound(ch->sfx, ch);
				if (sc)
				{
					if (fPaintHiresSounds)
					{
						if (sc->speed <= shm->speed)
							continue;
					}
					else
					{
						if (sc->speed != shm->speed)
							continue;
					}

					// get playback pitch
					pitch = ch->pitch;

					if (ch->isentence >= 0)
					{
						if (rgrgvoxword[ch->isentence][ch->iword].pitch > 0)
						{
							pitch += rgrgvoxword[ch->isentence][ch->iword].pitch - PITCH_NORM;
						}
					}
				}
			}

			// If we can get this channel into 3D hardware, do it.
			if (PaintToA3D(i, ch, sc, 0, 0, pitch / 100.0))
				A3D_good = TRUE;
		}
#endif

		if (!ch->sfx)
		{
			continue;
		}

		// UNDONE: Can get away with not filling the cache until
		// we decide it should be mixed

		sc = S_LoadSound(ch->sfx, ch);

		// Don't mix sound data for sounds with zero volume. If it's a non-looping sound, 
		// just remove the sound when its volume goes to zero.

		if (!ch->leftvol && !ch->rightvol)
		{
			// NOTE: Since we've loaded the sound, check to see if it's a sentence.  Play them at zero anyway
			// to keep the character's lips moving and the captions happening.
			if (sc->loopstart < 0) // non-looped sound, we can free it.
			{
				S_FreeChannel(ch);
			}

			continue; // don't mix it
		}

		if (!sc)
			continue;

		if (fPaintHiresSounds)
		{
			if (sc->speed <= shm->speed)
				continue;
		}
		else
		{
			if (sc->speed != shm->speed)
				continue;
		}

		ltime = paintedtime;

		// get playback pitch
		pitch = ch->pitch;

		if (ch->isentence < 0)
		{
			timecompress = 0;
		}
		else
		{
			if (rgrgvoxword[ch->isentence][ch->iword].pitch > 0)
			{
				pitch += rgrgvoxword[ch->isentence][ch->iword].pitch - PITCH_NORM;
			}

			timecompress = rgrgvoxword[ch->isentence][ch->iword].timecompress;
		}

		// do this until we reach the end 
		fhitend = FALSE;
		while (ltime < end)
		{
			// See if painting highres sounds
			if (hires)
				chend = (sc->length >> 1) + ch->end - sc->length;
			else
				chend = ch->end;

			if (chend < end)
				count = chend - ltime;
			else
				count = end - ltime;

			if (hires)
				count <<= 1;

			offset = (ltime - paintedtime);

			if (hires)
				offset <<= 1;

			if (count > 0)
			{
				if (sc->width == 1 && ch->entnum > 0)
				{
					if (ch->entchannel == CHAN_VOICE || ch->entchannel == CHAN_STREAM)
					{
						SND_MoveMouth(ch, sc, count);
					}
				}

#if defined (__USEA3D)
				// No good, gotta use 100% software...
				if (A3D_good)
				{
					pout = drybuffer;

					if (s_verbwet.value < S_VERBWET_EPS)
					{
						ch->pos += count;
						goto A3D_SkipPainting;
					}
				}
				else
#endif
				{
					pout = paintbuffer;
				}

				if (sc->width == 1)
				{
					if ((pitch == PITCH_NORM && !timecompress) || ch->isentence < 0)
					{
						if (paintedtime == ltime)
						{
							if (pout == paintbuffer)
							{
								SND_PaintChannelFrom8(ch, sc, count);
							}
							else
							{
#if defined(__USEA3D)
								SND_PaintChannelFrom8toDry(ch, sc, count);
#else
								SND_PaintChannelFrom8(ch, sc, count);
#endif
							}
						}
						else
						{
							SND_PaintChannelFrom8Offs(pout, ch, sc, count, offset);
						}
					}
					else
					{
						fhitend = VOX_FPaintPitchChannelFrom8Offs(pout, ch, sc, count, pitch, timecompress, offset);
					}
				}
				else
				{
					SND_PaintChannelFrom16Offs(pout, ch, sc, count, offset);
				}

#if defined (__USEA3D)
				// No need to paint dry buffer
				A3D_SkipPainting:
#endif
				if (hires)
					ltime += count >> 1;
				else
					ltime += count;

				if (ch->entchannel == CHAN_STREAM)
				{
					wavstreams[i].csamplesplayed += count;
				}
			}

			if (fhitend || ltime >= chend)
			{
				fhitend = FALSE;
				if (S_CheckWavEnd(ch, &sc, ltime, i))
				{
					break;
				}
			}
		}
	}
}

#define AVG(a,b) (((a) + (b)) >> 1)

void S_PaintChannels( int endtime )
{
	int		end;
	int		count;
	static portable_samplepair_t paintprev = { 0, 0 };
#if defined (__USEA3D)
	static portable_samplepair_t drypaintprev = { 0, 0 };
#endif

#if defined (__USEA3D)
	if (snd_isa3d)
	{
//		if (paintedtime >= endtime) TODO: Implement
//			hA3D_StopAllSounds(paintedtime); TODO: Implement
	}
#endif

	while (paintedtime < endtime)
	{
		// mix a full 'paintbuffer' of sound

		// clamp at paintbuffer size

		end = endtime;
		if (end - paintedtime > PAINTBUFFER_SIZE)
			end = paintedtime + PAINTBUFFER_SIZE;

		// number of 11khz samples to mix into paintbuffer, up to paintbuffer size

		count = end - paintedtime;

		// clear all mix buffers

		Q_memset(paintbuffer, 0, sizeof(portable_samplepair_t) * count);

#if defined (__USEA3D)
		if (s_verbwet.value >= S_VERBWET_EPS)
		{
			Q_memset(drybuffer, 0, sizeof(portable_samplepair_t) * count);
		}
#endif

		// upsample all mix buffers
		// results in 11khz versions of:

		S_MixChannelsToPaintbuffer(end, FALSE);

		// upsample by 2x, optionally using interpolation

		int i, j;
		int count2x = count << 1;

		if (!hisound.value)
			SX_RoomFX(count, TRUE, TRUE);

#if defined (__USEA3D)
		if (snd_isa3d && s_verbwet.value >= S_VERBWET_EPS)
		{
			// reverse through buffer, duplicating contents for 'count' samples

			for (i = count2x - 1, j = count - 1; j > 0; i -= 2, j--)
			{
				// use linear interpolation for upsampling

				paintbuffer[i].left = paintbuffer[j].left;
				paintbuffer[i].right = paintbuffer[j].right;

				paintbuffer[i - 1].left = AVG(paintbuffer[j].left, paintbuffer[j - 1].left);
				paintbuffer[i - 1].right = AVG(paintbuffer[j].right, paintbuffer[j - 1].right);

				drybuffer[i].left = drybuffer[j].left;
				drybuffer[i].right = drybuffer[j].right;

				drybuffer[i - 1].left = AVG(drybuffer[j].left, drybuffer[j - 1].left);
				drybuffer[i - 1].right = AVG(drybuffer[j].right, drybuffer[j - 1].right);
			}

			paintbuffer[1].left = paintbuffer[0].left;
			paintbuffer[1].right = paintbuffer[0].right;

			drybuffer[1].left = drybuffer[0].left;
			drybuffer[1].right = drybuffer[0].right;

			// use interpolation value from previous mix

			paintbuffer[0].left = AVG(paintbuffer[0].left, paintprev.left);
			paintbuffer[0].right = AVG(paintbuffer[0].right, paintprev.right);

			drybuffer[0].left = AVG(drybuffer[0].left, drypaintprev.left);
			drybuffer[0].right = AVG(drybuffer[0].right, drypaintprev.right);

			// save last value to be played out in buffer
			drypaintprev.left = drybuffer[count2x - 1].left;
			drypaintprev.right = drybuffer[count2x - 1].right;
		}
		else
#endif
		{
			// reverse through buffer, duplicating contents for 'count' samples

			for (i = count2x - 1, j = count - 1; j > 0; i -= 2, j--)
			{
				// use linear interpolation for upsampling

				paintbuffer[i].left = paintbuffer[j].left;
				paintbuffer[i].right = paintbuffer[j].right;

				paintbuffer[i - 1].left = AVG(paintbuffer[j].left, paintbuffer[j - 1].left);
				paintbuffer[i - 1].right = AVG(paintbuffer[j].right, paintbuffer[j - 1].right);
			}

			paintbuffer[1].left = paintbuffer[0].left;
			paintbuffer[1].right = paintbuffer[0].right;

			// use interpolation value from previous mix

			paintbuffer[0].left = AVG(paintbuffer[0].left, paintprev.left);
			paintbuffer[0].right = AVG(paintbuffer[0].right, paintprev.right);
		}

		// save last value to be played out in buffer
		paintprev.left = paintbuffer[count2x - 1].left;
		paintprev.right = paintbuffer[count2x - 1].right;

		if (hisound.value > 0.0)
		{
			S_MixChannelsToPaintbuffer(end, TRUE); // mix hires
			SX_RoomFX(count2x, TRUE, TRUE);
		}

		// transfer out according to DMA format
		S_TransferPaintBuffer(end);
		paintedtime = end;
	}
}

void SND_InitScaletable( void )
{
	int i, j;

	for (i = 0; i < 32; i++)
		for (j = 0; j < 256; j++)
			snd_scaletable[i][j] = ((signed char)j) * i * 8;
}


#if	!id386

// 8-bit sound-mixing code
void SND_PaintChannelFrom8( channel_t* ch, sfxcache_t* sc, int count )
{
	int		data;
	int* lscale, * rscale;
	unsigned char* sfx;
	int		i;

	if (ch->leftvol > 255)
		ch->leftvol = 255;
	if (ch->rightvol > 255)
		ch->rightvol = 255;

	lscale = snd_scaletable[ch->leftvol >> 3];
	rscale = snd_scaletable[ch->rightvol >> 3];
	sfx = sc->data + ch->pos;

	for (i = 0; i < count; i++)
	{
		data = sfx[i];
		paintbuffer[i].left += lscale[data];
		paintbuffer[i].right += rscale[data];
	}

	ch->pos += count;
}

#if	defined (__USEA3D)
void SND_PaintChannelFrom8toDry( channel_t* ch, sfxcache_t* sc, int count )
{
	int 	data;
	int* lscale, * rscale;
	unsigned char* sfx;
	int		i;

	if (ch->leftvol > 255)
		ch->leftvol = 255;
	if (ch->rightvol > 255)
		ch->rightvol = 255;

	lscale = snd_scaletable[ch->leftvol >> 3];
	rscale = snd_scaletable[ch->rightvol >> 3];
	sfx = sc->data + ch->pos;

	for (i = 0; i < count; i++)
	{
		data = sfx[i];
		drybuffer[i].left += lscale[data];
		drybuffer[i].right += rscale[data];
	}

	ch->pos += count;
}

#endif	// __USEA3D

#endif	// !id386

void SND_PaintChannelFrom8Offs( portable_samplepair_t* paintbuffer, channel_t* ch, sfxcache_t* sc, int count, int offset )
{
	int		data;
	int* lscale, * rscale;
	unsigned char* sfx;
	int		i;
	portable_samplepair_t* pbuffer;

	pbuffer = &paintbuffer[offset];

	if (ch->leftvol > 255)
		ch->leftvol = 255;
	if (ch->rightvol > 255)
		ch->rightvol = 255;

	lscale = snd_scaletable[ch->leftvol >> 3];
	rscale = snd_scaletable[ch->rightvol >> 3];
	sfx = sc->data + ch->pos;

	for (i = 0; i < count; i++)
	{
		data = sfx[i];
		pbuffer[i].left += lscale[data];
		pbuffer[i].right += rscale[data];
	}

	ch->pos += count;
}

void SND_PaintChannelFrom16Offs( portable_samplepair_t* paintbuffer, channel_t* ch, sfxcache_t* sc, int count, int offset )
{
	int		data;
	int		left, right;
	int		leftvol, rightvol;
	short* sfx;
	int		i;
	portable_samplepair_t* pbuffer;

	pbuffer = &paintbuffer[offset];

	leftvol = ch->leftvol;
	rightvol = ch->rightvol;
	sfx = (short*)&sc->data[ch->pos * 2];

	for (i = 0; i < count; i++)
	{
		data = sfx[i];
		left = leftvol * data;
		right = rightvol * data;
		pbuffer[i].left += left >> 8;
		pbuffer[i].right += right >> 8;
	}

	ch->pos += count;
}

//===============================================================================
//
// Digital Signal Processing algorithms for audio FX.
//
// KellyB 1/24/97
//===============================================================================


#define SXDLY_MAX		0.400							// max delay in seconds
#define SXRVB_MAX		0.100							// max reverb reflection time
#define SXSTE_MAX		0.100							// max stereo delay line time

typedef short sample_t;									// delay lines must be 32 bit, now that we have 16 bit samples

typedef struct dlyline_s {
	int cdelaysamplesmax;								// size of delay line in samples
	int lp;												// lowpass flag 0 = off, 1 = on

	int idelayinput;									// i/o indices into circular delay line
	int idelayoutput;

	int idelayoutputxf;									// crossfade output pointer
	int xfade;											// crossfade value

	int delaysamples;									// current delay setting
	int delayfeed;										// current feedback setting

	int lp0, lp1, lp2;									// lowpass filter buffer

	int mod;											// sample modulation count
	int modcur;

	HANDLE hdelayline;									// handle to delay line buffer
	sample_t* lpdelayline;								// buffer
} dlyline_t;

#define CSXDLYMAX		4

#define ISXMONODLY		0								// mono delay line
#define ISXRVB			1								// first of the reverb delay lines
#define CSXRVBMAX		2
#define ISXSTEREODLY	3								// 50ms left side delay

dlyline_t rgsxdly[CSXDLYMAX];							// array of delay lines

#define gdly0 (rgsxdly[ISXMONODLY])
#define gdly1 (rgsxdly[ISXRVB])
#define gdly2 (rgsxdly[ISXRVB + 1])
#define gdly3 (rgsxdly[ISXSTEREODLY])

#define CSXLPMAX		10								// lowpass filter memory

int rgsxlp[CSXLPMAX];

int sxamodl, sxamodr;									// amplitude modulation values
int sxamodlt, sxamodrt;									// modulation targets
int sxmod1, sxmod2;
int sxmod1cur, sxmod2cur;

// Mono Delay parameters
cvar_t sxdly_delay = { "room_delay", "0" };				// current delay in seconds
cvar_t sxdly_feedback = { "room_feedback", "0.2" };		// cyles
cvar_t sxdly_lp = { "room_dlylp", "1.0" };				// lowpass filter

float sxdly_delayprev;									// previous delay setting value

// Mono Reverb parameters

cvar_t sxrvb_size = { "room_size", "0" };				// room size 0 (off) 0.1 small - 0.35 huge
cvar_t sxrvb_feedback = { "room_refl", "0.7" };			// reverb decay 0.1 short - 0.9 long
cvar_t sxrvb_lp = { "room_rvblp", "1.0" };				// lowpass filter

float sxrvb_sizeprev;

// Stereo delay (no feedback)

cvar_t sxste_delay = { "room_left", "0" };				// straight left delay
float sxste_delayprev;

// Underwater/special fx modulations

cvar_t sxmod_lowpass = { "room_lp", "0" };
cvar_t sxmod_mod = { "room_mod", "0" };

// Main interface

cvar_t sxroom_type = { "room_type", "0" };
cvar_t sxroomwater_type = { "waterroom_type", "14" };
float sxroom_typeprev;

cvar_t sxroom_off = { "room_off", "0" };

int sxhires = 0;
int sxhiresprev = 0;

qboolean SXDLY_Init( int idelay, float delay );
void SXDLY_Free( int idelay );
void SXDLY_DoDelay( int count );
void SXRVB_DoReverb( int count );
void SXDLY_DoStereoDelay( int count );
void SXRVB_DoAMod( int count );

//=====================================================================
// Init/release all structures for sound effects
//=====================================================================

void SX_Init( void )
{
	Q_memset(rgsxdly, 0, sizeof(dlyline_t) * CSXDLYMAX);
	Q_memset(rgsxlp, 0, sizeof(int) * CSXLPMAX);

	sxdly_delayprev = -1.0;
	sxrvb_sizeprev = -1.0;
	sxste_delayprev = -1.0;
	sxroom_typeprev = -1.0;

	// flag hires sound mode
	sxhires = (hisound.value == 0 ? 0 : 1);
	sxhiresprev = sxhires;

	// init amplitude modulation params

	sxamodl = sxamodr = 255;
	sxamodlt = sxamodrt = 255;

	sxmod1 = 350 * (shm->speed / SOUND_11k);	// 11k was the original sample rate all dsp was tuned at
	sxmod2 = 450 * (shm->speed / SOUND_11k);
	sxmod1cur = sxmod1;
	sxmod2cur = sxmod2;

	Con_DPrintf("\nFX Processor Initialization\n");

	Cvar_RegisterVariable(&sxdly_delay);
	Cvar_RegisterVariable(&sxdly_feedback);
	Cvar_RegisterVariable(&sxdly_lp);

	Cvar_RegisterVariable(&sxrvb_size);
	Cvar_RegisterVariable(&sxrvb_feedback);
	Cvar_RegisterVariable(&sxrvb_lp);

	Cvar_RegisterVariable(&sxste_delay);

	Cvar_RegisterVariable(&sxmod_lowpass);
	Cvar_RegisterVariable(&sxmod_mod);

	Cvar_RegisterVariable(&sxroom_type);
	Cvar_RegisterVariable(&sxroomwater_type);
	Cvar_RegisterVariable(&sxroom_off);
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

// Close wavestream files
void Wavstream_Close( int i )
{
	// TODO: Implement
}

// Move to the next wave chunk
void Wavstream_GetNextChunk( channel_t* ch, sfx_t* s )
{
	// TODO: Implement
}


// TODO: Implement


void SND_InitMouth( int entnum, int entchannel )
{
	// TODO: Implement
}

void SND_CloseMouth( channel_t* ch )
{
	// TODO: Implement
}

#define CAVGSAMPLES 10

void SND_MoveMouth( channel_t* ch, sfxcache_t* sc, int count )
{
	// TODO: Implement
}


// TODO: Implement

//===============================================================================
// VOX. Algorithms to load and play spoken text sentences from a file:
//
// In ambient sounds or entity sounds, precache the 
// name of the sentence instead of the wave name, ie: !C1A2S4
//
// During sound system init, the 'sentences.txt' is read.
// This file has the format:
//
//		HG_ALERT0 hgrunt/(t30) squad!, we!(e80) got!(e80) freeman!(t20 p105), clik(p110)
//      HG_ALERT1 hgrunt/clik(p110) target! clik
//		...
//
//		There must be at least one space between the sentence name and the sentence.
//		Sentences may be separated by one or more lines
//		There may be tabs or spaces preceding the sentence name
//		The sentence must end in a /n or /r
//		Lines beginning with // are ignored as comments
//
//		Period or comma will insert a pause in the wave unless
//		the period or comma is the last character in the string.
//
//		If first 2 chars of a word are upper case, word volume increased by 25%
// 
//		If last char of a word is a number from 0 to 9
//		then word will be pitch-shifted up by 0 to 9, where 0 is a small shift
//		and 9 is a very high pitch shift.
//
// We alloc heap space to contain this data, and track total 
// sentences read.  A pointer to each sentence is maintained in rgpszrawsentence.
//
// When sound is played back in S_StartDynamicSound or s_startstaticsound, we detect the !name
// format and lookup the actual sentence in the sentences array
//
// To play, we parse each word in the sentence, chain the words, and play the sentence
// each word's data is loaded directy from disk and freed right after playback.
//===============================================================================

// Module Locals
static char*	rgpparseword[CVOXWORDMAX];	// array of pointers to parsed words
static char		voxperiod[] = "_period";				// vocal pause
static char		voxcomma[] = "_comma";				// vocal pause

voxword_t rgrgvoxword[CBSENTENCENAME_MAX][CVOXWORDMAX];

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

int	VOX_FPaintPitchChannelFrom8Offs( portable_samplepair_t* paintbuffer, channel_t* ch, sfxcache_t* sc, int count, int pitch, int timecompress, int offset )
{
	// TODO: Implement
	return 0;
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


#if defined (__USEA3D)
int PaintToA3D( int iChannel, channel_t* ch, sfxcache_t* sc, int count, int feedStart, float flPitch )
{
	// TODO: Implement
	return FALSE;
}
#endif

// TODO: Implement