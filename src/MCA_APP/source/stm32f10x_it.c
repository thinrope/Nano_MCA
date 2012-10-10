/*******************************************************************************
File Name: stm32f10x_it.c
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
#include "main.h"
#include "Function.h"
#include "Menu.h"
#include "Lcd.h"
#include "stm32f10x_lib.h"
#include "stm32f10x_conf.h"
#include "stm32f10x_it.h"
#include "HW_V1_Config.h"
#include "Files.h"
#include "string.h"

extern volatile short ctr_refresh, ctr_key, ctr_beep;
extern volatile u8 scan_segment, sync_segment, scan_mode;
extern bool stop;			// stop / start sampling
extern volatile s16 scan_buffer[SCAN_BUFFER_SZ];
extern configurations configuration;	// Configuration parameters
extern u32 sys_time;					// real time clock in mS
extern u32 samp_time;					// sampling time in mS
extern bool SD_Card;			// true if SD card is present
extern unsigned const char *SD_Msgs[];

#define LANGUAGE EN
#if LANGUAGE == EN
const static char *msghard = "Hard Fault Exception";
const static char *msgmm = "Memory Management Exception";
const static char *msgbfe = "Bus Fault Exception";
const static char *msgufe = "Usage Fault Exception";
#endif

void NMIException(void)
{}

void HardFaultException(void)
{
	__Display_Str(100, 20, RED, BKGND_COLOR, msghard);
	while (1) {}
}

void MemManageException(void)
{
	__Display_Str(100, 20, RED, BKGND_COLOR, msgmm);
	while (1) {}
}

void BusFaultException(void)
{
	__Display_Str(100, 20, RED, BKGND_COLOR, msgbfe);
	while (1) {}
}
void UsageFaultException(void)
{
	__Display_Str(100, 20, RED, BKGND_COLOR, msgufe);
	while (1) {}
}

void DebugMonitor(void)
{}

void SVCHandler(void)
{}

void PendSVC(void)
{}

void SysTickHandler(void)
{}

void WWDG_IRQHandler(void)
{}

void PVD_IRQHandler(void)
{}

void TAMPER_IRQHandler(void)
{}

void RTC_IRQHandler(void)
{}

void FLASH_IRQHandler(void)
{}

void RCC_IRQHandler(void)
{}

void EXTI0_IRQHandler(void)
{}

void EXTI1_IRQHandler(void)
{}

void EXTI2_IRQHandler(void)
{}

void EXTI3_IRQHandler(void)
{}

void EXTI4_IRQHandler(void)
{}

void            DMAChannel1_IRQHandler(void)
{
	u8 ss = scan_segment;

	DMA_CCR1 = 0x00000000; // disable DMA1
	DMA_IFCR = 0x00000002; // clear transfer complete interrupt for DMA channel1
	if (ss >= SCAN_SEGMENTS - 1)
		ss = 0;
	else
		ss++;

	DMA_CMAR1 = (u32)&scan_buffer + ss * SCAN_SEGMENT_SZ * sizeof(scan_buffer[0]);
	DMA_CNDTR1 = SCAN_SEGMENT_SZ;
	DMA_CCR1 = 0x00003583; // enable DMA1
	scan_segment = ss;
}


void            DMAChannel2_IRQHandler(void)
{
}

void            DMAChannel3_IRQHandler(void)
{
}

void            DMAChannel4_IRQHandler(void)
{
}

void            DMAChannel5_IRQHandler(void)
{
}

void            DMAChannel6_IRQHandler(void)
{
}

void            DMAChannel7_IRQHandler(void)
{
}

void            ADC_IRQHandler(void)
{
}

void            USB_HP_CAN_TX_IRQHandler(void)
{
	//__Display_Str(100, 20, RED, PRN, "USB_HP_CAN_TX_IRQHandler");
	__CTR_HP();
}

void            USB_LP_CAN_RX0_IRQHandler(void)
{
	//__Display_Str(100, 20, RED, PRN, "USB_LP_CAN_RX0_IRQHandler");
	__USB_Istr();
}

void            CAN_RX1_IRQHandler(void)
{
}

void            CAN_SCE_IRQHandler(void)
{
}

void            EXTI9_5_IRQHandler(void)
{
}

void            TIM1_BRK_IRQHandler(void)
{
}

void            TIM1_UP_IRQHandler(void)
{
}

void            TIM1_TRG_COM_IRQHandler(void)
{
}

void            TIM1_CC_IRQHandler(void)
{
}

// TIM2 is used as a PWM output to the dc bias offset opamp
void            TIM2_IRQHandler(void)
{
}
/****************************************************************************
Function Name : TIM3_IRQHandler
Description : TIMER 3 interrupt request handle,
              its mainly uesed for system clock delay
	      and detect the key periodically
******************************************************************************/
// ADC data comes in at 2 mHz rate, with 10 mS 20,000 samples
void            TIM3_IRQHandler(void)
{
	keycodes key;
	configurations *cp = &configuration;

	TIM3_SR = 0; // clear the interrupt flag
	sys_time++;
	if (ctr_refresh >= 0)
		ctr_refresh--;	// display updates
	if (ctr_key >= 0)
		ctr_key--;		// keystroke checking etc
	if (ctr_beep >= 0)
		ctr_beep--;		// sound

	if (ctr_key == 0) { // check key press every 20ms
		ctr_key = KEY_SAMPLE;
		key = Get_Key();	// Get_Key handles repeats etc.
		if(key != KEYCODE_VOID){
			switch(cp->mode){
				case PHA:
					Do_Keys_PHA(key);
					break;
				case RATE:
					Do_Keys_Rate(key);
					break;
				//case SCOPE:
				//case DERIV:
				//	Do_Keys_Scope(key);
				//	break;
				case MENU:
					Do_Keys_Menu(key);
					break;
			}
		}
	}

	if (ctr_beep == 0) {	// Turn off the beep signal
		TIM4_CR1 &= ~(0x0001);  //CEN=0, TIMER4 Enable
	}

	if (ctr_refresh == 0){
		ctr_refresh = cp->refresh_time * 1000;
		Update_Battery();
		switch(cp->mode){
			case PHA:
				Display_Spectrum();
				break;
			case RATE:
				Display_Rate();
				break;
			//case SCOPE:
			//case DERIV:
			//	Display_Common();
			//	Display_Scope();
			//	break;
			case MENU:
				break;
		}
		if(!stop){		// stop == TRUE also stops signal collection in main.c
			Update_Rate();
		}
		if (SD_Card_On()){
			SD_Card = FALSE;	// no TF, micro SD card
		} else {	// card detected
			if(SD_Card == FALSE){
				if (FAT_Info()){
					__Display_Str(0, 0, RED, BKGND_COLOR, SD_Msgs[SDErr]);
				} else {
					SD_Card = TRUE;
				}
			}
		}
	}
}


// TIM4 drives the reference output
void            TIM4_IRQHandler(void)
{
}

void            I2C1_EV_IRQHandler(void)
{
}

void            I2C1_ER_IRQHandler(void)
{
}

void            I2C2_EV_IRQHandler(void)
{
}

void            I2C2_ER_IRQHandler(void)
{
}

void            SPI1_IRQHandler(void)
{
}

void            SPI2_IRQHandler(void)
{
}

void            USART1_IRQHandler(void)
{
}

void            USART2_IRQHandler(void)
{
}

void            USART3_IRQHandler(void)
{
}

void            EXTI15_10_IRQHandler(void)
{
}

void            RTCAlarm_IRQHandler(void)
{
}

void            USBWakeUp_IRQHandler(void)
{
}
/********************************* END OF FILE ********************************/
