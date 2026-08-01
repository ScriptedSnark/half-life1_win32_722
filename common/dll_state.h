// DLL State Flags

#define DLL_INACTIVE 0		// no dll
#define DLL_ACTIVE   1		// dll is running
#define DLL_PAUSED   2		// dll is paused
#define DLL_CLOSE    3		// closing down dll
#define DLL_TRANS    4 		// Level Transition
#define DLL_RESTART  5		// engine is shutting down but will restart right away

// DLL Pause reasons

#define DLL_NORMAL        0   // User hit Esc or something.
#define DLL_QUIT          4   // Quit now

// DLL Substate info ( not relevant )
#define ENG_NORMAL         (1<<0)
#define ENG_RESET          (1<<1)   // Force state to latch back to ENG_NORMAL
#define ENG_NOLOADCONSOLE  (1<<2)
#define ENG_NOINPUTCONTROL (1<<3)
#define ENG_ESCAPEEXITS    (1<<4)   // Hitting escape exits the level.  Valve.bsp

// DLL State info
#define STATE_DORMANT		0
#define STATE_ENDTRAINING	1	// OEM
#define STATE_ENDLOGO		2	// OEM
#define STATE_ENDDEMO		3	// OEM
#define STATE_QUITTING		4	// exiting the engine
#define STATE_GOINGTOWC		5	// changing the window to Worldcraft