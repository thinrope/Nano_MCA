
// FIXME: add support for sight imparred
// FIXME: complete changing vertical scaling to fixed, starting with 0.1 scale

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



/* From wikipedia "low pass filters":
 Algorithmic implementation

The filter recurrence relation provides a way to determine the output samples
in terms of the input samples and the preceding output. The following pseudocode
algorithm will simulate the effect of a low-pass filter on a series of digital
samples:

 // Return RC low-pass filter output samples, given input samples,
 // time interval dt, and time constant RC
 function lowpass(real[0..n] x, real dt, real RC)
   var real[0..n] y
   var real α := dt / (RC + dt)
   α := refresh_time / (filter_time + refresh_time)
   y[0] := x[0]
   for i from 1 to n
       y[i] := α * x[i] + (1-α) * y[i-1]
   return y

The loop that calculates each of the n outputs can be refactored into the equivalent:

   for i from 1 to n
       y[i] = y[i-1] + α * (x[i] - y[i-1])

That is, the change from one filter output to the next is proportional to the
difference between the previous output and the next input. This
exponential smoothing property matches the exponential decay seen
in the continuous-time system. As expected, as the time constant RC
increases, the discrete-time smoothing parameter \alpha decreases,
and the output samples (y_1,y_2,\ldots,y_n) respond more slowly to a
change in the input samples (x_1,x_2,\ldots,x_n) – the system will
have more inertia. This filter is an infinite-impulse-response
IIR) single-pole lowpass filter.
 *
 */

/*
The pulse detection algorithm is based on using a smoothed derivative (slope) of the data. We use the derivative so that DC level of
the data doesn't affect the detection of pulses. The derivative is calculated by convolving the data with a boxcar derivative function.
That is, to find the derivative at time T, we multiply several data points on each side of point T by the corresponding point in an array
that looks like this: (-1,-1,-1,-1, 1, 1, 1, 1)
(The number of samples—the length of the "boxcar"—is an adjustable parameter.) The
smoothed derivative is the sum of those multiplications.
This convolution smoothes the data as well as finding the derivative.

From: http://universe.gsfc.nasa.gov/xrays/programs/astroe/eng/detection.html

Character set has these special characters:
	" = run
	# = pause
	$ = lower left corner
	& = lower right corner
	' = empty battery bottom
	* = raised x
	, = |
	[ =  triangle,
	\ = high dash
	] = jagged line
	^= up arrow
	_ = down arrow


Screen layout:
	Graphics is displayed as a square at top of screen.
	Various settings and readings are displayed under that.  Lower display
	is 30 x 5 characters.
*/


/*******************************************************************************
File Name: main.c
********************************************************************************/
#include "stm32f10x_type.h"
#include "Fixed.h"
#include "main.h"
#include "Function.h"
#include "Menu.h"
#include "Lcd.h"
#include "stm32f10x_lib.h"
#include "HW_V1_Config.h"
#include "Files.h"
#include "string.h"


//#define TRACE		// turn on debug code

void Display_Common(void);
void Display_Spectrum(void);
void Display_Rate(void);
void Plot_Graph(s16 x, s16 y, s16 y_old);
void Disp_Clear_All(void);
fixed Chan2Energy(s16 chan);
s16 Energy2Chan(fixed energy);

// external variables
extern u32 sys_time;		// real time clock in mS
extern u32 live_time;		// sampling time in mS
extern configurations configuration;	// Configuration parameters
extern volatile u8 pwr_lvl;
extern scan_states scan_state;
extern u32 pulse_height[MAX_CHANNELS];
extern u16 rate[320];			// rate data in cpm
extern u32 live_rate;
extern bool stop;

// Global variables, must be cleared by Clear_all()
const static char *Battery_Status[5] = {"~`'", "~`}", "~|}", "{|}", "USB"};
const static u16 Battery_Color[5] = {RED, YEL, GRN, GRN, GRN};
static fixed pha_vscale; 		// vertical scaling
static fixed rate_vscale;

#if LANGUAGE == EN

// VERTICAL SCALE SETTINGS
// 240x240 graph area above this text in vertical mode
// 320x208 graph area above this test in horizonatal mode
// (0,0) is always lower left corner
#define MSG_LINE_1	0
#define MSG_LINE_0	(CHAR_HEIGHT)
#define RATE_BAR_Y ((2 * CHAR_HEIGHT) - 1)
#define GRAPH_Y (RATE_BAR_Y + 4)

#define COM_X_PAUSE (23 * CHAR_WIDTH)
#define COM_X_BAT (26 * CHAR_WIDTH)

								  // 012345678901234567890123456789
const static char *msg_pha0 =    	"PHA          xxx:1     >  USB ";
const static char *msg_pha1_cal =   "xxxxxxxxxxxxx @xxxxxx.xxx keV ";
const static char *msg_pha1_nocal = "xxxxxxxxxxxxx @xxxxxx.xxx chan";
#define PHA_X_ZOOM (13 * CHAR_WIDTH)
#define PHA_X_CURS_CNT (0)
#define PHA_X_CURS_KEV (15 * CHAR_WIDTH)

								  // 012345678901234567890123456789
const static char *msg_rate0 =    	"Rate     xxxxxx cpm    >  USB ";
const static char *msg_rate1 =    	"@ Cursor xxxxxx cpm           ";
#define RATE_CPM (9 * CHAR_WIDTH)
#define RATE_CURSOR (9 * CHAR_WIDTH)



#endif


/*******************************************************************************
 Function Name : Clear_All
 Description : clear all data
 *******************************************************************************/
void Disp_Clear_All(void) {
	register configurations *cp = &configuration;

	//__Display_Str(0, 0, WHITE, "Clear All");
	bzero(&pulse_height[0], sizeof(pulse_height));
	bzero(&rate[0], sizeof(rate));
	//bzero(&scope[0], sizeof(scope));
	cp->pha_window_hi = cp->pha_channels - 1;
	cp->pha_window_lo = 0;
	cp->pha_cursor_hi = cp->pha_cursor_lo = cp->graph_width / 2;
	cp->pha_zoom = 1;
	pha_vscale = 1;
	rate_vscale = 1;
}

/*******************************************************************************
 Function Name : void CalCalib(void)
 Description :  determines curve coefficents from points in config struct
 *******************************************************************************/
void CalCalib(void)
{
	return;
}

/*******************************************************************************
 Function Name : u16 Chan2Energy(u16 cursor)
 Description :  returns energy associated with cursor position
 *******************************************************************************/
// FIXME: implement calibration here
fixed Chan2Energy(s16 chan)
{
	return fixed_from_int(chan);
}
/*******************************************************************************
 Function Name : u16 Energy2Chan(u16 cursor)
 Description : returns cursor position for given energy
 *******************************************************************************/
// FIXME: implement calibration here
s16 Energy2Chan(fixed energy)
{
	return(fixed_to_int(energy));
}

/*******************************************************************************
 Function Name : Display_Spectrum
 Description : Display the spectrum
 Spectrum is scrolled under a cursor fixed at the center.

 *******************************************************************************/

// Initialize the display for spectrum
void Init_Spectrum(void)
{
	register configurations *cp = &configuration;
	u16 y;

	Clear_Screen( BKGND_COLOR );
	__Display_Str(0, MSG_LINE_0, TXT_COLOR, BKGND_COLOR, msg_pha0);
	if(cp->pha_cal){
		__Display_Str(0, MSG_LINE_1, TXT_COLOR, BKGND_COLOR, msg_pha1_cal);
	}else{
		__Display_Str(0, MSG_LINE_1, TXT_COLOR, BKGND_COLOR, msg_pha1_nocal);
	}
}

void Display_Spectrum(void){
	u32 cnt, cnt_max, curs_cnt, y_max, *chanp;
	s16 x, x_nu, y, y_old;
	s16 chan, chan_lo, chan_hi, channels, window;
	s16 cursor_lo, cursor_hi;	// units are channels
	register configurations *cp = &configuration;

	// clear the graphical display
	__Fill_Rectangle(0, RATE_BAR_Y, cp->graph_width, cp->graph_height + 2, BKGND_COLOR);
	y_max = 0;
	cnt = curs_cnt = 0;
	x_nu = x = 0;
	y = y_old = 0;
	chan_lo = cp->pha_window_lo;
	chan_hi = cp->pha_window_hi;
	cursor_hi = cp->pha_cursor_hi;
	cursor_lo = cp->pha_cursor_lo;
	window = chan_hi - chan_lo;
	// display cursor
	x = ((cursor_lo - chan_lo) * cp->graph_width) / window;  		// map cursor to display column
	x_nu = ((cursor_hi - cursor_lo) * cp->graph_width) / window;	// map cursor width
	x_nu += 1; // do this as cursor_hi == cursor_lo is 1 pixel wide
	if(cp->graph == FATLINE){
		if(x <= 0)
			x = 1;
		__Fill_Rectangle((u16)(x - 1), (u16)GRAPH_Y, (u16)3, (u16)cp->graph_height, CURSOR_COLOR);
	} else {
		__Fill_Rectangle((u16)x, (u16)GRAPH_Y, 1, (u16)cp->graph_height, CURSOR_COLOR);
	}

	// display the spectrum
	x = ((chan - chan_lo) * cp->graph_width) / window;  // map channel to display column;
	y_old = 0;
	y_max = 0;
	cnt_max = 0;
	chanp = &pulse_height[chan_lo];
	for (chan = chan_lo; chan <= chan_hi; chanp++, chan++) {
		// map current channel onto display
		x_nu = ((chan - chan_lo) * cp->graph_width) / window;  // map channel to display column
		if(x_nu != x || chan == chan_hi){  // plot this column of pixels
			if( cnt_max > y_max)	// track to update vertical scale later
				y_max = cnt_max;
			y = cnt_max / pha_vscale;
			Plot_Graph(x, y, y_old);
			cnt_max = 0;
			x = x_nu;
			y_old = y;
		}
		cnt = *chanp;
		if(cp->pha_energy_comp){	// compensate for poor high energy response
			cnt *= chan;
		}
		// find max as # channels mapping to 1 pixel column varies
		if(cnt > cnt_max)
			cnt_max = cnt;
		// find total counts under cursor
		if(cursor_lo <= chan && chan <= cursor_hi)
			curs_cnt += cnt;
	}
	pha_vscale = (y_max / cp->graph_height) + 1;
	// update numeric display
	Display_U8(PHA_X_ZOOM, MSG_LINE_0, TXT_COLOR, BKGND_COLOR, cp->pha_zoom);
	Display_U32(PHA_X_CURS_CNT, MSG_LINE_1, TXT_COLOR, BKGND_COLOR, curs_cnt);
	chan = (cursor_hi + cursor_lo) / 2;
	if(cp->pha_cal){
		Display_Fixed(PHA_X_CURS_KEV, MSG_LINE_1, TXT_COLOR, BKGND_COLOR, Chan2Energy(chan));
	}else{
		Display_U16(PHA_X_CURS_KEV, MSG_LINE_1, TXT_COLOR, BKGND_COLOR, chan);
	}
	Display_Common();
}

/*******************************************************************************
 Function Name : Display_Rate
 Description : Display the values shown on spectrum display window
 *******************************************************************************/
/* Optional statistics:

    StdDev of TotalCount = SQRT(TotalCount) = SQRT(15330) =~ 123.8

    If we need count rate then
    CPM = 15330/15 = 1022
    StdDev of CountRate = SQRT(15330)/15 =~ 8

    Thus: Total = 15330 +/-123.8 counts
    CPM = 1022+/- 8

    under assumption it's a Gaussian distribution.

    Seems we got the same numbers with Mike.

    Critical Level which is the minimal level your counting must exceed above
    background to claim 95% sample activity:

    Lc = 2.326 * 123.8 =~ 288

	Butterworth low pass filter:
	alpha = dt / (t + dt)
	dt = sample period
	t = filter time
	y(t) = α * x(t) + (1-α) * y(t-1)
			or
	y(t) = y(t-1) + alpha * (x(t) - y(t-1))
	x(t) is source
	y(t]) output
*/

void Init_Rate(void)
{
	register configurations *cp = &configuration;

	Clear_Screen( BKGND_COLOR );
	__Display_Str(0, MSG_LINE_0, TXT_COLOR, BKGND_COLOR, msg_rate0);
	__Display_Str(0, MSG_LINE_1, TXT_COLOR, BKGND_COLOR, msg_rate1);

}

void Display_Rate()
{
	u16 tmp, n;
	fixed  y_i, y_prev, x_i, tau, dt, alpha;
	u16  cnt, cnt_prev, cnt_max, cnt_at_cur;
	u16 *rp;
	configurations *cp = &configuration;

	// clear the graphical display
	__Fill_Rectangle(0, RATE_BAR_Y, cp->graph_width, cp->graph_height + 2, BKGND_COLOR);
	// rate[] is unfiltered, filtered result gets plotted
	if(cp->rate_filter){
		// Set up Butterworth low pass filter
		dt = fixed_from_int(cp->refresh_time);
		tau = fixed_from_int(cp->rate_filter_coef);
		alpha = fixed_div(dt, tau + dt);
		y_prev = 0;
		cnt_prev = 0;
		cnt_max = 0;
		rp = &rate[0];
		for (n = 0; n < cp->graph_width; n++, rp++) {
			// Butterworth low pass filter
			// y(t) = y(t-1) + alpha * (x(t) - y(t-1))
			cnt = *rp;
			x_i = fixed_from_uint(cnt);
			if(x_i < 0)
				x_i = (fixed) 0x7fffffff;
			y_i = y_prev + fixed_mul(alpha, (x_i - y_prev));
			if(y_i < 0)
				y_i = (fixed) 0x7fffffff;
			y_prev = y_i;
			cnt = fixed_to_uint(y_i);
			Plot_Graph(n, (cnt / rate_vscale), (cnt_prev / rate_vscale));
			cnt_prev = cnt;
			if(cnt > cnt_max)
				cnt_max = cnt;
			if(n == cp->rate_cursor)
				cnt_at_cur = cnt;
		}
	} else {
		cnt_prev = 0;
		rp = &rate[0];
		for (n = 0; n < cp->graph_width; n++, rp++) {
			cnt = *rp;
			Plot_Graph(n, (cnt / rate_vscale), (cnt_prev / rate_vscale));
			cnt_prev = cnt;
			if(cnt > cnt_max)
				cnt_max = cnt;
			if(n == cp->rate_cursor)
				cnt_at_cur = cnt;
		}
	}
	rate_vscale = cnt_max / cp->graph_height + 1;
	// display cursor
	tmp = cp->rate_cursor;  			// map cursor to display column
	if(cp->graph == FATLINE){
		if(tmp == 0)
			tmp = 1;	// avoid negative x
		__Fill_Rectangle(tmp - 1, GRAPH_Y, 3, cp->graph_height, CURSOR_COLOR);
	} else {
		__Fill_Rectangle(tmp, GRAPH_Y, 1, cp->graph_height, CURSOR_COLOR);
	}

	Display_U16(RATE_CPM, MSG_LINE_0, TXT_COLOR, BKGND_COLOR, cnt);	// filtered rate
	Display_U16(RATE_CURSOR, MSG_LINE_1, TXT_COLOR, BKGND_COLOR, cnt_at_cur);	// at cursor
	Display_Common();
}
/*******************************************************************************
 Function Name : void Display_Common(void)
 Description : Updates display that is common to all modes
 *******************************************************************************/

void Display_Common(void){
	u32 tmp;
	u32 scale;
	register configurations *cp = &configuration;

	// update live count, a horizontal bar
	scale = cp->rate_alarm / cp->display_width;
	tmp = live_rate / scale;
	__Fill_Rectangle(0, RATE_BAR_Y, (u16)tmp, 3, YEL);
	// update run/stop
	if(stop){
		__Display_Str(COM_X_PAUSE, MSG_LINE_0, RED, BKGND_COLOR, "#");
	}
	else{
		__Display_Str(COM_X_PAUSE, MSG_LINE_0, GRN, BKGND_COLOR, "\"");
	}
	// Update Battery status
	__Display_Str(COM_X_BAT, MSG_LINE_0, Battery_Color[pwr_lvl], BKGND_COLOR, Battery_Status[pwr_lvl]);
}

/*******************************************************************************
 Function Name : void Plot_Graph(void)
 Description : Updates display
	x and y are relative to the lower left of graphics area, excluding text below
*******************************************************************************/
void Plot_Graph(s16 x, s16 y, s16 y_old){
	register configurations *cp = &configuration;

	if(x >= cp->graph_width)
		x = cp->graph_width - 1;
	if(x < 0)
		x = 0;
	if(y_old < 0)
		y_old = 0;
	if(y_old >= cp->graph_height)
		y_old = cp->graph_height - 1;
	if(y < 0)
		y = 0;
	if(y >= cp->graph_height)
		y = cp->graph_height - 1;
	y += GRAPH_Y;
	y_old += GRAPH_Y;
	if(cp->graph == DOT){
		Draw_Dot(x, y, SPECT_COLOR);
	} else if(cp->graph == LINE){
		if(y > y_old){
			__Fill_Rectangle((u16)x, (u16)y_old, 1, (u16)(y - y_old), SPECT_COLOR);
		}
		if(y < y_old){
			__Fill_Rectangle((u16)x, (u16)y, 1, (u16)(y_old - y), SPECT_COLOR);
		}
		if(y == y_old){
			__Fill_Rectangle((u16)x, (u16)y, 1, 1, SPECT_COLOR);
		}
	} else if(cp->graph == FATLINE){
		if(y > y_old){
			__Fill_Rectangle((u16)(x - 1), (u16)y_old, 3, (u16)(y - y_old + 2), SPECT_COLOR);
		}
		if(y < y_old){
			__Fill_Rectangle((u16)(x - 1), (u16)y, 3, (u16)(y_old - y + 2), SPECT_COLOR);
		}
		if(y == y_old){
			__Fill_Rectangle((u16)(x - 1), (u16)y, 3, 3, SPECT_COLOR);
		}
	}
}
