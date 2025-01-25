#ifndef SHAKE_H
#define SHAKE_H

// Screen / View effects

// screen shake
extern int gmsgShake;


extern void V_ApplyShake( float* origin, float* angles, float factor );
extern void V_CalcShake( void );


#define FFADE_IN			0x0000		// Just here so we don't pass 0 into the function
#define FFADE_OUT			0x0001		// Fade out (not in)
#define FFADE_MODULATE		0x0002		// Modulate (don't blend)


// This structure is sent over the net to describe a screen fade event
typedef struct
{
	unsigned short 	duration;		// FIXED 4.12 seconds duration
	unsigned short 	holdTime;		// FIXED 4.12 seconds duration until reset (fade & hold)
	short			fadeFlags;		// flags
	byte			r, g, b, a;		// fade to color ( max alpha )
} ScreenFade;

#endif // SHAKE_H