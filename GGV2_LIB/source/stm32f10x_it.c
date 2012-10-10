/*******************************************************************************
* File Name: stm32f10x_it.c
*******************************************************************************/
#include "stm32f10x_it.h"
#include "Lcd.h"
#include "usb_lib.h"
#include "usb_istr.h"
#include "HW_V1_Config.h"

extern orient orientation;

#define LANGUAGE EN
#if LANGAUGE == EN
const static char *msghfe = "Hard Fault Exception";
const static char *msgmme =  "Memory Management Exception";
const static char *msgbfe = "Bus Fault Exception";
const static char *msgufe = "Usage Fault Exception";
#endif

void NMIException(void)
{}

void HardFaultException(void)
{
	Display_Str(100, 20, RED, BKGND_COLOR, msghfe);
	while (1) {}
}

void MemManageException(void)
{
	Display_Str(100, 20, RED, BKGND_COLOR, msgmme);
	while (1) {}
}

void BusFaultException(void)
{
	Display_Str(100, 20, RED, BKGND_COLOR, msgbfe);
	while (1) {}
}
void UsageFaultException(void)
{
	Display_Str(100, 20, RED, BKGND_COLOR, msgufe);
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

void DMAChannel1_IRQHandler(void)
{}

void DMAChannel2_IRQHandler(void)
{}

void DMAChannel3_IRQHandler(void)
{}

void DMAChannel4_IRQHandler(void)
{}

void DMAChannel5_IRQHandler(void)
{}

void DMAChannel6_IRQHandler(void)
{}

void DMAChannel7_IRQHandler(void)
{}

void ADC_IRQHandler(void)
{}

void USB_HP_CAN_TX_IRQHandler(void)
{
	CTR_HP();
}

void USB_LP_CAN_RX0_IRQHandler(void)
{
	USB_Istr();
}

void CAN_RX1_IRQHandler(void)
{}

void CAN_SCE_IRQHandler(void)
{}

void EXTI9_5_IRQHandler(void)
{}

void TIM1_BRK_IRQHandler(void)
{}

void TIM1_UP_IRQHandler(void)
{}

void TIM1_TRG_COM_IRQHandler(void)
{}

void TIM1_CC_IRQHandler(void)
{}

void TIM2_IRQHandler(void)
{}

void TIM3_IRQHandler(void)
{
	static unsigned char CounterKey = 10;
	static unsigned char KeyCodeLast, KeyWaitCounter, m_count;
	unsigned char   KeyCode = 0;

	TIM3_SR = 0; // clear the interrupt flag

	if (BeepCounter > 0) {
		BeepCounter--;
	}
	if (DelayCounter > 0){
		DelayCounter--;
	}
	if (CounterKey > 0){
		CounterKey--;
	}

	if (BeepCounter <= 0) {
		// Turn off the beep signal
		TIM4_CR1 &= ~(0x0001);  //CEN=0, TIMER4 Enable
	}
	if (CounterKey <= 0){ // checking for key press every 20ms
		CounterKey = 10;

		if (KeyWaitCounter)
			KeyWaitCounter--;

		if(orientation == VERT){
			if (!(GPIOD_IDR & KEY_M)) KeyCode = KEYCODE_M;
			else if (!(GPIOA_IDR & KEY_B)) KeyCode = KEYCODE_B;
			else if (!(GPIOA_IDR & KEY_PLAY)) KeyCode = KEYCODE_PLAY;
			else if (!(GPIOA_IDR & KEY_UP)) KeyCode = KEYCODE_RIGHT;
			else if (!(GPIOD_IDR & KEY_DOWN)) KeyCode =  KEYCODE_LEFT;
			else if (!(GPIOA_IDR & KEY_LEFT)) KeyCode = KEYCODE_UP;
			else if (!(GPIOA_IDR & KEY_RIGHT)) KeyCode = KEYCODE_DOWN;
		} else {
			if (!(GPIOD_IDR & KEY_M)) KeyCode = KEYCODE_M;
			else if (!(GPIOA_IDR & KEY_B)) KeyCode = KEYCODE_B;
			else if (!(GPIOA_IDR & KEY_PLAY)) KeyCode = KEYCODE_PLAY;
			else if (!(GPIOA_IDR & KEY_UP)) KeyCode = KEYCODE_UP;
			else if (!(GPIOD_IDR & KEY_DOWN)) KeyCode = KEYCODE_DOWN;
			else if (!(GPIOA_IDR & KEY_LEFT)) KeyCode = KEYCODE_LEFT;
			else if (!(GPIOA_IDR & KEY_RIGHT)) KeyCode = KEYCODE_RIGHT;
		}
		if (KeyCode) {
			if (KeyCode == KeyCodeLast) {
				if (KeyWaitCounter == 0) {
					if (KeyCode >= KEYCODE_UP) {
						KeyWaitCounter = 2; // 40ms repeat interval
						KeyBuffer = KeyCode;
					} else if ((KeyCode == KEYCODE_M) && (m_count == 0)) {
						m_count++;
						KeyBuffer = KEYCODE_B;
					}
				}
			} else {
				KeyWaitCounter = 25;  // 500ms delay before repeat
				m_count = 0;
				KeyBuffer = KeyCode;
			}
		}
		KeyCodeLast = KeyCode;
	}
}

void TIM4_IRQHandler(void)
{}

void I2C1_EV_IRQHandler(void)
{}

void I2C1_ER_IRQHandler(void)
{}

void I2C2_EV_IRQHandler(void)
{}

void I2C2_ER_IRQHandler(void)
{}

void SPI1_IRQHandler(void)
{}

void SPI2_IRQHandler(void)
{}

void USART1_IRQHandler(void)
{}

void USART2_IRQHandler(void)
{}

void USART3_IRQHandler(void)
{}

void EXTI15_10_IRQHandler(void)
{}

void RTCAlarm_IRQHandler(void)
{}

void USBWakeUp_IRQHandler(void)
{}
/********************************* END OF FILE ********************************/
