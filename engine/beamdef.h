#if !defined ( BEAMDEFH )
#define BEAMDEFH
#ifdef _WIN32
#pragma once
#endif

#define FBEAM_STARTENTITY		0x00000001
#define FBEAM_ENDENTITY			0x00000002
#define FBEAM_FADEIN			0x00000004
#define FBEAM_FADEOUT			0x00000008
#define FBEAM_SINENOISE			0x00000010
#define FBEAM_SOLID				0x00000020
#define FBEAM_SHADEIN			0x00000040
#define FBEAM_SHADEOUT			0x00000080
#define FBEAM_STARTVISIBLE		0x10000000		// Has this client actually seen this beam's start entity yet?
#define FBEAM_ENDVISIBLE		0x20000000		// Has this client actually seen this beam's end entity yet?
#define FBEAM_ISACTIVE			0x40000000
#define FBEAM_FOREVER			0x80000000

typedef struct beam_s
{
	struct beam_s* next;
	int			type;
	int			flags;
	vec3_t		source;
	vec3_t		target;
	vec3_t		delta;
	float		t;		// 0 .. 1 over lifetime of beam
	float		freq;
	float		die;
	float		width;
	float		amplitude;
	float		r;
	float		g;
	float		b;
	float		brightness;
	float		speed;
	float		frameRate;
	float		frame;
	int			segments;
	int			startEntity;
	int			endEntity;
	int			modelIndex;
	int			frameCount;
	struct particle_s* particles;
} BEAM;

#endif