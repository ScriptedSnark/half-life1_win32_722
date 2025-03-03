#include "extdll.h"
#include "plane.h"

//=========================================================
// Plane
//=========================================================
CPlane :: CPlane ( void )
{
	m_fInitialized = FALSE;
}

//=========================================================
// InitializePlane - Takes a normal for the plane and a
// point on the plane and 
//=========================================================
void CPlane :: InitializePlane ( const Vector &vecNormal, const Vector &vecPoint )
{
	m_vecNormal = vecNormal;
	m_flDist = DotProduct ( m_vecNormal, vecPoint );
	m_fInitialized = TRUE;
}


//=========================================================
// PointInFront - determines whether the given vector is 
// in front of the plane. 
//=========================================================
BOOL CPlane :: PointInFront ( const Vector &vecPoint )
{
	float flFace;

	if ( !m_fInitialized )
	{
		return FALSE;
	}

	flFace = DotProduct ( m_vecNormal, vecPoint ) - m_flDist;

	if ( flFace >= 0 )
	{
		return TRUE;
	}

	return FALSE;
}

