#include <stdint.h>
#include <stdio.h>
#include <cmsis_os2.h>
#include <MKL25Z4.h>

#include "LCD.h"
#include "touchscreen.h"
#include "LCD_driver.h"
#include "font.h"
#include "threads.h"
#include "MMA8451.h"
#include "gpio_defs.h"
#include "debug.h"

#include "usb_config.h"
#include "LEDs.h"
#include "endpoint.h"
#include "buffer_descriptor.h"

#include "ST7789.h"
#include "T6963.h"

void Thread_Read_TS(void * arg);
void Thread_Read_Accelerometer(void * arg);

osThreadId_t t_Read_TS, t_USB_keyboard, t_USB_mouse, t_Read_Accelerometer;
osEventFlagsId_t e_USB_keyboard_transmit, e_USB_mouse_transmit;
osMessageQueueId_t  m_USB_keyboard_transmit, m_USB_mouse_transmit ;

// Thread priority options: osPriority[RealTime|High|AboveNormal|Normal|BelowNormal|Low|Idle]

const osThreadAttr_t Read_TS_attr = {
  .priority = THREAD_RTS_PRIO            
};

const osThreadAttr_t USB_keyboard_attr = {
  .priority = THREAD_USB_KEYBOARD_PRIO            
};

const osThreadAttr_t USB_mouse_attr = {
  .priority = THREAD_USB_MOUSE_PRIO            
};

const osThreadAttr_t Read_Accelerometer_attr = {
  .priority = THREAD_RA_PRIO,            
	.stack_size = READ_ACCEL_STK_SZ
};

const osEventFlagsAttr_t e_USB_attr = {      
};

osMutexId_t LCD_mutex;

void Create_OS_Objects(void) {
	
	t_Read_TS = osThreadNew(Thread_Read_TS, NULL, &Read_TS_attr); 
	t_USB_keyboard = osThreadNew(Thread_USB_keyboard, NULL, &USB_keyboard_attr);
	t_USB_mouse = osThreadNew(Thread_USB_mouse, NULL, &USB_mouse_attr);
	t_Read_Accelerometer = osThreadNew(Thread_Read_Accelerometer, NULL, &Read_Accelerometer_attr);
	
	e_USB_keyboard_transmit = osEventFlagsNew(&e_USB_attr);
	m_USB_keyboard_transmit = osMessageQueueNew(message_count, sizeof(keyboard), NULL);
	e_USB_mouse_transmit = osEventFlagsNew(&e_USB_attr);
	m_USB_mouse_transmit = osMessageQueueNew(message_count, sizeof(mouse), NULL);
}

void Thread_Read_TS(void * arg) {
	PT_T p, p1, p2;
	COLOR_T c;
	keyboard k;
	
	c.R = 255;
	c.G = 0;
	c.B = 0;
	
	p1.X = 0;
	p1.Y = 80;
	p2.X= 240;
	p2.Y = 80;
	LCD_Draw_Line(&p1, &p2, &c);
	p1.X = 0;
	p1.Y = 160;
	p2.X= 240;
	p2.Y = 160;
	LCD_Draw_Line(&p1, &p2, &c);
	p1.X = 0;
	p1.Y = 240;
	p2.X= 240;
	p2.Y = 240;
	LCD_Draw_Line(&p1, &p2, &c);
	
	LCD_Text_PrintStr_RC(LCD_MAX_ROWS-14, 8, "A");
	LCD_Text_PrintStr_RC(LCD_MAX_ROWS-10, 8, "B");
	LCD_Text_PrintStr_RC(LCD_MAX_ROWS-6, 8, "C");
	LCD_Text_PrintStr_RC(LCD_MAX_ROWS-2, 8, "D");
	
	while (1) {
		UDEBUG_START(UDBG_3);
		if (LCD_TS_Read(&p)) { 
				if (p.Y < 80) {
					LCD_Text_PrintStr_RC(0, 0, "A");
					k.key = 4;
					k.modifier = 0;
					osMessageQueuePut(m_USB_keyboard_transmit, &k, 0, osWaitForever);
				}	else if (p.Y < 160) {
					LCD_Text_PrintStr_RC(0, 0, "B");
					k.key = 5;
					k.modifier = 0;
					osMessageQueuePut(m_USB_keyboard_transmit, &k, 0, osWaitForever);
				}	else if (p.Y < 240) {
					LCD_Text_PrintStr_RC(0, 0, "C");
					k.key = 6;
					k.modifier = 0;
					osMessageQueuePut(m_USB_keyboard_transmit, &k, 0, osWaitForever);
				} else {
					LCD_Text_PrintStr_RC(0, 0, "D");
					k.key = 7;
					k.modifier = 0;
					osMessageQueuePut(m_USB_keyboard_transmit, &k, 0, osWaitForever);
				}
		} 
		UDEBUG_STOP(UDBG_3);
		osDelay(THREAD_READ_TS_PERIOD_MS);
	}
}

void Thread_Read_Accelerometer(void * arg) {	
	mouse m;
	
	while (1) {
		
		read_full_xyz();
		m.buttons = 0;
		m.x = acc_X/255;
		m.y = acc_Y/255;
		osMessageQueuePut(m_USB_mouse_transmit, &m, 0, osWaitForever);
		osDelay(THREAD_READ_ACCELEROMETER_PERIOD_MS);
	}
}

