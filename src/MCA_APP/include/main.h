/*******************************************************************************
 File name  : main.h
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
#include "Lcd.h"
#include "Fixed.h"

#ifndef __main_h
#define __main_h



//#define TRACE(m) __Display_Str(0, 0, TXT_COLOR, BKGND_COLOR, ADD, "                         ");\
__Display_Str(0, 0, TXT_COLOR, BKGND_COLOR, ADD, m)

#define TRACE(m)

#define LANGUAGE EN


#define FRM_COLOR   	GRAY
#define BKGND_COLOR 	BLACK
#define CURSOR_COLOR	RED
#define TXT_COLOR 		WHITE
#define EMER_TXT_COLOR 	RED
#define SPECT_COLOR 	WHITE
#define PEAK_COLOR 		GRN

#define DISPLAY_REFRESH 500		// refesh interval in mS
#define KEY_REFRESH 80		// refesh interval in mS

#define KEY_SAMPLE 10		// periodic search for a pressed key, in mS
#define KEY_RELEASE 400 	// allow time for key to be released
#define KEY_REPEAT 200

// NOTE: map ADC 1:1 with pulse height index after allowing for 0v to be midrange of ADC
#define MAX_CHANNELS	2048	// highest number of channels supported
#define MAX_RATE_SLOTS	320		// plot
//#define MAX_SCOPE_SLOTS 100


// operational modes
typedef enum {
	PHA, 		// classic pulse height analysis spectrum
	RATE, 		// rate meter
	MENU		// modify configuration structure
} modes;

typedef enum {
	RESTART,	// start new sampling for SCOPE mode
	LEADING,	// find leading edge of a pulse
	PEAK, 		// find peak
	TAIL		// find tail of pulse
} scan_states;

// pulse polarity
typedef enum {
	PULSE_POS = 0,
	PULSE_NEG = 1
} sig_types;

// display type
typedef enum {
	DOT, 		// small dots for graphs
	LINE, 		// continuous lines for graphs
	FATLINE		// 3 pixel wide lines for better visibility in sun
} graphs;

// display orientation
typedef enum {
	HORZ = 0, 		// landscape
	VERT = 1		// portrait
} orientations;

typedef enum {
	EN,			// English
	FR,			// French
	SP,			// Spanish
	JP,			// Japanese
	RU			// Russian
} languages;


#define INIT_FLAG 0x120915	// used to determine if configuration struct up to date

// NOTE: all new settings must go on the end.  This is necessary so previously
// saved settings will still work with new software.
typedef struct{
	// Configuration parameters saved in flash
	// general purpose
	u32 initialized;		// set to INIT_FLAG to indicate initialized
	u16 menu_index;
	modes mode;			// current mode
	modes rtn_mode;		// mode to return to from menu mode
	bool alarm_en;		// enable alarm
	// display
	s16 refresh_time;		// seconds
	graphs graph;
	orientations orient;
	orientations rtn_orient;
	languages language;
	s16 display_width;		// dimensions of LCD in pixels
	s16 display_height;
	s16 graph_width;		// dimensions of graph area in pixels
	s16 graph_height;
	// probe signal processing
	s16 sig_input_range;
	sig_types sig_type;
	bool sig_filter;
	fixed sig_filter_coef;
	bool sig_gaussian;
	fixed sig_dvdt_lim;
	fixed sig_lo_lim;		// peak range
	fixed sig_hi_lim;		// peak range
	u16 sig_zero;
	fixed sig_range;			// in volts
	// pha mode
	u16 pha_file_seq;
	bool pha_energy_comp;
	s16 pha_cursor_width;
	s16 pha_channels;
	s16 pha_window_hi;		// in channels
	s16 pha_window_lo;		// in channels
	u8 pha_zoom;
	s16 pha_cursor_hi;		// in channels
	s16 pha_cursor_lo;		// in channels
	bool pha_cal;
	fixed pha_cal_a;
	fixed pha_cal_b;
	fixed pha_cal_c;
	// rate mode
	u32 rate_alarm;			// alarm level in cpm
	bool rate_filter;
	s16 rate_filter_coef;
	bool rate_beep;
	s16 rate_cursor;
	// Administration
	s16 debug_level;
} configurations;


#endif
/******************************** END OF FILE *********************************/
