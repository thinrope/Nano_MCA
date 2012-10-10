/*******************************************************************************
 File name  : Menu.h
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

#ifndef __Menu_h
#define __Menu_h

typedef enum {
	COMMENT,
	ACT_EXIT,
	ACT_RECAL,
	ACT_CLR_SPECT,
	ACT_FLTR_SPECT,
	ACT_SAV_CSV,
	ACT_SAV_N42,
	ACT_RESET_PARAM,
	BOOL_TYPE,
	ENUM_TYPE,
	CHAR_TYPE,
	U8_TYPE,
	S8_TYPE,
	U16_TYPE,
	S16_TYPE,
	S32_TYPE,
	U32_TYPE,
	FLOAT_TYPE,
	STR_TYPE,
	FIXED_TYPE
} menuvar;


typedef struct{
	char *txt;
	menuvar type;
	void *valp;
	s16 increment;
	s32 min_val;
	s32 max_val;
} menus;


extern menus Menu;


#endif
/******************************** END OF FILE *********************************/
