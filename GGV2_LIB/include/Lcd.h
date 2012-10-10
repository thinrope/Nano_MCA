/*******************************************************************************
* File Name: lcd.h
*******************************************************************************/
#ifndef __LCD_H
#define __LCD_H

// display orientation
typedef enum {HORZ = 0, VERT = 1} orient;

#define FONT_WIDTH		8
#define FONT_HEIGHT 	14
// define display field widths in pixels

// define display field widths in pixels

#define CHAR_WIDTH		8
#define CHAR_HEIGHT		16
#define FONT_WIDTH		8
#define FONT_HEIGHT 	14
#define U8_DIGITS    	3
#define U8_WIDTH 		(U8_DIGITS * CHAR_WIDTH)
#define S8_DIGITS    	4
#define S8_WIDTH 		(S8_DIGITS * CHAR_WIDTH)
#define U16_DIGITS    	5
#define U16_WIDTH 		(U16_DIGITS * CHAR_WIDTH)
#define S16_DIGITS    	6
#define S16_WIDTH 		(S16_DIGITS * CHAR_WIDTH)
#define U32_DIGITS      12
#define U32_WIDTH 		(U32_DIGITS * CHAR_WIDTH)
#define S32_DIGITS      13
#define S32_WIDTH 		(S32_DIGITS * CHAR_WIDTH)
#define FIXED_DIGITS	10
#define FIXED_WIDTH		(FIXED_DIGITS * CHAR_WIDTH)
#define BOOL_CHARS		5
#define BOOL_WIDTH		(BOOL_CHARS * CHAR_WIDTH)
#define SPACE_WIDTH		CHAR_WIDTH
#define HEX_DIGITS		8
#define HEX_WIDTH		(HEX_DIGITS * CHAR_WIDTH)

#define VERT_WIDTH		240
#define VERT_HEIGHT		320
#define HORZ_WIDTH		320
#define HORZ_HEIGHT		239

#define CHAR_HEIGHT		16
#define SHORT_DIGITS    7
#define SHORT_WIDTH 	(SHORT_DIGITS * CHAR_WIDTH)
#define INT_DIGITS      14
#define INT_WIDTH 		(INT_DIGITS * CHAR_WIDTH)

//============================== Color definitions =============================

#define RGB(_R,_G,_B) (((_R & 0x3E) >> 1) | ((_G & 0x3f) << 5) | ((_B & 0x3e) << 10))
#define WHITE       RGB(63,63,63)
#define YEL         RGB(63,63,0)
#define DRK_YEL     RGB(32,32,0)
#define RED         RGB(63,0,0)
#define DRK_RED     RGB(32,0,0)
#define GRN         RGB(0,63,0)
#define DRK_GRN     RGB(0,32,0)
#define BLUE		RGB(0,0,63)
#define DRK_BLUE	RGB(0,0,32)
#define BLACK       RGB(0,0,0)
#define GRAY		RGB(24,24,24)


#define FRM_COLOR   	GRAY
#define BKGND_COLOR 	BLACK
#define CURSOR_COLOR	RED
#define TXT_COLOR 		WHITE
#define EMER_TXT_COLOR 	RED
#define SPECT_COLOR 	BLUE
#define PEAK_COLOR 		GRN


//========================== Forward function delclaratons =====================

void Lcd_Wrt_Reg(u16 Reg, u16 Data);
void LCD_Initial(void);
void Point_SCR(u16 x0, u16 y0);
void Clear_Screen(u16 bg);

u16 Get_Font_8x14(u8 chr, u8 row);
//u8 Get_Ref_Wave(u16 idx);

void Display_Str(u16 x0, u16 y0, u16 fg, u16 bg, const char *s);
//void Display_Info(u16 x0, u16 y0, char *Pre, long Num);
//void Display_Logo(const u8 *Pict, u16 fg, u16 x, u16 y, u16 dx, u16 dy);

#endif

/********************************* END OF FILE ********************************/
