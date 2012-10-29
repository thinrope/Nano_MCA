// FIXME: improve signal peak height measurement
// FIXME: add support for sight imparred


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



/*******************************************************************************
File Name: main.c
********************************************************************************/
#include "stm32f10x_type.h"
#include "main.h"
#include "Function.h"
#include "Menu.h"
#include "Lcd.h"
#include "stm32f10x_lib.h"
#include "HW_V1_Config.h"
#include "Files.h"
#include "string.h"


// external variables
extern volatile s16 ctr_refresh;	// display refresh in milliseconds
extern volatile s16 ctr_delay;		// delay in milliseconds
extern volatile s16 ctr_key;		// process keys in milliseconds
extern const char *SD_Msgs[];

// Pictures are stored as a row of 8 verticle pixels, repeated with next lower row
//extern void __Display_Picture(const uchar *Pictp, ushort color, ushort x0, ushort y0, ushort dx, ushort dy);

// Nuclear Trefoil
const static u8 Small_Trefoil[] = {
	0x00, 0x00, 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xF0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xE0, 0xF8, 0xF8, 0xF0, 0xF0, 0xE0, 0xC0, 0x80, 0x00, 0x00, 0x00,
	0xE0, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3C, 0x10, 0x80, 0xC0, 0xC0, 0xC0, 0x88, 0x1E, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFC, 0xF0,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xC0, 0xF1, 0xF3, 0xE3, 0xF3, 0xF1, 0xE0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x38, 0x7C, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0x3E, 0x1C, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
#define SMALL_TREFOIL_WIDTH 32
#define SMALL_TREFOIL_HEIGHT 32




// Global variables
u32 live_time = 0;				// sampling time in mS
bool stop = FALSE;			// stop / start sampling
scan_states scan_state = LEADING;
u32 spectrum[MAX_CHANNELS];	// pha data in counts
u16 rate[MAX_RATE_SLOTS];		// rate in cpm
u32 live_rate = 0;				// in cpm
configurations configuration;		// run time configuration
configurations *confp = &configuration;

// forward definitions
void Clear_All_Data(void);
void Do_Keys_PHA (keycodes key);
void Do_Keys_Rate (keycodes key);
void Do_Keys_Scope (keycodes key);
void Set_Orientation(orientations orient);
void Set_Polarity(sig_types polarity);

void Read_Config(void);
void Write_Config(void);
void Reset_Config(void);

/*******************************************************************************
 Function Name : main
 Description :
*******************************************************************************/
void   main(void){
	u16 i, x, y;
	u16 pict_y;

	Display_Info("main", 0);
	/*--------------initialize the hardware-----------*/
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0xC000);
	NVIC_Configuration();
	/*----------display APP version ----------*/
	// (0,0) lower left
	__Display_Str(0, 112, TXT_COLOR, BKGND_COLOR, __msg_app_ver__);
	__Display_Str(0, 96, TXT_COLOR, BKGND_COLOR, __msg_chans__);
	__Display_Str(0, 80, RED, BKGND_COLOR, __msg_free__);
	__Display_Str(0, 64, RED, BKGND_COLOR, __msg_warr__);
	/*--------initialization --------*/
	Read_Config();
	if(confp->initialized != INIT_FLAG){	// check for first time run
		Reset_Config();
	}
	// initialize based on saved parameters
	Set_Base(0);	// fastest sample speed
	Set_Range(confp->sig_range);
	Set_Orientation(confp->orient);
	Delay_MS(4000);
	Clear_Screen( BKGND_COLOR );
	confp->mode = confp->rtn_mode;
	switch(confp->mode){
		case PHA:
			Init_Spectrum();
			Display_Spectrum ();
			break;
		case RATE:
			Init_Rate();
			Display_Rate();
			break;
		case MENU:
			Display_Menu(confp->menu_index);
			break;
	}
	if (SD_Card_On() == 0){
		if (FAT_Info()){
			if (FAT_Info()){
				__Display_Str(0, 0, RED, BKGND_COLOR, SD_Msgs[SDErr]);
			}
		}
	}
	Beep(BEEP_500Hz, 500);
	/*--------application main loop --------*/
	ctr_key = KEY_SAMPLE;
	ctr_refresh = confp->refresh_time * 1000;	// in mS
	Scan_Samples();	// never returns from this call
}

/*******************************************************************************
 Function Name : Clear_All
 Description : clear all data
 *******************************************************************************/
void Clear_All_Data(void) {
	Display_Info("Clear_All_Data", 0);
	stop = FALSE;				// stop / start sampling
	Acq_Clear_All();
	Disp_Clear_All();
	ADC_Start();
}

/*******************************************************************************
 Function Name : void Do_Keys_PHA(void)
 Description : adjusts cursor, zoom, etc.  Cursor moves within the window.  When
	 it reaches end of window then shift the window.
*******************************************************************************/

#define KEY_PERIOD 200		// spacing of key processing, controls auto repeat

void Do_Keys_PHA (keycodes key){
	s16 tmp, step, channels;
	s16 cursor_hi, cursor_lo, window_hi, window_lo, zoom;

	Display_Info("Do_Keys_PHA", 0);
	channels = confp->pha_channels;
	cursor_hi = confp->pha_cursor_hi;
	cursor_lo = confp->pha_cursor_lo;
	window_hi = confp->pha_window_hi;
	window_lo = confp->pha_window_lo;
	zoom = confp->pha_zoom;
	if(zoom >= 8)
		step = 1;
	else
		step = (window_hi - window_lo) / 64;
	// Handle keys here as mode dependant
	switch (key){
		case KEYCODE_PLAY:
			stop = !stop;
			if(stop)
				ADC_Stop();
			else
				ADC_Start();
			break;
		case KEYCODE_M:
			confp->rtn_mode = confp->mode;
			confp->mode = MENU;
			Display_Menu(confp->menu_index);
			break;
		case KEYCODE_UP:		// zoom in on cursor by factor of 2
			if(confp->pha_zoom >= (MAX_CHANNELS / confp->graph_width))
				break;		// limit zoom in
			zoom *= 2;		// shrink window by 2
			tmp = channels / zoom;
			window_hi = cursor_hi + (tmp / 2);
			if(window_hi > channels)
				window_hi = channels - 1;
			window_lo = cursor_lo - (tmp / 2);
			if(window_lo < 0)
				window_lo = 0;
			Display_Spectrum();
			break;
		case KEYCODE_DOWN:		// zoom out by factor of 2
			if(confp->pha_zoom <= 1)
				break;		// limit zoom in
			zoom /= 2;		// shrink window by 2
			if(zoom <= 1){	// compensate for division truncation
				window_hi = confp->pha_channels;
				window_lo = 0;
				break;
			}
			tmp = channels / zoom;
			window_hi = cursor_hi + (tmp / 2);
			if(window_hi > channels)
				window_hi = channels - 1;
			window_lo = cursor_lo - (tmp / 2);
			if(window_lo < 0)
				window_lo = 0;
			Display_Spectrum();
			break;
		case KEYCODE_RIGHT:
			tmp = cursor_hi - cursor_lo;
			cursor_hi += step;
			if(cursor_hi >= channels){		// stop on far right side
				cursor_hi = channels - 1;
			}
			cursor_lo = cursor_hi - tmp;
			tmp = window_hi - window_lo;
			if(cursor_hi > window_hi){		// adjust window when cursor hits end
				window_hi = cursor_hi;
				window_lo = window_hi - tmp;
			}
			Display_Spectrum();
			break;
		case KEYCODE_LEFT:
			tmp = cursor_hi - cursor_lo;
			cursor_lo -= step;
			if(cursor_lo < 0){		// stop on far right side
				cursor_lo = 0;
			}
			cursor_hi = cursor_lo + tmp;
			tmp = window_hi - window_lo;
			if(cursor_lo < window_lo){		// adjust window when cursor hits end
				window_lo = cursor_lo;
				window_hi = window_lo + tmp;
			}
			Display_Spectrum ();
			break;
	}
	confp->pha_cursor_hi = cursor_hi;
	confp->pha_cursor_lo = cursor_lo;
	confp->pha_window_hi = window_hi;
	confp->pha_window_lo = window_lo;
	confp->pha_zoom = zoom;
}

/*******************************************************************************
 Function Name : void Do_Keys_Rate(void)
 Description : adjusts cursor
*******************************************************************************/

#define KEY_PERIOD 200		// spacing of key processing, controls auto repeat

void Do_Keys_Rate (keycodes key){
	s16 tmp, step;
	s16 cursor;

	Display_Info("Do_Keys_Rate", 0);
	cursor = confp->rate_cursor;
	switch (key){
		case KEYCODE_PLAY:
			stop = !stop;
			if(stop)
				ADC_Stop();
			else
				ADC_Start();
			break;
		case KEYCODE_M:
			confp->rtn_mode = confp->mode;
			confp->mode = MENU;
			Display_Menu(confp->menu_index);
			break;
		case KEYCODE_UP:		// no zoom in rate mode
			break;
		case KEYCODE_DOWN:		// no zoom in rate mode
			break;
		case KEYCODE_RIGHT:
			step = 2;
			cursor += step;
			if(cursor >= confp->graph_width){		// stop on far right side
				cursor = confp->graph_width - 1;
			}
			Display_Rate();
			break;
		case KEYCODE_LEFT:
			step = 2;
			cursor -= step;
			if(cursor < 0){		// stop on far right side
				cursor = 0;
			}
			Display_Rate();
			break;
	}
	confp->rate_cursor = cursor;
}

