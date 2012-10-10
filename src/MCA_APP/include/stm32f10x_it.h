/*******************************************************************************
File Name: stm32f10x_it.h
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

#ifndef __STM32F10x_IT_H
#define __STM32F10x_IT_H

#include "stm32f10x_lib.h"

void            NMIException(void);
void            HardFaultException(void);
void            MemManageException(void);
void            BusFaultException(void);
void            UsageFaultException(void);
void            DebugMonitor(void);
void            SVCHandler(void);
void            PendSVC(void);
void            SysTickHandler(void);
void            WWDG_IRQHandler(void);
void            PVD_IRQHandler(void);
void            TAMPER_IRQHandler(void);
void            RTC_IRQHandler(void);
void            FLASH_IRQHandler(void);
void            RCC_IRQHandler(void);
void            EXTI0_IRQHandler(void);
void            EXTI1_IRQHandler(void);
void            EXTI2_IRQHandler(void);
void            EXTI3_IRQHandler(void);
void            EXTI4_IRQHandler(void);
void            DMAChannel1_IRQHandler(void);
void            DMAChannel2_IRQHandler(void);
void            DMAChannel3_IRQHandler(void);
void            DMAChannel4_IRQHandler(void);
void            DMAChannel5_IRQHandler(void);
void            DMAChannel6_IRQHandler(void);
void            DMAChannel7_IRQHandler(void);
void            ADC_IRQHandler(void);
void            USB_HP_CAN_TX_IRQHandler(void);
void            USB_LP_CAN_RX0_IRQHandler(void);
void            CAN_RX1_IRQHandler(void);
void            CAN_SCE_IRQHandler(void);
void            EXTI9_5_IRQHandler(void);
void            TIM1_BRK_IRQHandler(void);
void            TIM1_UP_IRQHandler(void);
void            TIM1_TRG_COM_IRQHandler(void);
void            TIM1_CC_IRQHandler(void);
void            TIM2_IRQHandler(void);
void            TIM3_IRQHandler(void);
void            TIM4_IRQHandler(void);
void            I2C1_EV_IRQHandler(void);
void            I2C1_ER_IRQHandler(void);
void            I2C2_EV_IRQHandler(void);
void            I2C2_ER_IRQHandler(void);
void            SPI1_IRQHandler(void);
void            SPI2_IRQHandler(void);
void            USART1_IRQHandler(void);
void            USART2_IRQHandler(void);
void            USART3_IRQHandler(void);
void            EXTI15_10_IRQHandler(void);
void            RTCAlarm_IRQHandler(void);
void            USBWakeUp_IRQHandler(void);

#endif

/********************************* END OF FILE ********************************/
