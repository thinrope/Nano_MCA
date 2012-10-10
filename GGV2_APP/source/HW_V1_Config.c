/*******************************************************************************
File Name: HW_V1_config.c
 This file provides the interface to hardware. Interrupt processing is in
 another file.
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
 *   This file contains portions of example code from STMicroelectronics.  *
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
#include "Main.h"
#include "Function.h"
#include "Menu.h"
#include "stm32f10x_it.h"
#include "stm32f10x_systick.h"
#include "HW_V1_Config.h"
#include "Lcd.h"

volatile s16 ctr_refresh, ctr_key, ctr_beep;
volatile u8 scan_segment, sync_segment, scan_mode;
volatile s16 scan_buffer[SCAN_BUFFER_SZ]; // sampling buffer
volatile u32 sys_time;					// real time clock in mS
volatile u8 pwr_lvl;

extern configurations configuration;	// Configuration parameters

//------------ ADC sample rate ------------
const u16 Scan_PSC[22] = // prescale of ADC sample interval counter - 1
 {11, 11, 11, 11, 11, 15, 15, 15, 15, 15, 15, 31, 63, 63, 127, 255, 255, 255, 511, 511, 511, 1023 };

const u16 Scan_ARR[22] = // frequency division of ADC sample interval counter - 1
 {6, 6, 6, 6, 6, 8, 17, 35, 89, 179, 359, 449, 449, 899, 1124, 1124, 2249, 5624, 5624, 11249, 28124, 28124 };

//------------ output signal used for audio output ------------
// output frequency prescaler
unsigned const int Fout_PSC[16] =
//1MHz 500kHz 200kHz 100kHz 50kHz 20kHz 10kHz 5kHz 2kHz 1kHz 500Hz 200Hz 100Hz  50Hz   20Hz   10Hz"
{    0,     0,     0,     0,   0,     0,   0,    0, 4-1, 4-1, 16-1, 16-1, 64-1, 64-1, 128-1, 128-1};
// frequency division factors
unsigned const int Fout_ARR[16] =
//1MHz 500kHz 200kHz 100kHz   50kHz   20kHz   10kHz     5kHz    2kHz     1kHz 5  00Hz    200Hz    100Hz     50Hz     20Hz     10Hz"
{ 72-1, 144-1, 360-1, 720-1, 1440-1, 3600-1, 7200-1, 14400-1, 9000-1, 18000-1, 9000-1, 22500-1, 11250-1, 22500-1, 28125-1, 56250-1};


#if defined(__IAR_SYSTEMS_ICC__)
// IAR CODE NOT TESTED
#pragma language=extended
#pragma segment="CONFIG"

#pragma location = ".config_flash"
const u16  config_flash[512];
{
  { .__ptr = __sfe( "CONFIG" ) },
  __iar_program_start,

#else

u16  config_flash[512] __attribute__ ((section(".config_flash")));
#endif /* IAR */


#define Page_Address    0x0801F000 //start address of the stm32 flash

// External calls
/* u8 __MSD_WriteBlock(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite) */
u8 __MSD_WriteBlock(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
/* u8 __MSD_ReadBlock(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead) */
u8 __MSD_ReadBlock(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead);

/*******************************************************************************
Function Name : NVIC_Configuration
Description : Configure DMA inbterrupt channel priority
*******************************************************************************/
void NVIC_Configuration(void){
	TRACE("NVIC_Configuration");
	NVIC_InitType NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = DMAChannel1_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
Function Name : Set_Range
Description : set the range of the voltage
	 if attenuator = 0 0 <= range <= 9
	 if attenuator = 1 11 <= range <= 18
 *******************************************************************************/
void Set_Range(fixed range)
{
	TRACE("Set_Range");
	if(range == 0){
		// Connected To GND
		RANGE_A_LOW();
		RANGE_B_LOW();
		RANGE_C_LOW();
		RANGE_D_LOW();
	} else if(range <= FIXED_HALF){
		RANGE_A_HIGH();
		RANGE_B_HIGH();
		RANGE_C_HIGH();
		RANGE_D_HIGH();
	}else if(range <= fixed_from_int(4)){
		RANGE_A_HIGH();
		RANGE_B_HIGH();
		RANGE_C_HIGH();
		RANGE_D_LOW();
	} else if(range <= fixed_from_int(4)){
		RANGE_A_LOW();
		RANGE_B_HIGH();
		RANGE_C_HIGH();
		RANGE_D_LOW();
	} else if(range > fixed_from_int(4)){
		RANGE_A_HIGH();
		RANGE_B_LOW();
		RANGE_C_HIGH();
		RANGE_D_LOW();
	}
	/*
	RANGE_A_LOW();
	RANGE_B_LOW();
	RANGE_C_HIGH();
	RANGE_D_LOW();


	RANGE_A_HIGH();
	RANGE_B_HIGH();
	RANGE_C_LOW();
	RANGE_D_LOW();
	*/
}

/*******************************************************************************
Function Name : Cal_HW
Description :
This is used to set the hardware 0, to offset dc bias with opamp
ADC input between ground and analog supply, opamp converts
negative signals to be positive
*******************************************************************************/
void Cal_HW()
{
	u32 tim2;
	u16 adc0, adc1;
	configurations *cp = &configuration;

	TRACE("Cal_HW");
	ADC_Stop();
	// Rerun internal ADC calibration
	ADC1_CR2 |=0x00000001;
	ADC2_CR2 |=0x00000001;       //ADON=1 : ADC1 & ADC2 Start
	ADC1_CR2 |=0x00000008;
	while(ADC1_CR2 & 0x00000008);//ADC1 Initialize calibration register
	ADC2_CR2 |=0x00000008;
	while(ADC2_CR2 & 0x00000008);//ADC2 Initialize calibration register
	ADC1_CR2 |=0x00000004;
	while(ADC1_CR2 & 0x00000004);//ADC1 calibration
	ADC2_CR2 |=0x00000004;
	ADC_Start();
	// analog zero is adjusted in sig_acq.c
}


/*******************************************************************************
Function Name : Set_Time_Base
Description : set ADC clock
Para : Base is the index of the Scan_PSC&Scan_ARR
 0 <= base <= 21
*******************************************************************************/
void Set_Base(char Base)
{
	TRACE("Set_Base");
	TIM1_PSC = Scan_PSC[Base];
	TIM1_ARR = Scan_ARR[Base];
	TIM1_CCR1 = (Scan_ARR[Base] + 1) / 2;
}

/*******************************************************************************
Function Name : ADC_Stop
Description : start the ADC sampling scan
*******************************************************************************/
void ADC_Stop(void)
{
	TRACE("ADC_Stop");
   	DMA_CCR1 = 0x00000000; // disable DMA1
   	scan_mode = IDLE;     // 0=idle, 1=pre-fetch, 2=trig-fetch, 3=post-fetch
}
/*******************************************************************************
Function Name : ADC_Start
Description : start the ADC sampling scan
*******************************************************************************/
void ADC_Start(void)
{
	TRACE("ADC_Start");
	ADC_Stop(); // disable DMA1
	DMA_CPAR1 = ADC1_DR_ADDR; // base address of the peripheral's data register for DMA1
	DMA_CMAR1 = (u32)scan_buffer;
	DMA_CNDTR1 = SCAN_SEGMENT_SZ;
	DMA_CCR1 = 0x00003583; // enable DMA1
}
/*******************************************************************************
 * Drive the beeper sounder		Input: index of frequency and duration in mS
 *******************************************************************************/
void Beep(beep_t Freq, u16 Duration)
{
	TRACE("Beep");
	TIM4_PSC = Fout_PSC[Freq];		// Prescaler
	TIM4_ARR = Fout_ARR[Freq];        //Cycle
	TIM4_CCR1 = (Fout_ARR[Freq] + 1) / 2;	// 50% duty cycle
	TIM4_CR1 |=0x0001;//CEN=1, TIMER4 Enable
	ctr_beep = Duration;
}
/*******************************************************************************
Function Name : USB_On
Description : detect the USB status
Return:	 0: USB plug in, 0xff:USB plug out
*******************************************************************************/
char USB_On(void)
{
	TRACE("USB_On");
   	if (GPIOB_IDR & 0x0400)
    	return 0;
   	else
      	return 0xff;
}
/*******************************************************************************
Function Name : SD_Card_On
Description : detect the SD card status
Return:	 0: SD card plug in, 0xff:SD card plug out
*******************************************************************************/
char SD_Card_On(void)
{
	TRACE("SD_Card_On");
   	if (GPIOD_IDR & 0x0400)
     	return 0xff;
   	else
      	return 0; // SD_Card_ON
}

/*******************************************************************************
 Delay_MS
*******************************************************************************/
// NOTE: this does not work if called from method called through timer interrupt code
void Delay_MS(u16 delay)
{
	u32 end;

	TRACE("Delay_MS");
  	end = delay + sys_time;
  	while (sys_time < end);
}

/*******************************************************************************
 wait for keypress
*******************************************************************************/
keycodes Wait_For_Key(void)
{
	TRACE("Wait_For_Key");
}
/*******************************************************************************
 Function Name : GetScanPos
 Description : find and reurn index of current point in scan buffer, returns -1
 			if no peak found
*******************************************************************************/
u16 Get_Scan_Pos(void)
{
   u16 t;
   u8  ss;

	TRACE("Get_Scan_Pos");
   	do {
    	ss = scan_segment;
     	t = (ss + 1) * SCAN_SEGMENT_SZ;
     	t = t - DMA_CNDTR1;
   	} while (ss != scan_segment);
   	if (t >= SCAN_BUFFER_SZ) t -= SCAN_BUFFER_SZ;
   	return t;
}
/*******************************************************************************
----  Here below are the interupt routines for hardware ----
*******************************************************************************/

/*******************************************************************************
 Function Name : Update_Battery Interrupt
 Description : update status of battery, called from a timer interrupt
*******************************************************************************/

void Update_Battery(void)
{
	static u32 bat_sum;
	u16 bat_avg, bat_now;

	//TRACE("Update_Battery");
	bat_sum = 3200 * 32;
	bat_avg = bat_sum / 32,
	bat_now = *((vu16 *)(ADC1_DR_ADDR + 2));
	bat_avg = bat_sum / 32;
	bat_sum = bat_sum + bat_now - bat_avg;
	if (GPIOB_IDR & 0x0400) pwr_lvl = 4; // USB power
	else if (bat_avg > 3000) pwr_lvl = 3;
	else if (bat_avg > 2730) pwr_lvl = 2;
	else if (bat_avg > 2460) pwr_lvl = 1;
	else                    pwr_lvl = 0;
}

/*******************************************************************************
 Function Name : Get_Key
 Description : find pressed key, called every KEY_SAMPLE milliseconds
*******************************************************************************/
keycodes Get_Key(void)
{
	static keycodes key_prev;
	static s16 key_down_ctr;
	keycodes key;
	register configurations *cp = &configuration;

	//key = __Get_Key();
	if(cp->orient == VERT){
		if (!(GPIOD_IDR & KEY_M)) key = KEYCODE_M;
		else if (!(GPIOA_IDR & KEY_B)) key = KEYCODE_B;
		else if (!(GPIOA_IDR & KEY_PLAY)) key = KEYCODE_PLAY;
		else if (!(GPIOA_IDR & KEY_UP)) key = KEYCODE_RIGHT;
		else if (!(GPIOD_IDR & KEY_DOWN)) key = KEYCODE_LEFT;
		else if (!(GPIOA_IDR & KEY_LEFT)) key = KEYCODE_UP;
		else if (!(GPIOA_IDR & KEY_RIGHT)) key = KEYCODE_DOWN;
		else key = KEYCODE_VOID;
	} else {
		if (!(GPIOD_IDR & KEY_M)) key = KEYCODE_M;
		else if (!(GPIOA_IDR & KEY_B)) key = KEYCODE_B;
		else if (!(GPIOA_IDR & KEY_PLAY)) key = KEYCODE_PLAY;
		else if (!(GPIOA_IDR & KEY_UP)) key = KEYCODE_UP;
		else if (!(GPIOD_IDR & KEY_DOWN)) key = KEYCODE_DOWN;
		else if (!(GPIOA_IDR & KEY_LEFT)) key = KEYCODE_LEFT;
		else if (!(GPIOA_IDR & KEY_RIGHT)) key = KEYCODE_RIGHT;
		else key = KEYCODE_VOID;
	}
	// no key pressed
	if(key == KEYCODE_VOID){
		key_prev = key;
		return(key);
	}
	// new key pressed
	if(key != key_prev){
		key_down_ctr = KEY_RELEASE / KEY_SAMPLE;
		key_prev = key;
		return(key);
	}
	// do auto repeat
	else {
		if( --key_down_ctr > 0){
			return(KEYCODE_VOID);
		}
		else {
			key_down_ctr = KEY_REPEAT / KEY_SAMPLE;
			return(key);
		}
	}
}


/******************************** END OF FILE *********************************/
