/*******************************************************************************
File Name: Function.c
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

#include "stm32f10x_type.h"
#include "main.h"
#include "Function.h"
#include "Menu.h"
#include "Lcd.h"
#include "stm32f10x_lib.h"
#include "HW_V1_Config.h"
#include "string.h"
#include "Files.h"

extern u16	pulse_height[];
extern u16 	background[];

#define LANGUAGE EN
#if LANGUAGE == EN
const static char *msgt = "TRUE ";
const static char *msgf = "FALSE";
#endif
const char *hex_conv = "0123456789ABCDEF";
//'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F''}

/*******************************************************************************
 Function Name : Ring_Buf_Empty
 Description : Returns TRUE if buffer is empty
*******************************************************************************/
bool Ring_Buf_Empty(RingBuf *rbp)
{
	// empty when out points to empty in position
	return (rbp->In == rbp->Out);
}

/*******************************************************************************
 Function Name : Ring_Buf_Get
 Description : Get int from ring buffer, return FALSE if buffer is empty
*******************************************************************************/
bool Ring_Buf_Get(RingBuf *rbp, s16 *valp)
{
	if (rbp->In == rbp->Out) return FALSE;
	else {
		*valp = rbp->Buffer[rbp->Out++];
		if(rbp->Out >= BUFFER_SIZE) rbp->Out = 0;
		return TRUE;
	}
}


/*******************************************************************************
 Function Name : Ring_Buf_Full
 Description : Returns TRUE if buffer is full
*******************************************************************************/
bool Ring_Buf_Full(RingBuf *rbp)
{
	u16 index;
	index = rbp->In;
	index++;
	if(index >= BUFFER_SIZE) index = 0;
	// return TRUE if another puting another value would overflow
	return(index == rbp->Out);
}

/*******************************************************************************
 Function Name : Ring_Buf_Put
 Description : Insert u16 into ring buffer, return FALSE if failed
*******************************************************************************/
bool Ring_Buf_Put(RingBuf *rbp, s16 val)
{
	u16 index;
	// in points to next empty location
	index = rbp->In;
	index++;
	if(index >= BUFFER_SIZE) index = 0;
	// return FALSE if another putting another value would overflow
	if(index == rbp->Out)
		return FALSE;
	else {
		rbp->Buffer[rbp->In] = val;
		rbp->In = index;
		return TRUE;
	}
}

/*******************************************************************************
Function Name : Unsigned_To_Dec_Str, Signed_To_Dec_Str
Description : print one decimal integer in the specific  position in a field
 of INT_WIDTH characters, right justified, with sign.
Para :
	buf: pointer to string buffer
	width:  number of characters to be displayed, not including null terminator
	num: number to be displayed, cast to u32 or s32
*******************************************************************************/

void Unsigned_To_Hex_Str(char *buf, u8 width, u32 num){
	char i, n;

	for(n = 0; n < width; n++)
		buf[n] = ' ';
	n = width - 1;  // start index at far right of field
	i = 0;
	do {  // isolate each decimal digit
		buf[n--] = hex_conv[(num & 0xF)];  // convert to text
		num >>= 4;
	} while (num > 0  && n >= 0);
}

void Unsigned_To_Dec_Str(char *buf, u8 width, u32 num){
	char i, n;

	for(n = 0; n < width; n++)
		buf[n] = ' ';
	n = width - 1;  // start index at far right of field
	i = 0;
	do {  // isolate each decimal digit
		buf[n--] = hex_conv[(num % 10)];  // convert to text
		num /= 10;
	} while (num > 0  && n >= 0);
}

void Signed_To_Dec_Str(char *buf, u8 width, s32 num){
	char i, n;
	bool neg_flg;

	for(n = 0; n < width; n++)
		buf[n] = ' ';
	if(num < 0){
		neg_flg = TRUE;
		num = -num;
	} else
		neg_flg = FALSE;
	buf[n] = 0;
	n = width - 1;  // start index at far right of field
	i = 0;
	do {  // isolate each decimal digit
		buf[n--] = hex_conv[(num % 10)];  // convert to text
		num /= 10;
	} while (num > 0  && n >= 1);
	if(neg_flg)
		buf[n] = '-';
}

void Fixed_To_Dec_Str(char *buf, u8 width, fixed num){
	char *bp;
	u8 bcd[FIXED_BCD], *bcdp;
	s16 i, n;
	bool isneg = FALSE;
	s16 tmp;
	u32 unum;

	// take care of a negative number
	if(num < 0){
		isneg = TRUE;
		unum = 0 - num;
	} else {
		unum = num;
	}
	// convert to bcd
	bcdp = &bcd[FIXED_BCD - 1];
	for(i = 0; i < FIXED_BCD; i++){  // isolate each decimal digit
		*bcdp-- = (unum % 10);  // convert to bcd
		unum /= 10;
	}
	// at this point number is in bcd format for editing
	// now display number
	bp = &buf[0];
	for (i = 0; i < FIXED_DIGITS; i++){
		*bp++ = ' ';
	}
	*bp = '\0';
	bcdp = &bcd[0];				// eliminate leading zeros
	for(n = 0; *bcdp == 0; n++, bcdp++);
	bcdp = &bcd[FIXED_BCD - 1];	// do 3 digit fraction
	bp--;
	for(i = 0; i < 3; i++){
		*bp-- = *bcdp-- + '0';
	}
	*bp-- = '.';
	// do integer
	for(i = n; i < 5; i++){
		*bp-- = *bcdp-- + '0';
	}
	if(isneg) *bp = '-';
}

inline void Bool_To_Str(char *buf, u8 width, bool val){
	if(val == TRUE){
		strcpy(buf, msgt);
	} else {
		strcpy(buf, msgf);
	}
}

/*******************************************************************************
Function Name : Time_To_Dec_Str
Description : Convert time in milliseconds to HH:MM:SS
Para :
	buf: pointer to string buffer, must be at least 12 characters in length
	time: u32
*******************************************************************************/
#define day_div 	(24 * 60 * 60)	// days
#define hour_div 	(60 * 60)		// hours
#define minute_div	(60)			// minutes

void Time_To_Dec_Str(char *buf, u32 time){
	u16 days, hours, minutes, seconds;

	time /= 1000;	// convert to seconds
	days = time / day_div;
	time = time % day_div;
	hours = time / hour_div;
	time = time % hour_div;
	minutes = time / minute_div;
	seconds = time % minute_div;
	*buf++ = (days / 10) + '0';
	*buf++ = (days % 10) + '0';
	*buf++ = ' ';
	*buf++ = (hours / 10) + '0';
	*buf++ = (hours % 10) + '0';
	*buf++ = ':';
	*buf++ = (minutes / 10) + '0';
	*buf++ = (minutes % 10) + '0';
	*buf++ = ':';
	*buf++ = (seconds / 10) + '0';
	*buf++ = (seconds % 10) + '0';
	*buf = NULL;
}

/******************************** END OF FILE *********************************/
