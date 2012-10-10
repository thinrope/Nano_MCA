/* ******************************************************************************* */
/*  File name  : ASM_Function.s  */
/*  File description: start addresses for exported functions */
/* ******************************************************************************* */

#ifdef __GNUC__
#define EXPORT .globl
#define EXTERN .extern
#define THUMBFUNC .thumb_func
#else
#define THUMBFUNC
#endif

	EXTERN USB_Istr
	EXTERN CTR_HP
	EXTERN MSD_WriteBlock
	EXTERN MSD_ReadBlock
	EXTERN SD_Set_Changed
	EXTERN Get_Pixel
	EXTERN Set_Color
	EXTERN Display_Char
	EXTERN Display_Str
	EXTERN Display_Picture
	EXTERN Fill_Rectangle
	EXTERN Beep
	EXTERN Set_Orientation
	EXTERN Get_Key
	EXTERN Flash_Write
	EXTERN Flash_Read
  EXPORT __BRANCH_TABLE


#ifdef __GNUC__
.cpu cortex-m3
.syntax unified
.thumb
.text
#else
  RSEG CODE:CODE(2)
#endif

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
/*  Table of library function start addresses */
/* ******************************************************************************* */

/* placeholder */
THUMBFUNC
__BRANCH_TABLE:
    B       Reset_Handler

/* void USB_Istr(void) */
THUMBFUNC
__USB_Istr:
    B       USB_Istr

/* void CTR_HP(void) */
THUMBFUNC
__CTR_HP:
    B       CTR_HP

/* u8 MSD_WriteBlock(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite) */
THUMBFUNC
__MSD_WriteBlock:
    B       MSD_WriteBlock

/* u8 MSD_ReadBlock(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead) */
THUMBFUNC
__MSD_ReadBlock:
    B       MSD_ReadBlock

/* void SD_Set_Changed(void); */
THUMBFUNC
__SD_Set_Changed:
    B       SD_Set_Changed

/* short Get_Pixel(ushort x0, ushort y0) */
THUMBFUNC
__Get_Pixel:
	B		Get_Pixel

/* void Set_Color(ushort x0, ushort y0, ushort Color) */
THUMBFUNC
__Set_Color:
	B		Set_Color

/* void Display_Char(ushort x0, ushort y0, ushort fg, ushort bg, char c) */
THUMBFUNC
__Display_Char:
	B       Display_Char

/* void Display_Str(ushort x0, ushort y0, ushort fg, ushort bg, char *s) */
THUMBFUNC
__Display_Str:
    B       Display_Str

/* void Display_Picture(const uchar *Pictp, ushort color, ushort x0, ushort y0, ushort dx, ushort dy) */
THUMBFUNC
__Display_Picture:
    B       Display_Picture

/* void Fill_Rectangle(ushort x0, ushort y0, ushort width, ushort height, ushort color) */
THUMBFUNC
__Fill_Rectangle:
    B       Fill_Rectangle

/* void Beep(beep_t Freq, u16 Duration)*/
THUMBFUNC
__Beep:
    B       Beep

/* void Set_Orientation(orient_t orientation)*/
THUMBFUNC
__Set_Orientation:
    B       Set_Orientation

/* Keycode Get_Key(void) */
THUMBFUNC
__Get_Key:
    B       Get_Key

/* void Flash_Write(u16 *hwordp, u32 hword_adr, u16 sz) */
THUMBFUNC
__Flash_Write:
    B       Flash_Write

/* void Flash_Read(u16 *hwordp, u32 hword_adr, u16 sz) */
THUMBFUNC
__Flash_Read:
    B       Flash_Read

/*******************************************************************************/
/* WARNING This code only supports horizontal orientation  */
.thumb_func
Add_Color:            /* void __Add_Color(unsigned short x,unsigned short y,unsigned short Color) */

    STMDB   SP!, {R4-R7}

    MOVW    R7,  #LCD_DATA_PORT
    MOVT    R7,  #GPIO
    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO
    MOVS    R3,  #LCD_RS
    MOVS    R4,  #LCD_WR

    MOVS    R5,  #0x20              /* Y_Addr REG.  */
    STR     R5,  [R7, #ODR]         /* [LCD_DATA] = Y_Addr REG. */
    STR     R3,  [R6, #OUT_L]       /* RS = 0 */
    STR     R4,  [R6, #OUT_L]       /* WR = 0 */

    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL */

    STR     R4,  [R6, #OUT_H]       /* WR = 1 */
    STR     R3,  [R6, #OUT_H]       /* RS = 1 */
    STR     R1,  [R7, #ODR]         /* [LCD_DATA] = Y_Addr */
    STR     R4,  [R6, #OUT_L]       /* WR = 0 */

    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL */

    STR     R4,  [R6, #OUT_H]       /* WR = 1 */

    MOVS    R5,  #0x21              /* X_Addr REG.  */
    STR     R5,  [R7, #ODR]         /* [LCD_DATA] = X_Addr REG. */
    STR     R3,  [R6, #OUT_L]       /* RS = 0 */
    STR     R4,  [R6, #OUT_L]       /* WR = 0 */

    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL */

    STR     R4,  [R6, #OUT_H]       /* WR = 1 */
    STR     R3,  [R6, #OUT_H]       /* RS = 1 */
    STR     R0,  [R7, #ODR]         /* [LCD_DATA] = X_Addr */
    STR     R4,  [R6, #OUT_L]       /* WR = 0 */

    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL */

    STR     R4,  [R6, #OUT_H]       /* WR = 1 */

    MOVS    R5,  #0x22              /* GRAM_Addr REG.  */
    STR     R5,  [R7, #ODR]         /* [LCD_DATA] = GRAM_Addr REG. */
    STR     R3,  [R6, #OUT_L]       /* RS = 0 */
    STR     R4,  [R6, #OUT_L]       /* WR = 0 */

    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL */

    STR     R4,  [R6, #OUT_H]       /* WR = 1 */
    STR     R3,  [R6, #OUT_H]       /* RS = 1 */

    MOVS    R5,  #0x44444444        /* INPUT MODE   */
    STR     R5,  [R7, #CRL]
    STR     R5,  [R7, #CRH]

    MOVS    R0,  #LCD_RD
    STR     R0,  [R6, #OUT_L]       /* RD=0 */

    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL  */
    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL  */
    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL  */
    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL  */

    STR     R0,  [R6, #OUT_H]       /* RD=1  */

    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL   */
    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL   */
    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL   */
    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL   */

    STR     R0,  [R6, #OUT_L]       /* RD = 0 */

    MOVW    R6,  #LCD_DATA_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL */
    MOVW    R6,  #LCD_DATA_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL */
    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL   */
    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL   */

    LDR     R3,  [R7, #IDR]         /* R3 = [LCD_DATA] */

    STR     R0,  [R6, #OUT_H]       /* RD = 1 */

    MOVS    R5,  #0x33333333        /* OUTPUT MODE   */
    STR     R5,  [R7, #CRL]
    STR     R5,  [R7, #CRH]

    MOVS    R0,  #LCD_RS
    MOVS    R5,  #0x20              /* Y_Addr REG.  */
    STR     R5,  [R7, #ODR]         /* [LCD_DATA] = Y_Addr REG. */
    STR     R0,  [R6, #OUT_L]       /* RS = 0 */
    STR     R4,  [R6, #OUT_L]       /* WR = 0 */

    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL */

    STR     R4,  [R6, #OUT_H]       /* WR = 1 */
    STR     R0,  [R6, #OUT_H]       /* RS = 1 */
    STR     R1,  [R7, #ODR]         /* [LCD_DATA] = Y_Addr */
    STR     R4,  [R6, #OUT_L]       /* WR = 0 */

    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL */

    STR     R4,  [R6, #OUT_H]       /* WR = 1 */

    MOVS    R5,  #0x22              /* GRAM_Addr REG.  */
    STR     R5,  [R7, #ODR]         /* [LCD_DATA] = GRAM_Addr REG. */
    STR     R0,  [R6, #OUT_L]       /* RS = 0 */
    STR     R4,  [R6, #OUT_L]       /* WR = 0 */

    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL */

    STR     R4,  [R6, #OUT_H]       /* WR = 1 */
    STR     R0,  [R6, #OUT_H]       /* RS = 1 */

    MOVW    R5,  #C_GROUP
    ANDS    R0,  R3,  R5
    BEQ     ADD_1                   /* If original color is not curve then add color */
    ANDS    R0,  R2,  R5
    BEQ     ADD_2                   /* If new color is not curve then keep color as is */
ADD_1:
    MOVW    R5,  #F_SELEC
    AND     R3,  R3,  R5            /* Erase color, keep flags */
    ORR     R3,  R3,  R2            /* Add new color */
    B       ADD_3
ADD_2:
    MOVW    R5,  #F_SELEC
    AND     R2,  R2,  R5            /* Erase new color, keep flags */
    ORR     R3,  R3,  R2            /* Add flag */
ADD_3:
    STR     R3,  [R7, #ODR]         /* [LCD_DATA] = R3 */
    STR     R4,  [R6, #OUT_L]       /* WR = 0 */

    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL */

    STR     R4,  [R6, #OUT_H]       /* WR = 1 */

    LDMIA   SP!, {R4-R7}
    BX      LR


/* ******************************************************************************* */
/* WARNING This code only supports horizontal orientation  */
.thumb_func
Erase_Color:            /* void __Erase_Color(unsigned short x,unsigned short y,unsigned short Color) */

    STMDB   SP!, {R4-R7}

    MOVW    R7,  #LCD_DATA_PORT
    MOVT    R7,  #GPIO
    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO
    MOVS    R3,  #LCD_RS
    MOVS    R4,  #LCD_WR

    MOVS    R5,  #0x20              /* Y_Addr REG.  */
    STR     R5,  [R7, #ODR]         /* [LCD_DATA] = Y_Addr REG. */
    STR     R3,  [R6, #OUT_L]       /* RS = 0 */
    STR     R4,  [R6, #OUT_L]       /* WR = 0 */

    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL */

    STR     R4,  [R6, #OUT_H]       /* WR = 1 */
    STR     R3,  [R6, #OUT_H]       /* RS = 1 */
    STR     R1,  [R7, #ODR]         /* [LCD_DATA] = Y_Addr */
    STR     R4,  [R6, #OUT_L]       /* WR = 0 */

    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL */

    STR     R4,  [R6, #OUT_H]       /* WR = 1 */

    MOVS    R5,  #0x21              /* X_Addr REG.  */
    STR     R5,  [R7, #ODR]         /* [LCD_DATA] = X_Addr REG. */
    STR     R3,  [R6, #OUT_L]       /* RS = 0 */
    STR     R4,  [R6, #OUT_L]       /* WR = 0 */

    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL */

    STR     R4,  [R6, #OUT_H]       /* WR = 1 */
    STR     R3,  [R6, #OUT_H]       /* RS = 1 */
    STR     R0,  [R7, #ODR]         /* [LCD_DATA] = X_Addr */
    STR     R4,  [R6, #OUT_L]       /* WR = 0 */

    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL */

    STR     R4,  [R6, #OUT_H]       /* WR = 1 */

    MOVS    R5,  #0x22              /* GRAM_Addr REG.  */
    STR     R5,  [R7, #ODR]         /* [LCD_DATA] = GRAM_Addr REG. */
    STR     R3,  [R6, #OUT_L]       /* RS = 0 */
    STR     R4,  [R6, #OUT_L]       /* WR = 0 */

    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL */

    STR     R4,  [R6, #OUT_H]       /* WR = 1 */
    STR     R3,  [R6, #OUT_H]       /* RS = 1 */

    MOVS    R5,  #0x44444444        /* INPUT MODE   */
    STR     R5,  [R7, #CRL]
    STR     R5,  [R7, #CRH]

    MOVS    R0,  #LCD_RD
    STR     R0,  [R6, #OUT_L]       /* RD=0 */

    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL  */
    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL  */
    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL  */
    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL  */

    STR     R0,  [R6, #OUT_H]       /* RD=1  */

    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL   */
    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL   */
    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL   */
    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL   */

    STR     R0,  [R6, #OUT_L]       /* RD = 0 */

    MOVW    R6,  #LCD_DATA_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL */
    MOVW    R6,  #LCD_DATA_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL */
    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL   */
    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL   */

    LDR     R3,  [R7, #IDR]         /* R3 = [LCD_DATA] */

    STR     R0,  [R6, #OUT_H]       /* RD = 1 */

    MOVS    R5,  #0x33333333        /* OUTPUT MODE   */
    STR     R5,  [R7, #CRL]
    STR     R5,  [R7, #CRH]

    MOVS    R0,  #LCD_RS
    MOVS    R5,  #0x20              /* Y_Addr REG.  */
    STR     R5,  [R7, #ODR]         /* [LCD_DATA] = Y_Addr REG. */
    STR     R0,  [R6, #OUT_L]       /* RS = 0 */
    STR     R4,  [R6, #OUT_L]       /* WR = 0 */

    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL */

    STR     R4,  [R6, #OUT_H]       /* WR = 1 */
    STR     R0,  [R6, #OUT_H]       /* RS = 1 */
    STR     R1,  [R7, #ODR]         /* [LCD_DATA] = Y_Addr */
    STR     R4,  [R6, #OUT_L]       /* WR = 0 */

    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL */

    STR     R4,  [R6, #OUT_H]       /* WR = 1 */

    MOVS    R5,  #0x22              /* GRAM_Addr REG.  */
    STR     R5,  [R7, #ODR]         /* [LCD_DATA] = GRAM_Addr REG. */
    STR     R0,  [R6, #OUT_L]       /* RS = 0 */
    STR     R4,  [R6, #OUT_L]       /* WR = 0 */

    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL */

    STR     R4,  [R6, #OUT_H]       /* WR = 1 */
    STR     R0,  [R6, #OUT_H]       /* RS = 1 */

    MOVW    R5,  #F_SELEC
    AND     R0,  R3, R5             /* extract color flags from pixel */
    ANDS    R0,  R0, R2             /* check if color flag is in erase color */
    BEQ     ERASE_3                 /* color not present, nothing to erase */

    AND     R3,  R3,  R5            /* Extract flags for original color */
    AND     R2,  R2,  R5            /* Extract flags for erase color */
    BICS    R3,  R3,  R2            /* Remove erase color flag */
    BEQ     ERASE_3                 /* No bits left, restore background */

ERASE_CH1:
    MOVW    R5,  #WAV_FLAG
    ANDS    R5,  R5,  R3
    BEQ     ERASE_CH2
    MOVW    R5,  #WAV_COLOR
    ORR     R3,  R3,  R5            /* Restore ch1 color */
    B       ERASE_3
ERASE_CH2:
    MOVW    R5,  #CH2_FLAG
    ANDS    R5,  R5,  R3
    BEQ     ERASE_CH3
    MOVW    R5,  #CH2_COLOR
    ORR     R3,  R3,  R5            /* Restore ch2 color */
    B       ERASE_3
ERASE_CH3:
    MOVW    R5,  #REF_FLAG
    ANDS    R5,  R5,  R3
    BEQ     ERASE_LN1
    MOVW    R5,  #REF_COLOR
    ORR     R3,  R3,  R5            /* Restore ch3 color */
    B       ERASE_3
ERASE_LN1:
    MOVW    R5,  #LN1_FLAG
    ANDS    R5,  R5,  R3
    BEQ     ERASE_LN2
    MOVW    R5,  #LN1_COLOR
    ORR     R3,  R3,  R5            /* Restore ln1 color */
    B       ERASE_3
ERASE_LN2:
    MOVW    R5,  #LN2_FLAG
    ANDS    R5,  R5,  R3
    BEQ     ERASE_GRD
    MOVW    R5,  #LN2_COLOR
    ORR     R3,  R3,  R5            /* Restore ln2 color */
    B       ERASE_3
ERASE_GRD:
    MOVW    R5,  #GRD_FLAG
    ANDS    R5,  R5,  R3
    BEQ     ERASE_3                 /* restores background */
    MOVW    R5,  #GRD_COLOR
    ORR     R3,  R3,  R5            /* Restore grd color */
ERASE_3:
    STR     R3,  [R7, #ODR]         /* [LCD_DATA] = R3 */
    STR     R4,  [R6, #OUT_L]       /* WR = 0 */

    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL */

    STR     R4,  [R6, #OUT_H]       /* WR = 1 */

    LDMIA   SP!, {R4-R7}
    BX      LR

/* ******************************************************************************* */
.thumb_func
Get_Pixel:    /* unsigned short __Get_Pixel(unsigned short x0,unsigned short y0) */

    STMDB   SP!, {R4-R7}

    MOVW    R7,  #LCD_DATA_PORT
    MOVT    R7,  #GPIO
    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO
    MOVS    R3,  #LCD_RS
    MOVS    R4,  #LCD_WR

    MOVS    R5,  #0x20              /* Y_Addr REG.  */
    STR     R5,  [R7, #ODR]         /* [LCD_DATA] = Y_Addr REG. */
    STR     R3,  [R6, #OUT_L]       /* RS = 0 */
    STR     R4,  [R6, #OUT_L]       /* WR = 0 */

    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL */

    STR     R4,  [R6, #OUT_H]       /* WR = 1 */
    STR     R3,  [R6, #OUT_H]       /* RS = 1 */
    STR     R1,  [R7, #ODR]         /* [LCD_DATA] = Y_Addr */
    STR     R4,  [R6, #OUT_L]       /* WR = 0 */

    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL */

    STR     R4,  [R6, #OUT_H]       /* WR = 1 */

    MOVS    R5,  #0x21              /* X_Addr REG.  */
    STR     R5,  [R7, #ODR]         /* [LCD_DATA] = X_Addr REG. */
    STR     R3,  [R6, #OUT_L]       /* RS = 0 */
    STR     R4,  [R6, #OUT_L]       /* WR = 0 */

    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL */

    STR     R4,  [R6, #OUT_H]       /* WR = 1 */
    STR     R3,  [R6, #OUT_H]       /* RS = 1 */
    STR     R0,  [R7, #ODR]         /* [LCD_DATA] = X_Addr */
    STR     R4,  [R6, #OUT_L]       /* WR = 0 */

    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL */

    STR     R4,  [R6, #OUT_H]       /* WR = 1 */

    MOVS    R5,  #0x22              /* GRAM_Addr REG.  */
    STR     R5,  [R7, #ODR]         /* [LCD_DATA] = GRAM_Addr REG. */
    STR     R3,  [R6, #OUT_L]       /* RS = 0 */
    STR     R4,  [R6, #OUT_L]       /* WR = 0 */

    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL */

    STR     R4,  [R6, #OUT_H]       /* WR = 1 */
    STR     R3,  [R6, #OUT_H]       /* RS = 1 */

    MOVS    R5,  #0x44444444        /* INPUT MODE   */
    STR     R5,  [R7, #CRL]
    STR     R5,  [R7, #CRH]

    MOVS    R1,  #LCD_RD
    STR     R1,  [R6, #OUT_L]       /* RD=0 */

    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL  */
    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL  */
    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL  */
    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL  */

    STR     R1,  [R6, #OUT_H]       /* RD=1  */

    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL   */
    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL   */
    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL   */
    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL   */

    STR     R1,  [R6, #OUT_L]       /* RD = 0 */

    MOVW    R6,  #LCD_DATA_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL */
    MOVW    R6,  #LCD_DATA_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL */
    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL   */
    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO              /* DELAY 2 CYCEL   */

    LDR     R0,  [R7, #IDR]         /* R3 = [LCD_DATA] */

    STR     R1,  [R6, #OUT_H]       /* RD = 1 */

    MOVS    R5,  #0x33333333        /* OUTPUT MODE   */
    STR     R5,  [R7, #CRL]
    STR     R5,  [R7, #CRH]


    LDMIA   SP!, {R4-R7}
    BX      LR


#ifdef __GNUC__
  .end
#else
  END
#endif

/* ******************************* END OF FILE *********************************** */


