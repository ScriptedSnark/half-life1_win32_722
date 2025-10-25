#include "opengl32.h"
#include "d3d_structs.h"

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
		// Flush any remaining primitives
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
	// TODO: Implement
}

static void QuakeFlushVertexBuffer( void )
{
	if (gD3D.vertStart != gD3D.vertCount)
	{
		gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
		gD3D.vertStart = gD3D.vertCount;
	}
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
		if (gD3D.alphaFunc != alphaFunc)
		{
			if (gD3D.indexCount)
			{
				// Flush any remaining primitives
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

			gD3D.alphaFunc = alphaFunc;
			gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_ALPHAFUNC, alphaFunc);
		}

		alphaRef = (DWORD)(ref * 255.0);
		if (gD3D.alphaRef != alphaRef)
		{
			if (gD3D.indexCount)
			{
				// Flush any remaining primitives
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

			gD3D.alphaRef = alphaRef;
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
	// TODO: Implement
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
		if (gD3D.srcBlend != srcBlend)
		{
			if (gD3D.indexCount)
			{
				// Flush any remaining primitives
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

			gD3D.srcBlend = srcBlend;
			gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_SRCBLEND, srcBlend);
		}
	}

	if (destBlend >= 0)
	{
		if (gD3D.destBlend != destBlend)
		{
			if (gD3D.indexCount)
			{
				// Flush any remaining primitives
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

			gD3D.destBlend = destBlend;
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
		// Flush any remaining primitives
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

		if (gD3D.cullMode != cullMode)
		{
			if (gD3D.indexCount)
			{
				// Flush any remaining primitives
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

			gD3D.cullMode = cullMode;
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
		if (gD3D.zFunc != zFunc)
		{
			if (gD3D.indexCount)
			{
				// Flush any remaining primitives
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

			gD3D.zFunc = zFunc;
			gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_ZFUNC, zFunc);
		}
	}
}

DLL_EXPORT void APIENTRY glDepthMask( GLboolean flag )
{
	DWORD	dummy;

	if (flag)
	{
		if (gD3D.zWriteEnable != TRUE)
		{
			if (gD3D.indexCount)
			{
				// Flush any remaining primitives
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

			gD3D.zWriteEnable = TRUE;
			gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_ZWRITEENABLE, TRUE);
		}
	}
	else
	{
		if (gD3D.zWriteEnable != FALSE)
		{
			if (gD3D.indexCount)
			{
				// Flush any remaining primitives
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

			gD3D.zWriteEnable = FALSE;
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
		// Flush any remaining primitives
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
	// TODO: Implement
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
	// TODO: Implement
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
	// TODO: Implement
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
	// TODO: Implement
}

DLL_EXPORT void APIENTRY glFogfv( GLenum pname, const GLfloat* params )
{
	// TODO: Implement
}

DLL_EXPORT void APIENTRY glFogi( GLenum pname, GLint param )
{
	// TODO: Implement
}

DLL_EXPORT void APIENTRY glFogiv( GLenum pname, const GLint* params )
{
}

DLL_EXPORT void APIENTRY glFrontFace( GLenum mode )
{
}

DLL_EXPORT void APIENTRY glFrustum( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar )
{
	// TODO: Implement
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
		[ 1 0 0 0 ]
		[ 0 1 0 0 ]
		[ 0 0 1 0 ]
		[ 0 0 0 1 ]
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
	// TODO: Implement
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
		if (gD3D.fillMode != fillMode)
		{
			if (gD3D.indexCount)
			{
				// Flush any remaining primitives
				if (gD3D.vertStart != gD3D.vertCount)
				{
					gD3D.lpD3DVB->lpVtbl->ProcessVertices(gD3D.lpD3DVB, 5, gD3D.vertStart, gD3D.vertCount - gD3D.vertStart, gD3D.lpD3DVBSrc, gD3D.vertStart, gD3D.lpD3DD3, 0);
					gD3D.vertStart = gD3D.vertCount;
				}

				gD3D.lpD3DVBSrc->lpVtbl->Unlock(gD3D.lpD3DVBSrc);
				gD3D.lpD3DD3->lpVtbl->DrawIndexedPrimitiveVB(gD3D.lpD3DD3, D3DPT_TRIANGLELIST, gD3D.lpD3DVB, gD3D.indexBuffer, gD3D.indexCount, 8);
				gD3D.lpD3DVBSrc->lpVtbl->Lock(gD3D.lpD3DVBSrc, 2081, (LPVOID*)&gD3D.verts, &dummy);

				gD3D.vertStart = 0;
				gD3D.vertCount = 0;
				gD3D.indexCount = 0;
			}

			gD3D.fillMode = fillMode;
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
		// Flush any remaining primitives
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
	// TODO: Implement
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
	// TODO: Implement
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
	// TODO: Implement
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
	// TODO: Implement
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
		[ x 0 0 0 ]
		[ 0 y 0 0 ]
		[ 0 0 z 0 ]
		[ 0 0 0 1 ]
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
		if (gD3D.shadeMode != D3DSHADE_GOURAUD)
		{
			if (gD3D.indexCount)
			{
				// Flush any remaining primitives
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

			gD3D.shadeMode = D3DSHADE_GOURAUD;
			gD3D.lpD3DD3->lpVtbl->SetRenderState(gD3D.lpD3DD3, D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
		}
	}
	else
	{
		if (gD3D.shadeMode != D3DSHADE_FLAT)
		{
			if (gD3D.indexCount)
			{
				// Flush any remaining primitives
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

			gD3D.shadeMode = D3DSHADE_FLAT;
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
	// TODO: Implement
}

DLL_EXPORT void APIENTRY glTexParameterf( GLenum target, GLenum pname, GLfloat param )
{
	// TODO: Implement
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
	// TODO: Implement
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
		[ 1 0 0 0 ]
		[ 0 1 0 0 ]
		[ 0 0 1 0 ]
		[ x y z 1 ]
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
	D3DVIEWPORT2	vport2;
	DWORD	dummy;

	if (gD3D.indexCount)
	{
		// Flush any remaining primitives
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
	vport2.dwSize = sizeof(vport2);
	gD3D.lpD3DVP3->lpVtbl->GetViewport2(gD3D.lpD3DVP3, &vport2);
	vport2.dwX = x;
	vport2.dwY = gD3D.wndHeight - height - y;
	vport2.dwWidth = width;
	vport2.dwHeight = height;
	vport2.dvClipX = -1.0;
	vport2.dvClipY = 1.0;
	vport2.dvClipWidth = 2.0;
	vport2.dvClipHeight = 2.0;

	gD3D.lpD3DVP3->lpVtbl->SetViewport2(gD3D.lpD3DVP3, &vport2);
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

	gD3D.hDC = hdc;
	gD3D.hWnd = WindowFromDC(hdc);

	GetClientRect(gD3D.hWnd, &rect);

	gD3D.wndWidth = (USHORT)rect.right;
	gD3D.wndHeight = (USHORT)rect.bottom;

	if (FAILED(CoInitialize(NULL)))
		return NULL;

	if (FAILED(DirectDrawCreate(NULL, &lpDD, NULL)))
	{
		CoUninitialize();
		return NULL;
	}

	ddsd.dwSize = sizeof(ddsd);
	if (FAILED(lpDD->lpVtbl->GetDisplayMode(lpDD, &ddsd)))
	{
		lpDD->lpVtbl->Release(lpDD);
		CoUninitialize();
		return NULL;
	}

	lpDD->lpVtbl->Release(lpDD);

	// TODO: Implement

	return (HGLRC)1;
}

DLL_EXPORT HGLRC WINAPI wglCreateLayerContext( HDC hdc, int iLayerPlan )
{
	return (HGLRC)1;
}

DLL_EXPORT BOOL WINAPI wglDeleteContext( HGLRC hglrc )
{
	// TODO: Implement
	
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

	// TODO: Implement
	gD3D.pDirectDrawMgr = NULL;

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
		// Flush any remaining primitives
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
	// gD3D.pDirectDrawMgr->Frame(gD3D.pDirectDrawMgr, gD3D.doFlip ? 0 : 2, 0); TODO: Implement
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