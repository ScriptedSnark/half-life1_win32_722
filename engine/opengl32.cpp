#include "opengl32.h"
#include "d3d_structs.h"
#include <math.h>

#include "../dx6sdk/include/dxmgr_i.c"

// conversion from 'type1' to 'type2', possible loss of data (triggered by glEnd's vanilla behavior)
#pragma warning( disable : 4244 )

#ifdef _WIN32
#define DLL_EXPORT extern "C" __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

D3D_GLOBALS gD3D;

static void QuakeFlushIndexedPrimitives( void )
{
	DWORD	dummy;

	if (gD3D.indexCount)
	{
		if (gD3D.vertStart != gD3D.vertCount)
		{
			gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
			gD3D.vertStart = gD3D.vertCount;
		}

		gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
		gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
		gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dummy);

		gD3D.vertStart = 0;
		gD3D.vertCount = 0;
		gD3D.indexCount = 0;
	}
}

static void QuakeSetTextureStageState( DWORD stage, D3DTEXTURESTAGESTATETYPE stageStartType, DWORD value )
{
	DWORD* pType;
	DWORD	dummy;

	pType = &gD3D.tsStates0[(D3D_MAX_TSSTAGES * stage) + stageStartType];

	if (*pType != value)
	{
		if (gD3D.indexCount)
		{
			if (gD3D.vertStart != gD3D.vertCount)
			{
				gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
				gD3D.vertStart = gD3D.vertCount;
			}

			gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
			gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
			gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dummy);

			gD3D.vertStart = 0;
			gD3D.vertCount = 0;
			gD3D.indexCount = 0;
		}

		*pType = value;
		gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, stage, stageStartType, value);
	}
}

static void QuakeFlushVertexBuffer( void )
{
	if (gD3D.vertStart != gD3D.vertCount)
	{
		gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
		gD3D.vertStart = gD3D.vertCount;
	}
}

static void ConvertSurfaceTo32Bit( DWORD internalFormat, DWORD width, DWORD height, BYTE* pixels, DWORD* pic )
{
	int i;

	if (internalFormat == 1)
	{
		for (i = 0; i < (int)(width * height); i++)
			pic[i] = RGBA_MAKE(pixels[i], pixels[i], pixels[i], pixels[i]);
	}
	else if (internalFormat > 2 && internalFormat <= 4)
	{
		memcpy(pic, pixels, width * height * 4);
	}
}

static void ResizeSurface( DWORD dwWidth, DWORD dwHeight, DWORD* src, DWORD dwNewWidth, DWORD dwNewHeight, DWORD* dest )
{
	DWORD x, y, srcX, srcY, destOffset;
	double scaleX, scaleY;

	scaleX = (double)dwWidth / (double)dwNewWidth;
	scaleY = (double)dwHeight / (double)dwNewHeight;

	if (dwNewHeight == 0)
		return;

	destOffset = 0;
	for (y = 0; y < dwNewHeight; y++)
	{
		srcY = (DWORD)((double)y * scaleY);
		for (x = 0; x < dwNewWidth; x++)
		{
			srcX = (DWORD)((double)x * scaleX);
			dest[x + destOffset] = src[srcY * dwWidth + srcX];
		}
		destOffset += dwNewWidth;
	}
}

static void SurfaceSingle8( RECT* rect, DWORD* src, LPDDSURFACEDESC2 lpDDSD )
{
	LONG i, j, k, l;
	BYTE* dest;

	dest = (BYTE*)lpDDSD->lpSurface;
	for (k = rect->top, i = 0; k < rect->bottom; k++)
	{
		for (j = rect->left, l = 0; j < rect->right; j++, i++, l++)
		{
			dest[l] = (src[i] & 0xFF);
		}
		dest += lpDDSD->lPitch;
	}
}

static void SurfaceRGBA4444( RECT* rect, DWORD* src, LPDDSURFACEDESC2 lpDDSD )
{
	LONG i, j, k, l;
	WORD* dest;

	dest = (WORD*)lpDDSD->lpSurface;
	for (k = rect->top, i = 0; k < rect->bottom; k++)
	{
		for (j = rect->left, l = 0; j < rect->right; j++, i++, l++)
		{
			dest[l] = (WORD)(((src[i] & 0xF0000000) >> 16) | ((src[i] & 0x00F00000) >> 12) | ((src[i] & 0x0000F000) >> 8) | ((src[i] & 0x000000F0) >> 4));
		}
		dest = (WORD*)((BYTE*)dest + lpDDSD->lPitch);
	}
}

static void SurfaceRGBA5551( RECT* rect, DWORD* src, LPDDSURFACEDESC2 lpDDSD )
{
	LONG i, j, k, l;
	WORD* dest;

	dest = (WORD*)lpDDSD->lpSurface;
	for (k = rect->top, i = 0; k < rect->bottom; k++)
	{
		for (j = rect->left, l = 0; j < rect->right; j++, i++, l++)
		{
			dest[l] = (WORD)(((src[i] & 0xF8) << 7) | ((src[i] >> 6) & 0x3E0) | ((src[i] >> 19) & 0x1F) | ((src[i] >> 16) & 0x8000));
		}
		dest = (WORD*)((BYTE*)dest + lpDDSD->lPitch);
	}
}

static void SurfaceRGB565( RECT* rect, DWORD* src, LPDDSURFACEDESC2 lpDDSD )
{
	LONG i, j, k, l;
	WORD* dest;

	dest = (WORD*)lpDDSD->lpSurface;
	for (k = rect->top, i = 0; k < rect->bottom; k++)
	{
		for (j = rect->left, l = 0; j < rect->right; j++, i++, l++)
		{
			dest[l] = (WORD)(((src[i] >> 19) & 0x1F) | ((src[i] >> 5) & 0x7E0) | ((src[i] & 0xFFF8) << 8));
		}
		dest = (WORD*)((BYTE*)dest + lpDDSD->lPitch);
	}
}

static void SurfaceRGB555( RECT* rect, DWORD* src, LPDDSURFACEDESC2 lpDDSD )
{
	LONG i, j, k, l;
	WORD* dest;

	dest = (WORD*)lpDDSD->lpSurface;
	for (k = rect->top, i = 0; k < rect->bottom; k++)
	{
		for (j = rect->left, l = 0; j < rect->right; j++, i++, l++)
		{
			dest[l] = (WORD)(((src[i] >> 19) & 0x1F) | ((src[i] >> 6) & 0x3E0) | ((src[i] & 0xF8) << 7));
		}
		dest = (WORD*)((BYTE*)dest + lpDDSD->lPitch);
	}
}

static void LoadSurface( LPDIRECTDRAWSURFACE4 lpDDS, DWORD internalFormat, DWORD width, DWORD height, DWORD newWidth, DWORD newHeight, BYTE* pixels )
{
	DDSURFACEDESC2	ddsd;
	HRESULT	hResult;
	RECT	rc;
	DWORD* srcPic, * pic;

	srcPic = (DWORD*)malloc(width * height * 4);

	ConvertSurfaceTo32Bit(internalFormat, width, height, pixels, srcPic);

	if (width == newWidth && height == newHeight)
	{
		pic = srcPic;
	}
	else
	{
		// The surface needs to be resized
		pic = (DWORD*)malloc(newWidth * newHeight * 4);
		ResizeSurface(width, height, srcPic, newWidth, newHeight, pic);
		free(srcPic);
	}

	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);

	hResult = lpDDS->lpVtbl->Lock(lpDDS, NULL, &ddsd, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, NULL);
	if (hResult != DD_OK)
	{
		lpDDS->lpVtbl->Release(lpDDS);
		OutputDebugString("Lock failed while loading surface\n");
		free(pic);
		return;
	}

	SetRect(&rc, 0, 0, ddsd.dwWidth, ddsd.dwHeight);

	if (ddsd.ddpfPixelFormat.dwRBitMask == 0xFF)
	{
		SurfaceSingle8(&rc, pic, &ddsd);
	}
	else if (ddsd.ddpfPixelFormat.dwRGBAlphaBitMask == 0xF000)
	{
		SurfaceRGBA4444(&rc, pic, &ddsd);
	}
	else if (ddsd.ddpfPixelFormat.dwRGBAlphaBitMask == 0x8000)
	{
		SurfaceRGBA5551(&rc, pic, &ddsd);
	}
	else if (ddsd.ddpfPixelFormat.dwRBitMask == 0xF800)
	{
		SurfaceRGB565(&rc, pic, &ddsd);
	}
	else
	{
		SurfaceRGB555(&rc, pic, &ddsd);
	}

	free(pic);

	lpDDS->lpVtbl->Unlock(lpDDS, NULL);
}

static HRESULT LoadSubSurface( LPDIRECTDRAWSURFACE4 lpDDS4, DWORD internalFormat, DWORD width, DWORD height, BYTE* pixels, RECT* rect )
{
	DDSURFACEDESC2	ddsd;
	HRESULT	hResult;
	DWORD newWidth, newHeight;
	DWORD* srcPic, * pic;

	newWidth = rect->right - rect->left;
	newHeight = rect->bottom - rect->top;

	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);

	hResult = lpDDS4->lpVtbl->Lock(lpDDS4, rect, &ddsd, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, NULL);
	if (hResult != DD_OK)
	{
		lpDDS4->lpVtbl->Release(lpDDS4);
		OutputDebugString("Lock failed while loading surface\n");
		return hResult;
	}

	if (internalFormat == 3 && ddsd.ddpfPixelFormat.dwRBitMask == 0xF800 && newWidth == width && newHeight == height)
	{
		SurfaceRGB565(rect, (DWORD*)pixels, &ddsd);
	}
	else if (internalFormat == 3 && ddsd.ddpfPixelFormat.dwRBitMask == 0x7C00 && newWidth == width && newHeight == height)
	{
		SurfaceRGB555(rect, (DWORD*)pixels, &ddsd);
	}
	else
	{
		srcPic = (DWORD*)malloc(width * height * 4);
		ConvertSurfaceTo32Bit(internalFormat, width, height, pixels, srcPic);

		if (newWidth == width && newHeight == height)
		{
			pic = srcPic;
		}
		else
		{
			// The surface needs to be resized
			pic = (DWORD*)malloc(newWidth * newHeight * 4);
			ResizeSurface(width, height, srcPic, newWidth, newHeight, pic);
			free(srcPic);
		}

		if (ddsd.ddpfPixelFormat.dwRBitMask == 0xFF)
		{
			SurfaceSingle8(rect, pic, &ddsd);
		}
		else if (ddsd.ddpfPixelFormat.dwRGBAlphaBitMask == 0xF000)
		{
			SurfaceRGBA4444(rect, pic, &ddsd);
		}
		else if (ddsd.ddpfPixelFormat.dwRGBAlphaBitMask == 0x8000)
		{
			SurfaceRGBA5551(rect, pic, &ddsd);
		}
		else if (ddsd.ddpfPixelFormat.dwRBitMask == 0xF800)
		{
			SurfaceRGB565(rect, pic, &ddsd);
		}
		else
		{
			SurfaceRGB555(rect, pic, &ddsd);
		}

		free(pic);
	}

	lpDDS4->lpVtbl->Unlock(lpDDS4, NULL);
	return DD_OK;
}

DLL_EXPORT void APIENTRY glAccum( GLenum op, GLfloat value )
{
}

DLL_EXPORT void APIENTRY glAlphaFunc( GLenum func, GLclampf ref )
{
	DWORD	alphaFunc;
	DWORD	alphaRef;
	DWORD	dummy;

	alphaFunc = -1;

	switch (func)
	{
	case GL_NEVER:
		alphaFunc = D3DCMP_NEVER;
		break;
	case GL_LESS:
		alphaFunc = D3DCMP_LESS;
		break;
	case GL_EQUAL:
		alphaFunc = D3DCMP_EQUAL;
		break;
	case GL_LEQUAL:
		alphaFunc = D3DCMP_LESSEQUAL;
		break;
	case GL_GREATER:
		alphaFunc = D3DCMP_GREATER;
		break;
	case GL_NOTEQUAL:
		alphaFunc = D3DCMP_NOTEQUAL;
		break;
	case GL_GEQUAL:
		alphaFunc = D3DCMP_GREATEREQUAL;
		break;
	case GL_ALWAYS:
		alphaFunc = D3DCMP_ALWAYS;
		break;
	}

	if (alphaFunc >= 0)
	{
		if (gD3D.rStates[D3DRENDERSTATE_ALPHAFUNC] != alphaFunc)
		{
			if (gD3D.indexCount)
			{
				if (gD3D.vertStart != gD3D.vertCount)
				{
					gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
					gD3D.vertStart = gD3D.vertCount;
				}

				gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
				gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
				gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dummy);

				gD3D.vertStart = 0;
				gD3D.vertCount = 0;
				gD3D.indexCount = 0;
			}

			gD3D.rStates[D3DRENDERSTATE_ALPHAFUNC] = alphaFunc;
			gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_ALPHAFUNC, alphaFunc);
		}

		alphaRef = (DWORD)(ref * 255.0);
		if (gD3D.rStates[D3DRENDERSTATE_ALPHAREF] != alphaRef)
		{
			if (gD3D.indexCount)
			{
				if (gD3D.vertStart != gD3D.vertCount)
				{
					gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
					gD3D.vertStart = gD3D.vertCount;
				}

				gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
				gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
				gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dummy);

				gD3D.vertStart = 0;
				gD3D.vertCount = 0;
				gD3D.indexCount = 0;
			}

			gD3D.rStates[D3DRENDERSTATE_ALPHAREF] = alphaRef;
			gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_ALPHAREF, alphaRef);
		}
	}
}

DLL_EXPORT GLboolean APIENTRY glAreTexturesResident( GLsizei n, const GLuint* textures, GLboolean* residences )
{
	return GL_FALSE;
}

DLL_EXPORT void APIENTRY glArrayElement( GLint i )
{
	static float scale = 255.0f;
	D3D_VERTEX* vert;
	D3DVALUE* verts;
	D3DVALUE* colors;
	unsigned int r, g, b;

	vert = &gD3D.verts[gD3D.vertCount + gD3D.primVertCount];

	verts = (D3DVALUE*)((byte*)gD3D.vertexPointer + (sizeof(D3DVALUE) * 4) * i);
	vert->x = verts[0];
	vert->y = verts[1];
	vert->z = verts[2];

	colors = (D3DVALUE*)((byte*)gD3D.colorPointer + (sizeof(D3DVALUE) * 3) * i);
	r = (unsigned int)(colors[0] * scale);
	g = (unsigned int)(colors[1] * scale);
	b = (unsigned int)(colors[2] * scale);
	if (r > 255)
		r = 255;
	if (g > 255)
		g = 255;
	if (b > 255)
		b = 255;
	vert->color = RGBA_MAKE(r, g, b, 255);
	vert->tu = gD3D.tu;
	vert->tv = gD3D.tv;

	if (gD3D.useSubStage)
	{
		vert->tu2 = gD3D.tu2;
		vert->tv2 = gD3D.tv2;
	}

	gD3D.primVertCount++;
}

DLL_EXPORT void APIENTRY glBegin( GLenum mode )
{
	D3D_TEXTURE* tex;

	gD3D.primMode = mode;
	gD3D.primVertCount = 0;

	if (!gD3D.stage0Active || gD3D.textureValid)
		return;

	tex = &gD3D.textures[gD3D.currentTexture[0]];

	if (gD3D.tsStates0[D3DTSS_ADDRESSU] != tex->addressU)
	{
		QuakeFlushIndexedPrimitives();
		gD3D.tsStates0[D3DTSS_ADDRESSU] = tex->addressU;
		gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_ADDRESSU, tex->addressU);
	}

	if (gD3D.tsStates0[D3DTSS_ADDRESSV] != tex->addressV)
	{
		QuakeFlushIndexedPrimitives();
		gD3D.tsStates0[D3DTSS_ADDRESSV] = tex->addressV;
		gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_ADDRESSV, tex->addressV);
	}

	if (gD3D.tsStates0[D3DTSS_MAGFILTER] != tex->magFilter)
	{
		QuakeFlushIndexedPrimitives();
		gD3D.tsStates0[D3DTSS_MAGFILTER] = tex->magFilter;
		gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_MAGFILTER, tex->magFilter);
	}

	if (gD3D.tsStates0[D3DTSS_MINFILTER] != tex->minFilter)
	{
		QuakeFlushIndexedPrimitives();
		gD3D.tsStates0[D3DTSS_MINFILTER] = tex->minFilter;
		gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_MINFILTER, tex->minFilter);
	}

	if (gD3D.tsStates0[D3DTSS_MIPFILTER] != tex->mipFilter)
	{
		QuakeFlushIndexedPrimitives();
		gD3D.tsStates0[D3DTSS_MIPFILTER] = tex->mipFilter;
		gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_MIPFILTER, tex->mipFilter);
	}

	if (gD3D.lpD3DT2 != tex->lpD3DT2)
	{
		QuakeFlushIndexedPrimitives();
		gD3D.lpD3DT2 = tex->lpD3DT2;
		gD3D.lpD3DD3->lpVtbl->SetTexture(gD3D.lpD3DD3, 0, tex->lpD3DT2);
	}

	if (gD3D.texEnvMode[0] == GL_BLEND)
	{
		if (tex->internalFormat == 1 || tex->internalFormat == 3)
		{
			QuakeSetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE | D3DTA_COMPLEMENT);
			QuakeSetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			QuakeSetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			QuakeSetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			QuakeSetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
			QuakeSetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
		}
		else if (tex->internalFormat == 4)
		{
			QuakeSetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE | D3DTA_COMPLEMENT);
			QuakeSetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			QuakeSetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			QuakeSetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			QuakeSetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
			QuakeSetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		}
	}
	else if (gD3D.texEnvMode[0] == GL_REPLACE)
	{
		if (tex->internalFormat == 1 || tex->internalFormat == 3)
		{
			if (gD3D.tsStates0[D3DTSS_COLORARG1] != D3DTA_TEXTURE)
			{
				QuakeFlushIndexedPrimitives();
				gD3D.tsStates0[D3DTSS_COLORARG1] = D3DTA_TEXTURE;
				gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			}

			if (gD3D.tsStates0[D3DTSS_COLORARG2] != D3DTA_DIFFUSE)
			{
				QuakeFlushIndexedPrimitives();
				gD3D.tsStates0[D3DTSS_COLORARG2] = D3DTA_DIFFUSE;
				gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			}

			if (gD3D.tsStates0[D3DTSS_COLOROP] != D3DTOP_SELECTARG1)
			{
				QuakeFlushIndexedPrimitives();
				gD3D.tsStates0[D3DTSS_COLOROP] = D3DTOP_SELECTARG1;
				gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
			}

			if (gD3D.tsStates0[D3DTSS_ALPHAARG1] != D3DTA_TEXTURE)
			{
				QuakeFlushIndexedPrimitives();
				gD3D.tsStates0[D3DTSS_ALPHAARG1] = D3DTA_TEXTURE;
				gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			}

			if (gD3D.tsStates0[D3DTSS_ALPHAARG2] != D3DTA_DIFFUSE)
			{
				QuakeFlushIndexedPrimitives();
				gD3D.tsStates0[D3DTSS_ALPHAARG2] = D3DTA_DIFFUSE;
				gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
			}

			if (gD3D.tsStates0[D3DTSS_ALPHAOP] != D3DTOP_SELECTARG2)
			{
				QuakeFlushIndexedPrimitives();
				gD3D.tsStates0[D3DTSS_ALPHAOP] = D3DTOP_SELECTARG2;
				gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
			}
		}
		else if (tex->internalFormat == 4)
		{
			if (gD3D.tsStates0[D3DTSS_COLORARG1] != D3DTA_TEXTURE)
			{
				QuakeFlushIndexedPrimitives();
				gD3D.tsStates0[D3DTSS_COLORARG1] = D3DTA_TEXTURE;
				gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			}

			if (gD3D.tsStates0[D3DTSS_COLORARG2] != D3DTA_DIFFUSE)
			{
				QuakeFlushIndexedPrimitives();
				gD3D.tsStates0[D3DTSS_COLORARG2] = D3DTA_DIFFUSE;
				gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			}

			if (gD3D.tsStates0[D3DTSS_COLOROP] != D3DTOP_SELECTARG1)
			{
				QuakeFlushIndexedPrimitives();
				gD3D.tsStates0[D3DTSS_COLOROP] = D3DTOP_SELECTARG1;
				gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
			}

			if (gD3D.tsStates0[D3DTSS_ALPHAARG1] != D3DTA_TEXTURE)
			{
				QuakeFlushIndexedPrimitives();
				gD3D.tsStates0[D3DTSS_ALPHAARG1] = D3DTA_TEXTURE;
				gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			}

			if (gD3D.tsStates0[D3DTSS_ALPHAARG2] != D3DTA_DIFFUSE)
			{
				QuakeFlushIndexedPrimitives();
				gD3D.tsStates0[D3DTSS_ALPHAARG2] = D3DTA_DIFFUSE;
				gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
			}

			if (gD3D.tsStates0[D3DTSS_ALPHAOP] != D3DTOP_SELECTARG1)
			{
				QuakeFlushIndexedPrimitives();
				gD3D.tsStates0[D3DTSS_ALPHAOP] = D3DTOP_SELECTARG1;
				gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
			}
		}
	}
	else if (gD3D.texEnvMode[0] == GL_MODULATE)
	{
		if (tex->internalFormat == 1 || tex->internalFormat == 3)
		{
			if (gD3D.tsStates0[D3DTSS_COLORARG1] != D3DTA_TEXTURE)
			{
				QuakeFlushIndexedPrimitives();
				gD3D.tsStates0[D3DTSS_COLORARG1] = D3DTA_TEXTURE;
				gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			}

			if (gD3D.tsStates0[D3DTSS_COLORARG2] != D3DTA_DIFFUSE)
			{
				QuakeFlushIndexedPrimitives();
				gD3D.tsStates0[D3DTSS_COLORARG2] = D3DTA_DIFFUSE;
				gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			}

			if (gD3D.tsStates0[D3DTSS_COLOROP] != D3DTOP_MODULATE)
			{
				QuakeFlushIndexedPrimitives();
				gD3D.tsStates0[D3DTSS_COLOROP] = D3DTOP_MODULATE;
				gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			}

			if (gD3D.tsStates0[D3DTSS_ALPHAARG1] != D3DTA_TEXTURE)
			{
				QuakeFlushIndexedPrimitives();
				gD3D.tsStates0[D3DTSS_ALPHAARG1] = D3DTA_TEXTURE;
				gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			}

			if (gD3D.tsStates0[D3DTSS_ALPHAARG2] != D3DTA_DIFFUSE)
			{
				QuakeFlushIndexedPrimitives();
				gD3D.tsStates0[D3DTSS_ALPHAARG2] = D3DTA_DIFFUSE;
				gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
			}

			if (gD3D.tsStates0[D3DTSS_ALPHAOP] != D3DTOP_SELECTARG2)
			{
				QuakeFlushIndexedPrimitives();
				gD3D.tsStates0[D3DTSS_ALPHAOP] = D3DTOP_SELECTARG2;
				gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
			}
		}
		else if (tex->internalFormat == 4)
		{
			if (gD3D.tsStates0[D3DTSS_COLORARG1] != D3DTA_TEXTURE)
			{
				QuakeFlushIndexedPrimitives();
				gD3D.tsStates0[D3DTSS_COLORARG1] = D3DTA_TEXTURE;
				gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			}

			if (gD3D.tsStates0[D3DTSS_COLORARG2] != D3DTA_DIFFUSE)
			{
				QuakeFlushIndexedPrimitives();
				gD3D.tsStates0[D3DTSS_COLORARG2] = D3DTA_DIFFUSE;
				gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			}

			if (gD3D.tsStates0[D3DTSS_COLOROP] != D3DTOP_MODULATE)
			{
				QuakeFlushIndexedPrimitives();
				gD3D.tsStates0[D3DTSS_COLOROP] = D3DTOP_MODULATE;
				gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			}

			if (gD3D.tsStates0[D3DTSS_ALPHAARG1] != D3DTA_TEXTURE)
			{
				QuakeFlushIndexedPrimitives();
				gD3D.tsStates0[D3DTSS_ALPHAARG1] = D3DTA_TEXTURE;
				gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			}

			if (gD3D.tsStates0[D3DTSS_ALPHAARG2] != D3DTA_DIFFUSE)
			{
				QuakeFlushIndexedPrimitives();
				gD3D.tsStates0[D3DTSS_ALPHAARG2] = D3DTA_DIFFUSE;
				gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
			}

			if (gD3D.tsStates0[D3DTSS_ALPHAOP] != D3DTOP_MODULATE)
			{
				QuakeFlushIndexedPrimitives();
				gD3D.tsStates0[D3DTSS_ALPHAOP] = D3DTOP_MODULATE;
				gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			}
		}
	}
	else if (gD3D.texEnvMode[0] == GL_DECAL)
	{
		if (tex->internalFormat == 3)
		{
			if (gD3D.tsStates0[D3DTSS_COLORARG1] != D3DTA_TEXTURE)
			{
				QuakeFlushIndexedPrimitives();
				gD3D.tsStates0[D3DTSS_COLORARG1] = D3DTA_TEXTURE;
				gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			}

			if (gD3D.tsStates0[D3DTSS_COLORARG2] != D3DTA_DIFFUSE)
			{
				QuakeFlushIndexedPrimitives();
				gD3D.tsStates0[D3DTSS_COLORARG2] = D3DTA_DIFFUSE;
				gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			}

			if (gD3D.tsStates0[D3DTSS_COLOROP] != D3DTOP_SELECTARG1)
			{
				QuakeFlushIndexedPrimitives();
				gD3D.tsStates0[D3DTSS_COLOROP] = D3DTOP_SELECTARG1;
				gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
			}

			if (gD3D.tsStates0[D3DTSS_ALPHAARG1] != D3DTA_TEXTURE)
			{
				QuakeFlushIndexedPrimitives();
				gD3D.tsStates0[D3DTSS_ALPHAARG1] = D3DTA_TEXTURE;
				gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			}

			if (gD3D.tsStates0[D3DTSS_ALPHAARG2] != D3DTA_DIFFUSE)
			{
				QuakeFlushIndexedPrimitives();
				gD3D.tsStates0[D3DTSS_ALPHAARG2] = D3DTA_DIFFUSE;
				gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
			}

			if (gD3D.tsStates0[D3DTSS_ALPHAOP] != D3DTOP_SELECTARG2)
			{
				QuakeFlushIndexedPrimitives();
				gD3D.tsStates0[D3DTSS_ALPHAOP] = D3DTOP_SELECTARG2;
				gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
			}
		}
		else if (tex->internalFormat == 4)
		{
			QuakeSetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			QuakeSetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			QuakeSetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA);
			QuakeSetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			QuakeSetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
			QuakeSetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
		}
	}

	if (gD3D.useSubStage)
	{
		tex = &gD3D.textures[gD3D.currentTexture[1]];

		QuakeSetTextureStageState(1, D3DTSS_ADDRESSU, tex->addressU);
		QuakeSetTextureStageState(1, D3DTSS_ADDRESSV, tex->addressV);
		QuakeSetTextureStageState(1, D3DTSS_MAGFILTER, tex->magFilter);
		QuakeSetTextureStageState(1, D3DTSS_MINFILTER, tex->minFilter);
		QuakeSetTextureStageState(1, D3DTSS_MIPFILTER, tex->mipFilter);

		if (gD3D.lpD3DT2SubStage != tex->lpD3DT2)
		{
			QuakeFlushIndexedPrimitives();
			gD3D.lpD3DT2SubStage = tex->lpD3DT2;
			gD3D.lpD3DD3->lpVtbl->SetTexture(gD3D.lpD3DD3, 1, tex->lpD3DT2);
		}

		if (gD3D.texEnvMode[1] == GL_BLEND)
		{
			if (tex->internalFormat == 1 || tex->internalFormat == 3)
			{
				QuakeSetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE | D3DTA_COMPLEMENT);
				QuakeSetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
				QuakeSetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
			}
			else if (tex->internalFormat == 4)
			{
				QuakeSetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE | D3DTA_COMPLEMENT);
				QuakeSetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
				QuakeSetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			}
		}
		else if (gD3D.texEnvMode[1] == GL_REPLACE)
		{
			if (tex->internalFormat == 1 || tex->internalFormat == 3)
			{
				QuakeSetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				QuakeSetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
				QuakeSetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
			}
			else if (tex->internalFormat == 4)
			{
				QuakeSetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				QuakeSetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
				QuakeSetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
			}
		}
		else if (gD3D.texEnvMode[1] == GL_MODULATE)
		{
			if (tex->internalFormat == 1 || tex->internalFormat == 3)
			{
				QuakeSetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				QuakeSetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
				QuakeSetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
			}
			else if (tex->internalFormat == 4)
			{
				QuakeSetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				QuakeSetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
				QuakeSetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			}
		}
		else if (gD3D.texEnvMode[1] == GL_DECAL)
		{
			if (tex->internalFormat == 3)
			{
				QuakeSetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				QuakeSetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
				QuakeSetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
			}
			else if (tex->internalFormat == 4)
			{
				QuakeSetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				QuakeSetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
				QuakeSetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
			}
		}
	}

	gD3D.textureValid = TRUE;
}

DLL_EXPORT void APIENTRY glBindTexture( GLenum target, GLuint texture )
{
	gD3D.currentTexture[gD3D.textureStage] = texture;
	gD3D.textureValid = FALSE;
}

DLL_EXPORT void APIENTRY glBitmap( GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte* bitmap )
{
}

DLL_EXPORT void APIENTRY glBlendFunc( GLenum sfactor, GLenum dfactor )
{
	DWORD	srcBlend;
	DWORD	destBlend;
	DWORD	dummy;

	srcBlend = -1;
	destBlend = -1;

	switch (sfactor)
	{
	case GL_ZERO:
		srcBlend = D3DBLEND_ZERO;
		break;
	case GL_ONE:
		srcBlend = D3DBLEND_ONE;
		break;
	case GL_SRC_ALPHA:
		srcBlend = D3DBLEND_SRCALPHA;
		break;
	case GL_ONE_MINUS_SRC_ALPHA:
		srcBlend = D3DBLEND_INVSRCALPHA;
		break;
	case GL_DST_ALPHA:
		srcBlend = D3DBLEND_DESTALPHA;
		break;
	case GL_ONE_MINUS_DST_ALPHA:
		srcBlend = D3DBLEND_INVDESTALPHA;
		break;
	case GL_DST_COLOR:
		srcBlend = D3DBLEND_DESTCOLOR;
		break;
	case GL_ONE_MINUS_DST_COLOR:
		srcBlend = D3DBLEND_INVDESTCOLOR;
		break;
	case GL_SRC_ALPHA_SATURATE:
		srcBlend = D3DBLEND_SRCALPHASAT;
		break;
	}

	switch (dfactor)
	{
	case GL_ZERO:
		destBlend = D3DBLEND_ZERO;
		break;
	case GL_ONE:
		destBlend = D3DBLEND_ONE;
		break;
	case GL_SRC_COLOR:
		destBlend = D3DBLEND_SRCCOLOR;
		break;
	case GL_ONE_MINUS_SRC_COLOR:
		destBlend = D3DBLEND_INVSRCCOLOR;
		break;
	case GL_SRC_ALPHA:
		destBlend = D3DBLEND_SRCALPHA;
		break;
	case GL_ONE_MINUS_SRC_ALPHA:
		destBlend = D3DBLEND_INVSRCALPHA;
		break;
	case GL_DST_ALPHA:
		destBlend = D3DBLEND_DESTALPHA;
		break;
	case GL_ONE_MINUS_DST_ALPHA:
		destBlend = D3DBLEND_INVDESTALPHA;
		break;
	}

	if (srcBlend >= 0)
	{
		if (gD3D.rStates[D3DRENDERSTATE_SRCBLEND] != srcBlend)
		{
			if (gD3D.indexCount)
			{
				if (gD3D.vertStart != gD3D.vertCount)
				{
					gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
					gD3D.vertStart = gD3D.vertCount;
				}

				gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
				gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
				gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dummy);

				gD3D.vertStart = 0;
				gD3D.vertCount = 0;
				gD3D.indexCount = 0;
			}

			gD3D.rStates[D3DRENDERSTATE_SRCBLEND] = srcBlend;
			gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_SRCBLEND, srcBlend);
		}
	}

	if (destBlend >= 0)
	{
		if (gD3D.rStates[D3DRENDERSTATE_DESTBLEND] != destBlend)
		{
			if (gD3D.indexCount)
			{
				if (gD3D.vertStart != gD3D.vertCount)
				{
					gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
					gD3D.vertStart = gD3D.vertCount;
				}

				gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
				gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
				gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dummy);

				gD3D.vertStart = 0;
				gD3D.vertCount = 0;
				gD3D.indexCount = 0;
			}

			gD3D.rStates[D3DRENDERSTATE_DESTBLEND] = destBlend;
			gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_DESTBLEND, destBlend);
		}
	}
}

DLL_EXPORT void APIENTRY glCallList( GLuint list )
{
}

DLL_EXPORT void APIENTRY glCallLists( GLsizei n, GLenum type, const GLvoid* lists )
{
}

DLL_EXPORT void APIENTRY glClear( GLbitfield mask )
{
	D3DRECT	screenRect;
	DWORD	flags;
	DWORD	dummy;

	if (gD3D.indexCount)
	{
		if (gD3D.vertStart != gD3D.vertCount)
		{
			gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
			gD3D.vertStart = gD3D.vertCount;
		}

		gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
		gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
		gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dummy);

		gD3D.vertStart = 0;
		gD3D.vertCount = 0;
		gD3D.indexCount = 0;
	}

	flags = 0;
	if (mask & GL_COLOR_BUFFER_BIT)
	{
		flags = D3DCLEAR_TARGET;
	}
	else if (mask & GL_DEPTH_BUFFER_BIT)
	{
		flags = D3DCLEAR_ZBUFFER;
	}
	else if (mask & GL_STENCIL_BUFFER_BIT)
	{
		flags = D3DCLEAR_STENCIL;
	}

	// Set the rectangle to clear
	screenRect.x1 = 0;
	screenRect.y1 = 0;
	screenRect.x2 = gD3D.wndWidth;
	screenRect.y2 = gD3D.wndHeight;

	gD3D.lpD3DVP3->lpVtbl->Clear2(gD3D.lpD3DVP3, 1, &screenRect, flags, gD3D.clearColor, 1.0, 0);
}

DLL_EXPORT void APIENTRY glClearAccum( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha )
{
}

DLL_EXPORT void APIENTRY glClearColor( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha )
{
	static float scale = 255.0f;
	unsigned int r, g, b, a;

	a = (unsigned int)(alpha * scale);
	if (a > 255)
		a = 255;
	b = (unsigned int)(blue * scale);
	if (b > 255)
		b = 255;
	g = (unsigned int)(green * scale);
	if (g > 255)
		g = 255;
	r = (unsigned int)(red * scale);
	if (r > 255)
		r = 255;
	gD3D.clearColor = RGBA_MAKE(r, g, b, a);
}

DLL_EXPORT void APIENTRY glClearDepth( GLclampd depth )
{
}

DLL_EXPORT void APIENTRY glClearIndex( GLfloat c )
{
}

DLL_EXPORT void APIENTRY glClearStencil( GLint s )
{
}

DLL_EXPORT void APIENTRY glClipPlane( GLenum plane, const GLdouble* equation )
{
}

DLL_EXPORT void APIENTRY glColor3b(GLbyte red, GLbyte green, GLbyte blue)
{
}

DLL_EXPORT void APIENTRY glColor3bv( const GLbyte* v )
{
}

DLL_EXPORT void APIENTRY glColor3d( GLdouble red, GLdouble green, GLdouble blue )
{
}

DLL_EXPORT void APIENTRY glColor3dv( const GLdouble* v )
{
}

DLL_EXPORT void APIENTRY glColor3f( GLfloat red, GLfloat green, GLfloat blue )
{
	static float scale = 255.0f;
	unsigned int r, g, b;

	b = (unsigned int)(blue * scale);
	if (b > 255)
		b = 255;
	g = (unsigned int)(green * scale);
	if (g > 255)
		g = 255;
	r = (unsigned int)(red * scale);
	if (r > 255)
		r = 255;
	gD3D.color = RGBA_MAKE(r, g, b, 255);
}

DLL_EXPORT void APIENTRY glColor3fv( const GLfloat* v )
{
}

DLL_EXPORT void APIENTRY glColor3i( GLint red, GLint green, GLint blue )
{
}

DLL_EXPORT void APIENTRY glColor3iv( const GLint* v )
{
}

DLL_EXPORT void APIENTRY glColor3s( GLshort red, GLshort green, GLshort blue )
{
}

DLL_EXPORT void APIENTRY glColor3sv( const GLshort* v )
{
}

DLL_EXPORT void APIENTRY glColor3ub( GLubyte red, GLubyte green, GLubyte blue )
{
	gD3D.color = RGBA_MAKE(red, green, blue, 255);
}

DLL_EXPORT void APIENTRY glColor3ubv( const GLubyte* v )
{
	gD3D.color = RGBA_MAKE(v[0], v[1], v[2], 255);
}

DLL_EXPORT void APIENTRY glColor3ui( GLuint red, GLuint green, GLuint blue )
{
}

DLL_EXPORT void APIENTRY glColor3uiv( const GLuint* v )
{
}

DLL_EXPORT void APIENTRY glColor3us( GLushort red, GLushort green, GLushort blue )
{
}

DLL_EXPORT void APIENTRY glColor3usv( const GLushort* v )
{
}

DLL_EXPORT void APIENTRY glColor4b( GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha )
{
}

DLL_EXPORT void APIENTRY glColor4bv( const GLbyte* v )
{
}

DLL_EXPORT void APIENTRY glColor4d( GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha )
{
}

DLL_EXPORT void APIENTRY glColor4dv( const GLdouble* v )
{
}

DLL_EXPORT void APIENTRY glColor4f( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha )
{
	static float scale = 255.0f;
	unsigned int r, g, b, a;

	a = (unsigned int)(alpha * scale);
	if (a > 255)
		a = 255;
	b = (unsigned int)(blue * scale);
	if (b > 255)
		b = 255;
	g = (unsigned int)(green * scale);
	if (g > 255)
		g = 255;
	r = (unsigned int)(red * scale);
	if (r > 255)
		r = 255;
	gD3D.color = RGBA_MAKE(r, g, b, a);
}

DLL_EXPORT void APIENTRY glColor4fv( const GLfloat* v )
{
	static float scale = 255.0f;
	unsigned int r, g, b, a;

	a = (unsigned int)(v[3] * scale);
	if (a > 255)
		a = 255;
	b = (unsigned int)(v[2] * scale);
	if (b > 255)
		b = 255;
	g = (unsigned int)(v[1] * scale);
	if (g > 255)
		g = 255;
	r = (unsigned int)(v[0] * scale);
	if (r > 255)
		r = 255;
	gD3D.color = RGBA_MAKE(r, g, b, a);
}

DLL_EXPORT void APIENTRY glColor4i( GLint red, GLint green, GLint blue, GLint alpha )
{
}

DLL_EXPORT void APIENTRY glColor4iv( const GLint* v )
{
}

DLL_EXPORT void APIENTRY glColor4s( GLshort red, GLshort green, GLshort blue, GLshort alpha )
{
}

DLL_EXPORT void APIENTRY glColor4sv( const GLshort* v )
{
}

DLL_EXPORT void APIENTRY glColor4ub( GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha )
{
	gD3D.color = RGBA_MAKE(red, green, blue, alpha);
}

DLL_EXPORT void APIENTRY glColor4ubv( const GLubyte* v )
{
	gD3D.color = RGBA_MAKE(v[0], v[1], v[2], v[3]);
}

DLL_EXPORT void APIENTRY glColor4ui( GLuint red, GLuint green, GLuint blue, GLuint alpha )
{
}

DLL_EXPORT void APIENTRY glColor4uiv( const GLuint* v )
{
}

DLL_EXPORT void APIENTRY glColor4us( GLushort red, GLushort green, GLushort blue, GLushort alpha )
{
}

DLL_EXPORT void APIENTRY glColor4usv( const GLushort* v )
{
}

DLL_EXPORT void APIENTRY glColorMask( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha )
{
}

DLL_EXPORT void APIENTRY glColorMaterial( GLenum face, GLenum mode )
{
}

DLL_EXPORT void APIENTRY glColorPointer( GLint size, GLenum type, GLsizei stride, const GLvoid* pointer )
{
	gD3D.colorPointer = pointer;

	if (size == 3 || type == GL_FLOAT || stride == 0)
	{
	}
	else
	{
		OutputDebugString("Wrapper: unsupported color array\n");
	}
}

DLL_EXPORT void APIENTRY glCopyPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum type )
{
}

DLL_EXPORT void APIENTRY glCopyTexImage1D( GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border )
{
}

DLL_EXPORT void APIENTRY glCopyTexImage2D( GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border )
{
}

DLL_EXPORT void APIENTRY glCopyTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width )
{
}

DLL_EXPORT void APIENTRY glCopyTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height )
{
}

DLL_EXPORT void APIENTRY glCullFace( GLenum mode )
{
	DWORD cullMode;
	DWORD dummy;

	gD3D.cullFaceMode = mode;

	if (gD3D.cullEnabled)
	{
		// Set cull mode
		if (mode == GL_BACK)
		{
			cullMode = D3DCULL_CW; 
		}
		else
		{
			cullMode = D3DCULL_CCW;
		}

		if (gD3D.rStates[D3DRENDERSTATE_CULLMODE] != cullMode)
		{
			if (gD3D.indexCount)
			{
				if (gD3D.vertStart != gD3D.vertCount)
				{
					gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
					gD3D.vertStart = gD3D.vertCount;
				}

				gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
				gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
				gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dummy);

				gD3D.vertStart = 0;
				gD3D.vertCount = 0;
				gD3D.indexCount = 0;
			}

			gD3D.rStates[D3DRENDERSTATE_CULLMODE] = cullMode;
			gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_CULLMODE, cullMode);
		}
	}
}

DLL_EXPORT void APIENTRY glDeleteLists( GLuint list, GLsizei range )
{
}

DLL_EXPORT void APIENTRY glDeleteTextures( GLsizei n, const GLuint* textures )
{
	D3D_TEXTURE* tex;
	int i;

	for (i = 0; i < n; i++, textures++)
	{
		tex = &gD3D.textures[*textures];

		if (tex->lpD3DT2)
		{
			tex->lpD3DT2->lpVtbl->Release(tex->lpD3DT2);
			tex->lpD3DT2 = NULL;
		}

		if (tex->lpDDS4)
		{
			tex->lpDDS4->lpVtbl->Release(tex->lpDDS4);
			tex->lpDDS4 = NULL;
		}

		// Reset texture parameters
		tex->minFilter = D3DTFN_POINT;
		tex->magFilter = D3DTFG_LINEAR;
		tex->mipFilter = D3DTFP_LINEAR;
		tex->addressU = D3DTADDRESS_WRAP;
		tex->addressV = D3DTADDRESS_WRAP;
	}
}

DLL_EXPORT void APIENTRY glDepthFunc( GLenum func )
{
	DWORD	zFunc;
	DWORD	dummy;

	zFunc = -1;

	switch (func)
	{
	case GL_NEVER:
		zFunc = D3DCMP_NEVER;
		break;
	case GL_LESS:
		zFunc = D3DCMP_LESS;
		break;
	case GL_EQUAL:
		zFunc = D3DCMP_EQUAL;
		break;
	case GL_LEQUAL:
		zFunc = D3DCMP_LESSEQUAL;
		break;
	case GL_GREATER:
		zFunc = D3DCMP_GREATER;
		break;
	case GL_NOTEQUAL:
		zFunc = D3DCMP_NOTEQUAL;
		break;
	case GL_GEQUAL:
		zFunc = D3DCMP_GREATEREQUAL;
		break;
	case GL_ALWAYS:
		zFunc = D3DCMP_ALWAYS;
		break;
	}

	if (zFunc >= 0)
	{
		if (gD3D.rStates[D3DRENDERSTATE_ZFUNC] != zFunc)
		{
			if (gD3D.indexCount)
			{
				if (gD3D.vertStart != gD3D.vertCount)
				{
					gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
					gD3D.vertStart = gD3D.vertCount;
				}

				gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
				gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
				gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dummy);

				gD3D.vertStart = 0;
				gD3D.vertCount = 0;
				gD3D.indexCount = 0;
			}

			gD3D.rStates[D3DRENDERSTATE_ZFUNC] = zFunc;
			gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_ZFUNC, zFunc);
		}
	}
}

DLL_EXPORT void APIENTRY glDepthMask( GLboolean flag )
{
	DWORD	dummy;

	if (flag)
	{
		if (gD3D.rStates[D3DRENDERSTATE_ZWRITEENABLE] != TRUE)
		{
			if (gD3D.indexCount)
			{
				if (gD3D.vertStart != gD3D.vertCount)
				{
					gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
					gD3D.vertStart = gD3D.vertCount;
				}

				gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
				gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
				gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dummy);

				gD3D.vertStart = 0;
				gD3D.vertCount = 0;
				gD3D.indexCount = 0;
			}

			gD3D.rStates[D3DRENDERSTATE_ZWRITEENABLE] = TRUE;
			gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_ZWRITEENABLE, TRUE);
		}
	}
	else
	{
		if (gD3D.rStates[D3DRENDERSTATE_ZWRITEENABLE] != FALSE)
		{
			if (gD3D.indexCount)
			{
				if (gD3D.vertStart != gD3D.vertCount)
				{
					gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
					gD3D.vertStart = gD3D.vertCount;
				}

				gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
				gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
				gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dummy);

				gD3D.vertStart = 0;
				gD3D.vertCount = 0;
				gD3D.indexCount = 0;
			}

			gD3D.rStates[D3DRENDERSTATE_ZWRITEENABLE] = FALSE;
			gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_ZWRITEENABLE, FALSE);
		}
	}
}

DLL_EXPORT void APIENTRY glDepthRange( GLclampd zNear, GLclampd zFar )
{
	D3DVIEWPORT2	vport;
	D3DVALUE	sum, diff;
	DWORD	dummy;

	if (gD3D.indexCount)
	{
		if (gD3D.vertStart != gD3D.vertCount)
		{
			gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
			gD3D.vertStart = gD3D.vertCount;
		}

		gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
		gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
		gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dummy);

		gD3D.vertStart = 0;
		gD3D.vertCount = 0;
		gD3D.indexCount = 0;
	}

	vport.dwSize = sizeof(vport);
	gD3D.lpD3DVP3->lpVtbl->GetViewport2(gD3D.lpD3DVP3, &vport);

	// Set new depth range
	sum = (D3DVALUE)(zFar + zNear);
	diff = (D3DVALUE)(zFar - zNear);
	vport.dvMinZ = -sum / diff;
	vport.dvMaxZ = -(sum - 2.0f) / diff;

	gD3D.lpD3DVP3->lpVtbl->SetViewport2(gD3D.lpD3DVP3, &vport);
}

DLL_EXPORT void APIENTRY glDisable( GLenum cap )
{
	DWORD	dummy;

	if (cap == GL_CULL_FACE)
	{
		gD3D.cullEnabled = FALSE;

		if (gD3D.rStates[D3DRENDERSTATE_CULLMODE] != D3DCULL_NONE)
		{
			QuakeFlushIndexedPrimitives();
			gD3D.rStates[D3DRENDERSTATE_CULLMODE] = D3DCULL_NONE;
			gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);
		}
	}
	else if (cap == GL_FOG)
	{
		if (gD3D.rStates[D3DRENDERSTATE_FOGENABLE] != FALSE)
		{
			QuakeFlushIndexedPrimitives();
			gD3D.rStates[D3DRENDERSTATE_FOGENABLE] = FALSE;
			gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_FOGENABLE, FALSE);
		}

		if (gD3D.rStates[D3DRENDERSTATE_RANGEFOGENABLE] != FALSE)
		{
			QuakeFlushIndexedPrimitives();
			gD3D.rStates[D3DRENDERSTATE_RANGEFOGENABLE] = FALSE;
			gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_RANGEFOGENABLE, FALSE);
		}
	}
	else if (cap == GL_DEPTH_TEST)
	{
		if (gD3D.rStates[D3DRENDERSTATE_ZENABLE] != FALSE)
		{
			QuakeFlushIndexedPrimitives();
			gD3D.rStates[D3DRENDERSTATE_ZENABLE] = FALSE;
			gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_ZENABLE, FALSE);
		}
	}
	else if (cap == GL_ALPHA_TEST)
	{
		if (gD3D.rStates[D3DRENDERSTATE_ALPHATESTENABLE] != FALSE)
		{
			QuakeFlushIndexedPrimitives();
			gD3D.rStates[D3DRENDERSTATE_ALPHATESTENABLE] = FALSE;
			gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_ALPHATESTENABLE, FALSE);
		}
	}
	else if (cap == GL_BLEND)
	{
		if (gD3D.rStates[D3DRENDERSTATE_ALPHABLENDENABLE] != FALSE)
		{
			QuakeFlushIndexedPrimitives();
			gD3D.rStates[D3DRENDERSTATE_ALPHABLENDENABLE] = FALSE;
			gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
		}
	}
	else if (cap == GL_SCISSOR_TEST)
	{
	}
	else if (cap == GL_TEXTURE_2D)
	{
		if (gD3D.textureStage)
		{
			if (gD3D.tsStates1[D3DTSS_COLOROP] != D3DTOP_DISABLE)
			{
				QuakeFlushIndexedPrimitives();
				gD3D.tsStates1[D3DTSS_COLOROP] = D3DTOP_DISABLE;
				gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 1, D3DTSS_COLOROP, D3DTOP_DISABLE);
			}

			if (gD3D.tsStates1[D3DTSS_ALPHAOP] != D3DTOP_DISABLE)
			{
				QuakeFlushIndexedPrimitives();
				gD3D.tsStates1[D3DTSS_ALPHAOP] = D3DTOP_DISABLE;
				gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
			}

			gD3D.useSubStage = FALSE;
		}
		else
		{
			if (gD3D.tsStates0[D3DTSS_COLOROP] != D3DTOP_DISABLE)
			{
				if (gD3D.indexCount)
				{
					QuakeFlushVertexBuffer();

					gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
					gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
					gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dummy);

					gD3D.vertStart = 0;
					gD3D.vertCount = 0;
					gD3D.indexCount = 0;
				}

				gD3D.tsStates0[D3DTSS_COLOROP] = D3DTOP_DISABLE;
				gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_COLOROP, D3DTOP_DISABLE);
			}

			if (gD3D.tsStates0[D3DTSS_ALPHAOP] != D3DTOP_DISABLE)
			{
				QuakeFlushIndexedPrimitives();
				gD3D.tsStates0[D3DTSS_ALPHAOP] = D3DTOP_DISABLE;
				gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
			}

			gD3D.stage0Active = FALSE;
		}

		gD3D.textureValid = FALSE;
	}
	else if (cap == GL_POLYGON_OFFSET_FILL)
	{
		D3DVIEWPORT2	vport;

		if (gD3D.indexCount)
		{
			if (gD3D.vertStart != gD3D.vertCount)
			{
				gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
				gD3D.vertStart = gD3D.vertCount;
			}

			gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
			gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
			gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dummy);

			gD3D.vertStart = 0;
			gD3D.vertCount = 0;
			gD3D.indexCount = 0;
		}

		vport.dwSize = sizeof(vport);
		gD3D.lpD3DVP3->lpVtbl->GetViewport2(gD3D.lpD3DVP3, &vport);

		vport.dvMaxZ = gD3D.dvMaxZ;
		gD3D.lpD3DVP3->lpVtbl->SetViewport2(gD3D.lpD3DVP3, &vport);
	}
	else
	{
		OutputDebugString("Wrapper: glDisable on this cap not supported\n");
		return;
	}
}

DLL_EXPORT void APIENTRY glDisableClientState( GLenum array )
{
	switch (array)
	{
	case GL_VERTEX_ARRAY:
		break;
	case GL_COLOR_ARRAY:
		break;
	default:
		OutputDebugString("Wrapper: array not supported\n");
		break;
	}		
}

DLL_EXPORT void APIENTRY glDrawArrays( GLenum mode, GLint first, GLsizei count )
{
}

DLL_EXPORT void APIENTRY glDrawBuffer( GLenum mode )
{
}

DLL_EXPORT void APIENTRY glDrawElements( GLenum mode, GLsizei count, GLenum type, const GLvoid* indices )
{
}

DLL_EXPORT void APIENTRY glDrawPixels( GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels )
{
}

DLL_EXPORT void APIENTRY glEdgeFlag( GLboolean flag )
{
}

DLL_EXPORT void APIENTRY glEdgeFlagPointer( GLsizei stride, const GLvoid* pointer )
{
}

DLL_EXPORT void APIENTRY glEdgeFlagv( const GLboolean* flag )
{
}

DLL_EXPORT void APIENTRY glEnable( GLenum cap )
{
	DWORD	dummy;

	if (cap == GL_CULL_FACE)
	{
		gD3D.cullEnabled = TRUE;

		if (gD3D.cullFaceMode == GL_BACK)
		{
			if (gD3D.rStates[D3DRENDERSTATE_CULLMODE] != D3DCULL_CW)
			{
				if (gD3D.indexCount)
				{
					QuakeFlushVertexBuffer();

					gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
					gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
					gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dummy);

					gD3D.vertStart = 0;
					gD3D.vertCount = 0;
					gD3D.indexCount = 0;
				}

				gD3D.rStates[D3DRENDERSTATE_CULLMODE] = D3DCULL_CW;
				gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_CULLMODE, D3DCULL_CW);
			}
		}
		else
		{
			if (gD3D.rStates[D3DRENDERSTATE_CULLMODE] != D3DCULL_CCW)
			{
				if (gD3D.indexCount)
				{
					QuakeFlushVertexBuffer();

					gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
					gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
					gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dummy);

					gD3D.vertStart = 0;
					gD3D.vertCount = 0;
					gD3D.indexCount = 0;
				}

				gD3D.rStates[D3DRENDERSTATE_CULLMODE] = D3DCULL_CCW;
				gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_CULLMODE, D3DCULL_CCW);
			}
		}
	}
	else if (cap == GL_FOG)
	{
		if (gD3D.rStates[D3DRENDERSTATE_FOGENABLE] != TRUE)
		{
			QuakeFlushIndexedPrimitives();
			gD3D.rStates[D3DRENDERSTATE_FOGENABLE] = TRUE;
			gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_FOGENABLE, TRUE);
		}

		if (gD3D.rStates[D3DRENDERSTATE_RANGEFOGENABLE] != TRUE)
		{
			if (gD3D.indexCount)
			{
				QuakeFlushVertexBuffer();

				gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
				gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
				gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dummy);

				gD3D.vertStart = 0;
				gD3D.vertCount = 0;
				gD3D.indexCount = 0;
			}

			gD3D.rStates[D3DRENDERSTATE_RANGEFOGENABLE] = TRUE;
			gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_RANGEFOGENABLE, TRUE);
		}
	}
	else if (cap == GL_DEPTH_TEST)
	{
		if (gD3D.rStates[D3DRENDERSTATE_ZENABLE] != TRUE)
		{
			if (gD3D.indexCount)
			{
				QuakeFlushVertexBuffer();

				gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
				gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
				gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dummy);

				gD3D.vertStart = 0;
				gD3D.vertCount = 0;
				gD3D.indexCount = 0;
			}

			gD3D.rStates[D3DRENDERSTATE_ZENABLE] = TRUE;
			gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_ZENABLE, TRUE);
		}
	}
	else if (cap == GL_ALPHA_TEST)
	{
		if (gD3D.rStates[D3DRENDERSTATE_ALPHATESTENABLE] != TRUE)
		{
			if (gD3D.indexCount)
			{
				QuakeFlushVertexBuffer();

				gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
				gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
				gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dummy);

				gD3D.vertStart = 0;
				gD3D.vertCount = 0;
				gD3D.indexCount = 0;
			}

			gD3D.rStates[D3DRENDERSTATE_ALPHATESTENABLE] = TRUE;
			gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_ALPHATESTENABLE, TRUE);
		}
	}
	else if (cap == GL_BLEND)
	{
		if (gD3D.rStates[D3DRENDERSTATE_ALPHABLENDENABLE] != TRUE)
		{
			QuakeFlushIndexedPrimitives();
			gD3D.rStates[D3DRENDERSTATE_ALPHABLENDENABLE] = TRUE;
			gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
		}
	}
	else if (cap == GL_SCISSOR_TEST)
	{
	}
	else if (cap == GL_TEXTURE_2D)
	{
		if (gD3D.textureStage)
		{
			gD3D.useSubStage = TRUE;
		}
		else
		{
			gD3D.stage0Active = TRUE;
		}

		gD3D.textureValid = FALSE;
	}
	else if (cap == GL_POLYGON_OFFSET_FILL)
	{
		D3DVIEWPORT2	vport;

		vport.dwSize = sizeof(vport);
		gD3D.lpD3DVP3->lpVtbl->GetViewport2(gD3D.lpD3DVP3, &vport);

		gD3D.dvMaxZ = vport.dvMaxZ;
	}
	else
	{
		OutputDebugString("Wrapper: glEnable on this cap not supported\n");
		return;
	}
}

DLL_EXPORT void APIENTRY glEnableClientState( GLenum array )
{
	switch (array)
	{
	case GL_VERTEX_ARRAY:
		break;
	case GL_COLOR_ARRAY:
		break;
	default:
		OutputDebugString("Wrapper: array not supported\n");
		break;
	}
}

DLL_EXPORT void APIENTRY glEnd( void )
{
	DWORD	i;

	switch (gD3D.primMode)
	{
	case GL_TRIANGLES:
		for (i = 0; i < gD3D.primVertCount; i++)
		{
			gD3D.indexBuffer[gD3D.indexCount++] = gD3D.vertCount++;
		}
		break;
	case GL_TRIANGLE_STRIP:
		gD3D.primVertCount -= 2;
		for (i = 0; i < gD3D.primVertCount; i += 2)
		{
			gD3D.indexBuffer[gD3D.indexCount++] = gD3D.vertCount + 0;
			gD3D.indexBuffer[gD3D.indexCount++] = gD3D.vertCount + 1;
			gD3D.indexBuffer[gD3D.indexCount++] = gD3D.vertCount + 2;
			gD3D.vertCount++;

			if (i + 1 < gD3D.primVertCount)
			{
				gD3D.indexBuffer[gD3D.indexCount++] = gD3D.vertCount + 1;
				gD3D.indexBuffer[gD3D.indexCount++] = gD3D.vertCount + 0;
				gD3D.indexBuffer[gD3D.indexCount++] = gD3D.vertCount + 2;
				gD3D.vertCount++;
			}
		}
		gD3D.vertCount += 2;
		break;
	case GL_TRIANGLE_FAN:
	case GL_POLYGON:
		gD3D.primVertCount -= 1;
		for (i = 1; i < gD3D.primVertCount; i++)
		{
			gD3D.indexBuffer[gD3D.indexCount++] = gD3D.vertCount;
			gD3D.indexBuffer[gD3D.indexCount++] = gD3D.vertCount + i;
			gD3D.indexBuffer[gD3D.indexCount++] = gD3D.vertCount + i + 1;
		}
		gD3D.vertCount += (gD3D.primVertCount + 1);
		break;
	case GL_QUADS:
		for (i = 0; i < gD3D.primVertCount; i += 4)
		{
			gD3D.indexBuffer[gD3D.indexCount++] = gD3D.vertCount + 0;
			gD3D.indexBuffer[gD3D.indexCount++] = gD3D.vertCount + 1;
			gD3D.indexBuffer[gD3D.indexCount++] = gD3D.vertCount + 2;
			gD3D.indexBuffer[gD3D.indexCount++] = gD3D.vertCount + 0;
			gD3D.indexBuffer[gD3D.indexCount++] = gD3D.vertCount + 2;
			gD3D.indexBuffer[gD3D.indexCount++] = gD3D.vertCount + 3;
			gD3D.vertCount += 4;
		}
		break;
	}
}

DLL_EXPORT void APIENTRY glEndList( void )
{
}

DLL_EXPORT void APIENTRY glEvalCoord1d( GLdouble u )
{
}

DLL_EXPORT void APIENTRY glEvalCoord1dv( const GLdouble* u )
{
}

DLL_EXPORT void APIENTRY glEvalCoord1f( GLfloat u )
{
}

DLL_EXPORT void APIENTRY glEvalCoord1fv( const GLfloat* u )
{
}

DLL_EXPORT void APIENTRY glEvalCoord2d( GLdouble u, GLdouble v )
{
}

DLL_EXPORT void APIENTRY glEvalCoord2dv( const GLdouble* u )
{
}

DLL_EXPORT void APIENTRY glEvalCoord2f( GLfloat u, GLfloat v )
{
}

DLL_EXPORT void APIENTRY glEvalCoord2fv( const GLfloat* u )
{
}

DLL_EXPORT void APIENTRY glEvalMesh1( GLenum mode, GLint i1, GLint i2 )
{
}

DLL_EXPORT void APIENTRY glEvalMesh2( GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2 )
{
}

DLL_EXPORT void APIENTRY glEvalPoint1( GLint i )
{
}

DLL_EXPORT void APIENTRY glEvalPoint2( GLint i, GLint j )
{
}

DLL_EXPORT void APIENTRY glFeedbackBuffer( GLsizei size, GLenum type, GLfloat* buffer )
{
}

DLL_EXPORT void APIENTRY glFinish( void )
{
}

DLL_EXPORT void APIENTRY glFlush( void )
{
}

DLL_EXPORT void APIENTRY glFogf( GLenum pname, GLfloat param )
{
	DWORD	dummy;

	if (pname == GL_FOG_START)
	{
		if (gD3D.rStates[D3DRENDERSTATE_FOGTABLESTART] != (DWORD)param)
		{
			if (gD3D.indexCount)
			{
				if (gD3D.vertStart != gD3D.vertCount)
				{
					gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
					gD3D.vertStart = gD3D.vertCount;
				}

				gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
				gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
				gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dummy);

				gD3D.vertStart = 0;
				gD3D.vertCount = 0;
				gD3D.indexCount = 0;
			}

			gD3D.rStates[D3DRENDERSTATE_FOGTABLESTART] = (DWORD)param;
			gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_FOGTABLESTART, (DWORD)param);
		}
	}
	else if (pname == GL_FOG_END)
	{
		if (gD3D.rStates[D3DRENDERSTATE_FOGTABLEEND] != (DWORD)param)
		{
			if (gD3D.indexCount)
			{
				if (gD3D.vertStart != gD3D.vertCount)
				{
					gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
					gD3D.vertStart = gD3D.vertCount;
				}

				gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
				gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
				gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dummy);

				gD3D.vertStart = 0;
				gD3D.vertCount = 0;
				gD3D.indexCount = 0;
			}

			gD3D.rStates[D3DRENDERSTATE_FOGTABLEEND] = (DWORD)param;
			gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_FOGTABLEEND, (DWORD)param);
		}
	}
}

DLL_EXPORT void APIENTRY glFogfv( GLenum pname, const GLfloat* params )
{
	static int lastFogColor = 0;
	DWORD	fogColor;
	DWORD	dummy;

	fogColor = RGBA_MAKE((int)(params[0] * 255.0f), (int)(params[1] * 255.0f), (int)(params[2] * 255.0f), (int)(params[3] * 255.0f));
	lastFogColor = fogColor;

	if (gD3D.rStates[D3DRENDERSTATE_FOGCOLOR] != fogColor)
	{
		if (gD3D.indexCount)
		{
			if (gD3D.vertStart != gD3D.vertCount)
			{
				gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
				gD3D.vertStart = gD3D.vertCount;
			}

			gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
			gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
			gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dummy);

			gD3D.vertStart = 0;
			gD3D.vertCount = 0;
			gD3D.indexCount = 0;
		}

		gD3D.rStates[D3DRENDERSTATE_FOGCOLOR] = fogColor;
		gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_FOGCOLOR, fogColor);
	}
}

DLL_EXPORT void APIENTRY glFogi( GLenum pname, GLint param )
{
	DWORD	dummy;

	if (pname == GL_FOG_MODE)
	{
		if (param == GL_EXP)
		{
			if (gD3D.rStates[D3DRENDERSTATE_FOGTABLEMODE] != D3DFOG_EXP)
			{
				if (gD3D.indexCount)
				{
					if (gD3D.vertStart != gD3D.vertCount)
					{
						gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
						gD3D.vertStart = gD3D.vertCount;
					}

					gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
					gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
					gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dummy);

					gD3D.vertStart = 0;
					gD3D.vertCount = 0;
					gD3D.indexCount = 0;
				}

				gD3D.rStates[D3DRENDERSTATE_FOGTABLEMODE] = D3DFOG_EXP;
				gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_EXP);
			}
		}
		else if (param == GL_EXP2)
		{
			if (gD3D.rStates[D3DRENDERSTATE_FOGTABLEMODE] != D3DFOG_EXP2)
			{
				if (gD3D.indexCount)
				{
					if (gD3D.vertStart != gD3D.vertCount)
					{
						gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
						gD3D.vertStart = gD3D.vertCount;
					}

					gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
					gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
					gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dummy);

					gD3D.vertStart = 0;
					gD3D.vertCount = 0;
					gD3D.indexCount = 0;
				}

				gD3D.rStates[D3DRENDERSTATE_FOGTABLEMODE] = D3DFOG_EXP2;
				gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_EXP2);
			}
		}
		else if (param == GL_LINEAR)
		{
			if (gD3D.rStates[D3DRENDERSTATE_FOGTABLEMODE] != D3DFOG_LINEAR)
			{
				if (gD3D.indexCount)
				{
					if (gD3D.vertStart != gD3D.vertCount)
					{
						gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
						gD3D.vertStart = gD3D.vertCount;
					}

					gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
					gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
					gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dummy);

					gD3D.vertStart = 0;
					gD3D.vertCount = 0;
					gD3D.indexCount = 0;
				}

				gD3D.rStates[D3DRENDERSTATE_FOGTABLEMODE] = D3DFOG_LINEAR;
				gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_LINEAR);
			}
		}
		else
		{
			if (gD3D.rStates[D3DRENDERSTATE_FOGTABLEMODE] != D3DFOG_NONE)
			{
				if (gD3D.indexCount)
				{
					QuakeFlushVertexBuffer();

					gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
					gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
					gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dummy);

					gD3D.vertStart = 0;
					gD3D.vertCount = 0;
					gD3D.indexCount = 0;
				}

				gD3D.rStates[D3DRENDERSTATE_FOGTABLEMODE] = D3DFOG_NONE;
				gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_NONE);
			}
		}
	}
}

DLL_EXPORT void APIENTRY glFogiv( GLenum pname, const GLint* params )
{
}

DLL_EXPORT void APIENTRY glFrontFace( GLenum mode )
{
}

DLL_EXPORT void APIENTRY glFrustum( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar )
{
	D3DMATRIX	matrix;
	D3DVALUE	sum, diff, twoNear, zFarTwoNear;
	D3DVALUE	xSum, xRange, ySum, yRange;

	if (gD3D.vertStart != gD3D.vertCount)
	{
		gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
		gD3D.vertStart = gD3D.vertCount;
	}

	sum = (D3DVALUE)(zFar + zNear);
	diff = (D3DVALUE)(zFar - zNear);
	twoNear = (D3DVALUE)(zNear * 2.0f);
	zFarTwoNear = (D3DVALUE)(zFar * twoNear);

	xSum = (D3DVALUE)(right + left);
	ySum = (D3DVALUE)(top + bottom);
	xRange = (D3DVALUE)(right - left);
	yRange = (D3DVALUE)(top - bottom);

	// Create frustum matrix
	/*
		| (2*near)/(right-left)       0                   (right+left)/(right-left)              0               |
		|     0               (2*near)/(top-bottom)       (top+bottom)/(top-bottom)              0               |
		|     0                       0					   -(far+near)/(far-near)    -(2*far*near)/(far-near)    |
		|     0                       0                               -1                         0               |
	*/
	matrix._11 = twoNear / xRange;	matrix._12 = 0.0f;				matrix._13 = 0.0f;					matrix._14 = 0.0f;
	matrix._21 = 0.0f;				matrix._22 = twoNear / yRange;	matrix._23 = 0.0f;					matrix._24 = 0.0f;
	matrix._31 = xSum / xRange;		matrix._32 = ySum / yRange;		matrix._33 = -(sum / diff);			matrix._34 = -1.0f;
	matrix._41 = 0.0f;				matrix._42 = 0.0f;				matrix._43 = -(zFarTwoNear / diff);	matrix._44 = 0.0f;

	gD3D.lpD3DD3->lpVtbl->MultiplyTransform(gD3D.lpD3DD3, gD3D.transformState, &matrix);
}

DLL_EXPORT GLuint APIENTRY glGenLists( GLsizei range )
{
	return 0;
}

DLL_EXPORT void APIENTRY glGenTextures( GLsizei n, GLuint* textures )
{
}

DLL_EXPORT void APIENTRY glGetBooleanv( GLenum pname, GLboolean* params )
{
}

DLL_EXPORT void APIENTRY glGetClipPlane( GLenum plane, GLdouble* equation )
{
}

DLL_EXPORT void APIENTRY glGetDoublev( GLenum pname, GLdouble* params )
{
}

DLL_EXPORT GLenum APIENTRY glGetError( void )
{
	return GL_NO_ERROR;
}

DLL_EXPORT void APIENTRY glGetFloatv( GLenum pname, GLfloat* params )
{
	switch (pname)
	{
	case GL_MODELVIEW_MATRIX:
		gD3D.lpD3DD3->lpVtbl->GetTransform(gD3D.lpD3DD3, D3DTRANSFORMSTATE_WORLD, (LPD3DMATRIX)params);
		break;
	case GL_PROJECTION_MATRIX:
		gD3D.lpD3DD3->lpVtbl->GetTransform(gD3D.lpD3DD3, D3DTRANSFORMSTATE_PROJECTION, (LPD3DMATRIX)params);
		break;
	}
}

DLL_EXPORT void APIENTRY glGetIntegerv( GLenum pname, GLint* params )
{
}

DLL_EXPORT void APIENTRY glGetLightfv( GLenum light, GLenum pname, GLfloat* params )
{
}

DLL_EXPORT void APIENTRY glGetLightiv( GLenum light, GLenum pname, GLint* params )
{
}

DLL_EXPORT void APIENTRY glGetMapdv( GLenum target, GLenum query, GLdouble* v )
{
}

DLL_EXPORT void APIENTRY glGetMapfv( GLenum target, GLenum query, GLfloat* v )
{
}

DLL_EXPORT void APIENTRY glGetMapiv( GLenum target, GLenum query, GLint* v )
{
}

DLL_EXPORT void APIENTRY glGetMaterialfv( GLenum face, GLenum pname, GLfloat* params )
{
}

DLL_EXPORT void APIENTRY glGetMaterialiv( GLenum face, GLenum pname, GLint* params )
{
}

DLL_EXPORT void APIENTRY glGetPixelMapfv( GLenum map, GLfloat* values )
{
}

DLL_EXPORT void APIENTRY glGetPixelMapuiv( GLenum map, GLuint* values )
{
}

DLL_EXPORT void APIENTRY glGetPixelMapusv( GLenum map, GLushort* values )
{
}

DLL_EXPORT void APIENTRY glGetPointerv( GLenum pname, GLvoid** params )
{
}

DLL_EXPORT void APIENTRY glGetPolygonStipple( GLubyte* mask )
{
}

DLL_EXPORT const GLubyte* APIENTRY glGetString( GLenum name )
{
	const char* string;

	switch (name)
	{
	case GL_VENDOR:
		string = "Microsoft Corp.";
		break;
	case GL_RENDERER:
		string = "Direct3D";
		break;
	case GL_VERSION:
		string = "6.0";
		break;
	case GL_EXTENSIONS:
		if (gD3D.useMultitexture)
			string = "GL_SGIS_multitexture";
		else
			string = "";
		break;
	default:
		string = "";
		break;
	}
	return (const GLubyte*)string;
}

DLL_EXPORT void APIENTRY glGetTexEnvfv( GLenum target, GLenum pname, GLfloat* params )
{
}

DLL_EXPORT void APIENTRY glGetTexEnviv( GLenum target, GLenum pname, GLint* params )
{
}

DLL_EXPORT void APIENTRY glGetTexGendv( GLenum coord, GLenum pname, GLdouble* params )
{
}

DLL_EXPORT void APIENTRY glGetTexGenfv( GLenum coord, GLenum pname, GLfloat* params )
{
}

DLL_EXPORT void APIENTRY glGetTexGeniv( GLenum coord, GLenum pname, GLint* params )
{
}

DLL_EXPORT void APIENTRY glGetTexImage( GLenum target, GLint level, GLenum format, GLenum type, GLvoid* pixels )
{
}

DLL_EXPORT void APIENTRY glGetTexLevelParameterfv( GLenum target, GLint level, GLenum pname, GLfloat* params )
{
}

DLL_EXPORT void APIENTRY glGetTexLevelParameteriv( GLenum target, GLint level, GLenum pname, GLint* params )
{
}

DLL_EXPORT void APIENTRY glGetTexParameterfv( GLenum target, GLenum pname, GLfloat* params )
{
}

DLL_EXPORT void APIENTRY glGetTexParameteriv( GLenum target, GLenum pname, GLint* params )
{
}

DLL_EXPORT void APIENTRY glHint( GLenum target, GLenum mode )
{
}

DLL_EXPORT void APIENTRY glIndexMask( GLuint mask )
{
}

DLL_EXPORT void APIENTRY glIndexPointer( GLenum type, GLsizei stride, const GLvoid* pointer )
{
}

DLL_EXPORT void APIENTRY glIndexd( GLdouble c )
{
}

DLL_EXPORT void APIENTRY glIndexdv( const GLdouble* c )
{
}

DLL_EXPORT void APIENTRY glIndexf( GLfloat c )
{
}

DLL_EXPORT void APIENTRY glIndexfv( const GLfloat* c )
{
}

DLL_EXPORT void APIENTRY glIndexi( GLint c )
{
}

DLL_EXPORT void APIENTRY glIndexiv( const GLint* c )
{
}

DLL_EXPORT void APIENTRY glIndexs( GLshort c )
{
}

DLL_EXPORT void APIENTRY glIndexsv( const GLshort* c )
{
}

DLL_EXPORT void APIENTRY glIndexub( GLubyte c )
{
}

DLL_EXPORT void APIENTRY glIndexubv( const GLubyte* c )
{
}

DLL_EXPORT void APIENTRY glInitNames( void )
{
}

DLL_EXPORT void APIENTRY glInterleavedArrays( GLenum format, GLsizei stride, const GLvoid* pointer )
{
}

DLL_EXPORT GLboolean APIENTRY glIsEnabled( GLenum cap )
{
	return GL_FALSE;
}

DLL_EXPORT GLboolean APIENTRY glIsList( GLuint list )
{
	return GL_FALSE;
}

DLL_EXPORT GLboolean APIENTRY glIsTexture( GLuint texture )
{
	return GL_FALSE;
}

DLL_EXPORT void APIENTRY glLightModelf( GLenum pname, GLfloat param )
{
}

DLL_EXPORT void APIENTRY glLightModelfv( GLenum pname, const GLfloat* params )
{
}

DLL_EXPORT void APIENTRY glLightModeli( GLenum pname, GLint param )
{
}

DLL_EXPORT void APIENTRY glLightModeliv( GLenum pname, const GLint* params )
{
}

DLL_EXPORT void APIENTRY glLightf( GLenum light, GLenum pname, GLfloat param )
{
}

DLL_EXPORT void APIENTRY glLightfv( GLenum light, GLenum pname, const GLfloat* params )
{
}

DLL_EXPORT void APIENTRY glLighti( GLenum light, GLenum pname, GLint param )
{
}

DLL_EXPORT void APIENTRY glLightiv( GLenum light, GLenum pname, const GLint* params )
{
}

DLL_EXPORT void APIENTRY glLineStipple( GLint factor, GLushort pattern )
{
}

DLL_EXPORT void APIENTRY glLineWidth( GLfloat width )
{
}

DLL_EXPORT void APIENTRY glListBase( GLuint base )
{
}

DLL_EXPORT void APIENTRY glLoadIdentity( void )
{
	D3DMATRIX	matrix;

	if (gD3D.vertStart != gD3D.vertCount)
	{
		gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
		gD3D.vertStart = gD3D.vertCount;
	}

	// Create identity matrix
	/*
		| 1 0 0 0 |
		| 0 1 0 0 |
		| 0 0 1 0 |
		| 0 0 0 1 |
	*/
	matrix._11 = 1.0;	matrix._12 = 0.0;	matrix._13 = 0.0;	matrix._14 = 0.0;
	matrix._21 = 0.0;	matrix._22 = 1.0;	matrix._23 = 0.0;	matrix._24 = 0.0;
	matrix._31 = 0.0;	matrix._32 = 0.0;	matrix._33 = 1.0;	matrix._34 = 0.0;
	matrix._41 = 0.0;	matrix._42 = 0.0;	matrix._43 = 0.0;	matrix._44 = 1.0;

	gD3D.lpD3DD3->lpVtbl->SetTransform(gD3D.lpD3DD3, gD3D.transformState, &matrix);
}

DLL_EXPORT void APIENTRY glLoadMatrixd( const GLdouble* m )
{
}

DLL_EXPORT void APIENTRY glLoadMatrixf( const GLfloat* m )
{
	if (gD3D.vertStart != gD3D.vertCount)
	{
		gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
		gD3D.vertStart = gD3D.vertCount;
	}

	gD3D.lpD3DD3->lpVtbl->SetTransform(gD3D.lpD3DD3, gD3D.transformState, (LPD3DMATRIX)m);
}

DLL_EXPORT void APIENTRY glLoadName( GLuint name )
{
}

DLL_EXPORT void APIENTRY glLogicOp( GLenum opcode )
{
}

DLL_EXPORT void APIENTRY glMap1d( GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble* points )
{
}

DLL_EXPORT void APIENTRY glMap1f( GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat* points )
{
}

DLL_EXPORT void APIENTRY glMap2d( GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble* points )
{
}

DLL_EXPORT void APIENTRY glMap2f( GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat* points )
{
}

DLL_EXPORT void APIENTRY glMapGrid1d( GLint un, GLdouble u1, GLdouble u2 )
{
}

DLL_EXPORT void APIENTRY glMapGrid1f( GLint un, GLfloat u1, GLfloat u2 )
{
}

DLL_EXPORT void APIENTRY glMapGrid2d( GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2 )
{
}

DLL_EXPORT void APIENTRY glMapGrid2f( GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2 )
{
}

DLL_EXPORT void APIENTRY glMaterialf( GLenum face, GLenum pname, GLfloat param )
{
}

DLL_EXPORT void APIENTRY glMaterialfv( GLenum face, GLenum pname, const GLfloat* params )
{
}

DLL_EXPORT void APIENTRY glMateriali( GLenum face, GLenum pname, GLint param )
{
}

DLL_EXPORT void APIENTRY glMaterialiv( GLenum face, GLenum pname, const GLint* params )
{
}

DLL_EXPORT void APIENTRY glMatrixMode( GLenum mode )
{
	if (mode == GL_MODELVIEW)
	{
		gD3D.transformState = D3DTRANSFORMSTATE_WORLD;
	}
	else
	{
		gD3D.transformState = D3DTRANSFORMSTATE_PROJECTION;
	}
}

DLL_EXPORT void APIENTRY glMultMatrixd( const GLdouble* m )
{
}

DLL_EXPORT void APIENTRY glMultMatrixf( const GLfloat* m )
{
}

DLL_EXPORT void APIENTRY glNewList( GLuint list, GLenum mode )
{
}

DLL_EXPORT void APIENTRY glNormal3b( GLbyte nx, GLbyte ny, GLbyte nz )
{
}

DLL_EXPORT void APIENTRY glNormal3bv( const GLbyte* v )
{
}

DLL_EXPORT void APIENTRY glNormal3d( GLdouble nx, GLdouble ny, GLdouble nz )
{
}

DLL_EXPORT void APIENTRY glNormal3dv( const GLdouble* v )
{
}

DLL_EXPORT void APIENTRY glNormal3f( GLfloat nx, GLfloat ny, GLfloat nz )
{
}

DLL_EXPORT void APIENTRY glNormal3fv( const GLfloat* v )
{
}

DLL_EXPORT void APIENTRY glNormal3i( GLint nx, GLint ny, GLint nz )
{
}

DLL_EXPORT void APIENTRY glNormal3iv( const GLint* v )
{
}

DLL_EXPORT void APIENTRY glNormal3s( GLshort nx, GLshort ny, GLshort nz )
{
}

DLL_EXPORT void APIENTRY glNormal3sv( const GLshort* v )
{
}

DLL_EXPORT void APIENTRY glNormalPointer( GLenum type, GLsizei stride, const GLvoid* pointer )
{
}

DLL_EXPORT void APIENTRY glOrtho( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar )
{
	D3DMATRIX	matrix;
	D3DVALUE	sum, diff;
	D3DVALUE	xSum, xRange, ySum, yRange;

	if (gD3D.vertStart != gD3D.vertCount)
	{
		gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
		gD3D.vertStart = gD3D.vertCount;
	}

	sum = (D3DVALUE)(zFar + zNear);
	diff = (D3DVALUE)(zFar - zNear);

	xSum = (D3DVALUE)(right + left);
	ySum = (D3DVALUE)(top + bottom);
	xRange = (D3DVALUE)(right - left);
	yRange = (D3DVALUE)(top - bottom);

	// Create orthographic matrix
	/*
		| 2/(r-l)      0         0        -(r+l)/(r-l) |
		|    0      2/(t-b)      0        -(t+b)/(t-b) |
		|    0         0      -2/(f-n)    -(f+n)/(f-n) |
		|    0         0         0             1       |
	*/
	matrix._11 = 2.0f / xRange;		matrix._12 = 0.0f;				matrix._13 = 0.0f;				matrix._14 = 0.0f;
	matrix._21 = 0.0f;				matrix._22 = 2.0f / yRange;		matrix._23 = 0.0f;				matrix._24 = 0.0f;
	matrix._31 = 0.0f;				matrix._32 = 0.0f;				matrix._33 = -(2.0f / diff);	matrix._34 = 0.0f;
	matrix._41 = -(xSum / xRange);	matrix._42 = -(ySum / yRange);	matrix._43 = -(sum / diff);		matrix._44 = 1.0f;

	gD3D.lpD3DD3->lpVtbl->MultiplyTransform(gD3D.lpD3DD3, gD3D.transformState, &matrix);
}

DLL_EXPORT void APIENTRY glPassThrough( GLfloat token )
{
}

DLL_EXPORT void APIENTRY glPixelMapfv( GLenum map, GLsizei mapsize, const GLfloat* values )
{
}

DLL_EXPORT void APIENTRY glPixelMapuiv( GLenum map, GLsizei mapsize, const GLuint* values )
{
}

DLL_EXPORT void APIENTRY glPixelMapusv( GLenum map, GLsizei mapsize, const GLushort* values )
{
}

DLL_EXPORT void APIENTRY glPixelStoref( GLenum pname, GLfloat param )
{
}

DLL_EXPORT void APIENTRY glPixelStorei( GLenum pname, GLint param )
{
}

DLL_EXPORT void APIENTRY glPixelTransferf( GLenum pname, GLfloat param )
{
}

DLL_EXPORT void APIENTRY glPixelTransferi( GLenum pname, GLint param )
{
}

DLL_EXPORT void APIENTRY glPixelZoom( GLfloat xfactor, GLfloat yfactor )
{
}

DLL_EXPORT void APIENTRY glPointSize( GLfloat size )
{
}

DLL_EXPORT void APIENTRY glPolygonMode( GLenum face, GLenum mode )
{
	DWORD	fillMode;
	DWORD	dummy;

	fillMode = -1;

	switch (mode)
	{
	case GL_POINT:
		fillMode = D3DFILL_POINT;
		break;
	case GL_LINE:
		fillMode = D3DFILL_WIREFRAME;
		break;
	case GL_FILL:
		fillMode = D3DFILL_SOLID;
		break;
	}

	if (fillMode >= 0)
	{
		if (gD3D.rStates[D3DRENDERSTATE_FILLMODE] != fillMode)
		{
			if (gD3D.indexCount)
			{
				if (gD3D.vertStart != gD3D.vertCount)
				{
					gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
					gD3D.vertStart = gD3D.vertCount;
				}

				gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
				gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
				gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dummy);

				gD3D.vertStart = 0;
				gD3D.vertCount = 0;
				gD3D.indexCount = 0;
			}

			gD3D.rStates[D3DRENDERSTATE_FILLMODE] = fillMode;
			gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_FILLMODE, fillMode);
		}
	}
}

DLL_EXPORT void APIENTRY glPolygonOffset( GLfloat factor, GLfloat units )
{
	D3DVIEWPORT2	vport;
	DWORD	dummy;

	if (gD3D.indexCount)
	{
		if (gD3D.vertStart != gD3D.vertCount)
		{
			gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
			gD3D.vertStart = gD3D.vertCount;
		}

		gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
		gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
		gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dummy);

		gD3D.vertStart = 0;
		gD3D.vertCount = 0;
		gD3D.indexCount = 0;
	}

	vport.dwSize = sizeof(vport);
	gD3D.lpD3DVP3->lpVtbl->GetViewport2(gD3D.lpD3DVP3, &vport);

	// Adjust depth range to add polygon offset
	vport.dvMaxZ = (units + 1.0f) * gD3D.dvMaxZ;

	gD3D.lpD3DVP3->lpVtbl->SetViewport2(gD3D.lpD3DVP3, &vport);
}

DLL_EXPORT void APIENTRY glPolygonStipple( const GLubyte* mask )
{
}

DLL_EXPORT void APIENTRY glPopAttrib( void )
{
}

DLL_EXPORT void APIENTRY glPopClientAttrib( void )
{
}

DLL_EXPORT void APIENTRY glPopMatrix( void )
{
	D3D_MATRIXCHAIN* pTop;

	if (gD3D.vertStart != gD3D.vertCount)
	{
		gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
		gD3D.vertStart = gD3D.vertCount;
	}

	if (gD3D.transformState == D3DTRANSFORMSTATE_WORLD)
	{
		gD3D.lpD3DD3->lpVtbl->SetTransform(gD3D.lpD3DD3, D3DTRANSFORMSTATE_WORLD, &gD3D.worldMatrixStack->matrix);

		pTop = gD3D.worldMatrixStack;
		gD3D.worldMatrixStack = pTop->pNext;
		delete pTop;
		gD3D.totalWorldMatrices--;
	}
	else
	{
		gD3D.lpD3DD3->lpVtbl->SetTransform(gD3D.lpD3DD3, gD3D.transformState, &gD3D.customMatrixStack->matrix);

		pTop = gD3D.customMatrixStack;
		gD3D.customMatrixStack = gD3D.customMatrixStack->pNext;
		delete pTop;
		gD3D.totalCustomMatrices--;
	}
}

DLL_EXPORT void APIENTRY glPopName( void )
{
}

DLL_EXPORT void APIENTRY glPrioritizeTextures( GLsizei n, const GLuint* textures, const GLclampf* priorities )
{
}

DLL_EXPORT void APIENTRY glPushAttrib( GLbitfield mask )
{
}

DLL_EXPORT void APIENTRY glPushClientAttrib( GLbitfield mask )
{
}

DLL_EXPORT void APIENTRY glPushMatrix( void )
{
	D3DMATRIX	matrix;
	D3D_MATRIXCHAIN* pCurrent, * pNext;

	gD3D.lpD3DD3->lpVtbl->GetTransform(gD3D.lpD3DD3, gD3D.transformState, &matrix);

	if (gD3D.transformState == D3DTRANSFORMSTATE_WORLD)
	{
		pCurrent = new D3D_MATRIXCHAIN;
		if (pCurrent)
		{
			pNext = gD3D.worldMatrixStack;
			pCurrent->matrix = matrix;
			pCurrent->pNext = pNext;
			gD3D.worldMatrixStack = pCurrent;
		}

		gD3D.totalWorldMatrices++;
	}
	else
	{
		pCurrent = new D3D_MATRIXCHAIN;
		if (pCurrent)
		{
			pNext = gD3D.customMatrixStack;
			pCurrent->matrix = matrix;
			pCurrent->pNext = pNext;
			gD3D.customMatrixStack = pCurrent;
		}

		gD3D.totalCustomMatrices++;
	}
}

DLL_EXPORT void APIENTRY glPushName( GLuint name )
{
}

DLL_EXPORT void APIENTRY glRasterPos2d( GLdouble x, GLdouble y )
{
}

DLL_EXPORT void APIENTRY glRasterPos2dv( const GLdouble* v )
{
}

DLL_EXPORT void APIENTRY glRasterPos2f( GLfloat x, GLfloat y )
{
}

DLL_EXPORT void APIENTRY glRasterPos2fv( const GLfloat* v )
{
}

DLL_EXPORT void APIENTRY glRasterPos2i( GLint x, GLint y )
{
}

DLL_EXPORT void APIENTRY glRasterPos2iv( const GLint* v )
{
}

DLL_EXPORT void APIENTRY glRasterPos2s( GLshort x, GLshort y )
{
}

DLL_EXPORT void APIENTRY glRasterPos2sv( const GLshort* v )
{
}

DLL_EXPORT void APIENTRY glRasterPos3d( GLdouble x, GLdouble y, GLdouble z )
{
}

DLL_EXPORT void APIENTRY glRasterPos3dv( const GLdouble* v )
{
}

DLL_EXPORT void APIENTRY glRasterPos3f( GLfloat x, GLfloat y, GLfloat z )
{
}

DLL_EXPORT void APIENTRY glRasterPos3fv( const GLfloat* v )
{
}

DLL_EXPORT void APIENTRY glRasterPos3i( GLint x, GLint y, GLint z )
{
}

DLL_EXPORT void APIENTRY glRasterPos3iv( const GLint* v )
{
}

DLL_EXPORT void APIENTRY glRasterPos3s( GLshort x, GLshort y, GLshort z )
{
}

DLL_EXPORT void APIENTRY glRasterPos3sv( const GLshort* v )
{
}

DLL_EXPORT void APIENTRY glRasterPos4d( GLdouble x, GLdouble y, GLdouble z, GLdouble w )
{
}

DLL_EXPORT void APIENTRY glRasterPos4dv( const GLdouble* v )
{
}

DLL_EXPORT void APIENTRY glRasterPos4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
}

DLL_EXPORT void APIENTRY glRasterPos4fv( const GLfloat* v )
{
}

DLL_EXPORT void APIENTRY glRasterPos4i( GLint x, GLint y, GLint z, GLint w )
{
}

DLL_EXPORT void APIENTRY glRasterPos4iv( const GLint* v )
{
}

DLL_EXPORT void APIENTRY glRasterPos4s( GLshort x, GLshort y, GLshort z, GLshort w )
{
}

DLL_EXPORT void APIENTRY glRasterPos4sv( const GLshort* v )
{
}

DLL_EXPORT void APIENTRY glReadBuffer( GLenum mode )
{
}

DLL_EXPORT void APIENTRY glReadPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels )
{
	LPDIRECTDRAWSURFACE4 lpDDS;
	DDSURFACEDESC2 ddsd2;
	DWORD rBitMask, gBitMask, bBitMask;
	DWORD rLeftShift, gLeftShift, bLeftShift;
	DWORD rRightShift, gRightShift, bRightShift;
	int i, j;

	gD3D.pDXMgr->lpVtbl->GetPrimary(gD3D.pDXMgr, &lpDDS);

	memset(&ddsd2, 0, sizeof(DDSURFACEDESC2));
	ddsd2.dwSize = sizeof(ddsd2);

	if (gD3D.isFullscreen)
	{
		if (lpDDS->lpVtbl->Lock(lpDDS, NULL, &ddsd2, DDLOCK_WAIT | DDLOCK_READONLY, NULL) != DD_OK)
		{
			OutputDebugString("Wrapper: Lock on primary failed");
			lpDDS->lpVtbl->Release(lpDDS);
			return;
		}
	}
	else
	{
		POINT point_lt, point_rb;
		RECT rect;

		point_lt.x = 0;
		point_lt.y = 0;
		ClientToScreen(gD3D.hWnd, &point_lt);

		point_rb.x = gD3D.wndWidth;
		point_rb.y = gD3D.wndHeight;
		ClientToScreen(gD3D.hWnd, &point_rb);

		rect.left = point_lt.x;
		rect.top = point_lt.y;
		rect.right = point_rb.x;
		rect.bottom = point_rb.y;

		if (lpDDS->lpVtbl->Lock(lpDDS, &rect, &ddsd2, DDLOCK_WAIT | DDLOCK_READONLY, NULL) != DD_OK)
		{
			OutputDebugString("Wrapper: Lock on primary failed");
			lpDDS->lpVtbl->Release(lpDDS);
			return;
		}
	}

	rRightShift = 0;
	rBitMask = ddsd2.ddpfPixelFormat.dwRBitMask;
	while (!(rBitMask & 1))
	{
		rBitMask >>= 1;
		rRightShift++;
	}
	rLeftShift = 8;
	while (rBitMask & 1)
	{
		rBitMask >>= 1;
		rLeftShift--;
	}

	gRightShift = 0;
	gBitMask = ddsd2.ddpfPixelFormat.dwGBitMask;
	while (!(gBitMask & 1))
	{
		gBitMask >>= 1;
		gRightShift++;
	}
	gLeftShift = 8;
	while (gBitMask & 1)
	{
		gBitMask >>= 1;
		gLeftShift--;
	}

	bRightShift = 0;
	bBitMask = ddsd2.ddpfPixelFormat.dwBBitMask;
	while (!(bBitMask & 1))
	{
		bBitMask >>= 1;
		bRightShift++;
	}
	bLeftShift = 8;
	while (bBitMask & 1)
	{
		bBitMask >>= 1;
		bLeftShift--;
	}

	if (ddsd2.ddpfPixelFormat.dwRGBBitCount == 16)
	{
		WORD* src;
		BYTE* dest;

		src = (WORD*)ddsd2.lpSurface;
		dest = (BYTE*)pixels + gD3D.wndWidth * (gD3D.wndHeight - 1) * 3;
		for (j = 0; j < gD3D.wndHeight; j++)
		{
			for (i = 0; i < gD3D.wndWidth; i++)
			{
				*dest++ = ((WORD)(*src & ddsd2.ddpfPixelFormat.dwRBitMask) >> rRightShift) << rLeftShift;
				*dest++ = ((WORD)(*src & ddsd2.ddpfPixelFormat.dwGBitMask) >> gRightShift) << gLeftShift;
				*dest++ = ((WORD)(*src & ddsd2.ddpfPixelFormat.dwBBitMask) >> bRightShift) << bLeftShift;
				src++;
			}
			src = (WORD*)((BYTE*)src + ddsd2.lPitch);
			dest -= gD3D.wndWidth * 6;
		}
	}
	else if (ddsd2.ddpfPixelFormat.dwRGBBitCount == 24)
	{
		DWORD* src;
		BYTE* dest;

		src = (DWORD*)ddsd2.lpSurface;
		dest = (BYTE*)pixels + gD3D.wndWidth * (gD3D.wndHeight - 1) * 3;
		for (j = 0; j < gD3D.wndHeight; j++)
		{
			for (i = 0; i < gD3D.wndWidth; i++)
			{
				*dest++ = ((*src & ddsd2.ddpfPixelFormat.dwRBitMask) >> rRightShift) << rLeftShift;
				*dest++ = ((*src & ddsd2.ddpfPixelFormat.dwGBitMask) >> gRightShift) << gLeftShift;
				*dest++ = ((*src & ddsd2.ddpfPixelFormat.dwBBitMask) >> bRightShift) << bLeftShift;
				src = (DWORD*)((BYTE*)src + 3);
			}
			src = (DWORD*)((BYTE*)src + ddsd2.lPitch);
			dest -= gD3D.wndWidth * 6;
		}
	}
	else if (ddsd2.ddpfPixelFormat.dwRGBBitCount == 32)
	{
		DWORD* src;
		BYTE* dest;

		src = (DWORD*)ddsd2.lpSurface;
		dest = (BYTE*)pixels + gD3D.wndWidth * (gD3D.wndHeight - 1) * 3;
		for (j = 0; j < gD3D.wndHeight; j++)
		{
			for (i = 0; i < gD3D.wndWidth; i++)
			{
				*dest++ = ((*src & ddsd2.ddpfPixelFormat.dwRBitMask) >> rRightShift) << rLeftShift;
				*dest++ = ((*src & ddsd2.ddpfPixelFormat.dwGBitMask) >> gRightShift) << gLeftShift;
				*dest++ = ((*src & ddsd2.ddpfPixelFormat.dwBBitMask) >> bRightShift) << bLeftShift;
				src++;
			}
			src = (DWORD*)((BYTE*)src + ddsd2.lPitch);
			dest -= gD3D.wndWidth * 6;
		}
	}

	lpDDS->lpVtbl->Release(lpDDS);
	lpDDS->lpVtbl->Unlock(lpDDS, NULL);
}

DLL_EXPORT void APIENTRY glRectd( GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2 )
{
}

DLL_EXPORT void APIENTRY glRectdv( const GLdouble* v1, const GLdouble* v2 )
{
}

DLL_EXPORT void APIENTRY glRectf( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 )
{
}

DLL_EXPORT void APIENTRY glRectfv( const GLfloat* v1, const GLfloat* v2 )
{
}

DLL_EXPORT void APIENTRY glRecti( GLint x1, GLint y1, GLint x2, GLint y2 )
{
}

DLL_EXPORT void APIENTRY glRectiv( const GLint* v1, const GLint* v2 )
{
}

DLL_EXPORT void APIENTRY glRects( GLshort x1, GLshort y1, GLshort x2, GLshort y2 )
{
}

DLL_EXPORT void APIENTRY glRectsv( const GLshort* v1, const GLshort* v2 )
{
}

DLL_EXPORT GLint APIENTRY glRenderMode( GLenum mode )
{
	return 0;
}

DLL_EXPORT void APIENTRY glRotated( GLdouble angle, GLdouble x, GLdouble y, GLdouble z )
{
}

DLL_EXPORT void APIENTRY glRotatef( GLfloat angle, GLfloat x, GLfloat y, GLfloat z )
{
	D3DMATRIX	matrix;
	double		rad, s, c;

	if (gD3D.vertStart != gD3D.vertCount)
	{
		gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
		gD3D.vertStart = gD3D.vertCount;
	}

	if (!(gD3D.angleConstFlag & 1))
	{
		gD3D.angleConstFlag |= 1;
		gD3D.angleDegToRad = atan(1.0) * 4.0;
	}

	rad = angle * gD3D.angleDegToRad / 180.0;
	s = sin(rad);
	c = cos(rad);

	if (x == 1.0f)
	{
		// Create X rotation matrix
		/* 
			|  1  0  0  0  |
			|  0  c  s  0  |
			|  0 -s  c  0  |
			|  0  0  0  1  |
		*/
		matrix._11 = 1.0f;	matrix._12 = 0.0f;	matrix._13 = 0.0f;	matrix._14 = 0.0f;
		matrix._21 = 0.0f;	matrix._22 = c;		matrix._23 = s;		matrix._24 = 0.0f;
		matrix._31 = 0.0f;	matrix._32 = -s;	matrix._33 = c;		matrix._34 = 0.0f;
		matrix._41 = 0.0f;	matrix._42 = 0.0f;	matrix._43 = 0.0f;	matrix._44 = 1.0f;
	}
	else if (y == 1.0f)
	{
		// Create Y rotation matrix
		/*
			|  c  0 -s  0  |
			|  0  1  0  0  |
			|  s  0  c  0  |
			|  0  0  0  1  |
		*/
		matrix._11 = c;		matrix._12 = 0.0f;	matrix._13 = -s;	matrix._14 = 0.0f;
		matrix._21 = 0.0f;	matrix._22 = 1.0f;	matrix._23 = 0.0f;	matrix._24 = 0.0f;
		matrix._31 = s;		matrix._32 = 0.0f;	matrix._33 = c;		matrix._34 = 0.0f;
		matrix._41 = 0.0f;	matrix._42 = 0.0f;	matrix._43 = 0.0f;	matrix._44 = 1.0f;
	}
	else if (z == 1.0f)
	{
		// Create Z rotation matrix
		/*
			|  c  s  0  0  |
			| -s  c  0  0  |
			|  0  0  1  0  |
			|  0  0  0  1  |
		*/
		matrix._11 = c;		matrix._12 = s;		matrix._13 = 0.0f;	matrix._14 = 0.0f;
		matrix._21 = -s;	matrix._22 = c;		matrix._23 = 0.0f;	matrix._24 = 0.0f;
		matrix._31 = 0.0f;	matrix._32 = 0.0f;	matrix._33 = 1.0f;	matrix._34 = 0.0f;
		matrix._41 = 0.0f;	matrix._42 = 0.0f;	matrix._43 = 0.0f;	matrix._44 = 1.0f;
	}
	else
	{
		// Nothing to rotate around
		return;
	}

	// Apply rotation
	gD3D.lpD3DD3->lpVtbl->MultiplyTransform(gD3D.lpD3DD3, gD3D.transformState, &matrix);
}

DLL_EXPORT void APIENTRY glScaled( GLdouble x, GLdouble y, GLdouble z )
{
}

DLL_EXPORT void APIENTRY glScalef( GLfloat x, GLfloat y, GLfloat z )
{
	D3DMATRIX	matrix;

	if (gD3D.vertStart != gD3D.vertCount)
	{
		gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
		gD3D.vertStart = gD3D.vertCount;
	}

	// Create scale matrix
	/*
		| x 0 0 0 |
		| 0 y 0 0 |
		| 0 0 z 0 |
		| 0 0 0 1 |
	*/
	matrix._11 = x;		matrix._12 = 0.0f;	matrix._13 = 0.0f;	matrix._14 = 0.0f;
	matrix._21 = 0.0f;	matrix._22 = y;		matrix._23 = 0.0f;	matrix._24 = 0.0f;
	matrix._31 = 0.0f;	matrix._32 = 0.0f;	matrix._33 = z;		matrix._34 = 0.0f;
	matrix._41 = 0.0f;	matrix._42 = 0.0f;	matrix._43 = 0.0f;	matrix._44 = 1.0f;

	gD3D.lpD3DD3->lpVtbl->MultiplyTransform(gD3D.lpD3DD3, gD3D.transformState, &matrix);
}

DLL_EXPORT void APIENTRY glScissor( GLint x, GLint y, GLsizei width, GLsizei height )
{
}

DLL_EXPORT void APIENTRY glSelectBuffer( GLsizei size, GLuint* buffer )
{
}

DLL_EXPORT void APIENTRY glShadeModel( GLenum mode )
{
	DWORD	dummy;

	if (mode == GL_SMOOTH)
	{
		if (gD3D.rStates[D3DRENDERSTATE_SHADEMODE] != D3DSHADE_GOURAUD)
		{
			if (gD3D.indexCount)
			{
				if (gD3D.vertStart != gD3D.vertCount)
				{
					gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
					gD3D.vertStart = gD3D.vertCount;
				}

				gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
				gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
				gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dummy);

				gD3D.vertStart = 0;
				gD3D.vertCount = 0;
				gD3D.indexCount = 0;
			}

			gD3D.rStates[D3DRENDERSTATE_SHADEMODE] = D3DSHADE_GOURAUD;
			gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
		}
	}
	else
	{
		if (gD3D.rStates[D3DRENDERSTATE_SHADEMODE] != D3DSHADE_FLAT)
		{
			if (gD3D.indexCount)
			{
				if (gD3D.vertStart != gD3D.vertCount)
				{
					gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
					gD3D.vertStart = gD3D.vertCount;
				}

				gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
				gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
				gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dummy);

				gD3D.vertStart = 0;
				gD3D.vertCount = 0;
				gD3D.indexCount = 0;
			}

			gD3D.rStates[D3DRENDERSTATE_SHADEMODE] = D3DSHADE_FLAT;
			gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_SHADEMODE, D3DSHADE_FLAT);
		}
	}
}

DLL_EXPORT void APIENTRY glStencilFunc( GLenum func, GLint ref, GLuint mask )
{
}

DLL_EXPORT void APIENTRY glStencilMask( GLuint mask )
{
}

DLL_EXPORT void APIENTRY glStencilOp( GLenum fail, GLenum zfail, GLenum zpass )
{
}

DLL_EXPORT void APIENTRY glTexCoord1d( GLdouble s )
{
}

DLL_EXPORT void APIENTRY glTexCoord1dv( const GLdouble* v )
{
}

DLL_EXPORT void APIENTRY glTexCoord1f( GLfloat s )
{
}

DLL_EXPORT void APIENTRY glTexCoord1fv( const GLfloat* v )
{
}

DLL_EXPORT void APIENTRY glTexCoord1i( GLint s )
{
}

DLL_EXPORT void APIENTRY glTexCoord1iv( const GLint* v )
{
}

DLL_EXPORT void APIENTRY glTexCoord1s( GLshort s )
{
}

DLL_EXPORT void APIENTRY glTexCoord1sv( const GLshort* v )
{
}

DLL_EXPORT void APIENTRY glTexCoord2d( GLdouble s, GLdouble t )
{
}

DLL_EXPORT void APIENTRY glTexCoord2dv( const GLdouble* v )
{
}

DLL_EXPORT void APIENTRY glTexCoord2f( GLfloat s, GLfloat t )
{
	gD3D.tu = s;
	gD3D.tv = t;
}

DLL_EXPORT void APIENTRY glTexCoord2fv( const GLfloat* v )
{
}

DLL_EXPORT void APIENTRY glTexCoord2i( GLint s, GLint t )
{
}

DLL_EXPORT void APIENTRY glTexCoord2iv( const GLint* v )
{
}

DLL_EXPORT void APIENTRY glTexCoord2s( GLshort s, GLshort t )
{
}

DLL_EXPORT void APIENTRY glTexCoord2sv( const GLshort* v )
{
}

DLL_EXPORT void APIENTRY glTexCoord3d( GLdouble s, GLdouble t, GLdouble r )
{
}

DLL_EXPORT void APIENTRY glTexCoord3dv( const GLdouble* v )
{
}

DLL_EXPORT void APIENTRY glTexCoord3f( GLfloat s, GLfloat t, GLfloat r )
{
}

DLL_EXPORT void APIENTRY glTexCoord3fv( const GLfloat* v )
{
}

DLL_EXPORT void APIENTRY glTexCoord3i( GLint s, GLint t, GLint r )
{
}

DLL_EXPORT void APIENTRY glTexCoord3iv( const GLint* v )
{
}

DLL_EXPORT void APIENTRY glTexCoord3s( GLshort s, GLshort t, GLshort r )
{
}

DLL_EXPORT void APIENTRY glTexCoord3sv( const GLshort* v )
{
}

DLL_EXPORT void APIENTRY glTexCoord4d( GLdouble s, GLdouble t, GLdouble r, GLdouble q )
{
}

DLL_EXPORT void APIENTRY glTexCoord4dv( const GLdouble* v )
{
}

DLL_EXPORT void APIENTRY glTexCoord4f( GLfloat s, GLfloat t, GLfloat r, GLfloat q )
{
}

DLL_EXPORT void APIENTRY glTexCoord4fv( const GLfloat* v )
{
}

DLL_EXPORT void APIENTRY glTexCoord4i( GLint s, GLint t, GLint r, GLint q )
{
}

DLL_EXPORT void APIENTRY glTexCoord4iv( const GLint* v )
{
}

DLL_EXPORT void APIENTRY glTexCoord4s( GLshort s, GLshort t, GLshort r, GLshort q )
{
}

DLL_EXPORT void APIENTRY glTexCoord4sv( const GLshort* v )
{
}

DLL_EXPORT void APIENTRY glTexCoordPointer( GLint size, GLenum type, GLsizei stride, const GLvoid* pointer )
{
}

DLL_EXPORT void APIENTRY glTexEnvf( GLenum target, GLenum pname, GLfloat param )
{
	switch (pname)
	{
	case GL_TEXTURE_ENV_MODE:
		gD3D.texEnvMode[gD3D.textureStage] = (int)param;
		gD3D.textureValid = FALSE;
		break;
	default:
		OutputDebugString("Wrapper: GL_TEXTURE_ENV_COLOR not implemented\n");
		break;
	}
}

DLL_EXPORT void APIENTRY glTexEnvfv( GLenum target, GLenum pname, const GLfloat* params )
{
}

DLL_EXPORT void APIENTRY glTexEnvi( GLenum target, GLenum pname, GLint param )
{
}

DLL_EXPORT void APIENTRY glTexEnviv( GLenum target, GLenum pname, const GLint* params )
{
}

DLL_EXPORT void APIENTRY glTexGend( GLenum coord, GLenum pname, GLdouble param )
{
}

DLL_EXPORT void APIENTRY glTexGendv( GLenum coord, GLenum pname, const GLdouble* params )
{
}

DLL_EXPORT void APIENTRY glTexGenf( GLenum coord, GLenum pname, GLfloat param )
{
}

DLL_EXPORT void APIENTRY glTexGenfv( GLenum coord, GLenum pname, const GLfloat* params )
{
}

DLL_EXPORT void APIENTRY glTexGeni( GLenum coord, GLenum pname, GLint param )
{
}

DLL_EXPORT void APIENTRY glTexGeniv( GLenum coord, GLenum pname, const GLint* params )
{
}

DLL_EXPORT void APIENTRY glTexImage1D( GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid* pixels )
{
}

DLL_EXPORT void APIENTRY glTexImage2D( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels )
{
	int		i;
	int		newWidth, newHeight;
	DDSURFACEDESC2 ddsd;
	LPDIRECTDRAWSURFACE4 lpDDS4, lpDDS42;
	D3D_TEXTURE* tex;

	tex = &gD3D.textures[gD3D.currentTexture[gD3D.textureStage]];

	if (gD3D.useSubsample)
	{
		if (width > 256 || height > 256)
		{
			if (width > height)
			{
				newWidth = 256;
				newHeight = (height << 8) / width;
			}
			else
			{
				newWidth = (width << 8) / height;
				newHeight = 256;
			}
		}
		else
		{
			newWidth = width;
			newHeight = height;
		}
	}
	else
	{
		newWidth = width;
		newHeight = height;
	}

	if (gD3D.squareTexturesOnly)
	{
		if (newHeight > newWidth)
			newWidth = newHeight;
		else
			newHeight = newWidth;
	}

	if (level == 0)
	{
		memset(&ddsd, 0, sizeof(ddsd));

		switch (internalformat)
		{
			case 1:
				ddsd.ddpfPixelFormat = gD3D.ddpf8888;
				break;
			case 3:
				ddsd.ddpfPixelFormat = gD3D.ddpf555_565;
				break;
			case 4:
				if (gD3D.bLoad4444)
				{
					ddsd.ddpfPixelFormat = gD3D.ddpf4444;
					gD3D.bLoad4444 = FALSE;
				}
				else
				{
					ddsd.ddpfPixelFormat = gD3D.ddpf5551;
				}
				break;
			default:
				OutputDebugString("Wrapper: texture format not supported\n");
				break;
		}

		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_TEXTURESTAGE;
		ddsd.dwHeight = newHeight;
		ddsd.dwWidth = newWidth;
		ddsd.dwTextureStage = gD3D.textureStage;
		ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
		ddsd.ddsCaps.dwCaps2 = DDSCAPS2_HINTDYNAMIC | DDSCAPS2_TEXTUREMANAGE;

		if (gD3D.lpDD4->lpVtbl->CreateSurface(gD3D.lpDD4, &ddsd, &lpDDS4, NULL) != DD_OK)
		{
			OutputDebugString("Wrapper: CreateSurface for texture failed\n");
			return;
		}

		LoadSurface(lpDDS4, internalformat, width, height, newWidth, newHeight, (BYTE*)pixels);

		if (tex->lpD3DT2)
			tex->lpD3DT2->lpVtbl->Release(tex->lpD3DT2);
		if (tex->lpDDS4)
			tex->lpDDS4->lpVtbl->Release(tex->lpDDS4);

		if (lpDDS4->lpVtbl->QueryInterface(lpDDS4, IID_IDirect3DTexture2, (LPVOID*)&tex->lpD3DT2) != DD_OK)
		{
			OutputDebugString("Wrapper: QueryInterface for Texture2 failed.\n");
			lpDDS4->lpVtbl->Release(lpDDS4);
			return;
		}

		tex->internalFormat = internalformat;
		tex->width = newWidth;
		tex->height = newHeight;
		tex->oldWidth = width;
		tex->oldHeight = height;
		tex->lpDDS4 = lpDDS4;
	}
	else if (level == 1 && gD3D.useMipmap)
	{
		memset(&ddsd, 0, sizeof(ddsd));

		switch (internalformat)
		{
			case 1:
				ddsd.ddpfPixelFormat = gD3D.ddpf8888;
				break;
			case 3:
				ddsd.ddpfPixelFormat = gD3D.ddpf555_565;
				break;
			case 4:
				if (gD3D.bLoad4444)
				{
					ddsd.ddpfPixelFormat = gD3D.ddpf4444;
					gD3D.bLoad4444 = FALSE;
				}
				else
				{
					ddsd.ddpfPixelFormat = gD3D.ddpf5551;
				}
				break;
		}

		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_TEXTURESTAGE;
		ddsd.dwHeight = tex->height;
		ddsd.dwWidth = tex->width;
		ddsd.dwTextureStage = gD3D.textureStage;
		ddsd.ddsCaps.dwCaps = DDSCAPS_COMPLEX | DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;
		ddsd.ddsCaps.dwCaps2 = DDSCAPS2_HINTDYNAMIC | DDSCAPS2_TEXTUREMANAGE;

		if (gD3D.lpDD4->lpVtbl->CreateSurface(gD3D.lpDD4, &ddsd, &lpDDS4, NULL) != DD_OK)
		{
			OutputDebugString("Wrapper: CreateSurface for texture failed\n");
			return;
		}

		lpDDS4->lpVtbl->Blt(lpDDS4, NULL, tex->lpDDS4, NULL, DDBLT_WAIT, NULL);

		tex->lpD3DT2->lpVtbl->Release(tex->lpD3DT2);
		tex->lpDDS4->lpVtbl->Release(tex->lpDDS4);

		tex->lpDDS4 = lpDDS4;
		lpDDS4->lpVtbl->QueryInterface(lpDDS4, IID_IDirect3DTexture2, (LPVOID*)&tex->lpD3DT2);

		memset(&ddsd.ddsCaps, 0, sizeof(ddsd.ddsCaps));
		ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;

		tex->lpDDS4->lpVtbl->GetAttachedSurface(tex->lpDDS4, &ddsd.ddsCaps, &lpDDS42);

		LoadSurface(lpDDS42, internalformat, width, height, newWidth, newHeight, (BYTE*)pixels);

		lpDDS42->lpVtbl->Release(lpDDS42);
	}
	else if (gD3D.useMipmap)
	{
		tex->lpDDS4->lpVtbl->AddRef(tex->lpDDS4);

		lpDDS4 = tex->lpDDS4;
		for (i = 0; i < level; i++)
		{
			HRESULT hResult;

			memset(&ddsd.ddsCaps, 0, sizeof(ddsd.ddsCaps));
			ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;

			hResult = lpDDS4->lpVtbl->GetAttachedSurface(lpDDS4, &ddsd.ddsCaps, &lpDDS42);
			lpDDS4->lpVtbl->Release(lpDDS4);
			if (hResult == DDERR_NOTFOUND)
				return;

			lpDDS4 = lpDDS42;
		}

		LoadSurface(lpDDS4, internalformat, width, height, newWidth, newHeight, (BYTE*)pixels);
		lpDDS4->lpVtbl->Release(lpDDS4);
	}

	gD3D.textureValid = FALSE;
}

DLL_EXPORT void APIENTRY glTexParameterf( GLenum target, GLenum pname, GLfloat param )
{
	switch (pname)
	{
	case GL_TEXTURE_MAG_FILTER:
		if (param == GL_NEAREST)
		{
			gD3D.textures[gD3D.currentTexture[gD3D.textureStage]].magFilter = D3DTFG_POINT;
		}
		else
		{
			gD3D.textures[gD3D.currentTexture[gD3D.textureStage]].magFilter = D3DTFG_LINEAR;
		}
		break;
	case GL_TEXTURE_MIN_FILTER:
		switch ((DWORD)param)
		{
		case GL_NEAREST:
			gD3D.textures[gD3D.currentTexture[gD3D.textureStage]].minFilter = D3DTFN_POINT;
			gD3D.textures[gD3D.currentTexture[gD3D.textureStage]].mipFilter = D3DTFP_NONE;
			break;
		case GL_LINEAR:
			gD3D.textures[gD3D.currentTexture[gD3D.textureStage]].minFilter = D3DTFN_LINEAR;
			gD3D.textures[gD3D.currentTexture[gD3D.textureStage]].mipFilter = D3DTFP_NONE;
			break;
		case GL_NEAREST_MIPMAP_NEAREST:
			gD3D.textures[gD3D.currentTexture[gD3D.textureStage]].minFilter = D3DTFN_POINT;
			gD3D.textures[gD3D.currentTexture[gD3D.textureStage]].mipFilter = D3DTFP_POINT;
			break;
		case GL_LINEAR_MIPMAP_NEAREST:
			gD3D.textures[gD3D.currentTexture[gD3D.textureStage]].minFilter = D3DTFN_LINEAR;
			gD3D.textures[gD3D.currentTexture[gD3D.textureStage]].mipFilter = D3DTFP_POINT;
			break;
		case GL_NEAREST_MIPMAP_LINEAR:
			gD3D.textures[gD3D.currentTexture[gD3D.textureStage]].minFilter = D3DTFN_POINT;
			gD3D.textures[gD3D.currentTexture[gD3D.textureStage]].mipFilter = D3DTFP_LINEAR;
			break;
		case GL_LINEAR_MIPMAP_LINEAR:
			gD3D.textures[gD3D.currentTexture[gD3D.textureStage]].minFilter = D3DTFN_LINEAR;
			gD3D.textures[gD3D.currentTexture[gD3D.textureStage]].mipFilter = D3DTFP_LINEAR;
			break;
		}
		break;
	case GL_TEXTURE_WRAP_S:
		if (param == GL_CLAMP)
		{
			gD3D.textures[gD3D.currentTexture[gD3D.textureStage]].addressU = D3DTADDRESS_CLAMP;
		}
		else
		{
			gD3D.textures[gD3D.currentTexture[gD3D.textureStage]].addressU = D3DTADDRESS_WRAP;
		}
		break;
	case GL_TEXTURE_WRAP_T:
		if (param == GL_CLAMP)
		{
			gD3D.textures[gD3D.currentTexture[gD3D.textureStage]].addressV = D3DTADDRESS_CLAMP;
		}
		else
		{
			gD3D.textures[gD3D.currentTexture[gD3D.textureStage]].addressV = D3DTADDRESS_WRAP;
		}
		break;
	}

	gD3D.textureValid = FALSE;
}

DLL_EXPORT void APIENTRY glTexParameterfv( GLenum target, GLenum pname, const GLfloat* params )
{
}

DLL_EXPORT void APIENTRY glTexParameteri( GLenum target, GLenum pname, GLint param )
{
}

DLL_EXPORT void APIENTRY glTexParameteriv( GLenum target, GLenum pname, const GLint* params )
{
}

DLL_EXPORT void APIENTRY glTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid* pixels )
{
}

DLL_EXPORT void APIENTRY glTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels )
{
	int		i;
	int		xLeft, yTop, xRight, yBottom;
	DDSCAPS2 ddsc2;
	RECT	rc;
	D3D_TEXTURE* tex;
	LPDIRECTDRAWSURFACE4 lpDDS4, lpDDS42;

	tex = &gD3D.textures[gD3D.currentTexture[gD3D.textureStage]];

	lpDDS4 = tex->lpDDS4;
	for (i = 0; i < level; i++)
	{
		HRESULT hResult;

		memset(&ddsc2, 0, sizeof(ddsc2));
		ddsc2.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;

		hResult = lpDDS4->lpVtbl->GetAttachedSurface(lpDDS4, &ddsc2, &lpDDS42);
		if (hResult == DDERR_NOTFOUND)
			return;

		lpDDS4 = lpDDS42;
		lpDDS4->lpVtbl->Release(lpDDS4);
	}

	xLeft = tex->width * xoffset / tex->oldWidth;
	yTop = tex->height * yoffset / tex->oldHeight;
	xRight = xLeft + tex->width * width / tex->oldWidth;
	yBottom = yTop + tex->height * height / tex->oldHeight;
	SetRect(&rc, xLeft, yTop, xRight, yBottom);

	if (LoadSubSurface(lpDDS4, tex->internalFormat, width, height, (BYTE*)pixels, &rc) != DD_OK)
	{
		OutputDebugString("Wrapper: LoadSubSurface Failure.\n");
		return;
	}
}

DLL_EXPORT void APIENTRY glTranslated( GLdouble x, GLdouble y, GLdouble z )
{
}

DLL_EXPORT void APIENTRY glTranslatef( GLfloat x, GLfloat y, GLfloat z )
{
	D3DMATRIX	matrix;

	if (gD3D.vertStart != gD3D.vertCount)
	{
		gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
		gD3D.vertStart = gD3D.vertCount;
	}

	// Create translation matrix
	/*
		| 1 0 0 0 |
		| 0 1 0 0 |
		| 0 0 1 0 |
		| x y z 1 |
	*/
	matrix._11 = 1.0f;	matrix._12 = 0.0f;	matrix._13 = 0.0f;	matrix._14 = 0.0f;
	matrix._21 = 0.0f;	matrix._22 = 1.0f;	matrix._23 = 0.0f;	matrix._24 = 0.0f;
	matrix._31 = 0.0f;	matrix._32 = 0.0f;	matrix._33 = 1.0f;	matrix._34 = 0.0f;
	matrix._41 = x;		matrix._42 = y;		matrix._43 = z;		matrix._44 = 1.0f;

	gD3D.lpD3DD3->lpVtbl->MultiplyTransform(gD3D.lpD3DD3, gD3D.transformState, &matrix);
}

DLL_EXPORT void APIENTRY glVertex2d( GLdouble x, GLdouble y )
{
}

DLL_EXPORT void APIENTRY glVertex2dv( const GLdouble* v )
{
}

DLL_EXPORT void APIENTRY glVertex2f( GLfloat x, GLfloat y )
{
	D3D_VERTEX* vert;

	vert = &gD3D.verts[gD3D.vertCount + gD3D.primVertCount];
	vert->x = x;
	vert->y = y;
	vert->z = 0.0f;
	vert->color = gD3D.color;
	vert->tu = gD3D.tu;
	vert->tv = gD3D.tv;

	if (gD3D.useSubStage)
	{
		vert->tu2 = gD3D.tu2;
		vert->tv2 = gD3D.tv2;
	}

	gD3D.primVertCount++;
}

DLL_EXPORT void APIENTRY glVertex2fv( const GLfloat* v )
{
}

DLL_EXPORT void APIENTRY glVertex2i( GLint x, GLint y )
{
}

DLL_EXPORT void APIENTRY glVertex2iv( const GLint* v )
{
}

DLL_EXPORT void APIENTRY glVertex2s( GLshort x, GLshort y )
{
}

DLL_EXPORT void APIENTRY glVertex2sv( const GLshort* v )
{
}

DLL_EXPORT void APIENTRY glVertex3d( GLdouble x, GLdouble y, GLdouble z )
{
}

DLL_EXPORT void APIENTRY glVertex3dv( const GLdouble* v )
{
}

DLL_EXPORT void APIENTRY glVertex3f( GLfloat x, GLfloat y, GLfloat z )
{
	D3D_VERTEX* vert;

	vert = &gD3D.verts[gD3D.vertCount + gD3D.primVertCount];
	vert->x = x;
	vert->y = y;
	vert->z = z;
	vert->color = gD3D.color;
	vert->tu = gD3D.tu;
	vert->tv = gD3D.tv;

	if (gD3D.useSubStage)
	{
		vert->tu2 = gD3D.tu2;
		vert->tv2 = gD3D.tv2;
	}

	gD3D.primVertCount++;
}

DLL_EXPORT void APIENTRY glVertex3fv( const GLfloat* v )
{
	D3D_VERTEX* vert;

	vert = &gD3D.verts[gD3D.vertCount + gD3D.primVertCount];
	vert->x = v[0];
	vert->y = v[1];
	vert->z = v[2];
	vert->color = gD3D.color;
	vert->tu = gD3D.tu;
	vert->tv = gD3D.tv;

	if (gD3D.useSubStage)
	{
		vert->tu2 = gD3D.tu2;
		vert->tv2 = gD3D.tv2;
	}

	gD3D.primVertCount++;
}

DLL_EXPORT void APIENTRY glVertex3i( GLint x, GLint y, GLint z )
{
}

DLL_EXPORT void APIENTRY glVertex3iv( const GLint* v )
{
}

DLL_EXPORT void APIENTRY glVertex3s( GLshort x, GLshort y, GLshort z )
{
}

DLL_EXPORT void APIENTRY glVertex3sv( const GLshort* v )
{
}

DLL_EXPORT void APIENTRY glVertex4d( GLdouble x, GLdouble y, GLdouble z, GLdouble w )
{
}

DLL_EXPORT void APIENTRY glVertex4dv( const GLdouble* v )
{
}

DLL_EXPORT void APIENTRY glVertex4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
}

DLL_EXPORT void APIENTRY glVertex4fv( const GLfloat* v )
{
}

DLL_EXPORT void APIENTRY glVertex4i( GLint x, GLint y, GLint z, GLint w )
{
}

DLL_EXPORT void APIENTRY glVertex4iv( const GLint* v )
{
}

DLL_EXPORT void APIENTRY glVertex4s( GLshort x, GLshort y, GLshort z, GLshort w )
{
}

DLL_EXPORT void APIENTRY glVertex4sv( const GLshort* v )
{
}

DLL_EXPORT void APIENTRY glVertexPointer( GLint size, GLenum type, GLsizei stride, const GLvoid* pointer )
{
	gD3D.vertexPointer = pointer;

	if (size == 3 || type == GL_FLOAT || stride == 16)
	{
	}
	else
	{
		OutputDebugString("Wrapper: unsupported vertex array\n");
	}
}

DLL_EXPORT void APIENTRY glViewport( GLint x, GLint y, GLsizei width, GLsizei height )
{
	D3DVIEWPORT2	vport;
	DWORD	dummy;

	if (gD3D.indexCount)
	{
		if (gD3D.vertStart != gD3D.vertCount)
		{
			gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
			gD3D.vertStart = gD3D.vertCount;
		}

		gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
		gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
		gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dummy);

		gD3D.vertStart = 0;
		gD3D.vertCount = 0;
		gD3D.indexCount = 0;
	}

	// Set viewport
	vport.dwSize = sizeof(vport);
	gD3D.lpD3DVP3->lpVtbl->GetViewport2(gD3D.lpD3DVP3, &vport);
	vport.dwX = x;
	vport.dwY = gD3D.wndHeight - height - y;
	vport.dwWidth = width;
	vport.dwHeight = height;
	vport.dvClipX = -1.0;
	vport.dvClipY = 1.0;
	vport.dvClipWidth = 2.0;
	vport.dvClipHeight = 2.0;

	gD3D.lpD3DVP3->lpVtbl->SetViewport2(gD3D.lpD3DVP3, &vport);
}

DLL_EXPORT void APIENTRY glSelectTextureSGIS( GLenum target )
{
	if (target == TEXTURE0_SGIS)
	{
		gD3D.textureStage = 0;
	}
	else
	{
		gD3D.textureStage = 1;
	}
}

DLL_EXPORT void APIENTRY glMTexCoord2fSGIS( GLenum target, GLfloat s, GLfloat t )
{
	if (target == TEXTURE0_SGIS)
	{
		gD3D.tu = s;
		gD3D.tv = t;
	}
	else
	{
		gD3D.tu2 = s;
		gD3D.tv2 = t;
	}
}

DLL_EXPORT BOOL WINAPI wglCopyContext( HGLRC hglrcSrc, HGLRC hglrcDst, UINT mask )
{
	return FALSE;
}

DLL_EXPORT HGLRC WINAPI wglCreateContext( HDC hdc )
{
	LPDIRECTDRAW lpDD;
	DDSURFACEDESC ddsd;
	RECT rect;
	int i;

	gD3D.hDC = hdc;
	gD3D.hWnd = WindowFromDC(hdc);

	GetClientRect(gD3D.hWnd, &rect);
	gD3D.wndWidth = (USHORT)rect.right;
	gD3D.wndHeight = (USHORT)rect.bottom;

	// Initialize COM
	if (FAILED(CoInitialize(NULL)))
		return NULL;

	// Create DirectDraw object
	if (FAILED(DirectDrawCreate(NULL, &lpDD, NULL)))
	{
		CoUninitialize();
		return NULL;
	}

	// Get display mode
	ddsd.dwSize = sizeof(ddsd);
	if (FAILED(lpDD->lpVtbl->GetDisplayMode(lpDD, &ddsd)))
	{
		lpDD->lpVtbl->Release(lpDD);
		CoUninitialize();
		return NULL;
	}

	lpDD->lpVtbl->Release(lpDD);

	// Create DX Manager
	if (FAILED(DXMgrCreate(IID_IDXMgr, (LPVOID*)&gD3D.pDXMgr)))
	{
		CoUninitialize();
		return NULL;
	}

	// Initialize DX Manager
	if (FAILED(gD3D.pDXMgr->lpVtbl->Initialize2(gD3D.pDXMgr, gD3D.lpDD4, gD3D.hWnd, 0)))
	{
		gD3D.pDXMgr->lpVtbl->Release(gD3D.pDXMgr);
		CoUninitialize();
		return NULL;
	}

	// Set DXMgr enable structure
	DXMGRENABLE enable;
	memset(&enable, 0, sizeof(enable));
	enable.uFlags = DXMGRENABLE_3D | DXMGRENABLE_ZBUFFER;

	LONG lResult;	// Registry function result code
	HKEY hKey;		// Handle of opened/created key
	DWORD dwType;	// Type of key
	DWORD dwSize;	// Size of element data
	DWORD dwData;	// DWORD data

	lResult = RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Quake", &hKey);
	if (lResult == ERROR_SUCCESS)
	{
		// We opened the existing key.
		dwSize = 4;
		lResult = RegQueryValueEx(hKey, "Emulation", NULL, &dwType, (LPBYTE)&dwData, &dwSize);

		// Success?
		if (lResult == ERROR_SUCCESS &&
			dwType == REG_DWORD &&	// Only copy DWORD values
			dwData
			)
		{
			// Use RGB device
			enable.iidDeviceType = IID_IDirect3DRGBDevice;
			enable.uFlags |= DXMGRENABLE_DEVICETYPE;
		}

		RegCloseKey(hKey);
	}

	if (gD3D.isFullscreen)
	{
		enable.uFlags |= DXMGRENABLE_FULLSCREEN;

		// Create display mode enumerator
		IEnumDisplayModes* pEnumDisplayModes;
		if (FAILED(gD3D.pDXMgr->lpVtbl->CreateEnumDisplayModes(gD3D.pDXMgr, &pEnumDisplayModes, 0)))
		{
			gD3D.pDXMgr->lpVtbl->Release(gD3D.pDXMgr);
			CoUninitialize();
			return NULL;
		}

		// Find matching display mode
		while (1)
		{
			// Get next display mode
			if (FAILED(pEnumDisplayModes->lpVtbl->Next(pEnumDisplayModes, 1, &enable.DisplayMode, (UINT*)&dwSize)))
			{
				pEnumDisplayModes->lpVtbl->Release(pEnumDisplayModes);
				gD3D.pDXMgr->lpVtbl->Release(gD3D.pDXMgr);
				CoUninitialize();
				return NULL;
			}

			// No more modes
			if (dwSize == 0)
				break;

			// Check for match
			if (enable.DisplayMode.dwWidth == ddsd.dwWidth &&
				enable.DisplayMode.dwHeight == ddsd.dwHeight &&
				enable.DisplayMode.dwBPP == ddsd.ddpfPixelFormat.dwRGBBitCount)
			{
				enable.uFlags |= DXMGRENABLE_DISPLAYMODE;
				break;
			}
		}

		pEnumDisplayModes->lpVtbl->Release(pEnumDisplayModes);
		enable.hwndDevice = NULL;
	}
	else
	{
		enable.hwndDevice = gD3D.hWnd;
	}

	enable.fNear = 1.0f;
	enable.fFar = 10.0f;

	// Enable DXMgr
	if (FAILED(gD3D.pDXMgr->lpVtbl->Enable(gD3D.pDXMgr, &enable)))
	{
		gD3D.pDXMgr->lpVtbl->Release(gD3D.pDXMgr);
		CoUninitialize();
		return NULL;
	}

	// Get D3D device
	if (FAILED(gD3D.pDXMgr->lpVtbl->GetDirect3DDevice(gD3D.pDXMgr, &gD3D.lpD3DD3)))
	{
		gD3D.pDXMgr->lpVtbl->Release(gD3D.pDXMgr);
		CoUninitialize();
		return NULL;
	}

	// Get device capabilities
	D3DDEVICEDESC heldevdesc, hwdevdesc;
	heldevdesc.dwSize = sizeof(heldevdesc);
	hwdevdesc.dwSize = sizeof(hwdevdesc);
	gD3D.lpD3DD3->lpVtbl->GetCaps(gD3D.lpD3DD3, &hwdevdesc, &heldevdesc);
	
	if (hwdevdesc.dwFlags)
	{
		gD3D.devdesc = hwdevdesc;
	}
	else
	{
		gD3D.devdesc = heldevdesc;
	}
	
	lResult = RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Quake", &hKey);
	if (lResult == ERROR_SUCCESS)
	{
		// We opened the existing key.
		dwSize = 4;
		lResult = RegQueryValueEx(hKey, "DisableMipMap", NULL, &dwType, (LPBYTE)&dwData, &dwSize);

		// Success?
		if (lResult == ERROR_SUCCESS &&
			dwType == REG_DWORD &&	// Only copy DWORD values
			dwData
			)
		{
			gD3D.useMipmap = FALSE;
			OutputDebugString("Wrapper: Mipmapping disabled\n");
		}
		else
		{
			gD3D.useMipmap = TRUE;
		}

		RegCloseKey(hKey);
	}
	else
	{
		gD3D.useMipmap = TRUE;
	}

	// Create texture format enumerator
	IEnumPixelFormats* pEnumPixelFormats;
	if (FAILED(gD3D.pDXMgr->lpVtbl->CreateEnumTextureFormats(gD3D.pDXMgr, &pEnumPixelFormats, 0)))
	{
		gD3D.lpD3DD3->lpVtbl->Release(gD3D.lpD3DD3);
		gD3D.pDXMgr->lpVtbl->Release(gD3D.pDXMgr);
		CoUninitialize();
		return NULL;
	}

	// Find 16-bit 5551 pixel format
	BOOL f5551 = FALSE;
	while (1)
	{
		// Get next pixel format
		if (FAILED(pEnumPixelFormats->lpVtbl->Next(pEnumPixelFormats, 1, &gD3D.ddpf5551, (UINT*)&dwSize)))
		{
			pEnumPixelFormats->lpVtbl->Release(pEnumPixelFormats);
			gD3D.lpD3DD3->lpVtbl->Release(gD3D.lpD3DD3);
			gD3D.pDXMgr->lpVtbl->Release(gD3D.pDXMgr);
			CoUninitialize();
			return NULL;
		}

		// No more formats
		if (dwSize == 0)
			break;

		// Check for 5551 format
		if ((gD3D.ddpf5551.dwFlags & DDPF_RGB) &&
			(gD3D.ddpf5551.dwRGBBitCount == 16) &&
			(gD3D.ddpf5551.dwFlags & DDPF_ALPHAPIXELS) &&
			(gD3D.ddpf5551.dwRGBAlphaBitMask == 0x8000) &&
			(gD3D.ddpf5551.dwRBitMask == 0x7C00) &&
			(gD3D.ddpf5551.dwGBitMask == 0x03E0) &&
			(gD3D.ddpf5551.dwBBitMask == 0x001F))
		{
			f5551 = TRUE;
			break;
		}
	}

	if (!f5551)
	{
		OutputDebugString("Wrapper: Unable to find 5551 texture.\n");
		pEnumPixelFormats->lpVtbl->Release(pEnumPixelFormats);
		gD3D.lpD3DD3->lpVtbl->Release(gD3D.lpD3DD3);
		gD3D.pDXMgr->lpVtbl->Release(gD3D.pDXMgr);
		CoUninitialize();
		return NULL;
	}

	// Reset enumerator
	if (FAILED(pEnumPixelFormats->lpVtbl->Reset(pEnumPixelFormats)))
	{
		pEnumPixelFormats->lpVtbl->Release(pEnumPixelFormats);
		gD3D.lpD3DD3->lpVtbl->Release(gD3D.lpD3DD3);
		gD3D.pDXMgr->lpVtbl->Release(gD3D.pDXMgr);
		CoUninitialize();
		return NULL;
	}

	// Find 16-bit 4444 pixel format
	BOOL f4444 = FALSE;
	while (1)
	{
		// Get next pixel format
		if (FAILED(pEnumPixelFormats->lpVtbl->Next(pEnumPixelFormats, 1, &gD3D.ddpf4444, (UINT*)&dwSize)))
		{
			pEnumPixelFormats->lpVtbl->Release(pEnumPixelFormats);
			gD3D.lpD3DD3->lpVtbl->Release(gD3D.lpD3DD3);
			gD3D.pDXMgr->lpVtbl->Release(gD3D.pDXMgr);
			CoUninitialize();
			return NULL;
		}

		// No more formats
		if (dwSize == 0)
			break;

		// Check for 4444 format
		if ((gD3D.ddpf4444.dwFlags & DDPF_RGB) &&
			(gD3D.ddpf4444.dwRGBBitCount == 16) &&
			(gD3D.ddpf4444.dwFlags & DDPF_ALPHAPIXELS) &&
			(gD3D.ddpf4444.dwRGBAlphaBitMask == 0xF000) &&
			(gD3D.ddpf4444.dwRBitMask == 0x0F00) &&
			(gD3D.ddpf4444.dwGBitMask == 0x00F0) &&
			(gD3D.ddpf4444.dwBBitMask == 0x000F))
		{
			f4444 = TRUE;
			break;
		}
	}

	if (!f4444)
	{
		OutputDebugString("Wrapper: Not using 4444 texture.\n");
		gD3D.ddpf4444 = gD3D.ddpf5551;
	}

	// Reset enumerator
	if (FAILED(pEnumPixelFormats->lpVtbl->Reset(pEnumPixelFormats)))
	{
		pEnumPixelFormats->lpVtbl->Release(pEnumPixelFormats);
		gD3D.lpD3DD3->lpVtbl->Release(gD3D.lpD3DD3);
		gD3D.pDXMgr->lpVtbl->Release(gD3D.pDXMgr);
		CoUninitialize();
		return NULL;
	}

	// Find 16-bit 555/565 pixel format
	BOOL f555_565 = FALSE;
	while (1)
	{
		// Get next pixel format
		if (FAILED(pEnumPixelFormats->lpVtbl->Next(pEnumPixelFormats, 1, &gD3D.ddpf555_565, (UINT*)&dwSize)))
		{
			pEnumPixelFormats->lpVtbl->Release(pEnumPixelFormats);
			gD3D.lpD3DD3->lpVtbl->Release(gD3D.lpD3DD3);
			gD3D.pDXMgr->lpVtbl->Release(gD3D.pDXMgr);
			CoUninitialize();
			return NULL;
		}

		// No more formats
		if (dwSize == 0)
			break;

		// Check for 555/565 format
		if ((gD3D.ddpf555_565.dwFlags & DDPF_RGB) &&
			(gD3D.ddpf555_565.dwRGBBitCount == 16) &&
			(gD3D.ddpf555_565.dwFlags & DDPF_ALPHAPIXELS) == 0 &&
			(gD3D.ddpf555_565.dwRBitMask == 0xF800 || gD3D.ddpf555_565.dwRBitMask == 0x7C00) &&
			(gD3D.ddpf555_565.dwGBitMask == 0x07E0 || gD3D.ddpf555_565.dwGBitMask == 0x03E0) &&
			(gD3D.ddpf555_565.dwBBitMask == 0x001F))
		{
			f555_565 = TRUE;
			break;
		}
	}

	if (!f555_565)
	{
		OutputDebugString("Wrapper: Unable to find 555 or 565 texture.\n");
		pEnumPixelFormats->lpVtbl->Release(pEnumPixelFormats);
		gD3D.lpD3DD3->lpVtbl->Release(gD3D.lpD3DD3);
		gD3D.pDXMgr->lpVtbl->Release(gD3D.pDXMgr);
		CoUninitialize();
		return NULL;
	}

	// Reset enumerator
	if (FAILED(pEnumPixelFormats->lpVtbl->Reset(pEnumPixelFormats)))
	{
		pEnumPixelFormats->lpVtbl->Release(pEnumPixelFormats);
		gD3D.lpD3DD3->lpVtbl->Release(gD3D.lpD3DD3);
		gD3D.pDXMgr->lpVtbl->Release(gD3D.pDXMgr);
		CoUninitialize();
		return NULL;
	}

	// Find 32-bit 8888 pixel format
	BOOL f8888 = FALSE;
	while (1)
	{
		// Get next pixel format
		if (FAILED(pEnumPixelFormats->lpVtbl->Next(pEnumPixelFormats, 1, &gD3D.ddpf8888, (UINT*)&dwSize)))
		{
			pEnumPixelFormats->lpVtbl->Release(pEnumPixelFormats);
			gD3D.lpD3DD3->lpVtbl->Release(gD3D.lpD3DD3);
			gD3D.pDXMgr->lpVtbl->Release(gD3D.pDXMgr);
			CoUninitialize();
			return NULL;
		}

		// No more formats
		if (dwSize == 0)
			break;

		// Check for 8888 format
		if ((gD3D.ddpf8888.dwFlags & DDPF_LUMINANCE) &&
			(gD3D.ddpf8888.dwRGBBitCount == 8) &&
			(gD3D.ddpf8888.dwRBitMask == 0xFF))
		{
			f8888 = TRUE;
			break;
		}
	}

	if (!f8888)
	{
		OutputDebugString("Wrapper: Not using luminance texture\n");
		gD3D.ddpf8888 = gD3D.ddpf555_565;
	}

	// We're done with the enumerator
	pEnumPixelFormats->lpVtbl->Release(pEnumPixelFormats);

	if (gD3D.devdesc.dwMaxTextureWidth < 512 || gD3D.devdesc.dwMaxTextureHeight < 512)
	{
		gD3D.useSubsample = TRUE;
		OutputDebugString("Wrapper: Subsampling textures to 256 x 256\n");
	}
	else
	{
		gD3D.useSubsample = FALSE;
	}

	if (gD3D.devdesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY)
	{
		gD3D.squareTexturesOnly = TRUE;
		OutputDebugString("Wrapper: Forcing all textures to be square\n");
	}
	else
	{
		gD3D.squareTexturesOnly = FALSE;
	}

	if (gD3D.devdesc.wMaxSimultaneousTextures > 1)
	{
		gD3D.useMultitexture = TRUE;
		OutputDebugString("Wrapper: Multitexturing enabled\n");
	}
	else
	{
		gD3D.useMultitexture = FALSE;
		OutputDebugString("Wrapper: Multitexturing not available with this driver\n");
	}

	if (!(gD3D.devdesc.dpcTriCaps.dwTextureFilterCaps & (D3DPTFILTERCAPS_MIPNEAREST | D3DPTFILTERCAPS_MIPLINEAR | D3DPTFILTERCAPS_LINEARMIPNEAREST | D3DPTFILTERCAPS_LINEARMIPLINEAR)))
	{
		gD3D.useMipmap = FALSE;
		OutputDebugString("Wrapper: Mipmapping disabled\n");
	}

	lResult = RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Quake", &hKey);
	if (lResult == ERROR_SUCCESS)
	{
		// We opened the existing key.
		dwSize = 4;
		lResult = RegQueryValueEx(hKey, "DoFlip", NULL, &dwType, (LPBYTE)&dwData, &dwSize);

		// Success?
		if (lResult == ERROR_SUCCESS &&
			dwType == 4 &&	// Only copy DWORD values
			dwData
			)
		{
			gD3D.doFlip = TRUE;
		}
		else
		{
			gD3D.doFlip = FALSE;
		}

		RegCloseKey(hKey);
	}
	else
	{
		gD3D.doFlip = FALSE;
	}

	// Get D3D interface
	LPDIRECT3D3 lpD3D3;
	if (FAILED(gD3D.pDXMgr->lpVtbl->GetDirect3D(gD3D.pDXMgr, &lpD3D3)))
	{
		gD3D.lpD3DD3->lpVtbl->Release(gD3D.lpD3DD3);
		gD3D.pDXMgr->lpVtbl->Release(gD3D.pDXMgr);
		CoUninitialize();
		return NULL;
	}

	D3DVERTEXBUFFERDESC vbdesc;
	vbdesc.dwSize = sizeof(vbdesc);

	if (hwdevdesc.dwFlags)
	{
		vbdesc.dwCaps = D3DVBCAPS_WRITEONLY;
	}
	else
	{
		vbdesc.dwCaps = D3DVBCAPS_SYSTEMMEMORY;
	}

	vbdesc.dwFVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2;
	vbdesc.dwNumVertices = D3D_MAX_VERTICES;

	// Create source vertex buffer
	if (FAILED(lpD3D3->lpVtbl->CreateVertexBuffer(lpD3D3, &vbdesc, &gD3D.lpD3DVBSrc, 0L, NULL)))
	{
		gD3D.lpD3DD3->lpVtbl->Release(gD3D.lpD3DD3);
		gD3D.pDXMgr->lpVtbl->Release(gD3D.pDXMgr);
		CoUninitialize();
		return NULL;
	}

	DWORD dummy;
	gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dummy);

	if (gD3D.useMultitexture)
	{
		vbdesc.dwFVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2;
	}
	else
	{
		vbdesc.dwFVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1;
	}

	// Create rendering vertex buffer
	if (FAILED(lpD3D3->lpVtbl->CreateVertexBuffer(lpD3D3, &vbdesc, &gD3D.lpD3DVB, 0L, NULL)))
	{
		gD3D.lpD3DD3->lpVtbl->Release(gD3D.lpD3DD3);
		gD3D.pDXMgr->lpVtbl->Release(gD3D.pDXMgr);
		CoUninitialize();
		return NULL;
	}

	lpD3D3->lpVtbl->Release(lpD3D3);

	gD3D.cullFaceMode = GL_BACK;
	gD3D.cullEnabled = FALSE;
	gD3D.textureValid = FALSE;
	gD3D.stage0Active = FALSE;
	gD3D.useSubStage = FALSE;
	gD3D.bLoad4444 = FALSE;
	gD3D.texEnvMode[0] = GL_MODULATE;
	gD3D.texEnvMode[1] = GL_MODULATE;
	gD3D.vertCount = 0;
	gD3D.vertStart = 0;
	gD3D.indexCount = 0;
	gD3D.textureStage = 0;

	for (i = 0; i < D3D_MAX_TEXTURES; i++)
	{
		gD3D.textures[i].lpDDS4 = NULL;
		gD3D.textures[i].minFilter = D3DTFN_POINT;
		gD3D.textures[i].magFilter = D3DTFG_LINEAR;
		gD3D.textures[i].mipFilter = D3DTFP_LINEAR;
		gD3D.textures[i].addressU = D3DTADDRESS_WRAP;
		gD3D.textures[i].addressV = D3DTADDRESS_WRAP;
		gD3D.textures[i].lpD3DT2 = NULL;
	}

	// Create identity matrix
	/*
		| 1 0 0 0 |
		| 0 1 0 0 |
		| 0 0 1 0 |
		| 0 0 0 1 |
	*/
	D3DMATRIX matrix;
	matrix._11 = 1.0f;	matrix._12 = 0.0f;	matrix._13 = 0.0f;	matrix._14 = 0.0f;
	matrix._21 = 0.0f;	matrix._22 = 1.0f;	matrix._23 = 0.0f;	matrix._24 = 0.0f;
	matrix._31 = 0.0f;	matrix._32 = 0.0f;	matrix._33 = 1.0f;	matrix._34 = 0.0f;
	matrix._41 = 0.0f;	matrix._42 = 0.0f;	matrix._43 = 0.0f;	matrix._44 = 1.0f;

	gD3D.lpD3DD3->lpVtbl->SetTransform(gD3D.lpD3DD3, D3DTRANSFORMSTATE_VIEW, &matrix);
	gD3D.lpD3DD3->lpVtbl->SetTransform(gD3D.lpD3DD3, D3DTRANSFORMSTATE_WORLD, &matrix);
	gD3D.lpD3DD3->lpVtbl->SetTransform(gD3D.lpD3DD3, D3DTRANSFORMSTATE_PROJECTION, &matrix);

	for (i = 1; i < D3D_MAX_RSTATES; i++)
	{
		gD3D.lpD3DD3->lpVtbl->GetRenderState(gD3D.lpD3DD3, (D3DRENDERSTATETYPE)i, &gD3D.rStates[i]);
	}

	for (i = 1; i < D3D_MAX_TSSTAGES; i++)
	{
		gD3D.lpD3DD3->lpVtbl->GetTextureStageState(gD3D.lpD3DD3, 0, (D3DTEXTURESTAGESTATETYPE)i, &gD3D.tsStates0[i]);
	}

	if (gD3D.useMultitexture)
	{
		for (i = 1; i < D3D_MAX_TSSTAGES; i++)
		{
			gD3D.lpD3DD3->lpVtbl->GetTextureStageState(gD3D.lpD3DD3, 1, (D3DTEXTURESTAGESTATETYPE)i, &gD3D.tsStates1[i]);
		}
	}

	gD3D.lpD3DT2 = NULL;
	gD3D.lpD3DT2SubStage = NULL;

	if (gD3D.rStates[D3DRENDERSTATE_TEXTUREPERSPECTIVE] != TRUE)
	{
		if (gD3D.indexCount)
		{
			if (gD3D.vertStart != gD3D.vertCount)
			{
				gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
				gD3D.vertStart = gD3D.vertCount;
			}

			gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
			gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
			gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dwSize);

			gD3D.vertStart = 0;
			gD3D.vertCount = 0;
			gD3D.indexCount = 0;
		}

		gD3D.rStates[D3DRENDERSTATE_TEXTUREPERSPECTIVE] = TRUE;
		gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_TEXTUREPERSPECTIVE, TRUE);
	}

	if (gD3D.rStates[D3DRENDERSTATE_SPECULARENABLE] != FALSE)
	{
		if (gD3D.indexCount)
		{
			if (gD3D.vertStart != gD3D.vertCount)
			{
				gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
				gD3D.vertStart = gD3D.vertCount;
			}

			gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
			gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
			gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dwSize);

			gD3D.vertStart = 0;
			gD3D.vertCount = 0;
			gD3D.indexCount = 0;
		}

		gD3D.rStates[D3DRENDERSTATE_SPECULARENABLE] = FALSE;
		gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_SPECULARENABLE, FALSE);
	}

	if (gD3D.rStates[D3DRENDERSTATE_DITHERENABLE] != TRUE)
	{
		if (gD3D.indexCount)
		{
			if (gD3D.vertStart != gD3D.vertCount)
			{
				gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
				gD3D.vertStart = gD3D.vertCount;
			}

			gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
			gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
			gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dwSize);

			gD3D.vertStart = 0;
			gD3D.vertCount = 0;
			gD3D.indexCount = 0;
		}

		gD3D.rStates[D3DRENDERSTATE_DITHERENABLE] = TRUE;
		gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_DITHERENABLE, TRUE);
	}

	if (gD3D.tsStates0[D3DTSS_TEXCOORDINDEX] != 0)
	{
		if (gD3D.indexCount)
		{
			if (gD3D.vertStart != gD3D.vertCount)
			{
				gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
				gD3D.vertStart = gD3D.vertCount;
			}

			gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
			gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
			gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dwSize);

			gD3D.vertStart = 0;
			gD3D.vertCount = 0;
			gD3D.indexCount = 0;
		}

		gD3D.tsStates0[D3DTSS_TEXCOORDINDEX] = 0;
		gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 0, D3DTSS_TEXCOORDINDEX, 0);
	}

	if (gD3D.useMultitexture == TRUE)
	{
		if (gD3D.tsStates1[D3DTSS_TEXCOORDINDEX] != 1)
		{
			if (gD3D.indexCount)
			{
				if (gD3D.vertStart != gD3D.vertCount)
				{
					gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
					gD3D.vertStart = gD3D.vertCount;
				}

				gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
				gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
				gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dwSize);

				gD3D.vertStart = 0;
				gD3D.vertCount = 0;
				gD3D.indexCount = 0;
			}

			gD3D.tsStates1[D3DTSS_TEXCOORDINDEX] = 1;
			gD3D.lpD3DD3->lpVtbl->SetTextureStageState(gD3D.lpD3DD3, 1, D3DTSS_TEXCOORDINDEX, 1);
		}
	}

	// Get DirectDraw interface
	if (FAILED(gD3D.pDXMgr->lpVtbl->GetDirectDraw(gD3D.pDXMgr, &gD3D.lpDD4)))
	{
		gD3D.lpD3DD3->lpVtbl->Release(gD3D.lpD3DD3);
		gD3D.pDXMgr->lpVtbl->Release(gD3D.pDXMgr);
		CoUninitialize();
		return NULL;
	}

	// Get current viewport
	if (FAILED(gD3D.lpD3DD3->lpVtbl->GetCurrentViewport(gD3D.lpD3DD3, &gD3D.lpD3DVP3)))
	{
		gD3D.lpD3DD3->lpVtbl->Release(gD3D.lpD3DD3);
		gD3D.lpDD4->lpVtbl->Release(gD3D.lpDD4);
		gD3D.pDXMgr->lpVtbl->Release(gD3D.pDXMgr);
		CoUninitialize();
		return NULL;
	}

	// Tell D3D we're ready to start rendering
	gD3D.lpD3DD3->lpVtbl->BeginScene(gD3D.lpD3DD3);

	return (HGLRC)1;
}

DLL_EXPORT HGLRC WINAPI wglCreateLayerContext( HDC hdc, int iLayerPlan )
{
	return (HGLRC)1;
}

DLL_EXPORT BOOL WINAPI wglDeleteContext( HGLRC hglrc )
{
	gD3D.lpD3DD3->lpVtbl->EndScene(gD3D.lpD3DD3);

	for (int i = 0; i < D3D_MAX_TEXTURES; i++)
	{
		D3D_TEXTURE* tex = &gD3D.textures[i];

		if (tex->lpD3DT2)
		{
			tex->lpD3DT2->lpVtbl->Release(tex->lpD3DT2);
			tex->lpD3DT2 = NULL;
		}

		if (tex->lpDDS4)
		{
			tex->lpDDS4->lpVtbl->Release(tex->lpDDS4);
			tex->lpDDS4 = NULL;
		}
	}
	
	gD3D.lpD3DVP3->lpVtbl->Release(gD3D.lpD3DVP3);
	gD3D.lpD3DVP3 = NULL;

	gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
	gD3D.lpD3DVBSrc->lpVtbl->Release(gD3D.lpD3DVBSrc);
	gD3D.lpD3DVBSrc = NULL;

	gD3D.lpD3DVB->lpVtbl->Release(gD3D.lpD3DVB);
	gD3D.lpD3DVB = NULL;

	gD3D.lpD3DD3->lpVtbl->Release(gD3D.lpD3DD3);
	gD3D.lpD3DD3 = NULL;

	gD3D.lpDD4->lpVtbl->Release(gD3D.lpDD4);
	gD3D.lpDD4 = NULL;

	gD3D.pDXMgr->lpVtbl->Release(gD3D.pDXMgr);
	gD3D.pDXMgr = NULL;

	CoUninitialize();

	return TRUE;
}

DLL_EXPORT BOOL WINAPI wglDescribeLayerPlane( HDC hdc, int iPixelFormat, int iLayerPlane, UINT nBytes, LPLAYERPLANEDESCRIPTOR plpd )
{
	return FALSE;
}

DLL_EXPORT HGLRC WINAPI wglGetCurrentContext( void )
{
	return (HGLRC)1;
}

DLL_EXPORT HDC WINAPI wglGetCurrentDC( void )
{
	return gD3D.hDC;
}

DLL_EXPORT int WINAPI wglGetLayerPaletteEntries( HDC hdc, int iLayerPlane, int iStart, int cEntries, COLORREF* pcr )
{
	return 0;
}

DLL_EXPORT PROC WINAPI wglGetProcAddress( LPCSTR lpszProc )
{
	if (!strcmp(lpszProc, "glMTexCoord2fSGIS"))
		return (PROC)glMTexCoord2fSGIS;
	else if (!strcmp(lpszProc, "glSelectTextureSGIS"))
		return (PROC)glSelectTextureSGIS;

	return NULL;
}

DLL_EXPORT BOOL WINAPI wglMakeCurrent( HDC hdc, HGLRC hglrc )
{
	return TRUE;
}

DLL_EXPORT BOOL WINAPI wglRealizeLayerPalette( HDC hdc, int iLayerPlane, BOOL bRealize )
{
	return FALSE;
}

DLL_EXPORT int WINAPI wglSetLayerPaletteEntries( HDC, int, int, int, CONST COLORREF* )
{
	return 0;
}

DLL_EXPORT BOOL APIENTRY wglShareLists( HGLRC, HGLRC )
{
	return FALSE;
}

DLL_EXPORT BOOL WINAPI wglSwapLayerBuffers( HDC, UINT )
{
	return FALSE;
}

DLL_EXPORT BOOL WINAPI wglUseFontBitmapsA( HDC, DWORD, DWORD, DWORD )
{
	return FALSE;
}

DLL_EXPORT BOOL WINAPI wglUseFontBitmapsW( HDC, DWORD, DWORD, DWORD )
{
	return FALSE;
}

DLL_EXPORT BOOL WINAPI wglUseFontOutlinesA( HDC, DWORD, DWORD, DWORD, FLOAT,
	FLOAT, int, LPGLYPHMETRICSFLOAT )
{
	return FALSE;
}

DLL_EXPORT BOOL WINAPI wglUseFontOutlinesW( HDC, DWORD, DWORD, DWORD, FLOAT,
	FLOAT, int, LPGLYPHMETRICSFLOAT )
{
	return FALSE;
}

DLL_EXPORT int APIENTRY wglChoosePixelFormat( HDC hdc, CONST PIXELFORMATDESCRIPTOR* ppfd )
{
	return 1;
}

DLL_EXPORT int APIENTRY wglDescribePixelFormat( HDC hdc, int iPixelFormat, UINT nBytes, LPPIXELFORMATDESCRIPTOR ppfd )
{
	ppfd->nSize = sizeof(PIXELFORMATDESCRIPTOR);	// size of this pfd
	ppfd->nVersion = 1;								// version number
	ppfd->dwFlags = PFD_GENERIC_ACCELERATED			// support GDI acceleration
		| PFD_DRAW_TO_WINDOW						// support window
		| PFD_SUPPORT_OPENGL						// support OpenGL
		| PFD_DOUBLEBUFFER;							// double buffered
	ppfd->iPixelType = PFD_TYPE_RGBA;				// RGBA type
	ppfd->cColorBits = 15;							// 15-bit color depth
	ppfd->cRedBits = 5;								// bits of red
	ppfd->cRedShift = 0;							// shift for red
	ppfd->cGreenBits = 5;							// bits of green
	ppfd->cGreenShift = 0;							// shift for green
	ppfd->cBlueBits = 5;							// bits of blue
	ppfd->cBlueShift = 0;							// shift for blue
	ppfd->cAlphaBits = 1;							// bits of alpha
	ppfd->cAlphaShift = 0;							// shift bit ignored
	ppfd->cAccumBits = 0;							// no accumulation buffer
	ppfd->cAccumRedBits = 0;						// accum bits ignored
	ppfd->cAccumGreenBits = 0;						// accum bits ignored
	ppfd->cAccumBlueBits = 0;						// accum bits ignored
	ppfd->cAccumAlphaBits = 0;						// accum bits ignored
	ppfd->cDepthBits = 16;							// 16-bit z-buffer
	ppfd->cStencilBits = 0;							// no stencil buffer
	ppfd->cAuxBuffers = 0;							// no auxiliary buffer
	ppfd->iLayerType = PFD_MAIN_PLANE;				// main layer
	ppfd->bReserved = 0;							// reserved
	ppfd->dwLayerMask = 0;							// layer masks ignored
	ppfd->dwVisibleMask = 0;						// layer masks ignored
	ppfd->dwDamageMask = 0;							// layer masks ignored

	return 1;
}

DLL_EXPORT int APIENTRY wglGetPixelFormat( HDC hdc )
{
	return 1;
}

DLL_EXPORT BOOL APIENTRY wglSetPixelFormat( HDC hdc, int iPixelFormat, CONST PIXELFORMATDESCRIPTOR* ppfd )
{
	return TRUE;
}

DLL_EXPORT BOOL APIENTRY wglSwapBuffers( HDC hdc )
{
	DWORD	dummy;

	if (gD3D.indexCount)
	{
		if (gD3D.vertStart != gD3D.vertCount)
		{
			gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
			gD3D.vertStart = gD3D.vertCount;
		}

		gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
		gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
		gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&gD3D.verts, &dummy);

		gD3D.vertStart = 0;
		gD3D.vertCount = 0;
		gD3D.indexCount = 0;
	}

	gD3D.lpD3DD3->lpVtbl->EndScene(gD3D.lpD3DD3);
	gD3D.pDXMgr->lpVtbl->Update(gD3D.pDXMgr, gD3D.doFlip ? 0 : DXMGRUPDATE_FORCEBLT, NULL);
	gD3D.lpD3DD3->lpVtbl->BeginScene(gD3D.lpD3DD3);

	return TRUE;
}

DLL_EXPORT void Download4444( void )
{
	gD3D.bLoad4444 = TRUE;
}

DLL_EXPORT void QGL_D3DShared( D3DGLOBALS* d3dGShared )
{
	gD3D.lpDD4 = d3dGShared->lpDD4;
	gD3D.isFullscreen = d3dGShared->bFullscreen;
	OutputDebugString("setting dd ipntr in dll\n");
}

/*
** QGL_D3DInit
*/
DLL_EXPORT HINSTANCE QGL_D3DInit( void )
{
	qglAccum					= dllAccum						= glAccum;
	qglAlphaFunc				= dllAlphaFunc					= glAlphaFunc;
	qglAreTexturesResident		= dllAreTexturesResident		= glAreTexturesResident;
	qglArrayElement				= dllArrayElement				= glArrayElement;
	qglBegin					= dllBegin						= glBegin;
	qglBindTexture				= dllBindTexture				= glBindTexture;
	qglBitmap					= dllBitmap						= glBitmap;
	qglBlendFunc				= dllBlendFunc					= glBlendFunc;
	qglCallList					= dllCallList					= glCallList;
	qglCallLists				= dllCallLists					= glCallLists;
	qglClear					= dllClear						= glClear;
	qglClearAccum				= dllClearAccum					= glClearAccum;
	qglClearColor				= dllClearColor					= glClearColor;
	qglClearDepth				= dllClearDepth					= glClearDepth;
	qglClearIndex				= dllClearIndex					= glClearIndex;
	qglClearStencil				= dllClearStencil				= glClearStencil;
	qglClipPlane				= dllClipPlane					= glClipPlane;
	qglColor3b					= dllColor3b					= glColor3b;
	qglColor3bv					= dllColor3bv					= glColor3bv;
	qglColor3d					= dllColor3d					= glColor3d;
	qglColor3dv					= dllColor3dv					= glColor3dv;
	qglColor3f					= dllColor3f					= glColor3f;
	qglColor3fv					= dllColor3fv					= glColor3fv;
	qglColor3i					= dllColor3i					= glColor3i;
	qglColor3iv					= dllColor3iv					= glColor3iv;
	qglColor3s					= dllColor3s					= glColor3s;
	qglColor3sv					= dllColor3sv					= glColor3sv;
	qglColor3ub					= dllColor3ub					= glColor3ub;
	qglColor3ubv				= dllColor3ubv					= glColor3ubv;
	qglColor3ui					= dllColor3ui					= glColor3ui;
	qglColor3uiv				= dllColor3uiv					= glColor3uiv;
	qglColor3us					= dllColor3us					= glColor3us;
	qglColor3usv				= dllColor3usv					= glColor3usv;
	qglColor4b					= dllColor4b					= glColor4b;
	qglColor4bv					= dllColor4bv					= glColor4bv;
	qglColor4d					= dllColor4d					= glColor4d;
	qglColor4dv					= dllColor4dv					= glColor4dv;
	qglColor4f					= dllColor4f					= glColor4f;
	qglColor4fv					= dllColor4fv					= glColor4fv;
	qglColor4i					= dllColor4i					= glColor4i;
	qglColor4iv					= dllColor4iv					= glColor4iv;
	qglColor4s					= dllColor4s					= glColor4s;
	qglColor4sv					= dllColor4sv					= glColor4sv;
	qglColor4ub					= dllColor4ub					= glColor4ub;
	qglColor4ubv				= dllColor4ubv					= glColor4ubv;
	qglColor4ui					= dllColor4ui					= glColor4ui;
	qglColor4uiv				= dllColor4uiv					= glColor4uiv;
	qglColor4us					= dllColor4us					= glColor4us;
	qglColor4usv				= dllColor4usv					= glColor4usv;
	qglColorMask				= dllColorMask					= glColorMask;
	qglColorMaterial			= dllColorMaterial				= glColorMaterial;
	qglColorPointer				= dllColorPointer				= glColorPointer;
	qglCopyPixels				= dllCopyPixels					= glCopyPixels;
	qglCopyTexImage1D			= dllCopyTexImage1D				= glCopyTexImage1D;
	qglCopyTexImage2D			= dllCopyTexImage2D				= glCopyTexImage2D;
	qglCopyTexSubImage1D		= dllCopyTexSubImage1D			= glCopyTexSubImage1D;
	qglCopyTexSubImage2D		= dllCopyTexSubImage2D			= glCopyTexSubImage2D;
	qglCullFace					= dllCullFace					= glCullFace;
	qglDeleteLists				= dllDeleteLists				= glDeleteLists;
	qglDeleteTextures			= dllDeleteTextures				= glDeleteTextures;
	qglDepthFunc				= dllDepthFunc					= glDepthFunc;
	qglDepthMask				= dllDepthMask					= glDepthMask;
	qglDepthRange				= dllDepthRange					= glDepthRange;
	qglDisable					= dllDisable					= glDisable;
	qglDisableClientState		= dllDisableClientState			= glDisableClientState;
	qglDrawArrays				= dllDrawArrays					= glDrawArrays;
	qglDrawBuffer				= dllDrawBuffer					= glDrawBuffer;
	qglDrawElements				= dllDrawElements				= glDrawElements;
	qglDrawPixels				= dllDrawPixels					= glDrawPixels;
	qglEdgeFlag					= dllEdgeFlag					= glEdgeFlag;
	qglEdgeFlagPointer			= dllEdgeFlagPointer			= glEdgeFlagPointer;
	qglEdgeFlagv				= dllEdgeFlagv					= glEdgeFlagv;
	qglEnable					= dllEnable						= glEnable;
	qglEnableClientState		= dllEnableClientState			= glEnableClientState;
	qglEnd						= dllEnd						= glEnd;
	qglEndList					= dllEndList					= glEndList;
	qglEvalCoord1d				= dllEvalCoord1d				= glEvalCoord1d;
	qglEvalCoord1dv				= dllEvalCoord1dv				= glEvalCoord1dv;
	qglEvalCoord1f				= dllEvalCoord1f				= glEvalCoord1f;
	qglEvalCoord1fv				= dllEvalCoord1fv				= glEvalCoord1fv;
	qglEvalCoord2d				= dllEvalCoord2d				= glEvalCoord2d;
	qglEvalCoord2dv				= dllEvalCoord2dv				= glEvalCoord2dv;
	qglEvalCoord2f				= dllEvalCoord2f				= glEvalCoord2f;
	qglEvalCoord2fv				= dllEvalCoord2fv				= glEvalCoord2fv;
	qglEvalMesh1				= dllEvalMesh1					= glEvalMesh1;
	qglEvalMesh2				= dllEvalMesh2					= glEvalMesh2;
	qglEvalPoint1				= dllEvalPoint1					= glEvalPoint1;
	qglEvalPoint2				= dllEvalPoint2					= glEvalPoint2;
	qglFeedbackBuffer			= dllFeedbackBuffer				= glFeedbackBuffer;
	qglFinish					= dllFinish						= glFinish;
	qglFlush					= dllFlush						= glFlush;
	qglFogf						= dllFogf						= glFogf;
	qglFogfv					= dllFogfv						= glFogfv;
	qglFogi						= dllFogi						= glFogi;
	qglFogiv					= dllFogiv						= glFogiv;
	qglFrontFace				= dllFrontFace					= glFrontFace;
	qglFrustum					= dllFrustum					= glFrustum;
	qglGenLists					= dllGenLists					= glGenLists;
	qglGenTextures				= dllGenTextures				= glGenTextures;
	qglGetBooleanv				= dllGetBooleanv				= glGetBooleanv;
	qglGetClipPlane				= dllGetClipPlane				= glGetClipPlane;
	qglGetDoublev				= dllGetDoublev					= glGetDoublev;
	qglGetError					= dllGetError					= glGetError;
	qglGetFloatv				= dllGetFloatv					= glGetFloatv;
	qglGetIntegerv				= dllGetIntegerv				= glGetIntegerv;
	qglGetLightfv				= dllGetLightfv					= glGetLightfv;
	qglGetLightiv				= dllGetLightiv					= glGetLightiv;
	qglGetMapdv					= dllGetMapdv					= glGetMapdv;
	qglGetMapfv					= dllGetMapfv					= glGetMapfv;
	qglGetMapiv					= dllGetMapiv					= glGetMapiv;
	qglGetMaterialfv			= dllGetMaterialfv				= glGetMaterialfv;
	qglGetMaterialiv			= dllGetMaterialiv				= glGetMaterialiv;
	qglGetPixelMapfv			= dllGetPixelMapfv				= glGetPixelMapfv;
	qglGetPixelMapuiv			= dllGetPixelMapuiv				= glGetPixelMapuiv;
	qglGetPixelMapusv			= dllGetPixelMapusv				= glGetPixelMapusv;
	qglGetPointerv				= dllGetPointerv				= glGetPointerv;
	qglGetPolygonStipple		= dllGetPolygonStipple			= glGetPolygonStipple;
	qglGetString				= dllGetString					= glGetString;
	qglGetTexEnvfv				= dllGetTexEnvfv				= glGetTexEnvfv;
	qglGetTexEnviv				= dllGetTexEnviv				= glGetTexEnviv;
	qglGetTexGendv				= dllGetTexGendv				= glGetTexGendv;
	qglGetTexGenfv				= dllGetTexGenfv				= glGetTexGenfv;
	qglGetTexGeniv				= dllGetTexGeniv				= glGetTexGeniv;
	qglGetTexImage				= dllGetTexImage				= glGetTexImage;
	qglGetTexLevelParameterfv	= dllGetTexLevelParameterfv		= NULL;
	qglGetTexLevelParameteriv	= dllGetTexLevelParameteriv		= NULL;
	qglGetTexParameterfv		= dllGetTexParameterfv			= glGetTexParameterfv;
	qglGetTexParameteriv		= dllGetTexParameteriv			= glGetTexParameteriv;
	qglHint						= dllHint						= glHint;
	qglIndexMask				= dllIndexMask					= glIndexMask;
	qglIndexPointer				= dllIndexPointer				= glIndexPointer;
	qglIndexd					= dllIndexd						= glIndexd;
	qglIndexdv					= dllIndexdv					= glIndexdv;
	qglIndexf					= dllIndexf						= glIndexf;
	qglIndexfv					= dllIndexfv					= glIndexfv;
	qglIndexi					= dllIndexi						= glIndexi;
	qglIndexiv					= dllIndexiv					= glIndexiv;
	qglIndexs					= dllIndexs						= glIndexs;
	qglIndexsv					= dllIndexsv					= glIndexsv;
	qglIndexub					= dllIndexub					= glIndexub;
	qglIndexubv					= dllIndexubv					= glIndexubv;
	qglInitNames				= dllInitNames					= glInitNames;
	qglInterleavedArrays		= dllInterleavedArrays			= glInterleavedArrays;
	qglIsEnabled				= dllIsEnabled					= glIsEnabled;
	qglIsList					= dllIsList						= glIsList;
	qglIsTexture				= dllIsTexture					= glIsTexture;
	qglLightModelf				= dllLightModelf				= glLightModelf;
	qglLightModelfv				= dllLightModelfv				= glLightModelfv;
	qglLightModeli				= dllLightModeli				= glLightModeli;
	qglLightModeliv				= dllLightModeliv				= glLightModeliv;
	qglLightf					= dllLightf						= glLightf;
	qglLightfv					= dllLightfv					= glLightfv;
	qglLighti					= dllLighti						= glLighti;
	qglLightiv					= dllLightiv					= glLightiv;
	qglLineStipple				= dllLineStipple				= glLineStipple;
	qglLineWidth				= dllLineWidth					= glLineWidth;
	qglListBase					= dllListBase					= glListBase;
	qglLoadIdentity				= dllLoadIdentity				= glLoadIdentity;
	qglLoadMatrixd				= dllLoadMatrixd				= glLoadMatrixd;
	qglLoadMatrixf				= dllLoadMatrixf				= glLoadMatrixf;
	qglLoadName					= dllLoadName					= glLoadName;
	qglLogicOp					= dllLogicOp					= glLogicOp;
	qglMap1d					= dllMap1d						= glMap1d;
	qglMap1f					= dllMap1f						= glMap1f;
	qglMap2d					= dllMap2d						= glMap2d;
	qglMap2f					= dllMap2f						= glMap2f;
	qglMapGrid1d				= dllMapGrid1d					= glMapGrid1d;
	qglMapGrid1f				= dllMapGrid1f					= glMapGrid1f;
	qglMapGrid2d				= dllMapGrid2d					= glMapGrid2d;
	qglMapGrid2f				= dllMapGrid2f					= glMapGrid2f;
	qglMaterialf				= dllMaterialf					= glMaterialf;
	qglMaterialfv				= dllMaterialfv					= glMaterialfv;
	qglMateriali				= dllMateriali					= glMateriali;
	qglMaterialiv				= dllMaterialiv					= glMaterialiv;
	qglMatrixMode				= dllMatrixMode					= glMatrixMode;
	qglMultMatrixd				= dllMultMatrixd				= glMultMatrixd;
	qglMultMatrixf				= dllMultMatrixf				= glMultMatrixf;
	qglNewList					= dllNewList					= glNewList;
	qglNormal3b					= dllNormal3b					= glNormal3b;
	qglNormal3bv				= dllNormal3bv					= glNormal3bv;
	qglNormal3d					= dllNormal3d					= glNormal3d;
	qglNormal3dv				= dllNormal3dv					= glNormal3dv;
	qglNormal3f					= dllNormal3f					= glNormal3f;
	qglNormal3fv				= dllNormal3fv					= glNormal3fv;
	qglNormal3i					= dllNormal3i					= glNormal3i;
	qglNormal3iv				= dllNormal3iv					= glNormal3iv;
	qglNormal3s					= dllNormal3s					= glNormal3s;
	qglNormal3sv				= dllNormal3sv					= glNormal3sv;
	qglNormalPointer			= dllNormalPointer				= glNormalPointer;
	qglOrtho					= dllOrtho						= glOrtho;
	qglPassThrough				= dllPassThrough				= glPassThrough;
	qglPixelMapfv				= dllPixelMapfv					= glPixelMapfv;
	qglPixelMapuiv				= dllPixelMapuiv				= glPixelMapuiv;
	qglPixelMapusv				= dllPixelMapusv				= glPixelMapusv;
	qglPixelStoref				= dllPixelStoref				= glPixelStoref;
	qglPixelStorei				= dllPixelStorei				= glPixelStorei;
	qglPixelTransferf			= dllPixelTransferf				= glPixelTransferf;
	qglPixelTransferi			= dllPixelTransferi				= glPixelTransferi;
	qglPixelZoom				= dllPixelZoom					= glPixelZoom;
	qglPointSize				= dllPointSize					= glPointSize;
	qglPolygonMode				= dllPolygonMode				= glPolygonMode;
	qglPolygonOffset			= dllPolygonOffset				= glPolygonOffset;
	qglPolygonStipple			= dllPolygonStipple				= glPolygonStipple;
	qglPopAttrib				= dllPopAttrib					= glPopAttrib;
	qglPopClientAttrib			= dllPopClientAttrib			= glPopClientAttrib;
	qglPopMatrix				= dllPopMatrix					= glPopMatrix;
	qglPopName					= dllPopName					= glPopName;
	qglPrioritizeTextures		= dllPrioritizeTextures			= glPrioritizeTextures;
	qglPushAttrib				= dllPushAttrib					= glPushAttrib;
	qglPushClientAttrib			= dllPushClientAttrib			= glPushClientAttrib;
	qglPushMatrix				= dllPushMatrix					= glPushMatrix;
	qglPushName					= dllPushName					= glPushName;
	qglRasterPos2d				= dllRasterPos2d				= glRasterPos2d;
	qglRasterPos2dv				= dllRasterPos2dv				= glRasterPos2dv;
	qglRasterPos2f				= dllRasterPos2f				= glRasterPos2f;
	qglRasterPos2fv				= dllRasterPos2fv				= glRasterPos2fv;
	qglRasterPos2i				= dllRasterPos2i				= glRasterPos2i;
	qglRasterPos2iv				= dllRasterPos2iv				= glRasterPos2iv;
	qglRasterPos2s				= dllRasterPos2s				= glRasterPos2s;
	qglRasterPos2sv				= dllRasterPos2sv				= glRasterPos2sv;
	qglRasterPos3d				= dllRasterPos3d				= glRasterPos3d;
	qglRasterPos3dv				= dllRasterPos3dv				= glRasterPos3dv;
	qglRasterPos3f				= dllRasterPos3f				= glRasterPos3f;
	qglRasterPos3fv				= dllRasterPos3fv				= glRasterPos3fv;
	qglRasterPos3i				= dllRasterPos3i				= glRasterPos3i;
	qglRasterPos3iv				= dllRasterPos3iv				= glRasterPos3iv;
	qglRasterPos3s				= dllRasterPos3s				= glRasterPos3s;
	qglRasterPos3sv				= dllRasterPos3sv				= glRasterPos3sv;
	qglRasterPos4d				= dllRasterPos4d				= glRasterPos4d;
	qglRasterPos4dv				= dllRasterPos4dv				= glRasterPos4dv;
	qglRasterPos4f				= dllRasterPos4f				= glRasterPos4f;
	qglRasterPos4fv				= dllRasterPos4fv				= glRasterPos4fv;
	qglRasterPos4i				= dllRasterPos4i				= glRasterPos4i;
	qglRasterPos4iv				= dllRasterPos4iv				= glRasterPos4iv;
	qglRasterPos4s				= dllRasterPos4s				= glRasterPos4s;
	qglRasterPos4sv				= dllRasterPos4sv				= glRasterPos4sv;
	qglReadBuffer				= dllReadBuffer					= glReadBuffer;
	qglReadPixels				= dllReadPixels					= glReadPixels;
	qglRectd					= dllRectd						= glRectd;
	qglRectdv					= dllRectdv						= glRectdv;
	qglRectf					= dllRectf						= glRectf;
	qglRectfv					= dllRectfv						= glRectfv;
	qglRecti					= dllRecti						= glRecti;
	qglRectiv					= dllRectiv						= glRectiv;
	qglRects					= dllRects						= glRects;
	qglRectsv					= dllRectsv						= glRectsv;
	qglRenderMode				= dllRenderMode					= glRenderMode;
	qglRotated					= dllRotated					= glRotated;
	qglRotatef					= dllRotatef					= glRotatef;
	qglScaled					= dllScaled						= glScaled;
	qglScalef					= dllScalef						= glScalef;
	qglScissor					= dllScissor					= glScissor;
	qglSelectBuffer				= dllSelectBuffer				= glSelectBuffer;
	qglShadeModel				= dllShadeModel					= glShadeModel;
	qglStencilFunc				= dllStencilFunc				= glStencilFunc;
	qglStencilMask				= dllStencilMask				= glStencilMask;
	qglStencilOp				= dllStencilOp					= glStencilOp;
	qglTexCoord1d				= dllTexCoord1d					= glTexCoord1d;
	qglTexCoord1dv				= dllTexCoord1dv				= glTexCoord1dv;
	qglTexCoord1f				= dllTexCoord1f					= glTexCoord1f;
	qglTexCoord1fv				= dllTexCoord1fv				= glTexCoord1fv;
	qglTexCoord1i				= dllTexCoord1i					= glTexCoord1i;
	qglTexCoord1iv				= dllTexCoord1iv				= glTexCoord1iv;
	qglTexCoord1s				= dllTexCoord1s					= glTexCoord1s;
	qglTexCoord1sv				= dllTexCoord1sv				= glTexCoord1sv;
	qglTexCoord2d				= dllTexCoord2d					= glTexCoord2d;
	qglTexCoord2dv				= dllTexCoord2dv				= glTexCoord2dv;
	qglTexCoord2f				= dllTexCoord2f					= glTexCoord2f;
	qglTexCoord2fv				= dllTexCoord2fv				= glTexCoord2fv;
	qglTexCoord2i				= dllTexCoord2i					= glTexCoord2i;
	qglTexCoord2iv				= dllTexCoord2iv				= glTexCoord2iv;
	qglTexCoord2s				= dllTexCoord2s					= glTexCoord2s;
	qglTexCoord2sv				= dllTexCoord2sv				= glTexCoord2sv;
	qglTexCoord3d				= dllTexCoord3d					= glTexCoord3d;
	qglTexCoord3dv				= dllTexCoord3dv				= glTexCoord3dv;
	qglTexCoord3f				= dllTexCoord3f					= glTexCoord3f;
	qglTexCoord3fv				= dllTexCoord3fv				= glTexCoord3fv;
	qglTexCoord3i				= dllTexCoord3i					= glTexCoord3i;
	qglTexCoord3iv				= dllTexCoord3iv				= glTexCoord3iv;
	qglTexCoord3s				= dllTexCoord3s					= glTexCoord3s;
	qglTexCoord3sv				= dllTexCoord3sv				= glTexCoord3sv;
	qglTexCoord4d				= dllTexCoord4d					= glTexCoord4d;
	qglTexCoord4dv				= dllTexCoord4dv				= glTexCoord4dv;
	qglTexCoord4f				= dllTexCoord4f					= glTexCoord4f;
	qglTexCoord4fv				= dllTexCoord4fv				= glTexCoord4fv;
	qglTexCoord4i				= dllTexCoord4i					= glTexCoord4i;
	qglTexCoord4iv				= dllTexCoord4iv				= glTexCoord4iv;
	qglTexCoord4s				= dllTexCoord4s					= glTexCoord4s;
	qglTexCoord4sv				= dllTexCoord4sv				= glTexCoord4sv;
	qglTexCoordPointer			= dllTexCoordPointer			= glTexCoordPointer;
	qglTexEnvf					= dllTexEnvf					= glTexEnvf;
	qglTexEnvfv					= dllTexEnvfv					= glTexEnvfv;
	qglTexEnvi					= dllTexEnvi					= glTexEnvi;
	qglTexEnviv					= dllTexEnviv					= glTexEnviv;
	qglTexGend					= dllTexGend					= glTexGend;
	qglTexGendv					= dllTexGendv					= glTexGendv;
	qglTexGenf					= dllTexGenf					= glTexGenf;
	qglTexGenfv					= dllTexGenfv					= glTexGenfv;
	qglTexGeni					= dllTexGeni					= glTexGeni;
	qglTexGeniv					= dllTexGeniv					= glTexGeniv;
	qglTexImage1D				= dllTexImage1D					= glTexImage1D;
	qglTexImage2D				= dllTexImage2D					= glTexImage2D;
	qglTexParameterf			= dllTexParameterf				= glTexParameterf;
	qglTexParameterfv			= dllTexParameterfv				= glTexParameterfv;
	qglTexParameteri			= dllTexParameteri				= glTexParameteri;
	qglTexParameteriv			= dllTexParameteriv				= glTexParameteriv;
	qglTexSubImage1D			= dllTexSubImage1D				= glTexSubImage1D;
	qglTexSubImage2D			= dllTexSubImage2D				= glTexSubImage2D;
	qglTranslated				= dllTranslated					= glTranslated;
	qglTranslatef				= dllTranslatef					= glTranslatef;
	qglVertex2d					= dllVertex2d					= glVertex2d;
	qglVertex2dv				= dllVertex2dv					= glVertex2dv;
	qglVertex2f					= dllVertex2f					= glVertex2f;
	qglVertex2fv				= dllVertex2fv					= glVertex2fv;
	qglVertex2i					= dllVertex2i					= glVertex2i;
	qglVertex2iv				= dllVertex2iv					= glVertex2iv;
	qglVertex2s					= dllVertex2s					= glVertex2s;
	qglVertex2sv				= dllVertex2sv					= glVertex2sv;
	qglVertex3d					= dllVertex3d					= glVertex3d;
	qglVertex3dv				= dllVertex3dv					= glVertex3dv;
	qglVertex3f					= dllVertex3f					= glVertex3f;
	qglVertex3fv				= dllVertex3fv					= glVertex3fv;
	qglVertex3i					= dllVertex3i					= glVertex3i;
	qglVertex3iv				= dllVertex3iv					= glVertex3iv;
	qglVertex3s					= dllVertex3s					= glVertex3s;
	qglVertex3sv				= dllVertex3sv					= glVertex3sv;
	qglVertex4d					= dllVertex4d					= glVertex4d;
	qglVertex4dv				= dllVertex4dv					= glVertex4dv;
	qglVertex4f					= dllVertex4f					= glVertex4f;
	qglVertex4fv				= dllVertex4fv					= glVertex4fv;
	qglVertex4i					= dllVertex4i					= glVertex4i;
	qglVertex4iv				= dllVertex4iv					= glVertex4iv;
	qglVertex4s					= dllVertex4s					= glVertex4s;
	qglVertex4sv				= dllVertex4sv					= glVertex4sv;
	qglVertexPointer			= dllVertexPointer				= glVertexPointer;
	qglViewport					= dllViewport					= glViewport;

	qwglCopyContext				= wglCopyContext;
	qwglCreateContext			= wglCreateContext;
	qwglCreateLayerContext		= wglCreateLayerContext;
	qwglDeleteContext			= wglDeleteContext;
	qwglDescribeLayerPlane		= wglDescribeLayerPlane;
	qwglGetCurrentContext		= wglGetCurrentContext;
	qwglGetCurrentDC			= wglGetCurrentDC;
	qwglGetLayerPaletteEntries	= wglGetLayerPaletteEntries;
	qwglGetProcAddress			= wglGetProcAddress;
	qwglMakeCurrent				= wglMakeCurrent;
	qwglRealizeLayerPalette		= wglRealizeLayerPalette;
	qwglSetLayerPaletteEntries	= wglSetLayerPaletteEntries;
	qwglShareLists				= wglShareLists;
	qwglSwapLayerBuffers		= wglSwapLayerBuffers;
	qwglUseFontBitmaps			= wglUseFontBitmaps;
	qwglUseFontOutlines			= wglUseFontOutlines;
	qwglChoosePixelFormat		= wglChoosePixelFormat;
	qwglDescribePixelFormat		= wglDescribePixelFormat;
	qwglGetPixelFormat			= wglGetPixelFormat;
	qwglSetPixelFormat			= wglSetPixelFormat;
	qwglSwapBuffers				= wglSwapBuffers;
	qwglSwapIntervalEXT			= NULL;

	qglPointParameterfEXT		= NULL;
	qglPointParameterfvEXT		= NULL;
	qglColorTableEXT			= NULL;
	qglSelectTextureSGIS		= NULL;
	qglMTexCoord2fSGIS			= NULL;

	return (HINSTANCE)1;
}