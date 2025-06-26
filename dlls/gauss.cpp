#if !defined( OEM_BUILD ) && !defined( HLDEMO_BUILD )

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "shake.h"
#include "gamerules.h"


#define	GAUSS_PRIMARY_CHARGE_VOLUME	256// how loud gauss is while charging
#define GAUSS_PRIMARY_FIRE_VOLUME	450// how loud gauss is when discharged

enum gauss_e {
	GAUSS_IDLE = 0,
	GAUSS_IDLE2,
	GAUSS_FIDGET,
	GAUSS_SPINUP,
	GAUSS_SPIN,
	GAUSS_FIRE,
	GAUSS_FIRE2,
	GAUSS_HOLSTER,
	GAUSS_DRAW
};

class CGauss : public CBasePlayerWeapon
{
public:
	int		Save( CSave &save );
	int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	void Spawn( void );
	void Precache( void );
	int iItemSlot( void ) { return 4; }
	int GetItemInfo(ItemInfo *p);
	int AddToPlayer( CBasePlayer *pPlayer );

	BOOL Deploy( void );
	void Holster( void );

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	void WeaponIdle( void );

	int m_fInAttack;
	float m_flStartCharge;
	float m_flPlayAftershock;
	void StartFire( void );
	void Fire( Vector vecOrigSrc, Vector vecDirShooting, float flDamage );
	float GetFullChargeTime( void );
	int m_iBalls;
	int m_iGlow;
	int m_iBeam;
	int m_iSoundState; // don't save this

	float m_flNextAmmoBurn;// while charging, when to absorb another unit of player's ammo?

	// was this weapon just fired primary or secondary?
	// we need to know so we can pick the right set of effects. 
	BOOL m_fPrimaryFire;
};
LINK_ENTITY_TO_CLASS( weapon_gauss, CGauss );


TYPEDESCRIPTION	CGauss::m_SaveData[] = 
{
	DEFINE_FIELD( CGauss, m_fInAttack, FIELD_INTEGER ),
	DEFINE_FIELD( CGauss, m_flStartCharge, FIELD_TIME ),
	DEFINE_FIELD( CGauss, m_flPlayAftershock, FIELD_TIME ),
	DEFINE_FIELD( CGauss, m_flNextAmmoBurn, FIELD_TIME ),
	DEFINE_FIELD( CGauss, m_fPrimaryFire, FIELD_BOOLEAN ),
};
IMPLEMENT_SAVERESTORE( CGauss, CBasePlayerWeapon );


float CGauss::GetFullChargeTime( void )
{
	if ( g_pGameRules->IsMultiplayer() )
	{
		return 1.5;
	}

	return 4;
}

void CGauss::Spawn( )
{
	Precache( );
	m_iId = WEAPON_GAUSS;
	SET_MODEL(ENT(pev), "models/w_gauss.mdl");

	m_iDefaultAmmo = GAUSS_DEFAULT_GIVE;

	FallInit();// get ready to fall down.
}


void CGauss::Precache( void )
{
	PRECACHE_MODEL("models/w_gauss.mdl");
	PRECACHE_MODEL("models/v_gauss.mdl");
	PRECACHE_MODEL("models/p_gauss.mdl");

	PRECACHE_SOUND("items/9mmclip1.wav");

	PRECACHE_SOUND("weapons/gauss2.wav");
	PRECACHE_SOUND("weapons/electro4.wav");
	PRECACHE_SOUND("weapons/electro5.wav");
	PRECACHE_SOUND("weapons/electro6.wav");
	PRECACHE_SOUND("ambience/pulsemachine.wav");
	
	m_iGlow = PRECACHE_MODEL( "sprites/hotglow.spr" );
	m_iBalls = PRECACHE_MODEL( "sprites/hotglow.spr" );
	m_iBeam = PRECACHE_MODEL( "sprites/smoke.spr" );
}

int CGauss::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
		MESSAGE_END();
		return TRUE;
	}
	return FALSE;
}

int CGauss::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "uranium";
	p->iAmmo1 = URANIUM_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 3;
	p->iPosition = 1;
	p->iId = m_iId = WEAPON_GAUSS;
	p->iFlags = 0;
	p->iWeight = GAUSS_WEIGHT;

	return 1;
}


BOOL CGauss::Deploy( )
{
	return DefaultDeploy( "models/v_gauss.mdl", "models/p_gauss.mdl", GAUSS_DRAW );
}


void CGauss::Holster( )
{
	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.5;
	// m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 10, 15 );
	SendWeaponAnim( GAUSS_HOLSTER );
	m_fInAttack = 0;
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "common/null.wav", 1.0, ATTN_NORM);
}


void CGauss::PrimaryAttack()
{
	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound( );
		m_flNextSecondaryAttack = m_flNextPrimaryAttack = gpGlobals->time + 0.15;
		return;
	}

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 2)
	{
		PlayEmptySound( );
		m_pPlayer->m_flNextAttack = gpGlobals->time + 0.5;
		return;
	}

	m_pPlayer->m_iWeaponVolume = GAUSS_PRIMARY_FIRE_VOLUME;

	m_fPrimaryFire = TRUE;

	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 2;

	StartFire();
	m_fInAttack = 0;
	m_flTimeWeaponIdle = gpGlobals->time + 1.0;
	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.2;
}

void CGauss::SecondaryAttack()
{
	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	{
		if ( m_fInAttack != 0 )
		{
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/electro4.wav", 1.0, ATTN_NORM, 0, 80 + RANDOM_LONG(0,0x3f));
			SendWeaponAnim( GAUSS_IDLE );
			m_fInAttack = 0;
		}
		else
		{
			PlayEmptySound( );
		}

		m_flNextSecondaryAttack = m_flNextPrimaryAttack = gpGlobals->time + 0.5;
		return;
	}

	if (m_fInAttack == 0)
	{
		if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		{
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/357_cock1.wav", 0.8, ATTN_NORM);
			m_pPlayer->m_flNextAttack = gpGlobals->time + 0.5;
			return;
		}

		m_fPrimaryFire = FALSE;

		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;// take one ammo just to start the spin
		m_flNextAmmoBurn = gpGlobals->time;

		// spin up
		m_pPlayer->m_iWeaponVolume = GAUSS_PRIMARY_CHARGE_VOLUME;
		
		SendWeaponAnim( GAUSS_SPINUP );
		m_fInAttack = 1;
		m_flTimeWeaponIdle = gpGlobals->time + 0.5;
		m_flStartCharge = gpGlobals->time;
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "ambience/pulsemachine.wav",1.0 , ATTN_NORM, 0, 110 );
		m_iSoundState = SND_CHANGE_PITCH;
	}
	else if (m_fInAttack == 1)
	{
		if (m_flTimeWeaponIdle < gpGlobals->time)
		{
			SendWeaponAnim( GAUSS_SPIN );
			m_fInAttack = 2;
		}
	}
	else
	{
		if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] == 0 )
		{
			// out of ammo! force the gun to fire
			StartFire();
			m_fInAttack = 0;
			m_flTimeWeaponIdle = gpGlobals->time + 1.0;
			m_pPlayer->m_flNextAttack = gpGlobals->time + 1;
			return;
		}

		// during the charging process, eat one bit of ammo every once in a while
		if ( gpGlobals->time > m_flNextAmmoBurn && m_flNextAmmoBurn != -1 )
		{
			if ( g_pGameRules->IsMultiplayer() )
			{
				m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
				m_flNextAmmoBurn = gpGlobals->time + 0.1;
			}
			else
			{
				m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
				m_flNextAmmoBurn = gpGlobals->time + 0.3;
			}
		}
		
		if ( gpGlobals->time - m_flStartCharge >= GetFullChargeTime() )
		{
			// don't eat any more ammo after gun is fully charged.
			m_flNextAmmoBurn = -1;
		}

		int pitch = (gpGlobals->time - m_flStartCharge) * (150/GetFullChargeTime()) + 100;
		if (pitch > 250) 
			pitch = 250;
		
		// ALERT( at_console, "%d %d %d\n", m_fInAttack, m_iSoundState, pitch );

		if (m_iSoundState == 0)
			ALERT( at_console, "sound state %d\n", m_iSoundState );

		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "ambience/pulsemachine.wav", 1.0, ATTN_NORM, m_iSoundState, pitch);
		
		m_iSoundState = SND_CHANGE_PITCH; // hack for going through level transitions

		m_pPlayer->m_iWeaponVolume = GAUSS_PRIMARY_CHARGE_VOLUME;
		
		// m_flTimeWeaponIdle = gpGlobals->time + 0.1;
		if (m_flStartCharge < gpGlobals->time - 10)
		{
			// Player charged up too long. Zap him.
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/electro4.wav", 1.0, ATTN_NORM, 0, 80 + RANDOM_LONG(0,0x3f));
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM,   "weapons/electro6.wav", 1.0, ATTN_NORM, 0, 75 + RANDOM_LONG(0,0x3f));
			
			m_fInAttack = 0;
			m_flTimeWeaponIdle = gpGlobals->time + 1.0;
			m_pPlayer->m_flNextAttack = gpGlobals->time + 1.0;
			m_pPlayer->TakeDamage( VARS(eoNullEntity), VARS(eoNullEntity), 50, DMG_SHOCK );
	
			UTIL_ScreenFade(Vector(255,128,0), 2, 0.5, 128, FFADE_IN );
			SendWeaponAnim( GAUSS_IDLE );
			
			// Player may have been killed and this weapon dropped, don't execute any more code after this!
			return;
		}
	}
}

//=========================================================
// StartFire- since all of this code has to run and then 
// call Fire(), it was easier at this point to rip it out 
// of weaponidle() and make its own function then to try to
// merge this into Fire(), which has some identical variable names 
//=========================================================
void CGauss::StartFire( void )
{
	float flDamage;
	
	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );
	Vector vecAiming = gpGlobals->v_forward;
	Vector vecSrc = m_pPlayer->GetGunPosition( ); // + gpGlobals->v_up * -8 + gpGlobals->v_right * 8;
	
	if (gpGlobals->time - m_flStartCharge > GetFullChargeTime())
	{
		flDamage = 200;
	}
	else
	{
		flDamage = 200 * ((gpGlobals->time - m_flStartCharge) / GetFullChargeTime() );
	}

	if ( m_fPrimaryFire )
	{
		// fixed damage on primary attack
		flDamage = gSkillData.plrDmgGauss;
	}

	if (m_fInAttack != 3)
	{
		//ALERT ( at_console, "Time:%f Damage:%f\n", gpGlobals->time - m_flStartCharge, flDamage );

		float flZVel = m_pPlayer->pev->velocity.z;

		if ( m_fPrimaryFire )
		{
			m_pPlayer->pev->velocity = m_pPlayer->pev->velocity - gpGlobals->v_forward * 300;
		}
		else
		{
			m_pPlayer->pev->velocity = m_pPlayer->pev->velocity - gpGlobals->v_forward * flDamage * 5;
		}

		if ( !g_pGameRules->IsDeathmatch() )
		{
			// in deathmatch, gauss can pop you up into the air. Not in single play.
			m_pPlayer->pev->velocity.z = flZVel;
		}

		SendWeaponAnim( GAUSS_FIRE2 );
	}

	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/gauss2.wav", 0.5 + flDamage * (1.0 / 400.0), ATTN_NORM, 0, 85 + RANDOM_LONG(0,0x1f)); 
	// time until aftershock 'static discharge' sound
	m_flPlayAftershock = gpGlobals->time + RANDOM_FLOAT(0.3, 0.8);

	Fire( vecSrc, vecAiming, flDamage );
}

void CGauss::Fire( Vector vecOrigSrc, Vector vecDir, float flDamage )
{
	m_pPlayer->m_iWeaponVolume = GAUSS_PRIMARY_FIRE_VOLUME;

	Vector vecSrc = vecOrigSrc;
	Vector vecDest = vecSrc + vecDir * 8192;
	edict_t		*pentIgnore;
	TraceResult tr, beam_tr;
	float flMaxFrac = 1.0;
	int	nTotal = 0;
	int fHasPunched = 0;
	int fFirstBeam = 1;
	int	nMaxHits = 10;

	pentIgnore = ENT( m_pPlayer->pev );

	/*
	ALERT( at_console, "%f %f %f\n%f %f %f\n", 
		vecSrc.x, vecSrc.y, vecSrc.z, 
		vecDest.x, vecDest.y, vecDest.z );
	*/

	// ALERT( at_console, "%f %f\n", tr.flFraction, flMaxFrac );

	while (flDamage > 10 && nMaxHits > 0)
	{
		nMaxHits--;

		// ALERT( at_console, "." );
		UTIL_TraceLine(vecSrc, vecDest, dont_ignore_monsters, pentIgnore, &tr);

		if (tr.fAllSolid)
			break;

		CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);

		if (pEntity == NULL)
			break;

		if (fFirstBeam)
		{
			m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
			fFirstBeam = 0;

			Vector tmpSrc = vecSrc + gpGlobals->v_up * -8 + gpGlobals->v_right * 3;

			// don't draw beam until the damn thing looks like it's coming out of the barrel
			// draw beam
			MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, tr.vecEndPos );
				WRITE_BYTE( TE_BEAMENTPOINT );
				WRITE_SHORT( m_pPlayer->entindex() + 0x1000 );
				WRITE_COORD( tr.vecEndPos.x);
				WRITE_COORD( tr.vecEndPos.y);
				WRITE_COORD( tr.vecEndPos.z);
				WRITE_SHORT( m_iBeam );
				WRITE_BYTE( 0 ); // startframe
				WRITE_BYTE( 0 ); // framerate
				WRITE_BYTE( 1 ); // life

				if ( m_fPrimaryFire )
				{
					WRITE_BYTE( 10 );  // width
				}
				else
				{
					WRITE_BYTE( 25 );  // width
				}

				WRITE_BYTE( 0 );   // noise

				if ( m_fPrimaryFire )
				{
					WRITE_BYTE( 255 );   // r, g, b
					WRITE_BYTE( 128 );   // r, g, b
					WRITE_BYTE( 0 );   // r, g, b

					WRITE_BYTE( 128 );	// brightness
				}
				else
				{
					// secondary shot is always white, and intensity based on charge
					WRITE_BYTE( 255 );   // r, g, b
					WRITE_BYTE( 255 );   // r, g, b
					WRITE_BYTE( 255 );   // r, g, b
				
					WRITE_BYTE( flDamage );	// brightness
				}

				WRITE_BYTE( 0 );		// speed
			MESSAGE_END();

			nTotal += 26;
		}
		else
		{
			// draw beam
			MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, vecSrc );
				WRITE_BYTE( TE_BEAMPOINTS);
				WRITE_COORD( vecSrc.x);
				WRITE_COORD( vecSrc.y);
				WRITE_COORD( vecSrc.z);
				WRITE_COORD( tr.vecEndPos.x);
				WRITE_COORD( tr.vecEndPos.y);
				WRITE_COORD( tr.vecEndPos.z);
				WRITE_SHORT( m_iBeam );
				WRITE_BYTE( 0 ); // startframe
				WRITE_BYTE( 0 ); // framerate
				WRITE_BYTE( 1 ); // life

				if ( m_fPrimaryFire )
				{
					WRITE_BYTE( 10 );  // width
				}
				else
				{
					WRITE_BYTE( 25 );  // width
				}

				WRITE_BYTE( 0 );   // noise

				if ( m_fPrimaryFire )
				{
					// primary shot always looks full intensity
					WRITE_BYTE( 255 );   // r, g, b
					WRITE_BYTE( 128 );   // r, g, b
					WRITE_BYTE( 0 );   // r, g, b

					WRITE_BYTE( 128 );	// brightness
				}
				else
				{
					// secondary shot is always white, and intensity based on charge
					WRITE_BYTE( 255 );   // r, g, b
					WRITE_BYTE( 255 );   // r, g, b
					WRITE_BYTE( 255 );   // r, g, b
				
					WRITE_BYTE( flDamage );	// brightness
				}

				WRITE_BYTE( 0 );		// speed
			MESSAGE_END();
			nTotal += 26;
		}

		if (pEntity->pev->takedamage)
		{
			ClearMultiDamage();
			pEntity->TraceAttack( m_pPlayer->pev, flDamage, vecDir, &tr, DMG_BULLET );
			ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);
		}

		// ALERT( at_console, "%s\n", STRING( pEntity->pev->classname ));

		if ( pEntity->ReflectGauss() )
		{
			float n;

			pentIgnore = NULL;

			n = -DotProduct(tr.vecPlaneNormal, vecDir);

			if (n < 0.5) // 60 degrees
			{
				// ALERT( at_console, "reflect %f\n", n );
				// reflect
				Vector r;
			
				r = 2.0 * tr.vecPlaneNormal * n + vecDir;
				flMaxFrac = flMaxFrac - tr.flFraction;
				vecDir = r;
				vecSrc = tr.vecEndPos + vecDir * 8;
				vecDest = vecSrc + vecDir * 8192;

				// explode a bit
				m_pPlayer->RadiusDamage( tr.vecEndPos, pev, m_pPlayer->pev, flDamage * n, CLASS_NONE, DMG_BLAST );

				// bounce wall glow
				MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, tr.vecEndPos );
					WRITE_BYTE( TE_GLOWSPRITE );
					WRITE_COORD( tr.vecEndPos.x);		// pos
					WRITE_COORD( tr.vecEndPos.y);
					WRITE_COORD( tr.vecEndPos.z);
					WRITE_SHORT( m_iGlow );				// model
					WRITE_BYTE( flDamage * n * 0.5 );	// life * 10
					WRITE_BYTE( 2 );					// size * 10
					WRITE_BYTE( flDamage * n );			// brightness
				MESSAGE_END();

				nTotal += 13;

				// balls
				MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, tr.vecEndPos );
					WRITE_BYTE( TE_SPRITETRAIL );// TE_RAILTRAIL);
					WRITE_COORD( tr.vecEndPos.x );
					WRITE_COORD( tr.vecEndPos.y );
					WRITE_COORD( tr.vecEndPos.z );
					WRITE_COORD( tr.vecEndPos.x + tr.vecPlaneNormal.x );
					WRITE_COORD( tr.vecEndPos.y + tr.vecPlaneNormal.y );
					WRITE_COORD( tr.vecEndPos.z + tr.vecPlaneNormal.z );
					WRITE_SHORT( m_iBalls );		// model
					WRITE_BYTE( n * flDamage * 0.3 );				// count
					WRITE_BYTE( 10 );				// life * 10
					WRITE_BYTE( RANDOM_LONG( 1, 2 ) );				// size * 10
					WRITE_BYTE( 10 );				// amplitude * 0.1
					WRITE_BYTE( 20 );				// speed * 100
				MESSAGE_END();

				nTotal += 21;

				// lose energy
				if (n == 0) n = 0.1;
				flDamage = flDamage * (1 - n);
			}
			else
			{
				// tunnel
				DecalGunshot( &tr, BULLET_MONSTER_12MM );

				// entry wall glow
				MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, tr.vecEndPos );
					WRITE_BYTE( TE_GLOWSPRITE );
					WRITE_COORD( tr.vecEndPos.x);	// pos
					WRITE_COORD( tr.vecEndPos.y);
					WRITE_COORD( tr.vecEndPos.z);
					WRITE_SHORT( m_iGlow );		// model
					WRITE_BYTE( 60 );				// life * 10
					WRITE_BYTE( 10 );				// size * 10
					WRITE_BYTE( flDamage );			// brightness
				MESSAGE_END();
				nTotal += 13;

				// limit it to one hole punch
				if (fHasPunched)
					break;
				fHasPunched = 1;

				// try punching through wall if secondary attack (primary is incapable of breaking through)
				if ( !m_fPrimaryFire )
				{
					UTIL_TraceLine( tr.vecEndPos + vecDir * 8, vecDest, dont_ignore_monsters, pentIgnore, &beam_tr);
					if (!beam_tr.fAllSolid)
					{
						// trace backwards to find exit point
						UTIL_TraceLine( beam_tr.vecEndPos, tr.vecEndPos, dont_ignore_monsters, pentIgnore, &beam_tr);

						float n = (beam_tr.vecEndPos - tr.vecEndPos).Length( );

						if (n < flDamage)
						{
							if (n == 0) n = 1;
							flDamage -= n;

							// ALERT( at_console, "punch %f\n", n );

							// absorption balls
							MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, tr.vecEndPos );
								WRITE_BYTE( TE_SPRITETRAIL );// TE_RAILTRAIL);
								WRITE_COORD( tr.vecEndPos.x );
								WRITE_COORD( tr.vecEndPos.y );
								WRITE_COORD( tr.vecEndPos.z );
								WRITE_COORD( tr.vecEndPos.x - vecDir.x );
								WRITE_COORD( tr.vecEndPos.y - vecDir.y );
								WRITE_COORD( tr.vecEndPos.z - vecDir.z );
								WRITE_SHORT( m_iBalls );		// model
								WRITE_BYTE( 3 );				// count
								WRITE_BYTE( 10 );				// life * 10
								WRITE_BYTE( RANDOM_LONG( 1, 2 ) );				// size * 10
								WRITE_BYTE( 10 );				// amplitude * 0.1
								WRITE_BYTE( 1 );				// speed * 100
							MESSAGE_END();
							nTotal += 21;

							// exit blast damage
							m_pPlayer->RadiusDamage( beam_tr.vecEndPos + vecDir * 8, pev, m_pPlayer->pev, flDamage, CLASS_NONE, DMG_BLAST );
							CSoundEnt::InsertSound ( bits_SOUND_COMBAT, pev->origin, NORMAL_EXPLOSION_VOLUME, 3.0 );

							DecalGunshot( &beam_tr, BULLET_MONSTER_12MM );
							nTotal += 19;

							// exit wall glow
							MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, beam_tr.vecEndPos );
								WRITE_BYTE( TE_GLOWSPRITE );
								WRITE_COORD( beam_tr.vecEndPos.x);	// pos
								WRITE_COORD( beam_tr.vecEndPos.y);
								WRITE_COORD( beam_tr.vecEndPos.z);
								WRITE_SHORT( m_iGlow );		// model
								WRITE_BYTE( 60 );				// life * 10
								WRITE_BYTE( 10 );				// size * 10
								WRITE_BYTE( flDamage );			// brightness
							MESSAGE_END();
							nTotal += 13;

							// balls
							MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, beam_tr.vecEndPos );
								WRITE_BYTE( TE_SPRITETRAIL );// TE_RAILTRAIL);
								WRITE_COORD( beam_tr.vecEndPos.x );
								WRITE_COORD( beam_tr.vecEndPos.y );
								WRITE_COORD( beam_tr.vecEndPos.z );
								WRITE_COORD( beam_tr.vecEndPos.x + vecDir.x );
								WRITE_COORD( beam_tr.vecEndPos.y + vecDir.y );
								WRITE_COORD( beam_tr.vecEndPos.z + vecDir.z );
								WRITE_SHORT( m_iBalls );		// model
								WRITE_BYTE( flDamage * 0.3 );				// count
								WRITE_BYTE( 10 );				// life * 10
								WRITE_BYTE( RANDOM_LONG( 1, 2 ) );				// size * 10
								WRITE_BYTE( 20 );				// amplitude * 0.1
								WRITE_BYTE( 40 );				// speed * 100
							MESSAGE_END();
							nTotal += 21;

							vecSrc = beam_tr.vecEndPos + vecDir;
						}
					}
					else
					{
						 //ALERT( at_console, "blocked %f\n", n );
						flDamage = 0;
					}
				}
				else
				{
					//ALERT( at_console, "blocked solid\n" );
					
					if ( m_fPrimaryFire )
					{
						// slug doesn't punch through ever with primary 
						// fire, so leave a little glowy bit and make some balls
						MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, tr.vecEndPos );
							WRITE_BYTE( TE_GLOWSPRITE );
							WRITE_COORD( tr.vecEndPos.x);	// pos
							WRITE_COORD( tr.vecEndPos.y);
							WRITE_COORD( tr.vecEndPos.z);
							WRITE_SHORT( m_iGlow );		// model
							WRITE_BYTE( 20 );				// life * 10
							WRITE_BYTE( 3 );				// size * 10
							WRITE_BYTE( 200 );			// brightness
						MESSAGE_END();


						MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, tr.vecEndPos );
							WRITE_BYTE( TE_SPRITETRAIL );// TE_RAILTRAIL);
							WRITE_COORD( tr.vecEndPos.x );
							WRITE_COORD( tr.vecEndPos.y );
							WRITE_COORD( tr.vecEndPos.z );
							WRITE_COORD( tr.vecEndPos.x + tr.vecPlaneNormal.x );
							WRITE_COORD( tr.vecEndPos.y + tr.vecPlaneNormal.y );
							WRITE_COORD( tr.vecEndPos.z + tr.vecPlaneNormal.z );
							WRITE_SHORT( m_iBalls );		// model
							WRITE_BYTE( 8 );				// count
							WRITE_BYTE( 6 );				// life * 10
							WRITE_BYTE( RANDOM_LONG( 1, 2 ) );	// size * 10
							WRITE_BYTE( 10 );				// amplitude * 0.1
							WRITE_BYTE( 20 );				// speed * 100
						MESSAGE_END();
					}

					flDamage = 0;
				}

			}
		}
		else
		{
			vecSrc = tr.vecEndPos + vecDir;
			pentIgnore = ENT( pEntity->pev );
		}
	}
	// ALERT( at_console, "%d bytes\n", nTotal );
}




void CGauss::WeaponIdle( void )
{
	ResetEmptySound( );

	// play aftershock static discharge
	if (m_flPlayAftershock && m_flPlayAftershock < gpGlobals->time)
	{
		switch (RANDOM_LONG(0,3))
		{
		case 0:	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/electro4.wav", RANDOM_FLOAT(0.7, 0.8), ATTN_NORM); break;
		case 1:	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/electro5.wav", RANDOM_FLOAT(0.7, 0.8), ATTN_NORM); break;
		case 2:	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/electro6.wav", RANDOM_FLOAT(0.7, 0.8), ATTN_NORM); break;
		case 3:	break; // no sound
		}
		m_flPlayAftershock = 0.0;
	}

	if (m_flTimeWeaponIdle > gpGlobals->time)
		return;

	if (m_fInAttack != 0)
	{
		StartFire();
		m_fInAttack = 0;
		m_flTimeWeaponIdle = gpGlobals->time + 2.0;
	}
	else
	{
		float flRand = RANDOM_FLOAT(0, 1);
		if (flRand <= 0.5)
		{
			m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 10, 15 );
		}
		else if (flRand <= 0.75)
		{
			m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 10, 15 );
		}
		else
		{
			m_flTimeWeaponIdle = gpGlobals->time + 3;
		}		
	}
}






class CGaussAmmo : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_gaussammo.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_gaussammo.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo( CBaseEntity *pOther ) 
	{ 
		if (pOther->GiveAmmo( AMMO_URANIUMBOX_GIVE, "uranium", URANIUM_MAX_CARRY, NULL ) != -1)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
			return TRUE;
		}
		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS( ammo_gaussclip, CGaussAmmo );











#endif