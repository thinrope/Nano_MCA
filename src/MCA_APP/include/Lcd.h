/*******************************************************************************
 File Name: lcd.h
*******************************************************************************/

/***************************************************************************
 *   Copyright (C) 2012 by LeDoyle R. Pingel, safelyiniowa@yahoo.com       *
 *                                                                         *
 *   This file is part of Gamma Grapher Version 2                          *
 *                                                                         *
 *   Gamma Grapher Version 2 is free software; you can redistribute it     *
 *   and/or modify it under the terms of the GNU General Public License as *
 *	 published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This software is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with OpenGamma. If not, see <http://www.gnu.org/licenses/>.     *
 ***************************************************************************/



#ifndef __LCD_H
#define __LCD_H


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

//======================= display color mapping ================================
// R = 5 bits 0:4, G = 6 bits 5:10, B = 5 bits 11:15
#define RGB(_R,_G,_B) (((_R & 0x3E) >> 1) | ((_G & 0x3f) << 5) | ((_B & 0x3e) << 10))



//========================= color definitions ==================================
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



//========================= type definitions ===============================

//===================== fuction declarations ===================================
void LCD_WR_REG(u16 reg, u16 data);
void LCD_SET_WINDOW(u16 x1, u16 x2,u16 y1, u16 y2);
void Point_SCR(u16 x0, u16 y0);
void Set_Pixel(u16 color);
void Clear_Screen(u16 color);
void Display_Frame(void);
void Fill_Rectangle(u16 x0, u16 y0, u16 width, u16 height, u16 color);
void Rounded_Rectangle(u16 x0, u16 y0, short width, short height, u16 color);
void Display_Int(u16 x0, u16 y0, u16 fg, u16 bg, int num);
void Display_Bool(u16 x0, u16 y0, u16 fg, u16 bg, bool val);
void Display_Str(u16 x0, u16 y0, u16 fg, u16 bg, char *s);
void Draw_Vert_Line(u16 x, u16 y1, u16 y2, u16 color);
void Draw_Horz_Line(u16 y, u16 x1, u16 x2, u16 color);
void Draw_Ti_Mark(u16 x, u16 y, u16 color);
void Draw_Trig_Mark(u16 x, u16 y, u16 color);
void Draw_Dot(u16 x, u16 y, u16 color);
void Display_Info(char *Pre, u32 Num);
#endif

/********************************* END OF FILE ********************************/
