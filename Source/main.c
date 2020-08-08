/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include <MKL25Z4.H>
#include <stdio.h>
#include "gpio_defs.h"
#include <cmsis_os2.h>
#include "threads.h"

#include "LCD.h"
#include "LCD_driver.h"
#include "font.h"

#include "LEDs.h"
#include "timers.h"
#include "delay.h"
#include "profile.h"
#include "math.h"
#include "EventRecorder.h"
#include "I2C.h"
#include "mma8451.h"

#include "usb_config.h"
/*
Changes to make for lab code
Remove RTOS calls for delay, semaphore, mutex, message queue, etc.
Remove debug set/clear statements

*/

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {

	Init_Debug_Signals();
	Init_RGB_LEDs();
	Control_RGB_LEDs(0,0,0);			

	LCD_Init();
	LCD_Text_Init(1);
	LCD_Erase();

	Delay(70);
	LCD_Erase();

	usb_init();
	i2c_init();											// init I2C peripheral
	if (!init_mma()) {							// init accelerometer
		I2C_OK = 0;	// code won't read accelerometer
		// Initialize variables for fake data
		acc_X = 0;
		acc_Y = 0;
		acc_Z = -4000;
	} else {
		I2C_OK = 1;
	}

	
	osKernelInitialize();
	Create_OS_Objects();
	EventRecorderInitialize(EventRecordAll, 1U);
	osKernelStart();	
	while(1);
}
