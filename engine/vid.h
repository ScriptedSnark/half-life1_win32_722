// vid.h -- video driver defs
#ifndef VID_H
#define VID_H

#include "vmodes.h"

// TODO: This struct is 76 bytes sized instead of 80
typedef struct viddef_s
{
	pixel_t* buffer;			// invisible buffer
	pixel_t* colormap;		// 256 * VID_GRADES size
	unsigned short* colormap16;		// 256 * VID_GRADES size
	int				fullbright;		// index of first fullbright color
	int				bits;
	int				is15bit;
	unsigned		rowbytes;		// may be > width if displayed in a window
	unsigned		width;
	unsigned		height;
	float			aspect;			// width / height -- < 0 is taller than wide
	int				numpages;
	int				recalc_refdef;	// if true, recalc vid-based stuff
	pixel_t* conbuffer;
	int				conrowbytes;
	unsigned		conwidth;
	unsigned		conheight;
	unsigned 		maxwarpwidth;
	unsigned 		maxwarpheight;
	pixel_t* direct;			// direct drawing to framebuffer, if not
	//  NULL
	VidTypes		vidtype;
} viddef_t;

extern	viddef_t	vid;				// global video state

extern	byte* vid_surfcache;
extern	int		vid_surfcachesize;

int		VID_Init( word* palette );
// Called at startup to set up translation tables, takes 256 8 bit RGB values
// the palette data will go away after the call, so it must be copied off if
// the video driver will need it again

void	VID_WriteBuffer( const char* pFilename );
// Writes vid.buffer contents to a bmp file

#endif //VIDH