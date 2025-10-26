// Base class for flying monsters.  This overrides the movement test & execution code from CBaseMonster

#ifndef FLYINGMONSTER_H
#define FLYINGMONSTER_H

class CFlyingMonster : public CBaseMonster
{
public:
	int 		CheckLocalMove ( const Vector &vecStart, Vector &vecEnd, CBaseEntity *pTarget, float *pflDist );// check validity of a straight move through space
	BOOL		FTriangulate ( const Vector &vecStart , const Vector &vecEnd, float flDist, CBaseEntity *pTargetEnt, Vector *pApex );
	Activity	GetStoppedActivity( void );
	void		Killed( entvars_t *pevAttacker, int iGib );
	void		Stop( void );
	float		ChangeYaw( int speed );
	void		HandleAnimEvent( MonsterEvent_t *pEvent );
	void		MoveExecute( CBaseEntity *pTargetEnt, const Vector &vecDir, float flInterval );
	void		Move( float flInterval = 0.1 );
	BOOL		ShouldAdvanceRoute( float flWaypointDist );

	inline void	SetFlyingMomentum( float momentum ) { m_momentum = momentum; }
	inline void	SetFlyingFlapSound( char *pFlapSound ) { m_pFlapSound = pFlapSound; }
	inline void	SetFlyingSpeed( float speed ) { m_flightSpeed = speed; }
	float		CeilingZ( const Vector &position );
	float		FloorZ( const Vector &position );
	BOOL		ProbeZ( const Vector &position, const Vector &probe, float *pFraction );
	
	
	// UNDONE:  Save/restore this stuff!!!
protected:
	Vector		m_vecTravel;		// Current direction
	float		m_flightSpeed;		// Current flight speed (decays when not flapping or gliding)
	float		m_stopTime;			// Last time we stopped (to avoid switching states too soon)
	float		m_momentum;			// Weight for desired vs. momentum velocity
	char	*m_pFlapSound;
};


#endif		//FLYINGMONSTER_H

