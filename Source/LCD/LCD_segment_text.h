#ifndef LCD_SEGMENT_TEXT_H
#define LCD_SEGMENT_TEXT_H

/* Segment display constants */
#define W_SEG (5*8)
#define H_SEG (16*8)


#define DASH_SEG (10)
#define ALL_SEG (12)

#define FONT_8_SEG (0)

#if FONT_8_SEG
	#define SEG_MASK_START (0x080)
	#define NUM_SEGS (8)
	#define SEG_DP_7SEG (0x100)
	#define STROKE_WID_SEG (14) // stroke width
	#define DP_SEG (8)
#else // FONT_10_SEG
	#define SEG_MASK_START (0x0200)
	#define NUM_SEGS (10)
	#define SEG_DP_7SEG (0x0400)
	#define STROKE_WID_SEG (10) // stroke width
	#define DP_SEG (10)
#endif


void GrLCD_draw_seg_digit(unsigned char digit, unsigned char x0, unsigned char y0, unsigned char color);
void GrLCD_update_seg_digit(unsigned char new_digit, unsigned char old_digit, unsigned char x0, unsigned char y0);

#endif
