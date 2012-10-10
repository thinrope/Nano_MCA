/* ******************************************************************************* */
/*  File name  : ASM_Function.s  */
/* ******************************************************************************* */

/* NOTE: The changes in this file have not been duplicated in ASM_Function.s */
#ifdef __GNUC__
#define EXPORT .globl
.cpu cortex-m3
.syntax unified
.thumb
.text
#else
  RSEG CODE:CODE(2)
#endif


EXPORT __USB_Istr 			/* void __USB_Istr(void); */
EXPORT __CTR_HP				/* void __CTR_HP(void) */
EXPORT __MSD_WriteBlock		/* u8 __MSD_WriteBlock(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite) */
EXPORT __MSD_ReadBlock		/* u8 __MSD_ReadBlock(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead) */
EXPORT __SD_Set_Changed		/* void __SD_Set_Changed(void) */
EXPORT __Get_Pixel			/* short Get_Pixel(ushort x0, ushort y0) */
EXPORT __Set_Color			/* void Set_Color(ushort x0, ushort y0, ushort Color) */
EXPORT __Display_Char		/* void Display_Char(ushort x0, ushort y0, ushort fg, ushort bg, char c) */
EXPORT __Display_Str		/* void Display_Str(ushort x0, ushort y0, ushort fg, ushort bg, char *s) */
EXPORT __Display_Picture	/* void Display_Picture(const uchar *Pictp, ushort color, ushort x0, ushort y0, ushort dx, ushort dy) */
EXPORT __Fill_Rectangle		/* void Fill_Rectangle(u16 x0, u16 y0, u16 width, u16 height, u16 color) */
EXPORT __Beep			    /* void Beep(beep_t Freq, u16 Duration) */
EXPORT __Set_Orientation    /* void Set_Orientation(orient_t orientation) */
EXPORT __Get_Key			/* KeyCode Get_Key(void) */
EXPORT __Flash_Write		/* void Flash_Write(u16 *hwordp, u32 hword_adr, u16 sz) */
EXPORT __Flash_Read			/* void Flash_Read(u16 *hwordp, u32 hword_adr, u16 sz) */

.equiv GPIO,   0x4001
.equiv LCD_DATA_PORT,   0x1800   /* PORT_E */
.equiv LCD_Ctrl_PORT,   0x1400   /* PORT_D */
.equiv CRH,   0x00
.equiv CRL,   0x04
.equiv IDR,   0x08
.equiv ODR,   0x0C
.equiv OUT_H,   0x10
.equiv OUT_L,   0x14

.equiv LCD_RS,   0x02
.equiv LCD_WR,   0x20
.equiv LCD_RD,   0x10
.equiv LCD_RST,   0x01

#define RGB(_R,_G,_B) (((_R & 0x3E) >> 1) | ((_G & 0x3f) << 5) | ((_B & 0x3e) << 10))

.equiv C_GROUP,   0x1082
.equiv F_SELEC,   0x18E3
.equiv GRD_FLAG,   0x0040
.equiv LN1_FLAG,   0x0020
.equiv LN2_FLAG,   0x0800
.equiv LN1_COLOR, (RGB(32,63,32) & ~F_SELEC) | LN1_FLAG
.equiv GRD_COLOR, (RGB(32,32,32) & ~F_SELEC) | GRD_FLAG
.equiv LN2_COLOR, (RGB(63,63,63) & ~F_SELEC) | LN2_FLAG
.equiv WAV_FLAG,   0x0080
.equiv CH2_FLAG,   0x0002
.equiv REF_FLAG,   0x1000
.equiv WAV_COLOR, (RGB(0,63,63) & ~F_SELEC) | WAV_FLAG
.equiv CH2_COLOR, (RGB(63,63,0) & ~F_SELEC) | CH2_FLAG
.equiv REF_COLOR, (RGB(63,0,63) & ~F_SELEC) | REF_FLAG

/* ******************************************************************************* */
/*  Table of library functions start address, based on LIB memory map */
/* ******************************************************************************* */
.thumb_set __USB_Istr,        	0x08004110 /* void __USB_Istr(void); */
.thumb_set __CTR_HP,           	0x08004114 /* void __CTR_HP(void) */
.thumb_set __MSD_WriteBlock,  	0x08004118 /* u8 __MSD_WriteBlock(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite) */
.thumb_set __MSD_ReadBlock,   	0x0800411C /* u8 __MSD_ReadBlock(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead) */
.thumb_set __SD_Set_Changed,   	0x08004120 /* void __SD_Set_Changed(void) */
.thumb_set __Get_Pixel,        	0x08004124 /* short Get_Pixel(ushort x0, ushort y0) */
.thumb_set __Set_Color,        	0x08004126 /* void Set_Color(ushort x0, ushort y0, ushort Color) */
.thumb_set __Display_Char,     	0x0800412A /* void Display_Str(ushort x0, ushort y0, ushort fg, ushort bg, char c) */
.thumb_set __Display_Str,      	0x0800412E /* void Display_Str(ushort x0, ushort y0, ushort fg, ushort bg, char *s) */
.thumb_set __Display_Picture,  	0x08004132 /* void Display_Picture(const uchar *Pictp, ushort color, ushort x0, ushort y0, ushort dx, ushort dy) */
.thumb_set __Fill_Rectangle,   	0x08004136 /* void Fill_Rectangle(ushort x0, ushort y0, ushort width, ushort height, ushort color) */
.thumb_set __Beep,  			0x0800413A /* void Beep(beep_t Freq, u16 Duration) */
.thumb_set __Set_Orientation,  	0x0800413E /* void Set_Orientation(orient_t orientation) */
.thumb_set __Get_Key,			0x08004142 /* KeyCode Get_Key(void) */
.thumb_set __Flash_Write,		0x08004146 /* void Flash_Write(u16 *hwordp, u32 hword_adr, u16 sz) */
.thumb_set __Flash_Read,			0x0800414a /* void Flash_Read(u16 *hwordp, u32 hword_adr, u16 sz) */
#ifdef __GNUC__
  .end
#else
  END
#endif

/* ******************************* END OF FILE *********************************** */


