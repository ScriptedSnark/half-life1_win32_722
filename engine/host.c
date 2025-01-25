// host.c -- coordinates spawning and killing of local servers

#include "quakedef.h"
#include "winquake.h"
#include "cmodel.h"
#include "profile.h"

/*

A server can allways be started, even if the system started out as a client
to a remote system.

A client can NOT be started if the system started as a dedicated server.

Memory is cleared / released when a server or client begins, not when they end.

*/

quakeparms_t host_parms;

qboolean	host_initialized;		// true if into command execution

typedef struct adrlist_s
{
	qboolean heartbeatwaiting;
	float heartbeatwaitingtime;
	int heartbeatchallenge;
	int heartbeat_sequence;
	double last_heartbeat;
	netadr_t adr;
	struct adrlist_s* next;
} adrlist_t;

adrlist_t* valvemaster_adr;
qboolean gfNoMasterServer;

float		last_master_heartbeat;

double		host_frametime;
double		host_time;
double		realtime;			// without any filtering or bounding
double		oldrealtime;		// last frame run

int			host_framecount;
int			host_hunklevel;

int			minimum_memory;




client_t* host_client;			// current client

jmp_buf 	host_abortserver;
jmp_buf		host_enddemo;

unsigned short* host_basepal;
unsigned char* host_colormap;

void SV_ClearClientStates(void);
void Master_Shutdown(void);

void Host_NextDemo(void);

cvar_t	maxfps = { "maxfps", "72.0" };

float g_fFrameTime = 0.0f;

cvar_t	host_framerate = { "host_framerate", "0" };
cvar_t	host_speeds = { "host_speeds", "0" };			// set for running times
cvar_t	host_killtime = { "host_killtime", "0" };
cvar_t	sys_ticrate = { "sys_ticrate", "0.5" };

cvar_t	serverprofile = { "serverprofile", "0" };

// multiplayer server rules
cvar_t	teamplay = { "mp_teamplay","0", FALSE, TRUE };
cvar_t	fraglimit = { "mp_fraglimit","0", FALSE, TRUE };
cvar_t	timelimit = { "mp_timelimit","0", FALSE, TRUE };
cvar_t	friendlyfire = { "mp_friendlyfire","0", FALSE, TRUE };
cvar_t	falldamage = { "mp_falldamage","0", FALSE, TRUE };
cvar_t	weaponstay = { "mp_weaponstay","0", FALSE, TRUE };
cvar_t	forcerespawn = { "mp_forcerespawn","0", FALSE, TRUE };

cvar_t	displaysoundlist = { "displaysoundlist","0" };

cvar_t	deathmatch = { "deathmatch", "0", FALSE, TRUE };	// 0, 1, or 2
cvar_t	mapcyclefile = { "mapcyclefile", "mapcycle.txt"};
cvar_t	coop = { "coop", "0", FALSE, TRUE };
cvar_t	pausable = { "pausable", "1", FALSE, TRUE };
cvar_t	skill = { "skill", "1" };			// 1 - 3


//CVARS FOR SKILL LEVEL SETTINGS
// Agrunt
cvar_t	sk_agrunt_health1 = { "sk_agrunt_health1","0" };
cvar_t	sk_agrunt_health2 = { "sk_agrunt_health2","0" };
cvar_t	sk_agrunt_health3 = { "sk_agrunt_health3","0" };

cvar_t	sk_agrunt_dmg_punch1 = { "sk_agrunt_dmg_punch1","0" };
cvar_t	sk_agrunt_dmg_punch2 = { "sk_agrunt_dmg_punch2","0" };
cvar_t	sk_agrunt_dmg_punch3 = { "sk_agrunt_dmg_punch3","0" };

// Apache
cvar_t	sk_apache_health1 = { "sk_apache_health1","0" };
cvar_t	sk_apache_health2 = { "sk_apache_health2","0" };
cvar_t	sk_apache_health3 = { "sk_apache_health3","0" };

// Barney
cvar_t	sk_barney_health1 = { "sk_barney_health1","0" };
cvar_t	sk_barney_health2 = { "sk_barney_health2","0" };
cvar_t	sk_barney_health3 = { "sk_barney_health3","0" };

// Bullsquid
cvar_t	sk_bullsquid_health1 = { "sk_bullsquid_health1","0" };
cvar_t	sk_bullsquid_health2 = { "sk_bullsquid_health2","0" };
cvar_t	sk_bullsquid_health3 = { "sk_bullsquid_health3","0" };

cvar_t	sk_bullsquid_dmg_bite1 = { "sk_bullsquid_dmg_bite1","0" };
cvar_t	sk_bullsquid_dmg_bite2 = { "sk_bullsquid_dmg_bite2","0" };
cvar_t	sk_bullsquid_dmg_bite3 = { "sk_bullsquid_dmg_bite3","0" };

cvar_t	sk_bullsquid_dmg_whip1 = { "sk_bullsquid_dmg_whip1","0" };
cvar_t	sk_bullsquid_dmg_whip2 = { "sk_bullsquid_dmg_whip2","0" };
cvar_t	sk_bullsquid_dmg_whip3 = { "sk_bullsquid_dmg_whip3","0" };

cvar_t	sk_bullsquid_dmg_spit1 = { "sk_bullsquid_dmg_spit1","0" };
cvar_t	sk_bullsquid_dmg_spit2 = { "sk_bullsquid_dmg_spit2","0" };
cvar_t	sk_bullsquid_dmg_spit3 = { "sk_bullsquid_dmg_spit3","0" };


// Big Momma
cvar_t	sk_bigmomma_health_factor1 = { "sk_bigmomma_health_factor1","1.0" };
cvar_t	sk_bigmomma_health_factor2 = { "sk_bigmomma_health_factor2","1.0" };
cvar_t	sk_bigmomma_health_factor3 = { "sk_bigmomma_health_factor3","1.0" };

cvar_t	sk_bigmomma_dmg_slash1 = { "sk_bigmomma_dmg_slash1","50" };
cvar_t	sk_bigmomma_dmg_slash2 = { "sk_bigmomma_dmg_slash2","50" };
cvar_t	sk_bigmomma_dmg_slash3 = { "sk_bigmomma_dmg_slash3","50" };

cvar_t	sk_bigmomma_dmg_blast1 = { "sk_bigmomma_dmg_blast1","100" };
cvar_t	sk_bigmomma_dmg_blast2 = { "sk_bigmomma_dmg_blast2","100" };
cvar_t	sk_bigmomma_dmg_blast3 = { "sk_bigmomma_dmg_blast3","100" };

cvar_t	sk_bigmomma_radius_blast1 = { "sk_bigmomma_radius_blast1","250" };
cvar_t	sk_bigmomma_radius_blast2 = { "sk_bigmomma_radius_blast2","250" };
cvar_t	sk_bigmomma_radius_blast3 = { "sk_bigmomma_radius_blast3","250" };

// Gargantua
cvar_t	sk_gargantua_health1 = { "sk_gargantua_health1","0" };
cvar_t	sk_gargantua_health2 = { "sk_gargantua_health2","0" };
cvar_t	sk_gargantua_health3 = { "sk_gargantua_health3","0" };

cvar_t	sk_gargantua_dmg_slash1 = { "sk_gargantua_dmg_slash1","0" };
cvar_t	sk_gargantua_dmg_slash2 = { "sk_gargantua_dmg_slash2","0" };
cvar_t	sk_gargantua_dmg_slash3 = { "sk_gargantua_dmg_slash3","0" };

cvar_t	sk_gargantua_dmg_fire1 = { "sk_gargantua_dmg_fire1","0" };
cvar_t	sk_gargantua_dmg_fire2 = { "sk_gargantua_dmg_fire2","0" };
cvar_t	sk_gargantua_dmg_fire3 = { "sk_gargantua_dmg_fire3","0" };

cvar_t	sk_gargantua_dmg_stomp1 = { "sk_gargantua_dmg_stomp1","0" };
cvar_t	sk_gargantua_dmg_stomp2 = { "sk_gargantua_dmg_stomp2","0" };
cvar_t	sk_gargantua_dmg_stomp3 = { "sk_gargantua_dmg_stomp3","0" };


// Hassassin
cvar_t	sk_hassassin_health1 = { "sk_hassassin_health1","0" };
cvar_t	sk_hassassin_health2 = { "sk_hassassin_health2","0" };
cvar_t	sk_hassassin_health3 = { "sk_hassassin_health3","0" };


// Headcrab
cvar_t	sk_headcrab_health1 = { "sk_headcrab_health1","0" };
cvar_t	sk_headcrab_health2 = { "sk_headcrab_health2","0" };
cvar_t	sk_headcrab_health3 = { "sk_headcrab_health3","0" };

cvar_t	sk_headcrab_dmg_bite1 = { "sk_headcrab_dmg_bite1","0" };
cvar_t	sk_headcrab_dmg_bite2 = { "sk_headcrab_dmg_bite2","0" };
cvar_t	sk_headcrab_dmg_bite3 = { "sk_headcrab_dmg_bite3","0" };


// Hgrunt 
cvar_t	sk_hgrunt_health1 = { "sk_hgrunt_health1","0" };
cvar_t	sk_hgrunt_health2 = { "sk_hgrunt_health2","0" };
cvar_t	sk_hgrunt_health3 = { "sk_hgrunt_health3","0" };

cvar_t	sk_hgrunt_kick1 = { "sk_hgrunt_kick1","0" };
cvar_t	sk_hgrunt_kick2 = { "sk_hgrunt_kick2","0" };
cvar_t	sk_hgrunt_kick3 = { "sk_hgrunt_kick3","0" };

cvar_t	sk_hgrunt_pellets1 = { "sk_hgrunt_pellets1","0" };
cvar_t	sk_hgrunt_pellets2 = { "sk_hgrunt_pellets2","0" };
cvar_t	sk_hgrunt_pellets3 = { "sk_hgrunt_pellets3","0" };

cvar_t	sk_hgrunt_gspeed1 = { "sk_hgrunt_gspeed1","0" };
cvar_t	sk_hgrunt_gspeed2 = { "sk_hgrunt_gspeed2","0" };
cvar_t	sk_hgrunt_gspeed3 = { "sk_hgrunt_gspeed3","0" };

// Houndeye
cvar_t	sk_houndeye_health1 = { "sk_houndeye_health1","0" };
cvar_t	sk_houndeye_health2 = { "sk_houndeye_health2","0" };
cvar_t	sk_houndeye_health3 = { "sk_houndeye_health3","0" };

cvar_t	sk_houndeye_dmg_blast1 = { "sk_houndeye_dmg_blast1","0" };
cvar_t	sk_houndeye_dmg_blast2 = { "sk_houndeye_dmg_blast2","0" };
cvar_t	sk_houndeye_dmg_blast3 = { "sk_houndeye_dmg_blast3","0" };


// ISlave
cvar_t	sk_islave_health1 = { "sk_islave_health1","0" };
cvar_t	sk_islave_health2 = { "sk_islave_health2","0" };
cvar_t	sk_islave_health3 = { "sk_islave_health3","0" };

cvar_t	sk_islave_dmg_claw1 = { "sk_islave_dmg_claw1","0" };
cvar_t	sk_islave_dmg_claw2 = { "sk_islave_dmg_claw2","0" };
cvar_t	sk_islave_dmg_claw3 = { "sk_islave_dmg_claw3","0" };

cvar_t	sk_islave_dmg_clawrake1 = { "sk_islave_dmg_clawrake1","0" };
cvar_t	sk_islave_dmg_clawrake2 = { "sk_islave_dmg_clawrake2","0" };
cvar_t	sk_islave_dmg_clawrake3 = { "sk_islave_dmg_clawrake3","0" };

cvar_t	sk_islave_dmg_zap1 = { "sk_islave_dmg_zap1","0" };
cvar_t	sk_islave_dmg_zap2 = { "sk_islave_dmg_zap2","0" };
cvar_t	sk_islave_dmg_zap3 = { "sk_islave_dmg_zap3","0" };


// Icthyosaur
cvar_t	sk_ichthyosaur_health1 = { "sk_ichthyosaur_health1","0" };
cvar_t	sk_ichthyosaur_health2 = { "sk_ichthyosaur_health2","0" };
cvar_t	sk_ichthyosaur_health3 = { "sk_ichthyosaur_health3","0" };

cvar_t	sk_ichthyosaur_shake1 = { "sk_ichthyosaur_shake1","0" };
cvar_t	sk_ichthyosaur_shake2 = { "sk_ichthyosaur_shake2","0" };
cvar_t	sk_ichthyosaur_shake3 = { "sk_ichthyosaur_shake3","0" };


// Leech
cvar_t	sk_leech_health1 = { "sk_leech_health1","0" };
cvar_t	sk_leech_health2 = { "sk_leech_health2","0" };
cvar_t	sk_leech_health3 = { "sk_leech_health3","0" };

cvar_t	sk_leech_dmg_bite1 = { "sk_leech_dmg_bite1","0" };
cvar_t	sk_leech_dmg_bite2 = { "sk_leech_dmg_bite2","0" };
cvar_t	sk_leech_dmg_bite3 = { "sk_leech_dmg_bite3","0" };

// Controller
cvar_t	sk_controller_health1 = { "sk_controller_health1","0" };
cvar_t	sk_controller_health2 = { "sk_controller_health2","0" };
cvar_t	sk_controller_health3 = { "sk_controller_health3","0" };

cvar_t	sk_controller_dmgzap1 = { "sk_controller_dmgzap1","0" };
cvar_t	sk_controller_dmgzap2 = { "sk_controller_dmgzap2","0" };
cvar_t	sk_controller_dmgzap3 = { "sk_controller_dmgzap3","0" };

cvar_t	sk_controller_speedball1 = { "sk_controller_speedball1","0" };
cvar_t	sk_controller_speedball2 = { "sk_controller_speedball2","0" };
cvar_t	sk_controller_speedball3 = { "sk_controller_speedball3","0" };

cvar_t	sk_controller_dmgball1 = { "sk_controller_dmgball1","0" };
cvar_t	sk_controller_dmgball2 = { "sk_controller_dmgball2","0" };
cvar_t	sk_controller_dmgball3 = { "sk_controller_dmgball3","0" };

// Nihilanth
cvar_t	sk_nihilanth_health1 = { "sk_nihilanth_health1","0" };
cvar_t	sk_nihilanth_health2 = { "sk_nihilanth_health2","0" };
cvar_t	sk_nihilanth_health3 = { "sk_nihilanth_health3","0" };

cvar_t	sk_nihilanth_zap1 = { "sk_nihilanth_zap1","0" };
cvar_t	sk_nihilanth_zap2 = { "sk_nihilanth_zap2","0" };
cvar_t	sk_nihilanth_zap3 = { "sk_nihilanth_zap3","0" };

// Scientist
cvar_t	sk_scientist_health1 = { "sk_scientist_health1","0" };
cvar_t	sk_scientist_health2 = { "sk_scientist_health2","0" };
cvar_t	sk_scientist_health3 = { "sk_scientist_health3","0" };


// Snark
cvar_t	sk_snark_health1 = { "sk_snark_health1","0" };
cvar_t	sk_snark_health2 = { "sk_snark_health2","0" };
cvar_t	sk_snark_health3 = { "sk_snark_health3","0" };

cvar_t	sk_snark_dmg_bite1 = { "sk_snark_dmg_bite1","0" };
cvar_t	sk_snark_dmg_bite2 = { "sk_snark_dmg_bite2","0" };
cvar_t	sk_snark_dmg_bite3 = { "sk_snark_dmg_bite3","0" };

cvar_t	sk_snark_dmg_pop1 = { "sk_snark_dmg_pop1","0" };
cvar_t	sk_snark_dmg_pop2 = { "sk_snark_dmg_pop2","0" };
cvar_t	sk_snark_dmg_pop3 = { "sk_snark_dmg_pop3","0" };



// Zombie
cvar_t	sk_zombie_health1 = { "sk_zombie_health1","0" };
cvar_t	sk_zombie_health2 = { "sk_zombie_health2","0" };
cvar_t	sk_zombie_health3 = { "sk_zombie_health3","0" };

cvar_t	sk_zombie_dmg_one_slash1 = { "sk_zombie_dmg_one_slash1","0" };
cvar_t	sk_zombie_dmg_one_slash2 = { "sk_zombie_dmg_one_slash2","0" };
cvar_t	sk_zombie_dmg_one_slash3 = { "sk_zombie_dmg_one_slash3","0" };

cvar_t	sk_zombie_dmg_both_slash1 = { "sk_zombie_dmg_both_slash1","0" };
cvar_t	sk_zombie_dmg_both_slash2 = { "sk_zombie_dmg_both_slash2","0" };
cvar_t	sk_zombie_dmg_both_slash3 = { "sk_zombie_dmg_both_slash3","0" };


//Turret
cvar_t	sk_turret_health1 = { "sk_turret_health1","0" };
cvar_t	sk_turret_health2 = { "sk_turret_health2","0" };
cvar_t	sk_turret_health3 = { "sk_turret_health3","0" };


// MiniTurret
cvar_t	sk_miniturret_health1 = { "sk_miniturret_health1","0" };
cvar_t	sk_miniturret_health2 = { "sk_miniturret_health2","0" };
cvar_t	sk_miniturret_health3 = { "sk_miniturret_health3","0" };


// Sentry Turret
cvar_t	sk_sentry_health1 = { "sk_sentry_health1","0" };
cvar_t	sk_sentry_health2 = { "sk_sentry_health2","0" };
cvar_t	sk_sentry_health3 = { "sk_sentry_health3","0" };


// PLAYER WEAPONS

// Crowbar whack
cvar_t	sk_plr_crowbar1 = { "sk_plr_crowbar1","0" };
cvar_t	sk_plr_crowbar2 = { "sk_plr_crowbar2","0" };
cvar_t	sk_plr_crowbar3 = { "sk_plr_crowbar3","0" };

// Glock Round
cvar_t	sk_plr_9mm_bullet1 = { "sk_plr_9mm_bullet1","0" };
cvar_t	sk_plr_9mm_bullet2 = { "sk_plr_9mm_bullet2","0" };
cvar_t	sk_plr_9mm_bullet3 = { "sk_plr_9mm_bullet3","0" };

// 357 Round
cvar_t	sk_plr_357_bullet1 = { "sk_plr_357_bullet1","0" };
cvar_t	sk_plr_357_bullet2 = { "sk_plr_357_bullet2","0" };
cvar_t	sk_plr_357_bullet3 = { "sk_plr_357_bullet3","0" };

// MP5 Round
cvar_t	sk_plr_9mmAR_bullet1 = { "sk_plr_9mmAR_bullet1","0" };
cvar_t	sk_plr_9mmAR_bullet2 = { "sk_plr_9mmAR_bullet2","0" };
cvar_t	sk_plr_9mmAR_bullet3 = { "sk_plr_9mmAR_bullet3","0" };


// M203 grenade
cvar_t	sk_plr_9mmAR_grenade1 = { "sk_plr_9mmAR_grenade1","0" };
cvar_t	sk_plr_9mmAR_grenade2 = { "sk_plr_9mmAR_grenade2","0" };
cvar_t	sk_plr_9mmAR_grenade3 = { "sk_plr_9mmAR_grenade3","0" };


// Shotgun buckshot
cvar_t	sk_plr_buckshot1 = { "sk_plr_buckshot1","0" };
cvar_t	sk_plr_buckshot2 = { "sk_plr_buckshot2","0" };
cvar_t	sk_plr_buckshot3 = { "sk_plr_buckshot3","0" };


// Crossbow
cvar_t	sk_plr_xbow_bolt_client1 = { "sk_plr_xbow_bolt_client1","0" };
cvar_t	sk_plr_xbow_bolt_client2 = { "sk_plr_xbow_bolt_client2","0" };
cvar_t	sk_plr_xbow_bolt_client3 = { "sk_plr_xbow_bolt_client3","0" };

cvar_t	sk_plr_xbow_bolt_monster1 = { "sk_plr_xbow_bolt_monster1","0" };
cvar_t	sk_plr_xbow_bolt_monster2 = { "sk_plr_xbow_bolt_monster2","0" };
cvar_t	sk_plr_xbow_bolt_monster3 = { "sk_plr_xbow_bolt_monster3","0" };


// RPG
cvar_t	sk_plr_rpg1 = { "sk_plr_rpg1","0" };
cvar_t	sk_plr_rpg2 = { "sk_plr_rpg2","0" };
cvar_t	sk_plr_rpg3 = { "sk_plr_rpg3","0" };


// Zero Point Generator
cvar_t	sk_plr_gauss1 = { "sk_plr_gauss1","0" };
cvar_t	sk_plr_gauss2 = { "sk_plr_gauss2","0" };
cvar_t	sk_plr_gauss3 = { "sk_plr_gauss3","0" };


// Tau Cannon
cvar_t	sk_plr_egon_narrow1 = { "sk_plr_egon_narrow1","0" };
cvar_t	sk_plr_egon_narrow2 = { "sk_plr_egon_narrow2","0" };
cvar_t	sk_plr_egon_narrow3 = { "sk_plr_egon_narrow3","0" };

cvar_t	sk_plr_egon_wide1 = { "sk_plr_egon_wide1","0" };
cvar_t	sk_plr_egon_wide2 = { "sk_plr_egon_wide2","0" };
cvar_t	sk_plr_egon_wide3 = { "sk_plr_egon_wide3","0" };


// Hand Grendade
cvar_t	sk_plr_hand_grenade1 = { "sk_plr_hand_grenade1","0" };
cvar_t	sk_plr_hand_grenade2 = { "sk_plr_hand_grenade2","0" };
cvar_t	sk_plr_hand_grenade3 = { "sk_plr_hand_grenade3","0" };


// Satchel Charge
cvar_t	sk_plr_satchel1 = { "sk_plr_satchel1","0" };
cvar_t	sk_plr_satchel2 = { "sk_plr_satchel2","0" };
cvar_t	sk_plr_satchel3 = { "sk_plr_satchel3","0" };


// Tripmine
cvar_t	sk_plr_tripmine1 = { "sk_plr_tripmine1","0" };
cvar_t	sk_plr_tripmine2 = { "sk_plr_tripmine2","0" };
cvar_t	sk_plr_tripmine3 = { "sk_plr_tripmine3","0" };


// WORLD WEAPONS
cvar_t	sk_12mm_bullet1 = { "sk_12mm_bullet1","0" };
cvar_t	sk_12mm_bullet2 = { "sk_12mm_bullet2","0" };
cvar_t	sk_12mm_bullet3 = { "sk_12mm_bullet3","0" };

cvar_t	sk_9mmAR_bullet1 = { "sk_9mmAR_bullet1","0" };
cvar_t	sk_9mmAR_bullet2 = { "sk_9mmAR_bullet2","0" };
cvar_t	sk_9mmAR_bullet3 = { "sk_9mmAR_bullet3","0" };

cvar_t	sk_9mm_bullet1 = { "sk_9mm_bullet1","0" };
cvar_t	sk_9mm_bullet2 = { "sk_9mm_bullet2","0" };
cvar_t	sk_9mm_bullet3 = { "sk_9mm_bullet3","0" };


// HORNET
cvar_t	sk_hornet_dmg1 = { "sk_hornet_dmg1","0" };
cvar_t	sk_hornet_dmg2 = { "sk_hornet_dmg2","0" };
cvar_t	sk_hornet_dmg3 = { "sk_hornet_dmg3","0" };

// HEALTH/CHARGE
cvar_t	sk_suitcharger1 = { "sk_suitcharger1","0" };
cvar_t	sk_suitcharger2 = { "sk_suitcharger2","0" };
cvar_t	sk_suitcharger3 = { "sk_suitcharger3","0" };

cvar_t	sk_battery1 = { "sk_battery1","0" };
cvar_t	sk_battery2 = { "sk_battery2","0" };
cvar_t	sk_battery3 = { "sk_battery3","0" };

cvar_t	sk_healthcharger1 = { "sk_healthcharger1","0" };
cvar_t	sk_healthcharger2 = { "sk_healthcharger2","0" };
cvar_t	sk_healthcharger3 = { "sk_healthcharger3","0" };

cvar_t	sk_healthkit1 = { "sk_healthkit1","0" };
cvar_t	sk_healthkit2 = { "sk_healthkit2","0" };
cvar_t	sk_healthkit3 = { "sk_healthkit3","0" };

cvar_t	sk_scientist_heal1 = { "sk_scientist_heal1","0" };
cvar_t	sk_scientist_heal2 = { "sk_scientist_heal2","0" };
cvar_t	sk_scientist_heal3 = { "sk_scientist_heal3","0" };


// monster damage adjusters
cvar_t	sk_monster_head1 = { "sk_monster_head1","2" };
cvar_t	sk_monster_head2 = { "sk_monster_head2","2" };
cvar_t	sk_monster_head3 = { "sk_monster_head3","2" };

cvar_t	sk_monster_chest1 = { "sk_monster_chest1","1" };
cvar_t	sk_monster_chest2 = { "sk_monster_chest2","1" };
cvar_t	sk_monster_chest3 = { "sk_monster_chest3","1" };

cvar_t	sk_monster_stomach1 = { "sk_monster_stomach1","1" };
cvar_t	sk_monster_stomach2 = { "sk_monster_stomach2","1" };
cvar_t	sk_monster_stomach3 = { "sk_monster_stomach3","1" };

cvar_t	sk_monster_arm1 = { "sk_monster_arm1","1" };
cvar_t	sk_monster_arm2 = { "sk_monster_arm2","1" };
cvar_t	sk_monster_arm3 = { "sk_monster_arm3","1" };

cvar_t	sk_monster_leg1 = { "sk_monster_leg1","1" };
cvar_t	sk_monster_leg2 = { "sk_monster_leg2","1" };
cvar_t	sk_monster_leg3 = { "sk_monster_leg3","1" };

// player damage adjusters
cvar_t	sk_player_head1 = { "sk_player_head1","2" };
cvar_t	sk_player_head2 = { "sk_player_head2","2" };
cvar_t	sk_player_head3 = { "sk_player_head3","2" };

cvar_t	sk_player_chest1 = { "sk_player_chest1","1" };
cvar_t	sk_player_chest2 = { "sk_player_chest2","1" };
cvar_t	sk_player_chest3 = { "sk_player_chest3","1" };

cvar_t	sk_player_stomach1 = { "sk_player_stomach1","1" };
cvar_t	sk_player_stomach2 = { "sk_player_stomach2","1" };
cvar_t	sk_player_stomach3 = { "sk_player_stomach3","1" };

cvar_t	sk_player_arm1 = { "sk_player_arm1","1" };
cvar_t	sk_player_arm2 = { "sk_player_arm2","1" };
cvar_t	sk_player_arm3 = { "sk_player_arm3","1" };

cvar_t	sk_player_leg1 = { "sk_player_leg1","1" };
cvar_t	sk_player_leg2 = { "sk_player_leg2","1" };
cvar_t	sk_player_leg3 = { "sk_player_leg3","1" };


cvar_t	developer = { "developer", "0" };




void Profile_Init( void );

/*
================
COM_EntsForPlayerSlots

Returns the appropriate size of the edicts array to allocate
based on the stated # of max players
================
*/
int COM_EntsForPlayerSlots( int nPlayers )
{
	return 15 * (nPlayers - 1) + 800;
}

// TODO: Implement

/*
================
Host_EndGame
================
*/
void Host_EndGame( char* message, ... )
{
	int oldn;
	va_list		argptr;
	char		string[1024];

	va_start(argptr, message);
	vsnprintf(string, sizeof(string), message, argptr);
	va_end(argptr);
	Con_DPrintf("Host_EndGame: %s\n", string);

	oldn = cls.demonum;

	scr_disabled_for_loading = FALSE;

	if (sv.active)
		Host_ShutdownServer(FALSE);

	cls.demonum = oldn;

	if (cls.state == ca_dedicated)
		Sys_Error("Host_EndGame: %s\n", string);	// dedicated servers exit

	if (cls.demonum != -1)
	{
		CL_Disconnect_f();

		cls.demonum = oldn;

		Host_NextDemo();
		longjmp(host_enddemo, 1);
	}

	scr_disabled_for_loading = FALSE;

	CL_Disconnect();

	Cbuf_AddText("cd stop\n");
	Cbuf_Execute();
	longjmp(host_abortserver, 1);
}


/*
================
Host_Error

This shuts down both the client and server
================
*/
void Host_Error( char* error, ... )
{
	va_list		argptr;
	char		string[1024];
	static	qboolean inerror = FALSE;

	if (inerror)
		Sys_Error("Host_Error: recursively entered");
	inerror = TRUE;

	SCR_EndLoadingPlaque();		// reenable screen updates

	va_start(argptr, error);
	vsprintf(string, error, argptr);
	va_end(argptr);
	Con_Printf("Host_Error: %s\n", string);

	if (sv.active)
		Host_ShutdownServer(FALSE);

	if (cls.state == ca_dedicated)
		Sys_Error("Host_Error: %s\n", string);	// dedicated servers exit

	CL_Disconnect();
	cls.demonum = -1;

	inerror = FALSE;

	longjmp(host_abortserver, 1);
}

/*
================
Host_FindMaxClients

Determine the value of svs.maxclients
================
*/
void Host_FindMaxClients( void )
{
	int		i;
	client_t* cl;

	svs.maxclients = 1;

	// Check for command line override
	i = COM_CheckParm("-maxplayers");
	if (i)
	{
		svs.maxclients = Q_atoi(com_argv[i + 1]);
	}
	else if (isDedicated)
	{
		svs.maxclients = MAX_CLIENTS;
	}

	if (isDedicated)
		cls.state = ca_dedicated;
	else
		cls.state = ca_disconnected;

	if (svs.maxclients < 1)
	{
		svs.maxclients = DEFAULT_SERVER_CLIENTS;
	}
	else if (svs.maxclients > MAX_CLIENTS)
	{
		svs.maxclients = MAX_CLIENTS;
	}

	// Determine absolute limit
	svs.maxclientslimit = 32;

	// If we're a listen server and we're low on memory, reduce maximum player limit
	if (host_parms.memsize <= 0x1000000)
	{
		svs.maxclientslimit = 4;
	}

	svs.clients = (client_t*)Hunk_AllocName(sizeof(client_t) * svs.maxclientslimit, "clients");

	for (i = 0, cl = svs.clients; i < svs.maxclientslimit; i++, cl++)
	{
		memset(cl, 0, sizeof(client_t));

		cl->resourcesneeded.pPrev = &cl->resourcesneeded;
		cl->resourcesneeded.pNext = &cl->resourcesneeded;
		cl->resourcesonhand.pPrev = &cl->resourcesonhand;
		cl->resourcesonhand.pNext = &cl->resourcesonhand;
	}

	if (svs.maxclients > 1)
		Cvar_SetValue("deathmatch", 1);
	else
		Cvar_SetValue("deathmatch", 0);
}

/*
================
Host_InitLocal
================
*/
void Host_InitLocal( void )
{
	Host_InitCommands();

	Cvar_RegisterVariable(&maxfps);
	Cvar_RegisterVariable(&host_framerate);
	Cvar_RegisterVariable(&host_speeds);
	Cvar_RegisterVariable(&host_killtime);
	Cvar_RegisterVariable(&sys_ticrate);

	Cvar_RegisterVariable(&serverprofile);

	// multiplayer
	Cvar_RegisterVariable(&fraglimit);
	Cvar_RegisterVariable(&timelimit);
	Cvar_RegisterVariable(&teamplay);
	Cvar_RegisterVariable(&friendlyfire);
	Cvar_RegisterVariable(&falldamage);
	Cvar_RegisterVariable(&weaponstay);
	Cvar_RegisterVariable(&forcerespawn);
	Cvar_RegisterVariable(&forcerespawn);

	Cvar_RegisterVariable(&developer);

	Cvar_RegisterVariable(&displaysoundlist);
	Cvar_RegisterVariable(&deathmatch);
	Cvar_RegisterVariable(&mapcyclefile);
	Cvar_RegisterVariable(&coop);
	Cvar_RegisterVariable(&pausable);
	Cvar_RegisterVariable(&skill);

	// REGISTER CVARS FOR SKILL LEVEL STUFF
	// Agrunt
	Cvar_RegisterVariable(&sk_agrunt_health1);// {"sk_agrunt_health1","0"};
	Cvar_RegisterVariable(&sk_agrunt_health2);// {"sk_agrunt_health2","0"};
	Cvar_RegisterVariable(&sk_agrunt_health3);// {"sk_agrunt_health3","0"};

	Cvar_RegisterVariable(&sk_agrunt_dmg_punch1);// {"sk_agrunt_dmg_punch1","0"};
	Cvar_RegisterVariable(&sk_agrunt_dmg_punch2);// {"sk_agrunt_dmg_punch2","0"};
	Cvar_RegisterVariable(&sk_agrunt_dmg_punch3);// {"sk_agrunt_dmg_punch3","0"};

	// Apache
	Cvar_RegisterVariable(&sk_apache_health1);// {"sk_apache_health1","0"};
	Cvar_RegisterVariable(&sk_apache_health2);// {"sk_apache_health2","0"};
	Cvar_RegisterVariable(&sk_apache_health3);// {"sk_apache_health3","0"};

	// Barney
	Cvar_RegisterVariable(&sk_barney_health1);// {"sk_barney_health1","0"};
	Cvar_RegisterVariable(&sk_barney_health2);// {"sk_barney_health2","0"};
	Cvar_RegisterVariable(&sk_barney_health3);// {"sk_barney_health3","0"};

	// Bullsquid
	Cvar_RegisterVariable(&sk_bullsquid_health1);// {"sk_bullsquid_health1","0"};
	Cvar_RegisterVariable(&sk_bullsquid_health2);// {"sk_bullsquid_health2","0"};
	Cvar_RegisterVariable(&sk_bullsquid_health3);// {"sk_bullsquid_health3","0"};

	Cvar_RegisterVariable(&sk_bullsquid_dmg_bite1);// {"sk_bullsquid_dmg_bite1","0"};
	Cvar_RegisterVariable(&sk_bullsquid_dmg_bite2);// {"sk_bullsquid_dmg_bite2","0"};
	Cvar_RegisterVariable(&sk_bullsquid_dmg_bite3);// {"sk_bullsquid_dmg_bite3","0"};

	Cvar_RegisterVariable(&sk_bullsquid_dmg_whip1);// {"sk_bullsquid_dmg_whip1","0"};
	Cvar_RegisterVariable(&sk_bullsquid_dmg_whip2);// {"sk_bullsquid_dmg_whip2","0"};
	Cvar_RegisterVariable(&sk_bullsquid_dmg_whip3);// {"sk_bullsquid_dmg_whip3","0"};

	Cvar_RegisterVariable(&sk_bullsquid_dmg_spit1);// {"sk_bullsquid_dmg_spit1","0"};
	Cvar_RegisterVariable(&sk_bullsquid_dmg_spit2);// {"sk_bullsquid_dmg_spit2","0"};
	Cvar_RegisterVariable(&sk_bullsquid_dmg_spit3);// {"sk_bullsquid_dmg_spit3","0"};


	Cvar_RegisterVariable(&sk_bigmomma_health_factor1);// {"sk_bigmomma_health_factor1","1.0"};
	Cvar_RegisterVariable(&sk_bigmomma_health_factor2);// {"sk_bigmomma_health_factor2","1.0"};
	Cvar_RegisterVariable(&sk_bigmomma_health_factor3);// {"sk_bigmomma_health_factor3","1.0"};

	Cvar_RegisterVariable(&sk_bigmomma_dmg_slash1);// {"sk_bigmomma_dmg_slash1","50"};
	Cvar_RegisterVariable(&sk_bigmomma_dmg_slash2);// {"sk_bigmomma_dmg_slash2","50"};
	Cvar_RegisterVariable(&sk_bigmomma_dmg_slash3);// {"sk_bigmomma_dmg_slash3","50"};

	Cvar_RegisterVariable(&sk_bigmomma_dmg_blast1);// {"sk_bigmomma_dmg_blast1","100"};
	Cvar_RegisterVariable(&sk_bigmomma_dmg_blast2);// {"sk_bigmomma_dmg_blast2","100"};
	Cvar_RegisterVariable(&sk_bigmomma_dmg_blast3);// {"sk_bigmomma_dmg_blast3","100"};

	Cvar_RegisterVariable(&sk_bigmomma_radius_blast1);// {"sk_bigmomma_radius_blast1","250"};
	Cvar_RegisterVariable(&sk_bigmomma_radius_blast2);// {"sk_bigmomma_radius_blast2","250"};
	Cvar_RegisterVariable(&sk_bigmomma_radius_blast3);// {"sk_bigmomma_radius_blast3","250"};

	// Gargantua
	Cvar_RegisterVariable(&sk_gargantua_health1);// {"sk_gargantua_health1","0"};
	Cvar_RegisterVariable(&sk_gargantua_health2);// {"sk_gargantua_health2","0"};
	Cvar_RegisterVariable(&sk_gargantua_health3);// {"sk_gargantua_health3","0"};

	Cvar_RegisterVariable(&sk_gargantua_dmg_slash1);// {"sk_gargantua_dmg_slash1","0"};
	Cvar_RegisterVariable(&sk_gargantua_dmg_slash2);// {"sk_gargantua_dmg_slash2","0"};
	Cvar_RegisterVariable(&sk_gargantua_dmg_slash3);// {"sk_gargantua_dmg_slash3","0"};

	Cvar_RegisterVariable(&sk_gargantua_dmg_fire1);// {"sk_gargantua_dmg_fire1","0"};
	Cvar_RegisterVariable(&sk_gargantua_dmg_fire2);// {"sk_gargantua_dmg_fire2","0"};
	Cvar_RegisterVariable(&sk_gargantua_dmg_fire3);// {"sk_gargantua_dmg_fire3","0"};

	Cvar_RegisterVariable(&sk_gargantua_dmg_stomp1);// {"sk_gargantua_dmg_stomp1","0"};
	Cvar_RegisterVariable(&sk_gargantua_dmg_stomp2);// {"sk_gargantua_dmg_stomp2","0"};
	Cvar_RegisterVariable(&sk_gargantua_dmg_stomp3);// {"sk_gargantua_dmg_stomp3","0"};


	// Hassassin
	Cvar_RegisterVariable(&sk_hassassin_health1);// {"sk_hassassin_health1","0"};
	Cvar_RegisterVariable(&sk_hassassin_health2);// {"sk_hassassin_health2","0"};
	Cvar_RegisterVariable(&sk_hassassin_health3);// {"sk_hassassin_health3","0"};


	// Headcrab
	Cvar_RegisterVariable(&sk_headcrab_health1);// {"sk_headcrab_health1","0"};
	Cvar_RegisterVariable(&sk_headcrab_health2);// {"sk_headcrab_health2","0"};
	Cvar_RegisterVariable(&sk_headcrab_health3);// {"sk_headcrab_health3","0"};

	Cvar_RegisterVariable(&sk_headcrab_dmg_bite1);// {"sk_headcrab_dmg_bite1","0"};
	Cvar_RegisterVariable(&sk_headcrab_dmg_bite2);// {"sk_headcrab_dmg_bite2","0"};
	Cvar_RegisterVariable(&sk_headcrab_dmg_bite3);// {"sk_headcrab_dmg_bite3","0"};


	// Hgrunt 
	Cvar_RegisterVariable(&sk_hgrunt_health1);// {"sk_hgrunt_health1","0"};
	Cvar_RegisterVariable(&sk_hgrunt_health2);// {"sk_hgrunt_health2","0"};
	Cvar_RegisterVariable(&sk_hgrunt_health3);// {"sk_hgrunt_health3","0"};

	Cvar_RegisterVariable(&sk_hgrunt_kick1);// {"sk_hgrunt_kick1","0"};
	Cvar_RegisterVariable(&sk_hgrunt_kick2);// {"sk_hgrunt_kick2","0"};
	Cvar_RegisterVariable(&sk_hgrunt_kick3);// {"sk_hgrunt_kick3","0"};

	Cvar_RegisterVariable(&sk_hgrunt_pellets1);
	Cvar_RegisterVariable(&sk_hgrunt_pellets2);
	Cvar_RegisterVariable(&sk_hgrunt_pellets3);

	Cvar_RegisterVariable(&sk_hgrunt_gspeed1);
	Cvar_RegisterVariable(&sk_hgrunt_gspeed2);
	Cvar_RegisterVariable(&sk_hgrunt_gspeed3);

	// Houndeye
	Cvar_RegisterVariable(&sk_houndeye_health1);// {"sk_houndeye_health1","0"};
	Cvar_RegisterVariable(&sk_houndeye_health2);// {"sk_houndeye_health2","0"};
	Cvar_RegisterVariable(&sk_houndeye_health3);// {"sk_houndeye_health3","0"};

	Cvar_RegisterVariable(&sk_houndeye_dmg_blast1);// {"sk_houndeye_dmg_blast1","0"};
	Cvar_RegisterVariable(&sk_houndeye_dmg_blast2);// {"sk_houndeye_dmg_blast2","0"};
	Cvar_RegisterVariable(&sk_houndeye_dmg_blast3);// {"sk_houndeye_dmg_blast3","0"};


	// ISlave
	Cvar_RegisterVariable(&sk_islave_health1);// {"sk_islave_health1","0"};
	Cvar_RegisterVariable(&sk_islave_health2);// {"sk_islave_health2","0"};
	Cvar_RegisterVariable(&sk_islave_health3);// {"sk_islave_health3","0"};

	Cvar_RegisterVariable(&sk_islave_dmg_claw1);// {"sk_islave_dmg_claw1","0"};
	Cvar_RegisterVariable(&sk_islave_dmg_claw2);// {"sk_islave_dmg_claw2","0"};
	Cvar_RegisterVariable(&sk_islave_dmg_claw3);// {"sk_islave_dmg_claw3","0"};

	Cvar_RegisterVariable(&sk_islave_dmg_clawrake1);// {"sk_islave_dmg_clawrake1","0"};
	Cvar_RegisterVariable(&sk_islave_dmg_clawrake2);// {"sk_islave_dmg_clawrake2","0"};
	Cvar_RegisterVariable(&sk_islave_dmg_clawrake3);// {"sk_islave_dmg_clawrake3","0"};

	Cvar_RegisterVariable(&sk_islave_dmg_zap1);// {"sk_islave_dmg_zap1","0"};
	Cvar_RegisterVariable(&sk_islave_dmg_zap2);// {"sk_islave_dmg_zap2","0"};
	Cvar_RegisterVariable(&sk_islave_dmg_zap3);// {"sk_islave_dmg_zap3","0"};


	// Icthyosaur
	Cvar_RegisterVariable(&sk_ichthyosaur_health1);// {"sk_ichthyosaur_health1","0"};
	Cvar_RegisterVariable(&sk_ichthyosaur_health2);// {"sk_ichthyosaur_health2","0"};
	Cvar_RegisterVariable(&sk_ichthyosaur_health3);// {"sk_ichthyosaur_health3","0"};

	Cvar_RegisterVariable(&sk_ichthyosaur_shake1);// {"sk_ichthyosaur_health3","0"};
	Cvar_RegisterVariable(&sk_ichthyosaur_shake2);// {"sk_ichthyosaur_health3","0"};
	Cvar_RegisterVariable(&sk_ichthyosaur_shake3);// {"sk_ichthyosaur_health3","0"};



	// Leech
	Cvar_RegisterVariable(&sk_leech_health1);// {"sk_leech_health1","0"};
	Cvar_RegisterVariable(&sk_leech_health2);// {"sk_leech_health2","0"};
	Cvar_RegisterVariable(&sk_leech_health3);// {"sk_leech_health3","0"};

	Cvar_RegisterVariable(&sk_leech_dmg_bite1);// {"sk_leech_dmg_bite1","0"};
	Cvar_RegisterVariable(&sk_leech_dmg_bite2);// {"sk_leech_dmg_bite2","0"};
	Cvar_RegisterVariable(&sk_leech_dmg_bite3);// {"sk_leech_dmg_bite3","0"};


	// Controller
	Cvar_RegisterVariable(&sk_controller_health1);
	Cvar_RegisterVariable(&sk_controller_health2);
	Cvar_RegisterVariable(&sk_controller_health3);

	Cvar_RegisterVariable(&sk_controller_dmgzap1);
	Cvar_RegisterVariable(&sk_controller_dmgzap2);
	Cvar_RegisterVariable(&sk_controller_dmgzap3);

	Cvar_RegisterVariable(&sk_controller_speedball1);
	Cvar_RegisterVariable(&sk_controller_speedball2);
	Cvar_RegisterVariable(&sk_controller_speedball3);

	Cvar_RegisterVariable(&sk_controller_dmgball1);
	Cvar_RegisterVariable(&sk_controller_dmgball2);
	Cvar_RegisterVariable(&sk_controller_dmgball3);

	// Nihilanth
	Cvar_RegisterVariable(&sk_nihilanth_health1);// {"sk_nihilanth_health1","0"};
	Cvar_RegisterVariable(&sk_nihilanth_health2);// {"sk_nihilanth_health2","0"};
	Cvar_RegisterVariable(&sk_nihilanth_health3);// {"sk_nihilanth_health3","0"};

	Cvar_RegisterVariable(&sk_nihilanth_zap1);
	Cvar_RegisterVariable(&sk_nihilanth_zap2);
	Cvar_RegisterVariable(&sk_nihilanth_zap3);

	// Scientist
	Cvar_RegisterVariable(&sk_scientist_health1);// {"sk_scientist_health1","0"};
	Cvar_RegisterVariable(&sk_scientist_health2);// {"sk_scientist_health2","0"};
	Cvar_RegisterVariable(&sk_scientist_health3);// {"sk_scientist_health3","0"};


	// Snark
	Cvar_RegisterVariable(&sk_snark_health1);// {"sk_snark_health1","0"};
	Cvar_RegisterVariable(&sk_snark_health2);// {"sk_snark_health2","0"};
	Cvar_RegisterVariable(&sk_snark_health3);// {"sk_snark_health3","0"};

	Cvar_RegisterVariable(&sk_snark_dmg_bite1);// {"sk_snark_dmg_bite1","0"};
	Cvar_RegisterVariable(&sk_snark_dmg_bite2);// {"sk_snark_dmg_bite2","0"};
	Cvar_RegisterVariable(&sk_snark_dmg_bite3);// {"sk_snark_dmg_bite3","0"};

	Cvar_RegisterVariable(&sk_snark_dmg_pop1);// {"sk_snark_dmg_pop1","0"};
	Cvar_RegisterVariable(&sk_snark_dmg_pop2);// {"sk_snark_dmg_pop2","0"};
	Cvar_RegisterVariable(&sk_snark_dmg_pop3);// {"sk_snark_dmg_pop3","0"};



	// Zombie
	Cvar_RegisterVariable(&sk_zombie_health1);// {"sk_zombie_health1","0"};
	Cvar_RegisterVariable(&sk_zombie_health2);// {"sk_zombie_health3","0"};
	Cvar_RegisterVariable(&sk_zombie_health3);// {"sk_zombie_health3","0"};

	Cvar_RegisterVariable(&sk_zombie_dmg_one_slash1);// {"sk_zombie_dmg_one_slash1","0"};
	Cvar_RegisterVariable(&sk_zombie_dmg_one_slash2);// {"sk_zombie_dmg_one_slash2","0"};
	Cvar_RegisterVariable(&sk_zombie_dmg_one_slash3);// {"sk_zombie_dmg_one_slash3","0"};

	Cvar_RegisterVariable(&sk_zombie_dmg_both_slash1);// {"sk_zombie_dmg_both_slash1","0"};
	Cvar_RegisterVariable(&sk_zombie_dmg_both_slash2);// {"sk_zombie_dmg_both_slash2","0"};
	Cvar_RegisterVariable(&sk_zombie_dmg_both_slash3);// {"sk_zombie_dmg_both_slash3","0"};


	//Turret
	Cvar_RegisterVariable(&sk_turret_health1);// {"sk_turret_health1","0"};
	Cvar_RegisterVariable(&sk_turret_health2);// {"sk_turret_health2","0"};
	Cvar_RegisterVariable(&sk_turret_health3);// {"sk_turret_health3","0"};


	// MiniTurret
	Cvar_RegisterVariable(&sk_miniturret_health1);// {"sk_miniturret_health1","0"};
	Cvar_RegisterVariable(&sk_miniturret_health2);// {"sk_miniturret_health2","0"};
	Cvar_RegisterVariable(&sk_miniturret_health3);// {"sk_miniturret_health3","0"};


	// Sentry Turret
	Cvar_RegisterVariable(&sk_sentry_health1);// {"sk_sentry_health1","0"};
	Cvar_RegisterVariable(&sk_sentry_health2);// {"sk_sentry_health2","0"};
	Cvar_RegisterVariable(&sk_sentry_health3);// {"sk_sentry_health3","0"};


	// PLAYER WEAPONS

	// Crowbar whack
	Cvar_RegisterVariable(&sk_plr_crowbar1);// {"sk_plr_crowbar1","0"};
	Cvar_RegisterVariable(&sk_plr_crowbar2);// {"sk_plr_crowbar2","0"};
	Cvar_RegisterVariable(&sk_plr_crowbar3);// {"sk_plr_crowbar3","0"};

	// Glock Round
	Cvar_RegisterVariable(&sk_plr_9mm_bullet1);// {"sk_plr_9mm_bullet1","0"};
	Cvar_RegisterVariable(&sk_plr_9mm_bullet2);// {"sk_plr_9mm_bullet2","0"};
	Cvar_RegisterVariable(&sk_plr_9mm_bullet3);// {"sk_plr_9mm_bullet3","0"};

	// 357 Round
	Cvar_RegisterVariable(&sk_plr_357_bullet1);// {"sk_plr_357_bullet1","0"};
	Cvar_RegisterVariable(&sk_plr_357_bullet2);// {"sk_plr_357_bullet2","0"};
	Cvar_RegisterVariable(&sk_plr_357_bullet3);// {"sk_plr_357_bullet3","0"};

	// MP5 Round
	Cvar_RegisterVariable(&sk_plr_9mmAR_bullet1);// {"sk_plr_9mmAR_bullet1","0"};
	Cvar_RegisterVariable(&sk_plr_9mmAR_bullet2);// {"sk_plr_9mmAR_bullet2","0"};
	Cvar_RegisterVariable(&sk_plr_9mmAR_bullet3);// {"sk_plr_9mmAR_bullet3","0"};


	// M203 grenade
	Cvar_RegisterVariable(&sk_plr_9mmAR_grenade1);// {"sk_plr_9mmAR_grenade1","0"};
	Cvar_RegisterVariable(&sk_plr_9mmAR_grenade2);// {"sk_plr_9mmAR_grenade2","0"};
	Cvar_RegisterVariable(&sk_plr_9mmAR_grenade3);// {"sk_plr_9mmAR_grenade3","0"};


	// Shotgun buckshot
	Cvar_RegisterVariable(&sk_plr_buckshot1);// {"sk_plr_buckshot1","0"};
	Cvar_RegisterVariable(&sk_plr_buckshot2);// {"sk_plr_buckshot2","0"};
	Cvar_RegisterVariable(&sk_plr_buckshot3);// {"sk_plr_buckshot3","0"};


	// Crossbow
	Cvar_RegisterVariable(&sk_plr_xbow_bolt_monster1);// {"sk_plr_xbow_bolt1","0"};
	Cvar_RegisterVariable(&sk_plr_xbow_bolt_monster2);// {"sk_plr_xbow_bolt2","0"};
	Cvar_RegisterVariable(&sk_plr_xbow_bolt_monster3);// {"sk_plr_xbow_bolt3","0"};

	Cvar_RegisterVariable(&sk_plr_xbow_bolt_client1);// {"sk_plr_xbow_bolt1","0"};
	Cvar_RegisterVariable(&sk_plr_xbow_bolt_client2);// {"sk_plr_xbow_bolt2","0"};
	Cvar_RegisterVariable(&sk_plr_xbow_bolt_client3);// {"sk_plr_xbow_bolt3","0"};


	// RPG
	Cvar_RegisterVariable(&sk_plr_rpg1);// {"sk_plr_rpg1","0"};
	Cvar_RegisterVariable(&sk_plr_rpg2);// {"sk_plr_rpg2","0"};
	Cvar_RegisterVariable(&sk_plr_rpg3);// {"sk_plr_rpg3","0"};


	// Gauss Gun
	Cvar_RegisterVariable(&sk_plr_gauss1);// {"sk_plr_gauss1","0"};
	Cvar_RegisterVariable(&sk_plr_gauss2);// {"sk_plr_gauss2","0"};
	Cvar_RegisterVariable(&sk_plr_gauss3);// {"sk_plr_gauss3","0"};


	// Egon Gun
	Cvar_RegisterVariable(&sk_plr_egon_narrow1);// {"sk_plr_egon_narrow1","0"};
	Cvar_RegisterVariable(&sk_plr_egon_narrow2);// {"sk_plr_egon_narrow2","0"};
	Cvar_RegisterVariable(&sk_plr_egon_narrow3);// {"sk_plr_egon_narrow3","0"};

	Cvar_RegisterVariable(&sk_plr_egon_wide1);// {"sk_plr_egon_wide1","0"};
	Cvar_RegisterVariable(&sk_plr_egon_wide2);// {"sk_plr_egon_wide2","0"};
	Cvar_RegisterVariable(&sk_plr_egon_wide3);// {"sk_plr_egon_wide3","0"};


	// Hand Grendade
	Cvar_RegisterVariable(&sk_plr_hand_grenade1);// {"sk_plr_hand_grenade1","0"};
	Cvar_RegisterVariable(&sk_plr_hand_grenade2);// {"sk_plr_hand_grenade2","0"};
	Cvar_RegisterVariable(&sk_plr_hand_grenade3);// {"sk_plr_hand_grenade3","0"};


	// Satchel Charge
	Cvar_RegisterVariable(&sk_plr_satchel1);// {"sk_plr_satchel1","0"};
	Cvar_RegisterVariable(&sk_plr_satchel2);// {"sk_plr_satchel2","0"};
	Cvar_RegisterVariable(&sk_plr_satchel3);// {"sk_plr_satchel3","0"};


	// Tripmine
	Cvar_RegisterVariable(&sk_plr_tripmine1);// {"sk_plr_tripmine1","0"};
	Cvar_RegisterVariable(&sk_plr_tripmine2);// {"sk_plr_tripmine2","0"};
	Cvar_RegisterVariable(&sk_plr_tripmine3);// {"sk_plr_tripmine3","0"};


	// WORLD WEAPONS
	Cvar_RegisterVariable(&sk_12mm_bullet1);// {"sk_12mm_bullet1","0"};
	Cvar_RegisterVariable(&sk_12mm_bullet2);// {"sk_12mm_bullet2","0"};
	Cvar_RegisterVariable(&sk_12mm_bullet3);// {"sk_12mm_bullet3","0"};

	Cvar_RegisterVariable(&sk_9mmAR_bullet1);// {"sk_9mm_bullet1","0"};
	Cvar_RegisterVariable(&sk_9mmAR_bullet2);// {"sk_9mm_bullet1","0"};
	Cvar_RegisterVariable(&sk_9mmAR_bullet3);// {"sk_9mm_bullet1","0"};

	Cvar_RegisterVariable(&sk_9mm_bullet1);// {"sk_9mm_bullet1","0"};
	Cvar_RegisterVariable(&sk_9mm_bullet2);// {"sk_9mm_bullet2","0"};
	Cvar_RegisterVariable(&sk_9mm_bullet3);// {"sk_9mm_bullet3","0"};


	// HORNET
	Cvar_RegisterVariable(&sk_hornet_dmg1);// {"sk_hornet_dmg1","0"};
	Cvar_RegisterVariable(&sk_hornet_dmg2);// {"sk_hornet_dmg2","0"};
	Cvar_RegisterVariable(&sk_hornet_dmg3);// {"sk_hornet_dmg3","0"};

	// HEALTH/SUIT CHARGE DISTRIBUTION
	Cvar_RegisterVariable(&sk_suitcharger1);
	Cvar_RegisterVariable(&sk_suitcharger2);
	Cvar_RegisterVariable(&sk_suitcharger3);

	Cvar_RegisterVariable(&sk_battery1);
	Cvar_RegisterVariable(&sk_battery2);
	Cvar_RegisterVariable(&sk_battery3);

	Cvar_RegisterVariable(&sk_healthcharger1);
	Cvar_RegisterVariable(&sk_healthcharger2);
	Cvar_RegisterVariable(&sk_healthcharger3);

	Cvar_RegisterVariable(&sk_healthkit1);
	Cvar_RegisterVariable(&sk_healthkit2);
	Cvar_RegisterVariable(&sk_healthkit3);

	Cvar_RegisterVariable(&sk_scientist_heal1);
	Cvar_RegisterVariable(&sk_scientist_heal2);
	Cvar_RegisterVariable(&sk_scientist_heal3);

// monster damage adjusters
	Cvar_RegisterVariable(&sk_monster_head1);
	Cvar_RegisterVariable(&sk_monster_head2);
	Cvar_RegisterVariable(&sk_monster_head3);

	Cvar_RegisterVariable(&sk_monster_chest1);
	Cvar_RegisterVariable(&sk_monster_chest2);
	Cvar_RegisterVariable(&sk_monster_chest3);

	Cvar_RegisterVariable(&sk_monster_stomach1);
	Cvar_RegisterVariable(&sk_monster_stomach2);
	Cvar_RegisterVariable(&sk_monster_stomach3);

	Cvar_RegisterVariable(&sk_monster_arm1);
	Cvar_RegisterVariable(&sk_monster_arm2);
	Cvar_RegisterVariable(&sk_monster_arm3);

	Cvar_RegisterVariable(&sk_monster_leg1);
	Cvar_RegisterVariable(&sk_monster_leg2);
	Cvar_RegisterVariable(&sk_monster_leg3);

// player damage adjusters
	Cvar_RegisterVariable(&sk_player_head1);
	Cvar_RegisterVariable(&sk_player_head2);
	Cvar_RegisterVariable(&sk_player_head3);

	Cvar_RegisterVariable(&sk_player_chest1);
	Cvar_RegisterVariable(&sk_player_chest2);
	Cvar_RegisterVariable(&sk_player_chest3);

	Cvar_RegisterVariable(&sk_player_stomach1);
	Cvar_RegisterVariable(&sk_player_stomach2);
	Cvar_RegisterVariable(&sk_player_stomach3);

	Cvar_RegisterVariable(&sk_player_arm1);
	Cvar_RegisterVariable(&sk_player_arm2);
	Cvar_RegisterVariable(&sk_player_arm3);

	Cvar_RegisterVariable(&sk_player_leg1);
	Cvar_RegisterVariable(&sk_player_leg2);
	Cvar_RegisterVariable(&sk_player_leg3);
// END REGISTER CVARS FOR SKILL LEVEL STUFF

	Host_FindMaxClients();

	host_time = 1.0;		// so a think at time 0 won't get called
}

/*
===============
Host_WriteConfiguration

Writes key bindings and archived cvars to config.cfg
===============
*/
void Host_WriteConfiguration( void )
{
#ifndef SWDS
	FILE* f;

	if (!host_initialized)
	{
		return;
	}

	// dedicated servers initialize the host but don't parse and set the
	// config.cfg cvars
	if (cls.state != ca_dedicated)
	{
		f = fopen( va( "%s/config.cfg", com_gamedir ), "w" );
		if (!f)
		{
			Con_Printf( "Couldn't write config.cfg.\n" );
			return;
		}

		Key_WriteBindings( f );
		Cvar_WriteVariables( f );
		fclose( f );
	}
#endif
}

/*
=================
SV_ClientPrintf

Sends text across to be displayed
=================
*/
void SV_ClientPrintf( char* fmt, ... )
{
	va_list		argptr;
	char		string[1024];

	va_start( argptr, fmt );
	vsnprintf( string, sizeof( string ), fmt, argptr );
	va_end( argptr );

	MSG_WriteByte( &host_client->netchan.message, svc_print );
	MSG_WriteString( &host_client->netchan.message, string );
}

/*
=================
SV_BroadcastPrintf

Sends text to all active clients
=================
*/
void SV_BroadcastPrintf( char* fmt, ... )
{
	va_list		argptr;
	char		string[1024];
	int			i;

	va_start( argptr, fmt );
	vsnprintf( string, sizeof( string ), fmt, argptr );
	va_end( argptr );

	for (i = 0; i < svs.maxclients; i++)
	{
		if (svs.clients[i].active || svs.clients[i].spawned)
		{
			MSG_WriteByte( &svs.clients[i].netchan.message, svc_print );
			MSG_WriteString( &svs.clients[i].netchan.message, string );
		}
	}
}

/*
=================
Host_ClientCommands

Send text over to the client to be executed
=================
*/
void Host_ClientCommands( char* fmt, ... )
{
	va_list		argptr;
	char		string[1024];

	va_start( argptr, fmt );
	vsnprintf( string, sizeof( string ), fmt, argptr );
	va_end( argptr );

	string[sizeof( string ) - 1] = 0;

	MSG_WriteByte( &host_client->netchan.message, svc_stufftext );
	MSG_WriteString( &host_client->netchan.message, string );
}

/*
=================
SV_DropClient

Drops client from server, with explanation
if (crash = true), don't bother sending signofs
=================
*/
void SV_DropClient( client_t *cl, qboolean crash )
{
	// TODO: Implement
}

/*
==================
Host_ClearClients

==================
*/
void Host_ClearClients( qboolean bFramesOnly )
{
	int		i;

	host_client = svs.clients;

	for (i = 0; i < svs.maxclients; i++, host_client++)
	{
		// TODO: Implement
	}

	if (!bFramesOnly)
	{
		memset( svs.clients, 0, sizeof( client_t ) * svs.maxclientslimit );
	}
}

/*
==================
Host_ShutdownServer

This only happens at the end of a game, not between levels
==================
*/
void Host_ShutdownServer( qboolean crash )
{
//	int		i;

	if (!sv.active)
		return;

	sv.active = FALSE;

	if (cls.state == ca_active
	  || cls.state == ca_connected
	  || cls.state == ca_uninitialized
	  || cls.state == ca_connecting)
	{
		CL_Disconnect();
	}

	if (sv.active)
	{
		host_client = svs.clients;

		// TODO: implement

		// Clear all entities
		//SV_ClearEntities();
	}

	memset(&sv, 0, sizeof(server_t));

	//
	// clear structures
	//

	CL_ClearClientState();

	SV_ClearClientStates();

	Host_ClearClients(FALSE);

	Master_Shutdown();
}

void SV_ClearClientStates( void )
{
	int		i;
	client_t* cl;

	for (i = 0, cl = svs.clients; i < svs.maxclients; i++, cl++)
	{
		COM_ClearCustomizationList(&cl->customdata, FALSE);
		SV_ClearResourceLists(cl);
	}
}

/*
================
Host_ClearMemory

This clears all the memory used by both the client and server, but does
not reinitialize anything.
================
*/
void Host_ClearMemory( qboolean bQuiet )
{
	CM_FreePAS();

	// TODO: Implement

	if (!bQuiet)
		Con_DPrintf("Clearing memory\n");

	// TODO: Implement
}


//============================================================================


/*
===============
Host_FilterTime

Computes simulation time (FPS value)
===============
*/
qboolean Host_FilterTime( float time )
{
	realtime += time;

	if (!isDedicated)
	{
		if (maxfps.value >= 0.1)
		{
			if (maxfps.value > 72.0)
				Cvar_SetValue("maxfps", 72.0);
		}
		else
		{
			Cvar_SetValue("maxfps", 0.1);
		}

		if (!cls.timedemo && realtime - oldrealtime < 1.0 / maxfps.value)
			return FALSE;		// framerate is too high
	}

	host_frametime = realtime - oldrealtime;
	oldrealtime = realtime;

	if (host_framerate.value > 0.0)
		host_frametime = host_framerate.value;
	else
	{	// don't allow really long or short frames
		if (host_frametime > 0.1)
			host_frametime = 0.1;
		if (host_frametime < 0.001)
			host_frametime = 0.001;
	}	
		
	return TRUE;
}

void SV_FilterTime(float time)
{
	// TODO: Implement
}

void CL_FilterTime(float time)
{
	// TODO: Implement
}

/*
==================
Host_ServerFrame

==================
*/
void Host_ServerFrame( void )
{
	// TODO: Implement
}

void Master_RequestHeartbeat( adrlist_t* p )
{
	// TODO: Implement
	// https://github.com/shefben/world-opponent-network/blob/940fa4448afafbb7ed381e9bed580606ff7b6695/%20world-opponent-network/OSHLDS/master.c#L79
}

void Master_Heartbeat( void )
{
	if (gfNoMasterServer)
	{
		return;
	}

	// TODO: Implement
}

void Master_Shutdown( void )
{
	// TODO: Implement
}

void Master_AddServer( void )
{
	// TODO: Implement
}

void Sys_SetRegKeyValue(char *pszSubKey,
		char *pszElement,
		char *pszReturnString,
		int nReturnLength,
		char *pszDefaultValue)
{
	LONG lResult;           // Registry function result code
	HKEY hKey;              // Handle of opened/created key
	DWORD dwDisposition;    // Type of key opening event
	char szBuff[1024];      // Temp. buffer
	DWORD dwSize;           // Size of element data
	DWORD dwType;           // Type of key

	lResult = RegCreateKeyEx(
		HKEY_CURRENT_USER,	// handle of open key 
		pszSubKey,			// address of name of subkey to open 
		0,					// DWORD ulOptions,	  // reserved 
		"String",			// Type of value
		REG_OPTION_NON_VOLATILE, // Store permanently in reg.
		KEY_ALL_ACCESS,		// REGSAM samDesired, // security access mask 
		NULL,
		&hKey,				// Key we are creating
		&dwDisposition);    // Type of creation

	if (lResult != ERROR_SUCCESS)  // Failure
		return;

	// First time, just set to Valve default
	if (dwDisposition == REG_CREATED_NEW_KEY)
	{
		// Just Set the Values according to the defaults
		lResult = RegSetValueEx(hKey, pszElement, 0, REG_SZ, (CONST BYTE*)pszDefaultValue, strlen(pszDefaultValue) + 1);
	}
	else
	{
		// We opened the existing key. Now go ahead and find out how big the key is.
		dwSize = nReturnLength;
		lResult = RegQueryValueEx(hKey, pszElement, 0, &dwType, (unsigned char*)szBuff, &dwSize);

		if (lResult == ERROR_SUCCESS)
		{
			// Only copy strings, and only copy as much data as requested.
			if (dwType == REG_SZ)
			{
				strncpy(pszReturnString, szBuff, nReturnLength);
				pszReturnString[nReturnLength - 1] = '\0';
			}
		}
		else
		// Didn't find it, so write out new value
		{
			// Just Set the Values according to the defaults
			lResult = RegSetValueEx(hKey, pszElement, 0, REG_SZ, (CONST BYTE*)pszDefaultValue, strlen(pszDefaultValue) + 1);
		}
	}

	// Always close this key before exiting.
	RegCloseKey(hKey);
}

void Master_Init( void )
{
	// TODO: Implement
}

void Host_PostFrameRate(float frameTime)
{
	g_fFrameTime = frameTime;
}

/*
==================
Master_Heartbeat_f

Force last_heartbeat to invalid value so that we get updated immediately
==================
*/
void Master_Heartbeat_f( void )
{
	// TODO: This function is not supposed to be in host.c

	// ISteamMasterServerUpdater_ForceHeartbeat();

	adrlist_t* p;
	p = valvemaster_adr;
	while (p)
	{
		p->last_heartbeat = -9999.0f;

		p = p->next;
	}
}

/*
==================
Host_GetHostInfo
==================
*/
DLL_EXPORT void Host_GetHostInfo(float* fps, int* nActive, int* nBots, int* nMaxPlayers, char* pszMap)
{
	*fps = g_fFrameTime;

	// TODO: Implement
}


/*
==================
Host_Frame

Runs all active servers
==================
*/
void _Host_Frame(float time)
{
	static double		time1 = 0;
	static double		time2 = 0;
	static double		time3 = 0;
	float pass1, pass2, pass3;
	float frameTime;
	float fps;

	if (setjmp(host_enddemo))
		return;			// demo finished.

// decide the simulation time
	if (!Host_FilterTime(time))
		return;			// don't run too fast, or packets will flood out

	R_SetStackBase();

	if ((cls.state == ca_connected || cls.state == ca_uninitialized) && cls.signon == SIGNONS)
	{
		cls.state = ca_active;
	}

// get new key events
	Sys_SendKeyEvents();

	if (g_bInactive)
		return;

// allow mice or other external controllers to add commands
	IN_Commands();

// process console commands
	Cbuf_Execute();

	if (cls.state == ca_active)
		ClientDLL_UpdateClientData();

// if running the server locally, make intentions now
	if (sv.active)
		CL_SendCmd();

//-------------------
//
// server operations
//
//-------------------

	if (sv.active)
		Host_ServerFrame();

//-------------------
//
// client operations
//
//-------------------

// if running the server remotely, send intentions now after
// the incoming messages have been read
	if (!sv.active)
		CL_SendCmd();

	// fetch results from server
	CL_ReadPackets();

	if (cls.state == ca_active)
	{
		CL_SetUpPlayerPrediction(FALSE);
		CL_PredictMove();
		CL_SetUpPlayerPrediction(TRUE);
	}

	CL_EmitEntities();

	// Resend connection request if needed.
	CL_CheckForResend();

	while (CL_RequestMissingResources());

// check timeouts
	SV_CheckTimeouts();

	host_time += host_frametime;

	CAM_Think();

// update video
	time1 = Sys_FloatTime();
	if (!gfBackground)
	{
		// Refresh the screen
		SCR_UpdateScreen();

		// If recording movie and the console is totally up, then write out this frame to movie file.
		if (cl_inmovie && !scr_con_current)
		{
			VID_WriteBuffer(NULL);
		}
	}
	time2 = Sys_FloatTime();

	CL_UpdateSoundFade();

// update audio
	if (!gfBackground)
	{
		if (cls.signon == SIGNONS)
		{
			vec3_t vSoundForward, vSoundRight, vSoundUp;
			AngleVectors(r_playerViewportAngles, vSoundForward, vSoundRight, vSoundUp);

			S_Update(r_soundOrigin, vSoundForward, vSoundRight, vSoundUp);
			CL_DecayLights();
		}
		else
		{
			S_Update(vec3_origin, vec3_origin, vec3_origin, vec3_origin);
		}
	}

	CDAudio_Update();

	pass1 = (float)((time1 - time3) * 1000.0);
	time3 = Sys_FloatTime();
	pass2 = (float)((time2 - time1) * 1000.0);
	pass3 = (float)((time3 - time2) * 1000.0);

	frameTime = pass3 + pass2 + pass1;
	if (frameTime == 0.0)
	{
		fps = 100.0;
		Host_PostFrameRate(fps);
	}
	else
	{
		fps = 1000.0 / frameTime;
		Host_PostFrameRate(fps);
	}

	if (host_speeds.value)
	{
		int ent_count = 0;
		int i;

		// count used entities
		for (i = 0; i < sv.num_edicts; i++)
		{
			if (!sv.edicts[i].free)
				ent_count++;
		}

		Con_DPrintf("%3i fps tot %3.0f server %3.0f gfx %3.0f snd %d ents\n",
		  (int)fps,
		  pass1,
		  pass2,
		  pass3,
		  ent_count);
	}

	if ((giSubState & 4) && cls.state == ca_disconnected)
	{
		giActive = DLL_PAUSED;
	}

	host_framecount++;
}


/*
==============================
Host_Frame

==============================
*/
DLL_EXPORT int Host_Frame(float time, int iState, int* stateInfo)
{
	double	time1, time2;
	static double	timetotal = 0;
	static int		timecount = 0;
	int		i, c, m;

	if (setjmp(host_abortserver))
		return giActive;			// something bad happened, or the server disconnected

	if (cls.state == ca_active && g_bForceReloadOnCA_Active)
	{
		ExecuteProfileSettings(g_szProfileName);
		g_bForceReloadOnCA_Active = FALSE;
		memset(g_szProfileName, 0, sizeof(g_szProfileName));
	}

	giActive = iState;
	*stateInfo = 0;

	if (!serverprofile.value)
	{
		_Host_Frame(time);

		if (giStateInfo)
		{
			*stateInfo = giStateInfo;
			giStateInfo = 0;
			Cbuf_Execute();
		}

		if (cls.state == ca_disconnected && con_loading)
		{
			giActive = DLL_PAUSED;
		}

		return giActive;
	}

	time1 = Sys_FloatTime();
	_Host_Frame(time);
	time2 = Sys_FloatTime();

	if (giStateInfo)
	{
		*stateInfo = giStateInfo;
		giStateInfo = 0;
		Cbuf_Execute();
	}

	if (cls.state == ca_disconnected && con_loading)
	{
		giActive = DLL_PAUSED;
	}

	timetotal += time2 - time1;
	timecount++;

	if (timecount < 1000)
		return giActive;

	m = timetotal * 1000 / timecount;
	timecount = 0;
	timetotal = 0;
	c = 0;
	for (i = 0; i < svs.maxclients; i++)
	{
		if (svs.clients[i].active)
			c++;
	}
	Con_Printf("serverprofile: %2i clients %2i msec\n", c, m);

	return giActive;
}

/*
====================
Host_Init
====================
*/
int Host_Init( quakeparms_t* parms )
{
	if (standard_quake)
		minimum_memory = MINIMUM_MEMORY;
	else
		minimum_memory = MINIMUM_MEMORY_LEVELPAK;

	if (COM_CheckParm("-minmemory"))
		parms->memsize = minimum_memory;

	host_parms = *parms;

	if (parms->memsize < minimum_memory)
		Sys_Error("Only %4.1f megs of memory available, can't execute game", parms->memsize / (float)0x100000);

	com_argc = parms->argc;
	com_argv = parms->argv;

	Master_Init();

	realtime = 0.0;

	Memory_Init(parms->membase, parms->memsize);

	// Initialize command system
	Cbuf_Init();
	Cmd_Init();
	Cvar_CmdInit();

	V_Init();
	Chase_Init();

	COM_Init(parms->basedir);

	Host_InitLocal();

	W_LoadWadFile("gfx.wad");

	Key_Init();

	Con_Init();

	Decal_Init();
	Mod_Init();

	NET_Init();
	// Sequenced message stream layer.
	Netchan_Init();

	SV_Init();

	Con_DPrintf("Exe: "__TIME__" "__DATE__"\n");
	Con_DPrintf("%4.1f megabyte heap\n", parms->memsize / (1024 * 1024.0));

	R_InitTextures();		// needed even for dedicated servers

	if (cls.state != ca_dedicated)
	{
		int i;
		char* disk_basepal;

		disk_basepal = COM_LoadHunkFile("gfx/palette.lmp");
		if (!disk_basepal)
			Sys_Error("Couldn't load gfx/palette.lmp");

		host_basepal = Hunk_AllocName(sizeof(PackedColorVec) * 256, "palette.lmp");

		// Convert from BGR to RGBA
		for (i = 0; i < 256; i++)
		{
			host_basepal[i * 4 + 0] = disk_basepal[i * 3 + 2];
			host_basepal[i * 4 + 1] = disk_basepal[i * 3 + 1];
			host_basepal[i * 4 + 2] = disk_basepal[i * 3 + 0];
			host_basepal[i * 4 + 3] = 0; // alpha
		}

		ClientDLL_Init();

		if (!VID_Init(host_basepal))
			return 0;

		ClientDLL_HudVidInit();

		Draw_Init();

		SCR_Init();

		R_Init();

		S_Init();

		CL_Init();

		IN_Init();

		Launcher_InitCmds();
	}

	// Execute valve.rc
	Cbuf_InsertText("exec valve.rc\n");

#if defined ( GLQUAKE )
	GL_Config();
#endif

	// Mark hunklevel at end of startup
	Hunk_AllocName(0, "-HOST_HUNKLEVEL-");
	host_hunklevel = Hunk_LowMark();

	Profile_Init();

	// Mark DLL as active
	giActive = DLL_ACTIVE;
	// Enable rendering
	scr_skipupdate = FALSE;

	// Check for special -dev flag
	if (COM_CheckParm("-dev"))
	{
		Cvar_SetValue("sv_cheats", 1.0);
		Cvar_SetValue("developer", 1.0);
	}

	return 1;
}


/*
===============
Host_Shutdown

FIXME: this is a callback from Sys_Quit and Sys_Error.  It would be better
to run quit through here before the final handoff to the sys code.
===============
*/
void Host_Shutdown( void )
{
	static qboolean isdown = FALSE;

	if (isdown)
	{
		printf( "recursive shutdown\n" );
		return;
	}
	isdown = TRUE;

	if (host_initialized)
	{
		// Store active configuration settings
		Host_WriteConfiguration();
	}

	SV_DeallocateDynamicData();

	Master_Shutdown();

	scr_disabled_for_loading = TRUE;

	// sub_10078D40(cl_name.string) TODO: Implement

	Host_WriteConfiguration();

	SV_ClearChannel(FALSE);

	NET_Shutdown();
	S_Shutdown();
	IN_Shutdown();
	Con_Shutdown();

	if (cls.state != ca_dedicated)
	{
		VID_Shutdown();
	}

	ReleaseEntityDlls();
}

