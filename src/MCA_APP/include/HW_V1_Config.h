/*******************************************************************************
 File name  :  HW_V1_config.h
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

#ifndef __HWV1_CONFIG_H
#define __HWV1_CONFIG_H


#define TIM1_CR1    (*((vu32 *)(TIM1_BASE+0x00)))
#define TIM1_CR2    (*((vu32 *)(TIM1_BASE+0x04)))
#define TIM1_DIER   (*((vu32 *)(TIM1_BASE+0x0C)))
#define TIM1_SR     (*((vu32 *)(TIM1_BASE+0x10)))
#define TIM1_CCMR1  (*((vu32 *)(TIM1_BASE+0x18)))
#define TIM1_CCER   (*((vu32 *)(TIM1_BASE+0x20)))
#define TIM1_PSC    (*((vu32 *)(TIM1_BASE+0x28)))
#define TIM1_ARR    (*((vu32 *)(TIM1_BASE+0x2C)))
#define TIM1_RCR    (*((vu32 *)(TIM1_BASE+0x30)))
#define TIM1_CCR1   (*((vu32 *)(TIM1_BASE+0x34)))
#define TIM1_BDTR   (*((vu32 *)(TIM1_BASE+0x44)))

#define TIM2_CR1    (*((vu32 *)(TIM2_BASE+0x00)))
#define TIM2_DIER   (*((vu32 *)(TIM2_BASE+0x0C)))
#define TIM2_SR     (*((vu32 *)(TIM2_BASE+0x10)))
#define TIM2_CCMR2  (*((vu32 *)(TIM2_BASE+0x1C)))
#define TIM2_CCER   (*((vu32 *)(TIM2_BASE+0x20)))
#define TIM2_PSC    (*((vu32 *)(TIM2_BASE+0x28)))
#define TIM2_ARR    (*((vu32 *)(TIM2_BASE+0x2C)))
#define TIM2_CCR4   (*((vu32 *)(TIM2_BASE+0x40)))

#define TIM3_CR1    (*((vu32 *)(TIM3_BASE+0x00)))
#define TIM3_DIER   (*((vu32 *)(TIM3_BASE+0x0C)))
#define TIM3_SR     (*((vu32 *)(TIM3_BASE+0x10)))
#define TIM3_CCMR2  (*((vu32 *)(TIM3_BASE+0x1C)))
#define TIM3_CCER   (*((vu32 *)(TIM3_BASE+0x20)))
#define TIM3_PSC    (*((vu32 *)(TIM3_BASE+0x28)))
#define TIM3_ARR    (*((vu32 *)(TIM3_BASE+0x2C)))
#define TIM3_CCR1   (*((vu32 *)(TIM3_BASE+0x34)))
#define TIM4_CR1    (*((vu32 *)(TIM4_BASE+0x00)))
#define TIM4_DIER   (*((vu32 *)(TIM4_BASE+0x0C)))
#define TIM4_SR     (*((vu32 *)(TIM4_BASE+0x10)))
#define TIM4_CCMR1  (*((vu32 *)(TIM4_BASE+0x18)))
#define TIM4_CCMR2  (*((vu32 *)(TIM4_BASE+0x1C)))
#define TIM4_CCER   (*((vu32 *)(TIM4_BASE+0x20)))
#define TIM4_PSC    (*((vu32 *)(TIM4_BASE+0x28)))
#define TIM4_ARR    (*((vu32 *)(TIM4_BASE+0x2C)))
#define TIM4_CCR1   (*((vu32 *)(TIM4_BASE+0x34)))


typedef enum {
	BEEP_1MHz,
	BEEP_500kHz,
	BEEP_200kHz,
	BEEP_100kHz,
	BEEP_50kHz,
	BEEP_20kHz,
	BEEP_10kHz,
	BEEP_5kHz,
	BEEP_2kHz,
	BEEP_1kHz,
	BEEP_500Hz,
	BEEP_200Hz,
	BEEP_100Hz,
	BEEP_50Hz,
	BEEP_20Hz,
	BEEP_10Hz} beep_t;

#define ADC2_CR1    (*((vu32 *)(ADC2_BASE+0x04)))
#define ADC2_CR2    (*((vu32 *)(ADC2_BASE+0x08)))
#define ADC2_SMPR1  (*((vu32 *)(ADC2_BASE+0x0C)))
#define ADC2_SMPR2  (*((vu32 *)(ADC2_BASE+0x10)))
#define ADC2_SQR1   (*((vu32 *)(ADC2_BASE+0x2C)))
#define ADC2_SQR3   (*((vu32 *)(ADC2_BASE+0x34)))
#define ADC2_DR     (*((vu32 *)(ADC2_BASE+0x4C)))
#define ADC1_CR1    (*((vu32 *)(ADC1_BASE+0x04)))
#define ADC1_CR2    (*((vu32 *)(ADC1_BASE+0x08)))
#define ADC1_SMPR1  (*((vu32 *)(ADC1_BASE+0x0C)))
#define ADC1_SMPR2  (*((vu32 *)(ADC1_BASE+0x10)))
#define ADC1_SQR1   (*((vu32 *)(ADC1_BASE+0x2C)))
#define ADC1_SQR3   (*((vu32 *)(ADC1_BASE+0x34)))
#define ADC1_DR     (*((vu32 *)(ADC1_BASE+0x4C)))
#define DMA_ISR     (*((vu32 *)(DMA_BASE+0x00)))
#define DMA_IFCR    (*((vu32 *)(DMA_BASE+0x04)))
#define DMA_CCR1    (*((vu32 *)(DMA_BASE+0x08)))
#define DMA_CNDTR1  (*((vu32 *)(DMA_BASE+0x0C)))
#define DMA_CPAR1   (*((vu32 *)(DMA_BASE+0x10)))
#define DMA_CMAR1   (*((vu32 *)(DMA_BASE+0x14)))
#define DMA_CCR2    (*((vu32 *)(DMA_BASE+0x1C)))
#define DMA_CNDTR2  (*((vu32 *)(DMA_BASE+0x20)))
#define DMA_CPAR2   (*((vu32 *)(DMA_BASE+0x24)))
#define DMA_CMAR2   (*((vu32 *)(DMA_BASE+0x28)))
#define ADC1_DR_ADDR  ((u32)0x4001244C)

#define GPIOA_CRL   (*((vu32 *)(GPIOA_BASE+0x00)))
#define GPIOB_CRL   (*((vu32 *)(GPIOB_BASE+0x00)))
#define GPIOC_CRL   (*((vu32 *)(GPIOC_BASE+0x00)))
#define GPIOD_CRL   (*((vu32 *)(GPIOD_BASE+0x00)))
#define GPIOE_CRL   (*((vu32 *)(GPIOE_BASE+0x00)))

#define GPIOA_CRH   (*((vu32 *)(GPIOA_BASE+0x04)))
#define GPIOB_CRH   (*((vu32 *)(GPIOB_BASE+0x04)))
#define GPIOC_CRH   (*((vu32 *)(GPIOC_BASE+0x04)))
#define GPIOD_CRH   (*((vu32 *)(GPIOD_BASE+0x04)))
#define GPIOE_CRH   (*((vu32 *)(GPIOE_BASE+0x04)))

#define GPIOA_ODR   (*((vu32 *)(GPIOA_BASE+0x0C)))
#define GPIOB_ODR   (*((vu32 *)(GPIOB_BASE+0x0C)))
#define GPIOC_ODR   (*((vu32 *)(GPIOC_BASE+0x0C)))
#define GPIOD_ODR   (*((vu32 *)(GPIOD_BASE+0x0C)))
#define GPIOE_ODR   (*((vu32 *)(GPIOE_BASE+0x0C)))

#define GPIOA_IDR   (*((vu32 *)(GPIOA_BASE+0x08)))
#define GPIOB_IDR   (*((vu32 *)(GPIOB_BASE+0x08)))
#define GPIOC_IDR   (*((vu32 *)(GPIOC_BASE+0x08)))
#define GPIOD_IDR   (*((vu32 *)(GPIOD_BASE+0x08)))
#define GPIOE_IDR   (*((vu32 *)(GPIOE_BASE+0x08)))

#define GPIOA_BSRR  (*((vu32 *)(GPIOA_BASE+0x10)))
#define GPIOB_BSRR  (*((vu32 *)(GPIOB_BASE+0x10)))
#define GPIOC_BSRR  (*((vu32 *)(GPIOC_BASE+0x10)))
#define GPIOD_BSRR  (*((vu32 *)(GPIOD_BASE+0x10)))
#define GPIOE_BSRR  (*((vu32 *)(GPIOE_BASE+0x10)))

#define GPIOA_BRR   (*((vu32 *)(GPIOA_BASE+0x14)))
#define GPIOB_BRR   (*((vu32 *)(GPIOB_BASE+0x14)))
#define GPIOC_BRR   (*((vu32 *)(GPIOC_BASE+0x14)))
#define GPIOD_BRR   (*((vu32 *)(GPIOD_BASE+0x14)))
#define GPIOE_BRR   (*((vu32 *)(GPIOE_BASE+0x14)))

#define AFIO_MAPR   (*((vu32 *)(AFIO_BASE+0x04)))

#define SCS_BASE       ((u32)0xE000E000)
#define SysTick_BASE   (SCS_BASE + 0x0010)

#define MSD_CS_LOW()    GPIOB_BRR  = GPIO_Pin_12  //Select MSD Card
#define MSD_CS_HIGH()   GPIOB_BSRR = GPIO_Pin_12  //Deselect MSD Card

#define KEY_UP         (GPIO_Pin_6)  //GPIOA
#define KEY_DOWN       (GPIO_Pin_9)  //GPIOD
#define KEY_LEFT       (GPIO_Pin_5)  //GPIOA
#define KEY_RIGHT      (GPIO_Pin_7)  //GPIOA
#define KEY_PLAY       (GPIO_Pin_4)  //GPIOA
#define KEY_M          (GPIO_Pin_11) //GPIOD
#define KEY_B          (GPIO_Pin_3)  //GPIOA

#define LDC_DATA_OUT        GPIOE_ODR
#define LDC_DATA_INP        GPIOE_IDR
#define LCD_DATA_BUS_INP()  GPIOC_CRH = 0x44444444 ; GPIOE_CRL = 0x44444444
#define LCD_DATA_BUS_OUT()  GPIOC_CRH = 0x33333333 ; GPIOE_CRL = 0x33333333

#define LCD_nRST_LOW()  GPIOC_BRR  = GPIO_Pin_0
#define LCD_nRST_HIGH() GPIOC_BSRR = GPIO_Pin_0
#define LCD_RS_LOW()    GPIOD_BRR  = GPIO_Pin_1
#define LCD_RS_HIGH()   GPIOD_BSRR = GPIO_Pin_1
#define LCD_nWR_LOW()   GPIOD_BRR  = GPIO_Pin_5
#define LCD_nWR_HIGH()  GPIOD_BSRR = GPIO_Pin_5
#define LCD_nWR_ACT()   GPIOD_BRR  = GPIO_Pin_5 ; GPIOD_BSRR = GPIO_Pin_5
#define LCD_nRD_LOW()   GPIOD_BRR  = GPIO_Pin_4
#define LCD_nRD_HIGH()  GPIOD_BSRR = GPIO_Pin_4
#define LCD_nRD_ACT()   GPIOB_BRR  = GPIO_Pin_4 ; GPIOD_BSRR = GPIO_Pin_4

#define RANGE_A_LOW()   GPIOB_BRR  = GPIO_Pin_0
#define RANGE_A_HIGH()  GPIOB_BSRR = GPIO_Pin_0
#define RANGE_B_LOW()   GPIOC_BRR  = GPIO_Pin_5
#define RANGE_B_HIGH()  GPIOC_BSRR = GPIO_Pin_5
#define RANGE_C_LOW()   GPIOC_BRR  = GPIO_Pin_4
#define RANGE_C_HIGH()  GPIOC_BSRR = GPIO_Pin_4
#define RANGE_D_LOW()   GPIOB_BRR  = GPIO_Pin_1
#define RANGE_D_HIGH()  GPIOB_BSRR = GPIO_Pin_1

#define FOUT_LOW()      GPIOD_BRR  = GPIO_Pin_12
#define FOUT_HIGH()     GPIOD_BSRR = GPIO_Pin_12

// Settings for ADC with DMA
#define SCAN_SEGMENT_SZ 1024
#define SCAN_SEGMENTS 2
#define SCAN_BUFFER_SZ (SCAN_SEGMENT_SZ * SCAN_SEGMENTS)

typedef enum{ IDLE, PRE_FETCH, TRIG_FETCH, POST_FETCH} Scan_Modes;

#define CHANNELS 2048	// 12 bit adc but 0 volts is at mid range

typedef enum {
	KEYCODE_EMPTY,	// key has been retrieved
	KEYCODE_VOID,	// key check shows no keys pressed
	KEYCODE_PLAY,	// play/stop button
	KEYCODE_M,		// M key, used for menui
	KEYCODE_B,		// unknown key
	KEYCODE_UP,
	KEYCODE_DOWN,
	KEYCODE_LEFT,
	KEYCODE_RIGHT} keycodes;



void NVIC_Configuration(void);
void Set_Range(fixed Range);
void Set_Y_Pos(u16 i, u16 Y0);
void Set_Base(char Base);
void GPIO_Config(void);
void DMA_Configuration(void);
void ADC_Configuration(void);
void Timer_Configuration(void);
void ADC_Stop(void);
void ADC_Start(void);
char USB_On(void);
char SD_Card_On(void);
void Delay_MS(u16 delay);
keycodes Wait_For_Key(void);
keycodes Get_Key(void);
u16 Get_Scan_Pos(void);
void Beep(beep_t freq_index, u16 duration);	// duration in mS

// library routines
void __USB_Istr(void);
void __CTR_HP(void);
u8 __MSD_WriteBlock(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
u8 __MSD_ReadBlock(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead);
void __SD_Set_Changed(void);
short __Get_Pixel(u16 x0, u16 y0);
void __Set_Color(u16 x0, u16 y0, u16 Color);
void __Erase_Color(u16 x0, u16 y0, u16 Color);
void __Set_Color(u16 x0, u16 y0, u16 Color);
void __Display_Char(u16 x0, u16 y0, u16 fg, u16 bg, const char c);
void __Display_Str(u16 x0, u16 y0, u16 fg, u16 bg, const char *s);
void __Display_Picture(const u8 *Pictp, u16 color, u16 x0, u16 y0, u16 dx, u16 dy);
void __Fill_Rectangle(u16 x0, u16 y0, u16 width, u16 height, u16 color);


#endif
/****************************** END OF FILE ***********************************/
