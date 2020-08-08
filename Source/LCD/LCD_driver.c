/* Code for configuring and controlling TFT LCD using ILI9341 controller. */

#include <stdint.h>
#include <stdio.h>
#include <cmsis_os.h>
#include "MKL25Z4.h"

#include "LCD.h"
#include "LCD_driver.h"
#include "font.h"

#include "gpio_defs.h"
#include "timers.h"

extern void Delay(uint32_t);

/* Is set to one if touchscreen been calibrated. */
uint8_t LCD_TS_Calibrated = 1;
uint32_t LCD_TS_X_Scale=209, LCD_TS_X_Offset=6648;
uint32_t LCD_TS_Y_Scale=159, LCD_TS_Y_Offset=6689;

PT_T TS_Min;
PT_T TS_Max;

/* Initialize GPIO ports for communicating with TFT LCD controller. */
void LCD_GPIO_Init(void) {
	unsigned i;
	
	// Enable clock to ports
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTE_MASK;
	
	// Make digital pins GPIO
	// Data bus pins
	for (i=LCD_DB8_POS; i<=LCD_DB15_POS; i++) {
		PORTC->PCR[i] &= ~PORT_PCR_MUX_MASK;
		PORTC->PCR[i] |= PORT_PCR_MUX(1);
		PTC->PDDR |= MASK(i);
	}
	
	// Data/Command
	PORTC->PCR[LCD_D_NC_POS] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[LCD_D_NC_POS] |= PORT_PCR_MUX(1);
	// /Write
	PORTC->PCR[LCD_NWR_POS] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[LCD_NWR_POS] |= PORT_PCR_MUX(1);
	// /Read
	PORTC->PCR[LCD_NRD_POS] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[LCD_NRD_POS] |= PORT_PCR_MUX(1);
	// /Reset
	PORTC->PCR[LCD_NRST_POS] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[LCD_NRST_POS] |= PORT_PCR_MUX(1);

	// Set port directions to outputs
	FPTC->PDDR |= MASK(LCD_D_NC_POS);
	FPTC->PDDR |= MASK(LCD_NWR_POS);
	FPTC->PDDR |= MASK(LCD_NRD_POS);
	FPTC->PDDR |= MASK(LCD_NRST_POS);

	// Set initial control line values to inactive (1)
	FPTC->PDOR |= MASK(LCD_D_NC_POS);
	FPTC->PDOR |= MASK(LCD_NWR_POS);
	FPTC->PDOR |= MASK(LCD_NRD_POS);
	FPTC->PDOR |= MASK(LCD_NRST_POS);
}

/* Initialize hardware for LCD backlight control and set to default value. */
static void LCD_Init_Backlight(void) {
	PWM_Init(LCD_BL_TPM, LCD_BL_TPM_CHANNEL, LCD_BL_PERIOD, LCD_BL_PERIOD/3);	
	//set multiplexer to connect TPM1 Ch 0 to PTA12
	PORTA->PCR[12] &= PORT_PCR_MUX_MASK; 
	PORTA->PCR[12] |= PORT_PCR_MUX(3); 
}

/* Set backlight brightness for LCD via duty cycle. */
void LCD_Set_Backlight_Brightness(uint32_t brightness_percent){
	if (brightness_percent > 100)
		brightness_percent = 100;
	PWM_Set_Value(LCD_BL_TPM, LCD_BL_TPM_CHANNEL, (brightness_percent*LCD_BL_PERIOD)/100);
}

/* Write one byte as a command to the TFT LCD controller. */
static void LCD_24S_Write_Command(uint8_t command)
{
	GPIO_ResetBit(LCD_D_NC_POS);
	GPIO_Write(command);
	GPIO_ResetBit(LCD_NWR_POS);
	GPIO_SetBit(LCD_NWR_POS);
}

/* Write one byte as data to the TFT LCD Controller. */
static void LCD_24S_Write_Data(uint8_t data)
{
	GPIO_SetBit(LCD_D_NC_POS);
	GPIO_Write(data);
	GPIO_ResetBit(LCD_NWR_POS);
	GPIO_SetBit(LCD_NWR_POS);
}

void LCD_Controller_Init(const LCD_CTLR_INIT_SEQ_T init_seq[]) {
	unsigned i=0, done=0;
	
	GPIO_SetBit(LCD_NRD_POS);
	GPIO_ResetBit(LCD_NWR_POS);
	GPIO_ResetBit(LCD_NRST_POS);
	Delay(100);
	GPIO_SetBit(LCD_NRST_POS);
	Delay(100);
	
	while (!done) {
		switch (init_seq[i].Type) {
			case LCD_CTRL_INIT_SEQ_CMD: 
				LCD_24S_Write_Command(init_seq[i].Value);
				break;
			case LCD_CTRL_INIT_SEQ_DAT: 
				LCD_24S_Write_Data(init_seq[i].Value);
				break;
			case LCD_CTRL_INIT_SEQ_END:
				done = 1;
				break;
			default:
				break;
		}
		i++;
	}
	Delay(10);
}

/* Initialize the relevant peripherals (GPIO, TPM, ADC) and the display
components (TFT LCD controller, touch screen and backlight controller). */ 
void LCD_Init()
{
	LCD_GPIO_Init();
	LCD_TS_Init();
	LCD_Init_Backlight();

#if LCD_CONTROLLER == CTLR_ILI9341
	LCD_Controller_Init(Init_Seq_ILI9341);
#else
	LCD_Controller_Init(Init_Seq_ST7789);
#endif
	
}

/* Set the pixel at pos to the given color. */
void LCD_Plot_Pixel(PT_T * pos, COLOR_T * color) {
	uint8_t b1, b2;

	// Column address set 0x2a
	LCD_24S_Write_Command(0x002A); //column address set
	LCD_24S_Write_Data(0);
	LCD_24S_Write_Data(pos->X & 0xff); //start 
	LCD_24S_Write_Data(0x0000);
	LCD_24S_Write_Data(0x00EF); //end 0x00EF
	
	// Page (row) address set 0x2b
	LCD_24S_Write_Command(0x002B); //page address set
	LCD_24S_Write_Data(pos->Y >> 8);
	LCD_24S_Write_Data(pos->Y & 0xff); //start 
	LCD_24S_Write_Data(0x0001);
	LCD_24S_Write_Data(0x003F); //end 0x013F	
	
	// Memory Write 0x2c
	// 16 bpp, 5-6-5. Assume color channel data is left-aligned
	b1 = (color->R&0xf8) | ((color->G&0xe0)>>5);
	b2 = ((color->G&0x1c)<<3) | ((color->B&0xf8)>>3);

	LCD_24S_Write_Command(0x002c);
	LCD_24S_Write_Data(b1);
	LCD_24S_Write_Data(b2);
}

/* Fill the entire display buffer with the given color. */
void LCD_Fill_Buffer(COLOR_T * color) {
	uint32_t x, y;
	uint8_t b1, b2;
	
	// Enable access to full screen, reset write pointer to origin
	LCD_24S_Write_Command(0x002A); //column address set
	LCD_24S_Write_Data(0x0000);
	LCD_24S_Write_Data(0x0000); //start 0x0000
	LCD_24S_Write_Data(0x0000);
	LCD_24S_Write_Data(0x00EF); //end 0x00EF
	LCD_24S_Write_Command(0x002B); //page address set
	LCD_24S_Write_Data(0x0000);
	LCD_24S_Write_Data(0x0000); //start 0x0000
	LCD_24S_Write_Data(0x0001);
	LCD_24S_Write_Data(0x003F); //end 0x013F
	
	// Memory Write 0x2c
	// 16 bpp, 5-6-5. Assume color channel data is left-aligned
	b1 = (color->R&0xf8) | ((color->G&0xe0)>>5);
	b2 = ((color->G&0x1c)<<3) | ((color->B&0xf8)>>3);
	
	LCD_24S_Write_Command(0x002c);
	for (y=0; y<320; y++) {
		for (x=0; x<240; x++) {
			LCD_24S_Write_Data(b1);
			LCD_24S_Write_Data(b2);
		}
	}		
}
/* Draw a rectangle from p1 to p2 filled with specified color. */
void LCD_Fill_Rectangle(PT_T * p1, PT_T * p2, COLOR_T * color) {
	uint32_t n;
	uint8_t b1, b2;
	uint16_t c_min, c_max, r_min, r_max;
	
	c_min = MIN(p1->X, p2->X);
	c_max = MAX(p1->X, p2->X);
	c_max = MIN(c_max, LCD_WIDTH-1);
	
	r_min = MIN(p1->Y, p2->Y);
	r_max = MAX(p1->Y, p2->Y);
	r_max = MIN(r_max, LCD_HEIGHT-1);

	n = (c_max - c_min + 1)*(r_max - r_min + 1);
	if (n == 0)
		return;
	
	// Enable access to full screen, reset write pointer to origin
	LCD_24S_Write_Command(0x002A); //column address set
	LCD_24S_Write_Data(c_min >> 8);
	LCD_24S_Write_Data(c_min & 0xff); //start 
	LCD_24S_Write_Data(c_max >> 8);
	LCD_24S_Write_Data(c_max & 0xff); //end 
	LCD_24S_Write_Command(0x002B); //page address set
	LCD_24S_Write_Data(r_min >> 8);
	LCD_24S_Write_Data(r_min & 0xff); //start 
	LCD_24S_Write_Data(r_max >> 8);
	LCD_24S_Write_Data(r_max & 0xff); //end 
	
	// Memory Write 0x2c
	// 16 bpp, 5-6-5. Assume color channel data is left-aligned
	b1 = (color->R&0xf8) | ((color->G&0xe0)>>5);
	b2 = ((color->G&0x1c)<<3) | ((color->B&0xf8)>>3);
	
	LCD_24S_Write_Command(0x002c);
	
	while (n-- > 0) {
		LCD_24S_Write_Data(b1);
		LCD_24S_Write_Data(b2);
	}	
}

/* Prepare LCD controller draw rectangle from p1 to p2 using future pixels provided 
by calls to LCD_Write_Rectangle_pixel. 
Return value is number of pixels expected. 
*/
uint32_t LCD_Start_Rectangle(PT_T * p1, PT_T * p2) {
	uint32_t n;
	uint16_t c_min, c_max, r_min, r_max;
	
	// Find bounds of rectangle
	c_min = MIN(p1->X, p2->X);
	c_max = MAX(p1->X, p2->X);
	
	r_min = MIN(p1->Y, p2->Y);
	r_max = MAX(p1->Y, p2->Y);

	// Clip to display size
	c_max = MIN(c_max, LCD_WIDTH-1);
	r_max = MIN(r_max, LCD_HEIGHT-1);
	
	n = (c_max - c_min + 1)*(r_max - r_min + 1);
	if (n > 0) {
		// Enable access to full screen, reset write pointer to origin
		LCD_24S_Write_Command(0x002A); //column address set
		LCD_24S_Write_Data(c_min >> 8);
		LCD_24S_Write_Data(c_min & 0xff); //start 
		LCD_24S_Write_Data(c_max >> 8);
		LCD_24S_Write_Data(c_max & 0xff); //end 
		LCD_24S_Write_Command(0x002B); //page address set
		LCD_24S_Write_Data(r_min >> 8);
		LCD_24S_Write_Data(r_min & 0xff); //start 
		LCD_24S_Write_Data(r_max >> 8);
		LCD_24S_Write_Data(r_max & 0xff); //end 
		
		// Memory Write 0x2c
		LCD_24S_Write_Command(0x002c);
	}	
	return n;
}

/* Plot this pixel in the next location as defined by LCD_Start_Rectangle. You must 
have called LCD_Write_Rectangle before calling this function. */
void LCD_Write_Rectangle_Pixel(COLOR_T * color, unsigned int count) {
	uint8_t b1, b2;

	// 16 bpp, 5-6-5. Assume color channel data is left-aligned
	b1 = (color->R&0xf8) | ((color->G&0xe0)>>5);
	b2 = ((color->G&0x1c)<<3) | ((color->B&0xf8)>>3);
	while (count--) {
		LCD_24S_Write_Data(b1);
		LCD_24S_Write_Data(b2);
	}
}

/* Enable (on > 0) or disable LED backlight via LCD controller. */
void LCD_Set_BL(uint8_t on) {
	LCD_24S_Write_Command(0x53);
	LCD_24S_Write_Data(0x28 | (on? 4 : 0));
}

/* Set brightness via LCD controller. */
void LCD_Set_Controller_Brightness(uint8_t brightness) {
	LCD_24S_Write_Command(0x51);
	LCD_24S_Write_Data(brightness);
}
