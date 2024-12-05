#include "quakedef.h"

/*
=================
GLBeginHud

=================
*/
void GLBeginHud( void )
{
	// set to projection matrix
	qglViewport(glx, gly, glwidth, glheight);
	qglMatrixMode(GL_PROJECTION);
	qglPushMatrix();
	qglLoadIdentity();
	qglOrtho(0, glwidth, glheight, 0, -99999, 99999);

	qglMatrixMode(GL_MODELVIEW);
	qglPushMatrix();
	qglLoadIdentity();

	qglDisable(GL_DEPTH_TEST);
	qglDisable(GL_CULL_FACE);
	qglDisable(GL_BLEND);
	qglEnable(GL_ALPHA_TEST);

	qglColor4f(1, 1, 1, 1);
}

/*
=================
DrawWedge

=================
*/
void DrawWedge( float centerx, float centery, float angle1, float angle2, float radius )
{
	qglBegin(GL_TRIANGLES);

	qglVertex2f(centerx, centery);
	qglVertex2f(centerx - cos(angle1 * (M_PI / 180.0)) * radius, centery - sin(angle1 * (M_PI / 180.0)) * radius);
	qglVertex2f(centerx - cos(angle2 * (M_PI / 180.0)) * radius, centery - sin(angle2 * (M_PI / 180.0)) * radius);

	qglEnd();
}

void GLFinishHud( void )
{
	qglMatrixMode(GL_PROJECTION);
	qglPopMatrix();

	qglMatrixMode(GL_MODELVIEW);
	qglPopMatrix();

	qglEnable(GL_DEPTH_TEST);
	qglEnable(GL_CULL_FACE);
	qglEnable(GL_BLEND);
}