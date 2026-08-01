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

D3D_GLOBALS g;

static void QuakeFlushIndexedPrimitives( void )
{
	DWORD	dummy;

	if (g.m_indexCount)
	{
		if (g.m_vertStart != g.m_vertCount)
		{
			g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
			g.m_vertStart = g.m_vertCount;
		}

		g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
		g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
		g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dummy);

		g.m_vertStart = 0;
		g.m_vertCount = 0;
		g.m_indexCount = 0;
	}
}

static void QuakeSetTextureStageState( DWORD stage, D3DTEXTURESTAGESTATETYPE stageStartType, DWORD value )
{
	DWORD* pType;
	DWORD	dummy;

	pType = &g.m_tsStates0[(D3D_MAX_TSSTAGES * stage) + stageStartType];

	if (*pType != value)
	{
		if (g.m_indexCount)
		{
			if (g.m_vertStart != g.m_vertCount)
			{
				g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
				g.m_vertStart = g.m_vertCount;
			}

			g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
			g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
			g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dummy);

			g.m_vertStart = 0;
			g.m_vertCount = 0;
			g.m_indexCount = 0;
		}

		*pType = value;
		g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, stage, stageStartType, value);
	}
}

static void QuakeFlushVertexBuffer( void )
{
	if (g.m_vertStart != g.m_vertCount)
	{
		g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
		g.m_vertStart = g.m_vertCount;
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

static HRESULT LoadSubSurface( LPDIRECTDRAWSURFACE4 m_pDDS4, DWORD internalFormat, DWORD width, DWORD height, BYTE* pixels, RECT* rect )
{
	DDSURFACEDESC2	ddsd;
	HRESULT	hResult;
	DWORD newWidth, newHeight;
	DWORD* srcPic, * pic;

	newWidth = rect->right - rect->left;
	newHeight = rect->bottom - rect->top;

	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);

	hResult = m_pDDS4->lpVtbl->Lock(m_pDDS4, rect, &ddsd, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, NULL);
	if (hResult != DD_OK)
	{
		m_pDDS4->lpVtbl->Release(m_pDDS4);
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

	m_pDDS4->lpVtbl->Unlock(m_pDDS4, NULL);
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
		if (g.m_rStates[D3DRENDERSTATE_ALPHAFUNC] != alphaFunc)
		{
			if (g.m_indexCount)
			{
				if (g.m_vertStart != g.m_vertCount)
				{
					g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
					g.m_vertStart = g.m_vertCount;
				}

				g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
				g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
				g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dummy);

				g.m_vertStart = 0;
				g.m_vertCount = 0;
				g.m_indexCount = 0;
			}

			g.m_rStates[D3DRENDERSTATE_ALPHAFUNC] = alphaFunc;
			g.m_pD3DDev->lpVtbl->SetRenderState(g.m_pD3DDev, D3DRENDERSTATE_ALPHAFUNC, alphaFunc);
		}

		alphaRef = (DWORD)(ref * 255.0);
		if (g.m_rStates[D3DRENDERSTATE_ALPHAREF] != alphaRef)
		{
			if (g.m_indexCount)
			{
				if (g.m_vertStart != g.m_vertCount)
				{
					g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
					g.m_vertStart = g.m_vertCount;
				}

				g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
				g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
				g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dummy);

				g.m_vertStart = 0;
				g.m_vertCount = 0;
				g.m_indexCount = 0;
			}

			g.m_rStates[D3DRENDERSTATE_ALPHAREF] = alphaRef;
			g.m_pD3DDev->lpVtbl->SetRenderState(g.m_pD3DDev, D3DRENDERSTATE_ALPHAREF, alphaRef);
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
	unsigned int r2, g2, b2;

	vert = &g.m_verts[g.m_vertCount + g.m_primVertCount];

	verts = (D3DVALUE*)((byte*)g.m_vertexPointer + (sizeof(D3DVALUE) * 4) * i);
	vert->m_x = verts[0];
	vert->m_y = verts[1];
	vert->m_z = verts[2];

	colors = (D3DVALUE*)((byte*)g.m_colorPointer + (sizeof(D3DVALUE) * 3) * i);
	r2 = (unsigned int)(colors[0] * scale);
	g2 = (unsigned int)(colors[1] * scale);
	b2 = (unsigned int)(colors[2] * scale);
	if (r2 > 255)
		r2 = 255;
	if (g2 > 255)
		g2 = 255;
	if (b2 > 255)
		b2 = 255;
	vert->m_color = RGBA_MAKE(r2, g2, b2, 255);
	vert->m_tu = g.m_tu;
	vert->m_tv = g.m_tv;

	if (g.m_useSubStage)
	{
		vert->m_tu2 = g.m_tu2;
		vert->m_tv2 = g.m_tv2;
	}

	g.m_primVertCount++;
}

DLL_EXPORT void APIENTRY glBegin( GLenum mode )
{
	D3D_TEXTURE* tex;

	g.m_primMode = mode;
	g.m_primVertCount = 0;

	if (!g.m_stage0Active || g.m_textureValid)
		return;

	tex = &g.m_textures[g.m_currentTexture[0]];

	if (g.m_tsStates0[D3DTSS_ADDRESSU] != tex->m_addressU)
	{
		QuakeFlushIndexedPrimitives();
		g.m_tsStates0[D3DTSS_ADDRESSU] = tex->m_addressU;
		g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_ADDRESSU, tex->m_addressU);
	}

	if (g.m_tsStates0[D3DTSS_ADDRESSV] != tex->m_addressV)
	{
		QuakeFlushIndexedPrimitives();
		g.m_tsStates0[D3DTSS_ADDRESSV] = tex->m_addressV;
		g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_ADDRESSV, tex->m_addressV);
	}

	if (g.m_tsStates0[D3DTSS_MAGFILTER] != tex->m_magFilter)
	{
		QuakeFlushIndexedPrimitives();
		g.m_tsStates0[D3DTSS_MAGFILTER] = tex->m_magFilter;
		g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_MAGFILTER, tex->m_magFilter);
	}

	if (g.m_tsStates0[D3DTSS_MINFILTER] != tex->m_minFilter)
	{
		QuakeFlushIndexedPrimitives();
		g.m_tsStates0[D3DTSS_MINFILTER] = tex->m_minFilter;
		g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_MINFILTER, tex->m_minFilter);
	}

	if (g.m_tsStates0[D3DTSS_MIPFILTER] != tex->m_mipFilter)
	{
		QuakeFlushIndexedPrimitives();
		g.m_tsStates0[D3DTSS_MIPFILTER] = tex->m_mipFilter;
		g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_MIPFILTER, tex->m_mipFilter);
	}

	if (g.m_pD3DTexture != tex->m_pD3DTexture)
	{
		QuakeFlushIndexedPrimitives();
		g.m_pD3DTexture = tex->m_pD3DTexture;
		g.m_pD3DDev->lpVtbl->SetTexture(g.m_pD3DDev, 0, tex->m_pD3DTexture);
	}

	if (g.m_texEnvMode[0] == GL_BLEND)
	{
		if (tex->m_internalFormat == 1 || tex->m_internalFormat == 3)
		{
			QuakeSetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE | D3DTA_COMPLEMENT);
			QuakeSetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			QuakeSetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			QuakeSetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			QuakeSetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
			QuakeSetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
		}
		else if (tex->m_internalFormat == 4)
		{
			QuakeSetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE | D3DTA_COMPLEMENT);
			QuakeSetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			QuakeSetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			QuakeSetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			QuakeSetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
			QuakeSetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		}
	}
	else if (g.m_texEnvMode[0] == GL_REPLACE)
	{
		if (tex->m_internalFormat == 1 || tex->m_internalFormat == 3)
		{
			if (g.m_tsStates0[D3DTSS_COLORARG1] != D3DTA_TEXTURE)
			{
				QuakeFlushIndexedPrimitives();
				g.m_tsStates0[D3DTSS_COLORARG1] = D3DTA_TEXTURE;
				g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			}

			if (g.m_tsStates0[D3DTSS_COLORARG2] != D3DTA_DIFFUSE)
			{
				QuakeFlushIndexedPrimitives();
				g.m_tsStates0[D3DTSS_COLORARG2] = D3DTA_DIFFUSE;
				g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			}

			if (g.m_tsStates0[D3DTSS_COLOROP] != D3DTOP_SELECTARG1)
			{
				QuakeFlushIndexedPrimitives();
				g.m_tsStates0[D3DTSS_COLOROP] = D3DTOP_SELECTARG1;
				g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
			}

			if (g.m_tsStates0[D3DTSS_ALPHAARG1] != D3DTA_TEXTURE)
			{
				QuakeFlushIndexedPrimitives();
				g.m_tsStates0[D3DTSS_ALPHAARG1] = D3DTA_TEXTURE;
				g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			}

			if (g.m_tsStates0[D3DTSS_ALPHAARG2] != D3DTA_DIFFUSE)
			{
				QuakeFlushIndexedPrimitives();
				g.m_tsStates0[D3DTSS_ALPHAARG2] = D3DTA_DIFFUSE;
				g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
			}

			if (g.m_tsStates0[D3DTSS_ALPHAOP] != D3DTOP_SELECTARG2)
			{
				QuakeFlushIndexedPrimitives();
				g.m_tsStates0[D3DTSS_ALPHAOP] = D3DTOP_SELECTARG2;
				g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
			}
		}
		else if (tex->m_internalFormat == 4)
		{
			if (g.m_tsStates0[D3DTSS_COLORARG1] != D3DTA_TEXTURE)
			{
				QuakeFlushIndexedPrimitives();
				g.m_tsStates0[D3DTSS_COLORARG1] = D3DTA_TEXTURE;
				g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			}

			if (g.m_tsStates0[D3DTSS_COLORARG2] != D3DTA_DIFFUSE)
			{
				QuakeFlushIndexedPrimitives();
				g.m_tsStates0[D3DTSS_COLORARG2] = D3DTA_DIFFUSE;
				g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			}

			if (g.m_tsStates0[D3DTSS_COLOROP] != D3DTOP_SELECTARG1)
			{
				QuakeFlushIndexedPrimitives();
				g.m_tsStates0[D3DTSS_COLOROP] = D3DTOP_SELECTARG1;
				g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
			}

			if (g.m_tsStates0[D3DTSS_ALPHAARG1] != D3DTA_TEXTURE)
			{
				QuakeFlushIndexedPrimitives();
				g.m_tsStates0[D3DTSS_ALPHAARG1] = D3DTA_TEXTURE;
				g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			}

			if (g.m_tsStates0[D3DTSS_ALPHAARG2] != D3DTA_DIFFUSE)
			{
				QuakeFlushIndexedPrimitives();
				g.m_tsStates0[D3DTSS_ALPHAARG2] = D3DTA_DIFFUSE;
				g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
			}

			if (g.m_tsStates0[D3DTSS_ALPHAOP] != D3DTOP_SELECTARG1)
			{
				QuakeFlushIndexedPrimitives();
				g.m_tsStates0[D3DTSS_ALPHAOP] = D3DTOP_SELECTARG1;
				g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
			}
		}
	}
	else if (g.m_texEnvMode[0] == GL_MODULATE)
	{
		if (tex->m_internalFormat == 1 || tex->m_internalFormat == 3)
		{
			if (g.m_tsStates0[D3DTSS_COLORARG1] != D3DTA_TEXTURE)
			{
				QuakeFlushIndexedPrimitives();
				g.m_tsStates0[D3DTSS_COLORARG1] = D3DTA_TEXTURE;
				g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			}

			if (g.m_tsStates0[D3DTSS_COLORARG2] != D3DTA_DIFFUSE)
			{
				QuakeFlushIndexedPrimitives();
				g.m_tsStates0[D3DTSS_COLORARG2] = D3DTA_DIFFUSE;
				g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			}

			if (g.m_tsStates0[D3DTSS_COLOROP] != D3DTOP_MODULATE)
			{
				QuakeFlushIndexedPrimitives();
				g.m_tsStates0[D3DTSS_COLOROP] = D3DTOP_MODULATE;
				g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			}

			if (g.m_tsStates0[D3DTSS_ALPHAARG1] != D3DTA_TEXTURE)
			{
				QuakeFlushIndexedPrimitives();
				g.m_tsStates0[D3DTSS_ALPHAARG1] = D3DTA_TEXTURE;
				g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			}

			if (g.m_tsStates0[D3DTSS_ALPHAARG2] != D3DTA_DIFFUSE)
			{
				QuakeFlushIndexedPrimitives();
				g.m_tsStates0[D3DTSS_ALPHAARG2] = D3DTA_DIFFUSE;
				g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
			}

			if (g.m_tsStates0[D3DTSS_ALPHAOP] != D3DTOP_SELECTARG2)
			{
				QuakeFlushIndexedPrimitives();
				g.m_tsStates0[D3DTSS_ALPHAOP] = D3DTOP_SELECTARG2;
				g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
			}
		}
		else if (tex->m_internalFormat == 4)
		{
			if (g.m_tsStates0[D3DTSS_COLORARG1] != D3DTA_TEXTURE)
			{
				QuakeFlushIndexedPrimitives();
				g.m_tsStates0[D3DTSS_COLORARG1] = D3DTA_TEXTURE;
				g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			}

			if (g.m_tsStates0[D3DTSS_COLORARG2] != D3DTA_DIFFUSE)
			{
				QuakeFlushIndexedPrimitives();
				g.m_tsStates0[D3DTSS_COLORARG2] = D3DTA_DIFFUSE;
				g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			}

			if (g.m_tsStates0[D3DTSS_COLOROP] != D3DTOP_MODULATE)
			{
				QuakeFlushIndexedPrimitives();
				g.m_tsStates0[D3DTSS_COLOROP] = D3DTOP_MODULATE;
				g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			}

			if (g.m_tsStates0[D3DTSS_ALPHAARG1] != D3DTA_TEXTURE)
			{
				QuakeFlushIndexedPrimitives();
				g.m_tsStates0[D3DTSS_ALPHAARG1] = D3DTA_TEXTURE;
				g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			}

			if (g.m_tsStates0[D3DTSS_ALPHAARG2] != D3DTA_DIFFUSE)
			{
				QuakeFlushIndexedPrimitives();
				g.m_tsStates0[D3DTSS_ALPHAARG2] = D3DTA_DIFFUSE;
				g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
			}

			if (g.m_tsStates0[D3DTSS_ALPHAOP] != D3DTOP_MODULATE)
			{
				QuakeFlushIndexedPrimitives();
				g.m_tsStates0[D3DTSS_ALPHAOP] = D3DTOP_MODULATE;
				g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			}
		}
	}
	else if (g.m_texEnvMode[0] == GL_DECAL)
	{
		if (tex->m_internalFormat == 3)
		{
			if (g.m_tsStates0[D3DTSS_COLORARG1] != D3DTA_TEXTURE)
			{
				QuakeFlushIndexedPrimitives();
				g.m_tsStates0[D3DTSS_COLORARG1] = D3DTA_TEXTURE;
				g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			}

			if (g.m_tsStates0[D3DTSS_COLORARG2] != D3DTA_DIFFUSE)
			{
				QuakeFlushIndexedPrimitives();
				g.m_tsStates0[D3DTSS_COLORARG2] = D3DTA_DIFFUSE;
				g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			}

			if (g.m_tsStates0[D3DTSS_COLOROP] != D3DTOP_SELECTARG1)
			{
				QuakeFlushIndexedPrimitives();
				g.m_tsStates0[D3DTSS_COLOROP] = D3DTOP_SELECTARG1;
				g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
			}

			if (g.m_tsStates0[D3DTSS_ALPHAARG1] != D3DTA_TEXTURE)
			{
				QuakeFlushIndexedPrimitives();
				g.m_tsStates0[D3DTSS_ALPHAARG1] = D3DTA_TEXTURE;
				g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			}

			if (g.m_tsStates0[D3DTSS_ALPHAARG2] != D3DTA_DIFFUSE)
			{
				QuakeFlushIndexedPrimitives();
				g.m_tsStates0[D3DTSS_ALPHAARG2] = D3DTA_DIFFUSE;
				g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
			}

			if (g.m_tsStates0[D3DTSS_ALPHAOP] != D3DTOP_SELECTARG2)
			{
				QuakeFlushIndexedPrimitives();
				g.m_tsStates0[D3DTSS_ALPHAOP] = D3DTOP_SELECTARG2;
				g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
			}
		}
		else if (tex->m_internalFormat == 4)
		{
			QuakeSetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			QuakeSetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			QuakeSetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA);
			QuakeSetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			QuakeSetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
			QuakeSetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
		}
	}

	if (g.m_useSubStage)
	{
		tex = &g.m_textures[g.m_currentTexture[1]];

		QuakeSetTextureStageState(1, D3DTSS_ADDRESSU, tex->m_addressU);
		QuakeSetTextureStageState(1, D3DTSS_ADDRESSV, tex->m_addressV);
		QuakeSetTextureStageState(1, D3DTSS_MAGFILTER, tex->m_magFilter);
		QuakeSetTextureStageState(1, D3DTSS_MINFILTER, tex->m_minFilter);
		QuakeSetTextureStageState(1, D3DTSS_MIPFILTER, tex->m_mipFilter);

		if (g.m_pD3DTextureSubStage != tex->m_pD3DTexture)
		{
			QuakeFlushIndexedPrimitives();
			g.m_pD3DTextureSubStage = tex->m_pD3DTexture;
			g.m_pD3DDev->lpVtbl->SetTexture(g.m_pD3DDev, 1, tex->m_pD3DTexture);
		}

		if (g.m_texEnvMode[1] == GL_BLEND)
		{
			if (tex->m_internalFormat == 1 || tex->m_internalFormat == 3)
			{
				QuakeSetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE | D3DTA_COMPLEMENT);
				QuakeSetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
				QuakeSetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
			}
			else if (tex->m_internalFormat == 4)
			{
				QuakeSetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE | D3DTA_COMPLEMENT);
				QuakeSetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
				QuakeSetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			}
		}
		else if (g.m_texEnvMode[1] == GL_REPLACE)
		{
			if (tex->m_internalFormat == 1 || tex->m_internalFormat == 3)
			{
				QuakeSetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				QuakeSetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
				QuakeSetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
			}
			else if (tex->m_internalFormat == 4)
			{
				QuakeSetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				QuakeSetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
				QuakeSetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
			}
		}
		else if (g.m_texEnvMode[1] == GL_MODULATE)
		{
			if (tex->m_internalFormat == 1 || tex->m_internalFormat == 3)
			{
				QuakeSetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				QuakeSetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
				QuakeSetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
			}
			else if (tex->m_internalFormat == 4)
			{
				QuakeSetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				QuakeSetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
				QuakeSetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			}
		}
		else if (g.m_texEnvMode[1] == GL_DECAL)
		{
			if (tex->m_internalFormat == 3)
			{
				QuakeSetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				QuakeSetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
				QuakeSetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
				QuakeSetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
			}
			else if (tex->m_internalFormat == 4)
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

	g.m_textureValid = TRUE;
}

DLL_EXPORT void APIENTRY glBindTexture( GLenum target, GLuint texture )
{
	g.m_currentTexture[g.m_textureStage] = texture;
	g.m_textureValid = FALSE;
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
		if (g.m_rStates[D3DRENDERSTATE_SRCBLEND] != srcBlend)
		{
			if (g.m_indexCount)
			{
				if (g.m_vertStart != g.m_vertCount)
				{
					g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
					g.m_vertStart = g.m_vertCount;
				}

				g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
				g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
				g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dummy);

				g.m_vertStart = 0;
				g.m_vertCount = 0;
				g.m_indexCount = 0;
			}

			g.m_rStates[D3DRENDERSTATE_SRCBLEND] = srcBlend;
			g.m_pD3DDev->lpVtbl->SetRenderState(g.m_pD3DDev, D3DRENDERSTATE_SRCBLEND, srcBlend);
		}
	}

	if (destBlend >= 0)
	{
		if (g.m_rStates[D3DRENDERSTATE_DESTBLEND] != destBlend)
		{
			if (g.m_indexCount)
			{
				if (g.m_vertStart != g.m_vertCount)
				{
					g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
					g.m_vertStart = g.m_vertCount;
				}

				g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
				g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
				g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dummy);

				g.m_vertStart = 0;
				g.m_vertCount = 0;
				g.m_indexCount = 0;
			}

			g.m_rStates[D3DRENDERSTATE_DESTBLEND] = destBlend;
			g.m_pD3DDev->lpVtbl->SetRenderState(g.m_pD3DDev, D3DRENDERSTATE_DESTBLEND, destBlend);
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

	if (g.m_indexCount)
	{
		if (g.m_vertStart != g.m_vertCount)
		{
			g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
			g.m_vertStart = g.m_vertCount;
		}

		g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
		g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
		g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dummy);

		g.m_vertStart = 0;
		g.m_vertCount = 0;
		g.m_indexCount = 0;
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
	screenRect.x2 = g.m_wndWidth;
	screenRect.y2 = g.m_wndHeight;

	g.m_pD3DVP3->lpVtbl->Clear2(g.m_pD3DVP3, 1, &screenRect, flags, g.m_clearColor, 1.0, 0);
}

DLL_EXPORT void APIENTRY glClearAccum( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha )
{
}

DLL_EXPORT void APIENTRY glClearColor( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha )
{
	static float scale = 255.0f;
	unsigned int r2, g2, b2, a;

	a = (unsigned int)(alpha * scale);
	if (a > 255)
		a = 255;
	b2 = (unsigned int)(blue * scale);
	if (b2 > 255)
		b2 = 255;
	g2 = (unsigned int)(green * scale);
	if (g2 > 255)
		g2 = 255;
	r2 = (unsigned int)(red * scale);
	if (r2 > 255)
		r2 = 255;
	g.m_clearColor = RGBA_MAKE(r2, g2, b2, a);
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
	unsigned int r2, g2, b2;

	b2 = (unsigned int)(blue * scale);
	if (b2 > 255)
		b2 = 255;
	g2 = (unsigned int)(green * scale);
	if (g2 > 255)
		g2 = 255;
	r2 = (unsigned int)(red * scale);
	if (r2 > 255)
		r2 = 255;
	g.m_color = RGBA_MAKE(r2, g2, b2, 255);
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
	g.m_color = RGBA_MAKE(red, green, blue, 255);
}

DLL_EXPORT void APIENTRY glColor3ubv( const GLubyte* v )
{
	g.m_color = RGBA_MAKE(v[0], v[1], v[2], 255);
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
	unsigned int r2, g2, b2, a;

	a = (unsigned int)(alpha * scale);
	if (a > 255)
		a = 255;
	b2 = (unsigned int)(blue * scale);
	if (b2 > 255)
		b2 = 255;
	g2 = (unsigned int)(green * scale);
	if (g2 > 255)
		g2 = 255;
	r2 = (unsigned int)(red * scale);
	if (r2 > 255)
		r2 = 255;
	g.m_color = RGBA_MAKE(r2, g2, b2, a);
}

DLL_EXPORT void APIENTRY glColor4fv( const GLfloat* v )
{
	static float scale = 255.0f;
	unsigned int r2, g2, b2, a;

	a = (unsigned int)(v[3] * scale);
	if (a > 255)
		a = 255;
	b2 = (unsigned int)(v[2] * scale);
	if (b2 > 255)
		b2 = 255;
	g2 = (unsigned int)(v[1] * scale);
	if (g2 > 255)
		g2 = 255;
	r2 = (unsigned int)(v[0] * scale);
	if (r2 > 255)
		r2 = 255;
	g.m_color = RGBA_MAKE(r2, g2, b2, a);
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
	g.m_color = RGBA_MAKE(red, green, blue, alpha);
}

DLL_EXPORT void APIENTRY glColor4ubv( const GLubyte* v )
{
	g.m_color = RGBA_MAKE(v[0], v[1], v[2], v[3]);
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
	g.m_colorPointer = pointer;

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

	g.m_cullFaceMode = mode;

	if (g.m_cullEnabled)
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

		if (g.m_rStates[D3DRENDERSTATE_CULLMODE] != cullMode)
		{
			if (g.m_indexCount)
			{
				if (g.m_vertStart != g.m_vertCount)
				{
					g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
					g.m_vertStart = g.m_vertCount;
				}

				g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
				g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
				g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dummy);

				g.m_vertStart = 0;
				g.m_vertCount = 0;
				g.m_indexCount = 0;
			}

			g.m_rStates[D3DRENDERSTATE_CULLMODE] = cullMode;
			g.m_pD3DDev->lpVtbl->SetRenderState(g.m_pD3DDev, D3DRENDERSTATE_CULLMODE, cullMode);
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
		tex = &g.m_textures[*textures];

		if (tex->m_pD3DTexture)
		{
			tex->m_pD3DTexture->lpVtbl->Release(tex->m_pD3DTexture);
			tex->m_pD3DTexture = NULL;
		}

		if (tex->m_pDDSurface)
		{
			tex->m_pDDSurface->lpVtbl->Release(tex->m_pDDSurface);
			tex->m_pDDSurface = NULL;
		}

		// Reset texture parameters
		tex->m_minFilter = D3DTFN_POINT;
		tex->m_magFilter = D3DTFG_LINEAR;
		tex->m_mipFilter = D3DTFP_LINEAR;
		tex->m_addressU = D3DTADDRESS_WRAP;
		tex->m_addressV = D3DTADDRESS_WRAP;
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
		if (g.m_rStates[D3DRENDERSTATE_ZFUNC] != zFunc)
		{
			if (g.m_indexCount)
			{
				if (g.m_vertStart != g.m_vertCount)
				{
					g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
					g.m_vertStart = g.m_vertCount;
				}

				g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
				g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
				g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dummy);

				g.m_vertStart = 0;
				g.m_vertCount = 0;
				g.m_indexCount = 0;
			}

			g.m_rStates[D3DRENDERSTATE_ZFUNC] = zFunc;
			g.m_pD3DDev->lpVtbl->SetRenderState(g.m_pD3DDev, D3DRENDERSTATE_ZFUNC, zFunc);
		}
	}
}

DLL_EXPORT void APIENTRY glDepthMask( GLboolean flag )
{
	DWORD	dummy;

	if (flag)
	{
		if (g.m_rStates[D3DRENDERSTATE_ZWRITEENABLE] != TRUE)
		{
			if (g.m_indexCount)
			{
				if (g.m_vertStart != g.m_vertCount)
				{
					g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
					g.m_vertStart = g.m_vertCount;
				}

				g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
				g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
				g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dummy);

				g.m_vertStart = 0;
				g.m_vertCount = 0;
				g.m_indexCount = 0;
			}

			g.m_rStates[D3DRENDERSTATE_ZWRITEENABLE] = TRUE;
			g.m_pD3DDev->lpVtbl->SetRenderState(g.m_pD3DDev, D3DRENDERSTATE_ZWRITEENABLE, TRUE);
		}
	}
	else
	{
		if (g.m_rStates[D3DRENDERSTATE_ZWRITEENABLE] != FALSE)
		{
			if (g.m_indexCount)
			{
				if (g.m_vertStart != g.m_vertCount)
				{
					g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
					g.m_vertStart = g.m_vertCount;
				}

				g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
				g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
				g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dummy);

				g.m_vertStart = 0;
				g.m_vertCount = 0;
				g.m_indexCount = 0;
			}

			g.m_rStates[D3DRENDERSTATE_ZWRITEENABLE] = FALSE;
			g.m_pD3DDev->lpVtbl->SetRenderState(g.m_pD3DDev, D3DRENDERSTATE_ZWRITEENABLE, FALSE);
		}
	}
}

DLL_EXPORT void APIENTRY glDepthRange( GLclampd zNear, GLclampd zFar )
{
	D3DVIEWPORT2	vport;
	D3DVALUE	sum, diff;
	DWORD	dummy;

	if (g.m_indexCount)
	{
		if (g.m_vertStart != g.m_vertCount)
		{
			g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
			g.m_vertStart = g.m_vertCount;
		}

		g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
		g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
		g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dummy);

		g.m_vertStart = 0;
		g.m_vertCount = 0;
		g.m_indexCount = 0;
	}

	vport.dwSize = sizeof(vport);
	g.m_pD3DVP3->lpVtbl->GetViewport2(g.m_pD3DVP3, &vport);

	// Set new depth range
	sum = (D3DVALUE)(zFar + zNear);
	diff = (D3DVALUE)(zFar - zNear);
	vport.dvMinZ = -sum / diff;
	vport.dvMaxZ = -(sum - 2.0f) / diff;

	g.m_pD3DVP3->lpVtbl->SetViewport2(g.m_pD3DVP3, &vport);
}

DLL_EXPORT void APIENTRY glDisable( GLenum cap )
{
	DWORD	dummy;

	if (cap == GL_CULL_FACE)
	{
		g.m_cullEnabled = FALSE;

		if (g.m_rStates[D3DRENDERSTATE_CULLMODE] != D3DCULL_NONE)
		{
			QuakeFlushIndexedPrimitives();
			g.m_rStates[D3DRENDERSTATE_CULLMODE] = D3DCULL_NONE;
			g.m_pD3DDev->lpVtbl->SetRenderState(g.m_pD3DDev, D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);
		}
	}
	else if (cap == GL_FOG)
	{
		if (g.m_rStates[D3DRENDERSTATE_FOGENABLE] != FALSE)
		{
			QuakeFlushIndexedPrimitives();
			g.m_rStates[D3DRENDERSTATE_FOGENABLE] = FALSE;
			g.m_pD3DDev->lpVtbl->SetRenderState(g.m_pD3DDev, D3DRENDERSTATE_FOGENABLE, FALSE);
		}

		if (g.m_rStates[D3DRENDERSTATE_RANGEFOGENABLE] != FALSE)
		{
			QuakeFlushIndexedPrimitives();
			g.m_rStates[D3DRENDERSTATE_RANGEFOGENABLE] = FALSE;
			g.m_pD3DDev->lpVtbl->SetRenderState(g.m_pD3DDev, D3DRENDERSTATE_RANGEFOGENABLE, FALSE);
		}
	}
	else if (cap == GL_DEPTH_TEST)
	{
		if (g.m_rStates[D3DRENDERSTATE_ZENABLE] != FALSE)
		{
			QuakeFlushIndexedPrimitives();
			g.m_rStates[D3DRENDERSTATE_ZENABLE] = FALSE;
			g.m_pD3DDev->lpVtbl->SetRenderState(g.m_pD3DDev, D3DRENDERSTATE_ZENABLE, FALSE);
		}
	}
	else if (cap == GL_ALPHA_TEST)
	{
		if (g.m_rStates[D3DRENDERSTATE_ALPHATESTENABLE] != FALSE)
		{
			QuakeFlushIndexedPrimitives();
			g.m_rStates[D3DRENDERSTATE_ALPHATESTENABLE] = FALSE;
			g.m_pD3DDev->lpVtbl->SetRenderState(g.m_pD3DDev, D3DRENDERSTATE_ALPHATESTENABLE, FALSE);
		}
	}
	else if (cap == GL_BLEND)
	{
		if (g.m_rStates[D3DRENDERSTATE_ALPHABLENDENABLE] != FALSE)
		{
			QuakeFlushIndexedPrimitives();
			g.m_rStates[D3DRENDERSTATE_ALPHABLENDENABLE] = FALSE;
			g.m_pD3DDev->lpVtbl->SetRenderState(g.m_pD3DDev, D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
		}
	}
	else if (cap == GL_SCISSOR_TEST)
	{
	}
	else if (cap == GL_TEXTURE_2D)
	{
		if (g.m_textureStage)
		{
			if (g.m_tsStates1[D3DTSS_COLOROP] != D3DTOP_DISABLE)
			{
				QuakeFlushIndexedPrimitives();
				g.m_tsStates1[D3DTSS_COLOROP] = D3DTOP_DISABLE;
				g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 1, D3DTSS_COLOROP, D3DTOP_DISABLE);
			}

			if (g.m_tsStates1[D3DTSS_ALPHAOP] != D3DTOP_DISABLE)
			{
				QuakeFlushIndexedPrimitives();
				g.m_tsStates1[D3DTSS_ALPHAOP] = D3DTOP_DISABLE;
				g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
			}

			g.m_useSubStage = FALSE;
		}
		else
		{
			if (g.m_tsStates0[D3DTSS_COLOROP] != D3DTOP_DISABLE)
			{
				if (g.m_indexCount)
				{
					QuakeFlushVertexBuffer();

					g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
					g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
					g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dummy);

					g.m_vertStart = 0;
					g.m_vertCount = 0;
					g.m_indexCount = 0;
				}

				g.m_tsStates0[D3DTSS_COLOROP] = D3DTOP_DISABLE;
				g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_COLOROP, D3DTOP_DISABLE);
			}

			if (g.m_tsStates0[D3DTSS_ALPHAOP] != D3DTOP_DISABLE)
			{
				QuakeFlushIndexedPrimitives();
				g.m_tsStates0[D3DTSS_ALPHAOP] = D3DTOP_DISABLE;
				g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
			}

			g.m_stage0Active = FALSE;
		}

		g.m_textureValid = FALSE;
	}
	else if (cap == GL_POLYGON_OFFSET_FILL)
	{
		D3DVIEWPORT2	vport;

		if (g.m_indexCount)
		{
			if (g.m_vertStart != g.m_vertCount)
			{
				g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
				g.m_vertStart = g.m_vertCount;
			}

			g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
			g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
			g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dummy);

			g.m_vertStart = 0;
			g.m_vertCount = 0;
			g.m_indexCount = 0;
		}

		vport.dwSize = sizeof(vport);
		g.m_pD3DVP3->lpVtbl->GetViewport2(g.m_pD3DVP3, &vport);

		vport.dvMaxZ = g.m_dvMaxZ;
		g.m_pD3DVP3->lpVtbl->SetViewport2(g.m_pD3DVP3, &vport);
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
		g.m_cullEnabled = TRUE;

		if (g.m_cullFaceMode == GL_BACK)
		{
			if (g.m_rStates[D3DRENDERSTATE_CULLMODE] != D3DCULL_CW)
			{
				if (g.m_indexCount)
				{
					QuakeFlushVertexBuffer();

					g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
					g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
					g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dummy);

					g.m_vertStart = 0;
					g.m_vertCount = 0;
					g.m_indexCount = 0;
				}

				g.m_rStates[D3DRENDERSTATE_CULLMODE] = D3DCULL_CW;
				g.m_pD3DDev->lpVtbl->SetRenderState(g.m_pD3DDev, D3DRENDERSTATE_CULLMODE, D3DCULL_CW);
			}
		}
		else
		{
			if (g.m_rStates[D3DRENDERSTATE_CULLMODE] != D3DCULL_CCW)
			{
				if (g.m_indexCount)
				{
					QuakeFlushVertexBuffer();

					g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
					g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
					g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dummy);

					g.m_vertStart = 0;
					g.m_vertCount = 0;
					g.m_indexCount = 0;
				}

				g.m_rStates[D3DRENDERSTATE_CULLMODE] = D3DCULL_CCW;
				g.m_pD3DDev->lpVtbl->SetRenderState(g.m_pD3DDev, D3DRENDERSTATE_CULLMODE, D3DCULL_CCW);
			}
		}
	}
	else if (cap == GL_FOG)
	{
		if (g.m_rStates[D3DRENDERSTATE_FOGENABLE] != TRUE)
		{
			QuakeFlushIndexedPrimitives();
			g.m_rStates[D3DRENDERSTATE_FOGENABLE] = TRUE;
			g.m_pD3DDev->lpVtbl->SetRenderState(g.m_pD3DDev, D3DRENDERSTATE_FOGENABLE, TRUE);
		}

		if (g.m_rStates[D3DRENDERSTATE_RANGEFOGENABLE] != TRUE)
		{
			if (g.m_indexCount)
			{
				QuakeFlushVertexBuffer();

				g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
				g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
				g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dummy);

				g.m_vertStart = 0;
				g.m_vertCount = 0;
				g.m_indexCount = 0;
			}

			g.m_rStates[D3DRENDERSTATE_RANGEFOGENABLE] = TRUE;
			g.m_pD3DDev->lpVtbl->SetRenderState(g.m_pD3DDev, D3DRENDERSTATE_RANGEFOGENABLE, TRUE);
		}
	}
	else if (cap == GL_DEPTH_TEST)
	{
		if (g.m_rStates[D3DRENDERSTATE_ZENABLE] != TRUE)
		{
			if (g.m_indexCount)
			{
				QuakeFlushVertexBuffer();

				g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
				g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
				g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dummy);

				g.m_vertStart = 0;
				g.m_vertCount = 0;
				g.m_indexCount = 0;
			}

			g.m_rStates[D3DRENDERSTATE_ZENABLE] = TRUE;
			g.m_pD3DDev->lpVtbl->SetRenderState(g.m_pD3DDev, D3DRENDERSTATE_ZENABLE, TRUE);
		}
	}
	else if (cap == GL_ALPHA_TEST)
	{
		if (g.m_rStates[D3DRENDERSTATE_ALPHATESTENABLE] != TRUE)
		{
			if (g.m_indexCount)
			{
				QuakeFlushVertexBuffer();

				g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
				g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
				g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dummy);

				g.m_vertStart = 0;
				g.m_vertCount = 0;
				g.m_indexCount = 0;
			}

			g.m_rStates[D3DRENDERSTATE_ALPHATESTENABLE] = TRUE;
			g.m_pD3DDev->lpVtbl->SetRenderState(g.m_pD3DDev, D3DRENDERSTATE_ALPHATESTENABLE, TRUE);
		}
	}
	else if (cap == GL_BLEND)
	{
		if (g.m_rStates[D3DRENDERSTATE_ALPHABLENDENABLE] != TRUE)
		{
			QuakeFlushIndexedPrimitives();
			g.m_rStates[D3DRENDERSTATE_ALPHABLENDENABLE] = TRUE;
			g.m_pD3DDev->lpVtbl->SetRenderState(g.m_pD3DDev, D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
		}
	}
	else if (cap == GL_SCISSOR_TEST)
	{
	}
	else if (cap == GL_TEXTURE_2D)
	{
		if (g.m_textureStage)
		{
			g.m_useSubStage = TRUE;
		}
		else
		{
			g.m_stage0Active = TRUE;
		}

		g.m_textureValid = FALSE;
	}
	else if (cap == GL_POLYGON_OFFSET_FILL)
	{
		D3DVIEWPORT2	vport;

		vport.dwSize = sizeof(vport);
		g.m_pD3DVP3->lpVtbl->GetViewport2(g.m_pD3DVP3, &vport);

		g.m_dvMaxZ = vport.dvMaxZ;
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

	switch (g.m_primMode)
	{
	case GL_TRIANGLES:
		for (i = 0; i < g.m_primVertCount; i++)
		{
			g.m_indexBuffer[g.m_indexCount++] = g.m_vertCount++;
		}
		break;
	case GL_TRIANGLE_STRIP:
		g.m_primVertCount -= 2;
		for (i = 0; i < g.m_primVertCount; i += 2)
		{
			g.m_indexBuffer[g.m_indexCount++] = g.m_vertCount + 0;
			g.m_indexBuffer[g.m_indexCount++] = g.m_vertCount + 1;
			g.m_indexBuffer[g.m_indexCount++] = g.m_vertCount + 2;
			g.m_vertCount++;

			if (i + 1 < g.m_primVertCount)
			{
				g.m_indexBuffer[g.m_indexCount++] = g.m_vertCount + 1;
				g.m_indexBuffer[g.m_indexCount++] = g.m_vertCount + 0;
				g.m_indexBuffer[g.m_indexCount++] = g.m_vertCount + 2;
				g.m_vertCount++;
			}
		}
		g.m_vertCount += 2;
		break;
	case GL_TRIANGLE_FAN:
	case GL_POLYGON:
		g.m_primVertCount -= 1;
		for (i = 1; i < g.m_primVertCount; i++)
		{
			g.m_indexBuffer[g.m_indexCount++] = g.m_vertCount;
			g.m_indexBuffer[g.m_indexCount++] = g.m_vertCount + i;
			g.m_indexBuffer[g.m_indexCount++] = g.m_vertCount + i + 1;
		}
		g.m_vertCount += (g.m_primVertCount + 1);
		break;
	case GL_QUADS:
		for (i = 0; i < g.m_primVertCount; i += 4)
		{
			g.m_indexBuffer[g.m_indexCount++] = g.m_vertCount + 0;
			g.m_indexBuffer[g.m_indexCount++] = g.m_vertCount + 1;
			g.m_indexBuffer[g.m_indexCount++] = g.m_vertCount + 2;
			g.m_indexBuffer[g.m_indexCount++] = g.m_vertCount + 0;
			g.m_indexBuffer[g.m_indexCount++] = g.m_vertCount + 2;
			g.m_indexBuffer[g.m_indexCount++] = g.m_vertCount + 3;
			g.m_vertCount += 4;
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
	DWORD	value;

	// ScriptedSnark: Original D3D6 wrapper had this mistake which was fixed
	// while reversing the func. Won't remove fix here, instead cover under
	// HLNT1_FIXES macro to let you know what was solved accidentally
#if defined ( HLNT1_FIXES )
	value = (DWORD)param;
#else
	value = *(DWORD*)&param;
#endif

	if (pname == GL_FOG_START)
	{
		if (g.m_rStates[D3DRENDERSTATE_FOGTABLESTART] != value)
		{
			if (g.m_indexCount)
			{
				if (g.m_vertStart != g.m_vertCount)
				{
					g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
					g.m_vertStart = g.m_vertCount;
				}

				g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
				g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
				g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dummy);

				g.m_vertStart = 0;
				g.m_vertCount = 0;
				g.m_indexCount = 0;
			}

			g.m_rStates[D3DRENDERSTATE_FOGTABLESTART] = value;
			g.m_pD3DDev->lpVtbl->SetRenderState(g.m_pD3DDev, D3DRENDERSTATE_FOGTABLESTART, value);
		}
	}
	else if (pname == GL_FOG_END)
	{
		if (g.m_rStates[D3DRENDERSTATE_FOGTABLEEND] != value)
		{
			if (g.m_indexCount)
			{
				if (g.m_vertStart != g.m_vertCount)
				{
					g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
					g.m_vertStart = g.m_vertCount;
				}

				g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
				g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
				g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dummy);

				g.m_vertStart = 0;
				g.m_vertCount = 0;
				g.m_indexCount = 0;
			}

			g.m_rStates[D3DRENDERSTATE_FOGTABLEEND] = value;
			g.m_pD3DDev->lpVtbl->SetRenderState(g.m_pD3DDev, D3DRENDERSTATE_FOGTABLEEND, value);
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

	if (g.m_rStates[D3DRENDERSTATE_FOGCOLOR] != fogColor)
	{
		if (g.m_indexCount)
		{
			if (g.m_vertStart != g.m_vertCount)
			{
				g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
				g.m_vertStart = g.m_vertCount;
			}

			g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
			g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
			g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dummy);

			g.m_vertStart = 0;
			g.m_vertCount = 0;
			g.m_indexCount = 0;
		}

		g.m_rStates[D3DRENDERSTATE_FOGCOLOR] = fogColor;
		g.m_pD3DDev->lpVtbl->SetRenderState(g.m_pD3DDev, D3DRENDERSTATE_FOGCOLOR, fogColor);
	}
}

DLL_EXPORT void APIENTRY glFogi( GLenum pname, GLint param )
{
	DWORD	dummy;

	if (pname == GL_FOG_MODE)
	{
		if (param == GL_EXP)
		{
			if (g.m_rStates[D3DRENDERSTATE_FOGTABLEMODE] != D3DFOG_EXP)
			{
				if (g.m_indexCount)
				{
					if (g.m_vertStart != g.m_vertCount)
					{
						g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
						g.m_vertStart = g.m_vertCount;
					}

					g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
					g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
					g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dummy);

					g.m_vertStart = 0;
					g.m_vertCount = 0;
					g.m_indexCount = 0;
				}

				g.m_rStates[D3DRENDERSTATE_FOGTABLEMODE] = D3DFOG_EXP;
				g.m_pD3DDev->lpVtbl->SetRenderState(g.m_pD3DDev, D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_EXP);
			}
		}
		else if (param == GL_EXP2)
		{
			if (g.m_rStates[D3DRENDERSTATE_FOGTABLEMODE] != D3DFOG_EXP2)
			{
				if (g.m_indexCount)
				{
					if (g.m_vertStart != g.m_vertCount)
					{
						g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
						g.m_vertStart = g.m_vertCount;
					}

					g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
					g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
					g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dummy);

					g.m_vertStart = 0;
					g.m_vertCount = 0;
					g.m_indexCount = 0;
				}

				g.m_rStates[D3DRENDERSTATE_FOGTABLEMODE] = D3DFOG_EXP2;
				g.m_pD3DDev->lpVtbl->SetRenderState(g.m_pD3DDev, D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_EXP2);
			}
		}
		else if (param == GL_LINEAR)
		{
			if (g.m_rStates[D3DRENDERSTATE_FOGTABLEMODE] != D3DFOG_LINEAR)
			{
				if (g.m_indexCount)
				{
					if (g.m_vertStart != g.m_vertCount)
					{
						g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
						g.m_vertStart = g.m_vertCount;
					}

					g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
					g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
					g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dummy);

					g.m_vertStart = 0;
					g.m_vertCount = 0;
					g.m_indexCount = 0;
				}

				g.m_rStates[D3DRENDERSTATE_FOGTABLEMODE] = D3DFOG_LINEAR;
				g.m_pD3DDev->lpVtbl->SetRenderState(g.m_pD3DDev, D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_LINEAR);
			}
		}
		else
		{
			if (g.m_rStates[D3DRENDERSTATE_FOGTABLEMODE] != D3DFOG_NONE)
			{
				if (g.m_indexCount)
				{
					QuakeFlushVertexBuffer();

					g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
					g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
					g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dummy);

					g.m_vertStart = 0;
					g.m_vertCount = 0;
					g.m_indexCount = 0;
				}

				g.m_rStates[D3DRENDERSTATE_FOGTABLEMODE] = D3DFOG_NONE;
				g.m_pD3DDev->lpVtbl->SetRenderState(g.m_pD3DDev, D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_NONE);
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

	if (g.m_vertStart != g.m_vertCount)
	{
		g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
		g.m_vertStart = g.m_vertCount;
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
	matrix._11 = twoNear / xRange;	matrix._12 = 0.0f;				matrix._13 = 0.0f;					matrix._14 = 0.0f;
	matrix._21 = 0.0f;				matrix._22 = twoNear / yRange;	matrix._23 = 0.0f;					matrix._24 = 0.0f;
	matrix._31 = xSum / xRange;		matrix._32 = ySum / yRange;		matrix._33 = -(sum / diff);			matrix._34 = -1.0f;
	matrix._41 = 0.0f;				matrix._42 = 0.0f;				matrix._43 = -(zFarTwoNear / diff);	matrix._44 = 0.0f;

	g.m_pD3DDev->lpVtbl->MultiplyTransform(g.m_pD3DDev, g.m_transformState, &matrix);
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
		g.m_pD3DDev->lpVtbl->GetTransform(g.m_pD3DDev, D3DTRANSFORMSTATE_WORLD, (LPD3DMATRIX)params);
		break;
	case GL_PROJECTION_MATRIX:
		g.m_pD3DDev->lpVtbl->GetTransform(g.m_pD3DDev, D3DTRANSFORMSTATE_PROJECTION, (LPD3DMATRIX)params);
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
		if (g.m_useMultitexture)
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

	if (g.m_vertStart != g.m_vertCount)
	{
		g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
		g.m_vertStart = g.m_vertCount;
	}

	// Create identity matrix
	matrix._11 = 1.0;	matrix._12 = 0.0;	matrix._13 = 0.0;	matrix._14 = 0.0;
	matrix._21 = 0.0;	matrix._22 = 1.0;	matrix._23 = 0.0;	matrix._24 = 0.0;
	matrix._31 = 0.0;	matrix._32 = 0.0;	matrix._33 = 1.0;	matrix._34 = 0.0;
	matrix._41 = 0.0;	matrix._42 = 0.0;	matrix._43 = 0.0;	matrix._44 = 1.0;

	g.m_pD3DDev->lpVtbl->SetTransform(g.m_pD3DDev, g.m_transformState, &matrix);
}

DLL_EXPORT void APIENTRY glLoadMatrixd( const GLdouble* m )
{
}

DLL_EXPORT void APIENTRY glLoadMatrixf( const GLfloat* m )
{
	if (g.m_vertStart != g.m_vertCount)
	{
		g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
		g.m_vertStart = g.m_vertCount;
	}

	g.m_pD3DDev->lpVtbl->SetTransform(g.m_pD3DDev, g.m_transformState, (LPD3DMATRIX)m);
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
		g.m_transformState = D3DTRANSFORMSTATE_WORLD;
	}
	else
	{
		g.m_transformState = D3DTRANSFORMSTATE_PROJECTION;
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

	if (g.m_vertStart != g.m_vertCount)
	{
		g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
		g.m_vertStart = g.m_vertCount;
	}

	sum = (D3DVALUE)(zFar + zNear);
	diff = (D3DVALUE)(zFar - zNear);

	xSum = (D3DVALUE)(right + left);
	ySum = (D3DVALUE)(top + bottom);
	xRange = (D3DVALUE)(right - left);
	yRange = (D3DVALUE)(top - bottom);

	// Create orthographic matrix
	matrix._11 = 2.0f / xRange;		matrix._12 = 0.0f;				matrix._13 = 0.0f;				matrix._14 = 0.0f;
	matrix._21 = 0.0f;				matrix._22 = 2.0f / yRange;		matrix._23 = 0.0f;				matrix._24 = 0.0f;
	matrix._31 = 0.0f;				matrix._32 = 0.0f;				matrix._33 = -(2.0f / diff);	matrix._34 = 0.0f;
	matrix._41 = -(xSum / xRange);	matrix._42 = -(ySum / yRange);	matrix._43 = -(sum / diff);		matrix._44 = 1.0f;

	g.m_pD3DDev->lpVtbl->MultiplyTransform(g.m_pD3DDev, g.m_transformState, &matrix);
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
		if (g.m_rStates[D3DRENDERSTATE_FILLMODE] != fillMode)
		{
			if (g.m_indexCount)
			{
				if (g.m_vertStart != g.m_vertCount)
				{
					g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
					g.m_vertStart = g.m_vertCount;
				}

				g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
				g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
				g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dummy);

				g.m_vertStart = 0;
				g.m_vertCount = 0;
				g.m_indexCount = 0;
			}

			g.m_rStates[D3DRENDERSTATE_FILLMODE] = fillMode;
			g.m_pD3DDev->lpVtbl->SetRenderState(g.m_pD3DDev, D3DRENDERSTATE_FILLMODE, fillMode);
		}
	}
}

DLL_EXPORT void APIENTRY glPolygonOffset( GLfloat factor, GLfloat units )
{
	D3DVIEWPORT2	vport;
	DWORD	dummy;

	if (g.m_indexCount)
	{
		if (g.m_vertStart != g.m_vertCount)
		{
			g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
			g.m_vertStart = g.m_vertCount;
		}

		g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
		g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
		g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dummy);

		g.m_vertStart = 0;
		g.m_vertCount = 0;
		g.m_indexCount = 0;
	}

	vport.dwSize = sizeof(vport);
	g.m_pD3DVP3->lpVtbl->GetViewport2(g.m_pD3DVP3, &vport);

	// Adjust depth range to add polygon offset
	vport.dvMaxZ = (units + 1.0f) * g.m_dvMaxZ;

	g.m_pD3DVP3->lpVtbl->SetViewport2(g.m_pD3DVP3, &vport);
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

	if (g.m_vertStart != g.m_vertCount)
	{
		g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
		g.m_vertStart = g.m_vertCount;
	}

	if (g.m_transformState == D3DTRANSFORMSTATE_WORLD)
	{
		g.m_pD3DDev->lpVtbl->SetTransform(g.m_pD3DDev, D3DTRANSFORMSTATE_WORLD, &g.m_worldMatrixStack->m_matrix);

		pTop = g.m_worldMatrixStack;
		g.m_worldMatrixStack = pTop->m_pNext;
		delete pTop;
		g.m_totalWorldMatrices--;
	}
	else
	{
		g.m_pD3DDev->lpVtbl->SetTransform(g.m_pD3DDev, g.m_transformState, &g.m_customMatrixStack->m_matrix);

		pTop = g.m_customMatrixStack;
		g.m_customMatrixStack = g.m_customMatrixStack->m_pNext;
		delete pTop;
		g.m_totalCustomMatrices--;
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

	g.m_pD3DDev->lpVtbl->GetTransform(g.m_pD3DDev, g.m_transformState, &matrix);

	if (g.m_transformState == D3DTRANSFORMSTATE_WORLD)
	{
		pCurrent = new D3D_MATRIXCHAIN;
		if (pCurrent)
		{
			pNext = g.m_worldMatrixStack;
			pCurrent->m_matrix = matrix;
			pCurrent->m_pNext = pNext;
			g.m_worldMatrixStack = pCurrent;
		}

		g.m_totalWorldMatrices++;
	}
	else
	{
		pCurrent = new D3D_MATRIXCHAIN;
		if (pCurrent)
		{
			pNext = g.m_customMatrixStack;
			pCurrent->m_matrix = matrix;
			pCurrent->m_pNext = pNext;
			g.m_customMatrixStack = pCurrent;
		}

		g.m_totalCustomMatrices++;
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

	g.m_pDXMgr->lpVtbl->GetPrimary(g.m_pDXMgr, &lpDDS);

	memset(&ddsd2, 0, sizeof(DDSURFACEDESC2));
	ddsd2.dwSize = sizeof(ddsd2);

	if (g.m_fullscreen)
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
		ClientToScreen(g.m_hWnd, &point_lt);

		point_rb.x = g.m_wndWidth;
		point_rb.y = g.m_wndHeight;
		ClientToScreen(g.m_hWnd, &point_rb);

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
		dest = (BYTE*)pixels + g.m_wndWidth * (g.m_wndHeight - 1) * 3;
		for (j = 0; j < g.m_wndHeight; j++)
		{
			for (i = 0; i < g.m_wndWidth; i++)
			{
				*dest++ = ((WORD)(*src & ddsd2.ddpfPixelFormat.dwRBitMask) >> rRightShift) << rLeftShift;
				*dest++ = ((WORD)(*src & ddsd2.ddpfPixelFormat.dwGBitMask) >> gRightShift) << gLeftShift;
				*dest++ = ((WORD)(*src & ddsd2.ddpfPixelFormat.dwBBitMask) >> bRightShift) << bLeftShift;
				src++;
			}
			src = (WORD*)((BYTE*)src + ddsd2.lPitch);
			dest -= g.m_wndWidth * 6;
		}
	}
	else if (ddsd2.ddpfPixelFormat.dwRGBBitCount == 24)
	{
		DWORD* src;
		BYTE* dest;

		src = (DWORD*)ddsd2.lpSurface;
		dest = (BYTE*)pixels + g.m_wndWidth * (g.m_wndHeight - 1) * 3;
		for (j = 0; j < g.m_wndHeight; j++)
		{
			for (i = 0; i < g.m_wndWidth; i++)
			{
				*dest++ = ((*src & ddsd2.ddpfPixelFormat.dwRBitMask) >> rRightShift) << rLeftShift;
				*dest++ = ((*src & ddsd2.ddpfPixelFormat.dwGBitMask) >> gRightShift) << gLeftShift;
				*dest++ = ((*src & ddsd2.ddpfPixelFormat.dwBBitMask) >> bRightShift) << bLeftShift;
				src = (DWORD*)((BYTE*)src + 3);
			}
			src = (DWORD*)((BYTE*)src + ddsd2.lPitch);
			dest -= g.m_wndWidth * 6;
		}
	}
	else if (ddsd2.ddpfPixelFormat.dwRGBBitCount == 32)
	{
		DWORD* src;
		BYTE* dest;

		src = (DWORD*)ddsd2.lpSurface;
		dest = (BYTE*)pixels + g.m_wndWidth * (g.m_wndHeight - 1) * 3;
		for (j = 0; j < g.m_wndHeight; j++)
		{
			for (i = 0; i < g.m_wndWidth; i++)
			{
				*dest++ = ((*src & ddsd2.ddpfPixelFormat.dwRBitMask) >> rRightShift) << rLeftShift;
				*dest++ = ((*src & ddsd2.ddpfPixelFormat.dwGBitMask) >> gRightShift) << gLeftShift;
				*dest++ = ((*src & ddsd2.ddpfPixelFormat.dwBBitMask) >> bRightShift) << bLeftShift;
				src++;
			}
			src = (DWORD*)((BYTE*)src + ddsd2.lPitch);
			dest -= g.m_wndWidth * 6;
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

	if (g.m_vertStart != g.m_vertCount)
	{
		g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
		g.m_vertStart = g.m_vertCount;
	}

	if (!(g.m_angleConstFlag & 1))
	{
		g.m_angleConstFlag |= 1;
		g.m_angleDegToRad = atan(1.0) * 4.0;
	}

	rad = angle * g.m_angleDegToRad / 180.0;
	s = sin(rad);
	c = cos(rad);

	if (x == 1.0f)
	{
		// Create X rotation matrix
		matrix._11 = 1.0f;	matrix._12 = 0.0f;	matrix._13 = 0.0f;	matrix._14 = 0.0f;
		matrix._21 = 0.0f;	matrix._22 = c;		matrix._23 = s;		matrix._24 = 0.0f;
		matrix._31 = 0.0f;	matrix._32 = -s;	matrix._33 = c;		matrix._34 = 0.0f;
		matrix._41 = 0.0f;	matrix._42 = 0.0f;	matrix._43 = 0.0f;	matrix._44 = 1.0f;
	}
	else if (y == 1.0f)
	{
		// Create Y rotation matrix
		matrix._11 = c;		matrix._12 = 0.0f;	matrix._13 = -s;	matrix._14 = 0.0f;
		matrix._21 = 0.0f;	matrix._22 = 1.0f;	matrix._23 = 0.0f;	matrix._24 = 0.0f;
		matrix._31 = s;		matrix._32 = 0.0f;	matrix._33 = c;		matrix._34 = 0.0f;
		matrix._41 = 0.0f;	matrix._42 = 0.0f;	matrix._43 = 0.0f;	matrix._44 = 1.0f;
	}
	else if (z == 1.0f)
	{
		// Create Z rotation matrix
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
	g.m_pD3DDev->lpVtbl->MultiplyTransform(g.m_pD3DDev, g.m_transformState, &matrix);
}

DLL_EXPORT void APIENTRY glScaled( GLdouble x, GLdouble y, GLdouble z )
{
}

DLL_EXPORT void APIENTRY glScalef( GLfloat x, GLfloat y, GLfloat z )
{
	D3DMATRIX	matrix;

	if (g.m_vertStart != g.m_vertCount)
	{
		g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
		g.m_vertStart = g.m_vertCount;
	}

	// Create scale matrix
	matrix._11 = x;		matrix._12 = 0.0f;	matrix._13 = 0.0f;	matrix._14 = 0.0f;
	matrix._21 = 0.0f;	matrix._22 = y;		matrix._23 = 0.0f;	matrix._24 = 0.0f;
	matrix._31 = 0.0f;	matrix._32 = 0.0f;	matrix._33 = z;		matrix._34 = 0.0f;
	matrix._41 = 0.0f;	matrix._42 = 0.0f;	matrix._43 = 0.0f;	matrix._44 = 1.0f;

	g.m_pD3DDev->lpVtbl->MultiplyTransform(g.m_pD3DDev, g.m_transformState, &matrix);
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
		if (g.m_rStates[D3DRENDERSTATE_SHADEMODE] != D3DSHADE_GOURAUD)
		{
			if (g.m_indexCount)
			{
				if (g.m_vertStart != g.m_vertCount)
				{
					g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
					g.m_vertStart = g.m_vertCount;
				}

				g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
				g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
				g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dummy);

				g.m_vertStart = 0;
				g.m_vertCount = 0;
				g.m_indexCount = 0;
			}

			g.m_rStates[D3DRENDERSTATE_SHADEMODE] = D3DSHADE_GOURAUD;
			g.m_pD3DDev->lpVtbl->SetRenderState(g.m_pD3DDev, D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
		}
	}
	else
	{
		if (g.m_rStates[D3DRENDERSTATE_SHADEMODE] != D3DSHADE_FLAT)
		{
			if (g.m_indexCount)
			{
				if (g.m_vertStart != g.m_vertCount)
				{
					g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
					g.m_vertStart = g.m_vertCount;
				}

				g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
				g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
				g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dummy);

				g.m_vertStart = 0;
				g.m_vertCount = 0;
				g.m_indexCount = 0;
			}

			g.m_rStates[D3DRENDERSTATE_SHADEMODE] = D3DSHADE_FLAT;
			g.m_pD3DDev->lpVtbl->SetRenderState(g.m_pD3DDev, D3DRENDERSTATE_SHADEMODE, D3DSHADE_FLAT);
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
	g.m_tu = s;
	g.m_tv = t;
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
		g.m_texEnvMode[g.m_textureStage] = (int)param;
		g.m_textureValid = FALSE;
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
	LPDIRECTDRAWSURFACE4 m_pDDS4, lpDDS42;
	D3D_TEXTURE* tex;

	tex = &g.m_textures[g.m_currentTexture[g.m_textureStage]];

	if (g.m_useSubsample)
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

	if (g.m_squareTexturesOnly)
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
				ddsd.ddpfPixelFormat = g.m_ddEightBitSurfFormat;
				break;
			case 3:
				ddsd.ddpfPixelFormat = g.m_ddFiveBitSurfFormat;
				break;
			case 4:
				if (g.m_load4444)
				{
					ddsd.ddpfPixelFormat = g.m_ddFourBitAlphaSurfFormat;
					g.m_load4444 = FALSE;
				}
				else
				{
					ddsd.ddpfPixelFormat = g.m_ddOneBitAlphaSurfFormat;
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
		ddsd.dwTextureStage = g.m_textureStage;
		ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
		ddsd.ddsCaps.dwCaps2 = DDSCAPS2_HINTDYNAMIC | DDSCAPS2_TEXTUREMANAGE;

		if (g.m_pDD4->lpVtbl->CreateSurface(g.m_pDD4, &ddsd, &m_pDDS4, NULL) != DD_OK)
		{
			OutputDebugString("Wrapper: CreateSurface for texture failed\n");
			return;
		}

		LoadSurface(m_pDDS4, internalformat, width, height, newWidth, newHeight, (BYTE*)pixels);

		if (tex->m_pD3DTexture)
			tex->m_pD3DTexture->lpVtbl->Release(tex->m_pD3DTexture);
		if (tex->m_pDDSurface)
			tex->m_pDDSurface->lpVtbl->Release(tex->m_pDDSurface);

		if (m_pDDS4->lpVtbl->QueryInterface(m_pDDS4, IID_IDirect3DTexture2, (LPVOID*)&tex->m_pD3DTexture) != DD_OK)
		{
			OutputDebugString("Wrapper: QueryInterface for Texture2 failed.\n");
			m_pDDS4->lpVtbl->Release(m_pDDS4);
			return;
		}

		tex->m_internalFormat = internalformat;
		tex->m_width = newWidth;
		tex->m_height = newHeight;
		tex->m_oldWidth = width;
		tex->m_oldHeight = height;
		tex->m_pDDSurface = m_pDDS4;
	}
	else if (level == 1 && g.m_useMipmap)
	{
		memset(&ddsd, 0, sizeof(ddsd));

		switch (internalformat)
		{
			case 1:
				ddsd.ddpfPixelFormat = g.m_ddEightBitSurfFormat;
				break;
			case 3:
				ddsd.ddpfPixelFormat = g.m_ddFiveBitSurfFormat;
				break;
			case 4:
				if (g.m_load4444)
				{
					ddsd.ddpfPixelFormat = g.m_ddFourBitAlphaSurfFormat;
					g.m_load4444 = FALSE;
				}
				else
				{
					ddsd.ddpfPixelFormat = g.m_ddOneBitAlphaSurfFormat;
				}
				break;
		}

		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_TEXTURESTAGE;
		ddsd.dwHeight = tex->m_height;
		ddsd.dwWidth = tex->m_width;
		ddsd.dwTextureStage = g.m_textureStage;
		ddsd.ddsCaps.dwCaps = DDSCAPS_COMPLEX | DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;
		ddsd.ddsCaps.dwCaps2 = DDSCAPS2_HINTDYNAMIC | DDSCAPS2_TEXTUREMANAGE;

		if (g.m_pDD4->lpVtbl->CreateSurface(g.m_pDD4, &ddsd, &m_pDDS4, NULL) != DD_OK)
		{
			OutputDebugString("Wrapper: CreateSurface for texture failed\n");
			return;
		}

		m_pDDS4->lpVtbl->Blt(m_pDDS4, NULL, tex->m_pDDSurface, NULL, DDBLT_WAIT, NULL);

		tex->m_pD3DTexture->lpVtbl->Release(tex->m_pD3DTexture);
		tex->m_pDDSurface->lpVtbl->Release(tex->m_pDDSurface);

		tex->m_pDDSurface = m_pDDS4;
		m_pDDS4->lpVtbl->QueryInterface(m_pDDS4, IID_IDirect3DTexture2, (LPVOID*)&tex->m_pD3DTexture);

		memset(&ddsd.ddsCaps, 0, sizeof(ddsd.ddsCaps));
		ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;

		tex->m_pDDSurface->lpVtbl->GetAttachedSurface(tex->m_pDDSurface, &ddsd.ddsCaps, &lpDDS42);

		LoadSurface(lpDDS42, internalformat, width, height, newWidth, newHeight, (BYTE*)pixels);

		lpDDS42->lpVtbl->Release(lpDDS42);
	}
	else if (g.m_useMipmap)
	{
		tex->m_pDDSurface->lpVtbl->AddRef(tex->m_pDDSurface);

		m_pDDS4 = tex->m_pDDSurface;
		for (i = 0; i < level; i++)
		{
			HRESULT hResult;

			memset(&ddsd.ddsCaps, 0, sizeof(ddsd.ddsCaps));
			ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;

			hResult = m_pDDS4->lpVtbl->GetAttachedSurface(m_pDDS4, &ddsd.ddsCaps, &lpDDS42);
			m_pDDS4->lpVtbl->Release(m_pDDS4);
			if (hResult == DDERR_NOTFOUND)
				return;

			m_pDDS4 = lpDDS42;
		}

		LoadSurface(m_pDDS4, internalformat, width, height, newWidth, newHeight, (BYTE*)pixels);
		m_pDDS4->lpVtbl->Release(m_pDDS4);
	}

	g.m_textureValid = FALSE;
}

DLL_EXPORT void APIENTRY glTexParameterf( GLenum target, GLenum pname, GLfloat param )
{
	switch (pname)
	{
	case GL_TEXTURE_MAG_FILTER:
		if (param == GL_NEAREST)
		{
			g.m_textures[g.m_currentTexture[g.m_textureStage]].m_magFilter = D3DTFG_POINT;
		}
		else
		{
			g.m_textures[g.m_currentTexture[g.m_textureStage]].m_magFilter = D3DTFG_LINEAR;
		}
		break;
	case GL_TEXTURE_MIN_FILTER:
		switch ((DWORD)param)
		{
		case GL_NEAREST:
			g.m_textures[g.m_currentTexture[g.m_textureStage]].m_minFilter = D3DTFN_POINT;
			g.m_textures[g.m_currentTexture[g.m_textureStage]].m_mipFilter = D3DTFP_NONE;
			break;
		case GL_LINEAR:
			g.m_textures[g.m_currentTexture[g.m_textureStage]].m_minFilter = D3DTFN_LINEAR;
			g.m_textures[g.m_currentTexture[g.m_textureStage]].m_mipFilter = D3DTFP_NONE;
			break;
		case GL_NEAREST_MIPMAP_NEAREST:
			g.m_textures[g.m_currentTexture[g.m_textureStage]].m_minFilter = D3DTFN_POINT;
			g.m_textures[g.m_currentTexture[g.m_textureStage]].m_mipFilter = D3DTFP_POINT;
			break;
		case GL_LINEAR_MIPMAP_NEAREST:
			g.m_textures[g.m_currentTexture[g.m_textureStage]].m_minFilter = D3DTFN_LINEAR;
			g.m_textures[g.m_currentTexture[g.m_textureStage]].m_mipFilter = D3DTFP_POINT;
			break;
		case GL_NEAREST_MIPMAP_LINEAR:
			g.m_textures[g.m_currentTexture[g.m_textureStage]].m_minFilter = D3DTFN_POINT;
			g.m_textures[g.m_currentTexture[g.m_textureStage]].m_mipFilter = D3DTFP_LINEAR;
			break;
		case GL_LINEAR_MIPMAP_LINEAR:
			g.m_textures[g.m_currentTexture[g.m_textureStage]].m_minFilter = D3DTFN_LINEAR;
			g.m_textures[g.m_currentTexture[g.m_textureStage]].m_mipFilter = D3DTFP_LINEAR;
			break;
		}
		break;
	case GL_TEXTURE_WRAP_S:
		if (param == GL_CLAMP)
		{
			g.m_textures[g.m_currentTexture[g.m_textureStage]].m_addressU = D3DTADDRESS_CLAMP;
		}
		else
		{
			g.m_textures[g.m_currentTexture[g.m_textureStage]].m_addressU = D3DTADDRESS_WRAP;
		}
		break;
	case GL_TEXTURE_WRAP_T:
		if (param == GL_CLAMP)
		{
			g.m_textures[g.m_currentTexture[g.m_textureStage]].m_addressV = D3DTADDRESS_CLAMP;
		}
		else
		{
			g.m_textures[g.m_currentTexture[g.m_textureStage]].m_addressV = D3DTADDRESS_WRAP;
		}
		break;
	}

	g.m_textureValid = FALSE;
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
	LPDIRECTDRAWSURFACE4 m_pDDS4, lpDDS42;

	tex = &g.m_textures[g.m_currentTexture[g.m_textureStage]];

	m_pDDS4 = tex->m_pDDSurface;
	for (i = 0; i < level; i++)
	{
		HRESULT hResult;

		memset(&ddsc2, 0, sizeof(ddsc2));
		ddsc2.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;

		hResult = m_pDDS4->lpVtbl->GetAttachedSurface(m_pDDS4, &ddsc2, &lpDDS42);
		if (hResult == DDERR_NOTFOUND)
			return;

		m_pDDS4 = lpDDS42;
		m_pDDS4->lpVtbl->Release(m_pDDS4);
	}

	xLeft = tex->m_width * xoffset / tex->m_oldWidth;
	yTop = tex->m_height * yoffset / tex->m_oldHeight;
	xRight = xLeft + tex->m_width * width / tex->m_oldWidth;
	yBottom = yTop + tex->m_height * height / tex->m_oldHeight;
	SetRect(&rc, xLeft, yTop, xRight, yBottom);

	if (LoadSubSurface(m_pDDS4, tex->m_internalFormat, width, height, (BYTE*)pixels, &rc) != DD_OK)
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

	if (g.m_vertStart != g.m_vertCount)
	{
		g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
		g.m_vertStart = g.m_vertCount;
	}

	// Create translation matrix
	matrix._11 = 1.0f;	matrix._12 = 0.0f;	matrix._13 = 0.0f;	matrix._14 = 0.0f;
	matrix._21 = 0.0f;	matrix._22 = 1.0f;	matrix._23 = 0.0f;	matrix._24 = 0.0f;
	matrix._31 = 0.0f;	matrix._32 = 0.0f;	matrix._33 = 1.0f;	matrix._34 = 0.0f;
	matrix._41 = x;		matrix._42 = y;		matrix._43 = z;		matrix._44 = 1.0f;

	g.m_pD3DDev->lpVtbl->MultiplyTransform(g.m_pD3DDev, g.m_transformState, &matrix);
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

	vert = &g.m_verts[g.m_vertCount + g.m_primVertCount];
	vert->m_x = x;
	vert->m_y = y;
	vert->m_z = 0.0f;
	vert->m_color = g.m_color;
	vert->m_tu = g.m_tu;
	vert->m_tv = g.m_tv;

	if (g.m_useSubStage)
	{
		vert->m_tu2 = g.m_tu2;
		vert->m_tv2 = g.m_tv2;
	}

	g.m_primVertCount++;
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

	vert = &g.m_verts[g.m_vertCount + g.m_primVertCount];
	vert->m_x = x;
	vert->m_y = y;
	vert->m_z = z;
	vert->m_color = g.m_color;
	vert->m_tu = g.m_tu;
	vert->m_tv = g.m_tv;

	if (g.m_useSubStage)
	{
		vert->m_tu2 = g.m_tu2;
		vert->m_tv2 = g.m_tv2;
	}

	g.m_primVertCount++;
}

DLL_EXPORT void APIENTRY glVertex3fv( const GLfloat* v )
{
	D3D_VERTEX* vert;

	vert = &g.m_verts[g.m_vertCount + g.m_primVertCount];
	vert->m_x = v[0];
	vert->m_y = v[1];
	vert->m_z = v[2];
	vert->m_color = g.m_color;
	vert->m_tu = g.m_tu;
	vert->m_tv = g.m_tv;

	if (g.m_useSubStage)
	{
		vert->m_tu2 = g.m_tu2;
		vert->m_tv2 = g.m_tv2;
	}

	g.m_primVertCount++;
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
	g.m_vertexPointer = pointer;

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

	if (g.m_indexCount)
	{
		if (g.m_vertStart != g.m_vertCount)
		{
			g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
			g.m_vertStart = g.m_vertCount;
		}

		g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
		g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
		g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dummy);

		g.m_vertStart = 0;
		g.m_vertCount = 0;
		g.m_indexCount = 0;
	}

	// Set viewport
	vport.dwSize = sizeof(vport);
	g.m_pD3DVP3->lpVtbl->GetViewport2(g.m_pD3DVP3, &vport);
	vport.dwX = x;
	vport.dwY = g.m_wndHeight - height - y;
	vport.dwWidth = width;
	vport.dwHeight = height;
	vport.dvClipX = -1.0;
	vport.dvClipY = 1.0;
	vport.dvClipWidth = 2.0;
	vport.dvClipHeight = 2.0;

	g.m_pD3DVP3->lpVtbl->SetViewport2(g.m_pD3DVP3, &vport);
}

DLL_EXPORT void APIENTRY glSelectTextureSGIS( GLenum target )
{
	if (target == TEXTURE0_SGIS)
	{
		g.m_textureStage = 0;
	}
	else
	{
		g.m_textureStage = 1;
	}
}

DLL_EXPORT void APIENTRY glMTexCoord2fSGIS( GLenum target, GLfloat s, GLfloat t )
{
	if (target == TEXTURE0_SGIS)
	{
		g.m_tu = s;
		g.m_tv = t;
	}
	else
	{
		g.m_tu2 = s;
		g.m_tv2 = t;
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

	g.m_hDC = hdc;
	g.m_hWnd = WindowFromDC(hdc);

	GetClientRect(g.m_hWnd, &rect);
	g.m_wndWidth = (USHORT)rect.right;
	g.m_wndHeight = (USHORT)rect.bottom;

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
	if (FAILED(DXMgrCreate(IID_IDXMgr, (LPVOID*)&g.m_pDXMgr)))
	{
		CoUninitialize();
		return NULL;
	}

	// Initialize DX Manager
	if (FAILED(g.m_pDXMgr->lpVtbl->Initialize2(g.m_pDXMgr, g.m_pDD4, g.m_hWnd, 0)))
	{
		g.m_pDXMgr->lpVtbl->Release(g.m_pDXMgr);
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

	if (g.m_fullscreen)
	{
		enable.uFlags |= DXMGRENABLE_FULLSCREEN;

		// Create display mode enumerator
		IEnumDisplayModes* pEnumDisplayModes;
		if (FAILED(g.m_pDXMgr->lpVtbl->CreateEnumDisplayModes(g.m_pDXMgr, &pEnumDisplayModes, 0)))
		{
			g.m_pDXMgr->lpVtbl->Release(g.m_pDXMgr);
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
				g.m_pDXMgr->lpVtbl->Release(g.m_pDXMgr);
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
		enable.hwndDevice = g.m_hWnd;
	}

	enable.fNear = 1.0f;
	enable.fFar = 10.0f;

	// Enable DXMgr
	if (FAILED(g.m_pDXMgr->lpVtbl->Enable(g.m_pDXMgr, &enable)))
	{
		g.m_pDXMgr->lpVtbl->Release(g.m_pDXMgr);
		CoUninitialize();
		return NULL;
	}

	// Get D3D device
	if (FAILED(g.m_pDXMgr->lpVtbl->GetDirect3DDevice(g.m_pDXMgr, &g.m_pD3DDev)))
	{
		g.m_pDXMgr->lpVtbl->Release(g.m_pDXMgr);
		CoUninitialize();
		return NULL;
	}

	// Get device capabilities
	D3DDEVICEDESC heldevdesc;
	D3DDEVICEDESC hwdevdesc;
	heldevdesc.dwSize = sizeof(heldevdesc);
	hwdevdesc.dwSize = sizeof(hwdevdesc);
	g.m_pD3DDev->lpVtbl->GetCaps(g.m_pD3DDev, &hwdevdesc, &heldevdesc);
	
	if (hwdevdesc.dwFlags)
	{
		g.m_devdesc = hwdevdesc;
	}
	else
	{
		g.m_devdesc = heldevdesc;
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
			g.m_useMipmap = FALSE;
			OutputDebugString("Wrapper: Mipmapping disabled\n");
		}
		else
		{
			g.m_useMipmap = TRUE;
		}

		RegCloseKey(hKey);
	}
	else
	{
		g.m_useMipmap = TRUE;
	}

	// Create texture format enumerator
	IEnumPixelFormats* pEnumPixelFormats;
	if (FAILED(g.m_pDXMgr->lpVtbl->CreateEnumTextureFormats(g.m_pDXMgr, &pEnumPixelFormats, 0)))
	{
		g.m_pD3DDev->lpVtbl->Release(g.m_pD3DDev);
		g.m_pDXMgr->lpVtbl->Release(g.m_pDXMgr);
		CoUninitialize();
		return NULL;
	}

	// Find 16-bit 5551 pixel format
	BOOL f5551 = FALSE;
	while (1)
	{
		// Get next pixel format
		if (FAILED(pEnumPixelFormats->lpVtbl->Next(pEnumPixelFormats, 1, &g.m_ddOneBitAlphaSurfFormat, (UINT*)&dwSize)))
		{
			pEnumPixelFormats->lpVtbl->Release(pEnumPixelFormats);
			g.m_pD3DDev->lpVtbl->Release(g.m_pD3DDev);
			g.m_pDXMgr->lpVtbl->Release(g.m_pDXMgr);
			CoUninitialize();
			return NULL;
		}

		// No more formats
		if (dwSize == 0)
			break;

		// Check for 5551 format
		if ((g.m_ddOneBitAlphaSurfFormat.dwFlags & DDPF_RGB) &&
			(g.m_ddOneBitAlphaSurfFormat.dwRGBBitCount == 16) &&
			(g.m_ddOneBitAlphaSurfFormat.dwFlags & DDPF_ALPHAPIXELS) &&
			(g.m_ddOneBitAlphaSurfFormat.dwRGBAlphaBitMask == 0x8000) &&
			(g.m_ddOneBitAlphaSurfFormat.dwRBitMask == 0x7C00) &&
			(g.m_ddOneBitAlphaSurfFormat.dwGBitMask == 0x03E0) &&
			(g.m_ddOneBitAlphaSurfFormat.dwBBitMask == 0x001F))
		{
			f5551 = TRUE;
			break;
		}
	}

	if (!f5551)
	{
		OutputDebugString("Wrapper: Unable to find 5551 texture.\n");
		pEnumPixelFormats->lpVtbl->Release(pEnumPixelFormats);
		g.m_pD3DDev->lpVtbl->Release(g.m_pD3DDev);
		g.m_pDXMgr->lpVtbl->Release(g.m_pDXMgr);
		CoUninitialize();
		return NULL;
	}

	// Reset enumerator
	if (FAILED(pEnumPixelFormats->lpVtbl->Reset(pEnumPixelFormats)))
	{
		pEnumPixelFormats->lpVtbl->Release(pEnumPixelFormats);
		g.m_pD3DDev->lpVtbl->Release(g.m_pD3DDev);
		g.m_pDXMgr->lpVtbl->Release(g.m_pDXMgr);
		CoUninitialize();
		return NULL;
	}

	// Find 16-bit 4444 pixel format
	BOOL f4444 = FALSE;
	while (1)
	{
		// Get next pixel format
		if (FAILED(pEnumPixelFormats->lpVtbl->Next(pEnumPixelFormats, 1, &g.m_ddFourBitAlphaSurfFormat, (UINT*)&dwSize)))
		{
			pEnumPixelFormats->lpVtbl->Release(pEnumPixelFormats);
			g.m_pD3DDev->lpVtbl->Release(g.m_pD3DDev);
			g.m_pDXMgr->lpVtbl->Release(g.m_pDXMgr);
			CoUninitialize();
			return NULL;
		}

		// No more formats
		if (dwSize == 0)
			break;

		// Check for 4444 format
		if ((g.m_ddFourBitAlphaSurfFormat.dwFlags & DDPF_RGB) &&
			(g.m_ddFourBitAlphaSurfFormat.dwRGBBitCount == 16) &&
			(g.m_ddFourBitAlphaSurfFormat.dwFlags & DDPF_ALPHAPIXELS) &&
			(g.m_ddFourBitAlphaSurfFormat.dwRGBAlphaBitMask == 0xF000) &&
			(g.m_ddFourBitAlphaSurfFormat.dwRBitMask == 0x0F00) &&
			(g.m_ddFourBitAlphaSurfFormat.dwGBitMask == 0x00F0) &&
			(g.m_ddFourBitAlphaSurfFormat.dwBBitMask == 0x000F))
		{
			f4444 = TRUE;
			break;
		}
	}

	if (!f4444)
	{
		OutputDebugString("Wrapper: Not using 4444 texture.\n");
		g.m_ddFourBitAlphaSurfFormat = g.m_ddOneBitAlphaSurfFormat;
	}

	// Reset enumerator
	if (FAILED(pEnumPixelFormats->lpVtbl->Reset(pEnumPixelFormats)))
	{
		pEnumPixelFormats->lpVtbl->Release(pEnumPixelFormats);
		g.m_pD3DDev->lpVtbl->Release(g.m_pD3DDev);
		g.m_pDXMgr->lpVtbl->Release(g.m_pDXMgr);
		CoUninitialize();
		return NULL;
	}

	// Find 16-bit 555/565 pixel format
	BOOL f555_565 = FALSE;
	while (1)
	{
		// Get next pixel format
		if (FAILED(pEnumPixelFormats->lpVtbl->Next(pEnumPixelFormats, 1, &g.m_ddFiveBitSurfFormat, (UINT*)&dwSize)))
		{
			pEnumPixelFormats->lpVtbl->Release(pEnumPixelFormats);
			g.m_pD3DDev->lpVtbl->Release(g.m_pD3DDev);
			g.m_pDXMgr->lpVtbl->Release(g.m_pDXMgr);
			CoUninitialize();
			return NULL;
		}

		// No more formats
		if (dwSize == 0)
			break;

		// Check for 555/565 format
		if ((g.m_ddFiveBitSurfFormat.dwFlags & DDPF_RGB) &&
			(g.m_ddFiveBitSurfFormat.dwRGBBitCount == 16) &&
			(g.m_ddFiveBitSurfFormat.dwFlags & DDPF_ALPHAPIXELS) == 0 &&
			(g.m_ddFiveBitSurfFormat.dwRBitMask == 0xF800 || g.m_ddFiveBitSurfFormat.dwRBitMask == 0x7C00) &&
			(g.m_ddFiveBitSurfFormat.dwGBitMask == 0x07E0 || g.m_ddFiveBitSurfFormat.dwGBitMask == 0x03E0) &&
			(g.m_ddFiveBitSurfFormat.dwBBitMask == 0x001F))
		{
			f555_565 = TRUE;
			break;
		}
	}

	if (!f555_565)
	{
		OutputDebugString("Wrapper: Unable to find 555 or 565 texture.\n");
		pEnumPixelFormats->lpVtbl->Release(pEnumPixelFormats);
		g.m_pD3DDev->lpVtbl->Release(g.m_pD3DDev);
		g.m_pDXMgr->lpVtbl->Release(g.m_pDXMgr);
		CoUninitialize();
		return NULL;
	}

	// Reset enumerator
	if (FAILED(pEnumPixelFormats->lpVtbl->Reset(pEnumPixelFormats)))
	{
		pEnumPixelFormats->lpVtbl->Release(pEnumPixelFormats);
		g.m_pD3DDev->lpVtbl->Release(g.m_pD3DDev);
		g.m_pDXMgr->lpVtbl->Release(g.m_pDXMgr);
		CoUninitialize();
		return NULL;
	}

	// Find 32-bit 8888 pixel format
	BOOL f8888 = FALSE;
	while (1)
	{
		// Get next pixel format
		if (FAILED(pEnumPixelFormats->lpVtbl->Next(pEnumPixelFormats, 1, &g.m_ddEightBitSurfFormat, (UINT*)&dwSize)))
		{
			pEnumPixelFormats->lpVtbl->Release(pEnumPixelFormats);
			g.m_pD3DDev->lpVtbl->Release(g.m_pD3DDev);
			g.m_pDXMgr->lpVtbl->Release(g.m_pDXMgr);
			CoUninitialize();
			return NULL;
		}

		// No more formats
		if (dwSize == 0)
			break;

		// Check for 8888 format
		if ((g.m_ddEightBitSurfFormat.dwFlags & DDPF_LUMINANCE) &&
			(g.m_ddEightBitSurfFormat.dwRGBBitCount == 8) &&
			(g.m_ddEightBitSurfFormat.dwRBitMask == 0xFF))
		{
			f8888 = TRUE;
			break;
		}
	}

	if (!f8888)
	{
		OutputDebugString("Wrapper: Not using luminance texture\n");
		g.m_ddEightBitSurfFormat = g.m_ddFiveBitSurfFormat;
	}

	// We're done with the enumerator
	pEnumPixelFormats->lpVtbl->Release(pEnumPixelFormats);

	if (g.m_devdesc.dwMaxTextureWidth < 512 || g.m_devdesc.dwMaxTextureHeight < 512)
	{
		g.m_useSubsample = TRUE;
		OutputDebugString("Wrapper: Subsampling textures to 256 x 256\n");
	}
	else
	{
		g.m_useSubsample = FALSE;
	}

	if (g.m_devdesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY)
	{
		g.m_squareTexturesOnly = TRUE;
		OutputDebugString("Wrapper: Forcing all textures to be square\n");
	}
	else
	{
		g.m_squareTexturesOnly = FALSE;
	}

	if (g.m_devdesc.wMaxSimultaneousTextures > 1)
	{
		g.m_useMultitexture = TRUE;
		OutputDebugString("Wrapper: Multitexturing enabled\n");
	}
	else
	{
		g.m_useMultitexture = FALSE;
		OutputDebugString("Wrapper: Multitexturing not available with this driver\n");
	}

	if (!(g.m_devdesc.dpcTriCaps.dwTextureFilterCaps & (D3DPTFILTERCAPS_MIPNEAREST | D3DPTFILTERCAPS_MIPLINEAR | D3DPTFILTERCAPS_LINEARMIPNEAREST | D3DPTFILTERCAPS_LINEARMIPLINEAR)))
	{
		g.m_useMipmap = FALSE;
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
			g.m_doFlip = TRUE;
		}
		else
		{
			g.m_doFlip = FALSE;
		}

		RegCloseKey(hKey);
	}
	else
	{
		g.m_doFlip = FALSE;
	}

	// Get D3D interface
	LPDIRECT3D3 lpD3D3;
	if (FAILED(g.m_pDXMgr->lpVtbl->GetDirect3D(g.m_pDXMgr, &lpD3D3)))
	{
		g.m_pD3DDev->lpVtbl->Release(g.m_pD3DDev);
		g.m_pDXMgr->lpVtbl->Release(g.m_pDXMgr);
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
	if (FAILED(lpD3D3->lpVtbl->CreateVertexBuffer(lpD3D3, &vbdesc, &g.m_pD3DVBSrc, 0L, NULL)))
	{
		g.m_pD3DDev->lpVtbl->Release(g.m_pD3DDev);
		g.m_pDXMgr->lpVtbl->Release(g.m_pDXMgr);
		CoUninitialize();
		return NULL;
	}

	DWORD dummy;
	g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dummy);

	if (g.m_useMultitexture)
	{
		vbdesc.dwFVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2;
	}
	else
	{
		vbdesc.dwFVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1;
	}

	// Create rendering vertex buffer
	if (FAILED(lpD3D3->lpVtbl->CreateVertexBuffer(lpD3D3, &vbdesc, &g.m_pD3DVB, 0L, NULL)))
	{
		g.m_pD3DDev->lpVtbl->Release(g.m_pD3DDev);
		g.m_pDXMgr->lpVtbl->Release(g.m_pDXMgr);
		CoUninitialize();
		return NULL;
	}

	lpD3D3->lpVtbl->Release(lpD3D3);

	g.m_cullFaceMode = GL_BACK;
	g.m_cullEnabled = FALSE;
	g.m_textureValid = FALSE;
	g.m_stage0Active = FALSE;
	g.m_useSubStage = FALSE;
	g.m_load4444 = FALSE;
	g.m_texEnvMode[0] = GL_MODULATE;
	g.m_texEnvMode[1] = GL_MODULATE;
	g.m_vertCount = 0;
	g.m_vertStart = 0;
	g.m_indexCount = 0;
	g.m_textureStage = 0;

	for (i = 0; i < D3D_MAX_TEXTURES; i++)
	{
		g.m_textures[i].m_pDDSurface = NULL;
		g.m_textures[i].m_minFilter = D3DTFN_POINT;
		g.m_textures[i].m_magFilter = D3DTFG_LINEAR;
		g.m_textures[i].m_mipFilter = D3DTFP_LINEAR;
		g.m_textures[i].m_addressU = D3DTADDRESS_WRAP;
		g.m_textures[i].m_addressV = D3DTADDRESS_WRAP;
		g.m_textures[i].m_pD3DTexture = NULL;
	}

	// Create identity matrix
	D3DMATRIX matrix;
	matrix._11 = 1.0f;	matrix._12 = 0.0f;	matrix._13 = 0.0f;	matrix._14 = 0.0f;
	matrix._21 = 0.0f;	matrix._22 = 1.0f;	matrix._23 = 0.0f;	matrix._24 = 0.0f;
	matrix._31 = 0.0f;	matrix._32 = 0.0f;	matrix._33 = 1.0f;	matrix._34 = 0.0f;
	matrix._41 = 0.0f;	matrix._42 = 0.0f;	matrix._43 = 0.0f;	matrix._44 = 1.0f;

	g.m_pD3DDev->lpVtbl->SetTransform(g.m_pD3DDev, D3DTRANSFORMSTATE_VIEW, &matrix);
	g.m_pD3DDev->lpVtbl->SetTransform(g.m_pD3DDev, D3DTRANSFORMSTATE_WORLD, &matrix);
	g.m_pD3DDev->lpVtbl->SetTransform(g.m_pD3DDev, D3DTRANSFORMSTATE_PROJECTION, &matrix);

	for (i = 1; i < D3D_MAX_RSTATES; i++)
	{
		g.m_pD3DDev->lpVtbl->GetRenderState(g.m_pD3DDev, (D3DRENDERSTATETYPE)i, &g.m_rStates[i]);
	}

	for (i = 1; i < D3D_MAX_TSSTAGES; i++)
	{
		g.m_pD3DDev->lpVtbl->GetTextureStageState(g.m_pD3DDev, 0, (D3DTEXTURESTAGESTATETYPE)i, &g.m_tsStates0[i]);
	}

	if (g.m_useMultitexture)
	{
		for (i = 1; i < D3D_MAX_TSSTAGES; i++)
		{
			g.m_pD3DDev->lpVtbl->GetTextureStageState(g.m_pD3DDev, 1, (D3DTEXTURESTAGESTATETYPE)i, &g.m_tsStates1[i]);
		}
	}

	g.m_pD3DTexture = NULL;
	g.m_pD3DTextureSubStage = NULL;

	if (g.m_rStates[D3DRENDERSTATE_TEXTUREPERSPECTIVE] != TRUE)
	{
		if (g.m_indexCount)
		{
			if (g.m_vertStart != g.m_vertCount)
			{
				g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
				g.m_vertStart = g.m_vertCount;
			}

			g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
			g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
			g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dwSize);

			g.m_vertStart = 0;
			g.m_vertCount = 0;
			g.m_indexCount = 0;
		}

		g.m_rStates[D3DRENDERSTATE_TEXTUREPERSPECTIVE] = TRUE;
		g.m_pD3DDev->lpVtbl->SetRenderState(g.m_pD3DDev, D3DRENDERSTATE_TEXTUREPERSPECTIVE, TRUE);
	}

	if (g.m_rStates[D3DRENDERSTATE_SPECULARENABLE] != FALSE)
	{
		if (g.m_indexCount)
		{
			if (g.m_vertStart != g.m_vertCount)
			{
				g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
				g.m_vertStart = g.m_vertCount;
			}

			g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
			g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
			g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dwSize);

			g.m_vertStart = 0;
			g.m_vertCount = 0;
			g.m_indexCount = 0;
		}

		g.m_rStates[D3DRENDERSTATE_SPECULARENABLE] = FALSE;
		g.m_pD3DDev->lpVtbl->SetRenderState(g.m_pD3DDev, D3DRENDERSTATE_SPECULARENABLE, FALSE);
	}

	if (g.m_rStates[D3DRENDERSTATE_DITHERENABLE] != TRUE)
	{
		if (g.m_indexCount)
		{
			if (g.m_vertStart != g.m_vertCount)
			{
				g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
				g.m_vertStart = g.m_vertCount;
			}

			g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
			g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
			g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dwSize);

			g.m_vertStart = 0;
			g.m_vertCount = 0;
			g.m_indexCount = 0;
		}

		g.m_rStates[D3DRENDERSTATE_DITHERENABLE] = TRUE;
		g.m_pD3DDev->lpVtbl->SetRenderState(g.m_pD3DDev, D3DRENDERSTATE_DITHERENABLE, TRUE);
	}

	if (g.m_tsStates0[D3DTSS_TEXCOORDINDEX] != 0)
	{
		if (g.m_indexCount)
		{
			if (g.m_vertStart != g.m_vertCount)
			{
				g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
				g.m_vertStart = g.m_vertCount;
			}

			g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
			g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
			g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dwSize);

			g.m_vertStart = 0;
			g.m_vertCount = 0;
			g.m_indexCount = 0;
		}

		g.m_tsStates0[D3DTSS_TEXCOORDINDEX] = 0;
		g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 0, D3DTSS_TEXCOORDINDEX, 0);
	}

	if (g.m_useMultitexture == TRUE)
	{
		if (g.m_tsStates1[D3DTSS_TEXCOORDINDEX] != 1)
		{
			if (g.m_indexCount)
			{
				if (g.m_vertStart != g.m_vertCount)
				{
					g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
					g.m_vertStart = g.m_vertCount;
				}

				g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
				g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
				g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dwSize);

				g.m_vertStart = 0;
				g.m_vertCount = 0;
				g.m_indexCount = 0;
			}

			g.m_tsStates1[D3DTSS_TEXCOORDINDEX] = 1;
			g.m_pD3DDev->lpVtbl->SetTextureStageState(g.m_pD3DDev, 1, D3DTSS_TEXCOORDINDEX, 1);
		}
	}

	// Get DirectDraw interface
	if (FAILED(g.m_pDXMgr->lpVtbl->GetDirectDraw(g.m_pDXMgr, &g.m_pDD4)))
	{
		g.m_pD3DDev->lpVtbl->Release(g.m_pD3DDev);
		g.m_pDXMgr->lpVtbl->Release(g.m_pDXMgr);
		CoUninitialize();
		return NULL;
	}

	// Get current viewport
	if (FAILED(g.m_pD3DDev->lpVtbl->GetCurrentViewport(g.m_pD3DDev, &g.m_pD3DVP3)))
	{
		g.m_pD3DDev->lpVtbl->Release(g.m_pD3DDev);
		g.m_pDD4->lpVtbl->Release(g.m_pDD4);
		g.m_pDXMgr->lpVtbl->Release(g.m_pDXMgr);
		CoUninitialize();
		return NULL;
	}

	// Tell D3D we're ready to start rendering
	g.m_pD3DDev->lpVtbl->BeginScene(g.m_pD3DDev);

	return (HGLRC)1;
}

DLL_EXPORT HGLRC WINAPI wglCreateLayerContext( HDC hdc, int iLayerPlan )
{
	return (HGLRC)1;
}

DLL_EXPORT BOOL WINAPI wglDeleteContext( HGLRC hglrc )
{
	g.m_pD3DDev->lpVtbl->EndScene(g.m_pD3DDev);

	for (int i = 0; i < D3D_MAX_TEXTURES; i++)
	{
		D3D_TEXTURE* tex = &g.m_textures[i];

		if (tex->m_pD3DTexture)
		{
			tex->m_pD3DTexture->lpVtbl->Release(tex->m_pD3DTexture);
			tex->m_pD3DTexture = NULL;
		}

		if (tex->m_pDDSurface)
		{
			tex->m_pDDSurface->lpVtbl->Release(tex->m_pDDSurface);
			tex->m_pDDSurface = NULL;
		}
	}
	
	g.m_pD3DVP3->lpVtbl->Release(g.m_pD3DVP3);
	g.m_pD3DVP3 = NULL;

	g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
	g.m_pD3DVBSrc->lpVtbl->Release(g.m_pD3DVBSrc);
	g.m_pD3DVBSrc = NULL;

	g.m_pD3DVB->lpVtbl->Release(g.m_pD3DVB);
	g.m_pD3DVB = NULL;

	g.m_pD3DDev->lpVtbl->Release(g.m_pD3DDev);
	g.m_pD3DDev = NULL;

	g.m_pDD4->lpVtbl->Release(g.m_pDD4);
	g.m_pDD4 = NULL;

	g.m_pDXMgr->lpVtbl->Release(g.m_pDXMgr);
	g.m_pDXMgr = NULL;

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
	return g.m_hDC;
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

	if (g.m_indexCount)
	{
		if (g.m_vertStart != g.m_vertCount)
		{
			g.m_pD3DVB->lpVtbl->ProcessVertices(g.m_pD3DVB, 5, g.m_vertStart, g.m_vertCount - g.m_vertStart, g.m_pD3DVBSrc, g.m_vertStart, g.m_pD3DDev, 0);
			g.m_vertStart = g.m_vertCount;
		}

		g.m_pD3DVBSrc->lpVtbl->Unlock(g.m_pD3DVBSrc);
		g.m_pD3DDev->lpVtbl->DrawIndexedPrimitiveVB(g.m_pD3DDev, D3DPT_TRIANGLELIST, g.m_pD3DVB, g.m_indexBuffer, g.m_indexCount, 8);
		g.m_pD3DVBSrc->lpVtbl->Lock(g.m_pD3DVBSrc, DDLOCK_NOSYSLOCK | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, (LPVOID*)&g.m_verts, &dummy);

		g.m_vertStart = 0;
		g.m_vertCount = 0;
		g.m_indexCount = 0;
	}

	g.m_pD3DDev->lpVtbl->EndScene(g.m_pD3DDev);
	g.m_pDXMgr->lpVtbl->Update(g.m_pDXMgr, g.m_doFlip ? 0 : DXMGRUPDATE_FORCEBLT, NULL);
	g.m_pD3DDev->lpVtbl->BeginScene(g.m_pD3DDev);

	return TRUE;
}

DLL_EXPORT void Download4444( void )
{
	g.m_load4444 = TRUE;
}

DLL_EXPORT void QGL_D3DShared( D3DGLOBALS* d3dGShared )
{
	g.m_pDD4 = d3dGShared->lpDD4;
	g.m_fullscreen = d3dGShared->bFullscreen;
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