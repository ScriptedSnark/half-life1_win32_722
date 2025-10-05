#ifndef EXPLODE_H
#define EXPLODE_H


#define	SF_ENVEXPLOSION_NODAMAGE	( 1 << 0 ) // when set, ENV_EXPLOSION will not actually inflict damage
#define	SF_ENVEXPLOSION_REPEATABLE	( 1 << 1 ) // can this entity be refired?
#define SF_ENVEXPLOSION_NOFIREBALL	( 1 << 2 ) // don't draw the fireball
#define SF_ENVEXPLOSION_NOSMOKE		( 1 << 3 ) // don't draw the smoke
#define SF_ENVEXPLOSION_NODECAL		( 1 << 4 ) // don't make a scorch mark
#define SF_ENVEXPLOSION_NOSPARKS	( 1 << 5 ) // don't make a scorch mark

extern DLL_GLOBAL	short	g_sModelIndexFireball;
extern DLL_GLOBAL	short	g_sModelIndexSmoke;


extern void ExplosionCreate( const Vector &center, const Vector &angles, edict_t *pOwner, int magnitude, BOOL doDamage );

#endif			//EXPLODE_H
