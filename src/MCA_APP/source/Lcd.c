/*******************************************************************************
File Name: lcd.c
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
#include <string.h>
#include "stm32f10x_type.h"
#include "Main.h"
#include "Function.h"
#include "Menu.h"
#include "Lcd.h"
#include "stm32f10x_it.h"
#include "HW_V1_Config.h"

extern configurations *confp;

// NOTE: using __Add_Color() and __Erase_Color() makes possible overlapping
// graphics, including text

/*******************************************************************************
Function Name : Clear_Screen
Description : clear screen
 *******************************************************************************/
void    Clear_Screen(u16 color)
{
	u16 x, y;

	__Fill_Rectangle(0, 0, confp->display_width, confp->display_height, color);
}

/***************************************************************************
Function Name : Draw_Dot
Description : draw a dot
 *****************************************************************************/
void Draw_Dot(u16 x, u16 y, u16 color)
{
	__Set_Color (x,y,color);
	__Set_Color (x,y+1,color);
	__Set_Color (x,y-1,color);
	__Set_Color (x+1,y,color);
	__Set_Color (x-1,y,color);
}
/*******************************************************************************
Function Name : Draw_SEG
Description : draw a vertical segment
Para : x is the horizontal coordinate, |y1-y2| is the segment heigth, Color
*******************************************************************************/
void Draw_Seg(u16 x, u16 y1, u16 y2, u16 Color)
{
	u16  j;

	if (y1 > y2)
	{
		for (j = y2; j <= y1; j++) {
			__Set_Color (x, j, Color);
		}
	}
	else {
		for (j = y1; j <= y2; j++) {
			__Set_Color (x, j, Color);
		}
	}
}
/*******************************************************************************
Function Name : Display_Char
Description : print one decimal integer in the specific  position in a field
 of INT_WIDTH characters, right justified, with sign.
Para :
	x0, y0: lower left corner
	color: color of rectangle
	num: number to be displayed, right justified in fixed field
*******************************************************************************/
void Display_Char(u16 x0, u16 y0, u16 fg, u16 bg, char achar)
{
	char buf[2];
	buf[0] = achar;
	buf[1] = 0;
	__Display_Str(x0, y0, fg, bg, buf);
	return;
}

/*******************************************************************************
Function Name : Display_u8
Description : print one decimal integer in the specific  position in a field
 of INT_WIDTH characters, right justified, with sign.
Para :
	x0, y0: lower left corner
	color: color of rectangle

	num: number to be displayed, right justified in fixed field
*******************************************************************************/

void Display_U8(u16 x0, u16 y0, u16 fg, u16 bg, u8 num)
{
	char  buf[U8_DIGITS + 1];

	Unsigned_To_Dec_Str(buf, U8_DIGITS, num);
	__Display_Str(x0, y0, fg, bg, buf);
}


/*******************************************************************************
Function Name : Display_s8
Description : print one decimal integer in the specific  position in a field
 of INT_WIDTH characters, right justified, with sign.
Para :
	x0, y0: lower left corner
	color: color of rectangle
	num: number to be displayed, right justified in fixed field
*******************************************************************************/


void Display_S8(u16 x0, u16 y0, u16 fg, u16 bg, s8 num)
{
	char  buf[S8_DIGITS + 1];

	Signed_To_Dec_Str(buf, S8_DIGITS, num);
	__Display_Str(x0, y0, fg, bg, buf);
}
/*******************************************************************************
Function Name : Display_u16
Description : print one decimal integer in the specific  position in a field
 of INT_WIDTH characters, right justified, with sign.
Para :
	x0, y0: lower left corner
	color: color of rectangle
	num: number to be displayed, right justified in fixed field
*******************************************************************************/
void Display_U16(u16 x0, u16 y0, u16 fg, u16 bg, u16 num)
{
	char  buf[S16_DIGITS + 1];

	Unsigned_To_Dec_Str(buf, S16_DIGITS, num);
	__Display_Str(x0, y0, fg, bg, buf);
}
/*******************************************************************************
Function Name : Display_s16
Description : print one decimal integer in the specific  position in a field
 of INT_WIDTH characters, right justified, with sign.
Para :
	x0, y0: lower left corner
	color: color of rectangle
	mode: PRN = replace color, ERASE = remove color but leave other colors,
		 ADD = add color to existing colors
	num: number to be displayed, right justified in fixed field
*******************************************************************************/
void Display_S16(u16 x0, u16 y0, u16 fg, u16 bg, s16 num)
{

	char  buf[S16_DIGITS + 1];

	Signed_To_Dec_Str(buf, S16_DIGITS, num);
	__Display_Str(x0, y0, fg, bg, buf);
}
/*******************************************************************************
Function Name : Display_U32
Description : print one decimal integer in the specific  position in a field
 of INT_WIDTH characters, right justified, with sign.
Para :
	x0, y0: lower left corner
	color: color of rectangle
	mode: PRN = replace color, ERASE = remove color but leave other colors,
		 ADD = add color to existing colors
	num: number to be displayed, right justified in fixed field
*******************************************************************************/
void Display_U32(u16 x0, u16 y0, u16 fg, u16 bg, u32 num)
{
	char  buf[S32_DIGITS + 1];
	char i, n;

	Unsigned_To_Dec_Str(buf, S32_DIGITS, num);
	__Display_Str(x0, y0, fg, bg, buf);
}
/*******************************************************************************
Function Name : Display_S32
Description : print one decimal integer in the specific  position in a field
 of INT_WIDTH characters, right justified, with sign.
Para :
	x0, y0: lower left corner
	color: color of rectangle
	mode: PRN = replace color, ERASE = remove color but leave other colors,
		 ADD = add color to existing colors
	num: number to be displayed, right justified in fixed field
*******************************************************************************/
void Display_S32(u16 x0, u16 y0, u16 fg, u16 bg, int num)
{
	char  buf[S32_DIGITS + 1];

	Signed_To_Dec_Str(buf, S32_DIGITS, num);
	__Display_Str(x0, y0, fg, bg, buf);
}
/*******************************************************************************
Function Name : Display_Fixed
Description : print one fixed point integer in the specific  position in a field
 of INT_WIDTH characters, right justified, with sign.
Para :
	x0, y0: lower left corner
	color: color of rectangle
	mode: PRN = replace color, ERASE = remove color but leave other colors,
		 ADD = add color to existing colors
	num: number to be displayed, right justified in fixed field
    format fixed at xx,xxx.xxx
*******************************************************************************/
void Display_Fixed(u16 x0, u16 y0, u16 fg, u16 bg, fixed num)
{

	char  buf[FIXED_DIGITS + 1];

	Fixed_To_Dec_Str(buf, FIXED_DIGITS, num);
	__Display_Str(x0, y0, fg, bg, buf);
}
/*******************************************************************************
Function Name : Display_bool
Description : print TRUE or FALSE
Para :
	x0, y0: lower left corner
	color: color of rectangle
	mode: PRN = replace color, ERASE = remove color but leave other colors,
		 ADD = add color to existing colors
	val: boolean to be displayed as TRUE or FALSE
*******************************************************************************/
void Display_Bool(u16 x0, u16 y0, u16 fg, u16 bg, bool val)
{
	char buf[10];

	Bool_To_Str(buf, BOOL_CHARS, val);
	__Display_Str(x0, y0, fg, bg, buf);
}

/*******************************************************************************
Function Name : Display_Info
Description : Prints text and value
Para :
	Pre: prefix text
	Num: long number to be printed
*******************************************************************************/
static s16 msgy = 240-16;
static u16 ln = 0;
void Display_Info(char *Pre, u32 Num)
{
	char  buf[HEX_DIGITS + 1], n;
	bool neg = FALSE;
	u16 x0 = 0;

	if(confp->debug_level < 1)
		return;
	__Fill_Rectangle(0, msgy, confp->display_width, 16, BKGND_COLOR);
	Display_U16(x0, msgy, TXT_COLOR, BKGND_COLOR, ln++);
	if (Pre) {
		char *p = Pre;
		while (*p) p++;
		x0 += 64;
		__Display_Str(x0, msgy, TXT_COLOR, BKGND_COLOR, (unsigned char const *)Pre);
		x0 += (p - Pre) * 8 + 8;
	}
	Unsigned_To_Hex_Str(buf, HEX_DIGITS, Num);
	buf[HEX_DIGITS] = 0;
	__Display_Str(x0, msgy, TXT_COLOR, BKGND_COLOR, buf);
	msgy -= 16;
	if(msgy < 0) msgy = 240 - 16;
}

/*******************************************************************************
Function Name : Set_Orientation(orient)
Description : Prints text and value
Para :
	orientations orientation
*******************************************************************************/
void Set_Orientation(orientations orient)
{
	confp->orient = orient;
	__Set_Orientation(confp->orient);
	if(confp->orient == VERT){
		confp->display_width = VERT_WIDTH;
		confp->display_height = VERT_HEIGHT;
	} else {
		confp->display_width = HORZ_WIDTH;
		confp->display_height = HORZ_HEIGHT;
	}
	confp->graph_width = confp->display_width;
	confp->graph_height = confp->display_height - (2 * CHAR_HEIGHT);
}
