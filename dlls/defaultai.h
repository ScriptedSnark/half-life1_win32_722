#ifndef DEFAULTAI_H
#define DEFAULTAI_H

//=========================================================
// Failed
//=========================================================
extern Schedule_t	slFail[];

//=========================================================
//	Idle Schedules
//=========================================================
extern Schedule_t	slIdleStand[];
extern Schedule_t	slIdleTrigger[];
extern Schedule_t	slIdleWalk[];

//=========================================================
//	Wake Schedules
//=========================================================
extern Schedule_t slWakeAngry[];

//=========================================================
// AlertTurn Schedules
//=========================================================
extern Schedule_t	slAlertFace[];

//=========================================================
// AlertIdle Schedules
//=========================================================
extern Schedule_t	slAlertStand[];

//=========================================================
// CombatIdle Schedule
//=========================================================
extern Schedule_t	slCombatStand[];

//=========================================================
// CombatFace Schedule
//=========================================================
extern Schedule_t	slCombatFace[];

//=========================================================
// reload schedule
//=========================================================
extern Schedule_t slReload[];

//=========================================================
//	Attack Schedules
//=========================================================

extern Schedule_t	slRangeAttack1[];
extern Schedule_t	slRangeAttack2[];

extern Schedule_t	slTakeCoverFromBestSound[];

// primary melee attack
extern Schedule_t	slMeleeAttack[];

// Chase enemy schedule
extern Schedule_t slChaseEnemy[];

//=========================================================
// small flinch, used when a relatively minor bit of damage
// is inflicted.
//=========================================================
extern Schedule_t slSmallFlinch[];

//=========================================================
// Die!
//=========================================================
extern Schedule_t slDie[];

//=========================================================
//	Universal Error Schedule
//=========================================================
extern Schedule_t slError[];

//=========================================================
//	Scripted sequences
//=========================================================
extern Schedule_t slWalkToScript[];
extern Schedule_t slRunToScript[];
extern Schedule_t slWaitScript[];

#endif		// DEFAULTAI_H
