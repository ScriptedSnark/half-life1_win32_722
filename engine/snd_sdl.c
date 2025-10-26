//
// snd_sdl.c -- POSIX implementation of surface sound API
//

#include "quakedef.h"
#include <SDL2/SDL.h>

// 64K is > 1 second at 16-bit, 22050 Hz
#define WAV_BUFFERS		64
#define WAV_MASK		(WAV_BUFFERS - 1)
#define WAV_BUFFER_SIZE 0x0400

int snd_sent_position;
void* sndBuffers;

qboolean g_fUseDInput;

char bInitialized[16];

// starts at 0 for disabled
static int snd_buffer_count = 0;
static int sample16;

SDL_AudioDeviceID m_devId;

//-----------------------------------------------------------------------------
// Purpose: I don't want to even know xd
//-----------------------------------------------------------------------------
void SetMouseEnable(qboolean fEnable)
{
}

//-----------------------------------------------------------------------------
// Purpose: Releases the sound buffer by decrementing the buffer count.
//          When count reaches zero, clears the buffer and shuts down sound.
//-----------------------------------------------------------------------------
void Snd_ReleaseBuffer()
{
	if (!--snd_buffer_count)
	{
		S_ClearBuffer();
		S_Shutdown();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Acquires the sound buffer by incrementing the buffer count.
//          When count becomes one, starts up the sound system.
//-----------------------------------------------------------------------------
void Snd_AcquireBuffer()
{
	if (++snd_buffer_count == 1)
	{
		S_Startup();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Blocks sound output by pausing the SDL audio device.
//          Uses a reference counter to handle nested block calls.
//-----------------------------------------------------------------------------
void S_BlockSound()
{
	if (++snd_blocked == 1)
	{
		SDL_PauseAudioDevice(m_devId, TRUE);
	}
}
//-----------------------------------------------------------------------------
// Purpose: Unblocks sound output by resuming the SDL audio device.
//          Uses a reference counter to handle nested unblock calls.
//-----------------------------------------------------------------------------
void S_UnblockSound()
{
	if (--snd_blocked == 0)
	{
		SDL_PauseAudioDevice(m_devId, FALSE);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Shuts down the SDL audio subsystem and frees allocated buffers
//-----------------------------------------------------------------------------
void SNDDMA_Shutdown()
{
	if (!bInitialized[0])
	{
		return;
	}

	bInitialized[0] = FALSE;

	// none of these SDL_* functions are available to call if this is false.
	if (m_devId != NULL)
	{
		SDL_CloseAudioDevice(m_devId);
		m_devId = NULL;
	}

	SDL_QuitSubSystem(SDL_INIT_AUDIO);
	free(sndBuffers); // Free output buffers
	sndBuffers = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: SDL audio callback function that fills the audio stream with
//          sound data from the ring buffer, handling wraparound as needed
//-----------------------------------------------------------------------------
void AudioCallbackEntry(void* userdata, Uint8* stream, int len)
{
  if (!m_devId)
  {
    puts("SDLAUDIO: uhoh, no audio device!");
    return; // can this even happen?
  }

  while (len > 0)
  {
    const uint8_t* buf = (uint8_t*)sndBuffers + snd_sent_position;
  
    // spaceAvailable == bytes before we overrun the end of the ring buffer.
    const int spaceAvailable = ((WAV_BUFFERS * WAV_BUFFER_SIZE) - snd_sent_position);
    int writeLen = (len < spaceAvailable) ? len : spaceAvailable;

    if ((writeLen + snd_sent_position) >= (WAV_BUFFERS * WAV_BUFFER_SIZE))
    {
      writeLen = (WAV_BUFFERS * WAV_BUFFER_SIZE) - snd_sent_position;
      snd_sent_position = (WAV_BUFFERS * WAV_BUFFER_SIZE);
    }
    else
    {
      snd_sent_position += writeLen;
    }

    *stream = '\0';
    *((unsigned char*)sndBuffers) = '\0';

    memcpy(stream, buf, writeLen);

    stream += writeLen;
    len -= writeLen;

    if (snd_sent_position >= (WAV_BUFFERS * WAV_BUFFER_SIZE))
    {
      snd_sent_position = 0;
    }
  }
}


//-----------------------------------------------------------------------------
// Purpose: Initializes the SDL audio subsystem and sets up the sound buffer
//-----------------------------------------------------------------------------
qboolean SNDDMA_Init()
{
	SDL_AudioSpec desired, obtained;
	int nBufferSize;

	if (bInitialized[0])
	{
		return TRUE;
	}

	shm = &sn;
	shm->channels = 2;
	shm->samplebits = 16;
	shm->speed = SOUND_11k;
	shm->dmaspeed = SOUND_DMA_SPEED;

	snd_sent_position = 0;

	bInitialized[0] = TRUE;

	// Set these environment variables, in case we're using PulseAudio.
	setenv("PULSE_PROP_application.name", "Half-Life 1", 1);
	setenv("PULSE_PROP_media.role", "game", 1);

	// !!! FIXME: specify channel map, etc
	// !!! FIXME: set properties (role, icon, etc).

#define SDLAUDIO_FAIL(fnstr)     \
	do                           \
	{                            \
		puts(fnstr);             \
		bInitialized[0] = FALSE; \
		printf("SDL error: %s\n", SDL_GetError()); \
		Sys_Error(fnstr);		 \
		return FALSE;            \
	} while (0)

	if (!SDL_WasInit(SDL_INIT_AUDIO))
	{
		if (SDL_InitSubSystem(SDL_INIT_AUDIO))
		{
			SDLAUDIO_FAIL("SDL_InitSubSystem(SDL_INIT_AUDIO) failed.");
		}
	}

	// Open an audio device...
	//  !!! FIXME: let user specify a device?
	// !!! FIXME: we can handle quad, 5.1, 7.1, etc here.
	memset(&desired, '\0', sizeof(desired));
	desired.freq = shm->dmaspeed;
	desired.format = AUDIO_S16SYS;
	desired.channels = shm->channels;
	desired.samples = 1024;
	desired.callback = AudioCallbackEntry;
	m_devId = SDL_OpenAudioDevice(NULL, 0, &desired, &obtained, SDL_AUDIO_ALLOW_ANY_CHANGE);

	if (!m_devId)
	{
		SDLAUDIO_FAIL("SDL_OpenAudioDevice failed.");
	}

	//
	// We're now ready to feed audio data to SDL!
	//

	// Allocate and lock memory for the waveform data.
	nBufferSize = WAV_BUFFER_SIZE * WAV_BUFFERS;
	sndBuffers = malloc(WAV_BUFFER_SIZE * WAV_BUFFERS);
	memset(sndBuffers, '\0', nBufferSize);

	shm->soundalive = TRUE;
	shm->splitbuffer = FALSE;

	shm->samples = (WAV_BUFFER_SIZE * WAV_BUFFERS) / (shm->samplebits / 8);
	shm->samplepos = 0;
	shm->submission_chunk = TRUE;
	shm->buffer = (byte*)sndBuffers;

	sample16 = (shm->samplebits / 8) - 1;
	SDL_PauseAudioDevice(m_devId, FALSE);

	return TRUE;
}

//-----------------------------------------------------------------------------
// Purpose: Submit sound data to the audio device (currently a no-op for SDL)
//-----------------------------------------------------------------------------
void SNDDMA_Submit()
{
}

//-----------------------------------------------------------------------------
// Purpose: Returns the current DMA position in the sound buffer as a sample offset
//-----------------------------------------------------------------------------
int SNDDMA_GetDMAPos()
{
	return (snd_sent_position >> sample16) & (shm->samples - 1);
}