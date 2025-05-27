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

// hard clip input value to -32767 <= y <= 32767
#define CLIP(x) ((x) > 32767 ? 32767 : ((x) < -32767 ? -32767 : (x)))

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
		int i, j;
		int count2x;
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

		count2x = count << 1;

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

void SX_Free( void ) {
	int i;

	// release mono delay line

	SXDLY_Free(ISXMONODLY);

	// release reverb lines

	for (i = 0; i < CSXRVBMAX; i++)
		SXDLY_Free(i + ISXRVB);

	SXDLY_Free(ISXSTEREODLY);
}

// Set up a delay line buffer allowing a max delay of 'delay' seconds 
// Frees current buffer if it already exists. idelay indicates which of 
// the available delay lines to init.

typedef char* HPSTR;


qboolean SXDLY_Init( int idelay, float delay ) {
	int cbsamples;
	HANDLE		hData;
	HPSTR		lpData;
	dlyline_t* pdly;

	pdly = &(rgsxdly[idelay]);

	if (delay > SXDLY_MAX)
		delay = SXDLY_MAX;

	if (pdly->lpdelayline) {
		GlobalUnlock(pdly->hdelayline);
		GlobalFree(pdly->hdelayline);
		pdly->hdelayline = NULL;
		pdly->lpdelayline = NULL;
	}

	if (delay == 0.0)
		return TRUE;

	pdly->cdelaysamplesmax = (int)(shm->speed * delay)
		<< sxhires; // << 1 for hires
	pdly->cdelaysamplesmax += 1;

	cbsamples = pdly->cdelaysamplesmax * sizeof(sample_t);
	hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, cbsamples);
	if (!hData)
	{
		Con_SafePrintf("Sound FX: Out of memory.\n");
		return FALSE;
	}

	lpData = (HPSTR)GlobalLock(hData);
	if (!lpData)
	{
		Con_SafePrintf("Sound FX: Failed to lock.\n");
		GlobalFree(hData);
		return FALSE;
	}

	memset(lpData, 0, cbsamples);

	pdly->hdelayline = hData;
	pdly->lpdelayline = (sample_t*)lpData;

	// init delay loop input and output counters.

	// NOTE: init of idelayoutput only valid if pdly->delaysamples is set
	// NOTE: before this call!

	pdly->idelayinput = 0;
	pdly->idelayoutput = pdly->cdelaysamplesmax - pdly->delaysamples;
	pdly->xfade = 0;
	pdly->lp = 1;
	pdly->mod = 0;
	pdly->modcur = 0;

	// init lowpass filter memory

	pdly->lp0 = pdly->lp1 = pdly->lp2 = 0;

	return TRUE;
}

// release delay buffer and deactivate delay

void SXDLY_Free( int idelay ) {
	dlyline_t* pdly = &(rgsxdly[idelay]);

	if (pdly->lpdelayline) {
		GlobalUnlock(pdly->hdelayline);
		GlobalFree(pdly->hdelayline);
		pdly->hdelayline = NULL;
		pdly->lpdelayline = NULL;				// this deactivates the delay
	}
}


// check for new stereo delay param

void SXDLY_CheckNewStereoDelayVal() {
	dlyline_t* pdly = &(rgsxdly[ISXSTEREODLY]);
	int delaysamples;

	// set up stereo delay

	if (sxste_delay.value != sxste_delayprev) {
		if (sxste_delay.value == 0.0) {
			
			// deactivate delay line

			SXDLY_Free(ISXSTEREODLY);
			sxste_delayprev = 0.0;

		} else {

			delaysamples = (int)(min(sxste_delay.value, SXSTE_MAX) * shm->speed)
				<< sxhires; // << 1 for hires

			// init delay line if not active

			if (pdly->lpdelayline == NULL) {

				pdly->delaysamples = delaysamples;

				SXDLY_Init(ISXSTEREODLY, SXSTE_MAX);
			}

			// do crossfade to new delay if delay has changed

			if (delaysamples != pdly->delaysamples) {

				// set up crossfade from old pdly->delaysamples to new delaysamples

				pdly->idelayoutputxf = pdly->idelayinput - delaysamples;

				if (pdly->idelayoutputxf < 0)
					pdly->idelayoutputxf += pdly->cdelaysamplesmax;

				pdly->xfade = 128;
			}

			sxste_delayprev = sxste_delay.value;

			// UNDONE: modulation disabled
			//pdly->mod = 500 * (shm->speed / SOUND_11k);		// change delay every n samples
			pdly->mod = 0;
			pdly->modcur = pdly->mod;

			// deactivate line if rounded down to 0 delay

			if (pdly->delaysamples == 0)
				SXDLY_Free(ISXSTEREODLY);

		}
	}
}

// stereo delay, left channel only, no feedback

void SXDLY_DoStereoDelay( int count ) {
	int left;
	sample_t sampledly;
	sample_t samplexf;
	portable_samplepair_t* pbuf;
	int countr;

	// process delay line if active

	if (rgsxdly[ISXSTEREODLY].lpdelayline) {
		
		pbuf = paintbuffer;
		countr = count;

		// process each sample in the paintbuffer...

		while (countr--) {
			
			if (gdly3.mod && (--gdly3.modcur < 0))
				gdly3.modcur = gdly3.mod;

			// get delay line sample from left line

			sampledly = *(gdly3.lpdelayline + gdly3.idelayoutput);
			left = pbuf->left;

			// only process if left value or delayline value are non-zero or xfading

			if (gdly3.xfade || sampledly || left) {
				
				// if we're not crossfading, and we're not modulating, but we'd like to be modulating,
				// then setup a new crossfade.

				if (!gdly3.xfade && !gdly3.modcur && gdly3.mod) {
					
					// set up crossfade to new delay value, if we're not already doing an xfade

					//gdly3.idelayoutputxf = gdly3.idelayoutput + 
					//		((RandomLong(0,0x7FFF) * gdly3.delaysamples) / (RAND_MAX * 2)); // 100 = ~ 9ms

					gdly3.idelayoutputxf = gdly3.idelayoutput +
						((RandomLong(0, 0xFF) * gdly3.delaysamples) >> 9); // 100 = ~ 9ms

					if (gdly3.idelayoutputxf >= gdly3.cdelaysamplesmax)
						gdly3.idelayoutputxf -= gdly3.cdelaysamplesmax;

					gdly3.xfade = 128;
				}

				// modify sampledly if crossfading to new delay value

				if (gdly3.xfade) {
					samplexf = (*(gdly3.lpdelayline + gdly3.idelayoutputxf) * (128 - gdly3.xfade)) >> 7;
					sampledly = ((sampledly * gdly3.xfade) >> 7) + samplexf;

					if (++gdly3.idelayoutputxf >= gdly3.cdelaysamplesmax)
						gdly3.idelayoutputxf = 0;

					if (--gdly3.xfade == 0)
						gdly3.idelayoutput = gdly3.idelayoutputxf;
				}

				// save output value into delay line

				left = CLIP(left);

				*(gdly3.lpdelayline + gdly3.idelayinput) = left;

				// save delay line sample into output buffer
				pbuf->left = sampledly;

			}
			else
			{
				// keep clearing out delay line, even if no signal in or out

				*(gdly3.lpdelayline + gdly3.idelayinput) = 0;
			}

			// update delay buffer pointers

			if (++gdly3.idelayinput >= gdly3.cdelaysamplesmax)
				gdly3.idelayinput = 0;

			if (++gdly3.idelayoutput >= gdly3.cdelaysamplesmax)
				gdly3.idelayoutput = 0;

			pbuf++;
		}

	}
}

// If sxdly_delay or sxdly_feedback have changed, update delaysamples
// and delayfeed values.  This applies only to delay 0, the main echo line.

void SXDLY_CheckNewDelayVal() {
	dlyline_t* pdly = &(rgsxdly[ISXMONODLY]);

	if (sxdly_delay.value != sxdly_delayprev) {
		
		if (sxdly_delay.value == 0.0) {
			
			// deactivate delay line

			SXDLY_Free(ISXMONODLY);
			sxdly_delayprev = sxdly_delay.value;

		} else {
			// init delay line if not active

			pdly->delaysamples = (int)(min(sxdly_delay.value, SXDLY_MAX) * shm->speed)
				<< sxhires; // << 1 for hires
			
			if (pdly->lpdelayline == NULL)
				SXDLY_Init(ISXMONODLY, SXDLY_MAX);

			// flush delay line and filters

			if (pdly->lpdelayline) {
				Q_memset(pdly->lpdelayline, 0, pdly->cdelaysamplesmax * sizeof(sample_t));
				pdly->lp0 = 0;
				pdly->lp1 = 0;
				pdly->lp2 = 0;
			}

			// init delay loop input and output counters

			pdly->idelayinput = 0;
			pdly->idelayoutput = pdly->cdelaysamplesmax - pdly->delaysamples;

			sxdly_delayprev = sxdly_delay.value;

			// deactivate line if rounded down to 0 delay

			if (pdly->delaysamples == 0)
				SXDLY_Free(ISXMONODLY);

		}
	}

	pdly->lp = (int)(sxdly_lp.value);
	pdly->delayfeed = sxdly_feedback.value * 255;
}


// This routine updates both left and right output with 
// the mono delayed signal.  Delay is set through console vars room_delay
// and room_feedback.

#define RVB_XFADE	 32	// xfade time between new delays
#define RVB_MODRATE1 (500 * (shm->speed / SOUND_11k))	// how often, in samples, to change delay (1st rvb)
#define RVB_MODRATE2 (700 * (shm->speed / SOUND_11k))	// how often, in samples, to change delay (2nd rvb)

void SXDLY_DoDelay( int count ) {
	int val;
	int valt;
	int left;
	int right;
	sample_t sampledly;
	portable_samplepair_t* pbuf;
	int countr;


	// process mono delay line if active

	if (rgsxdly[ISXMONODLY].lpdelayline)
	{
		pbuf = paintbuffer;
		countr = count;

		// process each sample in the paintbuffer...

		while (countr--)
		{

			// get delay line sample

			sampledly = *(gdly0.lpdelayline + gdly0.idelayoutput);

			left = pbuf->left;
			right = pbuf->right;

			// only process if delay line and paintbuffer samples are non zero

			if (sampledly || left || right)
			{
				// get current sample from delay buffer

				// calculate delayed value from avg of left and right channels

				val = ((left + right) >> 1) + ((gdly0.delayfeed * sampledly) >> 8);

				// limit val to short
				val = CLIP(val);

				// lowpass

				if (gdly0.lp)
				{
					//valt = (gdly0.lp0 + gdly0.lp1 + val) / 3;  // performance
					valt = (gdly0.lp0 + gdly0.lp1 + (val << 1)) >> 2;

					gdly0.lp0 = gdly0.lp1;
					gdly0.lp1 = val;
				}
				else
				{
					valt = val;
				}

				// store delay output value into output buffer

				*(gdly0.lpdelayline + gdly0.idelayinput) = valt;

				// mono delay in left and right channels

				pbuf->left = CLIP((valt >> 2) + left);
				pbuf->right = CLIP((valt >> 2) + right);
			}
			else
			{
				// not playing samples, but must still flush lowpass buffer and delay line
				valt = gdly0.lp0 = gdly0.lp1 = 0;

				*(gdly0.lpdelayline + gdly0.idelayinput) = valt;

			}

			// update delay buffer pointers

			if (++gdly0.idelayinput >= gdly0.cdelaysamplesmax)
				gdly0.idelayinput = 0;

			if (++gdly0.idelayoutput >= gdly0.cdelaysamplesmax)
				gdly0.idelayoutput = 0;

			pbuf++;
		}
	}
}

// Check for a parameter change on the reverb processor

void SXRVB_CheckNewReverbVal() {
	dlyline_t* pdly;
	int delaysamples;
	int i;
	int mod;	

	if (sxrvb_size.value != sxrvb_sizeprev)
	{
		sxrvb_sizeprev = sxrvb_size.value;

		if (sxrvb_size.value == 0.0)
		{
			// deactivate all delay lines

			SXDLY_Free(ISXRVB);
			SXDLY_Free(ISXRVB + 1);

		}
		else
		{

			for (i = ISXRVB; i < ISXRVB + CSXRVBMAX; i++)
			{
				// init delay line if not active

				pdly = &(rgsxdly[i]);

				switch (i) {
					case ISXRVB:
						delaysamples = (int)(min(sxrvb_size.value, SXRVB_MAX) * shm->speed) << sxhires; // << 1 for hires
						pdly->mod = RVB_MODRATE1 << sxhires; // << 1 for hires
						break;
					case ISXRVB + 1:
						delaysamples = (int)(min(sxrvb_size.value * 0.71, SXRVB_MAX) * shm->speed) << sxhires; // << 1 for hires
						pdly->mod = RVB_MODRATE2 << sxhires; // << 1 for hires
						break;
					default:
						delaysamples = 0;
						break;
				}

				mod = pdly->mod;				// KB: bug, SXDLY_Init clears mod, modcur, xfade and lp - save mod before call

				if (!pdly->lpdelayline)
				{
					pdly->delaysamples = delaysamples;

					SXDLY_Init(i, SXRVB_MAX);
				}

				pdly->modcur = pdly->mod = mod;	// KB: bug, SXDLY_Init clears mod, modcur, xfade and lp - restore mod after call

				// do crossfade to new delay if delay has changed

				if (delaysamples != pdly->delaysamples)
				{
					// set up crossfade from old pdly->delaysamples to new delaysamples

					pdly->idelayoutputxf = pdly->idelayinput - delaysamples;

					if (pdly->idelayoutputxf < 0)
						pdly->idelayoutputxf += pdly->cdelaysamplesmax;

					pdly->xfade = RVB_XFADE;
				}

				// deactivate line if rounded down to 0 delay

				if (pdly->delaysamples == 0)
					SXDLY_Free(i);
			}
		}
	}

	rgsxdly[ISXRVB].delayfeed = (sxrvb_feedback.value) * 255;
	rgsxdly[ISXRVB].lp = sxrvb_lp.value;

	rgsxdly[ISXRVB + 1].delayfeed = (sxrvb_feedback.value) * 255;
	rgsxdly[ISXRVB + 1].lp = sxrvb_lp.value;

}


// main routine for updating the paintbuffer with new reverb values.
// This routine updates both left and right lines with 
// the mono reverb signal.  Delay is set through console vars room_reverb
// and room_feedback.  2 reverbs operating in parallel.

void SXRVB_DoReverb( int count ) {
	int val;
	int valt;
	int left;
	int right;
	sample_t sampledly;
	sample_t samplexf;
	portable_samplepair_t* pbuf;
	int countr;
	int voutm;
	int vlr;

	// process reverb lines if active

	if (rgsxdly[ISXRVB].lpdelayline)
	{
		pbuf = paintbuffer;
		countr = count;

		// process each sample in the paintbuffer...

		while (countr--)
		{

			left = pbuf->left;
			right = pbuf->right;
			voutm = 0;
			vlr = (left + right) >> 1;

			// UNDONE: ignored
			if (--gdly1.modcur < 0)
				gdly1.modcur = gdly1.mod;

			// ========================== ISXRVB============================	

			// get sample from delay line

			sampledly = *(gdly1.lpdelayline + gdly1.idelayoutput);

			// only process if something is non-zero

			if (gdly1.xfade || sampledly || left || right)
			{
				// modulate delay rate
				if (!gdly1.xfade && !gdly1.mod)
				{
					// set up crossfade to new delay value, if we're not already doing an xfade

					//gdly1.idelayoutputxf = gdly1.idelayoutput + 
					//		((RandomLong(0, 0x7FFF) * gdly1.delaysamples) / (RAND_MAX * 2)); // performance

					gdly1.idelayoutputxf = gdly1.idelayoutput +
						((RandomLong(0, 0xFF) * gdly1.delaysamples) >> 9); // 100 = ~ 9ms

					if (gdly1.idelayoutputxf >= gdly1.cdelaysamplesmax)
						gdly1.idelayoutputxf -= gdly1.cdelaysamplesmax;

					gdly1.xfade = RVB_XFADE;
				}

				// modify sampledly if crossfading to new delay value

				if (gdly1.xfade)
				{
					samplexf = (*(gdly1.lpdelayline + gdly1.idelayoutputxf) * (RVB_XFADE - gdly1.xfade)) / RVB_XFADE;
					sampledly = ((sampledly * gdly1.xfade) / RVB_XFADE) + samplexf;

					if (++gdly1.idelayoutputxf >= gdly1.cdelaysamplesmax)
						gdly1.idelayoutputxf = 0;

					if (--gdly1.xfade == 0)
						gdly1.idelayoutput = gdly1.idelayoutputxf;
				}

				if (sampledly)
				{
					// get current sample from delay buffer

					// calculate delayed value from avg of left and right channels

					val = vlr + ((gdly1.delayfeed * sampledly) >> 8);

					// limit to short
					val = CLIP(val);

				}
				else
				{
					val = vlr;
				}

				// lowpass

				if (gdly1.lp)
				{
					valt = (val + gdly1.lp0) >> 1;
					gdly1.lp0 = val;
				}
				else
				{
					valt = val;
				}

				// store delay output value into output buffer

				*(gdly1.lpdelayline + gdly1.idelayinput) = valt;

				voutm = valt;
			}
			else
			{
				// not playing samples, but still must flush lowpass buffer & delay line

				gdly1.lp0 = gdly1.lp1 = 0;
				*(gdly1.lpdelayline + gdly1.idelayinput) = 0;

				voutm = 0;
			}

			// update delay buffer pointers

			if (++gdly1.idelayinput >= gdly1.cdelaysamplesmax)
				gdly1.idelayinput = 0;

			if (++gdly1.idelayoutput >= gdly1.cdelaysamplesmax)
				gdly1.idelayoutput = 0;

			// ========================== ISXRVB + 1========================

			// UNDONE: ignored
			if (--gdly2.modcur < 0)
				gdly2.modcur = gdly2.mod;

			if (gdly2.lpdelayline)
			{
				// get sample from delay line

				sampledly = *(gdly2.lpdelayline + gdly2.idelayoutput);

				// only process if something is non-zero

				if (gdly2.xfade || sampledly || left || right)
				{
					// modulate delay rate
					if (!gdly2.xfade && !gdly2.mod)
					{
						// set up crossfade to new delay value, if we're not already doing an xfade

						//gdly2.idelayoutputxf = gdly2.idelayoutput + 
						//		((RandomLong(0, RAND_MAX) * gdly2.delaysamples) / (RAND_MAX * 2)); // performance

						gdly2.idelayoutputxf = gdly2.idelayoutput +
							((RandomLong(0, 0xFF) * gdly2.delaysamples) >> 9); // 100 = ~ 9ms


						if (gdly2.idelayoutputxf >= gdly2.cdelaysamplesmax)
							gdly2.idelayoutputxf -= gdly2.cdelaysamplesmax;

						gdly2.xfade = RVB_XFADE;
					}

					// modify sampledly if crossfading to new delay value

					if (gdly2.xfade)
					{
						samplexf = (*(gdly2.lpdelayline + gdly2.idelayoutputxf) * (RVB_XFADE - gdly2.xfade)) / RVB_XFADE;
						sampledly = ((sampledly * gdly2.xfade) / RVB_XFADE) + samplexf;

						if (++gdly2.idelayoutputxf >= gdly2.cdelaysamplesmax)
							gdly2.idelayoutputxf = 0;

						if (--gdly2.xfade == 0)
							gdly2.idelayoutput = gdly2.idelayoutputxf;
					}

					if (sampledly)
					{
						// get current sample from delay buffer

						// calculate delayed value from avg of left and right channels

						val = vlr + ((gdly2.delayfeed * sampledly) >> 8);

						// limit to short
						val = CLIP(val);	
					}
					else
					{
						val = vlr;
					}

					// lowpass

					if (gdly2.lp)
					{
						valt = (val + gdly2.lp0) >> 1;
						gdly2.lp0 = val;
					}
					else
					{
						valt = val;
					}

					// store delay output value into output buffer

					*(gdly2.lpdelayline + gdly2.idelayinput) = valt;

					voutm += valt;
				}
				else
				{
					// not playing samples, but still must flush lowpass buffer

					gdly2.lp0 = gdly2.lp1 = 0;
					*(gdly2.lpdelayline + gdly2.idelayinput) = 0;
				}

				// update delay buffer pointers

				if (++gdly2.idelayinput >= gdly2.cdelaysamplesmax)
					gdly2.idelayinput = 0;

				if (++gdly2.idelayoutput >= gdly2.cdelaysamplesmax)
					gdly2.idelayoutput = 0;
			}

			// ============================ Mix================================

			// add mono delay to left and right channels

			// drop output by inverse of cascaded gain for both reverbs
			voutm = (11 * voutm) >> 6;

			left += voutm;
			right += voutm;

			pbuf->left = CLIP(left);
			pbuf->right = CLIP(right);

			pbuf++;
		}
	}
}

// amplitude modulator, low pass filter for underwater weirdness

void SXRVB_DoAMod( int count ) {
	
	int valtl, valtr;
	int left;
	int right;
	portable_samplepair_t* pbuf;
	int countr;
	int fLowpass;
	int fmod;

	// process reverb lines if active

	if (sxmod_lowpass.value != 0.0 || sxmod_mod.value != 0.0)
	{
		pbuf = paintbuffer;
		countr = count;

		fLowpass = (sxmod_lowpass.value != 0.0);
		fmod = (sxmod_mod.value != 0.0);

		// process each sample in the paintbuffer...

		while (countr--) {
			
			left = pbuf->left;
			right = pbuf->right;

			// only process if non-zero

			if (fLowpass) {
				
				valtl = left;
				valtr = right;

				left = (rgsxlp[0] + rgsxlp[1] + rgsxlp[2] + rgsxlp[3] + rgsxlp[4] + left);
				right = (rgsxlp[5] + rgsxlp[6] + rgsxlp[7] + rgsxlp[8] + rgsxlp[9] + right);

				left = left >> 2;
				right = right >> 2;

				rgsxlp[4] = valtl;
				rgsxlp[9] = valtr;

				rgsxlp[0] = rgsxlp[1];
				rgsxlp[1] = rgsxlp[2];
				rgsxlp[2] = rgsxlp[3];
				rgsxlp[3] = rgsxlp[4];
				rgsxlp[4] = rgsxlp[5];
				rgsxlp[5] = rgsxlp[6];
				rgsxlp[6] = rgsxlp[7];
				rgsxlp[7] = rgsxlp[8];
				rgsxlp[8] = rgsxlp[9];

			}


			if (fmod) {
				if (--sxmod1cur < 0)
					sxmod1cur = sxmod1;

				if (!sxmod1)
					sxamodlt = RandomLong(32, 255);

				if (--sxmod2cur < 0)
					sxmod2cur = sxmod2;

				if (!sxmod2)
					sxamodlt = RandomLong(32, 255);

				left = (left * sxamodl) >> 8;
				right = (right * sxamodr) >> 8;

				if (sxamodl < sxamodlt)
					sxamodl++;
				else if (sxamodl > sxamodlt)
					sxamodl--;

				if (sxamodr < sxamodrt)
					sxamodr++;
				else if (sxamodr > sxamodrt)
					sxamodr--;
			}

			left = CLIP(left);
			right = CLIP(right);

			pbuf->left = left;
			pbuf->right = right;

			pbuf++;
		}
	}
}


typedef struct sx_preset_s {
	float room_lp;					// for water fx, lowpass for entire room
	float room_mod;					// stereo amplitude modulation for room

	float room_size;				// reverb: initial reflection size
	float room_refl;				// reverb: decay time
	float room_rvblp;				// reverb: low pass filtering level

	float room_delay;				// mono delay: delay time
	float room_feedback;			// mono delay: decay time
	float room_dlylp;				// mono delay: low pass filtering level

	float room_left;				// left channel delay time
} sx_preset_t;

sx_preset_t rgsxpre[CSXROOM] = {

// SXROOM_OFF					0

//	lp		mod		size	refl	rvblp	delay	feedbk	dlylp	left  
	{0.0,	0.0,	0.0,	0.0,	1.0,	0.0,	0.0,	2.0,	0.0},

// SXROOM_GENERIC				1		// general, low reflective, diffuse room

//	lp		mod		size	refl	rvblp	delay	feedbk	dlylp	left  
	{0.0,	0.0,	0.0,	0.0,	1.0,	0.065,	0.1,	0.0,	0.01},

// SXROOM_METALIC_S				2		// highly reflective, parallel surfaces
// SXROOM_METALIC_M				3
// SXROOM_METALIC_L				4

//	lp		mod		size	refl	rvblp	delay	feedbk	dlylp	left  
	{0.0,	0.0,	0.0,	0.0,	1.0,	0.02,	0.75,	0.0,	0.01}, // 0.001
	{0.0,	0.0,	0.0,	0.0,	1.0,	0.03,	0.78,	0.0,	0.02}, // 0.002
	{0.0,	0.0,	0.0,	0.0,	1.0,	0.06,	0.77,	0.0,	0.03}, // 0.003


// SXROOM_TUNNEL_S				5		// resonant reflective, long surfaces
// SXROOM_TUNNEL_M				6
// SXROOM_TUNNEL_L				7

//	lp		mod		size	refl	rvblp	delay	feedbk	dlylp	left  
	{0.0,	0.0,	0.05,	0.85,	1.0,	0.008,	0.96,	2.0,	0.01}, // 0.01
	{0.0,	0.0,	0.05,	0.88,	1.0,	0.010,	0.98,	2.0,	0.02}, // 0.02
	{0.0,	0.0,	0.05,	0.92,	1.0,	0.015,	0.995,	2.0,	0.04}, // 0.04

// SXROOM_CHAMBER_S				8		// diffuse, moderately reflective surfaces
// SXROOM_CHAMBER_M				9
// SXROOM_CHAMBER_L				10

//	lp		mod		size	refl	rvblp	delay	feedbk	dlylp	left  
	{0.0,	0.0,	0.05,	0.84,	1.0,	0.0,	0.0,	2.0,	0.012}, // 0.003
	{0.0,	0.0,	0.05,	0.90,	1.0,	0.0,	0.0,	2.0,	0.008}, // 0.002
	{0.0,	0.0,	0.05,	0.95,	1.0,	0.0,	0.0,	2.0,	0.004}, // 0.001

// SXROOM_BRITE_S				11		// diffuse, highly reflective
// SXROOM_BRITE_M				12
// SXROOM_BRITE_L				13

//	lp		mod		size	refl	rvblp	delay	feedbk	dlylp	left  
	{0.0,	0.0,	0.05,	0.7,	0.0,	0.0,	0.0,	2.0,	0.012}, // 0.003
	{0.0,	0.0,	0.055,	0.78,	0.0,	0.0,	0.0,	2.0,	0.008}, // 0.002
	{0.0,	0.0,	0.05,	0.86,	0.0,	0.0,	0.0,	2.0,	0.002}, // 0.001

// SXROOM_WATER1				14		// underwater fx
// SXROOM_WATER2				15
// SXROOM_WATER3				16

//	lp		mod		size	refl	rvblp	delay	feedbk	dlylp	left  
	{1.0,	0.0,	0.0,	0.0,	1.0,	0.0,	0.0,	2.0,	0.01},
	{1.0,	0.0,	0.0,	0.0,	1.0,	0.06,	0.85,	2.0,	0.02},
	{1.0,	0.0,	0.0,	0.0,	1.0,	0.2,	0.6,	2.0,	0.05},

// SXROOM_CONCRETE_S			17		// bare, reflective, parallel surfaces
// SXROOM_CONCRETE_M			18
// SXROOM_CONCRETE_L			19

//	lp		mod		size	refl	rvblp	delay	feedbk	dlylp	left  
	{0.0,	0.0,	0.05,	0.8,	1.0,	0.0,	0.48,	2.0,	0.016}, // 0.15 delay, 0.008 left
	{0.0,	0.0,	0.06,	0.9,	1.0,	0.0,	0.52,	2.0,	0.01 }, // 0.22 delay, 0.005 left
	{0.0,	0.0,	0.07,	0.94,	1.0,	0.3,	0.6,	2.0,	0.008}, // 0.001

// SXROOM_OUTSIDE1				20		// echoing, moderately reflective
// SXROOM_OUTSIDE2				21		// echoing, dull
// SXROOM_OUTSIDE3				22		// echoing, very dull

//	lp		mod		size	refl	rvblp	delay	feedbk	dlylp	left  
	{0.0,	0.0,	0.0,	0.0,	1.0,	0.3,	0.42,	2.0,	0.0},
	{0.0,	0.0,	0.0,	0.0,	1.0,	0.35,	0.48,	2.0,	0.0},
	{0.0,	0.0,	0.0,	0.0,	1.0,	0.38,	0.6,	2.0,	0.0},

// SXROOM_CAVERN_S				23		// large, echoing area
// SXROOM_CAVERN_M				24
// SXROOM_CAVERN_L				25

//	lp		mod		size	refl	rvblp	delay	feedbk	dlylp	left  
	{0.0,	0.0,	0.05,	0.9,	1.0,	0.2,	0.28,	0.0,	0.0},
	{0.0,	0.0,	0.07,	0.9,	1.0,	0.3,	0.4,	0.0,	0.0},
	{0.0,	0.0,	0.09,	0.9,	1.0,	0.35,	0.5,	0.0,	0.0},

// SXROOM_WEIRDO1				26	
// SXROOM_WEIRDO2				27
// SXROOM_WEIRDO3				28
// SXROOM_WEIRDO3				29

//	lp		mod		size	refl	rvblp	delay	feedbk	dlylp	left  
	{0.0,	1.0,	0.01,	0.9,	0.0,	0.0,	0.0,	2.0,	0.05},
	{0.0,	0.0,	0.0,	0.0,	1.0,	0.009,	0.999,	2.0,	0.04},
	{0.0,	0.0,	0.001,	0.999,	0.0,	0.2,	0.8,	2.0,	0.05}

};

// force next call to sx_roomfx to reload all room parameters.
// used when switching to/from hires sound mode.

void SX_ReloadRoomFX()
{
	// reset all roomtype parms

	sxroom_typeprev = -1.0;

	sxdly_delayprev = -1.0;
	sxrvb_sizeprev = -1.0;
	sxste_delayprev = -1.0;
	sxroom_typeprev = -1.0;

	// UNDONE: handle sxmod and mod parms?
}


// main routine for processing room sound fx
// if fFilter is TRUE, then run in-line filter (for underwater fx)
// if fTimefx is TRUE, then run reverb and delay fx
// NOTE: only processes preset room_types from 0-29 (CSXROOM)

void SX_RoomFX( int count, int fFilter, int fTimefx )
{
	int fReset;
	int i;
	float roomType;

	// return right away if fx processing is turned off

	if (sxroom_off.value != 0.0)
		return;

	// detect changes in hires sound param

	sxhires = (hisound.value == 0 ? 0 : 1);

	if (sxhires != sxhiresprev)
	{
		SX_ReloadRoomFX();
		sxhiresprev = sxhires;
	}

	fReset = FALSE;
	if (cl.waterlevel > 2)
		roomType = sxroomwater_type.value;
	else
		roomType = sxroom_type.value;

	if (roomType != sxroom_typeprev)
	{

		//Con_Printf("Room_type: %2.1f\n", roomType);

		sxroom_typeprev = roomType;

		i = (int)(roomType);

		if (i < CSXROOM && i >= 0)
		{
			// Set hardcoded values from rgsxpre table
			Cvar_SetValue("room_lp", rgsxpre[i].room_lp);
			Cvar_SetValue("room_mod", rgsxpre[i].room_mod);
			Cvar_SetValue("room_size", rgsxpre[i].room_size);
			Cvar_SetValue("room_refl", rgsxpre[i].room_refl);
			Cvar_SetValue("room_rvblp", rgsxpre[i].room_rvblp);
			Cvar_SetValue("room_delay", rgsxpre[i].room_delay);
			Cvar_SetValue("room_feedback", rgsxpre[i].room_feedback);
			Cvar_SetValue("room_dlylp", rgsxpre[i].room_dlylp);
			Cvar_SetValue("room_left", rgsxpre[i].room_left);
		}

		SXRVB_CheckNewReverbVal();
		SXDLY_CheckNewDelayVal();
		SXDLY_CheckNewStereoDelayVal();

		fReset = TRUE;
	}

	if (fReset || roomType != 0.0)
	{
		// debug code
		SXRVB_CheckNewReverbVal();
		SXDLY_CheckNewDelayVal();
		SXDLY_CheckNewStereoDelayVal();
		// debug code

		if (fFilter)
			SXRVB_DoAMod(count);

		if (fTimefx)
		{
#if defined (__USEA3D)
			if (snd_isa3d)
			{
				// Add the reverb value in
				if (s_verbwet.value >= S_VERBWET_EPS)
				{
					for (i = 0; i < count; i++)
					{
						paintbuffer[i].left += drybuffer[i].left * s_verbwet.value;
						paintbuffer[i].right += drybuffer[i].right * s_verbwet.value;
					}
				}
			}
#endif

			SXRVB_DoReverb(count);
			SXDLY_DoDelay(count);

#if defined (__USEA3D)
			if (snd_isa3d)
			{
				// Restore it back to original
				if (s_verbwet.value >= S_VERBWET_EPS)
				{
					for (i = 0; i < count; i++)
					{
						paintbuffer[i].left -= drybuffer[i].left * s_verbwet.value;
						paintbuffer[i].right -= drybuffer[i].right * s_verbwet.value;
					}
				}
			}
#endif

			SXDLY_DoStereoDelay(count);
		}
	}
}

//===============================================================================
//
// WAVE Streams
// 
// Routines for CHAN_STREAM sound channel (from static/dynamic areas)
//===============================================================================

// Initialize wavestreams
qboolean Wavstream_Init( void )
{
	int i;
	for (i = 0; i < MAX_CHANNELS; i++)
	{
		Q_memset(&wavstreams[i], 0, sizeof(wavstreams[i]));
		wavstreams[i].hFile[2] = -1;
	}

	return TRUE;
}

// Close wavestream files
void Wavstream_Close( int i )
{
	// Close the file
	if (wavstreams[i].hFile[2] != -1)
		COM_CloseFile(wavstreams[i].hFile[0], wavstreams[i].hFile[1], wavstreams[i].hFile[2]);

	Q_memset(&wavstreams[i], 0, sizeof(wavstreams[i]));
	wavstreams[i].hFile[2] = -1;
}

// Move to the next wave chunk
void Wavstream_GetNextChunk( channel_t* ch, sfx_t* s )
{
	byte* data;
	sfxcache_t* sc;
	int cbread;
	int i = ch - channels;

	sc = (sfxcache_t*)Cache_Check(&s->cache);

	wavstreams[i].lastposloaded = wavstreams[i].info.dataofs + wavstreams[i].csamplesplayed;

	data = COM_LoadFileLimit(NULL, wavstreams[i].lastposloaded, 0x8000, &cbread, wavstreams[i].hFile);

	wavstreams[i].csamplesinmem = cbread;
	if (wavstreams[i].csamplesinmem > wavstreams[i].info.samples - wavstreams[i].csamplesplayed)
		wavstreams[i].csamplesinmem = wavstreams[i].info.samples - wavstreams[i].csamplesplayed;

	sc->length = wavstreams[i].csamplesinmem;

	ResampleSfx(s, sc->speed, sc->width, data);
}

//===============================================================================
// Client entity mouth movement code.  Set entity mouthopen variable, based
// on the sound envelope of the voice channel playing.
// KellyB 10/22/97
//===============================================================================


// called when voice channel is first opened on this entity
void SND_InitMouth( int entnum, int entchannel )
{
	if ((entchannel == CHAN_VOICE || entchannel == CHAN_STREAM) && entnum > 0)
	{
		// init mouth movement vars
		cl_entity_t* pent = &cl_entities[entnum];
		pent->mouth.mouthopen = 0;
		pent->mouth.sndavg = 0;
		pent->mouth.sndcount = 0;
	}
}

// called when channel stops

void SND_CloseMouth( channel_t* ch )
{
	if (ch->entnum > 0)
	{
		if (ch->entchannel == CHAN_VOICE || ch->entchannel == CHAN_STREAM)
		{
			// shut mouth
			cl_entities[ch->entnum].mouth.mouthopen = 0;
		}
	}
}

#define CAVGSAMPLES 10

void SND_MoveMouth( channel_t* ch, sfxcache_t* sc, int count )
{
	int		data;
	char* pdata = NULL;
	int		i;
	int		savg;
	int		scount;
	cl_entity_t* pent;

	pent = &cl_entities[ch->entnum];

	i = 0;
	scount = pent->mouth.sndcount;
	savg = 0;

	pdata = (char*)&sc->data[ch->pos];

	while (i < count && scount < CAVGSAMPLES)
	{
		data = pdata[i];
		savg += abs(data);

		i += 80 + ((byte)data & 0x1F);
		scount++;
	}

	pent->mouth.sndavg += savg;
	pent->mouth.sndcount = (byte)scount;

	if (pent->mouth.sndcount >= CAVGSAMPLES)
	{
		pent->mouth.mouthopen = pent->mouth.sndavg / CAVGSAMPLES;
		pent->mouth.sndavg = 0;
		pent->mouth.sndcount = 0;
	}
}

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
	Q_memset(rgrgvoxword, 0, sizeof(rgrgvoxword));

	VOX_ReadSentenceFile();
}

// parse a null terminated string of text into component words, with
// pointers to each word stored in rgpparseword
// note: this code actually alters the passed in string!

void VOX_ParseString( char* psz )
{
	int i;
	int fdone = 0;
	char* pszscan = psz;
	char c;

	Q_memset(rgpparseword, 0, sizeof(char*) * CVOXWORDMAX);

	if (!psz)
		return;

	i = 0;
	rgpparseword[i++] = psz;

	while (!fdone && i < CVOXWORDMAX)
	{
		// scan up to next word
		c = *pszscan;
		while (c && !(c == ' ' || c == '.' || c == ','))
			c = *(++pszscan);

		// if '(' then scan for matching ')'
		if (c == '(')
		{
			c = *(++pszscan);
			while (c)
			{
				if (c == ')')
				{
					c = *(++pszscan);
					break;
				}
				c = *(++pszscan);
			}
			if (!c)
				fdone = 1;
		}

		if (fdone || !c)
			fdone = 1;
		else
		{
			// if . or , insert pause into rgpparseword,
			// unless this is the last character
			if ((c == '.' || c == ',') && *(pszscan + 1) != '\n' && *(pszscan + 1) != '\r'
				&& *(pszscan + 1) != 0)
			{
				if (c == '.')
					rgpparseword[i++] = voxperiod;
				else
					rgpparseword[i++] = voxcomma;

				if (i >= CVOXWORDMAX)
					break;
			}

			// null terminate substring
			*pszscan++ = 0;

			// skip whitespace
			c = *pszscan;
			while (c && (c == ' ' || c == '.' || c == ','))
				c = *(++pszscan);

			if (!c)
				fdone = 1;
			else
				rgpparseword[i++] = pszscan;
		}
	}
}

// backwards scan psz for last '/'
// return substring in szpath null terminated
// if '/' not found, return 'vox/'

char* VOX_GetDirectory( char* szpath, char* psz )
{
	char c;
	int cb = 0;
	char* pszscan = psz + Q_strlen(psz) - 1;

	// scan backwards until first '/' or start of string
	c = *pszscan;
	while (pszscan > psz && c != '/')
	{
		c = *(--pszscan);
		cb++;
	}

	if (c != '/')
	{
		// didn't find '/', return default directory
		Q_strcpy(szpath, "vox/");
		return psz;
	}

	cb = Q_strlen(psz) - cb;
	Q_memcpy(szpath, psz, cb);
	szpath[cb] = 0;
	return pszscan + 1;
}

// set channel volume based on volume of current word

void VOX_SetChanVol( channel_t* ch )
{
	float scale;
	int vol;

	if (ch->isentence < 0)
		return;

	vol = rgrgvoxword[ch->isentence][ch->iword].volume;
	if (vol == 100)
		return;

	scale = vol / 100.0;

	// Remains the same
	if (scale == 1.0)
		return;

	ch->rightvol = (int)(ch->rightvol * scale);
	ch->leftvol = (int)(ch->leftvol * scale);
}

//===============================================================================
//  Get any pitch, volume, start, end params into voxword
//  and null out trailing format characters
//  Format: 
//		someword(v100 p110 s10 e20)
//		
//		v is volume, 0% to n%
//		p is pitch shift up 0% to n%
//		s is start wave offset %
//		e is end wave offset %
//		t is timecompression %
//
//	pass fFirst == 1 if this is the first string in sentence
//  returns 1 if valid string, 0 if parameter block only.
//
//  If a ( xxx ) parameter block does not directly follow a word, 
//  then that 'default' parameter block will be used as the default value
//  for all following words.  Default parameter values are reset
//  by another 'default' parameter block.  Default parameter values
//  for a single word are overridden for that word if it has a parameter block.
// 
//===============================================================================

int VOX_ParseWordParams( char* psz, voxword_t* pvoxword, int fFirst )
{
	char* pszsave = psz;
	char c;
	char ct;
	char sznum[8];
	int i;
	static voxword_t voxwordDefault;

	// init to defaults if this is the first word in string.
	if (fFirst)
	{
		voxwordDefault.pitch = -1;
		voxwordDefault.volume = 100;
		voxwordDefault.start = 0;
		voxwordDefault.end = 100;
		voxwordDefault.fKeepCached = 0;
		voxwordDefault.timecompress = 0;
	}

	*pvoxword = voxwordDefault;

	// look at next to last char to see if we have a 
	// valid format:

	c = *(psz + Q_strlen(psz) - 1);

	if (c != ')')
		return 1;		// no formatting, return

	// scan forward to first '('

	c = *psz;
	while (!(c == '(' || c == ')'))
		c = *(++psz);

	if (c == ')')
		return 0;		// bogus formatting

	// null terminate

	*psz = 0;
	ct = *(++psz);

	while (1)
	{
		// scan until we hit a character in the commandSet

		while (ct && !(ct == 'v' || ct == 'p' || ct == 's' || ct == 'e' || ct == 't'))
			ct = *(++psz);

		if (ct == ')')
			break;

		Q_memset(sznum, 0, sizeof(sznum));
		i = 0;

		c = *(++psz);

		if (!isdigit(c))
			break;

		// read number
		while (isdigit(c) && i < sizeof(sznum) - 1)
		{
			sznum[i++] = c;
			c = *(++psz);
		}

		// get value of number
		i = Q_atoi(sznum);

		switch (ct)
		{
			case 'v': pvoxword->volume = i; break;
			case 'p': pvoxword->pitch = i; break;
			case 's': pvoxword->start = i; break;
			case 'e': pvoxword->end = i; break;
			case 't': pvoxword->timecompress = i; break;
		}

		ct = c;
	}

	// if the string has zero length, this was an isolated
	// parameter block.  Set default voxword to these
	// values

	if (Q_strlen(pszsave) == 0)
	{
		voxwordDefault = *pvoxword;
		return 0;
	}
	else
		return 1;
}

int VOX_IFindEmptySentence( void )
{
	int k;

	for (k = 0; k < CBSENTENCENAME_MAX; k++)
	{
		if (!rgrgvoxword[k][0].sfx)
			return k;
	}

	Con_DPrintf("Sentence or Pitch shift ignored. > 16 playing!\n");
	return -1;
}

void VOX_MakeSingleWordSentence( channel_t* ch, int pitch )
{
	// Create a sentence with a single word

	voxword_t voxword;
	int k;

	// Find empty sentence
	k = VOX_IFindEmptySentence();

	if (k < 0)
	{
		ch->pitch = PITCH_NORM;
		ch->isentence = -1;
		return;
	}

	voxword.volume = 100;
	voxword.pitch = PITCH_NORM;
	voxword.start = 0;
	voxword.end = 100;
	voxword.sfx = ch->sfx;
	voxword.samplefrac = 0;
	voxword.timecompress = 0;
	voxword.fKeepCached = 1;

	rgrgvoxword[k][0] = voxword;
	rgrgvoxword[k][1].sfx = NULL;

	ch->pitch = pitch;
	ch->isentence = k;
	ch->iword = 0;
}

// link all sounds in sentence, start playing first word.

sfxcache_t* VOX_LoadSound( channel_t* pchan, char* pszin )
{
	char buffer[512];
	int i, j, k, cword;
	char	pathbuffer[64];
	char	szpath[32];
	sfxcache_t* sc;
	voxword_t rgvoxword[CVOXWORDMAX];
	char* psz;

	if (!pszin)
		return NULL;

	Q_memset(rgvoxword, 0, sizeof(voxword_t) * CVOXWORDMAX);
	Q_memset(buffer, 0, sizeof(buffer));

	// lookup actual string in rgpszrawsentence, 
	// set pointer to string data

	psz = VOX_LookupString(pszin, NULL);

	if (!psz)
	{
		Con_DPrintf("VOX_LoadSound: no sentence named %s\n", pszin);
		return NULL;
	}

	// get directory from string, advance psz
	psz = VOX_GetDirectory(szpath, psz);

	if (Q_strlen(psz) > sizeof(buffer) - 1)
	{
		Con_DPrintf("VOX_LoadSound: sentence is too long %s\n", psz);
		return NULL;
	}

	// copy into buffer
	Q_strcpy(buffer, psz);
	psz = buffer;

	// parse sentence (also inserts null terminators between words)

	VOX_ParseString(psz);

	// for each word in the sentence, construct the filename,
	// lookup the sfx and save each pointer in a temp array	

	i = 0;
	cword = 0;
	while (rgpparseword[i])
	{
		// Get any pitch, volume, start, end params into voxword

		if (VOX_ParseWordParams(rgpparseword[i], &rgvoxword[cword], i == 0))
		{
			// this is a valid word (as opposed to a parameter block)
			Q_strcpy(pathbuffer, szpath);
			Q_strcat(pathbuffer, rgpparseword[i]);
			Q_strcat(pathbuffer, ".wav");

			// find name, if already in cache, mark voxword
			// so we don't discard when word is done playing
			rgvoxword[cword].sfx = S_FindName(pathbuffer,
					&(rgvoxword[cword].fKeepCached));
			cword++;
		}
		i++;
	}

	k = VOX_IFindEmptySentence();
	if (k < 0)
		return NULL;

	// copy each pointer in the sfx temp array into the
	// sentence array, and set the channel to point to the
	// sentence array
	j = 0;
	while (rgvoxword[j].sfx != NULL)
	{
		rgrgvoxword[k][j] = rgvoxword[j];
		j++;
	}

	rgrgvoxword[k][j].sfx = NULL;

	pchan->isentence = k;
	pchan->iword = 0;
	pchan->sfx = rgrgvoxword[k][0].sfx;

	sc = S_LoadSound(rgvoxword[0].sfx, NULL);

	if (!sc)
	{
		S_FreeChannel(pchan);
		return NULL;
	}

	return sc;
}

int	VOX_FPaintPitchChannelFrom8Offs( portable_samplepair_t* paintbuffer, channel_t* ch, sfxcache_t* sc, int count, int pitch, int timecompress, int offset )
{
	int		data;
	int* lscale, * rscale;
	unsigned char* sfx;
	int		i;
	int		posold;
	int		samplefrac, fracstep, srcsample;
	float	stepscale;
	int		cb;
	portable_samplepair_t* pbuffer;

	pbuffer = &paintbuffer[offset];

	if (ch->isentence < 0)
		return 0;

	if (ch->leftvol > 255)
		ch->leftvol = 255;
	if (ch->rightvol > 255)
		ch->rightvol = 255;

	lscale = snd_scaletable[ch->leftvol >> 3];
	rscale = snd_scaletable[ch->rightvol >> 3];
	sfx = sc->data;

	int chunksize;
	int skipbytes;
	int lowsample;
	int playcount;
	int cdata = 0;
	int j;

	samplefrac = rgrgvoxword[ch->isentence][ch->iword].samplefrac;
	cb = rgrgvoxword[ch->isentence][ch->iword].cbtrim;

	// Calculate the step scale and fraction step for pitch adjustment
	stepscale = pitch / 100.0;
	fracstep = (stepscale * 256.0);

	j = samplefrac >> 8;
	samplefrac += fracstep;

	if (timecompress == 0) // Process samples without time compression
	{
		i = 0;
		while (i < count && j < cb)
		{
			data = sfx[j];
			pbuffer[i].left += lscale[data];
			pbuffer[i].right += rscale[data];
			j = samplefrac >> 8;
			samplefrac += fracstep;
			i++;
		}
	}
	else // Time-compressed samples
	{
		chunksize = cb >> 3;
		skipbytes = chunksize * timecompress / 100;

		i = 0;
		while (i < count)
		{
			if (j >= cb)
				break;

			lowsample = (j % chunksize);
			if (j >= skipbytes && lowsample < skipbytes)
			{
				srcsample = 0;
				while (i < count && j < cb)
				{
					if (srcsample >= 255)
						break;

					data = sfx[j];
					if (data <= 2)
						break;

					pbuffer[i].left += lscale[data];
					pbuffer[i].right += rscale[data];
					j = samplefrac >> 8;
					samplefrac += fracstep;
					srcsample++;
					i++;
				}

				if (i > PAINTBUFFER_SIZE)
					Con_DPrintf("timecompress scan forward: overwrote paintbuffer!");

				if (j >= cb || i >= count)
					break;

				while (1)
				{
					lowsample = (j % chunksize);
					if (lowsample < skipbytes)
					{
						j += (skipbytes - lowsample);
						samplefrac += (skipbytes - lowsample) << 8;
					}

					srcsample = 0;
					while (j < cb)
					{
						if (srcsample >= 255)
							break;

						data = sfx[j];
						if (data <= 2)
							break;

						j = samplefrac >> 8;
						samplefrac += fracstep;
						srcsample++;
					}

					if (j >= cb)
						break;

					lowsample = (j % chunksize);
					if (lowsample >= skipbytes)
					{
						// The remaining chunk data for playback
						cdata = chunksize - lowsample;
						break;
					}
				}
			}
			else
			{
				cdata = chunksize - lowsample;
			}

			playcount = (cdata << 8) / fracstep;
			if (playcount + i > count)
				playcount = count - i;

			if (playcount == 0)
				playcount = 1;

			srcsample = 0;
			while (i < count)
			{
				if (srcsample >= playcount)
					break;

				if (j >= cb)
					break;

				data = sfx[j];
				pbuffer[i].left += lscale[data];
				pbuffer[i].right += rscale[data];
				j = samplefrac >> 8;
				samplefrac += fracstep;
				srcsample++;
				i++;
			}

			if (i >= count)
				break;
		}
	}

	rgrgvoxword[ch->isentence][ch->iword].samplefrac = samplefrac;

	posold = ch->pos - (samplefrac >> 8);
	ch->pos = samplefrac >> 8;
	ch->end += i + posold;

	return j >= cb;
}

char szsentences[] = "sound/sentences.txt"; // sentence file

char* rgpszrawsentence[CVOXFILESENTENCEMAX];
int cszrawsentences;

// Load sentence file into memory, insert null terminators to
// delimit sentence name/sentence pairs.  Keep pointer to each
// sentence name so we can search later.

void VOX_ReadSentenceFile( void )
{
	char* pBuf;
	char* pch;
	char* pchlast;
	int nSentenceCount;
	int nFileLength;

	Q_memset(rgpszrawsentence, 0, sizeof(rgpszrawsentence));

	// load file
	pBuf = (char*)COM_LoadHunkFile(szsentences);
	if (!pBuf)
	{
		Con_DPrintf("Couldn't load %s\n", szsentences);
		return;
	}

	pch = pBuf;
	nFileLength = com_filesize;
	pchlast = pch + nFileLength;

	nSentenceCount = 0;

	while (pch < pchlast)
	{
		char c;

		// skip newline, cr, tab, space

		c = *pch;
		while (pch < pchlast && (c == '\n' || c == '\r' || c == '\t' || c == ' '))
			c = *(++pch);

		// skip entire line if first char is /
		if (*pch != '/')
		{
			rgpszrawsentence[nSentenceCount] = pch;
			nSentenceCount++;

			// scan forward to first space, insert null terminator
			// after sentence name

			c = *pch;
			while (pch < pchlast && c != ' ')
				c = *(++pch);

			if (pch < pchlast)
				*pch++ = 0;
		}
		// scan forward to end of sentence or eof
		while (pch < pchlast && pch[0] != '\n' && pch[0] != '\r')
			pch++;

		// insert null terminator
		if (pch < pchlast)
			*pch++ = 0;
	}
	cszrawsentences = nSentenceCount;
}

// scan rgpszrawsentence, looking for pszin sentence name
// return pointer to sentence data if found, null if not
// CONSIDER: if we have a large number of sentences, should
// CONSIDER: sort strings in rgpszrawsentence and do binary search.

char* VOX_LookupString( char* pszin, int* psentencenum )
{
	int i;
	char* cptr;

	for (i = 0; i < cszrawsentences; i++)
	{
		if (!Q_strcasecmp(pszin, rgpszrawsentence[i]))
		{
			if (psentencenum)
				*psentencenum = i;

			cptr = rgpszrawsentence[i] + Q_strlen(rgpszrawsentence[i]) + 1;
			return cptr; // return sentence value
		}
	}

	return NULL;
}

// trim the start and end times of a voice channel's audio data
// based on specified start and end points

void VOX_TrimStartEndTimes( channel_t* ch, sfxcache_t* sc )
{
	float sstart;
	float send;
	int	length;
	int	i;
	int	srcsample;
	int	skiplen;
	voxword_t* pvoxword;
	char* pdata;
	
	if (ch->isentence < 0)
		return;

	length = sc->length;
	pvoxword = &rgrgvoxword[ch->isentence][ch->iword];
	pvoxword->cbtrim = sc->length;
	sstart = pvoxword->start;
	send = pvoxword->end;

	// Negative length
	if (sstart >= send)
		return;

	if (sstart > 0.0 && sstart < 100.0)
	{
		skiplen = (int)(sstart / 100.0 * length);
		ch->pos += skiplen;

		srcsample = 0;
		i = ch->pos;
		while (i < length && srcsample < 255)
		{
			pdata = (char*)&sc->data[i];
			if (pdata[0] >= -2 && pdata[0] <= 2)
			{
				ch->pos = i;
				ch->end -= skiplen + srcsample;
				break;
			}

			i++;
			srcsample++;
		}

		if (pvoxword->pitch != PITCH_NORM)
		{
			pvoxword->samplefrac += ch->pos << 8;
		}
	}

	if (send > 0.0 && send < 100.0)
	{
		skiplen = (int)((100.0 - send) / 100.0 * length);
		ch->end -= skiplen;

		i = length - skiplen;
		srcsample = 0;
		while (i > ch->pos && srcsample < 255)
		{
			pdata = (char*)&sc->data[i];
			if (pdata[0] >= -2 && pdata[0] <= 2)
			{
				ch->end -= srcsample;
				pvoxword->cbtrim = length - srcsample - skiplen;
				return;
			}

			i--;
			srcsample++;
		}
	}
}

#if defined (__USEA3D)
int PaintToA3D( int iChannel, channel_t* ch, sfxcache_t* sc, int count, int feedStart, float flPitch )
{
	// TODO: Implement
	return FALSE;
}
#endif

// TODO: Implement