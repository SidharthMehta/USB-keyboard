#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>

#define MASK(x) (1UL << (x))

// Trace visualizer may use these signals
// Debug Signals on port B
#define DBG_PORT FPTB

#define	DBG_0 0 // Warning: PTB0 is used as BUCK_SENSE_M when using SMPS
								// Not brought out to Debug header J6
// On debug header J6
#define DBG_1 1 	
#define DBG_2 2	  
#define DBG_3 3		
#define DBG_4 8		
#define DBG_5 9		
#define DBG_6 10  
#define DBG_7 11
// Not on debug header J6
#define	DBG_CD_4	4
#define	DBG_CD_5	5
#define	DBG_CD_6	6
#define	DBG_CD_7	7
#define	DBG_C_12	12

// Additional debug signals for direct user control
// User debug signals on port E
#define UDBG_PORT FPTE
// On SPI debug connector J7 -- order is shuffled for easy connections
// Inner edge of connector
#define UDBG_1 2	 	// CLK: PTE2
#define UDBG_2 3		// DO: 	PTE3
#define UDBG_3 4 		// DI: PTE1 	
#define UDBG_4 1		// CS: PTE4
// Outer edge of connector

#define UDEBUG_START(channel) { UDBG_PORT->PSOR = MASK(channel); } 
#define UDEBUG_STOP(channel) { UDBG_PORT->PCOR = MASK(channel); }
#define UDEBUG_TOGGLE(channel) { UDBG_PORT->PTOR = MASK(channel); }

void Init_Debug_Signals(void);

#endif // DEBUG_H
