/*******************************************************************************
* File Name: lcd.c
*******************************************************************************/

#include <string.h>
#include "stm32f10x_it.h"
#include "Lcd.h"
#include "HW_V1_Config.h"
#include "usb_lib.h"
#include "msd.h"

//#define GENERIC	// do not use lcd specific features

// display orientation
orient orientation;
u16	lcd_width, lcd_height;

void Display_U16(u16 x0, u16 y0, u16 fg, u16 bg, u16 num);
const char *hex_conv = "0123456789ABCDEF";
//'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F''}

#define LANGUAGE EN
#if LANGAUGE == EN
const static char *msgt = "TRUE ";
const static char *msgf = "FALSE";
#endif

#if LANGAUGE == EN
const u16 Char_Dot[760] =
{
   0xfff8,0x7FF0,0x3FE0,0x1FC0,0x0F80,0x0700,0x0200,0x0000,// " Play
   0x7FF0,0x7FF0,0x0000,0x0000,0x0000,0x7FF0,0x7FF0,0x0000,// # Pause
   0x0000,0x0000,0x0000,0xE000,0x1000,0x0800,0x0800,0x0800,// $
   0x0C30,0x0C60,0x00C0,0x0180,0x0300,0x0630,0x0C30,0x0000,// %
   0x0800,0x0800,0x1000,0xE000,0x0000,0x0000,0x0000,0x0000,// &
   0x2010,0x2010,0x2010,0x2010,0x2010,0x2010,0x3FF0,0x0000,// ' Empty bottom
   0x0000,0x0000,0x0FC0,0x1FE0,0x3030,0x2010,0x0000,0x0000,// (
   0x0000,0x0000,0x2010,0x3030,0x1FE0,0x0FC0,0x0000,0x0000,// )
   0x0000,0x18C0,0x0D80,0x0700,0x0700,0x0D80,0x18C0,0x0000,// *
   0x0000,0x0100,0x0100,0x07C0,0x07C0,0x0100,0x0100,0x0000,// +
   0x0000,0x0000,0x0000,0xFFFC,0x0000,0x0000,0x0000,0x0000,// ,
   0x0000,0x0100,0x0100,0x0100,0x0100,0x0100,0x0100,0x0000,// -
   0x0000,0x0000,0x0000,0x0030,0x0030,0x0000,0x0000,0x0000,// .
   0x0030,0x0060,0x00C0,0x0180,0x0300,0x0600,0x0C00,0x0000,// /
   0x1FE0,0x3FF0,0x2090,0x2310,0x2410,0x3FF0,0x1FE0,0x0000,// 0
   0x0000,0x0810,0x1810,0x3FF0,0x3FF0,0x0010,0x0010,0x0000,// 1
   0x1070,0x30F0,0x2190,0x2310,0x2610,0x3830,0x1830,0x0000,// 2
   0x1020,0x3030,0x2210,0x2210,0x2210,0x3FF0,0x1DE0,0x0000,// 3
   0x0300,0x0700,0x0D00,0x1910,0x3FF0,0x3FF0,0x0110,0x0000,// 4
   0x3E20,0x3E30,0x2210,0x2210,0x2310,0x23F0,0x21E0,0x0000,// 5
   0x0FE0,0x1FF0,0x3210,0x2210,0x2210,0x03F0,0x01E0,0x0000,// 6
   0x3000,0x3000,0x20F0,0x21F0,0x2300,0x3E00,0x3C00,0x0000,// 7
   0x1DE0,0x3FF0,0x2210,0x2210,0x2210,0x3FF0,0x1DE0,0x0000,// 8
   0x1C00,0x3E10,0x2210,0x2210,0x2230,0x3FE0,0x1FC0,0x0000,// 9
   0x0000,0x0000,0x0000,0x0C60,0x0C60,0x0000,0x0000,0x0000,// :
   0x0040,0x0040,0x0040,0x0040,0x0040,0x0040,0x0040,0x0040,// ;
   0x0000,0x0100,0x0380,0x06C0,0x0C60,0x1830,0x1010,0x0000,// <
   0x0240,0x0240,0x0240,0x0240,0x0240,0x0240,0x0240,0x0000,// =
   0x0000,0x1010,0x1830,0x0C60,0x06C0,0x0380,0x0100,0x0000,// >
   0x1800,0x3800,0x2000,0x23B0,0x27B0,0x3C00,0x1800,0x0000,// ?
   0x0FC0,0x1020,0x2790,0x2490,0x27D0,0x1050,0x0F80,0x0000,// @
   0x07F0,0x0FF0,0x1900,0x3100,0x1900,0x0FF0,0x07F0,0x0000,// A
   0x2010,0x3FF0,0x3FF0,0x2210,0x2210,0x3FF0,0x1DE0,0x0000,// B
   0x0FC0,0x1FE0,0x3030,0x2010,0x2010,0x3030,0x1860,0x0000,// C
   0x2010,0x3FF0,0x3FF0,0x2010,0x3030,0x1FE0,0x0FC0,0x0000,// D
   0x2010,0x3FF0,0x3FF0,0x2210,0x2710,0x3030,0x3870,0x0000,// E
   0x2010,0x3FF0,0x3FF0,0x2210,0x2700,0x3000,0x3800,0x0000,// F
   0x0FC0,0x1FE0,0x3030,0x2110,0x2110,0x31E0,0x19F0,0x0000,// G
   0x3FF0,0x3FF0,0x0200,0x0200,0x0200,0x3FF0,0x3FF0,0x0000,// H
   0x0000,0x0000,0x2010,0x3FF0,0x3FF0,0x2010,0x0000,0x0000,// I
   0x00E0,0x00F0,0x0010,0x2010,0x3FF0,0x3FE0,0x2000,0x0000,// J
   0x2010,0x3FF0,0x3FF0,0x0300,0x0F00,0x3CF0,0x3070,0x0000,// K
   0x2010,0x3FF0,0x3FF0,0x2010,0x0010,0x0030,0x0030,0x0000,// L
   0x3FF0,0x3FF0,0x1C00,0x0E00,0x1C00,0x3FF0,0x3FF0,0x0000,// M
   0x3FF0,0x3FF0,0x1C00,0x0E00,0x0700,0x3FF0,0x3FF0,0x0000,// N
   0x0FC0,0x1FE0,0x3030,0x2010,0x3030,0x1FE0,0x0FC0,0x0000,// O
   0x2010,0x3FF0,0x3FF0,0x2210,0x2200,0x3E00,0x1C00,0x0000,// P
   0x1FE0,0x3FF0,0x2010,0x2070,0x2030,0x3FF8,0x1FE8,0x0000,// Q
   0x2010,0x3FF0,0x3FF0,0x2200,0x2300,0x3FF0,0x1CF0,0x0000,// R
   0x1860,0x3C70,0x2610,0x2210,0x2310,0x39F0,0x18E0,0x0000,// S
   0x0000,0x3800,0x3010,0x3FF0,0x3FF0,0x3010,0x3800,0x0000,// T
   0x3FE0,0x3FF0,0x0010,0x0010,0x0010,0x3FF0,0x3FE0,0x0000,// U
   0x3F80,0x3FC0,0x0060,0x0030,0x0060,0x3FC0,0x3F80,0x0000,// V
   0x3FC0,0x3FF0,0x0070,0x01C0,0x0070,0x3FF0,0x3FC0,0x0000,// W
   0x3030,0x3CF0,0x0FC0,0x0300,0x0FC0,0x3CF0,0x3030,0x0000,// X
   0x0000,0x3C00,0x3E10,0x03F0,0x03F0,0x3E10,0x3C00,0x0000,// Y
   0x3870,0x30F0,0x2190,0x2310,0x2610,0x3C30,0x3870,0x0000,// Z
   0x0010,0x0030,0x0050,0x0190,0x0310,0x0C10,0x3FF0,0x0000,// [ Delta
   0x0800,0x0800,0x0800,0x0800,0x0800,0x0800,0x0800,0x0800,/* \ */
   0x0000,0x0000,0x0000,0xFC7C,0x0280,0x0280,0x0100,0x0100,// ]
   0x0200,0x0400,0x0C00,0x1FF0,0x3FF0,0x0C00,0x0400,0x0200,// ^ Rising edge
   0x0100,0x0080,0x00C0,0x3FE0,0x3FF0,0x00C0,0x0080,0x0100,// _ Falling edge
   0x2010,0x2010,0x2010,0x2010,0x2010,0x2010,0x2010,0x2010,// ` Empty body
   0x00E0,0x05F0,0x0510,0x0510,0x07E0,0x03F0,0x0010,0x0000,// a
   0x2010,0x3FF0,0x3FE0,0x0210,0x0210,0x03F0,0x01E0,0x0000,// b
   0x03E0,0x07F0,0x0410,0x0410,0x0410,0x0630,0x0220,0x0000,// c
   0x01E0,0x03F0,0x0610,0x2410,0x3FF0,0x3FF0,0x0010,0x0000,// d
   0x03E0,0x07F0,0x0510,0x0510,0x0510,0x0730,0x0320,0x0000,// e
   0x0000,0x0210,0x1FF0,0x3FF0,0x2210,0x3000,0x1800,0x0000,// f
   0x03C8,0x07EC,0x0424,0x0424,0x03FC,0x07F8,0x0400,0x0000,// g
   0x2010,0x3FF0,0x3FF0,0x0220,0x0400,0x07F0,0x03F0,0x0000,// h
   0x0000,0x0000,0x0410,0x37F0,0x37F0,0x0010,0x0000,0x0000,// i
   0x0000,0x0018,0x001C,0x0404,0x37FC,0x37F8,0x0400,0x0000,// j
   0x2010,0x3FF0,0x3FF0,0x2180,0x03C0,0x0670,0x0430,0x0000,// k
   0x0000,0x0000,0x2010,0x3FF0,0x3FF0,0x0010,0x0000,0x0000,// l
   0x07F0,0x07F0,0x0600,0x03F0,0x0600,0x07F0,0x07F0,0x0000,// m
   0x0400,0x07F0,0x03F0,0x0400,0x0400,0x07F0,0x03F0,0x0000,// n
   0x03E0,0x07F0,0x0410,0x0410,0x0410,0x07F0,0x03E0,0x0000,// o
   0x0404,0x07FC,0x03FC,0x0444,0x0440,0x07C0,0x0380,0x0000,// p
   0x0380,0x07C0,0x0440,0x0444,0x07FC,0x03FC,0x0404,0x0000,// q
   0x0410,0x07F0,0x03F0,0x0610,0x0400,0x0600,0x0300,0x0000,// r
   0x0000,0x0220,0x0730,0x0590,0x0490,0x06F0,0x0260,0x0000,// s
   0x0400,0x0400,0x1FE0,0x3FF0,0x0410,0x0430,0x0020,0x0000,// t
   0x07E0,0x07F0,0x0010,0x0010,0x07E0,0x07F0,0x0010,0x0000,// u
   0x0000,0x07C0,0x07E0,0x0030,0x0030,0x07E0,0x07C0,0x0000,// v
   0x07E0,0x07F0,0x0030,0x00E0,0x0030,0x07F0,0x07E0,0x0000,// w
   0x0410,0x0630,0x03E0,0x01C0,0x03E0,0x0630,0x0410,0x0000,// x
   0x0780,0x07C4,0x0044,0x0044,0x0044,0x07F8,0x07F0,0x0000,// y
   0x0630,0x0670,0x04D0,0x0590,0x0710,0x0630,0x0430,0x0000,// z
   0x0780,0x0480,0x3CF0,0x2010,0x2FD0,0x2FD0,0x2FD0,0x2FD0,// { Battery head
   0x2FD0,0x2FD0,0x2FD0,0x2FD0,0x2FD0,0x2FD0,0x2FD0,0x2FD0,// | Battery body
   0x2FD0,0x2FD0,0x2FD0,0x2FD0,0x2FD0,0x2010,0x3FF0,0x0000,// } Battery bottom
   0x0780,0x0480,0x3CF0,0x2010,0x2010,0x2010,0x2010,0x2010 // ~ Empty head
};
#endif

 // pixels
u16 Get_Font_8x14(u8 chr, u8 row)
{
  u16 idx = (chr - 0x22) * 8;

  return Char_Dot[idx + row];
}


// NOTE: (0,0) is top left corner of screen if portrait or bottom left if landscape
/*******************************************************************************
 Set_Orientation: Sets orientation to landscape or portrait
*******************************************************************************/
void Set_Orientation(orient ori)
{
	orientation = ori;
	if(orientation == VERT){
		lcd_width = 240;
		lcd_height = 320;
	} else {
		lcd_width = 320;
		lcd_height = 240;
	}
}

/*******************************************************************************
 Lcd_Wrt_Reg: Set LCD Register  Input: Register addr., Data
*******************************************************************************/
void Lcd_Wrt_Reg(u16 Reg, u16 Data)
{
  LDC_DATA_OUT=Reg;     //Reg. Addr.
  LCD_RS_LOW();         //RS=0,Piont to Index Reg.
  LCD_nWR_ACT();        //WR Cycle from 1 -> 0 -> 1
  LCD_RS_HIGH();        //RS=1,Piont to object Reg.
  LDC_DATA_OUT=Data;    //Reg. Data
  LCD_nWR_ACT();        //WR Cycle from 1 -> 0 -> 1
}
/*******************************************************************************
Function Name : LCD_SET_WINDOW
Description : use  (x1,y1) and (x2,y2) to set a rectangle  area
Para :  (x1,y1) and (x2,y2)
(0,0) is always lower left
*******************************************************************************/
void    Lcd_Set_Window(u16 x1, u16 x2, u16 y1, u16 y2)
{
	u16 tmp1, tmp2;

	if(orientation == VERT){
		tmp1 = x1;
		tmp2 = x2;
		x1 = 319 - y2;
		x2 = 319 - y1;
		y1 = tmp1;
		y2 = tmp2;
	}
	/*
	if(x1 < 0) x1 = 0;
	if(x2 < 0) x2 = 0;
	if(y1 < 0) y1 = 0;
	if(y2 < 0) y2 = 0;
	if(x1 > x2){
		tmp1 = x1;
		x1 = x2;
		x2 = tmp1;
	}
	if(y1 > y2){
		tmp1 = y1;
		y1 = y2;
		y2 = tmp1;
	}
	*/
	Lcd_Wrt_Reg(0x0050, y1);
	Lcd_Wrt_Reg(0x0051, y2);
	Lcd_Wrt_Reg(0x0052, x1);
	Lcd_Wrt_Reg(0x0053, x2);
	Lcd_Wrt_Reg(0x0020, y1);
	Lcd_Wrt_Reg(0x0021, x1);

	LDC_DATA_OUT = 0x0022; // Reg. Addr.
	LCD_RS_LOW(); // RS=0,Piont to Index Reg.
	LCD_nWR_ACT(); // WR Cycle from 1 -> 0 -> 1
	LCD_nWR_ACT(); // WR Cycle from 1 -> 0 -> 1
	LCD_RS_HIGH(); // RS=1,Piont to object Reg.
}
/*******************************************************************************
 LCD  initializtion
*******************************************************************************/
void LCD_Initial(void)
{
  //------------------------- Reset LCD Driver ---------------------//
  LCD_DATA_BUS_OUT();   //Set LCD Data Bus as Output Mode
  LCD_nWR_HIGH();
  LCD_nRD_HIGH();
  LCD_RS_HIGH();        //RS=1
  LCD_nRST_HIGH();
  Delay_MS(1);           //RST=1, Delay 1ms
  LCD_nRST_LOW();
  Delay_MS(1);          //RST=0 Reset LCD, Delay 1ms
  LCD_nRST_HIGH();
  Delay_MS(5);          //RST=1, Delay 5 ms
 //----------------ST7781 Internal Register Initial------------//
  Lcd_Wrt_Reg(0x00FF, 0x0001);
  Lcd_Wrt_Reg(0x00F3, 0x0008);
  LDC_DATA_OUT=0x00F3;
  LCD_RS_LOW();
  LCD_nWR_ACT();       //Read Parameter
  LCD_RS_HIGH();
 //-------------------Display Control Setting-----------------//
  Lcd_Wrt_Reg(0x0001, 0x0100);    //Output Direct
  Lcd_Wrt_Reg(0x0002, 0x0700);    //Line Inversion
  Lcd_Wrt_Reg(0x0003, 0x0030);    //BGR=0,ID=11
  Lcd_Wrt_Reg(0x0008, 0x0302);    //Porch Setting
  Lcd_Wrt_Reg(0x0009, 0x0000);    //Scan Cycle
  Lcd_Wrt_Reg(0x000A, 0x0000);    //FMARK off
 //----------------power Control Register Initial------------//
  Lcd_Wrt_Reg(0x0010, 0x0790);    //Power Control1
  Lcd_Wrt_Reg(0x0011, 0x0005);    //Power Control2
  Lcd_Wrt_Reg(0x0012, 0x0000);    //Power Control3
  Lcd_Wrt_Reg(0x0013, 0x0000);    //Power Control4
  Delay_MS(100);                  //Delay 100ms
  Lcd_Wrt_Reg(0x0010, 0x12B0);    //Power Control1
  Delay_MS(50);                   //delay 50ms
  Lcd_Wrt_Reg(0x0011, 0x0007);    //Power Control2
  Delay_MS(50);                   //delay 50ms
  Lcd_Wrt_Reg(0x0012, 0x008C);    //Power Control3
  Lcd_Wrt_Reg(0x0013, 0x1700);    //Power Control4
  Lcd_Wrt_Reg(0x0029, 0x001A);    //VCOMH Setting
  Delay_MS(50);                   //delay 50ms
 //------------------GAMMA Cluster Setting-------------------//
  Lcd_Wrt_Reg(0x0030, 0x0000);
  Lcd_Wrt_Reg(0x0031, 0x0507);
  Lcd_Wrt_Reg(0x0032, 0x0003);
  Lcd_Wrt_Reg(0x0035, 0x0200);
  Lcd_Wrt_Reg(0x0036, 0x0106);
  Lcd_Wrt_Reg(0x0037, 0x0000);
  Lcd_Wrt_Reg(0x0038, 0x0507);
  Lcd_Wrt_Reg(0x0039, 0x0104);
  Lcd_Wrt_Reg(0x003C, 0x0200);
  Lcd_Wrt_Reg(0x003D, 0x0005);
 //------------------Display Windows 240*320-----------------//
  Lcd_Wrt_Reg(0x0050, 0x0000);
  Lcd_Wrt_Reg(0x0051, 0x00EF);
  Lcd_Wrt_Reg(0x0052, 0x0000);
  Lcd_Wrt_Reg(0x0053, 0x013F);
 //--------------------Frame Rate Setting--------------------//
  Lcd_Wrt_Reg(0x0060, 0xA700);
  Lcd_Wrt_Reg(0x0061, 0x0001);
  Lcd_Wrt_Reg(0x0090, 0x0033);
  Lcd_Wrt_Reg(0x0007, 0x0133);
  Delay_MS(50);                   //delay 50ms
}

/*******************************************************************************
Point_SCR: Set display position   Input: X, Y
*******************************************************************************/

void Point_SCR(u16 x0, u16 y0)
{
	if(orientation == VERT){
		Lcd_Wrt_Reg(0x0020,x0);
		Lcd_Wrt_Reg(0x0021,lcd_height - y0);
	} else {
		Lcd_Wrt_Reg(0x0020,y0);
		Lcd_Wrt_Reg(0x0021,x0);
	}
	LDC_DATA_OUT=0x0022;  //DRAM Reg.
	LCD_RS_LOW();
	LCD_nWR_ACT();        //WR Cycle from 1 -> 0 -> 1
	LCD_nWR_ACT();        //WR Cycle from 1 -> 0 -> 1
	LCD_RS_HIGH();
}
/*******************************************************************************
 Set_Pixel: Set a Pixel  Input: Color
*******************************************************************************/
void Set_Pixel_Fast(u16 Color)
{
  LDC_DATA_OUT=Color;   //Color Data
  LCD_nWR_ACT();        //WR Cycle from 1 -> 0 -> 1
}
/*******************************************************************************
 Set_Pixel: Set a Pixel  Input: Color
*******************************************************************************/
static inline void Set_Pixel(u16 x0, u16 y0, u16 Color)
{
	if(orientation == VERT){
		Lcd_Wrt_Reg(0x0020,x0);
		Lcd_Wrt_Reg(0x0021, 319 - y0);
	} else {
		Lcd_Wrt_Reg(0x0020,y0);
		Lcd_Wrt_Reg(0x0021,x0);
	}
	LDC_DATA_OUT=0x0022;  //DRAM Reg.
	LCD_RS_LOW();
	LCD_nWR_ACT();        //WR Cycle from 1 -> 0 -> 1
	LCD_nWR_ACT();        //WR Cycle from 1 -> 0 -> 1
	LCD_RS_HIGH();
	LDC_DATA_OUT=Color;   //Color Data
	LCD_nWR_ACT();        //WR Cycle from 1 -> 0 -> 1
}

// Wrapper for direct calls from application
void Set_Color(u16 x0, u16 y0, u16 Color)
{
	Set_Pixel(x0,y0,Color);
}
/***************************************************************************
Function Name : Fill_Rectangle
Description :
Para:
	x0, y0 = starting point
	width, height = size of rectangle
	color = color of rectangle
*****************************************************************************/
void Fill_Rectangle(u16 x0, u16 y0, u16 width, u16 height, u16 color)
{
	u16 tmp;
	int i, area;
	u16 x1, y1;
	register u16 x, y;

	y1 = y0 + height - 1;
	x1 = x0 + width - 1;

#ifdef GENERIC
	for(y = y0; y <= y1; y++){
		for(x = x0; x <= x1; x++){
			Set_Pixel(x, y, color);	// Set_Pixel takes care of orientation
		}
	}
#else
	Lcd_Set_Window(x0, x1, y0, y1);	// This takes care of orientation
	area = (int) width * (int) height;
	for (i = 0; i < area; i++){ // fill in all the pixels
		Set_Pixel_Fast(color);
	}
	Lcd_Set_Window(0, lcd_width - 1, 0, lcd_height - 1); // restore full screen
#endif
	//Display_U16(0, 3 * CHAR_HEIGHT, WHITE, BLACK, x0);
	//Display_U16(0, 2 * CHAR_HEIGHT, WHITE, BLACK, y0);
	//Display_U16(0, 1 * CHAR_HEIGHT, WHITE, BLACK, x1);
	//Display_U16(0, 0, WHITE, BLACK, y1);
}

/*******************************************************************************
Function Name : Clear_Screen
Description : clear screen
 *******************************************************************************/

void    Clear_Screen(u16 bg)
{
	Fill_Rectangle(0,0, lcd_width, lcd_height, bg);
}

/*******************************************************************************
Function Name : Unsigned_To_Dec_Str, Signed_To_Dec_Str
Description : print one decimal integer in the specific  position in a field
 of INT_WIDTH characters, right justified, with sign.
Para :
	buf: pointer to string buffer
	width:  number of characters to be displayed, not including null terminator
	num: number to be displayed, cast to u32 or s32
*******************************************************************************/

void Unsigned_To_Hex_Str(char *buf, u8 width, u32 num){
	char i, n;

	for(n = 0; n < width; n++)
		buf[n] = ' ';
	n = width - 1;  // start index at far right of field
	i = 0;
	do {  // isolate each decimal digit
		buf[n--] = hex_conv[(num & 0xF)];  // convert to text
		num >>= 4;
	} while (num > 0  && n >= 0);
}

void Unsigned_To_Dec_Str(char *buf, u8 width, u32 num){
	char i, n;

	for(n = 0; n < width; n++)
		buf[n] = ' ';
	n = width - 1;  // start index at far right of field
	i = 0;
	do {  // isolate each decimal digit
		buf[n--] = hex_conv[(num % 10)];  // convert to text
		num /= 10;
	} while (num > 0  && n >= 0);
}

void Signed_To_Dec_Str(char *buf, u8 width, s32 num){
	char i, n;
	bool neg_flg;

	for(n = 0; n < width; n++)
		buf[n] = ' ';
	if(num < 0){
		neg_flg = TRUE;
		num = -num;
	} else
		neg_flg = FALSE;
	buf[n] = 0;
	n = width - 1;  // start index at far right of field
	i = 0;
	do {  // isolate each decimal digit
		buf[n--] = hex_conv[(num % 10)];  // convert to text
		num /= 10;
	} while (num > 0  && n >= 1);
	if(neg_flg)
		buf[n] = '-';
}

void Bool_To_Str(char *buf, u8 width, bool val){
	if(val == TRUE){
		strcpy(buf, msgt);
	} else {
		strcpy(buf, msgf);
	}
}
/*******************************************************************************
Function Name : Display_Char
Description : print one character in the specific  position
Para : (x0,y0) - coordinate of the start point of the string
       fg = text color
		bg = background color
       Mode =  Normal replace Display,INV Inverse replace Display
       c = character to be displayed
WARNING: mode is ignored for now
*******************************************************************************/

void Display_Char(u16 x0, u16 y0, u16 fg, u16 bg, u8 c)
{
	short     i, j, b;		// can not use char as unsigned
	b = 0;
	for (i =  0; i < FONT_WIDTH ; i++) // for each column
	{
		if (c >= 0x22)
			b = Get_Font_8x14(c, i); // prototype calls i "row", but it is actually column
		for (j = 0; j < FONT_HEIGHT; j++)       // for each row
		{
			if (b & 4)
				Set_Pixel(x0 + i, y0 + j, fg);	// Set_Pixel takes care of orientation
			else
				Set_Pixel(x0 + i, y0 + j, bg);	// Set_Pixel takes care of orientation
			b >>= 1;
		}
	}
}
/*******************************************************************************
Function Name : Display_Str
Description : print one string in the specific  position
Para : (x0,y0) - coordinate of the start point of the string
        fg = text color
		bg = background color
       Mode = PRN Normal replace Display,INV Inverse replace Display
       s = pointer to text string
*******************************************************************************/

void Display_Str(u16 x0, u16 y0, u16 fg, u16 bg, const char *s)
{

	while (*s)
	{
		Display_Char(x0, y0, fg, bg, *s);
		x0 += CHAR_WIDTH;
		++s; // process next character
	}
	return;
}

/*******************************************************************************
 Display_Picture: Display Picture
 Input:
 x,y location of upper left corner
 dx, dy size of logo
 Location (x,y) is pixel address in lcd, dx and dy are size, (0,0) top left corner
 y and dy are in multiples of 8 only
*******************************************************************************/

void Display_Picture(const u8 *Pictp, u16 color, u16 x0, u16 y0, u16 dx, u16 dy){
	short x, y, k;
	u8 msk, col_of_8;

	// draw the picture
	for(y = dy - 1; y >= 0; y -= 8)	// each byte is 8 vertical pixels
	{
		for(x = dx - 1; x >= 0; x--)         // Loop through character byte data
		{
			msk = 0x80;
			col_of_8 = Pictp[x + (y >> 3) * dx];
			for(k = 7; k >= 0; k--)             // Loop through the vertical pixels
			{
				if((col_of_8 & msk) != 0 ) // Check if the pixel should be set
				{
					Set_Pixel(x0 + x, y0 + y + k, color);	// Set_Pixel takes care of orientation
				}
				msk = msk >> 1;
			}
		}
	}
}

/*******************************************************************************
Function Name : Display_u8
Description : print one decimal integer in the specific  position in a field
 of INT_WIDTH characters, right justified, with sign.
Para :
	x0, y0: lower left corner
	color: color of rectangle

	num: number to be displayed, right justified in fixed field
*******************************************************************************/

void Display_U8(u16 x0, u16 y0, u16 fg, u16 bg, u8 num)
{
	char  buf[U8_DIGITS + 1];

	Unsigned_To_Dec_Str(buf, U8_DIGITS, num);
	Display_Str(x0, y0, fg, bg, buf);
}


/*******************************************************************************
Function Name : Display_s8
Description : print one decimal integer in the specific  position in a field
 of INT_WIDTH characters, right justified, with sign.
Para :
	x0, y0: lower left corner
	color: color of rectangle
	num: number to be displayed, right justified in fixed field
*******************************************************************************/


void Display_S8(u16 x0, u16 y0, u16 fg, u16 bg, s8 num)
{
	char  buf[S8_DIGITS + 1];

	Signed_To_Dec_Str(buf, S8_DIGITS, num);
	Display_Str(x0, y0, fg, bg, buf);
}
/*******************************************************************************
Function Name : Display_u16
Description : print one decimal integer in the specific  position in a field
 of INT_WIDTH characters, right justified, with sign.
Para :
	x0, y0: lower left corner
	color: color of rectangle
	num: number to be displayed, right justified in fixed field
*******************************************************************************/
void Display_U16(u16 x0, u16 y0, u16 fg, u16 bg, u16 num)
{
	char  buf[S16_DIGITS + 1];

	Unsigned_To_Dec_Str(buf, S16_DIGITS, num);
	Display_Str(x0, y0, fg, bg, buf);
}
/*******************************************************************************
Function Name : Display_s16
Description : print one decimal integer in the specific  position in a field
 of INT_WIDTH characters, right justified, with sign.
Para :
	x0, y0: lower left corner
	color: color of rectangle
	mode: PRN = replace color, ERASE = remove color but leave other colors,
		 ADD = add color to existing colors
	num: number to be displayed, right justified in fixed field
*******************************************************************************/
void Display_S16(u16 x0, u16 y0, u16 fg, u16 bg, s16 num)
{

	char  buf[S16_DIGITS + 1];

	Signed_To_Dec_Str(buf, S16_DIGITS, num);
	Display_Str(x0, y0, fg, bg, buf);
}
/*******************************************************************************
Function Name : Display_U32
Description : print one decimal integer in the specific  position in a field
 of INT_WIDTH characters, right justified, with sign.
Para :
	x0, y0: lower left corner
	color: color of rectangle
	mode: PRN = replace color, ERASE = remove color but leave other colors,
		 ADD = add color to existing colors
	num: number to be displayed, right justified in fixed field
*******************************************************************************/
void Display_U32(u16 x0, u16 y0, u16 fg, u16 bg, u32 num)
{
	char  buf[S32_DIGITS + 1];
	char i, n;

	Unsigned_To_Dec_Str(buf, S32_DIGITS, num);
	Display_Str(x0, y0, fg, bg, buf);
}
/*******************************************************************************
Function Name : Display_S32
Description : print one decimal integer in the specific  position in a field
 of INT_WIDTH characters, right justified, with sign.
Para :
	x0, y0: lower left corner
	color: color of rectangle
	mode: PRN = replace color, ERASE = remove color but leave other colors,
		 ADD = add color to existing colors
	num: number to be displayed, right justified in fixed field
*******************************************************************************/
void Display_S32(u16 x0, u16 y0, u16 fg, u16 bg, int num)
{
	char  buf[S32_DIGITS + 1];

	Signed_To_Dec_Str(buf, S32_DIGITS, num);
	Display_Str(x0, y0, fg, bg, buf);
}
/*******************************************************************************
Function Name : Display_bool
Description : print TRUE or FALSE
Para :
	x0, y0: lower left corner
	color: color of rectangle
	mode: PRN = replace color, ERASE = remove color but leave other colors,
		 ADD = add color to existing colors
	val: boolean to be displayed as TRUE or FALSE
*******************************************************************************/
void Display_Bool(u16 x0, u16 y0, u16 fg, u16 bg, bool val)
{
	char buf[10];

	Bool_To_Str(buf, BOOL_CHARS, val);
	Display_Str(x0, y0, fg, bg, buf);
}

/*******************************************************************************
Function Name : Display_Info
Description : Prints text and value
Para :
	Pre: prefix text
	Num: long number to be printed
*******************************************************************************/
static s16 msgy = 240-16;
static u16 ln = 0;
void Display_Info(char *Pre, u32 Num)
{
	char  buf[HEX_DIGITS + 1], n;
	bool neg = FALSE;

	u16 x0 = 0;

	//if(ln <500){
		Fill_Rectangle(0, msgy, HORZ_WIDTH, 16, BKGND_COLOR);
		Display_U16(x0, msgy, TXT_COLOR, BKGND_COLOR, ln++);
		if (Pre) {
			char *p = Pre;
			while (*p) p++;
			x0 += 64;
			Display_Str(x0, msgy, TXT_COLOR, BKGND_COLOR, (unsigned char const *)Pre);
			x0 += (p - Pre) * 8 + 8;
		}
		Unsigned_To_Hex_Str(buf, HEX_DIGITS, Num);
		buf[HEX_DIGITS] = 0;
		Display_Str(x0, msgy, TXT_COLOR, BKGND_COLOR, buf);
		msgy -= 16;
		if(msgy < 0) msgy = 240 - 16;
	//}
	//else {
	//	while(1);
	//}
}

/******************************** END OF FILE *********************************/
