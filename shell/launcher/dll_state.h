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

// DLL State info
#define STATE_TRAINING		1	// training
#define STATE_ENDLOGO		2	// end of the "Half-Life" logo
#define STATE_ENDDEMO		3	// end of the demo

#define STATE_WORLDCRAFT	5	// end of the demo