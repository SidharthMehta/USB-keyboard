#include "LCD_segment_text.h"
#include "T6963.h" // TODO: clean this up, shouldn't need it

#if 0
// short non-overlapping horizontal segments
const Point_T SegStart[9] = { {STROKE_WID_SEG, 0},    // a
	{W_SEG-STROKE_WID_SEG, 0},   // b
	{W_SEG-STROKE_WID_SEG, H_SEG/2}, 		// c
	{STROKE_WID_SEG, H_SEG},	// d
	{0, (H_SEG)/2},	// e
	{0, 0},	// f
	{STROKE_WID_SEG, (H_SEG-STROKE_WID_SEG)/2},	// g
	{(W_SEG-STROKE_WID_SEG)/2, 0}, // h
	{W_SEG+5, H_SEG-STROKE_WID_SEG} // DP
	};	
const Point_T SegEnd[9] = {{W_SEG-STROKE_WID_SEG-1,STROKE_WID_SEG}, // a
	{W_SEG,H_SEG/2}, 		// b
	{W_SEG,H_SEG},		// c
	{W_SEG-STROKE_WID_SEG-1,H_SEG-STROKE_WID_SEG},		// d
	{STROKE_WID_SEG,H_SEG},			// e
	{STROKE_WID_SEG,H_SEG/2},		// f
	{W_SEG-STROKE_WID_SEG-1,(H_SEG+STROKE_WID_SEG)/2},	// g
	{(W_SEG+STROKE_WID_SEG)/2,H_SEG},	// h
	{W_SEG+2,H_SEG} // DP
	};
#endif

#if FONT_8_SEG
// full length horizontal segments
Point_T SegStart[] = { {0, 0},    // a
	{W_SEG-STROKE_WID_SEG, 0},   // b
	{W_SEG-STROKE_WID_SEG, H_SEG/2}, 		// c
	{0, H_SEG},	// d
	{0, (H_SEG)/2},	// e
	{0, 0},	// f
	{0, (H_SEG-STROKE_WID_SEG)/2},	// g
	{(W_SEG-STROKE_WID_SEG)/2, 0}, // h
	{W_SEG+3, H_SEG-STROKE_WID_SEG} // DP
	};	
Point_T SegEnd[] = {{W_SEG,STROKE_WID_SEG}, // a
	{W_SEG,H_SEG/2}, 		// b
	{W_SEG,H_SEG},		// c
	{W_SEG,H_SEG-STROKE_WID_SEG},		// d
	{STROKE_WID_SEG,H_SEG},			// e
	{STROKE_WID_SEG,H_SEG/2},		// f
	{W_SEG,(H_SEG+STROKE_WID_SEG)/2},	// g
	{(W_SEG+STROKE_WID_SEG)/2,H_SEG},	// h
	{W_SEG+1+STROKE_WID_SEG,H_SEG} // DP
	};
unsigned int SegsOn[] = {0x00fc, 0x0001, 0x00da, 0x00f2, 0x0066, 0x00b6, 
	0x00be, 0x00e0, 0x00fe, 0x00e6, 0x0002, 0x0100, 0x1FF};

#else // FONT_10_SEG	

Point_T SegStart[] = {{STROKE_WID_SEG,0},
	{W_SEG-STROKE_WID_SEG,0},
	{W_SEG-STROKE_WID_SEG,(H_SEG-STROKE_WID_SEG)/2}, //C
	{W_SEG-STROKE_WID_SEG,(H_SEG+STROKE_WID_SEG)/2},
	{STROKE_WID_SEG,H_SEG-STROKE_WID_SEG},
	{0,(H_SEG+STROKE_WID_SEG)/2}, //F
	{0,(H_SEG-STROKE_WID_SEG)/2},
	{0,0},
	{STROKE_WID_SEG,(H_SEG-STROKE_WID_SEG)/2},//I
	{(W_SEG-STROKE_WID_SEG)/2,0},
	{W_SEG+3, H_SEG-STROKE_WID_SEG} // DP
};

Point_T SegEnd[] = {{W_SEG-STROKE_WID_SEG,STROKE_WID_SEG},
	{W_SEG,(H_SEG-STROKE_WID_SEG)/2},
	{W_SEG,(H_SEG+STROKE_WID_SEG)/2}, //C
	{W_SEG,H_SEG},
	{W_SEG-STROKE_WID_SEG,H_SEG},
	{STROKE_WID_SEG,H_SEG}, //F
	{STROKE_WID_SEG,(H_SEG+STROKE_WID_SEG)/2},
	{STROKE_WID_SEG,(H_SEG-STROKE_WID_SEG)/2},
	{W_SEG-STROKE_WID_SEG,(H_SEG+STROKE_WID_SEG)/2}, //I
	{(W_SEG+STROKE_WID_SEG)/2,H_SEG},
	{W_SEG+1+STROKE_WID_SEG,H_SEG} // DP
};

unsigned int SegsOn[] = {0x0ff, 0x200, 0x0177, 0x11f, 0x01ce, 0x01dd, 
	0x01fc, 0x00f, 0x01ff, 0x01cf, 0x0144, 0x0100, 0x03ff};
	
#endif

/*
v- Unused
--v-- Segments a-d
----v-- Segments e-h
------V-- DP (msb)
0 0 0 0
*/ 

	void GrLCD_draw_seg_digit(unsigned char digit, unsigned char x0, unsigned char y0, unsigned char color) {
	unsigned int seg_mask = 1; // SEG_MASK_START; 
	unsigned char seg_num = 0;
	unsigned char x1, y1, x2, y2;
	
	if (digit > 9) {
		if (digit == '.') {
			//	digit = DP_SEG;
			x1 = x0 + SegStart[DP_SEG].x;
			y1 = y0 + SegStart[DP_SEG].y;
			x2 = x0 + SegEnd[DP_SEG].x;
			y2 = y0 + SegEnd[DP_SEG].y;
			GrLCD_DrawRectangle(x1, y1, x2, y2, color);
			return;
		} else if (digit == '-') {
			digit = DASH_SEG;
		} else if ((digit >= '0') && (digit <= '9')) {
			digit -= '0';
		}
	}
	
	do {
		if (SegsOn[digit] & seg_mask) {
			x1 = x0 + SegStart[seg_num].x;
			y1 = y0 + SegStart[seg_num].y;
			x2 = x0 + SegEnd[seg_num].x;
			y2 = y0 + SegEnd[seg_num].y;
			GrLCD_DrawRectangle(x1, y1, x2, y2, color);
		}
		seg_mask <<= 1; // /= 2
		seg_num++;
	} while (seg_num < NUM_SEGS);
}

void GrLCD_update_seg_digit(unsigned char new_digit, unsigned char old_digit, unsigned char x0, unsigned char y0) {
	unsigned int seg_mask = 1; // SEG_MASK_START;
	unsigned char seg_num = 0;
	unsigned char x1, y1, x2, y2;
	if (new_digit > 9) {
		if (new_digit == '.') {
			new_digit = DP_SEG;
		} else if (new_digit == '-') {
			new_digit = DASH_SEG;
		} else if ((new_digit >= '0') && (new_digit <= '9')) {
			new_digit -= '0';
		}
	}
	if (old_digit > 9) {
		if (old_digit == '.') {
			old_digit = DP_SEG;
		} else if (old_digit == '-') {
			old_digit = DASH_SEG;
		} else if ((old_digit >= '0') && (old_digit <= '9')) {
			old_digit -= '0';
		}
	}
	
	do {
		if ((SegsOn[old_digit] & seg_mask) && (!(SegsOn[new_digit] & seg_mask))) { // Erase the segment
			x1 = x0 + SegStart[seg_num].x;
			y1 = y0 + SegStart[seg_num].y;
			x2 = x0 + SegEnd[seg_num].x;
			y2 = y0 + SegEnd[seg_num].y;
			GrLCD_DrawRectangle(x1, y1, x2, y2, 0);
		} 
		seg_mask <<= 1; // /= 2;
		seg_num++;
	} while (seg_num < NUM_SEGS);

	seg_mask = 1; // SEG_MASK_START;
	seg_num = 0;	
	do {
		if ((!(SegsOn[old_digit] & seg_mask)) && (SegsOn[new_digit] & seg_mask)) { // Draw the segment
			x1 = x0 + SegStart[seg_num].x;
			y1 = y0 + SegStart[seg_num].y;
			x2 = x0 + SegEnd[seg_num].x;
			y2 = y0 + SegEnd[seg_num].y;
			GrLCD_DrawRectangle(x1, y1, x2, y2, 1);
		} 
		seg_mask <<= 1; // /= 2;
		seg_num++;
	} while (seg_num < NUM_SEGS);
}

