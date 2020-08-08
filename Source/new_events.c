#include <string.h>
#include "cmsis_compiler.h"
#include "rtx_evr.h"
#include "debug.h"
#include "new_events.h"
#include <MKL25Z4.h>


/* Function to implement BLOCKED, PREEMPTED or SWITCHED functionality based on input*/
void debug_signal(int id, int func, FGPIO_Type *port){
	volatile int j;
	switch(func){
		
		case(THREAD_BLOCKED): // --__--_________
		{
				port->PCOR=MASK(id);					/*Blocked thread*/
				for(j = 0;j<NOP_COUNT;j++){
					__nop();
				}														/*To add delay between consecutive Toggling of the same debug pin*/
				port->PTOR=MASK(id);
				for(j = 0;j<NOP_COUNT;j++){
					__nop();
				}	
				port->PTOR=MASK(id);
				break;
		}
		case(THREAD_SWITCHED): // __--------------
		{
				port->PSOR=MASK(id);							/*Setting the Debug pin */
				break;
		}
		
		case(THREAD_PREEMPTED): // --____________
		{
				port->PCOR=MASK(id);							/*Clearing the Debug pin */
				break;
		}
		default:
				break;
	}
	
}	
	

/*Overriding Existing Event Recorder functions.*/
/*Kept empty for future purposes*/
void EvrRtxKernelStart (void) {
	
}

/*Add the Thread ID of the thread created into the ThreadID field of the respective thread */
void EvrRtxThreadCreated (osThreadId_t thread_id, uint32_t thread_addr, const char *name) {
	static volatile int i=0;
	if (i < NUM_THREADS) {
		Threads_array[i].id = thread_id;
		i=i+1;
	}
}


/*Left empty for future modification */
void EvrRtxThreadYield (void) {
	
}

/*Thread Blocked event */
void EvrRtxThreadBlocked (osThreadId_t thread_id, uint32_t timeout) {
	int i = NUM_THREADS, j;
	for(j=0;j<NUM_THREADS;j++){
		if(thread_id == Threads_array[j].id){
			i = j;
#if TV_DETERM_TIMING
#else
			break;
#endif
		}
	}
	if (i<NUM_THREADS)
		debug_signal(Threads_array[i].pin,THREAD_BLOCKED,Threads_array[i].Port);			
}

/*Thread getting Switched in */

void EvrRtxThreadSwitched (osThreadId_t thread_id) {
	int i = NUM_THREADS, j;

	for(j=0;j<NUM_THREADS;j++){
		if(thread_id==Threads_array[j].id){
			i = j;
#if TV_DETERM_TIMING
#else
			break;
#endif
		}		
	}
	if (i < NUM_THREADS)
		debug_signal(Threads_array[i].pin,THREAD_SWITCHED,Threads_array[i].Port);
}

/*When the thread gets preempted by a Higher priority thread*/
void EvrRtxThreadPreempted (osThreadId_t thread_id) {
	int i = NUM_THREADS, j;
	for(j=0;j<NUM_THREADS;j++){
		if(thread_id == Threads_array[j].id){
				i = j;
		}
#if TV_DETERM_TIMING
#else
		break;
#endif
	}	
	if (i < NUM_THREADS)
		debug_signal(Threads_array[i].pin,THREAD_PREEMPTED,Threads_array[i].Port);
}
/*Added because they hamper the functioning of the RTOS. They corrupt the Return address on the stack.*/
void EvrRtxMessageQueueNew (uint32_t msg_count, uint32_t msg_size, const osMessageQueueAttr_t *attr) {
}


void EvrRtxMessageQueueGet (osMessageQueueId_t mq_id, void *msg_ptr, uint8_t *msg_prio, uint32_t timeout) {
}
