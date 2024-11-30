// wad.h
#ifndef WAD_H
#define WAD_H
#ifdef _WIN32
#pragma once
#endif

#define	CMP_NONE		0
#define	CMP_LZSS		1

#define	TYP_NONE		0
#define	TYP_LABEL		1

#define	TYP_LUMPY		64				// 64 + grab command number
#define	TYP_PALETTE		64
#define	TYP_QTEX		65
#define	TYP_QPIC		66
#define	TYP_SOUND		67
#define	TYP_MIPTEX		68

typedef struct qpic_s
{
	int			width, height;
	byte		data[4];			// variably sized
} qpic_t;

typedef struct wadinfo_s
{
	char		identification[4];		// should be WAD3 or 3DAW
	int			numlumps;
	int			infotableofs;
} wadinfo_t;

typedef struct lumpinfo_s
{
	int			filepos;
	int			disksize;
	int			size;					// uncompressed
	char		type;
	char		compression;
	char		pad1, pad2;
	char		name[16];				// must be null terminated
} lumpinfo_t;

typedef struct wadlist_s
{
	qboolean loaded;
	char wadname[32];
	int wad_numlumps;
	lumpinfo_t* wad_lumps;
	byte* wad_base;
} wadlist_t;

typedef struct wadlist_s wadlist_t;
typedef struct lumpinfo_s lumpinfo_t;

void	W_LoadWadFile( char* filename );
void	W_CleanupName( char* in, char* out );
lumpinfo_t* W_GetLumpinfo( char* name, qboolean doerror );
void*	W_GetLumpName( char* name );
void*	W_GetLumpNum( int num );

void SwapPic( qpic_t* pic );

#endif // WAD_H