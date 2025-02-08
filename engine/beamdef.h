#if !defined ( BEAMDEFH )
#define BEAMDEFH
#ifdef _WIN32
#pragma once
#endif

#define FBEAM_STARTENTITY		0x00000001


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
	int			unknown; // TODO: Figure out
} BEAM;

#endif