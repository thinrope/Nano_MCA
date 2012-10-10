/*******************************************************************************
File Name: Menu.c
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
#include "Fixed.h"
#include "main.h"
#include "Function.h"
#include "Menu.h"
#include "Lcd.h"
#include "stm32f10x_lib.h"
#include "stm32f10x_flash.h"
#include "HW_V1_Config.h"
#include "string.h"
#include "Files.h"

//enum {WriteErr, NoFile, SDErr, NoCard, SaveOk, Failed, ReadErr} SD_Enums;
//uchar   FileNum[4] = "000";
//I32STR_RES      Num;
extern configurations configuration;				// run time configuration
extern u16 config_flash[];
static s16 menu_y;

// Configuration parameters saved in flash
void __Flash_Read(u16 *hwordp, u32 hword_adr, u16 sz);
void __Flash_Write(u16 *hwordp, u32 hword_adr, u16 sz);
void Read_Config(void);
void Write_Config(void);
void Reset_Config(void);
void Set_Orientation(orientations orient);
void Set_Polarity(sig_types polarity);

#define MENU_LINES (sizeof(menu) / sizeof (menus))
#define MENU_CURSOR_X		0
#define MENU_LABEL_X  		4
#define MENU_END_VAR		(25 * CHAR_WIDTH)

#if LANGUAGE == EN

const static char *blank_line = "                              ";

const static menus menu[] = {
	//012345678901234567890123456789
	{"FREQUENTLY USED:              ", COMMENT, 0, 0, 0, 0},
	{" Mode: PHA, Rate              ", ENUM_TYPE, &configuration.rtn_mode, 1, PHA, RATE},
	{" File Seq Start:              ", U16_TYPE, &configuration.pha_file_seq, 1, 0, 0xffff},
	//{" Debug Level:                 ", S16_TYPE, &configuration.debug_level, 1, 0, 100},
	{" Save Spectrum To CSV File    ", ACT_SAV_CSV, 0, 0, 0, 0},
	{" *Save Spectrum To N42 File   ", ACT_SAV_N42, 0, 0, 0, 0},
	{" Clear All                    ", ACT_CLR_SPECT, 0, 0, 0, 0},
	{" Alarm Enable:                ", BOOL_TYPE, &configuration.alarm_en, 0, FALSE, TRUE},
	{" Alarm Level:             cpm ", U32_TYPE, &configuration.rate_alarm, 500, 1, 1000000},
	{" Beep:                        ", BOOL_TYPE, &configuration.rate_beep, 0, FALSE, TRUE},
	{"PHA MODE SETUP:               ", COMMENT, 0, 0, 0, 0},
	{" Energy Comp:                 ", BOOL_TYPE, &configuration.pha_energy_comp, 0, FALSE, TRUE},
	{" *Cursor Width:           chn ", S16_TYPE, &configuration.pha_cursor_width, 1, 1, 2000},
	{" *Channels:                   ", S16_TYPE, &configuration.pha_channels, 1, 256, MAX_CHANNELS},
	{" *Calibration Form (ax^2+bx+c)", COMMENT, 0, 0, 0, 0},
	{" *Coef a:                     ", FIXED_TYPE, &configuration.pha_cal_a, 0, 0, 0x7FFFFFFF},
	{" *Coef b:                     ", FIXED_TYPE, &configuration.pha_cal_b, 0, 0, 0x7FFFFFFF},
	{" *Coef c:                     ", FIXED_TYPE, &configuration.pha_cal_c, 0, 0, 0x7FFFFFFF},
	{"RATE MODE SETUP:              ", COMMENT, 0, 0, 0, 0},
	{" Low Pass Filter:             ", BOOL_TYPE, &configuration.rate_filter, 0, FALSE, TRUE},
	{" Filter Coef:               S ", S16_TYPE, &configuration.rate_filter_coef, 1, 0, 30*60},
	{"INPUT SIGNAL SETUP:           ", COMMENT, 0, 0, 0, 0},
	{" Pulse: Pos,Neg               ", ENUM_TYPE, &configuration.sig_type, 1, PULSE_POS, PULSE_NEG},
	{" Gaussian Approx:             ", BOOL_TYPE, &configuration.sig_gaussian, 0, FALSE, TRUE},
	{" dv/dt Thres:                 ", FIXED_TYPE, &configuration.sig_dvdt_lim, fixed_from_int(10), fixed_from_int(-1000), fixed_from_int(1000)},
	{" Low Limit:               kev ", FIXED_TYPE, &configuration.sig_lo_lim, fixed_from_int(5), 0, fixed_from_int(2048)},
	{" High Limit:              kev ", FIXED_TYPE, &configuration.sig_hi_lim, fixed_from_int(5), 0, fixed_from_int(2048)},
	{" Range +/-:                 v ", FIXED_TYPE, &configuration.sig_range, FIXED_HALF, 0, fixed_from_int(6)},
	{" Recal ADC                    ", ACT_RECAL, 0, 0, 0, 0},
	{"DISPLAY SETUP:                ", COMMENT, 0, 0, 0, 0},
	{" Refresh Time:              S ", S16_TYPE, &configuration.refresh_time, 1, 1, 24 * 60 * 60},
	{" Plot: Dot,Line,Wide          ", ENUM_TYPE, &configuration.graph, 1, DOT, FATLINE},
	{" Orientation: Horz,Vert       ", ENUM_TYPE, &configuration.rtn_orient, 1, HORZ, VERT},
	{" Language: EN                 ", ENUM_TYPE, &configuration.language, 1, EN, RU},
	{"ADMINISTRATIVE:               ", COMMENT, 0, 0, 0, 0},
	{" Reset Settings to Default    ", ACT_RESET_PARAM, 0, 0, 0, 0},
	{" Debug Level:                 ", S16_TYPE, &configuration.debug_level, 1, 0, 100},
	{"* = not implemented yet       ", COMMENT, 0, 0, 0, 0},
};

unsigned const char *msg_succeed = "Success";
#endif




/*******************************************************************************
 Function Name : Display_Menu_Line
 Description : Outputs a single menu line with value
 Note: keep menu narrow enough to be displayed in both portrait and landscape
 *******************************************************************************/
void Display_Menu_Line(u16 y, u16 fg, u16 bg, const menus *msp) {
	char charb;
	u8 u8b;
	s8 s8b;
	u16 u16b;
	s16 s16b;
	u32 u32b;
	s32 s32b;
	fixed fixb;

	TRACE("Display_Menu");
	__Display_Str(MENU_LABEL_X, y, fg, bg, msp->txt);
	switch(msp->type){
		case COMMENT:
			break;
		case BOOL_TYPE:
			Display_Bool(MENU_END_VAR - BOOL_WIDTH, y, fg, bg, *((bool *)msp->valp));
			break;
		case ENUM_TYPE:
			// will never have > 256 values;
			Display_U8(MENU_END_VAR - U8_WIDTH, y, fg, bg, *((u8 *)msp->valp));
			break;
		case CHAR_TYPE:
			Display_Char(MENU_END_VAR - CHAR_WIDTH, y, fg, bg, *((char *)msp->valp));
			break;
		case U8_TYPE:
			Display_U8(MENU_END_VAR - U8_WIDTH, y, fg, bg, *((u8 *)msp->valp));
			break;
		// Signed char does not exist for this compiler
		//case S8_TYPE:
		//	Display_S8(MENU_END_VAR - S8_WIDTH, y, fg, bg, *((u8 *)msp->valp));
		//	break;
		case U16_TYPE:
			Display_U16(MENU_END_VAR - U16_WIDTH, y, fg, bg, *((u16 *)msp->valp));
			break;
		case S16_TYPE:
			Display_S16(MENU_END_VAR - S16_WIDTH, y, fg, bg, *((s16 *)msp->valp));
			break;
		case U32_TYPE:
			Display_U32(MENU_END_VAR - U32_WIDTH, y, fg, bg, *((u32 *)msp->valp));
			break;
		case S32_TYPE:
			Display_S32(MENU_END_VAR - S32_WIDTH, y, fg, bg, *((s32 *)msp->valp));
			break;
		case FIXED_TYPE:
			Display_Fixed(MENU_END_VAR - FIXED_WIDTH, y, fg, bg, *((fixed *)msp->valp));
			break;
		case FLOAT_TYPE:
			break;
		//case STR_TYPE:
 		//	__Display_Str(MENU_END_VAR -, y, fg, bg, (char *)msp->valp);
		//	break;
	}
}


/*******************************************************************************
 Function Name : Display_Menu
 Description :   The entire menu list scrolls under the center of screen.  The
 menu item is marked with ">".  Left and right buttons change the value.
 Para:
 	Index = currently selected menu line.
*******************************************************************************/
#define MENU_BRACKET 6 	// menu lines displayed above and below

void Display_Menu(s16 index) {
	s16 y;
	s16 n, m;
	configurations *cp = &configuration;

	Clear_Screen( BKGND_COLOR );
	// start at top
	y =  (cp->display_height / 2) + (MENU_BRACKET * CHAR_HEIGHT);
	n = index - MENU_BRACKET;
	for(m = ( MENU_BRACKET * 2 ) + 1; m > 0; m--){
		if(n >= 0 && n < MENU_LINES){
			if(n == index){
				Display_Menu_Line(y, BKGND_COLOR, TXT_COLOR, &menu[n]);
			} else {
				Display_Menu_Line(y, TXT_COLOR, BKGND_COLOR, &menu[n]);
			}
		}
		n++;
		y -= CHAR_HEIGHT;
	}
}


/*******************************************************************************
 Function Name : Menu_Reset_Config
 Description : Reset configuration and handles menu specific items.
 *******************************************************************************/
void Menu_Reset_Config(void){
	configurations *cp = &configuration;
	u16 menu_index;
	menu_index = cp->menu_index;
	Reset_Config();
	__Display_Str(22 * CHAR_WIDTH, cp->display_height / 2, BKGND_COLOR, TXT_COLOR, msg_succeed);
	//Delay_MS(2000);
	cp->mode = MENU;
	cp->menu_index = menu_index;
	Display_Menu(cp->menu_index);
}

/*******************************************************************************
 Function Name : Do_Menu
 Description : Display menu and implement actions and change values until
 user selects "exit".
 *******************************************************************************/
void Do_Keys_Menu(keycodes key){
	const menus *msp;
	static modes rtn_mode;
	configurations *cp = &configuration;
	u16 menu_index;

	TRACE("Do_Keys_Menu");
	msp = &menu[cp->menu_index];
	switch(key){
		case KEYCODE_M: // exit menu mode
			cp->orient = cp->rtn_orient;
			Write_Config ();
			Set_Orientation(cp->orient);
			cp->mode = cp->rtn_mode;
			Clear_Screen( BKGND_COLOR );
			Set_Range(cp->sig_range);
			switch(cp->mode){
			case PHA:
				Init_Spectrum();
				Display_Spectrum ();
				break;
			case RATE:
				Init_Rate();
				Display_Rate();
				break;
			}
			return;
		case KEYCODE_DOWN:
			// scroll cursor down, menu up
			if(cp->menu_index < (MENU_LINES - 1))
				cp->menu_index += 1;
			Display_Menu(cp->menu_index);
			msp = &menu[cp->menu_index];
			break;
		case KEYCODE_UP:
			// scroll cursor up, menu down
			if(cp->menu_index > 0)
				cp->menu_index -= 1;
			Display_Menu(cp->menu_index);
			msp = &menu[cp->menu_index];
			break;
		case KEYCODE_LEFT:
			// execute action or change variable
			switch(msp->type){
				case COMMENT:
					break;
				case ACT_SAV_CSV:
					Save_Spectrum_CSV();
					__Display_Str(22 * CHAR_WIDTH, cp->display_height / 2, BKGND_COLOR, TXT_COLOR, msg_succeed);
					break;
				case ACT_SAV_N42:
					Save_Spectrum_N42();
					__Display_Str(22 * CHAR_WIDTH, cp->display_height / 2, BKGND_COLOR, TXT_COLOR, msg_succeed);
					break;
				case ACT_RECAL:
					Cal_HW();
					__Display_Str(22 * CHAR_WIDTH, cp->display_height / 2, BKGND_COLOR, TXT_COLOR, msg_succeed);
					//Delay_MS(2000);
					break;
				case ACT_CLR_SPECT:
					Clear_All_Data();
					__Display_Str(22 * CHAR_WIDTH, cp->display_height / 2, BKGND_COLOR, TXT_COLOR, msg_succeed);
					//Delay_MS(2000);
					break;
				case ACT_RESET_PARAM:
					Menu_Reset_Config();
					break;
				case BOOL_TYPE:
					*((bool *)msp->valp) = FALSE;
					break;
				case ENUM_TYPE:
					if(*((u8 *)msp->valp) > msp->min_val){
						*((u8 *)msp->valp) -= msp->increment;
					}
					break;
				case CHAR_TYPE:
					if(*((char *)msp->valp) > msp->min_val){
						*((char *)msp->valp) -= msp->increment;
					}
					break;
				case U8_TYPE:
					if(*((u8 *)msp->valp) > msp->min_val){
						*((u8 *)msp->valp) -= msp->increment;
					}
					break;
				case S8_TYPE:
					if(*((s8 *)msp->valp) > msp->min_val){
						*((s8 *)msp->valp) -= msp->increment;
					}
					break;
				case U16_TYPE:
					if(*((u16 *)msp->valp) > msp->min_val){
						*((u16 *)msp->valp) -= msp->increment;
					}
					break;
				case S16_TYPE:
					if(*((s16 *)msp->valp) > msp->min_val){
						*((s16 *)msp->valp) -= msp->increment;
					}
					break;
				case U32_TYPE:
					if(*((u32 *)msp->valp) > msp->min_val){
						*((u32 *)msp->valp) -= msp->increment;
					}
					break;
				case S32_TYPE:
					if(*((s32 *)msp->valp) > msp->min_val){
						*((s32 *)msp->valp) -= msp->increment;
					}
					break;
				case FIXED_TYPE:
					if(*((s32 *)msp->valp) > msp->min_val){
						*((s32 *)msp->valp) -= msp->increment;
					}
					break;
				case FLOAT_TYPE:
					break;
				case STR_TYPE:
					break;
			}
			Display_Menu_Line(cp->display_height / 2, BKGND_COLOR, TXT_COLOR, msp);
			break;
		case KEYCODE_RIGHT:
			// execute action or change variable
			switch(msp->type){
				case COMMENT:
					break;
				case ACT_SAV_CSV:
					Save_Spectrum_CSV();
					__Display_Str(22 * CHAR_WIDTH, cp->display_height / 2, BKGND_COLOR, TXT_COLOR, msg_succeed);
					break;
				case ACT_SAV_N42:
					Save_Spectrum_N42();
					__Display_Str(22 * CHAR_WIDTH, cp->display_height / 2, BKGND_COLOR, TXT_COLOR, msg_succeed);
					break;
				case ACT_RECAL:
					Cal_HW();
					__Display_Str(22 * CHAR_WIDTH, cp->display_height / 2, BKGND_COLOR, TXT_COLOR, msg_succeed);
					//Delay_MS(2000);
					break;
				case ACT_CLR_SPECT:
					Clear_All_Data();
					__Display_Str(22 * CHAR_WIDTH, cp->display_height / 2, BKGND_COLOR, TXT_COLOR, msg_succeed);
					//Delay_MS(2000);
					break;
				case ACT_RESET_PARAM:
					Menu_Reset_Config();
					break;
				case BOOL_TYPE:
					*((bool *)msp->valp) = TRUE;
					break;
				case ENUM_TYPE:
					if(*((u8 *)msp->valp) < msp->max_val){
						*((u8 *)msp->valp) += 1;
					}
					break;
				case CHAR_TYPE:
					if(*((char *)msp->valp) < msp->max_val){
						*((char *)msp->valp) += msp->increment;
					}
					break;
				case U8_TYPE:
					if(*((u8 *)msp->valp) < msp->max_val){
						*((u8 *)msp->valp) += msp->increment;
					}
					break;
				case S8_TYPE:
					if( *((s8 *)msp->valp) < msp->max_val){
						*((s8 *)msp->valp) += msp->increment;
					}
					break;
				case U16_TYPE:
					if(*((u16 *)msp->valp) < msp->max_val){
						*((u16 *)msp->valp) += msp->increment;
					}
					break;
				case S16_TYPE:
					if(*((s16 *)msp->valp) < msp->max_val){
						*((s16 *)msp->valp) += msp->increment;
					}
					break;
				case U32_TYPE:
					if(*((u32 *)msp->valp) < msp->max_val){
						*((u32 *)msp->valp) += msp->increment;
					}
					break;
				case S32_TYPE:
					if( *((s32 *)msp->valp) < msp->max_val){
						*((s32 *)msp->valp) += msp->increment;
					}
					break;
				case FIXED_TYPE:
					if( *((s32 *)msp->valp) < msp->max_val){
						*((s32 *)msp->valp) += msp->increment;
					}
					break;
				case FLOAT_TYPE:
					break;
				case STR_TYPE:
					break;
			}
			Display_Menu_Line(cp->display_height / 2,BKGND_COLOR, TXT_COLOR, msp);
			break;
	}
}

/*******************************************************************************
Function Name : Display_Fixed
Description : Display one fixed point number in the specific  position in a field
 of INT_WIDTH characters, right justified, with sign.  Allow each digit to be changed.
Para :
	x0, y0: lower left corner
	color: color of rectangle
	mode: PRN = replace color, ERASE = remove color but leave other colors,
		 ADD = add color to existing colors
	num: number to be displayed, right justified in fixed field
    format fixed at xx,xxx.xxx
*******************************************************************************/
// FIXME: NOT FINISHED
/*
void Enter_Fixed(u16 x0, u16 y0, u16 fg, u16 bg, fixed num)
{
	const menus *msp;
	keycodes key;
	char  buf[FIXED_DIGITS + 1], *bp;
	u8 bcd[FIXED_BCD], *bcdp;
	s16 i, n;
	bool isneg = FALSE;
	s16 tmp;
	u32 unum;
	configurations *cp = &configuration;

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
	while(TRUE){
		key = Get_Key();
		switch(key){
			case KEYCODE_M:	// return to display mode
				return;
			case KEYCODE_UP:	// increase digit
				break;
			case KEYCODE_DOWN:	// decrease digit
				break;
			case KEYCODE_LEFT:	// move left 1 digit
				break;
			case KEYCODE_RIGHT:	// move right 1 digit
				break;
		}
	}
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
	__Display_Str(x0, y0, fg, bg, buf);
	return;
}
*/
/*******************************************************************************
 Function Name : Write_SD_Config
 Description : stores configuration to flash drive
*******************************************************************************/
/*
 void Write_SD_Config(void)
{
  F_Buff[0] = 0x30;
  F_Buff[1] = 0x00;
  memcpy(F_Buff + 2, &configuration, sizeof(config));
  Update[SAVE_PROFILE] = 0;
  if (Item_Index[SAVE_PROFILE]) {
    if (SD_Card_ON())
    {
        if (FAT_Info() == 0)
        {
           Char_to_Str(FileNum, Item_Index[SAVE_PROFILE]);
           if (Open_File("FILE",FileNum,"CFG") == 0)
           {
              PutConfig();
              if (Write_File() != 0)
                 DisplayField(InfoF, WHITE, SD_Msgs[WriteErr]);
           } else
              DisplayField(InfoF, WHITE, SD_Msgs[NoFile]);
        } else
           DisplayField(InfoF, WHITE, SD_Msgs[SDErr]);
    } else
       DisplayField(InfoF, WHITE, SD_Msgs[NoCard]);
  } else {
    PutConfig();
    if (Write_Parameter() == FLASH_COMPLETE)
       DisplayField(InfoF, WHITE, SD_Msgs[SaveOk]);
    else
       DisplayField(InfoF, WHITE, SD_Msgs[Failed]);
  }
}
*/
/*******************************************************************************
 Function Name : Read_SD_Config
 Description : reads configuration from flash card
*******************************************************************************/
/*
void Read_SD_Config(void)
{
	if ((F_Buff[0] == 0x30) && (F_Buff[1] == 0x00)) {
      memcpy(&configuration, F_Buff + 2, sizeof(config));
   } else {
	   Init_Config();
	   Save_Config();
   }
  if (Item_Index[LOAD_PROFILE]) {
    Update[LOAD_PROFILE] = 0;
    if (SD_Card_ON())
    {
      if (FAT_Info() == 0)
      {
         Char_to_Str(FileNum, Item_Index[LOAD_PROFILE]);
         if (Open_File("FILE",FileNum,"CFG") == 0)
         {
            if (Read_File() == 0)
              RestoreConfig();
            else
               DisplayField(InfoF, WHITE, SD_Msgs[ReadErr]);
         } else
            DisplayField(InfoF, WHITE, SD_Msgs[NoFile]);
      } else
         DisplayField(InfoF, WHITE, SD_Msgs[SDErr]);
    } else
       DisplayField(InfoF, WHITE, SD_Msgs[NoCard]);
  } else {
    Read_Parameter();
    RestoreConfig();
  }
}
*/

/*******************************************************************************
 Function Name : void Read_Config(void)
 Description : Read configuration from flash memory.  Use this to hide flash setup
 *******************************************************************************/
void Read_Config(void){
	TRACE("Read_Config");
	__Flash_Read((u16 *) &configuration, (u32) &config_flash[0], sizeof(configurations));
}
/*******************************************************************************
 Function Name : void Write_Config(void)
 Description : Writes configuration to flash memory.  Use this to hide flash setup
 *******************************************************************************/
void Write_Config(void){
	TRACE("Write_Config");

	__Flash_Write((u16 *) &configuration, (u32) &config_flash[0], sizeof(configurations));
}
/**********************************************************************
 Function Name : Reset_Config
 Description : restores config to default and stores configuration in flash memory
*******************************************************************************/

void Reset_Config(void)
{
	const menus *msp;
	configurations *cp = &configuration;

	TRACE("Reset_Config");

	// Configuration parameters saved in flash
	// general purpose
	cp->initialized = INIT_FLAG;		// set to INIT_FLAG to indicate initialized
	cp->menu_index = 0;
	cp->mode = PHA;			// current mode
	cp->rtn_mode = PHA;		// mode to return to from menu mode
	cp->alarm_en = FALSE;		// enable alarm
	// display
	cp->refresh_time = 3;		// seconds
	cp->graph = LINE;
	cp->orient = HORZ;
	cp->rtn_orient = HORZ;
	cp->language = EN;
	cp->display_width = HORZ_WIDTH;
	cp->display_height = HORZ_HEIGHT;
	cp->graph_width = cp->display_width;
	cp->graph_height = cp->display_height - (2 * CHAR_HEIGHT);
	// probe signal processing
	cp->sig_input_range = 5;
	cp->sig_type = PULSE_NEG;
	cp->sig_filter = FALSE;
	cp->sig_filter_coef = fixed_from_int(5);
	cp->sig_gaussian = TRUE;
	cp->sig_dvdt_lim = fixed_from_int(-400);
	cp->sig_lo_lim = fixed_from_int(1);
	cp->sig_hi_lim = fixed_from_int(2046);
	cp->sig_zero = 900;
	cp->sig_range = FIXED_HALF;
	// pha mode
	cp->pha_file_seq = 0;
	cp->pha_energy_comp = FALSE;
	cp->pha_cursor_width = 1;
	cp->pha_channels = 2048;
	cp->pha_window_hi = 2047;		// in channels
	cp->pha_window_lo = 1;		// in channels
	cp->pha_zoom = 1;
	cp->pha_cursor_hi = 1024;		// in channels
	cp->pha_cursor_lo = 1024;		// in channels
	cp->pha_cal = FALSE;
	fixed pha_cal_a = fixed_from_int(0);
	fixed pha_cal_b = fixed_from_int(0);
	fixed pha_cal_c = fixed_from_int(0);
	// rate mode
	cp->rate_alarm = 100000;
	cp->rate_filter = FALSE;
	cp->rate_filter_coef = 15;
	cp->rate_beep = TRUE;
	cp->rate_cursor = 150;
	// Administration
	cp->debug_level = 0;
	// now store the reset configuration
	__Flash_Write((u16 *) cp, (u32) &config_flash[0], sizeof(configurations));
}

