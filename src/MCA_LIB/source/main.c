/*******************************************************************************
* File Name: main.c
*******************************************************************************/
#include "msd.h"
#include "Lcd.h"
#include "stm32f10x_lib.h"
#include "usb_lib.h"
#include "HW_V1_Config.h"


typedef  void (*pFunction)(void);
const u32 __APP_VECTORS = 0x0800C000;

sMSD_CSD MSD_csd;
u32 Mass_Block_Size;
u32 Mass_Block_Count;

u16 boot_msg_y = 240;	// used during boot to display hardware detected

extern u8 Data_Buffer[];
extern u16 lcd_width, lcd_height;
// Pictures are stored as a row of 8 verticle pixels, repeated with next lower row

// Nuclear Trefoil
const u8 small_trelfoil_bits[] = {
	0x00, 0x00, 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xF0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xE0, 0xF8, 0xF8, 0xF0, 0xF0, 0xE0, 0xC0, 0x80, 0x00, 0x00, 0x00,
	0xE0, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3C, 0x10, 0x80, 0xC0, 0xC0, 0xC0, 0x88, 0x1E, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFC, 0xF0,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xC0, 0xF1, 0xF3, 0xE3, 0xF3, 0xF1, 0xE0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x38, 0x7C, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0x3E, 0x1C, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
#define SMALL_TREFOIL_WIDTH 32
#define SMALL_TREFOIL_HEIGHT 32

#define LANGUAGE EN
#if LANGAUGE == EN
							//   012345678901234567890123456789
const static char *msg_id = 	    "    Gamma Grapher Version 2";
const static char *msg_devel = 	"     Multichannel Analyzer";
const static char *msg_team = 	"        By The GCE Team";
const static char *msg_libver =  "  LIB Revision 0.11 (2012/9/7)";
// ALSO CHANGE REVISION IN scsi_data.c !!!!

const static char *msg_noapp = 	"Error: No application found";
#define STRT_MSG_WIDTH	(20 * CHAR_WIDTH)
#define STRT_MSG_X ((320 / 2) - (STRT_MSG_WIDTH / 2))	// always horizontal to start
#endif

void main(void)
{
	void *app_stack;
	pFunction app_reset;
	u16 x, y;

	/*--------------initialization-----------*/
	Set_Orientation (HORZ);
	Set_System();
	NVIC_Configuration();
	GPIO_Config();
	//DMA_Configuration();
	ADC_Configuration();
	Timer_Configuration();
	LCD_Initial();
	Clear_Screen( BKGND_COLOR );
	USB_Init();
	MSD_Init();
	Get_Medium_Characteristics();
	MSD_ReadBlock(Data_Buffer, 0, 512);
	MSD_GoIdleState();
	/*----------Power ON Information----------*/
	// start up messages span no more 30 char to fit both orientations
	//if(SD_Card_ON()){
	//	Display_Info("SD Flash: block size", Mass_Block_Size);
	//	Display_Info("SD Flash: block count", Mass_Block_Count);
	//	Display_Info("SD Flash: CSD Struct", MSD_csd.CSDStruct);
	//} else {
	//	Display_Str(8, (boot_msg_y -= 16), TXT_COLOR, BKGND_COLOR, "SD Flash: not present");
	//}
	if(USB_ON()){
		Display_Str(8, (boot_msg_y -= 16), TXT_COLOR, BKGND_COLOR, "Disconnect USB before");
		Display_Str(8, (boot_msg_y -= 16), TXT_COLOR, BKGND_COLOR, "turning on device.");
		while(1);
	}
	//else {
	//	Display_Str(8, (boot_msg_y -= 16), TXT_COLOR, BKGND_COLOR, "USB: not connected");
	//}
	//Delay_MS(5000);
	// start up messages span entire 30 char available for both orientations
	/*----------Revision Information----------*/
	Clear_Screen( BKGND_COLOR );
	Display_Str(0, 176, TXT_COLOR, BKGND_COLOR, msg_id);
	Display_Str(0, 160, TXT_COLOR, BKGND_COLOR, msg_devel);
	Display_Str(0, 144, TXT_COLOR, BKGND_COLOR, msg_team);
	Display_Str(0, 128, TXT_COLOR, BKGND_COLOR, msg_libver);
	x = 120 - (SMALL_TREFOIL_WIDTH / 2);
	y = 8;
	Display_Picture(small_trelfoil_bits, YEL, x, y,
	                  SMALL_TREFOIL_WIDTH, SMALL_TREFOIL_HEIGHT );
	y = lcd_height - (y + SMALL_TREFOIL_HEIGHT);
	Display_Picture(small_trelfoil_bits, YEL,x, y,
	                  SMALL_TREFOIL_WIDTH, SMALL_TREFOIL_HEIGHT );
	//Delay_MS(500);
	app_stack = (void *) *(vu32 *)(__APP_VECTORS);
	app_reset = (pFunction) *(vu32 *)(__APP_VECTORS + 4);

	/* if stack pointer points to RAM this may be a valid vector table */
	if (((int) app_stack & 0xFFFE0000) == 0x20000000) {
		(*app_reset)();
	}

	/* No app found, just hang */
	Display_Str(100, 20, RED, BKGND_COLOR, msg_noapp);
	while (1) {};

}
/********************************* END OF FILE ********************************/
