/*******************************************************************************
 File name  : Function.h
 *******************************************************************************/
/***************************************************************************
 *   Copyright (C) 2012 by LeDoyle R. Pingel, safelyiniowa@yahoo.com       *
 *                                                                         *
 *   This file is part of Gamma Grapher V2                                 *
 *                                                                         *
 *   Gamma Grapher is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   OpenGamma is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with OpenGamma. If not, see <http://www.gnu.org/licenses/>.     *
 ***************************************************************************/



#ifndef __Function_h
#define __Function_h

#define BUFFER_SIZE 512

typedef struct {
	s16 Buffer[BUFFER_SIZE];
	u16 In;	// next available input
	u16 Out;	// next available output
} RingBuf;

bool Ring_Buf_Empty(RingBuf *rbp);
bool Ring_Buf_Full(RingBuf *rbp);
bool Ring_Buf_Put(RingBuf *rbp, s16 val);
bool Ring_Buf_Get(RingBuf *rbp, s16 *valp);

/* can not use printf library as it causes ram overflow */
void Unsigned_To_Str(char *buf, u8 width, u32 num);
void Signed_To_Str(char *buf, u8 width, s32 num);
void Bool_To_Str(char *buf, u8 width, bool val);
void Fixed_To_Str(char *buf, u8 width, fixed num);

#endif
/******************************** END OF FILE *********************************/
