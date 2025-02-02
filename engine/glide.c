#include "winsani_in.h"
#include <windows.h>
#include "winsani_out.h"

#include <quakedef.h>

/*
** basic data types
*/
typedef unsigned char FxU8;
typedef signed char FxI8;
typedef unsigned short FxU16;
typedef short FxI16;
typedef uint32 FxI32;
typedef uint32 FxU32;
typedef int FxBool;
typedef float FxFloat;
typedef double FxDouble;

typedef FxI32 GrLfbWriteMode_t;
#define GR_LFBWRITEMODE_565        0x0 /* RGB:RGB */
#define GR_LFBWRITEMODE_555        0x1 /* RGB:RGB */
#define GR_LFBWRITEMODE_1555       0x2 /* ARGB:ARGB */
#define GR_LFBWRITEMODE_RESERVED1  0x3
#define GR_LFBWRITEMODE_888        0x4 /* RGB */
#define GR_LFBWRITEMODE_8888       0x5 /* ARGB */
#define GR_LFBWRITEMODE_RESERVED2  0x6
#define GR_LFBWRITEMODE_RESERVED3  0x7
#define GR_LFBWRITEMODE_RESERVED4  0x8
#define GR_LFBWRITEMODE_RESERVED5  0x9
#define GR_LFBWRITEMODE_RESERVED6  0xa
#define GR_LFBWRITEMODE_RESERVED7  0xb
#define GR_LFBWRITEMODE_565_DEPTH  0xc /* RGB:DEPTH */
#define GR_LFBWRITEMODE_555_DEPTH  0xd /* RGB:DEPTH */
#define GR_LFBWRITEMODE_1555_DEPTH 0xe /* ARGB:DEPTH */
#define GR_LFBWRITEMODE_ZA16       0xf /* DEPTH:DEPTH */
#define GR_LFBWRITEMODE_ANY        0xFF

typedef FxI32 GrOriginLocation_t;
#define GR_ORIGIN_UPPER_LEFT    0x0
#define GR_ORIGIN_LOWER_LEFT    0x1
#define GR_ORIGIN_ANY           0xFF

typedef FxU32 GrLock_t;
#define GR_LFB_READ_ONLY  0x00
#define GR_LFB_WRITE_ONLY 0x01
#define GR_LFB_IDLE       0x00
#define GR_LFB_NOIDLE     0x10

typedef FxI32 GrBuffer_t;
#define GR_BUFFER_FRONTBUFFER   0x0
#define GR_BUFFER_BACKBUFFER    0x1
#define GR_BUFFER_AUXBUFFER     0x2
#define GR_BUFFER_DEPTHBUFFER   0x3
#define GR_BUFFER_ALPHABUFFER   0x4
#define GR_BUFFER_TRIPLEBUFFER  0x5

typedef FxI32 GrCmpFnc_t;
#define GR_CMP_NEVER    0x0
#define GR_CMP_LESS     0x1
#define GR_CMP_EQUAL    0x2
#define GR_CMP_LEQUAL   0x3
#define GR_CMP_GREATER  0x4
#define GR_CMP_NOTEQUAL 0x5
#define GR_CMP_GEQUAL   0x6
#define GR_CMP_ALWAYS   0x7

#define GR_ORIGIN_UPPER_LEFT    0x0
#define GR_ORIGIN_LOWER_LEFT    0x1
#define GR_ORIGIN_ANY           0xFF


typedef struct { int size; void* lfbPtr; FxU32 strideInBytes; GrLfbWriteMode_t writeMode; GrOriginLocation_t origin; } GrLfbInfo_t;

typedef struct GrSstPerfStats_s {
	FxU32  pixelsIn;              /* # pixels processed (minus buffer clears) */
	FxU32  chromaFail;            /* # pixels not drawn due to chroma key */
	FxU32  zFuncFail;             /* # pixels not drawn due to Z comparison */
	FxU32  aFuncFail;             /* # pixels not drawn due to alpha comparison */
	FxU32  pixelsOut;             /* # pixels drawn (including buffer clears) */
} GrSstPerfStats_t;

/*
** linear frame buffer functions
*/

typedef FxBool(*LFBLOCK)(GrLock_t type, GrBuffer_t buffer, GrLfbWriteMode_t writeMode, GrOriginLocation_t origin, FxBool pixelPipeline, GrLfbInfo_t* info);

typedef FxBool(*LFBUNLOCK)(GrLock_t type, GrBuffer_t buffer);
typedef void (*GAMMAFUNC)(float value);
typedef void (*DEPTHFUNC)(GrCmpFnc_t function);
typedef void (*PERFSTATS)(GrSstPerfStats_t* pStats);
typedef void (*PERFSTATSRESET)(void);
typedef void (*TRISTATS)(FxU32* trisProcessed, FxU32* trisDrawn);
typedef void (*TRISTATSRESET)(void);
typedef void (*GRIDLE)(void);
typedef int (*LFBREAD)(int src_buffer, FxU32 src_x, FxU32 src_y, FxU32 src_width, FxU32 src_height, FxU32 dst_stride, void* dst_data);


/*
** get glide2x module instance
*/
static HINSTANCE glideDLLInst = NULL;
void GlideLoad(void)
{
	if (glideDLLInst)
		return;
	glideDLLInst = GetModuleHandle("glide2x");
}


/* glide statistics */
unsigned int GlideStats(void)
{
	PERFSTATS grSstPerfStats;
	PERFSTATSRESET grSstResetPerfStats;
	GRIDLE grSstIdle;
	TRISTATS grTriStats;
	TRISTATSRESET grResetTriStats;

	GlideLoad();

	if (!glideDLLInst)
		return 0;

	grSstPerfStats = (PERFSTATS)GetProcAddress(glideDLLInst, "_grSstPerfStats@4");
	grSstResetPerfStats = (PERFSTATSRESET)GetProcAddress(glideDLLInst, "_grSstResetPerfStats@0");
	grSstIdle = (GRIDLE)GetProcAddress(glideDLLInst, "_grSstIdle@0");
	grTriStats = (TRISTATS)GetProcAddress(glideDLLInst, "_grTriStats@8");
	grResetTriStats = (TRISTATSRESET)GetProcAddress(glideDLLInst, "_grResetTriStats@0");

	if (!grSstPerfStats || !grSstIdle || !grSstResetPerfStats || !grTriStats || !grResetTriStats)
	{
		return 0;
	}

	grSstIdle();

	GrSstPerfStats_t stats;
	grSstPerfStats(&stats);

	FxU32 trisProcessed;
	grTriStats(&trisProcessed, NULL);

	grSstResetPerfStats();
	grResetTriStats();

	return stats.pixelsIn;
}


/*
** read and copy screen pixels to buffer
*/
int GlideReadPixels(int x, int y, int width, int height, word* pixels)
{
	LFBREAD grLfbReadRegion;
	int     i, j;
	int     sourcestep, sourcebase;
	word* pSource, * pDest, * pDestRGB;
	byte* pSourceBT, * pSourceRGB;

	GlideLoad();

	if (!glideDLLInst)
		return FALSE;

	grLfbReadRegion = (LFBREAD)GetProcAddress(glideDLLInst, "_grLfbReadRegion@28");
	if (!grLfbReadRegion)
	{
		return FALSE;
	}

	if (!grLfbReadRegion(GR_BUFFER_FRONTBUFFER, x, y, width, height, 2 * width, pixels))
	{
		return FALSE;
	}

	pDest = pixels;
	sourcestep = 0;
	sourcebase = width * (height - 1);

	for (i = 0; i < height / 2; i++)
	{
		pSource = &pixels[sourcebase + sourcestep];

		for (j = 0; j < width; j++)
		{
			GLushort tmp;
			tmp = pDest[0];
			pDest[0] = pSource[0];
			pSource[0] = tmp;

			pDest++;
			pSource++;
		}

		pDest += width;
		sourcestep += width;
		sourcebase += (-2 * width);
	}

	pSourceBT = (byte*)&pixels[height * width + 1] + height * width;
	pDest = &pixels[height * width];

	for (i = 0; i < height; i++)
	{
		pDestRGB = pDest;
		pSourceRGB = pSourceBT;

		for (j = 0; j < width; j++, pSourceRGB -= 3)
		{
			pDestRGB--;

			// Swap to BGR
			pSourceRGB[2] = (byte)((UINT)pDestRGB[0] >> 8) & 0xF8;
			pSourceRGB[1] = (byte)(pDestRGB[0] >> 3) & 0xFC;
			pSourceRGB[0] = (byte)(pDestRGB[0] << 3) & 0xFF;
		}

		pDest = (word*)((byte*)pDest + (-2 * width));
		pSourceBT += (-3 * width);
	}

	return TRUE;
}