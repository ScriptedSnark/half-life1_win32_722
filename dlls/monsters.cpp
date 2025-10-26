/*

===== monsters.cpp ========================================================

  Monster-related utility code

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "nodes.h"
#include "monsters.h"
#include "animation.h"
#include "saverestore.h"
#include "weapons.h"
#include "scripted.h"
#include "squadmonster.h"
#include "decals.h"
#include "defaultai.h"
#include "soundent.h"
#include "gamerules.h"

#define MONSTER_CUT_CORNER_DIST		8 // 8 means the monster's bounding box is contained without the box of the node in WC


Vector VecBModelOrigin( entvars_t* pevBModel );

extern DLL_GLOBAL	BOOL	g_fDrawLines;
extern DLL_GLOBAL	short	g_sModelIndexLaser;// holds the index for the laser beam
extern DLL_GLOBAL	short	g_sModelIndexLaserDot;// holds the index for the laser beam dot

extern CGraph WorldGraph;// the world node graph

TYPEDESCRIPTION	CBaseAnimating::m_SaveData[] = 
{
	DEFINE_FIELD( CBaseMonster, m_flFrameRate, FIELD_FLOAT ),
	DEFINE_FIELD( CBaseMonster, m_flGroundSpeed, FIELD_FLOAT ),
	DEFINE_FIELD( CBaseMonster, m_flLastEventCheck, FIELD_TIME ),
	DEFINE_FIELD( CBaseMonster, m_fSequenceFinished, FIELD_BOOLEAN ),
	DEFINE_FIELD( CBaseMonster, m_fSequenceLoops, FIELD_BOOLEAN ),
};

IMPLEMENT_SAVERESTORE( CBaseAnimating, CBaseDelay );

// Global Savedata for monster
// UNDONE: Save schedule data?  Can this be done?  We may
// lose our enemy pointer or other data (goal ent, target, etc)
// that make the current schedule invalid, perhaps it's best
// to just pick a new one when we start up again.
TYPEDESCRIPTION	CBaseMonster::m_SaveData[] = 
{
	DEFINE_FIELD( CBaseMonster, m_hEnemy, FIELD_EHANDLE ),
	DEFINE_FIELD( CBaseMonster, m_hTargetEnt, FIELD_EHANDLE ),
	DEFINE_ARRAY( CBaseMonster, m_hOldEnemy, FIELD_EHANDLE, MAX_OLD_ENEMIES ),
	DEFINE_ARRAY( CBaseMonster, m_vecOldEnemy, FIELD_POSITION_VECTOR, MAX_OLD_ENEMIES ),
	DEFINE_FIELD( CBaseMonster, m_flFieldOfView, FIELD_FLOAT ),
	DEFINE_FIELD( CBaseMonster, m_flWaitFinished, FIELD_TIME ),
	DEFINE_FIELD( CBaseMonster, m_flMoveWaitFinished, FIELD_TIME ),

	DEFINE_FIELD( CBaseMonster, m_Activity, FIELD_INTEGER ),
	DEFINE_FIELD( CBaseMonster, m_IdealActivity, FIELD_INTEGER ),
	DEFINE_FIELD( CBaseMonster, m_LastHitGroup, FIELD_INTEGER ),
	DEFINE_FIELD( CBaseMonster, m_MonsterState, FIELD_INTEGER ),
	DEFINE_FIELD( CBaseMonster, m_IdealMonsterState, FIELD_INTEGER ),
	DEFINE_FIELD( CBaseMonster, m_iTaskStatus, FIELD_INTEGER ),

	//Schedule_t			*m_pSchedule;

	DEFINE_FIELD( CBaseMonster, m_iScheduleIndex, FIELD_INTEGER ),
	DEFINE_FIELD( CBaseMonster, m_afConditions, FIELD_INTEGER ),
	//WayPoint_t			m_Route[ ROUTE_SIZE ];
//	DEFINE_FIELD( CBaseMonster, m_movementGoal, FIELD_INTEGER ),
//	DEFINE_FIELD( CBaseMonster, m_iRouteIndex, FIELD_INTEGER ),
//	DEFINE_FIELD( CBaseMonster, m_moveWaitTime, FIELD_FLOAT ),

	DEFINE_FIELD( CBaseMonster, m_vecMoveGoal, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( CBaseMonster, m_movementActivity, FIELD_INTEGER ),

	//		int					m_iAudibleList; // first index of a linked list of sounds that the monster can hear.
//	DEFINE_FIELD( CBaseMonster, m_afSoundTypes, FIELD_INTEGER ),
	DEFINE_FIELD( CBaseMonster, m_vecLastPosition, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( CBaseMonster, m_iHintNode, FIELD_INTEGER ),
	DEFINE_FIELD( CBaseMonster, m_afMemory, FIELD_INTEGER ),
	DEFINE_FIELD( CBaseMonster, m_iMaxHealth, FIELD_INTEGER ),

	DEFINE_FIELD( CBaseMonster, m_vecEnemyLKP, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( CBaseMonster, m_cAmmoLoaded, FIELD_INTEGER ),
	DEFINE_FIELD( CBaseMonster, m_afCapability, FIELD_INTEGER ),

	DEFINE_FIELD( CBaseMonster, m_flNextAttack, FIELD_TIME ),
	DEFINE_FIELD( CBaseMonster, m_bitsDamageType, FIELD_INTEGER ),
	DEFINE_ARRAY( CBaseMonster, m_rgbTimeBasedDamage, FIELD_CHARACTER, CDMG_TIMEBASED ),
	DEFINE_FIELD( CBaseMonster, m_bloodColor, FIELD_INTEGER ),
	DEFINE_FIELD( CBaseMonster, m_failSchedule, FIELD_INTEGER ),

	DEFINE_FIELD( CBaseMonster, m_flHungryTime, FIELD_TIME ),
	DEFINE_FIELD( CBaseMonster, m_flDistTooFar, FIELD_FLOAT ),
	DEFINE_FIELD( CBaseMonster, m_flDistLook, FIELD_FLOAT ),
	DEFINE_FIELD( CBaseMonster, m_iTriggerCondition, FIELD_INTEGER ),
	DEFINE_FIELD( CBaseMonster, m_iszTriggerTarget, FIELD_STRING ),

	DEFINE_FIELD( CBaseMonster, m_HackedGunPos, FIELD_VECTOR ),

	DEFINE_FIELD( CBaseMonster, m_scriptState, FIELD_INTEGER ),
	DEFINE_FIELD( CBaseMonster, m_pCine, FIELD_CLASSPTR ),
};

//IMPLEMENT_SAVERESTORE( CBaseMonster, CBaseToggle );
int CBaseMonster::Save( CSave &save )
{
	if ( !CBaseToggle::Save(save) )
		return 0;
	return save.WriteFields( "CBaseMonster", this, m_SaveData, ARRAYSIZE(m_SaveData) );
}

int CBaseMonster::Restore( CRestore &restore )
{
	if ( !CBaseToggle::Restore(restore) )
		return 0;
	int status = restore.ReadFields( "CBaseMonster", this, m_SaveData, ARRAYSIZE(m_SaveData) );
	
	// We don't save/restore routes yet
	RouteClear();

	// We don't save/restore schedules yet
	m_pSchedule = NULL;
	m_iTaskStatus = TASKSTATUS_NEW;
	
	// Reset animation
	m_Activity = ACT_RESET;

	// If we don't have an enemy, clear conditions like see enemy, etc.
	if ( m_hEnemy == NULL )
		m_afConditions = 0;

	return status;
}

//=========================================================
// CheckTraceHullAttack - expects a length to trace, amount 
// of damage to do, and damage type. Returns a pointer to
// the damaged entity in case the monster wishes to do
// other stuff to the victim (punchangle, etc)
//
// Used for many contact-range melee attacks. Bites, claws, etc.
//=========================================================
CBaseEntity* CBaseMonster :: CheckTraceHullAttack( float flDist, int iDamage, int iDmgType )
{
	TraceResult tr;

	if (pev->flags & FL_CLIENT)
		UTIL_MakeVectors( pev->angles );
	else
		UTIL_MakeAimVectors( pev->angles );

	Vector vecStart = pev->origin;
	vecStart.z += pev->size.z * 0.5;
	Vector vecEnd = vecStart + (gpGlobals->v_forward * flDist );

	UTIL_TraceHull( vecStart, vecEnd, dont_ignore_monsters, head_hull, ENT(pev), &tr );
	
	if ( tr.pHit )
	{
		CBaseEntity *pEntity = CBaseEntity::Instance( tr.pHit );

		if ( iDamage > 0 )
		{
			pEntity->TakeDamage( pev, pev, iDamage, iDmgType );
		}

		return pEntity;
	}

	return NULL;
}

//=========================================================
// Eat - makes a monster full for a little while.
//=========================================================
void CBaseMonster :: Eat ( float flFullDuration )
{
	m_flHungryTime = gpGlobals->time + flFullDuration;
}

//=========================================================
// FShouldEat - returns true if a monster is hungry.
//=========================================================
BOOL CBaseMonster :: FShouldEat ( void )
{
	if ( m_flHungryTime > gpGlobals->time )
	{
		return FALSE;
	}

	return TRUE;
}

//=========================================================
// BarnacleVictimBitten - called
// by Barnacle victims when the barnacle pulls their head
// into its mouth
//=========================================================
void CBaseMonster :: BarnacleVictimBitten ( entvars_t *pevBarnacle )
{
	Schedule_t	*pNewSchedule;

	pNewSchedule = GetScheduleOfType( SCHED_BARNACLE_VICTIM_CHOMP );

	if ( pNewSchedule )
	{
		ChangeSchedule( pNewSchedule );
	}
}

//=========================================================
// BarnacleVictimReleased - called by barnacle victims when
// the host barnacle is killed.
//=========================================================
void CBaseMonster :: BarnacleVictimReleased ( void )
{
	m_IdealMonsterState = MONSTERSTATE_IDLE;

	pev->velocity = g_vecZero;
	pev->movetype = MOVETYPE_STEP;
}

//=========================================================
// Listen - monsters dig through the active sound list for
// any sounds that may interest them. (smells, too!)
//=========================================================
void CBaseMonster :: Listen ( void )
{
	int		iSound;
	int		iMySounds;
	float	hearingSensitivity;
	CSound	*pCurrentSound;

	m_iAudibleList = SOUNDLIST_EMPTY; 
	ClearConditions(bits_COND_HEAR_SOUND | bits_COND_SMELL | bits_COND_SMELL_FOOD);
	m_afSoundTypes = 0;

	iMySounds = ISoundMask();

	if ( m_pSchedule )
	{
		//!!!WATCH THIS SPOT IF YOU ARE HAVING SOUND RELATED BUGS!
		// Make sure your schedule AND personal sound masks agree!
		iMySounds &= m_pSchedule->iSoundMask;
	}

	iSound = CSoundEnt::ActiveList();

	// UNDONE: Clear these here?
	ClearConditions( bits_COND_HEAR_SOUND | bits_COND_SMELL_FOOD | bits_COND_SMELL );
	hearingSensitivity = HearingSensitivity( );

	while ( iSound != SOUNDLIST_EMPTY )
	{
		pCurrentSound = CSoundEnt::SoundPointerForIndex( iSound );

		if ( pCurrentSound	&& 
			 ( pCurrentSound->m_iType & iMySounds )	&& 
			 ( pCurrentSound->m_vecOrigin - EarPosition() ).Length() <= pCurrentSound->m_iVolume * hearingSensitivity )

		//if ( ( g_pSoundEnt->m_SoundPool[ iSound ].m_iType & iMySounds ) && ( g_pSoundEnt->m_SoundPool[ iSound ].m_vecOrigin - EarPosition()).Length () <= g_pSoundEnt->m_SoundPool[ iSound ].m_iVolume * hearingSensitivity ) 
		{
 			// the monster cares about this sound, and it's close enough to hear.
			//g_pSoundEnt->m_SoundPool[ iSound ].m_iNextAudible = m_iAudibleList;
			pCurrentSound->m_iNextAudible = m_iAudibleList;
			
			if ( pCurrentSound->FIsSound() )
			{
				// this is an audible sound.
				SetConditions( bits_COND_HEAR_SOUND );
			}
			else
			{
				// if not a sound, must be a smell - determine if it's just a scent, or if it's a food scent
//				if ( g_pSoundEnt->m_SoundPool[ iSound ].m_iType & ( bits_SOUND_MEAT | bits_SOUND_CARCASS ) )
				if ( pCurrentSound->m_iType & ( bits_SOUND_MEAT | bits_SOUND_CARCASS ) )
				{
					// the detected scent is a food item, so set both conditions.
					// !!!BUGBUG - maybe a virtual function to determine whether or not the scent is food?
					SetConditions( bits_COND_SMELL_FOOD );
					SetConditions( bits_COND_SMELL );
				}
				else
				{
					// just a normal scent. 
					SetConditions( bits_COND_SMELL );
				}
			}

//			m_afSoundTypes |= g_pSoundEnt->m_SoundPool[ iSound ].m_iType;
			m_afSoundTypes |= pCurrentSound->m_iType;

			m_iAudibleList = iSound;
		}

//		iSound = g_pSoundEnt->m_SoundPool[ iSound ].m_iNext;
		iSound = pCurrentSound->m_iNext;
	}
}

//=========================================================
// FLSoundVolume - subtracts the volume of the given sound
// from the distance the sound source is from the caller, 
// and returns that value, which is considered to be the 'local' 
// volume of the sound. 
//=========================================================
float CBaseMonster :: FLSoundVolume ( CSound *pSound )
{
	return ( pSound->m_iVolume - ( ( pSound->m_vecOrigin - pev->origin ).Length() ) );
}

//=========================================================
// FValidateHintType - tells use whether or not the monster cares
// about the type of Hint Node given
//=========================================================
BOOL CBaseMonster :: FValidateHintType ( short sHint )
{
	return FALSE;
}

//=========================================================
// Look - Base class monster function to find enemies or 
// food by sight. iDistance is distance ( in units ) that the 
// monster can see.
//
// Sets the sight bits of the m_afConditions mask to indicate
// which types of entities were sighted.
// Function also sets the Looker's m_pLink 
// to the head of a link list that contains all visible ents.
// (linked via each ent's m_pLink field)
//
//=========================================================
void CBaseMonster :: Look ( int iDistance )
{
	int	iSighted = 0;

	// DON'T let visibility information from last frame sit around!
	ClearConditions(bits_COND_SEE_HATE | bits_COND_SEE_DISLIKE | bits_COND_SEE_ENEMY | bits_COND_SEE_FEAR | bits_COND_SEE_NEMESIS | bits_COND_SEE_CLIENT);

	m_pLink = NULL;

	CBaseEntity	*pSightEnt = NULL;// the current visible entity that we're dealing with

	// See no evil if prisoner is set
	if ( !FBitSet( pev->spawnflags, SF_MONSTER_PRISONER ) )
	{
		CBaseEntity *pList[100];

		Vector delta = Vector( iDistance, iDistance, iDistance );

		// Find only monsters/clients in box, NOT limited to PVS
		int count = UTIL_EntitiesInBox( pList, 100, pev->origin - delta, pev->origin + delta, FL_CLIENT|FL_MONSTER );
		for ( int i = 0; i < count; i++ )
		{
			pSightEnt = pList[i];
			// !!!temporarily only considering other monsters and clients, don't see prisoners
			if ( pSightEnt != this												&& 
				 !FBitSet( pSightEnt->pev->spawnflags, SF_MONSTER_PRISONER )	&& 
				 pSightEnt->pev->health > 0 )
			{
				// the looker will want to consider this entity
				// don't check anything else about an entity that can't be seen, or an entity that you don't care about.
				if ( IRelationship( pSightEnt ) != R_NO && FInViewCone( pSightEnt ) && !FBitSet( pSightEnt->pev->flags, FL_NOTARGET ) && FVisible( pSightEnt ) )
				{
					if ( pSightEnt->pev->flags & FL_CLIENT )
					{
						if ( pev->spawnflags & SF_MONSTER_WAIT_TILL_SEEN )
						{
							CBaseMonster *pClient;

							pClient = pSightEnt->MyMonsterPointer();
							// don't link this client in the list if the monster is wait till seen and the player isn't facing the monster
							if ( pSightEnt && !pClient->FInViewCone( this ) )
							{
								// we're not in the player's view cone. 
								continue;
							}
							else
							{
								// player sees us, become normal now.
								pev->spawnflags &= ~SF_MONSTER_WAIT_TILL_SEEN;
							}
						}

						// if we see a client, remember that (mostly for scripted AI)
						iSighted |= bits_COND_SEE_CLIENT;
					}

					pSightEnt->m_pLink = m_pLink;
					m_pLink = pSightEnt;

					if ( pSightEnt == m_hEnemy )
					{
						// we know this ent is visible, so if it also happens to be our enemy, store that now.
						iSighted |= bits_COND_SEE_ENEMY;
					}

					// don't add the Enemy's relationship to the conditions. We only want to worry about conditions when
					// we see monsters other than the Enemy.
					switch ( IRelationship ( pSightEnt ) )
					{
					case	R_NM:
						iSighted |= bits_COND_SEE_NEMESIS;		
						break;
					case	R_HT:		
						iSighted |= bits_COND_SEE_HATE;		
						break;
					case	R_DL:
						iSighted |= bits_COND_SEE_DISLIKE;
						break;
					case	R_FR:
						iSighted |= bits_COND_SEE_FEAR;
						break;
					case    R_AL:
						break;
					default:
						ALERT ( at_aiconsole, "%s can't assess %s\n", STRING(pev->classname), STRING(pSightEnt->pev->classname ) );
						break;
					}
				}
			}
		}
	}
	
	SetConditions( iSighted );
}

//=========================================================
// ISoundMask - returns a bit mask indicating which types
// of sounds this monster regards. In the base class implementation,
// monsters care about all sounds, but no scents.
//=========================================================
int CBaseMonster :: ISoundMask ( void )
{
	return	bits_SOUND_WORLD	|
			bits_SOUND_COMBAT	|
			bits_SOUND_PLAYER;
}

//=========================================================
// PBestSound - returns a pointer to the sound the monster 
// should react to. Right now responds only to nearest sound.
//=========================================================
CSound* CBaseMonster :: PBestSound ( void )
{	
	int iThisSound; 
	int	iBestSound = -1;
	float flBestDist = 8192;// so first nearby sound will become best so far.
	float flDist;
	CSound *pSound;

	iThisSound = m_iAudibleList; 

	if ( iThisSound == SOUNDLIST_EMPTY )
	{
		ALERT ( at_aiconsole, "ERROR! monster %s has no audible sounds!\n", STRING(pev->classname) );
#if _DEBUG
		ALERT( at_error, "NULL Return from PBestSound\n" );
#endif
		return NULL;
	}

	while ( iThisSound != SOUNDLIST_EMPTY )
	{
		pSound = CSoundEnt::SoundPointerForIndex( iThisSound );

		if ( pSound && pSound->FIsSound() )
		{
			flDist = ( pSound->m_vecOrigin - EarPosition()).Length();

			if ( flDist < flBestDist )
			{
				iBestSound = iThisSound;
				flBestDist = flDist;
			}
		}

		iThisSound = pSound->m_iNextAudible;
	}
	if ( iBestSound >= 0 )
	{
		pSound = CSoundEnt::SoundPointerForIndex( iBestSound );
		return pSound;
	}
#if _DEBUG
	ALERT( at_error, "NULL Return from PBestSound\n" );
#endif
	return NULL;
}

//=========================================================
// PBestScent - returns a pointer to the scent the monster 
// should react to. Right now responds only to nearest scent
//=========================================================
CSound* CBaseMonster :: PBestScent ( void )
{	
	int iThisScent; 
	int	iBestScent = -1;
	float flBestDist = 8192;// so first nearby smell will become best so far.
	float flDist;
	CSound *pSound;

	iThisScent = m_iAudibleList;// smells are in the sound list.

	if ( iThisScent == SOUNDLIST_EMPTY )
	{
		ALERT ( at_aiconsole, "ERROR! PBestScent() has empty soundlist!\n" );
#if _DEBUG
		ALERT( at_error, "NULL Return from PBestSound\n" );
#endif
		return NULL;
	}

	while ( iThisScent != SOUNDLIST_EMPTY )
	{
		pSound = CSoundEnt::SoundPointerForIndex( iThisScent );

		if ( pSound->FIsScent() )
		{
			flDist = ( pSound->m_vecOrigin - pev->origin ).Length();

			if ( flDist < flBestDist )
			{
				iBestScent = iThisScent;
				flBestDist = flDist;
			}
		}

		iThisScent = pSound->m_iNextAudible;
	}
	if ( iBestScent >= 0 )
	{
		pSound = CSoundEnt::SoundPointerForIndex( iBestScent );

		return pSound;
	}
#if _DEBUG
	ALERT( at_error, "NULL Return from PBestScent\n" );
#endif
	return NULL;
}

//=========================================================
// StudioFrameAdvance - advance the animation frame up to the current time
// if an flInterval is passed in, only advance animation that number of seconds
//=========================================================
float CBaseAnimating :: StudioFrameAdvance ( float flInterval )
{
	if (flInterval == 0.0)
	{
		flInterval = (gpGlobals->time - pev->animtime);
		if (flInterval <= 0.001)
		{
			pev->animtime = gpGlobals->time;
			return 0.0;
		}
	}
	if (! pev->animtime)
		flInterval = 0.0;
	
	pev->frame += flInterval * m_flFrameRate * pev->framerate;
	pev->animtime = gpGlobals->time;

	if (pev->frame < 0.0 || pev->frame >= 256.0) 
	{
		if (m_fSequenceLoops)
			pev->frame -= (int)(pev->frame / 256.0) * 256.0;
		else
			pev->frame = (pev->frame < 0.0) ? 0 : 255;
		m_fSequenceFinished = TRUE;	// just in case it wasn't caught in GetEvents
	}

	return flInterval;
}

//=========================================================
// Monster Think - calls out to core AI functions and handles this
// monster's specific animation events
//=========================================================
void CBaseMonster :: MonsterThink ( void )
{
	pev->nextthink = gpGlobals->time + 0.1;// keep monster thinking.


	RunAI();

	float flInterval = StudioFrameAdvance( ); // animate
// start or end a fidget
// This needs a better home -- switching animations over time should be encapsulated on a per-activity basis
// perhaps MaintainActivity() or a ShiftAnimationOverTime() or something.
	if ( m_MonsterState != MONSTERSTATE_SCRIPT && m_MonsterState != MONSTERSTATE_DEAD && m_Activity == ACT_IDLE && m_fSequenceFinished )
	{
		int iSequence;

		if ( m_fSequenceLoops )
		{
			// animation does loop, which means we're playing subtle idle. Might need to 
			// fidget.
			iSequence = LookupActivity ( m_Activity );
		}
		else
		{
			// animation that just ended doesn't loop! That means we just finished a fidget
			// and should return to our heaviest weighted idle (the subtle one)
			iSequence = LookupActivityHeaviest ( m_Activity );
		}
		if ( iSequence != ACTIVITY_NOT_AVAILABLE )
		{
			pev->sequence = iSequence;	// Set to new anim (if it's there)
			ResetSequenceInfo( );
		}
	}

	DispatchAnimEvents( flInterval );

	if ( !MovementIsComplete() )
	{
		Move( flInterval );
	}
#if _DEBUG	
	else 
	{
		if ( !TaskIsRunning() && !TaskIsComplete() )
			ALERT( at_error, "Schedule stalled!!\n" );
	}
#endif
}

//=========================================================
// CBaseMonster - USE - will make a monster angry at whomever
// activated it.
//=========================================================
void CBaseMonster :: MonsterUse ( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	m_IdealMonsterState = MONSTERSTATE_ALERT;
}

//=========================================================
// Ignore conditions - before a set of conditions is allowed
// to interrupt a monster's schedule, this function removes
// conditions that we have flagged to interrupt the current
// schedule, but may not want to interrupt the schedule every
// time. (Pain, for instance)
//=========================================================
int CBaseMonster :: IgnoreConditions ( void )
{
	int iIgnoreConditions = 0;

	if ( !FShouldEat() )
	{
		// not hungry? Ignore food smell.
		iIgnoreConditions |= bits_COND_SMELL_FOOD;
	}

	if ( m_MonsterState == MONSTERSTATE_SCRIPT && m_pCine )
		iIgnoreConditions |= m_pCine->IgnoreConditions();

	return iIgnoreConditions;
}

//=========================================================
// 	RouteClear - zeroes out the monster's route array and goal
//=========================================================
void CBaseMonster :: RouteClear ( void )
{
	RouteNew();
	m_movementGoal = MOVEGOAL_NONE;
	m_movementActivity = ACT_IDLE;
	Forget( bits_MEMORY_MOVE_FAILED );
}

//=========================================================
// Route New - clears out a route to be changed, but keeps
//				goal intact.
//=========================================================
void CBaseMonster :: RouteNew ( void )
{
	m_Route[ 0 ].iType		= 0;
	m_iRouteIndex			= 0;
}

//=========================================================
// FRouteClear - returns TRUE is the Route is cleared out
// ( invalid )
//=========================================================
BOOL CBaseMonster :: FRouteClear ( void )
{
	if ( m_Route[ m_iRouteIndex ].iType == 0 || m_movementGoal == MOVEGOAL_NONE )
		return TRUE;

	return FALSE;
}

//=========================================================
// FRefreshRoute - after calculating a path to the monster's
// target, this function copies as many waypoints as possible
// from that path to the monster's Route array
//=========================================================
BOOL CBaseMonster :: FRefreshRoute ( void )
{
	CBaseEntity	*pPathCorner;
	int			i;
	BOOL		returnCode;

	RouteNew();

	returnCode = FALSE;

	switch( m_movementGoal )
	{
		case MOVEGOAL_PATHCORNER:
			{
				// monster is on a path_corner loop
				pPathCorner = m_pGoalEnt;
				i = 0;

				while ( pPathCorner && i < ROUTE_SIZE )
				{
					m_Route[ i ].iType = bits_MF_TO_PATHCORNER;
					m_Route[ i ].vecLocation = pPathCorner->pev->origin;

					pPathCorner = pPathCorner->GetNextTarget();

					// Last path_corner in list?
					if ( !pPathCorner )
						m_Route[i].iType |= bits_MF_IS_GOAL;
					
					i++;
				}
			}
			returnCode = TRUE;
			break;

		case MOVEGOAL_ENEMY:
			returnCode = BuildRoute( m_vecEnemyLKP, bits_MF_TO_ENEMY, m_hEnemy );
			break;

		case MOVEGOAL_LOCATION:
			returnCode = BuildRoute( m_vecMoveGoal, bits_MF_TO_LOCATION, NULL );
			break;

		case MOVEGOAL_TARGETENT:
			if (m_hTargetEnt != NULL)
			{
				returnCode = BuildRoute( m_hTargetEnt->pev->origin, bits_MF_TO_TARGETENT, m_hTargetEnt );
			}
			break;

		case MOVEGOAL_NODE:
			returnCode = FGetNodeRoute( m_vecMoveGoal );
//			if ( returnCode )
//				RouteSimplify( NULL );
			break;
	}

	return returnCode;
}


BOOL CBaseMonster::MoveToEnemy( Activity movementAct, float waitTime )
{
	m_movementActivity = movementAct;
	m_moveWaitTime = waitTime;
	
	m_movementGoal = MOVEGOAL_ENEMY;
	return FRefreshRoute();
}


BOOL CBaseMonster::MoveToLocation( Activity movementAct, float waitTime, const Vector &goal )
{
	m_movementActivity = movementAct;
	m_moveWaitTime = waitTime;
	
	m_movementGoal = MOVEGOAL_LOCATION;
	m_vecMoveGoal = goal;
	return FRefreshRoute();
}


BOOL CBaseMonster::MoveToTarget( Activity movementAct, float waitTime )
{
	m_movementActivity = movementAct;
	m_moveWaitTime = waitTime;
	
	m_movementGoal = MOVEGOAL_TARGETENT;
	return FRefreshRoute();
}


BOOL CBaseMonster::MoveToNode( Activity movementAct, float waitTime, const Vector &goal )
{
	m_movementActivity = movementAct;
	m_moveWaitTime = waitTime;

	m_movementGoal = MOVEGOAL_NODE;
	m_vecMoveGoal = goal;
	return FRefreshRoute();
}


#ifdef _DEBUG
void DrawRoute( entvars_t *pev, WayPoint_t *m_Route, int m_iRouteIndex, int r, int g, int b )
{
	int			i;

	if ( m_Route[m_iRouteIndex].iType == 0 )
	{
		ALERT( at_aiconsole, "Can't draw route!\n" );
		return;
	}

//	UTIL_ParticleEffect ( m_Route[ m_iRouteIndex ].vecLocation, g_vecZero, 255, 25 );

	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_BEAMPOINTS);
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		WRITE_COORD( m_Route[ m_iRouteIndex ].vecLocation.x );
		WRITE_COORD( m_Route[ m_iRouteIndex ].vecLocation.y );
		WRITE_COORD( m_Route[ m_iRouteIndex ].vecLocation.z );

		WRITE_SHORT( g_sModelIndexLaser );
		WRITE_BYTE( 0 ); // frame start
		WRITE_BYTE( 10 ); // framerate
		WRITE_BYTE( 1 ); // life
		WRITE_BYTE( 16 );  // width
		WRITE_BYTE( 0 );   // noise
		WRITE_BYTE( r );   // r, g, b
		WRITE_BYTE( g );   // r, g, b
		WRITE_BYTE( b );   // r, g, b
		WRITE_BYTE( 255 );	// brightness
		WRITE_BYTE( 10 );		// speed
	MESSAGE_END();

	for ( i = m_iRouteIndex ; i < ROUTE_SIZE - 1; i++ )
	{
		if ( (m_Route[ i ].iType & bits_MF_IS_GOAL) || (m_Route[ i+1 ].iType == 0) )
			break;

		
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE( TE_BEAMPOINTS );
			WRITE_COORD( m_Route[ i ].vecLocation.x );
			WRITE_COORD( m_Route[ i ].vecLocation.y );
			WRITE_COORD( m_Route[ i ].vecLocation.z );
			WRITE_COORD( m_Route[ i + 1 ].vecLocation.x );
			WRITE_COORD( m_Route[ i + 1 ].vecLocation.y );
			WRITE_COORD( m_Route[ i + 1 ].vecLocation.z );
			WRITE_SHORT( g_sModelIndexLaser );
			WRITE_BYTE( 0 ); // frame start
			WRITE_BYTE( 10 ); // framerate
			WRITE_BYTE( 1 ); // life
			WRITE_BYTE( 8 );  // width
			WRITE_BYTE( 0 );   // noise
			WRITE_BYTE( r );   // r, g, b
			WRITE_BYTE( g );   // r, g, b
			WRITE_BYTE( b );   // r, g, b
			WRITE_BYTE( 255 );	// brightness
			WRITE_BYTE( 10 );		// speed
		MESSAGE_END();

//		UTIL_ParticleEffect ( m_Route[ i ].vecLocation, g_vecZero, 255, 25 );
	}
}
#endif


int ShouldSimplify( int routeType )
{
	routeType &= ~bits_MF_IS_GOAL;

	if ( (routeType == bits_MF_TO_PATHCORNER) || (routeType & bits_MF_DONT_SIMPLIFY) )
		return FALSE;
	return TRUE;
}

//=========================================================
// RouteSimplify
//
// Attempts to make the route more direct by cutting out
// unnecessary nodes & cutting corners.
//
//=========================================================
void CBaseMonster :: RouteSimplify( CBaseEntity *pTargetEnt )
{
	// BUGBUG: this doesn't work 100% yet
	int			i, count, outCount;
	Vector		vecStart;
	WayPoint_t	outRoute[ ROUTE_SIZE * 2 ];	// Any points except the ends can turn into 2 points in the simplified route

	count = 0;

	for ( i = m_iRouteIndex; i < ROUTE_SIZE; i++ )
	{
		if ( !m_Route[i].iType )
			break;
		else
			count++;
		if ( m_Route[i].iType & bits_MF_IS_GOAL )
			break;
	}
	// Can't simplify a direct route!
	if ( count < 2 )
	{
//		DrawRoute( pev, m_Route, m_iRouteIndex, 0, 0, 255 );
		return;
	}

	outCount = 0;
	vecStart = pev->origin;
	for ( i = 0; i < count-1; i++ )
	{
		// Don't eliminate path_corners
		if ( !ShouldSimplify( m_Route[m_iRouteIndex+i].iType ) )
		{
			outRoute[outCount] = m_Route[ m_iRouteIndex + i ];
			outCount++;
		}
		else if ( CheckLocalMove ( vecStart, m_Route[m_iRouteIndex+i+1].vecLocation, pTargetEnt, NULL ) == LOCALMOVE_VALID )
		{
			// Skip vert
			continue;
		}
		else
		{
			Vector vecTest, vecSplit;

			// Halfway between this and next
			vecTest = (m_Route[m_iRouteIndex+i+1].vecLocation + m_Route[m_iRouteIndex+i].vecLocation) * 0.5;

			// Halfway between this and previous
			vecSplit = (m_Route[m_iRouteIndex+i].vecLocation + vecStart) * 0.5;

			int iType = (m_Route[m_iRouteIndex+i].iType | bits_MF_TO_DETOUR) & ~bits_MF_NOT_TO_MASK;
			if ( CheckLocalMove ( vecStart, vecTest, pTargetEnt, NULL ) == LOCALMOVE_VALID )
			{
				outRoute[outCount].iType = iType;
				outRoute[outCount].vecLocation = vecTest;
			}
			else if ( CheckLocalMove ( vecSplit, vecTest, pTargetEnt, NULL ) == LOCALMOVE_VALID )
			{
				outRoute[outCount].iType = iType;
				outRoute[outCount].vecLocation = vecSplit;
				outRoute[outCount+1].iType = iType;
				outRoute[outCount+1].vecLocation = vecTest;
				outCount++; // Adding an extra point
			}
			else
			{
				outRoute[outCount] = m_Route[ m_iRouteIndex + i ];
			}
		}
		// Get last point
		vecStart = outRoute[ outCount ].vecLocation;
		outCount++;
	}
	ASSERT( i < count );
	outRoute[outCount] = m_Route[ m_iRouteIndex + i ];
	outCount++;
	
	// Terminate
	outRoute[outCount].iType = 0;
	ASSERT( outCount < (ROUTE_SIZE*2) );

// Copy the simplified route, disable for testing
	m_iRouteIndex = 0;
	for ( i = 0; i < ROUTE_SIZE && i < outCount; i++ )
	{
		m_Route[i] = outRoute[i];
	}

	// Terminate route
	if ( i < ROUTE_SIZE )
		m_Route[i].iType = 0;

// Debug, test movement code
#if 0
//	if ( CVAR_GET_FLOAT( "simplify" ) != 0 )
		DrawRoute( pev, outRoute, 0, 255, 0, 0 );
//	else
		DrawRoute( pev, m_Route, m_iRouteIndex, 0, 255, 0 );
#endif
}

//=========================================================
// FBecomeProne - tries to send a monster into PRONE state.
// right now only used when a barnacle snatches someone, so 
// may have some special case stuff for that.
//=========================================================
BOOL CBaseMonster :: FBecomeProne ( void )
{
	if ( FBitSet ( pev->flags, FL_ONGROUND ) )
	{
		pev->flags -= FL_ONGROUND;
	}

	m_IdealMonsterState = MONSTERSTATE_PRONE;
	return TRUE;
}

//=========================================================
// CheckRangeAttack1
//=========================================================
BOOL CBaseMonster :: CheckRangeAttack1 ( float flDot, float flDist )
{
	if ( flDist > 64 && flDist <= 784 && flDot >= 0.5 )
	{
		return TRUE;
	}
	return FALSE;
}

//=========================================================
// CheckRangeAttack2
//=========================================================
BOOL CBaseMonster :: CheckRangeAttack2 ( float flDot, float flDist )
{
	if ( flDist > 64 && flDist <= 512 && flDot >= 0.5 )
	{
		return TRUE;
	}
	return FALSE;
}

//=========================================================
// CheckMeleeAttack1
//=========================================================
BOOL CBaseMonster :: CheckMeleeAttack1 ( float flDot, float flDist )
{
	// Decent fix to keep folks from kicking/punching hornets and snarks is to check the onground flag(sjb)
	if ( flDist <= 64 && flDot >= 0.7 && m_hEnemy != NULL && FBitSet ( m_hEnemy->pev->flags, FL_ONGROUND ) )
	{
		return TRUE;
	}
	return FALSE;
}

//=========================================================
// CheckMeleeAttack2
//=========================================================
BOOL CBaseMonster :: CheckMeleeAttack2 ( float flDot, float flDist )
{
	if ( flDist <= 64 && flDot >= 0.7 )
	{
		return TRUE;
	}
	return FALSE;
}

//=========================================================
// CheckAttacks - sets all of the bits for attacks that the
// monster is capable of carrying out on the passed entity.
//=========================================================
void CBaseMonster :: CheckAttacks ( CBaseEntity *pTarget, float flDist )
{
	Vector2D	vec2LOS;
	float		flDot;

	UTIL_MakeVectors ( pev->angles );

	vec2LOS = ( pTarget->pev->origin - pev->origin ).Make2D();
	vec2LOS = vec2LOS.Normalize();

	flDot = DotProduct (vec2LOS , gpGlobals->v_forward.Make2D() );

	// we know the enemy is in front now. We'll find which attacks the monster is capable of by
	// checking for corresponding Activities in the model file, then do the simple checks to validate
	// those attack types.
	
	// Clear all attack conditions
	ClearConditions( bits_COND_CAN_RANGE_ATTACK1 | bits_COND_CAN_RANGE_ATTACK2 | bits_COND_CAN_MELEE_ATTACK1 |bits_COND_CAN_MELEE_ATTACK2 );

	if ( m_afCapability & bits_CAP_RANGE_ATTACK1 )
	{
		if ( CheckRangeAttack1 ( flDot, flDist ) )
			SetConditions( bits_COND_CAN_RANGE_ATTACK1 );
	}
	if ( m_afCapability & bits_CAP_RANGE_ATTACK2 )
	{
		if ( CheckRangeAttack2 ( flDot, flDist ) )
			SetConditions( bits_COND_CAN_RANGE_ATTACK2 );
	}
	if ( m_afCapability & bits_CAP_MELEE_ATTACK1 )
	{
		if ( CheckMeleeAttack1 ( flDot, flDist ) )
			SetConditions( bits_COND_CAN_MELEE_ATTACK1 );
	}
	if ( m_afCapability & bits_CAP_MELEE_ATTACK2 )
	{
		if ( CheckMeleeAttack2 ( flDot, flDist ) )
			SetConditions( bits_COND_CAN_MELEE_ATTACK2 );
	}
}

//=========================================================
// CanCheckAttacks - prequalifies a monster to do more fine
// checking of potential attacks. 
//=========================================================
BOOL CBaseMonster :: FCanCheckAttacks ( void )
{
	if ( HasConditions(bits_COND_SEE_ENEMY) && !HasConditions( bits_COND_ENEMY_TOOFAR ) )
	{
		return TRUE;
	}

	return FALSE;
}

//=========================================================
// CheckEnemy - part of the Condition collection process,
// gets and stores data and conditions pertaining to a monster's
// enemy. Returns TRUE if Enemy LKP was updated.
//=========================================================
int CBaseMonster :: CheckEnemy ( CBaseEntity *pEnemy )
{
	float	flDistToEnemy;
	int		iUpdatedLKP;// set this to TRUE if you update the EnemyLKP in this function.

	iUpdatedLKP = FALSE;
	ClearConditions ( bits_COND_ENEMY_FACING_ME );
	
	if ( !FVisible( pEnemy ) )
	{
		ASSERT(!HasConditions(bits_COND_SEE_ENEMY));
		SetConditions( bits_COND_ENEMY_OCCLUDED );
	}
	else
		ClearConditions( bits_COND_ENEMY_OCCLUDED );

	if ( !pEnemy->IsAlive() )
	{
		SetConditions ( bits_COND_ENEMY_DEAD );
		ClearConditions( bits_COND_SEE_ENEMY | bits_COND_ENEMY_OCCLUDED );
		return FALSE;
	}

	Vector vecEnemyPos = pEnemy->pev->origin;
	// distance to enemy's origin
	flDistToEnemy = ( vecEnemyPos - pev->origin ).Length();
	vecEnemyPos.z += pEnemy->pev->size.z * 0.5;
	// distance to enemy's head
	float flDistToEnemy2 = (vecEnemyPos - pev->origin).Length();
	if (flDistToEnemy2 < flDistToEnemy)
		flDistToEnemy = flDistToEnemy2;
	else
	{
		// distance to enemy's feet
		vecEnemyPos.z -= pEnemy->pev->size.z;
		float flDistToEnemy2 = (vecEnemyPos - pev->origin).Length();
		if (flDistToEnemy2 < flDistToEnemy)
			flDistToEnemy = flDistToEnemy2;
	}

	if ( HasConditions( bits_COND_SEE_ENEMY ) )
	{
		CBaseMonster *pEnemyMonster;

		iUpdatedLKP = TRUE;
		m_vecEnemyLKP = pEnemy->pev->origin;

		pEnemyMonster = pEnemy->MyMonsterPointer();

		if ( pEnemyMonster )
		{
			if ( pEnemyMonster->FInViewCone ( this ) )
			{
				SetConditions ( bits_COND_ENEMY_FACING_ME );
			}
			else
				ClearConditions( bits_COND_ENEMY_FACING_ME );
		}

		if (pEnemy->pev->velocity != Vector( 0, 0, 0))
		{
			// trail the enemy a bit
			m_vecEnemyLKP = m_vecEnemyLKP - pEnemy->pev->velocity * RANDOM_FLOAT( -0.05, 0 );
		}
		else
		{
			// UNDONE: use pev->oldorigin?
		}
	}
	else if ( !HasConditions(bits_COND_ENEMY_OCCLUDED|bits_COND_SEE_ENEMY) && ( flDistToEnemy <= 256 ) )
	{
		// if the enemy is not occluded, and unseen, that means it is behind or beside the monster.
		// if the enemy is near enough the monster, we go ahead and let the monster know where the
		// enemy is. 
		iUpdatedLKP = TRUE;
		m_vecEnemyLKP = pEnemy->pev->origin;
	}

	if ( flDistToEnemy >= m_flDistTooFar )
	{
		// enemy is very far away from monster
		SetConditions( bits_COND_ENEMY_TOOFAR );
	}
	else
		ClearConditions( bits_COND_ENEMY_TOOFAR );

	if ( FCanCheckAttacks() )	
	{
		CheckAttacks ( m_hEnemy, flDistToEnemy );
	}

	if ( m_movementGoal == MOVEGOAL_ENEMY )
	{
		for ( int i = m_iRouteIndex; i < ROUTE_SIZE; i++ )
		{
			if ( m_Route[ i ].iType == (bits_MF_IS_GOAL|bits_MF_TO_ENEMY) )
			{
				// UNDONE: Should we allow monsters to override this distance (80?)
				if ( (m_Route[ i ].vecLocation - m_vecEnemyLKP).Length() > 80 )
				{
					// Refresh
					FRefreshRoute();
					return iUpdatedLKP;
				}
			}
		}
	}

	return iUpdatedLKP;
}

//=========================================================
// PushEnemy - remember the last few enemies, always remember the player
//=========================================================
void CBaseMonster :: PushEnemy( CBaseEntity *pEnemy, const Vector &vecLastKnownPos )
{
	int i;

	if (pEnemy == NULL)
		return;

	// UNDONE: blah, this is bad, we should use a stack but I'm too lazy to code one.
	for (i = 0; i < MAX_OLD_ENEMIES; i++)
	{
		if (m_hOldEnemy[i] == pEnemy)
			return;
		if (m_hOldEnemy[i] == NULL) // someone died, reuse their slot
			break;
	}
	if (i >= MAX_OLD_ENEMIES)
		return;

	m_hOldEnemy[i] = pEnemy;
	m_vecOldEnemy[i] = vecLastKnownPos;
}

//=========================================================
// PopEnemy - try remembering the last few enemies
//=========================================================
BOOL CBaseMonster :: PopEnemy( )
{
	// UNDONE: blah, this is bad, we should use a stack but I'm too lazy to code one.
	for (int i = MAX_OLD_ENEMIES - 1; i >= 0; i--)
	{
		if (m_hOldEnemy[i] != NULL)
		{
			if (m_hOldEnemy[i]->IsAlive( )) // cheat and know when they die
			{
				m_hEnemy = m_hOldEnemy[i];
				m_vecEnemyLKP = m_vecOldEnemy[i];
				// ALERT( at_console, "remembering\n");
				return TRUE;
			}
			else
			{
				m_hOldEnemy[i] = NULL;
			}
		}
	}
	return FALSE;
}

//=========================================================
// SetActivity 
//=========================================================
void CBaseMonster :: SetActivity ( Activity NewActivity )
{
	int	iSequence;

	iSequence = LookupActivity ( NewActivity );

	// Set to the desired anim, or default anim if the desired is not present
	if ( iSequence > ACTIVITY_NOT_AVAILABLE )
	{
		if ( pev->sequence != iSequence || !m_fSequenceLoops )
		{
			// don't reset frame between walk and run
			if ( !(m_Activity == ACT_WALK || m_Activity == ACT_RUN) || !(NewActivity == ACT_WALK || NewActivity == ACT_RUN))
				pev->frame = 0;
		}

		pev->sequence		= iSequence;	// Set to the reset anim (if it's there)
		ResetSequenceInfo( );
		SetYawSpeed();
	}
	else
	{
		// Not available try to get default anim
		ALERT ( at_aiconsole, "%s has no sequence for act:%d\n", STRING(pev->classname), NewActivity );
		pev->sequence		= 0;	// Set to the reset anim (if it's there)
	}

	m_Activity = NewActivity; // Go ahead and set this so it doesn't keep trying when the anim is not present
	
	// In case someone calls this with something other than the ideal activity
	m_IdealActivity = m_Activity;


}

//=========================================================
// SetSequenceByName
//=========================================================
void CBaseMonster :: SetSequenceByName ( char *szSequence )
{
	int	iSequence;

	iSequence = LookupSequence ( szSequence );

	// Set to the desired anim, or default anim if the desired is not present
	if ( iSequence > ACTIVITY_NOT_AVAILABLE )
	{
		if ( pev->sequence != iSequence || !m_fSequenceLoops )
		{
			pev->frame = 0;
		}

		pev->sequence		= iSequence;	// Set to the reset anim (if it's there)
		ResetSequenceInfo( );
		SetYawSpeed();
	}
	else
	{
		// Not available try to get default anim
		ALERT ( at_aiconsole, "%s has no sequence named:%f\n", STRING(pev->classname), szSequence );
		pev->sequence		= 0;	// Set to the reset anim (if it's there)
	}
}

//=========================================================
// CheckLocalMove - returns TRUE if the caller can walk a 
// straight line from its current origin to the given 
// location. If so, don't use the node graph!
//
// if a valid pointer to a int is passed, the function
// will fill that int with the distance that the check 
// reached before hitting something. THIS ONLY HAPPENS
// IF THE LOCAL MOVE CHECK FAILS!
//
// !!!PERFORMANCE - should we try to load balance this?
// DON"T USE SETORIGIN! 
//=========================================================
#define	LOCAL_STEP_SIZE	16
int CBaseMonster :: CheckLocalMove ( const Vector &vecStart, const Vector &vecEnd, CBaseEntity *pTarget, float *pflDist )
{
	Vector	vecStartPos;// record monster's position before trying the move
	float	flYaw;
	float	flDist;
	float	flStep, stepSize;
	int		iReturn;

	vecStartPos = pev->origin;
	
	
	flYaw = UTIL_VecToYaw ( vecEnd - vecStart );// build a yaw that points to the goal.
	flDist = ( vecEnd - vecStart ).Length2D();// get the distance.
	iReturn = LOCALMOVE_VALID;// assume everything will be ok.

	// move the monster to the start of the local move that's to be checked.
	UTIL_SetOrigin( pev, vecStart );// !!!BUGBUG - won't this fire triggers? - nope, SetOrigin doesn't fire

	if ( !(pev->flags & (FL_FLY|FL_SWIM)) )
	{
		DROP_TO_FLOOR( ENT( pev ) );//make sure monster is on the floor!
	}

	//pev->origin.z = vecStartPos.z;//!!!HACKHACK

//	pev->origin = vecStart;

/*
	if ( flDist > 1024 )
	{
		// !!!PERFORMANCE - this operation may be too CPU intensive to try checks this large.
		// We don't lose much here, because a distance this great is very likely
		// to have something in the way.

		// since we've actually moved the monster during the check, undo the move.
		pev->origin = vecStartPos;
		return FALSE;
	}
*/
	// this loop takes single steps to the goal.
	for ( flStep = 0 ; flStep < flDist ; flStep += LOCAL_STEP_SIZE )
	{
		stepSize = LOCAL_STEP_SIZE;

		if ( (flStep + LOCAL_STEP_SIZE) >= (flDist-1) )
			stepSize = (flDist - flStep) - 1;
		
//		UTIL_ParticleEffect ( pev->origin, g_vecZero, 255, 25 );

		if ( !WALK_MOVE( ENT(pev), flYaw, stepSize, WALKMOVE_CHECKONLY ) )
		{// can't take the next step, fail!

			if ( pflDist != NULL )
			{
				*pflDist = flStep;
			}
			if ( pTarget && pTarget->edict() == gpGlobals->trace_ent )
			{
				// if this step hits target ent, the move is legal.
				iReturn = LOCALMOVE_VALID;
				break;
			}
			else
			{
				// If we're going toward an entity, and we're almost getting there, it's OK.
//				if ( pTarget && fabs( flDist - iStep ) < LOCAL_STEP_SIZE )
//					fReturn = TRUE;
//				else
				iReturn = LOCALMOVE_INVALID;
				break;
			}

		}
	}

	if ( iReturn == LOCALMOVE_VALID && 	!(pev->flags & (FL_FLY|FL_SWIM) ) && (!pTarget || (pTarget->pev->flags & FL_ONGROUND)) )
	{
		// The monster can move to a spot UNDER the target, but not to it. Don't try to triangulate, go directly to the node graph.
		// UNDONE: Magic # 64 -- this used to be pev->size.z but that won't work for small creatures like the headcrab
		if ( fabs(vecEnd.z - pev->origin.z) > 64 )
		{
			iReturn = LOCALMOVE_INVALID_DONT_TRIANGULATE;
		}
	}
	/*
	// uncommenting this block will draw a line representing the nearest legal move.
	WRITE_BYTE(MSG_BROADCAST, SVC_TEMPENTITY);
	WRITE_BYTE(MSG_BROADCAST, TE_SHOWLINE);
	WRITE_COORD(MSG_BROADCAST, pev->origin.x);
	WRITE_COORD(MSG_BROADCAST, pev->origin.y);
	WRITE_COORD(MSG_BROADCAST, pev->origin.z);
	WRITE_COORD(MSG_BROADCAST, vecStart.x);
	WRITE_COORD(MSG_BROADCAST, vecStart.y);
	WRITE_COORD(MSG_BROADCAST, vecStart.z);
	*/

	// since we've actually moved the monster during the check, undo the move.
	UTIL_SetOrigin( pev, vecStartPos );

	return iReturn;
}


float CBaseMonster :: OpenDoorAndWait( entvars_t *pevDoor )
{
	float flTravelTime = 0;

	//ALERT(at_aiconsole, "A door. ");
	CBaseEntity *pcbeDoor = CBaseEntity::Instance(pevDoor);
	if (pcbeDoor && !pcbeDoor->IsLockedByMaster())
	{
		//ALERT(at_aiconsole, "unlocked! ");
		pcbeDoor->Use(this, this, USE_ON, 0.0);
		//ALERT(at_aiconsole, "pevDoor->nextthink = %d ms\n", (int)(1000*pevDoor->nextthink));
		//ALERT(at_aiconsole, "pevDoor->ltime = %d ms\n", (int)(1000*pevDoor->ltime));
		//ALERT(at_aiconsole, "pev-> nextthink = %d ms\n", (int)(1000*pev->nextthink));
		//ALERT(at_aiconsole, "pev->ltime = %d ms\n", (int)(1000*pev->ltime));
		flTravelTime = pevDoor->nextthink - pevDoor->ltime;
		//ALERT(at_aiconsole, "Waiting %d ms\n", (int)(1000*flTravelTime));
		if ( pcbeDoor->pev->targetname )
		{
			edict_t *pentTarget = NULL;
			for (;;)
			{
				pentTarget = FIND_ENTITY_BY_TARGETNAME( pentTarget, STRING(pcbeDoor->pev->targetname));

				if ( VARS( pentTarget ) != pcbeDoor->pev )
				{
					if (FNullEnt(pentTarget))
						break;

					if ( FClassnameIs ( pentTarget, STRING(pcbeDoor->pev->classname) ) )
					{
						CBaseEntity *pDoor = Instance(pentTarget);
						if ( pDoor )
							pDoor->Use(this, this, USE_ON, 0.0);
					}
				}
			}
		}
	}

	return gpGlobals->time + flTravelTime;
}


//=========================================================
// AdvanceRoute - poorly named function that advances the 
// m_iRouteIndex. If it goes beyond ROUTE_SIZE, the route 
// is refreshed. 
//=========================================================
void CBaseMonster :: AdvanceRoute ( float distance )
{

	if ( m_iRouteIndex == ROUTE_SIZE - 1 )
	{
		// time to refresh the route.
		if ( !FRefreshRoute() )
		{
			ALERT ( at_aiconsole, "Can't Refresh Route!!\n" );
		}
	}
	else
	{
		if ( ! (m_Route[ m_iRouteIndex ].iType & bits_MF_IS_GOAL) )
		{
			// If we've just passed a path_corner, advance m_pGoalEnt
			if ( (m_Route[ m_iRouteIndex ].iType & ~bits_MF_NOT_TO_MASK) == bits_MF_TO_PATHCORNER )
				m_pGoalEnt = m_pGoalEnt->GetNextTarget();

			// IF both waypoints are nodes, then check for a link for a door and operate it.
			//
			if (  (m_Route[m_iRouteIndex].iType   & bits_MF_TO_NODE) == bits_MF_TO_NODE
			   && (m_Route[m_iRouteIndex+1].iType & bits_MF_TO_NODE) == bits_MF_TO_NODE)
			{
				//ALERT(at_aiconsole, "SVD: Two nodes. ");

				int iSrcNode  = WorldGraph.FindNearestNode(m_Route[m_iRouteIndex].vecLocation, this );
				int iDestNode = WorldGraph.FindNearestNode(m_Route[m_iRouteIndex+1].vecLocation, this );

				int iLink;
				WorldGraph.HashSearch(iSrcNode, iDestNode, iLink);

				if ( iLink >= 0 && WorldGraph.m_pLinkPool[iLink].m_pLinkEnt != NULL )
				{
					//ALERT(at_aiconsole, "A link. ");
					if ( WorldGraph.HandleLinkEnt ( iSrcNode, WorldGraph.m_pLinkPool[iLink].m_pLinkEnt, m_afCapability, CGraph::NODEGRAPH_DYNAMIC ) )
					{
						//ALERT(at_aiconsole, "usable.");
						entvars_t *pevDoor = WorldGraph.m_pLinkPool[iLink].m_pLinkEnt;
						if (pevDoor)
						{
							m_flMoveWaitFinished = OpenDoorAndWait( pevDoor );
//							ALERT( at_aiconsole, "Wating for door %.2f\n", m_flMoveWaitFinished-gpGlobals->time );
						}
					}
				}
				//ALERT(at_aiconsole, "\n");
			}
			m_iRouteIndex++;
		}
		else	// At goal!!!
		{
			if ( distance < m_flGroundSpeed * 0.2 /* FIX */ )
			{
				MovementComplete();
			}
		}
	}
}


int CBaseMonster :: RouteClassify( int iMoveFlag )
{
	int movementGoal;

	movementGoal = MOVEGOAL_NONE;

	if ( iMoveFlag & bits_MF_TO_TARGETENT )
		movementGoal = MOVEGOAL_TARGETENT;
	else if ( iMoveFlag & bits_MF_TO_ENEMY )
		movementGoal = MOVEGOAL_ENEMY;
	else if ( iMoveFlag & bits_MF_TO_PATHCORNER )
		movementGoal = MOVEGOAL_PATHCORNER;
	else if ( iMoveFlag & bits_MF_TO_NODE )
		movementGoal = MOVEGOAL_NODE;
	else if ( iMoveFlag & bits_MF_TO_LOCATION )
		movementGoal = MOVEGOAL_LOCATION;

	return movementGoal;
}

//=========================================================
// BuildRoute
//=========================================================
BOOL CBaseMonster :: BuildRoute ( const Vector &vecGoal, int iMoveFlag, CBaseEntity *pTarget )
{
	float	flDist;
	Vector	vecApex;
	int		iLocalMove;

	RouteNew();
	m_movementGoal = RouteClassify( iMoveFlag );

// so we don't end up with no moveflags
	m_Route[ 0 ].vecLocation = vecGoal;
	m_Route[ 0 ].iType = iMoveFlag | bits_MF_IS_GOAL;

// check simple local move
	iLocalMove = CheckLocalMove( pev->origin, vecGoal, pTarget, &flDist );

	if ( iLocalMove == LOCALMOVE_VALID )
	{
		// monster can walk straight there!
		return TRUE;
	}
// try to triangulate around any obstacles.
	else if ( iLocalMove != LOCALMOVE_INVALID_DONT_TRIANGULATE && FTriangulate( pev->origin, vecGoal, flDist, pTarget, &vecApex ) )
	{
		// there is a slightly more complicated path that allows the monster to reach vecGoal
		m_Route[ 0 ].vecLocation = vecApex;
		m_Route[ 0 ].iType = (iMoveFlag | bits_MF_TO_DETOUR);

		m_Route[ 1 ].vecLocation = vecGoal;
		m_Route[ 1 ].iType = iMoveFlag | bits_MF_IS_GOAL;

			/*
			WRITE_BYTE(MSG_BROADCAST, SVC_TEMPENTITY);
			WRITE_BYTE(MSG_BROADCAST, TE_SHOWLINE);
			WRITE_COORD(MSG_BROADCAST, vecApex.x );
			WRITE_COORD(MSG_BROADCAST, vecApex.y );
			WRITE_COORD(MSG_BROADCAST, vecApex.z );
			WRITE_COORD(MSG_BROADCAST, vecApex.x );
			WRITE_COORD(MSG_BROADCAST, vecApex.y );
			WRITE_COORD(MSG_BROADCAST, vecApex.z + 128 );
			*/

		RouteSimplify( pTarget );
		return TRUE;
	}

// last ditch, try nodes
	if ( FGetNodeRoute( vecGoal ) )
	{
//		ALERT ( at_console, "Can get there on nodes\n" );
		m_vecMoveGoal = vecGoal;
		RouteSimplify( pTarget );
		return TRUE;
	}

	// b0rk
	return FALSE;
}


//=========================================================
// InsertWaypoint - Rebuilds the existing route so that the
// supplied vector and moveflags are the first waypoint in
// the route, and fills the rest of the route with as much
// of the pre-existing route as possible
//=========================================================
void CBaseMonster :: InsertWaypoint ( Vector vecLocation, int afMoveFlags )
{
	int			i, type;

	
	// we have to save some Index and Type information from the real
	// path_corner or node waypoint that the monster was trying to reach. This makes sure that data necessary 
	// to refresh the original path exists even in the new waypoints that don't correspond directy to a path_corner
	// or node. 
	type = afMoveFlags | (m_Route[ m_iRouteIndex ].iType & ~bits_MF_NOT_TO_MASK);

	for ( i = ROUTE_SIZE-1; i > 0; i-- )
		m_Route[i] = m_Route[i-1];

	m_Route[ m_iRouteIndex ].vecLocation = vecLocation;
	m_Route[ m_iRouteIndex ].iType = type;
}

Vector CrossProduct( const Vector& a, const Vector& b )
{
	return Vector( a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x );
}

//=========================================================
// FTriangulate - tries to overcome local obstacles by 
// triangulating a path around them.
//
// iApexDist is how far the obstruction that we are trying
// to triangulate around is from the monster.
//=========================================================
BOOL CBaseMonster :: FTriangulate ( const Vector &vecStart , const Vector &vecEnd, float flDist, CBaseEntity *pTargetEnt, Vector *pApex )
{
	Vector		vecDir;
	Vector		vecForward;
	Vector		vecLeft;// the spot we'll try to triangulate to on the left
	Vector		vecRight;// the spot we'll try to triangulate to on the right
	Vector		vecTop;// the spot we'll try to triangulate to on the top
	Vector		vecBottom;// the spot we'll try to triangulate to on the bottom
	Vector		vecFarSide;// the spot that we'll move to after hitting the triangulated point, before moving on to our normal goal.
	int			i;
	float		sizeX, sizeZ;

	// If the hull width is less than 24, use 24 because CheckLocalMove uses a min of
	// 24.
	sizeX = pev->size.x;
	if (sizeX < 24.0)
		sizeX = 24.0;
	else if (sizeX > 48.0)
		sizeX = 48.0;
	sizeZ = pev->size.z;
	//if (sizeZ < 24.0)
	//	sizeZ = 24.0;

	vecForward = ( vecEnd - vecStart ).Normalize();

	Vector vecDirUp(0,0,1);
	vecDir = CrossProduct ( vecForward, vecDirUp);

	// start checking right about where the object is, picking two equidistant starting points, one on
	// the left, one on the right. As we progress through the loop, we'll push these away from the obstacle, 
	// hoping to find a way around on either side. pev->size.x is added to the ApexDist in order to help select
	// an apex point that insures that the monster is sufficiently past the obstacle before trying to turn back
	// onto its original course.

	vecLeft = pev->origin + ( vecForward * ( flDist + sizeX ) ) - vecDir * ( sizeX * 3 );
	vecRight = pev->origin + ( vecForward * ( flDist + sizeX ) ) + vecDir * ( sizeX * 3 );
	if (pev->movetype == MOVETYPE_FLY)
	{
		vecTop = pev->origin + (vecForward * flDist) + (vecDirUp * sizeZ * 3);
		vecBottom = pev->origin + (vecForward * flDist) - (vecDirUp *  sizeZ * 3);
	}

	vecFarSide = m_Route[ m_iRouteIndex ].vecLocation;
	
	vecDir = vecDir * sizeX * 2;
	if (pev->movetype == MOVETYPE_FLY)
		vecDirUp = vecDirUp * sizeZ * 2;

	for ( i = 0 ; i < 8; i++ )
	{
// Debug, Draw the triangulation
#if 0
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE( TE_SHOWLINE);
			WRITE_COORD( pev->origin.x );
			WRITE_COORD( pev->origin.y );
			WRITE_COORD( pev->origin.z );
			WRITE_COORD( vecRight.x );
			WRITE_COORD( vecRight.y );
			WRITE_COORD( vecRight.z );
		MESSAGE_END();

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE( TE_SHOWLINE );
			WRITE_COORD( pev->origin.x );
			WRITE_COORD( pev->origin.y );
			WRITE_COORD( pev->origin.z );
			WRITE_COORD( vecLeft.x );
			WRITE_COORD( vecLeft.y );
			WRITE_COORD( vecLeft.z );
		MESSAGE_END();
#endif

#if 0
		if (pev->movetype == MOVETYPE_FLY)
		{
			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
				WRITE_BYTE( TE_SHOWLINE );
				WRITE_COORD( pev->origin.x );
				WRITE_COORD( pev->origin.y );
				WRITE_COORD( pev->origin.z );
				WRITE_COORD( vecTop.x );
				WRITE_COORD( vecTop.y );
				WRITE_COORD( vecTop.z );
			MESSAGE_END();

			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
				WRITE_BYTE( TE_SHOWLINE );
				WRITE_COORD( pev->origin.x );
				WRITE_COORD( pev->origin.y );
				WRITE_COORD( pev->origin.z );
				WRITE_COORD( vecBottom.x );
				WRITE_COORD( vecBottom.y );
				WRITE_COORD( vecBottom.z );
			MESSAGE_END();
		}
#endif

		if ( CheckLocalMove( pev->origin, vecRight, pTargetEnt, NULL ) == LOCALMOVE_VALID )
		{
			if ( CheckLocalMove ( vecRight, vecFarSide, pTargetEnt, NULL ) == LOCALMOVE_VALID )
			{
				if ( pApex )
				{
					*pApex = vecRight;
				}

				return TRUE;
			}
		}
		if ( CheckLocalMove( pev->origin, vecLeft, pTargetEnt, NULL ) == LOCALMOVE_VALID )
		{
			if ( CheckLocalMove ( vecLeft, vecFarSide, pTargetEnt, NULL ) == LOCALMOVE_VALID )
			{
				if ( pApex )
				{
					*pApex = vecLeft;
				}

				return TRUE;
			}
		}

		if (pev->movetype == MOVETYPE_FLY)
		{
			if ( CheckLocalMove( pev->origin, vecTop, pTargetEnt, NULL ) == LOCALMOVE_VALID)
			{
				if ( CheckLocalMove ( vecTop, vecFarSide, pTargetEnt, NULL ) == LOCALMOVE_VALID )
				{
					if ( pApex )
					{
						*pApex = vecTop;
						//ALERT(at_aiconsole, "triangulate over\n");
					}

					return TRUE;
				}
			}
#if 1
			if ( CheckLocalMove( pev->origin, vecBottom, pTargetEnt, NULL ) == LOCALMOVE_VALID )
			{
				if ( CheckLocalMove ( vecBottom, vecFarSide, pTargetEnt, NULL ) == LOCALMOVE_VALID )
				{
					if ( pApex )
					{
						*pApex = vecBottom;
						//ALERT(at_aiconsole, "triangulate under\n");
					}

					return TRUE;
				}
			}
#endif
		}

		vecRight = vecRight + vecDir;
		vecLeft = vecLeft - vecDir;
		if (pev->movetype == MOVETYPE_FLY)
		{
			vecTop = vecTop + vecDirUp;
			vecBottom = vecBottom - vecDirUp;
		}
	}

	return FALSE;
}

//=========================================================
// Move - take a single step towards the next ROUTE location
//=========================================================
#define DIST_TO_CHECK	200

void CBaseMonster :: Move ( float flInterval ) 
{
	float		flWaypointDist;
	float		flCheckDist;
	float		flDist;// how far the lookahead check got before hitting an object.
	Vector		vecDir;
	Vector		vecApex;
	CBaseEntity	*pTargetEnt;

	// Don't move if no valid route
	if ( FRouteClear() )
	{
		// If we still have a movement goal, then this is probably a route truncated by SimplifyRoute()
		// so refresh it.
		if ( m_movementGoal == MOVEGOAL_NONE || !FRefreshRoute() )
		{
			ALERT( at_aiconsole, "Tried to move with no route!\n" );
			TaskFail();
			return;
		}
	}
	
	if ( m_flMoveWaitFinished > gpGlobals->time )
		return;

// Debug, test movement code
#if 0
//	if ( CVAR_GET_FLOAT("stopmove" ) != 0 )
	{
		if ( m_movementGoal == MOVEGOAL_ENEMY )
			RouteSimplify( m_hEnemy );
		else
			RouteSimplify( m_hTargetEnt );
		FRefreshRoute();
		return;
	}
#else
// Debug, draw the route
//	DrawRoute( pev, m_Route, m_iRouteIndex, 0, 200, 0 );
#endif

	// if the monster is moving directly towards an entity (enemy for instance), we'll set this pointer
	// to that entity for the CheckLocalMove and Triangulate functions.
	pTargetEnt = NULL;

	// local move to waypoint.
	vecDir = ( m_Route[ m_iRouteIndex ].vecLocation - pev->origin ).Normalize();
	flWaypointDist = ( m_Route[ m_iRouteIndex ].vecLocation - pev->origin ).Length2D();
	
	MakeIdealYaw ( m_Route[ m_iRouteIndex ].vecLocation );
	ChangeYaw ( pev->yaw_speed );

	// if the waypoint is closer than CheckDist, CheckDist is the dist to waypoint
	if ( flWaypointDist < DIST_TO_CHECK )
	{
		flCheckDist = flWaypointDist;
	}
	else
	{
		flCheckDist = DIST_TO_CHECK;
	}
	
	if ( (m_Route[ m_iRouteIndex ].iType & (~bits_MF_NOT_TO_MASK)) == bits_MF_TO_ENEMY )
	{
		// only on a PURE move to enemy ( i.e., ONLY MF_TO_ENEMY set, not MF_TO_ENEMY and DETOUR )
		pTargetEnt = m_hEnemy;
	}
	else if ( (m_Route[ m_iRouteIndex ].iType & ~bits_MF_NOT_TO_MASK) == bits_MF_TO_TARGETENT )
	{
		pTargetEnt = m_hTargetEnt;
	}

	// !!!BUGBUG - CheckDist should be derived from ground speed.
	// If this fails, it should be because of some dynamic entity blocking this guy.
	// We've already checked this path, so we should wait and time out if the entity doesn't move
	flDist = 0;
	if ( CheckLocalMove ( pev->origin, pev->origin + vecDir * flCheckDist, pTargetEnt, &flDist ) != LOCALMOVE_VALID )
	{
		CBaseEntity *pBlocker;

		// Can't move, stop
		Stop();
		// Blocking entity is in global trace_ent
		pBlocker = CBaseEntity::Instance( gpGlobals->trace_ent );
		if (pBlocker)
		{
			DispatchBlocked( edict(), pBlocker->edict() );
		}

		if ( pBlocker && m_moveWaitTime > 0 && pBlocker->IsMoving() && !(pBlocker->pev->flags & FL_CLIENT) && (gpGlobals->time-m_flMoveWaitFinished) > 3.0 )
		{
			// Can we still move toward our target?
			if ( flDist < m_flGroundSpeed )
			{
				// No, Wait for a second
				m_flMoveWaitFinished = gpGlobals->time + m_moveWaitTime;
				return;
			}
			// Ok, still enough room to take a step
		}
		else 
		{
			// try to triangulate around whatever is in the way.
			if ( FTriangulate( pev->origin, m_Route[ m_iRouteIndex ].vecLocation, flDist, pTargetEnt, &vecApex ) )
			{
				InsertWaypoint( vecApex, bits_MF_TO_DETOUR );
				RouteSimplify( pTargetEnt );
			}
			else
			{
//				ALERT ( at_aiconsole, "Couldn't Triangulate\n" );
				Stop();
				// Only do this once until your route is cleared
				if ( m_moveWaitTime > 0 && !(m_afMemory & bits_MEMORY_MOVE_FAILED) )
				{
					FRefreshRoute();
					if ( FRouteClear() )
					{
						TaskFail();
					}
					else
					{
						// Don't get stuck
						if ( (gpGlobals->time - m_flMoveWaitFinished) < 0.2 )
							Remember( bits_MEMORY_MOVE_FAILED );

						m_flMoveWaitFinished = gpGlobals->time + 0.1;
					}
				}
				else
				{
					TaskFail();
					ALERT( at_aiconsole, "%s Failed to move (%d)!\n", STRING(pev->classname), HasMemory( bits_MEMORY_MOVE_FAILED ) );
					//ALERT( at_aiconsole, "%f, %f, %f\n", pev->origin.z, (pev->origin + (vecDir * flCheckDist)).z, m_Route[m_iRouteIndex].vecLocation.z );
				}
				return;
			}
		}
	}

	// close enough to the target, now advance to the next target. This is done before actually reaching
	// the target so that we get a nice natural turn while moving.
	if ( ShouldAdvanceRoute( flWaypointDist ) )///!!!BUGBUG- magic number
	{
		AdvanceRoute( flWaypointDist );
	}

	// Might be waiting for a door
	if ( m_flMoveWaitFinished > gpGlobals->time )
	{
		Stop();
		return;
	}

	// UNDONE: this is a hack to quit moving farther than it has looked ahead.
	if (flCheckDist < m_flGroundSpeed * flInterval)
	{
		flInterval = flCheckDist / m_flGroundSpeed;
		// ALERT( at_console, "%.02f\n", flInterval );
	}
	MoveExecute( pTargetEnt, vecDir, flInterval );

	if ( MovementIsComplete() )
	{
		Stop();
		RouteClear();
	}
}


BOOL CBaseMonster:: ShouldAdvanceRoute( float flWaypointDist )
{
	if ( flWaypointDist <= MONSTER_CUT_CORNER_DIST )
	{
		// ALERT( at_console, "cut %f\n", flWaypointDist );
		return TRUE;
	}

	return FALSE;
}


void CBaseMonster::MoveExecute( CBaseEntity *pTargetEnt, const Vector &vecDir, float flInterval )
{
//	float flYaw = UTIL_VecToYaw ( m_Route[ m_iRouteIndex ].vecLocation - pev->origin );// build a yaw that points to the goal.
//	WALK_MOVE( ENT(pev), flYaw, m_flGroundSpeed * flInterval, WALKMOVE_NORMAL );
	if ( m_IdealActivity != m_movementActivity )
		m_IdealActivity = m_movementActivity;

	float flTotal = m_flGroundSpeed * pev->framerate * flInterval;
	float flStep;
	while (flTotal > 0.001)
	{
		// don't walk more than 16 units or stairs stop working
		flStep = V_min( 16.0, flTotal );
		UTIL_MoveToOrigin ( ENT(pev), m_Route[ m_iRouteIndex ].vecLocation, flStep, MOVE_NORMAL );
		flTotal -= flStep;
	}
	// ALERT( at_console, "dist %f\n", m_flGroundSpeed * pev->framerate * flInterval );
}


//=========================================================
// MonsterInit - after a monster is spawned, it needs to 
// be dropped into the world, checked for mobility problems,
// and put on the proper path, if any. This function does
// all of those things after the monster spawns. Any
// initialization that should take place for all monsters
// goes here.
//=========================================================
void CBaseMonster :: MonsterInit ( void )
{
	if (gpGlobals->deathmatch)
	{
		REMOVE_ENTITY(ENT(pev));
		return;
	}

	// Set fields common to all monsters
	pev->effects		= 0;
	pev->takedamage		= DAMAGE_AIM;
	pev->ideal_yaw		= pev->angles.y;
	pev->max_health		= pev->health;
	pev->deadflag		= DEAD_NO;
	m_IdealMonsterState	= MONSTERSTATE_IDLE;// Assume monster will be idle, until proven otherwise

	m_IdealActivity = ACT_IDLE;

	SetBits (pev->flags, FL_MONSTER);
	if ( pev->spawnflags & SF_MONSTER_HITMONSTERCLIP )
		pev->flags |= FL_MONSTERCLIP;
	
	ClearSchedule();
	RouteClear();
	InitBoneControllers( ); // FIX: should be done in Spawn

	m_iHintNode			= NO_NODE;

	m_afMemory			= MEMORY_CLEAR;

	m_hEnemy			= NULL;

	m_flDistTooFar		= 1024.0;
	m_flDistLook		= 2048.0;

	// set eye position
	SetEyePosition();

	SetThink( &CBaseMonster::MonsterInitThink );
	pev->nextthink = gpGlobals->time + 0.1;
	SetUse ( &CBaseMonster::MonsterUse );
}

//=========================================================
// MonsterInitThink - Calls StartMonster. Startmonster is 
// virtual, but this function cannot be 
//=========================================================
void CBaseMonster :: MonsterInitThink ( void )
{
	StartMonster();
}

//=========================================================
// StartMonster - final bit of initization before a monster 
// is turned over to the AI. 
//=========================================================
void CBaseMonster :: StartMonster ( void )
{
	// update capabilities
	if ( LookupActivity ( ACT_RANGE_ATTACK1 ) != ACTIVITY_NOT_AVAILABLE )
	{
		m_afCapability |= bits_CAP_RANGE_ATTACK1;
	}
	if ( LookupActivity ( ACT_RANGE_ATTACK2 ) != ACTIVITY_NOT_AVAILABLE )
	{
		m_afCapability |= bits_CAP_RANGE_ATTACK2;
	}
	if ( LookupActivity ( ACT_MELEE_ATTACK1 ) != ACTIVITY_NOT_AVAILABLE )
	{
		m_afCapability |= bits_CAP_MELEE_ATTACK1;
	}
	if ( LookupActivity ( ACT_MELEE_ATTACK2 ) != ACTIVITY_NOT_AVAILABLE )
	{
		m_afCapability |= bits_CAP_MELEE_ATTACK2;
	}

	// Raise monster off the floor one unit, then drop to floor
	if ( pev->movetype != MOVETYPE_FLY && !FBitSet( pev->spawnflags, SF_MONSTER_FALL_TO_GROUND ) )
	{
		pev->origin.z += 1;
		DROP_TO_FLOOR ( ENT(pev) );
		// Try to move the monster to make sure it's not stuck in a brush.
		if (!WALK_MOVE ( ENT(pev), 0, 0, WALKMOVE_NORMAL ) )
		{
			ALERT(at_error, "Monster %s stuck in wall--level design error", STRING(pev->classname));
			pev->effects = EF_BRIGHTFIELD;
		}
	}
	else 
	{
		pev->flags &= ~FL_ONGROUND;
	}
	
	if ( !FStringNull(pev->target) )// this monster has a target
	{
		// Find the monster's initial target entity, stash it
		m_pGoalEnt = CBaseEntity::Instance( FIND_ENTITY_BY_TARGETNAME ( NULL, STRING( pev->target ) ) );

		if ( !m_pGoalEnt )
		{
			ALERT(at_error, "ReadyMonster()--%s couldn't find target %s", STRING(pev->classname), STRING(pev->target));
		}
		else
		{
			// Monster will start turning towards his destination
			MakeIdealYaw ( m_pGoalEnt->pev->origin );

			// JAY: How important is this error message?  Big Momma doesn't obey this rule, so I took it out.
#if 0
			// At this point, we expect only a path_corner as initial goal
			if (!FClassnameIs( m_pGoalEnt->pev, "path_corner"))
			{
				ALERT(at_warning, "ReadyMonster--monster's initial goal '%s' is not a path_corner", STRING(pev->target));
			}
#endif

			// set the monster up to walk a path corner path. 
			// !!!BUGBUG - this is a minor bit of a hack.
			// JAYJAY
			m_movementGoal = MOVEGOAL_PATHCORNER;
			
			if ( pev->movetype == MOVETYPE_FLY )
				m_movementActivity = ACT_FLY;
			else
				m_movementActivity = ACT_WALK;

			if ( !FRefreshRoute() )
			{
				ALERT ( at_aiconsole, "Can't Create Route!\n" );
			}
			SetState( MONSTERSTATE_IDLE );
			ChangeSchedule( GetScheduleOfType( SCHED_IDLE_WALK ) );
		}
	}
	
	//SetState ( m_IdealMonsterState );
	//SetActivity ( m_IdealActivity );

	// Delay drop to floor to make sure each door in the level has had its chance to spawn
	// Spread think times so that they don't all happen at the same time (Carmack)
	SetThink ( &CBaseMonster::CallMonsterThink );
	pev->nextthink += RANDOM_FLOAT(0.1, 0.4); // spread think times.
	
	if ( !FStringNull(pev->targetname) )// wait until triggered
	{
		SetState( MONSTERSTATE_IDLE );
		// UNDONE: Some scripted sequence monsters don't have an idle?
		SetActivity( ACT_IDLE );
		ChangeSchedule( GetScheduleOfType( SCHED_WAIT_TRIGGER ) );
	}
}


void CBaseMonster :: MovementComplete( void ) 
{ 
	switch( m_iTaskStatus )
	{
	case TASKSTATUS_NEW:
	case TASKSTATUS_RUNNING:
		m_iTaskStatus = TASKSTATUS_RUNNING_TASK;
		break;

	case TASKSTATUS_RUNNING_MOVEMENT:
		TaskComplete();
		break;
	
	case TASKSTATUS_RUNNING_TASK:
		ALERT( at_error, "Movement completed twice!\n" );
		break;

	case TASKSTATUS_COMPLETE:		
		break;
	}
	m_movementGoal = MOVEGOAL_NONE;
}


int CBaseMonster::TaskIsRunning( void )
{
	if ( m_iTaskStatus != TASKSTATUS_COMPLETE && 
		 m_iTaskStatus != TASKSTATUS_RUNNING_MOVEMENT )
		 return 1;

	return 0;
}

//=========================================================
// IRelationship - returns an integer that describes the 
// relationship between two types of monster.
//=========================================================
int CBaseMonster::IRelationship ( CBaseEntity *pTarget )
{
	static int iEnemy[14][14] =
	{			 //   NONE	 MACH	 PLYR	 HPASS	 HMIL	 AMIL	 APASS	 AMONST	APREY	 APRED	 INSECT	PLRALY	PBWPN	ABWPN
	/*NONE*/		{ R_NO	,R_NO	,R_NO	,R_NO	,R_NO	,R_NO	,R_NO	,R_NO	,R_NO	,R_NO	,R_NO	,R_NO,	R_NO,	R_NO	},
	/*MACHINE*/		{ R_NO	,R_NO	,R_DL	,R_DL	,R_NO	,R_DL	,R_DL	,R_DL	,R_DL	,R_DL	,R_NO	,R_DL,	R_DL,	R_DL	},
	/*PLAYER*/		{ R_NO	,R_DL	,R_NO	,R_NO	,R_DL	,R_DL	,R_DL	,R_DL	,R_DL	,R_DL	,R_NO	,R_NO,	R_DL,	R_DL	},
	/*HUMANPASSIVE*/{ R_NO	,R_NO	,R_AL	,R_AL	,R_HT	,R_FR	,R_NO	,R_HT	,R_DL	,R_FR	,R_NO	,R_AL,	R_NO,	R_NO	},
	/*HUMANMILITAR*/{ R_NO	,R_NO	,R_HT	,R_DL	,R_NO	,R_HT	,R_DL	,R_DL	,R_DL	,R_DL	,R_NO	,R_HT,	R_NO,	R_NO	},
	/*ALIENMILITAR*/{ R_NO	,R_DL	,R_HT	,R_DL	,R_HT	,R_NO	,R_NO	,R_NO	,R_NO	,R_NO	,R_NO	,R_DL,	R_NO,	R_NO	},
	/*ALIENPASSIVE*/{ R_NO	,R_NO	,R_NO	,R_NO	,R_NO	,R_NO	,R_NO	,R_NO	,R_NO	,R_NO	,R_NO	,R_NO,	R_NO,	R_NO	},
	/*ALIENMONSTER*/{ R_NO	,R_DL	,R_DL	,R_DL	,R_DL	,R_NO	,R_NO	,R_NO	,R_NO	,R_NO	,R_NO	,R_DL,	R_NO,	R_NO	},
	/*ALIENPREY   */{ R_NO	,R_NO	,R_DL	,R_DL	,R_DL	,R_NO	,R_NO	,R_NO	,R_NO	,R_FR	,R_NO	,R_DL,	R_NO,	R_NO	},
	/*ALIENPREDATO*/{ R_NO	,R_NO	,R_DL	,R_DL	,R_DL	,R_NO	,R_NO	,R_NO	,R_HT	,R_DL	,R_NO	,R_DL,	R_NO,	R_NO	},
	/*INSECT*/		{ R_FR	,R_FR	,R_FR	,R_FR	,R_FR	,R_NO	,R_FR	,R_FR	,R_FR	,R_FR	,R_NO	,R_FR,	R_NO,	R_NO	},
	/*PLAYERALLY*/	{ R_NO	,R_DL	,R_AL	,R_AL	,R_DL	,R_DL	,R_DL	,R_DL	,R_DL	,R_DL	,R_NO	,R_NO,	R_NO,	R_NO	},
	/*PBIOWEAPON*/	{ R_NO	,R_NO	,R_DL	,R_DL	,R_DL	,R_DL	,R_DL	,R_DL	,R_DL	,R_DL	,R_NO	,R_DL,	R_NO,	R_DL	},
	/*ABIOWEAPON*/	{ R_NO	,R_NO	,R_DL	,R_DL	,R_DL	,R_AL	,R_NO	,R_DL	,R_DL	,R_NO	,R_NO	,R_DL,	R_DL,	R_NO	}
	};

	return iEnemy[ Classify() ][ pTarget->Classify() ];
}

//=========================================================
// FindCover - tries to find a nearby node that will hide
// the caller from its enemy. 
//
// If supplied, search will return a node at least as far
// away as MinDist, but no farther than MaxDist. 
// if MaxDist isn't supplied, it defaults to a reasonable 
// value
//=========================================================
// UNDONE: Should this find the nearest node?

//float CGraph::PathLength( int iStart, int iDest, int iHull, int afCapMask )

BOOL CBaseMonster :: FindCover ( Vector vecThreat, Vector vecViewOffset, float flMinDist, float flMaxDist )
{
	int i;
	int iMyHullIndex;
	int iMyNode;
	int iThreatNode;
	float flDist;
	Vector	vecLookersOffset;
	TraceResult tr;

	if ( !flMaxDist )
	{
		// user didn't supply a MaxDist, so work up a crazy one.
		flMaxDist = 784;
	}

	if ( flMinDist > 0.5 * flMaxDist)
	{
#if _DEBUG
		ALERT ( at_console, "FindCover MinDist (%.0f) too close to MaxDist (%.0f)\n", flMinDist, flMaxDist );
#endif
		flMinDist = 0.5 * flMaxDist;
	}

	if ( !WorldGraph.m_fGraphPresent || !WorldGraph.m_fGraphPointersSet )
	{
		ALERT ( at_aiconsole, "Graph not ready for findcover!\n" );
		return FALSE;
	}

	iMyNode = WorldGraph.FindNearestNode( pev->origin, this );
	iThreatNode = WorldGraph.FindNearestNode ( vecThreat, this );
	iMyHullIndex = WorldGraph.HullIndex( this );

	if ( iMyNode == NO_NODE )
	{
		ALERT ( at_aiconsole, "FindCover() - %s has no nearest node!\n", STRING(pev->classname));
		return FALSE;
	}
	if ( iThreatNode == NO_NODE )
	{
		// ALERT ( at_aiconsole, "FindCover() - Threat has no nearest node!\n" );
		iThreatNode = iMyNode;
		// return FALSE;
	}

	vecLookersOffset = vecThreat + vecViewOffset;// calculate location of enemy's eyes

	// we'll do a rough sample to find nodes that are relatively nearby
	for ( i = 0 ; i < WorldGraph.m_cNodes ; i++ )
	{
		int nodeNumber = (i + WorldGraph.m_iLastCoverSearch) % WorldGraph.m_cNodes;

		CNode &node = WorldGraph.Node( nodeNumber );
		WorldGraph.m_iLastCoverSearch = nodeNumber + 1; // next monster that searches for cover node will start where we left off here.

		// could use an optimization here!!
		flDist = ( pev->origin - node.m_vecOrigin ).Length();

		// DON'T do the trace check on a node that is farther away than a node that we've already found to 
		// provide cover! Also make sure the node is within the mins/maxs of the search.
		if ( flDist >= flMinDist && flDist < flMaxDist )
		{
			UTIL_TraceLine ( node.m_vecOrigin + vecViewOffset, vecLookersOffset, ignore_monsters, ignore_glass,  ENT(pev), &tr );

			// if this node will block the threat's line of sight to me...
			if ( tr.flFraction != 1.0 )
			{
				// ..and is also closer to me than the threat, or the same distance from myself and the threat the node is good.
				if ( ( iMyNode == iThreatNode ) || WorldGraph.PathLength( iMyNode, nodeNumber, iMyHullIndex, m_afCapability ) <= WorldGraph.PathLength( iThreatNode, nodeNumber, iMyHullIndex, m_afCapability ) )
				{
					if ( FValidateCover ( node.m_vecOrigin ) && MoveToLocation( ACT_RUN, 0, node.m_vecOrigin ) )
					{
						/*
						MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
							WRITE_BYTE( TE_SHOWLINE);
							
							WRITE_COORD( node.m_vecOrigin.x );
							WRITE_COORD( node.m_vecOrigin.y );
							WRITE_COORD( node.m_vecOrigin.z );

							WRITE_COORD( vecLookersOffset.x );
							WRITE_COORD( vecLookersOffset.y );
							WRITE_COORD( vecLookersOffset.z );
						MESSAGE_END();
						*/

						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}


//=========================================================
// BuildNearestRoute - tries to build a route as close to the target
// as possible, even if there isn't a path to the final point.
//
// If supplied, search will return a node at least as far
// away as MinDist from vecThreat, but no farther than MaxDist. 
// if MaxDist isn't supplied, it defaults to a reasonable 
// value
//=========================================================
BOOL CBaseMonster :: BuildNearestRoute ( Vector vecThreat, Vector vecViewOffset, float flMinDist, float flMaxDist )
{
	int i;
	int iMyHullIndex;
	int iMyNode;
	int iMyThreatNode;
	float flDist;
	Vector	vecLookersOffset;
	TraceResult tr;

	if ( !flMaxDist )
	{
		// user didn't supply a MaxDist, so work up a crazy one.
		flMaxDist = 784;
	}

	if ( flMinDist > 0.5 * flMaxDist)
	{
#if _DEBUG
		ALERT ( at_console, "FindCover MinDist (%.0f) too close to MaxDist (%.0f)\n", flMinDist, flMaxDist );
#endif
		flMinDist = 0.5 * flMaxDist;
	}

	if ( !WorldGraph.m_fGraphPresent || !WorldGraph.m_fGraphPointersSet )
	{
		ALERT ( at_aiconsole, "Graph not ready for BuildNearestRoute!\n" );
		return FALSE;
	}

	iMyNode = WorldGraph.FindNearestNode( pev->origin, this );
	iMyThreatNode = WorldGraph.FindNearestNode( vecThreat, this );
	iMyHullIndex = WorldGraph.HullIndex( this );

	if ( iMyNode == NO_NODE )
	{
		ALERT ( at_aiconsole, "BuildNearestRoute() - %s has no nearest node!\n", STRING(pev->classname));
		return FALSE;
	}

	if ( iMyThreatNode == NO_NODE )
	{
		ALERT ( at_aiconsole, "BuildNearestRoute() - Threat has no nearest node!\n" );
		return FALSE;
	}

	vecLookersOffset = vecThreat + vecViewOffset;// calculate location of enemy's eyes

	// we'll do a rough sample to find nodes that are relatively nearby
	for ( i = 0 ; i < WorldGraph.m_cNodes ; i++ )
	{
		int nodeNumber = (i + WorldGraph.m_iLastCoverSearch) % WorldGraph.m_cNodes;

		CNode &node = WorldGraph.Node( nodeNumber );
		WorldGraph.m_iLastCoverSearch = nodeNumber + 1; // next monster that searches for cover node will start where we left off here.

		// can I get there?
		if (WorldGraph.NextNodeInRoute( iMyNode, nodeNumber, iMyHullIndex, 0 ) != iMyNode)
		{
			flDist = ( vecThreat - node.m_vecOrigin ).Length();

			// is it close?
			if ( flDist > flMinDist && flDist < flMaxDist)
			{
				// can I see where I want to be from there?
				UTIL_TraceLine( node.m_vecOrigin + pev->view_ofs, vecLookersOffset, ignore_monsters, edict(), &tr );

				if (tr.flFraction == 1.0)
				{
					// try to actually get there
					if ( BuildRoute ( node.m_vecOrigin, bits_MF_TO_LOCATION, NULL ) )
					{
						flMaxDist = flDist;
						m_vecMoveGoal = node.m_vecOrigin;
						return TRUE; // UNDONE: keep looking for something closer!
					}
				}
			}
		}
	}

	return FALSE;
}



//=========================================================
// BestVisibleEnemy - this functions searches the link
// list whose head is the caller's m_pLink field, and returns
// a pointer to the enemy entity in that list that is nearest the 
// caller.
//
// !!!UNDONE - currently, this only returns the closest enemy.
// we'll want to consider distance, relationship, attack types, back turned, etc.
//=========================================================
CBaseEntity *CBaseMonster :: BestVisibleEnemy ( void )
{
	CBaseEntity	*pReturn;
	CBaseEntity	*pNextEnt;
	int			iNearest;
	int			iDist;
	int			iBestRelationship;

	iNearest = 8192;// so first visible entity will become the closest.
	pNextEnt = m_pLink;
	pReturn = NULL;
	iBestRelationship = R_NO;

	while ( pNextEnt != NULL )
	{
		if ( pNextEnt->IsAlive() )
		{
			if ( IRelationship( pNextEnt) > iBestRelationship )
			{
				// this entity is disliked MORE than the entity that we 
				// currently think is the best visible enemy. No need to do 
				// a distance check, just get mad at this one for now.
				iBestRelationship = IRelationship ( pNextEnt );
				iNearest = ( pNextEnt->pev->origin - pev->origin ).Length();
				pReturn = pNextEnt;
			}
			else if ( IRelationship( pNextEnt) == iBestRelationship )
			{
				// this entity is disliked just as much as the entity that
				// we currently think is the best visible enemy, so we only
				// get mad at it if it is closer.
				iDist = ( pNextEnt->pev->origin - pev->origin ).Length();
				
				if ( iDist <= iNearest )
				{
					iNearest = iDist;
					iBestRelationship = IRelationship ( pNextEnt );
					pReturn = pNextEnt;
				}
			}
		}

		pNextEnt = pNextEnt->m_pLink;
	}

	return pReturn;
}

//=========================================================
// FInViewCone - returns true is the passed ent is in
// the caller's forward view cone. The dot product is performed
// in 2d, making the view cone infinitely tall. 
//=========================================================
BOOL CBaseMonster :: FInViewCone ( CBaseEntity *pEntity )
{
	Vector2D	vec2LOS;
	float	flDot;

	UTIL_MakeVectors ( pev->angles );
	
	vec2LOS = ( pEntity->pev->origin - pev->origin ).Make2D();
	vec2LOS = vec2LOS.Normalize();

	flDot = DotProduct (vec2LOS , gpGlobals->v_forward.Make2D() );

	if ( flDot > m_flFieldOfView )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//=========================================================
// FInViewCone - returns true is the passed vector is in
// the caller's forward view cone. The dot product is performed
// in 2d, making the view cone infinitely tall. 
//=========================================================
BOOL CBaseMonster :: FInViewCone ( Vector *pOrigin )
{
	Vector2D	vec2LOS;
	float		flDot;

	UTIL_MakeVectors ( pev->angles );
	
	vec2LOS = ( *pOrigin - pev->origin ).Make2D();
	vec2LOS = vec2LOS.Normalize();

	flDot = DotProduct (vec2LOS , gpGlobals->v_forward.Make2D() );

	if ( flDot > m_flFieldOfView )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//=========================================================
// FVisible - returns true if a line can be traced from
// the caller's eyes to the target
//=========================================================
BOOL CBaseEntity :: FVisible ( CBaseEntity *pEntity )
{
	TraceResult tr;
	Vector		vecLookerOrigin;
	Vector		vecTargetOrigin;
	
	if (FBitSet( pEntity->pev->flags, FL_NOTARGET ))
		return FALSE;

	// don't look through water
	if ((pev->waterlevel != 3 && pEntity->pev->waterlevel == 3) 
		|| (pev->waterlevel == 3 && pEntity->pev->waterlevel == 0))
		return FALSE;

	vecLookerOrigin = pev->origin + pev->view_ofs;//look through the caller's 'eyes'
	vecTargetOrigin = pEntity->EyePosition();

	UTIL_TraceLine(vecLookerOrigin, vecTargetOrigin, ignore_monsters, ignore_glass, ENT(pev)/*pentIgnore*/, &tr);
	
	if (tr.flFraction != 1.0)
	{
		return FALSE;// Line of sight is not established
	}
	else
	{
		return TRUE;// line of sight is valid.
	}
}

//=========================================================
// FVisible - returns true if a line can be traced from
// the caller's eyes to the target vector
//=========================================================
BOOL CBaseEntity :: FVisible ( const Vector &vecOrigin )
{
	TraceResult tr;
	Vector		vecLookerOrigin;
	
	vecLookerOrigin = EyePosition();//look through the caller's 'eyes'

	UTIL_TraceLine(vecLookerOrigin, vecOrigin, ignore_monsters, ignore_glass, ENT(pev)/*pentIgnore*/, &tr);
	
	if (tr.flFraction != 1.0)
	{
		return FALSE;// Line of sight is not established
	}
	else
	{
		return TRUE;// line of sight is valid.
	}
}

//=========================================================
// MakeIdealYaw - gets a yaw value for the caller that would
// face the supplied vector. Value is stuffed into the monster's
// ideal_yaw
//=========================================================
void CBaseMonster :: MakeIdealYaw( Vector vecTarget )
{
	Vector	vecProjection;
	
	// strafing monster needs to face 90 degrees away from its goal
	if ( m_movementActivity == ACT_STRAFE_LEFT )
	{
		vecProjection.x = -vecTarget.y;
		vecProjection.y = vecTarget.x;

		pev->ideal_yaw = UTIL_VecToYaw( vecProjection - pev->origin );
	}
	else if ( m_movementActivity == ACT_STRAFE_RIGHT )
	{
		vecProjection.x = vecTarget.y;
		vecProjection.y = vecTarget.x;

		pev->ideal_yaw = UTIL_VecToYaw( vecProjection - pev->origin );
	}
	else
	{
		pev->ideal_yaw = UTIL_VecToYaw ( vecTarget - pev->origin );
	}
}

//=========================================================
// FlYawDiff - returns the difference ( in degrees ) between
// monster's current yaw and ideal_yaw
//
// Positive result is left turn, negative is right turn
//=========================================================
float	CBaseMonster::FlYawDiff ( void )
{
	float	flCurrentYaw;

	flCurrentYaw = UTIL_AngleMod( pev->angles.y );

	if ( flCurrentYaw == pev->ideal_yaw )
	{
		return 0;
	}


	return UTIL_AngleDiff( pev->ideal_yaw, flCurrentYaw );
}


//=========================================================
// Changeyaw - turns a monster towards its ideal_yaw
//=========================================================
float CBaseMonster::ChangeYaw ( int yawSpeed )
{
	float		ideal, current, move, speed;

	current = UTIL_AngleMod( pev->angles.y );
	ideal = pev->ideal_yaw;
	if (current != ideal)
	{
		speed = (float)yawSpeed * gpGlobals->frametime * 10;
		move = ideal - current;

		if (ideal > current)
		{
			if (move >= 180)
				move = move - 360;
		}
		else
		{
			if (move <= -180)
				move = move + 360;
		}

		if (move > 0)
		{// turning to the monster's left
			if (move > speed)
				move = speed;
		}
		else
		{// turning to the monster's right
			if (move < -speed)
				move = -speed;
		}
		
		pev->angles.y = UTIL_AngleMod (current + move);

		// turn head in desired direction only if they have a turnable head
		if (m_afCapability & bits_CAP_TURN_HEAD)
		{
			float yaw = pev->ideal_yaw - pev->angles.y;
			if (yaw > 180) yaw -= 360;
			if (yaw < -180) yaw += 360;
			// yaw *= 0.8;
			SetBoneController( 0, yaw );
		}
	}
	else
		move = 0;

	return move;
}

//=========================================================
// VecToYaw - turns a directional vector into a yaw value
// that points down that vector.
//=========================================================
float	CBaseMonster::VecToYaw ( Vector vecDir )
{
	if (vecDir.x == 0 && vecDir.y == 0 && vecDir.z == 0)
		return pev->angles.y;

	return UTIL_VecToYaw( vecDir );
}

//=========================================================
// LookupActivity
//=========================================================
int CBaseAnimating :: LookupActivity ( int activity )
{
	ASSERT( activity != 0 );
	void *pmodel = GET_MODEL_PTR( ENT(pev) );

	return ::LookupActivity( pmodel, pev, activity );
}

//=========================================================
// LookupActivityHeaviest
//
// Get activity with highest 'weight'
//
//=========================================================
int CBaseAnimating :: LookupActivityHeaviest ( int activity )
{
	void *pmodel = GET_MODEL_PTR( ENT(pev) );

	return ::LookupActivityHeaviest( pmodel, pev, activity );
}

//=========================================================
// SetEyePosition
//
// queries the monster's model for $eyeposition and copies
// that vector to the monster's view_ofs
//
//=========================================================
void CBaseMonster :: SetEyePosition ( void )
{
	Vector  vecEyePosition;
	void	*pmodel = GET_MODEL_PTR( ENT(pev) );

	GetEyePosition( pmodel, vecEyePosition );

	pev->view_ofs = vecEyePosition;

	if ( pev->view_ofs == g_vecZero )
	{
		ALERT ( at_aiconsole, "%s has no view_ofs!\n", STRING ( pev->classname ) );
	}
}

//=========================================================
//=========================================================
int CBaseAnimating :: LookupSequence ( const char *label )
{
	void *pmodel = GET_MODEL_PTR( ENT(pev) );

	return ::LookupSequence( pmodel, label );
}


//=========================================================
//=========================================================
void CBaseAnimating :: ResetSequenceInfo ( )
{
	void *pmodel = GET_MODEL_PTR( ENT(pev) );

	GetSequenceInfo( pmodel, pev, &m_flFrameRate, &m_flGroundSpeed );
	m_fSequenceLoops = ((GetSequenceFlags() & STUDIO_LOOPING) != 0);
	pev->animtime = gpGlobals->time;
	pev->framerate = 1.0;
	m_fSequenceFinished = FALSE;
	m_flLastEventCheck = gpGlobals->time;
}

//=========================================================
//=========================================================
BOOL CBaseAnimating :: GetSequenceFlags( )
{
	void *pmodel = GET_MODEL_PTR( ENT(pev) );

	return ::GetSequenceFlags( pmodel, pev );
}

//=========================================================
// DispatchAnimEvents
//=========================================================
void CBaseAnimating :: DispatchAnimEvents ( float flInterval )
{
	MonsterEvent_t	event;

	void *pmodel = GET_MODEL_PTR( ENT(pev) );

	if ( !pmodel )
	{
		ALERT( at_aiconsole, "Gibbed monster is thinking!\n" );
		return;
	}

	// FIXME: I have to do this or some events get missed, and this is probably causing the problem below
	flInterval = 0.1;

	// FIX: this still sometimes hits events twice
	float flStart = pev->frame + (m_flLastEventCheck - pev->animtime) * m_flFrameRate * pev->framerate;
	float flEnd = pev->frame + flInterval * m_flFrameRate * pev->framerate;
	m_flLastEventCheck = pev->animtime + flInterval;

	m_fSequenceFinished = FALSE;
	if (flEnd >= 256 || flEnd <= 0.0) 
		m_fSequenceFinished = TRUE;

	int index = 0;

	while ( (index = GetAnimationEvent( pmodel, pev, &event, flStart, flEnd, index ) ) != 0 )
	{
		HandleAnimEvent( &event );
	}
}

void CBaseMonster :: HandleAnimEvent( MonsterEvent_t *pEvent )
{
	switch( pEvent->event )
	{
	case SCRIPT_EVENT_DEAD:
		if ( m_MonsterState == MONSTERSTATE_SCRIPT )
		{
			pev->deadflag = DEAD_DYING;
			// Kill me now! (and fade out when CineCleanup() is called)
#if _DEBUG
			ALERT( at_aiconsole, "Death event: %s\n", STRING(pev->classname) );
#endif
			pev->health = 0;
		}
#if _DEBUG
		else
			ALERT( at_aiconsole, "INVALID death event:%s\n", STRING(pev->classname) );
#endif
		break;
	case SCRIPT_EVENT_NOT_DEAD:
		if ( m_MonsterState == MONSTERSTATE_SCRIPT )
		{
			pev->deadflag = DEAD_NO;
			// This is for life/death sequences where the player can determine whether a character is dead or alive after the script 
			pev->health = pev->max_health;
		}
		break;

	case SCRIPT_EVENT_SOUND:			// Play a named wave file
		EMIT_SOUND( edict(), CHAN_BODY, pEvent->options, 1.0, ATTN_IDLE );
		break;

	case SCRIPT_EVENT_SOUND_VOICE:
		EMIT_SOUND( edict(), CHAN_VOICE, pEvent->options, 1.0, ATTN_IDLE );
		break;

	case SCRIPT_EVENT_SENTENCE_RND1:		// Play a named sentence group 33% of the time
		if (RANDOM_LONG(0,2) == 0)
			break;
		// fall through...
	case SCRIPT_EVENT_SENTENCE:			// Play a named sentence group
		SENTENCEG_PlayRndSz( edict(), pEvent->options, 1.0, ATTN_IDLE, 0, 100 );
		break;

	case SCRIPT_EVENT_FIREEVENT:		// Fire a trigger
		FireTargets( pEvent->options, this, this, USE_TOGGLE, 0 );
		break;

	case SCRIPT_EVENT_NOINTERRUPT:		// Can't be interrupted from now on
		if ( m_pCine )
			m_pCine->AllowInterrupt( FALSE );
		break;

	case SCRIPT_EVENT_CANINTERRUPT:		// OK to interrupt now
		if ( m_pCine )
			m_pCine->AllowInterrupt( TRUE );
		break;

#if 0
	case SCRIPT_EVENT_INAIR:			// Don't DROP_TO_FLOOR()
	case SCRIPT_EVENT_ENDANIMATION:		// Set ending animation sequence to
		break;
#endif

	case MONSTER_EVENT_BODYDROP_HEAVY:
		if ( pev->flags & FL_ONGROUND )
		{
			if ( RANDOM_LONG( 0, 1 ) == 0 )
			{
				EMIT_SOUND_DYN( ENT(pev), CHAN_BODY, "common/bodydrop3.wav", 1, ATTN_NORM, 0, 90 );
			}
			else
			{
				EMIT_SOUND_DYN( ENT(pev), CHAN_BODY, "common/bodydrop4.wav", 1, ATTN_NORM, 0, 90 );
			}
		}
		break;

	case MONSTER_EVENT_BODYDROP_LIGHT:
		if ( pev->flags & FL_ONGROUND )
		{
			if ( RANDOM_LONG( 0, 1 ) == 0 )
			{
				EMIT_SOUND( ENT(pev), CHAN_BODY, "common/bodydrop3.wav", 1, ATTN_NORM );
			}
			else
			{
				EMIT_SOUND( ENT(pev), CHAN_BODY, "common/bodydrop4.wav", 1, ATTN_NORM );
			}
		}
		break;

	case MONSTER_EVENT_SWISHSOUND:
		{
			// NO MONSTER may use this anim event unless that monster's precache precaches this sound!!!
			EMIT_SOUND( ENT(pev), CHAN_BODY, "zombie/claw_miss2.wav", 1, ATTN_NORM );
			break;
		}

	default:
		ALERT( at_aiconsole, "Unhandled animation event %d for %s\n", pEvent->event, STRING(pev->classname) );
		break;

	}
}

//=========================================================
//=========================================================
float CBaseAnimating :: SetBoneController ( int iController, float flValue )
{
	void *pmodel = GET_MODEL_PTR( ENT(pev) );

	return SetController( pmodel, pev, iController, flValue );
}

//=========================================================
//=========================================================
void CBaseAnimating :: InitBoneControllers ( void )
{
	void *pmodel = GET_MODEL_PTR( ENT(pev) );

	SetController( pmodel, pev, 0, 0.0 );
	SetController( pmodel, pev, 1, 0.0 );
	SetController( pmodel, pev, 2, 0.0 );
	SetController( pmodel, pev, 3, 0.0 );
}

//=========================================================
//=========================================================
float CBaseAnimating :: SetBlending ( int iBlender, float flValue )
{
	void *pmodel = GET_MODEL_PTR( ENT(pev) );

	return ::SetBlending( pmodel, pev, iBlender, flValue );
}

//=========================================================
//=========================================================
void CBaseAnimating :: GetBonePosition ( int iBone, const Vector &origin, const Vector &angles )
{
	GET_BONE_POSITION( ENT(pev), iBone, origin, angles );
}

//=========================================================
//=========================================================
void CBaseAnimating :: GetAttachment ( int iAttachment, const Vector &origin, const Vector &angles )
{
	GET_ATTACHMENT( ENT(pev), iAttachment, origin, angles );
}

//=========================================================
//=========================================================
int CBaseAnimating :: FindTransition( int iEndingSequence, int iGoalSequence, int *piDir )
{
	void *pmodel = GET_MODEL_PTR( ENT(pev) );
	
	if (piDir == NULL)
	{
		int iDir;
		int sequence = ::FindTransition( pmodel, iEndingSequence, iGoalSequence, &iDir );
		if (iDir != 1)
			return -1;
		else
			return sequence;
	}

	return ::FindTransition( pmodel, iEndingSequence, iGoalSequence, piDir );
}

//=========================================================
//=========================================================
void CBaseAnimating :: GetAutomovement( const Vector &origin, const Vector &angles, float flInterval )
{

}

void CBaseAnimating :: SetBodygroup( int iGroup, int iValue )
{
	::SetBodygroup( GET_MODEL_PTR( ENT(pev) ), pev, iGroup, iValue );
}

int CBaseAnimating :: GetBodygroup( int iGroup )
{
	return ::GetBodygroup( GET_MODEL_PTR( ENT(pev) ), pev, iGroup );
}

// Combat

Vector CBaseMonster :: GetGunPosition( )
{
	UTIL_MakeVectors(pev->angles);

	// Vector vecSrc = pev->origin + gpGlobals->v_forward * 10;
	//vecSrc.z = pevShooter->absmin.z + pevShooter->size.z * 0.7;
	//vecSrc.z = pev->origin.z + (pev->view_ofs.z - 4);
	Vector vecSrc = pev->origin 
					+ gpGlobals->v_forward * m_HackedGunPos.y 
					+ gpGlobals->v_right * m_HackedGunPos.x 
					+ gpGlobals->v_up * m_HackedGunPos.z;

	return vecSrc;
}

/*
================
TraceAttack
================
*/
void CBaseEntity::TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType)
{
	Vector vecOrigin = ptr->vecEndPos - vecDir * 4;

	if ( pev->takedamage )
	{
		AddMultiDamage( pevAttacker, this, flDamage, bitsDamageType );

		int blood = BloodColor();
		
		if ( blood != DONT_BLEED )
		{
			SpawnBlood(vecOrigin, blood, flDamage);// a little surface blood.
			TraceBleed( flDamage, vecDir, ptr, bitsDamageType );
		}
	}
}

/*
================
FireBullets

Go to the trouble of combining multiple pellets into a single damage call.
================
*/
void CBaseEntity::FireBullets(ULONG cShots, Vector vecSrc, Vector vecDirShooting, Vector vecSpread, float flDistance, int iBulletType, int iTracerFreq, int iDamage, entvars_t *pevAttacker )
{
	static int tracerCount;
	int tracer;
	TraceResult tr;
	Vector vecRight = gpGlobals->v_right;
	Vector vecUp = gpGlobals->v_up;

	if ( pevAttacker == NULL )
		pevAttacker = pev;  // the default attacker is ourselves

	// Vector vecSrc = pev->origin + gpGlobals->v_forward * 10;
	//vecSrc.z = pevShooter->absmin.z + pevShooter->size.z * 0.7;
	//vecSrc.z = pev->origin.z + (pev->view_ofs.z - 4);
	ClearMultiDamage();
	gMultiDamage.type = DMG_BULLET | DMG_NEVERGIB;

	for (ULONG iShot = 1; iShot <= cShots; iShot++)
	{
		// get circular gaussian spread
		float x, y, z;
		do {
			x = RANDOM_FLOAT(-0.5,0.5) + RANDOM_FLOAT(-0.5,0.5);
			y = RANDOM_FLOAT(-0.5,0.5) + RANDOM_FLOAT(-0.5,0.5);
			z = x*x+y*y;
		} while (z > 1);

		Vector vecDir = vecDirShooting +
						x * vecSpread.x * vecRight +
						y * vecSpread.y * vecUp;
		Vector vecEnd;

		vecEnd = vecSrc + vecDir * flDistance;
		UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, ENT(pev)/*pentIgnore*/, &tr);

		tracer = 0;
		if (iTracerFreq != 0 && (tracerCount++ % iTracerFreq) == 0)
		{
			Vector vecTracerSrc;

			if ( pev->flags & FL_CLIENT )
			{// adjust tracer position for player
				vecTracerSrc = vecSrc + Vector ( 0 , 0 , -4 ) + gpGlobals->v_right * 2 + gpGlobals->v_forward * 16;
			}
			else
			{
				vecTracerSrc = vecSrc;
			}
			
			if ( iTracerFreq != 1 )		// guns that always trace also always decal
				tracer = 1;
			switch( iBulletType )
			{
			case BULLET_PLAYER_MP5:
			case BULLET_MONSTER_MP5:
			case BULLET_MONSTER_9MM:
			case BULLET_MONSTER_12MM:
			default:
				MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, vecTracerSrc );
					WRITE_BYTE( TE_TRACER );
					WRITE_COORD( vecTracerSrc.x );
					WRITE_COORD( vecTracerSrc.y );
					WRITE_COORD( vecTracerSrc.z );
					WRITE_COORD( tr.vecEndPos.x );
					WRITE_COORD( tr.vecEndPos.y );
					WRITE_COORD( tr.vecEndPos.z );
				MESSAGE_END();

				break;
				/*
			case BULLET_12MM:
				WRITE_BYTE(MSG_BROADCAST, SVC_TEMPENTITY);
				WRITE_BYTE(MSG_BROADCAST, TE_RAILTRAIL);
				WRITE_COORD(MSG_BROADCAST, vecSrc.x);
				WRITE_COORD(MSG_BROADCAST, vecSrc.y);
				WRITE_COORD(MSG_BROADCAST, vecSrc.z);
				WRITE_COORD(MSG_BROADCAST, tr.vecEndPos.x);
				WRITE_COORD(MSG_BROADCAST, tr.vecEndPos.y);
				WRITE_COORD(MSG_BROADCAST, tr.vecEndPos.z);
				break;
				*/
			}
		}
		// do damage, paint decals
		if (tr.flFraction != 1.0)
		{
			CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);

			if ( iDamage )
			{
				pEntity->TraceAttack(pevAttacker, iDamage, vecDir, &tr, DMG_BULLET | ((iDamage > 16) ? DMG_ALWAYSGIB : DMG_NEVERGIB) );
				
				TEXTURETYPE_PlaySound(&tr, vecSrc, vecEnd, iBulletType);
				DecalGunshot( &tr, iBulletType );
			} 
			else switch(iBulletType)
			{
			default:
			case BULLET_PLAYER_9MM:		
				pEntity->TraceAttack(pevAttacker, gSkillData.plrDmg9MM, vecDir, &tr, DMG_BULLET); 
				if ( !tracer )
				{
					TEXTURETYPE_PlaySound(&tr, vecSrc, vecEnd, iBulletType);
					DecalGunshot( &tr, iBulletType );
				}
				break;

			case BULLET_PLAYER_MP5:		
				pEntity->TraceAttack(pevAttacker, gSkillData.plrDmgMP5, vecDir, &tr, DMG_BULLET); 
				if ( !tracer )
				{
					TEXTURETYPE_PlaySound(&tr, vecSrc, vecEnd, iBulletType);
					DecalGunshot( &tr, iBulletType );
				}
				break;

			case BULLET_PLAYER_BUCKSHOT:	
				 // make distance based!
				pEntity->TraceAttack(pevAttacker, gSkillData.plrDmgBuckshot, vecDir, &tr, DMG_BULLET); 
				if ( !tracer )
				{
					// TEXTURETYPE_PlaySound(&tr, vecSrc, vecEnd, iBulletType);
					DecalGunshot( &tr, iBulletType );
				}
				break;
			
			case BULLET_PLAYER_357:		
				pEntity->TraceAttack(pevAttacker, gSkillData.plrDmg357, vecDir, &tr, DMG_BULLET); 
				if ( !tracer )
				{
					TEXTURETYPE_PlaySound(&tr, vecSrc, vecEnd, iBulletType);
					DecalGunshot( &tr, iBulletType );
				}
				break;

			case BULLET_MONSTER_9MM:
				pEntity->TraceAttack(pevAttacker, gSkillData.monDmg9MM, vecDir, &tr, DMG_BULLET);
				
				TEXTURETYPE_PlaySound(&tr, vecSrc, vecEnd, iBulletType);
				DecalGunshot( &tr, iBulletType );

				break;

			case BULLET_MONSTER_MP5:
				pEntity->TraceAttack(pevAttacker, gSkillData.monDmgMP5, vecDir, &tr, DMG_BULLET);
				
				TEXTURETYPE_PlaySound(&tr, vecSrc, vecEnd, iBulletType);
				DecalGunshot( &tr, iBulletType );

				break;

			case BULLET_MONSTER_12MM:		
				pEntity->TraceAttack(pevAttacker, gSkillData.monDmg12MM, vecDir, &tr, DMG_BULLET); 
				if ( !tracer )
				{
					TEXTURETYPE_PlaySound(&tr, vecSrc, vecEnd, iBulletType);
					DecalGunshot( &tr, iBulletType );
				}
				break;
			
			case BULLET_NONE: // FIX 
				pEntity->TraceAttack(pevAttacker, 50, vecDir, &tr, DMG_CLUB);
				TEXTURETYPE_PlaySound(&tr, vecSrc, vecEnd, iBulletType);
				// only decal glass
				if ( !FNullEnt(tr.pHit) && VARS(tr.pHit)->rendermode != 0)
				{
					UTIL_DecalTrace( &tr, DECAL_GLASSBREAK1 + RANDOM_LONG(0,2) );
				}

				break;
			}
		}
		// make bullet trails
		UTIL_BubbleTrail( vecSrc, tr.vecEndPos, (flDistance * tr.flFraction) / 64.0 );
	}
	ApplyMultiDamage(pev, pevAttacker);
}


void CBaseEntity :: TraceBleed( float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType )
{
	if (BloodColor() == DONT_BLEED)
		return;
	
	if (flDamage == 0)
		return;

	if (! (bitsDamageType & (DMG_CRUSH | DMG_BULLET | DMG_SLASH | DMG_BLAST | DMG_CLUB | DMG_MORTAR)))
		return;
	
	// make blood decal on the wall! 
	TraceResult Bloodtr;
	Vector vecTraceDir; 
	float flNoise;
	int cCount;
	int i;

/*
	if ( !IsAlive() )
	{
		// dealing with a dead monster. 
		if ( pev->max_health <= 0 )
		{
			// no blood decal for a monster that has already decalled its limit.
			return; 
		}
		else
		{
			pev->max_health--;
		}
	}
*/

	if (flDamage < 10)
	{
		flNoise = 0.1;
		cCount = 1;
	}
	else if (flDamage < 25)
	{
		flNoise = 0.2;
		cCount = 2;
	}
	else
	{
		flNoise = 0.3;
		cCount = 4;
	}

	for ( i = 0 ; i < cCount ; i++ )
	{
		vecTraceDir = vecDir * -1;// trace in the opposite direction the shot came from (the direction the shot is going)

		vecTraceDir.x += RANDOM_FLOAT( -flNoise, flNoise );
		vecTraceDir.y += RANDOM_FLOAT( -flNoise, flNoise );
		vecTraceDir.z += RANDOM_FLOAT( -flNoise, flNoise );

		UTIL_TraceLine( ptr->vecEndPos, ptr->vecEndPos + vecTraceDir * -172, ignore_monsters, ENT(pev), &Bloodtr);

		if ( Bloodtr.flFraction != 1.0 )
		{
			UTIL_BloodDecalTrace( &Bloodtr, BloodColor() );
		}
	}
}

//=========================================================
//=========================================================
void CBaseMonster :: MakeDamageBloodDecal ( int cCount, float flNoise, TraceResult *ptr, const Vector &vecDir )
{
	// make blood decal on the wall! 
	TraceResult Bloodtr;
	Vector vecTraceDir; 
	int i;

	if ( !IsAlive() )
	{
		// dealing with a dead monster. 
		if ( pev->max_health <= 0 )
		{
			// no blood decal for a monster that has already decalled its limit.
			return; 
		}
		else
		{
			pev->max_health--;
		}
	}

	for ( i = 0 ; i < cCount ; i++ )
	{
		vecTraceDir = vecDir;

		vecTraceDir.x += RANDOM_FLOAT( -flNoise, flNoise );
		vecTraceDir.y += RANDOM_FLOAT( -flNoise, flNoise );
		vecTraceDir.z += RANDOM_FLOAT( -flNoise, flNoise );

		UTIL_TraceLine( ptr->vecEndPos, ptr->vecEndPos + vecTraceDir * 172, ignore_monsters, ENT(pev), &Bloodtr);

/*
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE( TE_SHOWLINE);
			WRITE_COORD( ptr->vecEndPos.x );
			WRITE_COORD( ptr->vecEndPos.y );
			WRITE_COORD( ptr->vecEndPos.z );
			
			WRITE_COORD( Bloodtr.vecEndPos.x );
			WRITE_COORD( Bloodtr.vecEndPos.y );
			WRITE_COORD( Bloodtr.vecEndPos.z );
		MESSAGE_END();
*/

		if ( Bloodtr.flFraction != 1.0 )
		{
			UTIL_BloodDecalTrace( &Bloodtr, BloodColor() );
		}
	}
}

//=========================================================
// NODE GRAPH
//=========================================================





//=========================================================
// FGetNodeRoute - tries to build an entire node path from
// the callers origin to the passed vector. If this is 
// possible, ROUTE_SIZE waypoints will be copied into the
// callers m_Route. TRUE is returned if the operation 
// succeeds (path is valid) or FALSE if failed (no path 
// exists )
//=========================================================
BOOL CBaseMonster :: FGetNodeRoute ( Vector vecDest )
{
	int iPath[ MAX_PATH_SIZE ];
	int iSrcNode, iDestNode;
	int iResult;
	int i;
	int iNumToCopy;

	iSrcNode = WorldGraph.FindNearestNode ( pev->origin, this );
	iDestNode = WorldGraph.FindNearestNode ( vecDest, this );

	if ( iSrcNode == -1 )
	{
		// no node nearest self
//		ALERT ( at_aiconsole, "FGetNodeRoute: No valid node near self!\n" );
		return FALSE;
	}
	else if ( iDestNode == -1 )
	{
		// no node nearest target
//		ALERT ( at_aiconsole, "FGetNodeRoute: No valid node near target!\n" );
		return FALSE;
	}

	// valid src and dest nodes were found, so it's safe to proceed with
	// find shortest path
	int iNodeHull = WorldGraph.HullIndex( this ); // make this a monster virtual function
	iResult = WorldGraph.FindShortestPath ( iPath, iSrcNode, iDestNode, iNodeHull, m_afCapability );

	if ( !iResult )
	{
#if 1
		ALERT ( at_aiconsole, "No Path from %d to %d!\n", iSrcNode, iDestNode );
		return FALSE;
#else
		BOOL bRoutingSave = WorldGraph.m_fRoutingComplete;
		WorldGraph.m_fRoutingComplete = FALSE;
		iResult = WorldGraph.FindShortestPath(iPath, iSrcNode, iDestNode, iNodeHull, m_afCapability);
		WorldGraph.m_fRoutingComplete = bRoutingSave;
		if ( !iResult )
		{
			ALERT ( at_aiconsole, "No Path from %d to %d!\n", iSrcNode, iDestNode );
			return FALSE;
		}
		else
		{
			ALERT ( at_aiconsole, "Routing is inconsistent!" );
		}
#endif
	}

	// there's a valid path within iPath now, so now we will fill the route array
	// up with as many of the waypoints as it will hold.
	
	// don't copy ROUTE_SIZE entries if the path returned is shorter
	// than ROUTE_SIZE!!!
	if ( iResult < ROUTE_SIZE )
	{
		iNumToCopy = iResult;
	}
	else
	{
		iNumToCopy = ROUTE_SIZE;
	}
	
	for ( i = 0 ; i < iNumToCopy; i++ )
	{
		m_Route[ i ].vecLocation = WorldGraph.m_pNodes[ iPath[ i ] ].m_vecOrigin;
		m_Route[ i ].iType = bits_MF_TO_NODE;
	}
	
	if ( iNumToCopy < ROUTE_SIZE )
	{
		m_Route[ iNumToCopy ].vecLocation = vecDest;
		m_Route[ iNumToCopy ].iType |= bits_MF_IS_GOAL;
	}

	return TRUE;
}

//=========================================================
// FindHintNode
//=========================================================
int CBaseMonster :: FindHintNode ( void )
{
	int i;
	TraceResult tr;

	if ( !WorldGraph.m_fGraphPresent )
	{
		ALERT ( at_aiconsole, "find_hintnode: graph not ready!\n" );
		return NO_NODE;
	}

	if ( WorldGraph.m_iLastActiveIdleSearch >= WorldGraph.m_cNodes )
	{
		WorldGraph.m_iLastActiveIdleSearch = 0;
	}

	for ( i = 0; i < WorldGraph.m_cNodes ; i++ )
	{
		int nodeNumber = (i + WorldGraph.m_iLastActiveIdleSearch) % WorldGraph.m_cNodes;
		CNode &node = WorldGraph.Node( nodeNumber );

		if ( node.m_sHintType )
		{
			// this node has a hint. Take it if it is visible, the monster likes it, and the monster has an animation to match the hint's activity.
			if ( FValidateHintType ( node.m_sHintType ) )
			{
				if ( !node.m_sHintActivity || LookupActivity ( node.m_sHintActivity ) != ACTIVITY_NOT_AVAILABLE )
				{
					UTIL_TraceLine ( pev->origin + pev->view_ofs, node.m_vecOrigin + pev->view_ofs, ignore_monsters, ENT(pev), &tr );

					if ( tr.flFraction == 1.0 )
					{
						WorldGraph.m_iLastActiveIdleSearch = nodeNumber + 1; // next monster that searches for hint nodes will start where we left off.
						return nodeNumber;// take it!
					}
				}
			}
		}
	}

	WorldGraph.m_iLastActiveIdleSearch = 0;// start at the top of the list for the next search.

	return NO_NODE;
}
			

void CBaseMonster::ReportAIState( void )
{
	ALERT_TYPE level = at_console;

	static const char *pStateNames[] = { "None", "Idle", "Combat", "Alert", "Hunt", "Prone", "Scripted", "Dead" };

	ALERT( level, "%s: ", STRING(pev->classname) );
	if ( (int)m_MonsterState < ARRAYSIZE(pStateNames) )
		ALERT( level, "State: %s, ", pStateNames[m_MonsterState] );
	int i = 0;
	while ( activity_map[i].type != 0 )
	{
		if ( activity_map[i].type == (int)m_Activity )
		{
			ALERT( level, "Activity %s, ", activity_map[i].name );
			break;
		}
		i++;
	}

	if ( m_pSchedule )
	{
		const char *pName = NULL;
		pName = m_pSchedule->pName;
		if ( !pName )
			pName = "Unknown";
		ALERT( level, "Schedule %s, ", pName );
		Task_t *pTask = GetTask();
		if ( pTask )
			ALERT( level, "Task %d (#%d), ", pTask->iTask, m_iScheduleIndex );
	}
	else
		ALERT( level, "No Schedule, " );

	if ( m_hEnemy != NULL )
		ALERT( level, "\nEnemy is %s", STRING(m_hEnemy->pev->classname) );
	else
		ALERT( level, "No enemy" );

	if ( IsMoving() )
	{
		ALERT( level, " Moving " );
		if ( m_flMoveWaitFinished > gpGlobals->time )
			ALERT( level, ": Stopped for %.2f. ", m_flMoveWaitFinished - gpGlobals->time );
		else if ( m_IdealActivity == GetStoppedActivity() )
			ALERT( level, ": In stopped anim. " );
	}

	CSquadMonster *pSquadMonster = MySquadMonsterPointer();

	if ( pSquadMonster )
	{
		if ( !pSquadMonster->InSquad() )
		{
			ALERT ( level, "not " );
		}

		ALERT ( level, "In Squad, " );

		if ( !pSquadMonster->IsLeader() )
		{
			ALERT ( level, "not " );
		}

		ALERT ( level, "Leader." );
	}

	ALERT( level, "\n" );
	ALERT( level, "Yaw speed:%3.1f,Health: %3.1f\n", pev->yaw_speed, pev->health );
	if ( pev->spawnflags & SF_MONSTER_PRISONER )
		ALERT( level, " PRISONER! " );
	if ( pev->spawnflags & SF_MONSTER_PREDISASTER )
		ALERT( level, " Pre-Disaster! " );
	ALERT( level, "\n" );
}

//=========================================================
// KeyValue
//
// !!! netname entvar field is used in squadmonster for groupname!!!
//=========================================================
void CBaseMonster :: KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "TriggerTarget"))
	{
		m_iszTriggerTarget = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "TriggerCondition") )
	{
		m_iTriggerCondition = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
	{
		CBaseToggle::KeyValue( pkvd );
	}
}

//=========================================================
// FCheckAITrigger - checks the monster's AI Trigger Conditions,
// if there is a condition, then checks to see if condition is 
// met. If yes, the monster's TriggerTarget is fired.
//
// Returns TRUE if the target is fired.
//=========================================================
BOOL CBaseMonster :: FCheckAITrigger ( void )
{
	BOOL fFireTarget;

	if ( m_iTriggerCondition == AITRIGGER_NONE )
	{
		// no conditions, so this trigger is never fired.
		return FALSE; 
	}

	fFireTarget = FALSE;

	switch ( m_iTriggerCondition )
	{
	case AITRIGGER_SEEPLAYER_ANGRY_AT_PLAYER:
		if ( m_hEnemy != NULL && m_hEnemy->pev->flags & FL_CLIENT && HasConditions ( bits_COND_SEE_ENEMY ) )
		{
			fFireTarget = TRUE;
		}
		break;
	case AITRIGGER_SEEPLAYER_UNCONDITIONAL:
		if ( HasConditions ( bits_COND_SEE_CLIENT ) )
		{
			fFireTarget = TRUE;
		}
		break;
	case AITRIGGER_SEEPLAYER_NOT_IN_COMBAT:
		if ( HasConditions ( bits_COND_SEE_CLIENT ) && 
			 m_MonsterState != MONSTERSTATE_COMBAT	&& 
			 m_MonsterState != MONSTERSTATE_PRONE	&& 
			 m_MonsterState != MONSTERSTATE_SCRIPT)
		{
			fFireTarget = TRUE;
		}
		break;
	case AITRIGGER_TAKEDAMAGE:
		if ( m_afConditions & ( bits_COND_LIGHT_DAMAGE | bits_COND_HEAVY_DAMAGE ) )
		{
			fFireTarget = TRUE;
		}
		break;
	case AITRIGGER_DEATH:
		if ( pev->deadflag != DEAD_NO )
		{
			fFireTarget = TRUE;
		}
		break;
	case AITRIGGER_HALFHEALTH:
		if ( IsAlive() && pev->health <= ( pev->max_health / 2 ) )
		{
			fFireTarget = TRUE;
		}
		break;
/*

  // !!!UNDONE - no persistant game state that allows us to track these two. 

	case AITRIGGER_SQUADMEMBERDIE:
		break;
	case AITRIGGER_SQUADLEADERDIE:
		break;
*/
	case AITRIGGER_HEARWORLD:
		if ( m_afConditions & bits_COND_HEAR_SOUND && m_afSoundTypes & bits_SOUND_WORLD )
		{
			fFireTarget = TRUE;
		}
		break;
	case AITRIGGER_HEARPLAYER:
		if ( m_afConditions & bits_COND_HEAR_SOUND && m_afSoundTypes & bits_SOUND_PLAYER )
		{
			fFireTarget = TRUE;
		}
		break;
	case AITRIGGER_HEARCOMBAT:
		if ( m_afConditions & bits_COND_HEAR_SOUND && m_afSoundTypes & bits_SOUND_COMBAT )
		{
			fFireTarget = TRUE;
		}
		break;
	}

	if ( fFireTarget )
	{
		// fire the target, then set the trigger conditions to NONE so we don't fire again
		ALERT ( at_aiconsole, "AI Trigger Fire Target\n" );
		FireTargets( STRING( m_iszTriggerTarget ), this, this, USE_TOGGLE, 0 );
		m_iTriggerCondition = AITRIGGER_NONE;
		return TRUE;
	}

	return FALSE;
}

//=========================================================	
// CanPlaySequence - determines whether or not the monster
// can play the scripted sequence or AI sequence that is 
// trying to possess it. If DisregardState is set, the monster
// will be sucked into the script no matter what state it is
// in. ONLY Scripted AI ents should allow this.
//=========================================================	
int CBaseMonster :: CanPlaySequence( BOOL fDisregardMonsterState, int interruptLevel )
{
	if ( m_pCine || !IsAlive() || m_MonsterState == MONSTERSTATE_PRONE )
	{
		// monster is already running a scripted sequence or dead!
		return FALSE;
	}
	
	if ( fDisregardMonsterState )
	{
		// ok to go, no matter what the monster state. (scripted AI)
		return TRUE;
	}

	if ( m_MonsterState == MONSTERSTATE_NONE || m_MonsterState == MONSTERSTATE_IDLE || m_IdealMonsterState == MONSTERSTATE_IDLE )
	{
		// ok to go, but only in these states
		return TRUE;
	}
	
	if ( m_MonsterState == MONSTERSTATE_ALERT && interruptLevel >= SS_INTERRUPT_BY_NAME )
		return TRUE;

	// unknown situation
	return FALSE;
}


//=========================================================
// FindLateralCover - attempts to locate a spot in the world
// directly to the left or right of the caller that will
// conceal them from view of pSightEnt
//=========================================================
#define	COVER_CHECKS	5// how many checks are made
#define COVER_DELTA		48// distance between checks

BOOL CBaseMonster :: FindLateralCover ( const Vector &vecThreat, const Vector &vecViewOffset )
{
	TraceResult	tr;
	Vector	vecBestOnLeft;
	Vector	vecBestOnRight;
	Vector	vecLeftTest;
	Vector	vecRightTest;
	Vector	vecStepRight;
	int		i;

	UTIL_MakeVectors ( pev->angles );
	vecStepRight = gpGlobals->v_right * COVER_DELTA;
	vecStepRight.z = 0; 
	
	vecLeftTest = vecRightTest = pev->origin;

	for ( i = 0 ; i < COVER_CHECKS ; i++ )
	{
		vecLeftTest = vecLeftTest - vecStepRight;
		vecRightTest = vecRightTest + vecStepRight;

		// it's faster to check the SightEnt's visibility to the potential spot than to check the local move, so we do that first.
		UTIL_TraceLine( vecThreat + vecViewOffset, vecLeftTest + pev->view_ofs, ignore_monsters, ignore_glass, ENT(pev)/*pentIgnore*/, &tr);
		
		if (tr.flFraction != 1.0)
		{
			if ( FValidateCover ( vecLeftTest ) && CheckLocalMove( pev->origin, vecLeftTest, NULL, NULL ) == LOCALMOVE_VALID )
			{
				if ( MoveToLocation( ACT_RUN, 0, vecLeftTest ) )
				{
					return TRUE;
				}
			}
		}
		
		// it's faster to check the SightEnt's visibility to the potential spot than to check the local move, so we do that first.
		UTIL_TraceLine(vecThreat + vecViewOffset, vecRightTest + pev->view_ofs, ignore_monsters, ignore_glass, ENT(pev)/*pentIgnore*/, &tr);
		
		if ( tr.flFraction != 1.0 )
		{
			if (  FValidateCover ( vecRightTest ) && CheckLocalMove( pev->origin, vecRightTest, NULL, NULL ) == LOCALMOVE_VALID )
			{
				if ( MoveToLocation( ACT_RUN, 0, vecRightTest ) )
				{
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}


Vector CBaseMonster :: ShootAtEnemy( const Vector &shootOrigin )
{
	CBaseEntity *pEnemy = m_hEnemy;

	if ( pEnemy )
	{
		return ( (pEnemy->BodyTarget( shootOrigin ) - pEnemy->pev->origin) + m_vecEnemyLKP - shootOrigin ).Normalize();
	}
	else
		return gpGlobals->v_forward;
}



//=========================================================
// FacingIdeal - tells us if a monster is facing its ideal
// yaw. Created this function because many spots in the 
// code were checking the yawdiff against this magic
// number. Nicer to have it in one place if we're gonna
// be stuck with it.
//=========================================================
BOOL CBaseMonster :: FacingIdeal( void )
{
	if ( fabs( FlYawDiff() ) <= 0.006 )//!!!BUGBUG - no magic numbers!!!
	{
		return TRUE;
	}

	return FALSE;
}

//=========================================================
// FCanActiveIdle
//=========================================================
BOOL CBaseMonster :: FCanActiveIdle ( void )
{
	/*
	if ( m_MonsterState == MONSTERSTATE_IDLE && m_IdealMonsterState == MONSTERSTATE_IDLE && !IsMoving() )
	{
		return TRUE;
	}
	*/
	return FALSE;
}


void CBaseMonster::PlaySentence( char *pszSentence, float duration, float volume, float attenuation )
{
	if ( pszSentence && IsAlive() )
	{
		if ( pszSentence[0] == '!' )
			EMIT_SOUND_DYN( edict(), CHAN_VOICE, pszSentence, volume, attenuation, 0, PITCH_NORM );
		else
			SENTENCEG_PlayRndSz( edict(), pszSentence, volume, attenuation, 0, PITCH_NORM );
	}
}


void CBaseMonster::PlayScriptedSentence( char *pszSentence, float duration, float volume, float attenuation, BOOL bConcurrent, CBaseEntity *pListener )
{ 
	PlaySentence( pszSentence, duration, volume, attenuation );
}


void CBaseMonster::SentenceStop( void )
{
	EMIT_SOUND( edict(), CHAN_VOICE, "common/null.wav", 1.0, ATTN_IDLE );
}

void SetSequenceBox( CBaseMonster *pMonster )
{
	Vector mins, maxs;

	// Get sequence bbox
	if ( ExtractBbox( GET_MODEL_PTR( ENT(pMonster->pev) ), pMonster->pev->sequence, mins, maxs ) )
	{
		// expand box for rotation
		// find min / max for rotations
		float yaw = pMonster->pev->angles.y * (M_PI / 180.0);
		
		Vector xvector, yvector;
		xvector.x = cos(yaw);
		xvector.y = sin(yaw);
		yvector.x = -sin(yaw);
		yvector.y = cos(yaw);
		Vector bounds[2];

		bounds[0] = mins;
		bounds[1] = maxs;
		
		Vector rmin( 9999, 9999, 9999 );
		Vector rmax( -9999, -9999, -9999 );
		Vector base, transformed;

		for (int i = 0; i <= 1; i++ )
		{
			base.x = bounds[i].x;
			for ( int j = 0; j <= 1; j++ )
			{
				base.y = bounds[j].y;
				for ( int k = 0; k <= 1; k++ )
				{
					base.z = bounds[k].z;
					
				// transform the point
					transformed.x = xvector.x*base.x + yvector.x*base.y;
					transformed.y = xvector.y*base.x + yvector.y*base.y;
					transformed.z = base.z;
					
					for ( int l = 0; l < 3; l++ )
					{
						if (transformed[l] < rmin[l])
							rmin[l] = transformed[l];
						if (transformed[l] > rmax[l])
							rmax[l] = transformed[l];
					}
				}
			}
		}
		rmin.z = 0;
		rmax.z = rmin.z + 1;
		UTIL_SetSize( pMonster->pev, rmin, rmax );
	}
}

void CBaseMonster::CorpseFallThink( void )
{
	if ( pev->flags & FL_ONGROUND )
	{
		SetThink ( NULL );

		SetSequenceBox( this );
		UTIL_SetOrigin( pev, pev->origin );// link into world.
	}
	else
		pev->nextthink = gpGlobals->time + 0.1;
}

// Call after animation/pose is set up
void CBaseMonster :: MonsterInitDead( void )
{
	InitBoneControllers();

	pev->solid			= SOLID_BBOX;
	pev->movetype		= MOVETYPE_TOSS;// so he'll fall to ground

	pev->frame = 0;
	ResetSequenceInfo( );
	pev->framerate = 0;
	
	// Copy health
	pev->max_health		= pev->health;
	pev->deadflag		= DEAD_DEAD;
	
	UTIL_SetSize(pev, g_vecZero, g_vecZero );
	UTIL_SetOrigin( pev, pev->origin );

	// Setup health counters, etc.
	BecomeDead();
	SetThink( &CBaseMonster::CorpseFallThink );
	pev->nextthink = gpGlobals->time + 0.5;
}

//=========================================================
// BBoxIsFlat - check to see if the monster's bounding box
// is lying flat on a surface (traces from all four corners
// are same length.)
//=========================================================
BOOL CBaseMonster :: BBoxFlat ( void )
{
	TraceResult	tr;
	Vector		vecPoint;
	float		flXSize, flYSize;
	float		flLength;
	float		flLength2;

	flXSize = pev->size.x / 2;
	flYSize = pev->size.y / 2;

	vecPoint.x = pev->origin.x + flXSize;
	vecPoint.y = pev->origin.y + flYSize;
	vecPoint.z = pev->origin.z;

	UTIL_TraceLine ( vecPoint, vecPoint - Vector ( 0, 0, 100 ), ignore_monsters, ENT(pev), &tr );
	flLength = (vecPoint - tr.vecEndPos).Length();

	vecPoint.x = pev->origin.x - flXSize;
	vecPoint.y = pev->origin.y - flYSize;

	UTIL_TraceLine ( vecPoint, vecPoint - Vector ( 0, 0, 100 ), ignore_monsters, ENT(pev), &tr );
	flLength2 = (vecPoint - tr.vecEndPos).Length();
	if ( flLength2 > flLength )
	{
		return FALSE;
	}
	flLength = flLength2;

	vecPoint.x = pev->origin.x - flXSize;
	vecPoint.y = pev->origin.y + flYSize;
	UTIL_TraceLine ( vecPoint, vecPoint - Vector ( 0, 0, 100 ), ignore_monsters, ENT(pev), &tr );
	flLength2 = (vecPoint - tr.vecEndPos).Length();
	if ( flLength2 > flLength )
	{
		return FALSE;
	}
	flLength = flLength2;

	vecPoint.x = pev->origin.x + flXSize;
	vecPoint.y = pev->origin.y - flYSize;
	UTIL_TraceLine ( vecPoint, vecPoint - Vector ( 0, 0, 100 ), ignore_monsters, ENT(pev), &tr );
	flLength2 = (vecPoint - tr.vecEndPos).Length();
	if ( flLength2 > flLength )
	{
		return FALSE;
	}
	flLength = flLength2;

	return TRUE;
}

//=========================================================
// Get Enemy - tries to find the best suitable enemy for the monster.
//=========================================================
BOOL CBaseMonster :: GetEnemy ( void )
{
	CBaseEntity *pNewEnemy;

	if ( HasConditions(bits_COND_SEE_HATE | bits_COND_SEE_DISLIKE | bits_COND_SEE_NEMESIS) )
	{
		pNewEnemy = BestVisibleEnemy();

		if ( pNewEnemy != m_hEnemy && pNewEnemy != NULL)
		{
			// DO NOT mess with the monster's m_hEnemy pointer unless the schedule the monster is currently running will be interrupted
			// by COND_NEW_ENEMY. This will eliminate the problem of monsters getting a new enemy while they are in a schedule that doesn't care,
			// and then not realizing it by the time they get to a schedule that does. I don't feel this is a good permanent fix. 

			if ( m_pSchedule )
			{
				if ( m_pSchedule->iInterruptMask & bits_COND_NEW_ENEMY )
				{
					PushEnemy( m_hEnemy, m_vecEnemyLKP );
					SetConditions(bits_COND_NEW_ENEMY);
					m_hEnemy = pNewEnemy;
					m_vecEnemyLKP = m_hEnemy->pev->origin;
				}
				// if the new enemy has an owner, take that one as well
				if (pNewEnemy->pev->owner != NULL)
				{
					CBaseEntity *pOwner = GetMonsterPointer( pNewEnemy->pev->owner );
					if ( pOwner && (pOwner->pev->flags & FL_MONSTER) && IRelationship( pOwner ) != R_NO )
						PushEnemy( pOwner, m_vecEnemyLKP );
				}
			}
		}
	}

	// remember old enemies
	if (m_hEnemy == NULL && PopEnemy( ))
	{
		if ( m_pSchedule )
		{
			if ( m_pSchedule->iInterruptMask & bits_COND_NEW_ENEMY )
			{
				SetConditions(bits_COND_NEW_ENEMY);
			}
		}
	}

	if ( m_hEnemy != NULL )
	{
		// monster has an enemy.
		return TRUE;
	}

	return FALSE;// monster has no enemy
}

//=========================================================
// TraceAttack
//=========================================================
void CBaseMonster :: TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType)
{
	if ( pev->takedamage )
	{
		m_LastHitGroup = ptr->iHitgroup;

		switch ( ptr->iHitgroup )
		{
		case HITGROUP_GENERIC:
			break;
		case HITGROUP_HEAD:
			flDamage *= gSkillData.monHead;
			break;
		case HITGROUP_CHEST:
			flDamage *= gSkillData.monChest;
			break;
		case HITGROUP_STOMACH:
			flDamage *= gSkillData.monStomach;
			break;
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
			flDamage *= gSkillData.monArm;
			break;
		case HITGROUP_LEFTLEG:
		case HITGROUP_RIGHTLEG:
			flDamage *= gSkillData.monLeg;
			break;
		default:
			break;
		}

		SpawnBlood(ptr->vecEndPos, BloodColor(), flDamage);// a little surface blood.
		TraceBleed( flDamage, vecDir, ptr, bitsDamageType );
		AddMultiDamage( pevAttacker, this, flDamage, bitsDamageType );
	}
}

//=========================================================
// DropItem - dead monster drops named item 
//=========================================================
CBaseEntity* CBaseMonster :: DropItem ( char *pszItemName, const Vector &vecPos, const Vector &vecAng )
{
	if ( !pszItemName )
	{
		ALERT ( at_console, "DropItem() - No item name!\n" );
		return NULL;
	}

	CBaseEntity *pItem = CBaseEntity::Create( pszItemName, vecPos, vecAng, edict() );

	if ( pItem )
	{
		// do we want this behavior to be default?! (sjb)
		pItem->pev->velocity = pev->velocity;
		pItem->pev->avelocity = Vector ( 0, RANDOM_FLOAT( 0, 100 ), 0 );
		return pItem;
	}
	else
	{
		ALERT ( at_console, "DropItem() - Didn't create!\n" );
		return FALSE;
	}

}


BOOL CBaseMonster :: ShouldFadeOnDeath( void )
{
	// if flagged to fade out or I have an owner (I came from a monster spawner)
	if ( (pev->spawnflags & SF_MONSTER_FADECORPSE) || !FNullEnt( pev->owner ) )
		return TRUE;

	return FALSE;
}

//=========================================================
// FHaveSchedule - Returns TRUE if monster's m_pSchedule
// is anything other than NULL.
//=========================================================
BOOL CBaseMonster :: FHaveSchedule( void )
{
	if ( m_pSchedule == NULL )
	{
		return FALSE;
	}

	return TRUE;
}

//=========================================================
// ClearSchedule - blanks out the caller's schedule pointer
// and index.
//=========================================================
void CBaseMonster :: ClearSchedule( void )
{
	m_iTaskStatus = TASKSTATUS_NEW;
	m_pSchedule = NULL;
	m_iScheduleIndex = 0;
}

//=========================================================
// FScheduleDone - Returns TRUE if the caller is on the
// last task in the schedule
//=========================================================
BOOL CBaseMonster :: FScheduleDone ( void )
{
	ASSERT( m_pSchedule != NULL );
	
	if ( m_iScheduleIndex == m_pSchedule->cTasks )
	{
		return TRUE;
	}

	return FALSE;
}

//=========================================================
// ChangeSchedule - replaces the monster's schedule pointer
// with the passed pointer, and sets the ScheduleIndex back
// to 0
//=========================================================
void CBaseMonster :: ChangeSchedule ( Schedule_t *pNewSchedule )
{
	ASSERT( pNewSchedule != NULL );

	m_pSchedule			= pNewSchedule;
	m_iScheduleIndex	= 0;
	m_iTaskStatus		= TASKSTATUS_NEW;
	m_afConditions		= 0;// clear all of the conditions
	m_failSchedule		= SCHED_NONE;

	if ( m_pSchedule->iInterruptMask & bits_COND_HEAR_SOUND && !(m_pSchedule->iSoundMask) )
	{
		ALERT ( at_aiconsole, "COND_HEAR_SOUND with no sound mask!\n" );
	}
	else if ( m_pSchedule->iSoundMask && !(m_pSchedule->iInterruptMask & bits_COND_HEAR_SOUND) )
	{
		ALERT ( at_aiconsole, "Sound mask without COND_HEAR_SOUND!\n" );
	}

#if _DEBUG
	if ( !ScheduleFromName( pNewSchedule->pName ) )
	{
		ALERT( at_console, "Schedule %s not in table!!!\n", pNewSchedule->pName );
	}
#endif
	
// this is very useful code if you can isolate a test case in a level with a single monster. It will notify
// you of every schedule selection the monster makes.
#if 0
	if ( FClassnameIs( pev, "monster_human_grunt" ) )
	{
		Task_t *pTask = GetTask();
		
		if ( pTask )
		{
			const char *pName = NULL;

			if ( m_pSchedule )
			{
				pName = m_pSchedule->pName;
			}
			else
			{
				pName = "No Schedule";
			}
			
			if ( !pName )
			{
				pName = "Unknown";
			}

			ALERT( at_aiconsole, "%s: picked schedule %s\n", STRING( pev->classname ), pName );
		}
	}
#endif// 0

}

//=========================================================
// NextScheduledTask - increments the ScheduleIndex
//=========================================================
void CBaseMonster :: NextScheduledTask ( void )
{
	ASSERT( m_pSchedule != NULL );

	m_iTaskStatus = TASKSTATUS_NEW;
	m_iScheduleIndex++;

	if ( FScheduleDone() )
	{
		// just completed last task in schedule, so make it invalid by clearing it.
		SetConditions( bits_COND_SCHEDULE_DONE );
		//ClearSchedule();	
	}
}

//=========================================================
// IScheduleFlags - returns an integer with all Conditions
// bits that are currently set and also set in the current
// schedule's Interrupt mask.
//=========================================================
int CBaseMonster :: IScheduleFlags ( void )
{
	if( !m_pSchedule )
	{
		return 0;
	}
	
	// strip off all bits excepts the ones capable of breaking this schedule.
	return m_afConditions & m_pSchedule->iInterruptMask;
}

//=========================================================
// FScheduleValid - returns TRUE as long as the current
// schedule is still the proper schedule to be executing,
// taking into account all conditions
//=========================================================
BOOL CBaseMonster :: FScheduleValid ( void )
{
	if ( m_pSchedule == NULL )
	{
		// schedule is empty, and therefore not valid.
		return FALSE;
	}

	if ( HasConditions( m_pSchedule->iInterruptMask | bits_COND_SCHEDULE_DONE | bits_COND_TASK_FAILED ) )
	{
#ifdef DEBUG
		if ( HasConditions ( bits_COND_TASK_FAILED ) && m_failSchedule == SCHED_NONE )
		{
			// fail! Send a visual indicator.
			ALERT ( at_aiconsole, "Schedule: %s Failed\n", m_pSchedule->pName );

			Vector tmp = pev->origin;
			tmp.z = pev->absmax.z + 16;
			UTIL_Sparks( tmp );
		}
#endif // DEBUG

		// some condition has interrupted the schedule, or the schedule is done
		return FALSE;
	}
	
	return TRUE;
}

//=========================================================
// MaintainSchedule - does all the per-think schedule maintenance.
// ensures that the monster leaves this function with a valid
// schedule!
//=========================================================
void CBaseMonster :: MaintainSchedule ( void )
{
	Schedule_t	*pNewSchedule;
	int			i;

	// UNDONE: Tune/fix this 10... This is just here so infinite loops are impossible
	for ( i = 0; i < 10; i++ )
	{
		if ( m_pSchedule != NULL && TaskIsComplete() )
		{
			NextScheduledTask();                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
		}

	// validate existing schedule 
		if ( !FScheduleValid() || m_MonsterState != m_IdealMonsterState )
		{
			// if we come into this block of code, the schedule is going to have to be changed.
			// if the previous schedule was interrupted by a condition, GetIdealState will be 
			// called. Else, a schedule finished normally.

			// Notify the monster that his schedule is changing
			ScheduleChange();

			// Call GetIdealState if we're not dead and one or more of the following...
			// - in COMBAT state with no enemy (it died?)
			// - conditions bits (excluding SCHEDULE_DONE) indicate interruption,
			// - schedule is done but schedule indicates it wants GetIdealState called
			//   after successful completion (by setting bits_COND_SCHEDULE_DONE in iInterruptMask)
			// DEAD & SCRIPT are not suggestions, they are commands!
			if ( m_IdealMonsterState != MONSTERSTATE_DEAD && 
				 (m_IdealMonsterState != MONSTERSTATE_SCRIPT || m_IdealMonsterState == m_MonsterState) )
			{
				if (	(m_afConditions && !HasConditions(bits_COND_SCHEDULE_DONE)) ||
						(m_pSchedule && (m_pSchedule->iInterruptMask & bits_COND_SCHEDULE_DONE)) ||
						((m_MonsterState == MONSTERSTATE_COMBAT) && (m_hEnemy == NULL))	)
				{
					GetIdealState();
				}
			}
			if ( HasConditions( bits_COND_TASK_FAILED ) && m_MonsterState == m_IdealMonsterState )
			{
				if ( m_failSchedule != SCHED_NONE )
					pNewSchedule = GetScheduleOfType( m_failSchedule );
				else
					pNewSchedule = GetScheduleOfType( SCHED_FAIL );
				// schedule was invalid because the current task failed to start or complete
				ALERT ( at_aiconsole, "Schedule Failed at %d!\n", m_iScheduleIndex );
				ChangeSchedule( pNewSchedule );
			}
			else
			{
				SetState( m_IdealMonsterState );
				if ( m_MonsterState == MONSTERSTATE_SCRIPT || m_MonsterState == MONSTERSTATE_DEAD )
					pNewSchedule = CBaseMonster::GetSchedule();
				else
					pNewSchedule = GetSchedule();
				ChangeSchedule( pNewSchedule );
			}
		}

		if ( m_iTaskStatus == TASKSTATUS_NEW )
		{	
			Task_t *pTask = GetTask();
			ASSERT( pTask != NULL );
			TaskBegin();
			StartTask( pTask );
		}

		// UNDONE: Twice?!!!
		if ( m_Activity != m_IdealActivity )
		{
			SetActivity ( m_IdealActivity );
		}
		
		if ( !TaskIsComplete() && m_iTaskStatus != TASKSTATUS_NEW )
			break;
	}

	if ( TaskIsRunning() )
	{
		Task_t *pTask = GetTask();
		ASSERT( pTask != NULL );
		RunTask( pTask );
	}

	// UNDONE: We have to do this so that we have an animation set to blend to if RunTask changes the animation
	// RunTask() will always change animations at the end of a script!
	// Don't do this twice
	if ( m_Activity != m_IdealActivity )
	{
		SetActivity ( m_IdealActivity );
	}
}

//=========================================================
// RunTask 
//=========================================================
void CBaseMonster :: RunTask ( Task_t *pTask )
{
	switch ( pTask->iTask )
	{
	case TASK_TURN_RIGHT:
	case TASK_TURN_LEFT:
		{
			ChangeYaw( pev->yaw_speed );

			if ( FacingIdeal() )
			{
				TaskComplete();
			}
			break;
		}

	case TASK_PLAY_SEQUENCE_FACE_ENEMY:
	case TASK_PLAY_SEQUENCE_FACE_TARGET:
		{
			CBaseEntity *pTarget;

			if ( pTask->iTask == TASK_PLAY_SEQUENCE_FACE_TARGET )
				pTarget = m_hTargetEnt;
			else
				pTarget = m_hEnemy;
			if ( pTarget )
			{
				pev->ideal_yaw = UTIL_VecToYaw( pTarget->pev->origin - pev->origin );
				ChangeYaw( pev->yaw_speed );
			}
			if ( m_fSequenceFinished )
				TaskComplete();
		}
		break;

	case TASK_PLAY_SEQUENCE:
	case TASK_PLAY_ACTIVE_IDLE:
		{
			if ( m_fSequenceFinished )
			{
				TaskComplete();
			}
			break;
		}


	case TASK_FACE_ENEMY:
		{
			MakeIdealYaw( m_vecEnemyLKP );

			ChangeYaw( pev->yaw_speed );

			if ( FacingIdeal() )
			{
				TaskComplete();
			}
			break;
		}
	case TASK_FACE_HINTNODE:
	case TASK_FACE_LASTPOSITION:
	case TASK_FACE_TARGET:
	case TASK_FACE_IDEAL:
	case TASK_FACE_ROUTE:
		{
			ChangeYaw( pev->yaw_speed );

			if ( FacingIdeal() )
			{
				TaskComplete();
			}
			break;
		}
	case TASK_WAIT_PVS:
		{
			if ( !FNullEnt(FIND_CLIENT_IN_PVS(edict())) )
			{
				TaskComplete();
			}
			break;
		}
	case TASK_WAIT_INDEFINITE:
		{
			// don't do anything.
			break;
		}
	case TASK_WAIT:
	case TASK_WAIT_RANDOM:
		{
			if ( gpGlobals->time >= m_flWaitFinished )
			{
				TaskComplete();
			}
			break;
		}
	case TASK_WAIT_FACE_ENEMY:
		{
			MakeIdealYaw ( m_vecEnemyLKP );
			ChangeYaw( pev->yaw_speed ); 

			if ( gpGlobals->time >= m_flWaitFinished )
			{
				TaskComplete();
			}
			break;
		}
	case TASK_MOVE_TO_TARGET_RANGE:
		{
			float distance;

			if ( m_hTargetEnt == NULL )
				TaskFail();
			else
			{
				distance = ( m_vecMoveGoal - pev->origin ).Length2D();
				// Re-evaluate when you think your finished, or the target has moved too far
				if ( (distance < pTask->flData) || (m_vecMoveGoal - m_hTargetEnt->pev->origin).Length() > pTask->flData * 0.5 )
				{
					m_vecMoveGoal = m_hTargetEnt->pev->origin;
					distance = ( m_vecMoveGoal - pev->origin ).Length2D();
					FRefreshRoute();
				}

				// Set the appropriate activity based on an overlapping range
				// overlap the range to prevent oscillation
				if ( distance < pTask->flData )
				{
					TaskComplete();
					RouteClear();		// Stop moving
				}
				else if ( distance < 190 && m_movementActivity != ACT_WALK )
					m_movementActivity = ACT_WALK;
				else if ( distance >= 270 && m_movementActivity != ACT_RUN )
					m_movementActivity = ACT_RUN;
			}

			break;
		}
	case TASK_WAIT_FOR_MOVEMENT:
		{
			if (MovementIsComplete())
			{
				TaskComplete();
				RouteClear();		// Stop moving
			}
			break;
		}
	case TASK_DIE:
		{
			if ( m_fSequenceFinished && pev->frame >= 255 )
			{
				pev->deadflag = DEAD_DEAD;
				
				SetThink ( NULL );
				StopAnimation();

				if ( !BBoxFlat() )
				{
					// a bit of a hack. If a corpses' bbox is positioned such that being left solid so that it can be attacked will
					// block the player on a slope or stairs, the corpse is made nonsolid. 
//					pev->solid = SOLID_NOT;
					UTIL_SetSize ( pev, Vector ( -4, -4, 0 ), Vector ( 4, 4, 1 ) );
				}
				else // !!!HACKHACK - put monster in a thin, wide bounding box until we fix the solid type/bounding volume problem
					UTIL_SetSize ( pev, Vector ( pev->mins.x, pev->mins.y, pev->mins.z ), Vector ( pev->maxs.x, pev->maxs.y, pev->mins.z + 1 ) );

				if ( ShouldFadeOnDeath() )
				{
					// this monster was created by a monstermaker... fade the corpse out.
					SUB_StartFadeOut();
				}
				else
				{
					// body is gonna be around for a while, so have it stink for a bit.
					CSoundEnt::InsertSound ( bits_SOUND_CARCASS, pev->origin, 384, 30 );
				}
			}
			break;
		}
	case TASK_RANGE_ATTACK1_NOTURN:
	case TASK_MELEE_ATTACK1_NOTURN:
	case TASK_MELEE_ATTACK2_NOTURN:
	case TASK_RANGE_ATTACK2_NOTURN:
	case TASK_RELOAD_NOTURN:
		{
			if ( m_fSequenceFinished )
			{
				m_Activity = ACT_RESET;
				TaskComplete();
			}
			break;
		}
	case TASK_RANGE_ATTACK1:
	case TASK_MELEE_ATTACK1:
	case TASK_MELEE_ATTACK2:
	case TASK_RANGE_ATTACK2:
	case TASK_SPECIAL_ATTACK1:
	case TASK_SPECIAL_ATTACK2:
	case TASK_RELOAD:
		{
			MakeIdealYaw ( m_vecEnemyLKP );
			ChangeYaw ( pev->yaw_speed );

			if ( m_fSequenceFinished )
			{
				m_Activity = ACT_RESET;
				TaskComplete();
			}
			break;
		}
	case TASK_SMALL_FLINCH:
		{
			if ( m_fSequenceFinished )
			{
				TaskComplete();
			}
		}
		break;
	case TASK_WAIT_FOR_SCRIPT:
		{
			if ( m_pCine->m_iDelay <= 0 && gpGlobals->time >= m_pCine->m_startTime )
			{
				TaskComplete();
				m_pCine->StartSequence( (CBaseMonster *)this, m_pCine->m_iszPlay, TRUE );
				if ( m_fSequenceFinished )
					ClearSchedule();
				pev->framerate = 1.0;
				//ALERT( at_aiconsole, "Script %s has begun for %s\n", STRING( m_pCine->m_iszPlay ), STRING(pev->classname) );
			}
			break;
		}
	case TASK_PLAY_SCRIPT:
		{
			if (m_fSequenceFinished)
			{
				m_pCine->SequenceDone( this );
			}
			break;
		}
	}
}

//=========================================================
// SetTurnActivity - measures the difference between the way
// the monster is facing and determines whether or not to
// select one of the 180 turn animations.
//=========================================================
void CBaseMonster :: SetTurnActivity ( void )
{
	float flYD;
	flYD = FlYawDiff();

	if ( flYD <= -45 && LookupActivity ( ACT_TURN_RIGHT ) != ACTIVITY_NOT_AVAILABLE )
	{// big right turn
		m_IdealActivity = ACT_TURN_RIGHT;
	}
	else if ( flYD > 45 && LookupActivity ( ACT_TURN_LEFT ) != ACTIVITY_NOT_AVAILABLE )
	{// big left turn
		m_IdealActivity = ACT_TURN_LEFT;
	}
}

//=========================================================
// Start task - selects the correct activity and performs
// any necessary calculations to start the next task on the
// schedule. 
//=========================================================
void CBaseMonster :: StartTask ( Task_t *pTask )
{
	switch ( pTask->iTask )
	{
	case TASK_TURN_RIGHT:
		{
			float flCurrentYaw;
			
			flCurrentYaw = UTIL_AngleMod( pev->angles.y );
			pev->ideal_yaw = UTIL_AngleMod( flCurrentYaw - pTask->flData );
			SetTurnActivity();
			break;
		}
	case TASK_TURN_LEFT:
		{
			float flCurrentYaw;
			
			flCurrentYaw = UTIL_AngleMod( pev->angles.y );
			pev->ideal_yaw = UTIL_AngleMod( flCurrentYaw + pTask->flData );
			SetTurnActivity();
			break;
		}
	case TASK_REMEMBER:
		{
			Remember ( (int)pTask->flData );
			TaskComplete();
			break;
		}
	case TASK_FORGET:
		{
			Forget ( (int)pTask->flData );
			TaskComplete();
			break;
		}
	case TASK_FIND_HINTNODE:
		{
			m_iHintNode = FindHintNode();

			if ( m_iHintNode != NO_NODE )
			{
				TaskComplete();
			}
			else
			{
				TaskFail();
			}
			break;
		}
	case TASK_STORE_LASTPOSITION:
		{
			m_vecLastPosition = pev->origin;
			TaskComplete();
			break;
		}
	case TASK_CLEAR_LASTPOSITION:
		{
			m_vecLastPosition = g_vecZero;
			TaskComplete();
			break;
		}
	case TASK_CLEAR_HINTNODE:
		{
			m_iHintNode = NO_NODE;
			TaskComplete();
			break;
		}
	case TASK_STOP_MOVING:
		{
			if ( m_IdealActivity == m_movementActivity )
			{
				m_IdealActivity = GetStoppedActivity();
			}

			RouteClear();
			TaskComplete();
			break;
		}
	case TASK_PLAY_SEQUENCE_FACE_ENEMY:
	case TASK_PLAY_SEQUENCE_FACE_TARGET:
	case TASK_PLAY_SEQUENCE:
		{
			m_IdealActivity = ( Activity )( int )pTask->flData;
			break;
		}
	case TASK_PLAY_ACTIVE_IDLE:
		{
			// monsters verify that they have a sequence for the node's activity BEFORE
			// moving towards the node, so it's ok to just set the activity without checking here.
			m_IdealActivity = ( Activity )WorldGraph.m_pNodes[ m_iHintNode ].m_sHintActivity;
			break;
		}
	case TASK_SET_SCHEDULE:
		{
			Schedule_t *pNewSchedule;

			pNewSchedule = GetScheduleOfType( (int)pTask->flData );
			
			if ( pNewSchedule )
			{
				ChangeSchedule( pNewSchedule );
			}
			else
			{
				TaskFail();
			}

			break;
		}
	case TASK_FIND_NEAR_NODE_COVER_FROM_ENEMY:
		{
			if ( m_hEnemy == NULL )
			{
				TaskFail();
				return;
			}

			if ( FindCover( m_hEnemy->pev->origin, m_hEnemy->pev->view_ofs, 0, pTask->flData ) )
			{
				// try for cover farther than the FLData from the schedule.
				TaskComplete();
			}
			else
			{
				// no coverwhatsoever.
				TaskFail();
			}
			break;
		}
	case TASK_FIND_FAR_NODE_COVER_FROM_ENEMY:
		{
			if ( m_hEnemy == NULL )
			{
				TaskFail();
				return;
			}

			if ( FindCover( m_hEnemy->pev->origin, m_hEnemy->pev->view_ofs, pTask->flData, CoverRadius() ) )
			{
				// try for cover farther than the FLData from the schedule.
				TaskComplete();
			}
			else
			{
				// no coverwhatsoever.
				TaskFail();
			}
			break;
		}
	case TASK_FIND_NODE_COVER_FROM_ENEMY:
		{
			if ( m_hEnemy == NULL )
			{
				TaskFail();
				return;
			}

			if ( FindCover( m_hEnemy->pev->origin, m_hEnemy->pev->view_ofs, 0, CoverRadius() ) )
			{
				// try for cover farther than the FLData from the schedule.
				TaskComplete();
			}
			else
			{
				// no coverwhatsoever.
				TaskFail();
			}
			break;
		}
	case TASK_FIND_COVER_FROM_ENEMY:
		{
			entvars_t *pevCover;

			if ( m_hEnemy == NULL )
			{
				// Find cover from self if no enemy available
				pevCover = pev;
//				TaskFail();
//				return;
			}
			else
				pevCover = m_hEnemy->pev;

			if ( FindLateralCover( pevCover->origin, pevCover->view_ofs ) )
			{
				// try lateral first
				m_flMoveWaitFinished = gpGlobals->time + pTask->flData;
				TaskComplete();
			}
			else if ( FindCover( pevCover->origin, pevCover->view_ofs, 0, CoverRadius() ) )
			{
				// then try for plain ole cover
				m_flMoveWaitFinished = gpGlobals->time + pTask->flData;
				TaskComplete();
			}
			else
			{
				// no coverwhatsoever.
				TaskFail();
			}
			break;
		}
	case TASK_FIND_COVER_FROM_ORIGIN:
		{
			if ( FindCover( pev->origin, pev->view_ofs, 0, CoverRadius() ) )
			{
				// then try for plain ole cover
				m_flMoveWaitFinished = gpGlobals->time + pTask->flData;
				TaskComplete();
			}
			else
			{
				// no cover!
				TaskFail();
			}
		}
		break;
	case TASK_FIND_COVER_FROM_BEST_SOUND:
		{
			CSound *pBestSound;

			pBestSound = PBestSound();

			ASSERT( pBestSound != NULL );
			/*
			if ( pBestSound && FindLateralCover( pBestSound->m_vecOrigin, g_vecZero ) )
			{
				// try lateral first
				m_flMoveWaitFinished = gpGlobals->time + pTask->flData;
				TaskComplete();
			}
			*/

			if ( pBestSound && FindCover( pBestSound->m_vecOrigin, g_vecZero, pBestSound->m_iVolume, CoverRadius() ) )
			{
				// then try for plain ole cover
				m_flMoveWaitFinished = gpGlobals->time + pTask->flData;
				TaskComplete();
			}
			else
			{
				// no coverwhatsoever. or no sound in list
				TaskFail();
			}
			break;
		}
	case TASK_FACE_HINTNODE:
		{
			pev->ideal_yaw = WorldGraph.m_pNodes[ m_iHintNode ].m_flHintYaw;
			SetTurnActivity();
			break;
		}
	
	case TASK_FACE_LASTPOSITION:
		MakeIdealYaw ( m_vecLastPosition );
		SetTurnActivity(); 
		break;

	case TASK_FACE_TARGET:
		if ( m_hTargetEnt != NULL )
		{
			MakeIdealYaw ( m_hTargetEnt->pev->origin );
			SetTurnActivity(); 
		}
		else
			TaskFail();
		break;
	case TASK_FACE_ENEMY:
		{
			MakeIdealYaw ( m_vecEnemyLKP );
			SetTurnActivity(); 
			break;
		}
	case TASK_FACE_IDEAL:
		{
			SetTurnActivity();
			break;
		}
	case TASK_FACE_ROUTE:
		{
			if (FRouteClear())
			{
				ALERT(at_aiconsole, "No route to face!\n");
				TaskFail();
			}
			else
			{
				MakeIdealYaw(m_Route[m_iRouteIndex].vecLocation);
				SetTurnActivity();
			}
			break;
		}
	case TASK_WAIT_PVS:
	case TASK_WAIT_INDEFINITE:
		{
			// don't do anything.
			break;
		}
	case TASK_WAIT:
	case TASK_WAIT_FACE_ENEMY:
		{// set a future time that tells us when the wait is over.
			m_flWaitFinished = gpGlobals->time + pTask->flData;	
			break;
		}
	case TASK_WAIT_RANDOM:
		{// set a future time that tells us when the wait is over.
			m_flWaitFinished = gpGlobals->time + RANDOM_FLOAT( 0.1, pTask->flData );
			break;
		}
	case TASK_MOVE_TO_TARGET_RANGE:
		{
			if ( (m_hTargetEnt->pev->origin - pev->origin).Length() < 1 )
				TaskComplete();
			else
			{
				m_vecMoveGoal = m_hTargetEnt->pev->origin;
				if ( !MoveToTarget( ACT_WALK, 2 ) )
					TaskFail();
			}
			break;
		}
	case TASK_RUN_TO_TARGET:
	case TASK_WALK_TO_TARGET:
		{
			Activity newActivity;

			if ( (m_hTargetEnt->pev->origin - pev->origin).Length() < 1 )
				TaskComplete();
			else
			{
				if ( pTask->iTask == TASK_WALK_TO_TARGET )
					newActivity = ACT_WALK;
				else
					newActivity = ACT_RUN;
				// This monster can't do this!
				if ( LookupActivity( newActivity ) == ACTIVITY_NOT_AVAILABLE )
					TaskComplete();
				else 
				{
					if ( m_hTargetEnt == NULL || !MoveToTarget( newActivity, 2 ) )
					{
						TaskFail();
						ALERT( at_aiconsole, "%s Failed to reach target!!!\n", STRING(pev->classname) );
						RouteClear();
					}
				}
			}
			TaskComplete();
			break;
		}
	case TASK_CLEAR_MOVE_WAIT:
		{
			m_flMoveWaitFinished = gpGlobals->time;
			TaskComplete();
			break;
		}
	case TASK_MELEE_ATTACK1_NOTURN:
	case TASK_MELEE_ATTACK1:
		{
			m_IdealActivity = ACT_MELEE_ATTACK1;
			break;
		}
	case TASK_MELEE_ATTACK2_NOTURN:
	case TASK_MELEE_ATTACK2:
		{
			m_IdealActivity = ACT_MELEE_ATTACK2;
			break;
		}
	case TASK_RANGE_ATTACK1_NOTURN:
	case TASK_RANGE_ATTACK1:
		{
			m_IdealActivity = ACT_RANGE_ATTACK1;
			break;
		}
	case TASK_RANGE_ATTACK2_NOTURN:
	case TASK_RANGE_ATTACK2:
		{
			m_IdealActivity = ACT_RANGE_ATTACK2;
			break;
		}
	case TASK_RELOAD_NOTURN:
	case TASK_RELOAD:
		{
			m_IdealActivity = ACT_RELOAD;
			break;
		}
	case TASK_SPECIAL_ATTACK1:
		{
			m_IdealActivity = ACT_SPECIAL_ATTACK1;
			break;
		}
	case TASK_SPECIAL_ATTACK2:
		{
			m_IdealActivity = ACT_SPECIAL_ATTACK2;
			break;
		}
	case TASK_SET_ACTIVITY:
		{
			m_IdealActivity = (Activity)(int)pTask->flData;
			TaskComplete();
			break;
		}
	case TASK_GET_PATH_TO_ENEMY_LKP:
		{
			if ( BuildRoute ( m_vecEnemyLKP, bits_MF_TO_LOCATION, NULL ) )
			{
				TaskComplete();
			}
			else if (BuildNearestRoute( m_vecEnemyLKP, pev->view_ofs, 0, (m_vecEnemyLKP - pev->origin).Length() ))
			{
				TaskComplete();
			}
			else
			{
				// no way to get there =(
				ALERT ( at_aiconsole, "GetPathToEnemyLKP failed!!\n" );
				TaskFail();
			}
			break;
		}
	case TASK_GET_PATH_TO_ENEMY:
		{
			CBaseEntity *pEnemy = m_hEnemy;

			if ( pEnemy == NULL )
			{
				TaskFail();
				return;
			}

			if ( BuildRoute ( pEnemy->pev->origin, bits_MF_TO_ENEMY, pEnemy ) )
			{
				TaskComplete();
			}
			else if (BuildNearestRoute( pEnemy->pev->origin, pEnemy->pev->view_ofs, 0, (pEnemy->pev->origin - pev->origin).Length() ))
			{
				TaskComplete();
			}
			else
			{
				// no way to get there =(
				ALERT ( at_aiconsole, "GetPathToEnemy failed!!\n" );
				TaskFail();
			}
			break;
		}
	case TASK_GET_PATH_TO_ENEMY_CORPSE:
		{
			UTIL_MakeVectors( pev->angles );
			if ( BuildRoute ( m_vecEnemyLKP - gpGlobals->v_forward * 64, bits_MF_TO_LOCATION, NULL ) )
			{
				TaskComplete();
			}
			else
			{
				ALERT ( at_aiconsole, "GetPathToEnemyCorpse failed!!\n" );
				TaskFail();
			}
		}
		break;
	case TASK_GET_PATH_TO_SPOT:
		{
			CBaseEntity *pPlayer = CBaseEntity::Instance( FIND_ENTITY_BY_CLASSNAME( NULL, "player" ) );
			if ( BuildRoute ( m_vecMoveGoal, bits_MF_TO_LOCATION, pPlayer ) )
			{
				TaskComplete();
			}
			else
			{
				// no way to get there =(
				ALERT ( at_aiconsole, "GetPathToSpot failed!!\n" );
				TaskFail();
			}
			break;
		}

	case TASK_GET_PATH_TO_TARGET:
		{
			RouteClear();
			if ( m_hTargetEnt != NULL && MoveToTarget( m_movementActivity, 1 ) )
			{
				TaskComplete();
			}
			else
			{
				// no way to get there =(
				ALERT ( at_aiconsole, "GetPathToSpot failed!!\n" );
				TaskFail();
			}
			break;
		}
	case TASK_GET_PATH_TO_HINTNODE:// for active idles!
		{
			if ( MoveToLocation( m_movementActivity, 2, WorldGraph.m_pNodes[ m_iHintNode ].m_vecOrigin ) )
			{
				TaskComplete();
			}
			else
			{
				// no way to get there =(
				ALERT ( at_aiconsole, "GetPathToHintNode failed!!\n" );
				TaskFail();
			}
			break;
		}
	case TASK_GET_PATH_TO_LASTPOSITION:
		{
			m_vecMoveGoal = m_vecLastPosition;

			if ( MoveToLocation( m_movementActivity, 2, m_vecMoveGoal ) )
			{
				TaskComplete();
			}
			else
			{
				// no way to get there =(
				ALERT ( at_aiconsole, "GetPathToLastPosition failed!!\n" );
				TaskFail();
			}
			break;
		}
	case TASK_GET_PATH_TO_BESTSOUND:
		{
			CSound *pSound;

			pSound = PBestSound();

			if ( pSound && MoveToLocation( m_movementActivity, 2, pSound->m_vecOrigin ) )
			{
				TaskComplete();
			}
			else
			{
				// no way to get there =(
				ALERT ( at_aiconsole, "GetPathToBestSound failed!!\n" );
				TaskFail();
			}
			break;
		}
case TASK_GET_PATH_TO_BESTSCENT:
		{
			CSound *pScent;

			pScent = PBestScent();

			if ( pScent && MoveToLocation( m_movementActivity, 2, pScent->m_vecOrigin ) )
			{
				TaskComplete();
			}
			else
			{
				// no way to get there =(
				ALERT ( at_aiconsole, "GetPathToBestScent failed!!\n" );
				
				TaskFail();
			}
			break;
		}
	case TASK_RUN_PATH:
		{
			// UNDONE: This is in some default AI and some monsters can't run? -- walk instead?
			if ( LookupActivity( ACT_RUN ) != ACTIVITY_NOT_AVAILABLE )
			{
				m_movementActivity = ACT_RUN;
			}
			else
			{
				m_movementActivity = ACT_WALK;
			}
			TaskComplete();
			break;
		}
	case TASK_WALK_PATH:
		{
			if ( pev->movetype == MOVETYPE_FLY )
			{
				m_movementActivity = ACT_FLY;
			}
			if ( LookupActivity( ACT_WALK ) != ACTIVITY_NOT_AVAILABLE )
			{
				m_movementActivity = ACT_WALK;
			}
			else
			{
				m_movementActivity = ACT_RUN;
			}
			TaskComplete();
			break;
		}
	case TASK_STRAFE_PATH:
		{
			Vector2D	vec2DirToPoint; 
			Vector2D	vec2RightSide;

			// to start strafing, we have to first figure out if the target is on the left side or right side
			UTIL_MakeVectors ( pev->angles );

			vec2DirToPoint = ( m_Route[ 0 ].vecLocation - pev->origin ).Make2D().Normalize();
			vec2RightSide = gpGlobals->v_right.Make2D().Normalize();

			if ( DotProduct ( vec2DirToPoint, vec2RightSide ) > 0 )
			{
				// strafe right
				m_movementActivity = ACT_STRAFE_RIGHT;
			}
			else
			{
				// strafe left
				m_movementActivity = ACT_STRAFE_LEFT;
			}
			TaskComplete();
			break;
		}


	case TASK_WAIT_FOR_MOVEMENT:
		{
			if (FRouteClear())
			{
				TaskComplete();
			}
			break;
		}

	case TASK_EAT:
		{
			Eat( pTask->flData );
			TaskComplete();
			break;
		}
	case TASK_SMALL_FLINCH:
		{
			m_IdealActivity = GetSmallFlinchActivity();
			break;
		}
	case TASK_DIE:
		{
			RouteClear();	
			
			m_IdealActivity = GetDeathActivity();

			pev->deadflag = DEAD_DYING;
			break;
		}
	case TASK_SOUND_WAKE:
		{
			AlertSound();
			TaskComplete();
			break;
		}
	case TASK_SOUND_DIE:
		{
			DeathSound();
			TaskComplete();
			break;
		}
	case TASK_SOUND_IDLE:
		{
			IdleSound();
			TaskComplete();
			break;
		}
	case TASK_SOUND_PAIN:
		{
			PainSound();
			TaskComplete();
			break;
		}
	case TASK_SOUND_DEATH:
		{
			DeathSound();
			TaskComplete();
			break;
		}
	case TASK_SOUND_ANGRY:
		{
			// sounds are complete as soon as we get here, cause we've already played them.
			ALERT ( at_aiconsole, "SOUND\n" );			
			TaskComplete();
			break;
		}
	case TASK_WAIT_FOR_SCRIPT:
		{
			if (m_pCine->m_iszIdle)
			{
				m_pCine->StartSequence( (CBaseMonster *)this, m_pCine->m_iszIdle, FALSE );
				if (FStrEq( STRING(m_pCine->m_iszIdle), STRING(m_pCine->m_iszPlay)))
				{
					pev->framerate = 0;
				}
			}
			else
				m_IdealActivity = ACT_IDLE;

			break;
		}
	case TASK_PLAY_SCRIPT:
		{
			pev->movetype = MOVETYPE_FLY;
			ClearBits(pev->flags, FL_ONGROUND);
			m_scriptState = SCRIPT_PLAYING;
			break;
		}
	case TASK_ENABLE_SCRIPT:
		{
			m_pCine->DelayStart( 0 );
			TaskComplete();
			break;
		}
	case TASK_PLANT_ON_SCRIPT:
		{
			if ( m_hTargetEnt != NULL )
			{
				pev->origin = m_hTargetEnt->pev->origin;	// Plant on target
			}

			TaskComplete();
			break;
		}
	case TASK_FACE_SCRIPT:
		{
			if ( m_hTargetEnt != NULL )
			{
				pev->ideal_yaw = UTIL_AngleMod( m_hTargetEnt->pev->angles.y );
			}

			TaskComplete();
			m_IdealActivity = ACT_IDLE;
			RouteClear();
			break;
		}

	case TASK_SUGGEST_STATE:
		{
			m_IdealMonsterState = (MONSTERSTATE)(int)pTask->flData;
			TaskComplete();
			break;
		}

	case TASK_SET_FAIL_SCHEDULE:
		m_failSchedule = (int)pTask->flData;
		TaskComplete();
		break;

	case TASK_CLEAR_FAIL_SCHEDULE:
		m_failSchedule = SCHED_NONE;
		TaskComplete();
		break;

	default:
		{
			ALERT ( at_aiconsole, "No StartTask entry for %d\n", (SHARED_TASKS)pTask->iTask );
			break;
		}
	}
}

//=========================================================
// GetTask - returns a pointer to the current 
// scheduled task. NULL if there's a problem.
//=========================================================
Task_t	*CBaseMonster :: GetTask ( void ) 
{
	if ( m_iScheduleIndex < 0 || m_iScheduleIndex >= m_pSchedule->cTasks )
	{
		// m_iScheduleIndex is not within valid range for the monster's current schedule.
		return NULL;
	}
	else
	{
		return &m_pSchedule->pTasklist[ m_iScheduleIndex ];
	}
}

//=========================================================
// GetSchedule - Decides which type of schedule best suits
// the monster's current state and conditions. Then calls
// monster's member function to get a pointer to a schedule
// of the proper type.
//=========================================================
Schedule_t *CBaseMonster :: GetSchedule ( void )
{
	switch	( m_MonsterState )
	{
	case MONSTERSTATE_PRONE:
		{
			return GetScheduleOfType( SCHED_BARNACLE_VICTIM_GRAB );
			break;
		}
	case MONSTERSTATE_NONE:
		{
			ALERT ( at_aiconsole, "MONSTERSTATE IS NONE!\n" );
			break;
		}
	case MONSTERSTATE_IDLE:
		{
			if ( HasConditions ( bits_COND_HEAR_SOUND ) )
			{
				return GetScheduleOfType( SCHED_ALERT_FACE );
			}
			else if ( FRouteClear() )
			{
				// no valid route!
				return GetScheduleOfType( SCHED_IDLE_STAND );
			}
			else
			{
				// valid route. Get moving
				return GetScheduleOfType( SCHED_IDLE_WALK );
			}
			break;
		}
	case MONSTERSTATE_ALERT:
		{
			if ( HasConditions( bits_COND_ENEMY_DEAD ) && LookupActivity( ACT_VICTORY_DANCE ) != ACTIVITY_NOT_AVAILABLE )
			{
				return GetScheduleOfType ( SCHED_VICTORY_DANCE );
			}

			if ( HasConditions(bits_COND_LIGHT_DAMAGE | bits_COND_HEAVY_DAMAGE) )
			{
				if ( FacingIdeal() )
				{
					return GetScheduleOfType( SCHED_TAKE_COVER_FROM_ORIGIN );
				}
				else
				{
					return GetScheduleOfType( SCHED_ALERT_SMALL_FLINCH );
				}
			}

			else if ( HasConditions ( bits_COND_HEAR_SOUND ) )
			{
				return GetScheduleOfType( SCHED_ALERT_FACE );
			}
			else
			{
				return GetScheduleOfType( SCHED_ALERT_STAND );
			}
			break;
		}
	case MONSTERSTATE_COMBAT:
		{
			if ( HasConditions( bits_COND_ENEMY_DEAD ) )
			{
				// clear the current (dead) enemy and try to find another.
				m_hEnemy = NULL;

				if ( GetEnemy() )
				{
					ClearConditions( bits_COND_ENEMY_DEAD );
					return GetSchedule();
				}
				else
				{
					SetState( MONSTERSTATE_ALERT );
					return GetSchedule();
				}
			}

			if ( HasConditions(bits_COND_NEW_ENEMY) )
			{
				return GetScheduleOfType ( SCHED_WAKE_ANGRY );
			}
			else if (HasConditions(bits_COND_LIGHT_DAMAGE) && !HasMemory( bits_MEMORY_FLINCHED) )
			{
				return GetScheduleOfType( SCHED_SMALL_FLINCH );
			}
			else if ( !HasConditions(bits_COND_SEE_ENEMY) )
			{
				// we can't see the enemy
				if ( !HasConditions(bits_COND_ENEMY_OCCLUDED) )
				{
					// enemy is unseen, but not occluded!
					// turn to face enemy
					return GetScheduleOfType( SCHED_COMBAT_FACE );
				}
				else
				{
					// chase!
					return GetScheduleOfType( SCHED_CHASE_ENEMY );
				}
			}
			else  
			{
				// we can see the enemy
				if ( HasConditions(bits_COND_CAN_RANGE_ATTACK1) )
				{
					return GetScheduleOfType( SCHED_RANGE_ATTACK1 );
				}
				if ( HasConditions(bits_COND_CAN_RANGE_ATTACK2) )
				{
					return GetScheduleOfType( SCHED_RANGE_ATTACK2 );
				}
				if ( HasConditions(bits_COND_CAN_MELEE_ATTACK1) )
				{
					return GetScheduleOfType( SCHED_MELEE_ATTACK1 );
				}
				if ( HasConditions(bits_COND_CAN_MELEE_ATTACK2) )
				{
					return GetScheduleOfType( SCHED_MELEE_ATTACK2 );
				}
				if ( !HasConditions(bits_COND_CAN_RANGE_ATTACK1 | bits_COND_CAN_MELEE_ATTACK1) )
				{
					// if we can see enemy but can't use either attack type, we must need to get closer to enemy
					return GetScheduleOfType( SCHED_CHASE_ENEMY );
				}
				else if ( !FacingIdeal() )
				{
					//turn
					return GetScheduleOfType( SCHED_COMBAT_FACE );
				}
				else
				{
					ALERT ( at_aiconsole, "No suitable combat schedule!\n" );
				}
			}
			break;
		}
	case MONSTERSTATE_DEAD:
		{
			return GetScheduleOfType( SCHED_DIE );
			break;
		}
	case MONSTERSTATE_SCRIPT:
		{
			ASSERT( m_pCine != NULL );
			if ( !m_pCine )
			{
				ALERT( at_aiconsole, "Script failed for %s\n", STRING(pev->classname) );
				CineCleanup();
				return GetScheduleOfType( SCHED_IDLE_STAND );
			}

			return GetScheduleOfType( SCHED_AISCRIPT );
		}
	default:
		{
			ALERT ( at_aiconsole, "Invalid State for GetSchedule!\n" );
			break;
		}
	}

	return &slError[ 0 ];
}
