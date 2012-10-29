/*******************************************************************************
File Name: sig_acq.c
********************************************************************************/
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


/*

Butterworth low pass filter:
alpha := dt / (t + dt)
dt = sample period
t = filter time

y[t] = y[t-1] + alpha * (x[t] - y[t-1])
x[t] is source
y[t] output
 *
 */

/*
Parabolic peak from 3 samples at https://ccrma.stanford.edu/~jos/parshl/Peak_Detection_Steps_3.html

Parrabolic curve approximation:
P = ((a - g)/(a -2b + g))/2
p peak position
a 1st sample
b 2nd and highest sample
g 3rd sample

Peak value:
y(p) = b - ((a - g) * p)/4
y(p) = peak value at peak location
p peak position
a first sample
b 2nd and highest sample
g 3rd sample

*/

/*
The pulse detection algorithm is based on using a smoothed derivative (slope) of the data. We use the derivative so that DC level of
the data doesn't affect the detection of pulses. The derivative is calculated by convolving the data with a boxcar derivative function.
That is, to find the derivative at time T, we multiply several data points on each side of point T by the corresponding point in an array
that looks like this: (-1,-1,-1,-1, 1, 1, 1, 1)
(The number of samples—the length of the "boxcar"—is an adjustable parameter.) The
smoothed derivative is the sum of those multiplications.
This convolution smoothes the data as well as finding the derivative.


Derivative from convolution of surrounding points:

Multiply each point adjacent to t by the following and sum,
(-1,-1,-1, 1, 1, 1)
-1*y(t-3) + -1*y(t-2) + -1*y(t-1) + y(t+1) + y(t+2) + y(t+3)

From: http://universe.gsfc.nasa.gov/xrays/programs/astroe/eng/detection.html
*/



#include "stm32f10x_type.h"
#include "main.h"
#include "Function.h"
#include "Menu.h"
#include "Lcd.h"
#include "stm32f10x_lib.h"
#include "HW_V1_Config.h"
#include "Files.h"
#include "string.h"


//#define TRACE		// turn on debug code


// globals
extern configurations configuration;	// Configuration parameters
extern volatile s16 scan_buffer[SCAN_BUFFER_SZ]; //ring buffer of ADC readings
extern u32 spectrum[MAX_CHANNELS];	// pulse height histogram
extern u16 rate[320];					// rate histogram also used for scope mode
extern scan_states scan_state;			// global so scope mode can reset

extern u32 sys_time;				 	// real time clock in mS
extern u32 live_time;					// sampling time in mS
extern u32 live_rate;					// pulses detected in last full period

static u32 samp_cntr = 0;						// counts down the number of samples in 1 mS
static u32 cur_cnt = 0;						// pulses detected in current period
static fixed avg_zero = 0;
static u16 	tim2 = 970;
static bool alarm_on = FALSE;		// avoid interrupting alarm with beeps

/*******************************************************************************
 Function Name : Acq_Clear_All
 Description : clear all data
 *******************************************************************************/
void Acq_Clear_All(void) {
	//__Display_Str(0, 0, WHITE, PRN, "Clear All");
	bzero(&spectrum[0], sizeof(spectrum));
	bzero(&rate[0], sizeof(rate));
	//bzero(&scope[0], sizeof(scope));
	live_time = 0;
	sys_time = 0;
	scan_state = RESTART; 	// pulse detection state machine
}

/*******************************************************************************
 Function Name : Update_Rate
 Description : update rate data
 Note: typical rate of 120 cpm comes to 2 pulses per second
 *******************************************************************************/

void Update_Rate(void)
{
	s16 n;
	u16 cnt, *rp0, *rp1 ;
	configurations *cp = &configuration;
	fixed ftmp;
	static u32 last_live_time;

	// update zero adjust
	//tim2 += fixed_to_int(avg_zero) ;
	if(avg_zero < -FIXED_HALF){
		--tim2;
	}
	if(avg_zero > FIXED_HALF){
		++tim2;
	}
	TIM2_CCR4 = tim2;
	//Display_U16(220, 16, TXT_COLOR, BKGND_COLOR, tim2);
	//Display_Fixed(220, 0, TXT_COLOR, BKGND_COLOR, avg_zero);
	// scroll rate table
	rp0 = rp1 = &rate[0];
	rp1++;
	for (n = 1 ; n < MAX_RATE_SLOTS; n++) {
		*rp0++ = *rp1++;
	}
	// live time varies quite a bit so rate must be averaged
	// live time is in mS
	//live_rate = ((live_rate * 4) + (cur_cnt * 60 * 1000) / (live_time - last_live_time))/5;	// in cpm
	// refresh time in seconds
	live_rate = (cur_cnt * 60) / cp->refresh_time;
	last_live_time = live_time;
	*rp0 = (u16)live_rate;	// store into end of rate array
	cur_cnt = 0;
	// handle alarm
	if(live_rate > cp->rate_alarm && cp->alarm_en){
		alarm_on = TRUE;
		Beep(BEEP_500Hz, 5000);
	}else {
		alarm_on = FALSE;
	}
}
/*******************************************************************************
 Function Name : Scan_Samples
 Description : isolates pulses from stream of samples from ADC.  Note that ADC
 operates positive input only.  Negative input to Nano is converted by TIM2_CCR4
 feeding an opamp.  So zero is actually mid point in ADC counts. Also the hardware
 inverts the sample, so a positive pulse shows here as negative.

*******************************************************************************/

const static s16 zero = 2048; 	// ADC value representing an input of 0

#define SAMP_PER_MS 2000
static s16 head = -1;			// head of adc ring buffer
static s16 tail = -1; 			// tail of adc ring buffer


#define SAMP_PER_MS 2000

void  Scan_Samples(void){
	fixed ybox[7];	// array box car derivative
	s16 bin;						// index into peak height array
	fixed dt, tau, *yp0, *yp1, y, y_i;
	fixed threshold;				// trigger level for leading edge
	fixed deriv;
	fixed alpha, beta, gamma, peak;
	s16 i;
	configurations *cp = &configuration;


	ADC_Stop();
	tail = head = Get_Scan_Pos(); // get current absolute position in adc  buffer
	/*
	if(cp->sig_filter){
		// Set up Butterworth low pass filter
		dt = FIXED_HALF;			// sample time 0.5 uS
		tau = fixed_from_int(cp->sig_filter);  // filter time in uS
		alpha = fixed_div(dt, tau + dt);
	}
	*/
	ADC_Start();
	while(TRUE){
		if(tail == head){
			head = Get_Scan_Pos();
			// recalculate filter elements in case changed
			/*
			if(cp->sig_filter){
				// Set up Butterworth low pass filter in case of changes
				dt = FIXED_HALF;			// sample time 0.5 uS
				tau = fixed_from_int(cp->sig_filter);  // filter time in uS
				alpha = fixed_div(dt, tau + dt);
			}
			*/
		}
		if(tail == head)
			continue;
		// track live time
		if(samp_cntr++ >= SAMP_PER_MS){
			live_time++;
			samp_cntr = 0;
		}
		// get new value, adjust for zero and inversion at same time
		y = fixed_from_int(zero - scan_buffer[tail++]);
		if(tail >= SCAN_BUFFER_SZ){
			tail = 0;
		}
		// filter signal if needed
		/*
		if(cp->sig_filter){
			// Butterworth low pass filter
			y = *yp0 + fixed_mul(alpha, (y - *yp0));
		}
		*/
		// shift the boxcar window and find derivative
		yp0 =&ybox[0];
		yp1 = &ybox[1];
		for(i = 6; i > 0; i--){	// last box slot gets new y value
			*yp0++ = *yp1++;
		}
		*yp0 = y;		// place latest sample in end of boxcar
		// compute the derivative
		deriv = 0;
		yp0 =&ybox[0];
		deriv -= *yp0++;
		deriv -= *yp0++;
		alpha = *yp0;
		deriv -= *yp0++;
		beta = *yp0++;
		gamma = *yp0;
		deriv += *yp0++;
		deriv += *yp0++;
		deriv += *yp0++;
		// process depending on state
		switch(scan_state){
			case RESTART:
				scan_state = LEADING;
				//cp->scope_valid = FALSE;
				break;
			case LEADING:
				if(cp->sig_type == PULSE_POS && deriv > cp->sig_dvdt_lim){
					scan_state = PEAK;
					break;
				}
				if(cp->sig_type == PULSE_NEG && deriv < cp->sig_dvdt_lim){
					scan_state = PEAK;
					break;
				}
				// if no pulse then check the zero
				avg_zero = (avg_zero * 20 + y)/21;
				break;
			case PEAK:
				// reverse derivative indicates peak
				if(cp->sig_type == PULSE_POS && deriv < 0){
					scan_state = TAIL;
				}
				if(cp->sig_type == PULSE_NEG && deriv > 0){
					scan_state = TAIL;
				}
				if(scan_state == TAIL){
					// handle gaussian approximation if enabled
					if(cp->sig_gaussian){
						// p = ((a - g)/(a -2b + g))/2 = position of peak
						peak = (fixed_div((alpha - gamma),(alpha - (2 * beta) + gamma))) / 2;
						// y(p) = b - ((a - g) * p)/4  = peak value
						peak = beta - (fixed_mul((alpha - gamma), peak) / 4);
					} else {
						peak = (alpha + beta + gamma) / 3;
					}
					if(cp->sig_type == PULSE_NEG){
						peak = -peak;
					}
					// peak now always positive
					if( peak > cp->sig_lo_lim && peak < cp->sig_hi_lim){
						bin = fixed_to_int(peak);
						spectrum[bin]++;  // increment count in spectrum array
						cur_cnt++;
						// handle rate meter beeping
						if(cp->rate_beep && !alarm_on){
							Beep(BEEP_500Hz, 10);
						}
					}
				}
				break;
			case TAIL:
				// find where curve turns back to baseline
				if(cp->sig_type == PULSE_POS && deriv >= 0){
					scan_state = LEADING;
				}
				if(cp->sig_type == PULSE_NEG && deriv <= 0){
					scan_state = LEADING;
				}
				break;
		}	// switch(scan_state)
	}	// while ring buffer not empty
}



