#ifndef THREADS_H
#define THREADS_H

#include <cmsis_os2.h>
#include "LCD_driver.h"
#include <MKL25Z4.H>

#define THREAD_READ_TS_PERIOD_MS (100)  // 1 tick/ms
#define THREAD_USB_KEYBOARD_PERIOD_MS (100)  // 1 tick/ms
#define THREAD_USB_MOUSE_PERIOD_MS (100)  // 1 tick/ms
#define THREAD_READ_ACCELEROMETER_PERIOD_MS (100)  // 1 tick/ms

#define THREAD_RTS_PRIO 					osPriorityNormal
#define THREAD_USB_KEYBOARD_PRIO 	osPriorityNormal
#define THREAD_USB_MOUSE_PRIO 		osPriorityNormal
#define THREAD_RA_PRIO 						osPriorityNormal

// Custom stack sizes for larger threads
#define READ_ACCEL_STK_SZ 512
#define READ_TS_STK_SZ 192

void Init_Debug_Signals(void);

void Create_OS_Objects(void);
 
extern osThreadId_t t_Read_TS, t_USB_keyboard, t_USB_mouse, t_Read_Accelerometer;
extern osEventFlagsId_t e_USB_keyboard_transmit;
extern osMessageQueueId_t  m_USB_keyboard_transmit ;
extern osEventFlagsId_t e_USB_mouse_transmit;
extern osMessageQueueId_t  m_USB_mouse_transmit ;

#define message_count 1
typedef struct {
	uint8_t modifier;
	uint8_t key;
} keyboard;

typedef struct {
	uint8_t buttons;
	int8_t x;
	int8_t y;
} mouse;
#endif // THREADS_H
