#if !defined( QFONTH )
#define QFONTH
#ifdef _WIN32
#pragma once
#endif

// Font stuff

#define NUM_GLYPHS 256

typedef struct
{
	short startoffset;
	short charwidth;
} charinfo;

typedef struct qfont_s
{
	int 		width, height;
	int			rowcount;
	int			rowheight;
	charinfo	fontinfo[NUM_GLYPHS];
	byte 		data[4];
} qfont_t;

#endif // qfont.h