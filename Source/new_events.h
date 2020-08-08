#include<MKL25Z4.h>

/*Constants, DO NOT CHANGE*/
#define THREAD_BLOCKED 0								
#define THREAD_SWITCHED 1
#define THREAD_PREEMPTED 2

/*NUM_THREADS should be sum of Timer Thread, Idle thread and number of your threads(NUM_THREADS = USER_THREADS + 2)*/
#define NUM_THREADS 7

/*MAX_PORT_THREADS is maximum pins availabe for a thread on a PORT*/
#define MAX_PORT_THREADS 8

/* TV_DETERM_TIMING (when non-zero) reduces timing variability in the 
thread visualizer operations. Buggy: 0 value not working! */
#define TV_DETERM_TIMING 1

/*NOP_COUNT defines how many nops between toggling the pins when thread is blocked*/
#define NOP_COUNT	5

/*The port on which debug signals need to be observed. Default - PORTB selected.  
PORTB - FPTB
PORTC - FPTC
PORTA - FPTA
PORTD - FPTD
PORTE - FPTE
*/
//#define PORT FPTB

/*Variable to hold PORT value that can be used in debug.c*/
//FGPIO_Type *Debug_Gating = PORT;

/*Array of structures to hold Thread ID, PORT and pin details
Format of the entry for each thread:
 ___________ _______ _______
|	Thread ID	| PORT	|	Pin		|				
|___________|_______|_______|			

*/

/*
Keep number of entries same as NUM_THREADS
Thread ID(first field) always 0 here as it is updated when threads are created
Port (second field) selected as FPTB,FPTC or FPTD based on requirement for the thread.
Pin (third field) given values from the available Port pins. Filled with defualt available pins on PortB.

PORTB - 0,1,2,3,8,9,10,11
PORTC - Here, PORTC pins are used for LCD interfacing. DO NOT USE PORTC.
				Ideally, in other prjects, 0,1,2,3,4,5,6,7,8,9,10,11,12.
PORTD - 0,2,3,4,5,6,7


*/		

struct thread_ids{
	osThreadId_t id;
	FGPIO_Type *Port;
	uint16_t pin;
} Threads_array [NUM_THREADS] = {
															{0,FPTB,1},
															{0,FPTB,2},
															{0,FPTB,3},
															{0,FPTB,8},
															{0,FPTB,9},
															{0,FPTB,10},
															{0,FPTB,11}};

/*Last two entries always for Idle Thread and Timer Thread*/
													
															
															
extern void debug_signal(int id, int func, FGPIO_Type *port);
