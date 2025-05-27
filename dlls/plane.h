#ifndef PLANE_H
#define PLANE_H

//=========================================================
// Plane
//=========================================================
class CPlane 
{
public:
	CPlane ( void );

	//=========================================================
	// InitializePlane - Takes a normal for the plane and a
	// point on the plane and 
	//=========================================================
	void InitializePlane ( const Vector &vecNormal, const Vector &vecPoint );

	//=========================================================
	// PointInFront - determines whether the given vector is 
	// in front of the plane. 
	//=========================================================
	BOOL PointInFront ( const Vector &vecPoint );

	Vector	m_vecNormal;
	float	m_flDist;
	BOOL	m_fInitialized;
};

#endif // PLANE_H
