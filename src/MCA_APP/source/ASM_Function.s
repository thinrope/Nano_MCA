;*******************************************************************************
; File name  : ASM_Function.s 
;*******************************************************************************

#ifdef __GNUC__
#define EXPORT .globl
.cpu cortex-m3
.syntax unified
.thumb
.text
#else
  RSEG CODE:CODE(2)
#endif
  
  EXPORT __CTR_HP
  EXPORT __USB_Istr
  
  EXPORT __MSD_WriteBlock
  EXPORT __MSD_ReadBlock
  EXPORT __Get_Font_8x14
  #EXPORT __Get_Ref_Wave
  EXPORT __SD_Set_Changed

  EXPORT __Erase_Color
  EXPORT __Add_Color
  EXPORT __Get_Pixel
  
GPIO          =   0x4001
LCD_DATA_PORT =   0x1800   ;PORT_E
LCD_Ctrl_PORT =   0x1400   ;PORT_D
CRH           =   0x00
CRL           =   0x04
IDR           =   0x08
ODR           =   0x0C
OUT_H         =   0x10
OUT_L         =   0x14

LCD_RS        =   0x02
LCD_WR        =   0x20
LCD_RD        =   0x10
LCD_RST       =   0x01

#define RGB(_R,_G,_B) (((_R & 0x3E) >> 1) | ((_G & 0x3f) << 5) | ((_B & 0x3e) << 10))

C_GROUP       =   0x1082
F_SELEC       =   0x18E3
GRD_FLAG      =   0x0040
LN1_FLAG      =   0x0020
LN2_FLAG      =   0x0800
LN1_COLOR     = (RGB(32,63,32) & ~F_SELEC) | LN1_FLAG
GRD_COLOR     = (RGB(32,32,32) & ~F_SELEC) | GRD_FLAG
LN2_COLOR     = (RGB(63,63,63) & ~F_SELEC) | LN2_FLAG
WAV_FLAG      =   0x0080
CH2_FLAG      =   0x0002
REF_FLAG      =   0x1000
WAV_COLOR     = (RGB(0,63,63) & ~F_SELEC) | WAV_FLAG
CH2_COLOR     = (RGB(63,63,0) & ~F_SELEC) | CH2_FLAG
REF_COLOR     = (RGB(63,0,63) & ~F_SELEC) | REF_FLAG

;*******************************************************************************
; Table of library functions start address, based on LIB memory map
;*******************************************************************************
__USB_Istr                ;void __USB_Istr(void);
    B       0x0800410F
__CTR_HP                  ;void __CTR_HP(void)
    B       0x08004113
__MSD_WriteBlock          ;u8 __MSD_WriteBlock(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
    B       0x08004117
__MSD_ReadBlock           ;u8 __MSD_ReadBlock(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
    B       0x0800411B
__Get_Font_8x14           ;u16 __Get_TAB_8x14(unsigned char Code, unsigned short Row)
    B       0x0800411F
;__Get_Ref_Wave            ;u8 __Get_Ref_Wave(unsigned short i)
 ;   B       0x08004123
__SD_Set_Changed          ;void __SD_Set_Changed(void)
    B       0x08004123

;*******************************************************************************
__Add_Color:            ;void __Add_Color(unsigned short x,unsigned short y,unsigned short Color)

    STMDB   SP!, {R4-R7}       

    MOVW    R7,  #LCD_DATA_PORT  
    MOVT    R7,  #GPIO  
    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO  
    MOVS    R3,  #LCD_RS 
    MOVS    R4,  #LCD_WR
    
    MOVS    R5,  #0x20              ;Y_Addr REG. 
    STR     R5,  [R7, #ODR]         ;[LCD_DATA] = Y_Addr REG.
    STR     R3,  [R6, #OUT_L]       ;RS = 0
    STR     R4,  [R6, #OUT_L]       ;WR = 0
    
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL
    
    STR     R4,  [R6, #OUT_H]       ;WR = 1
    STR     R3,  [R6, #OUT_H]       ;RS = 1
    STR     R1,  [R7, #ODR]         ;[LCD_DATA] = Y_Addr
    STR     R4,  [R6, #OUT_L]       ;WR = 0
    
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL
    
    STR     R4,  [R6, #OUT_H]       ;WR = 1

    MOVS    R5,  #0x21              ;X_Addr REG. 
    STR     R5,  [R7, #ODR]         ;[LCD_DATA] = X_Addr REG.
    STR     R3,  [R6, #OUT_L]       ;RS = 0
    STR     R4,  [R6, #OUT_L]       ;WR = 0
    
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL
    
    STR     R4,  [R6, #OUT_H]       ;WR = 1
    STR     R3,  [R6, #OUT_H]       ;RS = 1
    STR     R0,  [R7, #ODR]         ;[LCD_DATA] = X_Addr
    STR     R4,  [R6, #OUT_L]       ;WR = 0
    
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL
    
    STR     R4,  [R6, #OUT_H]       ;WR = 1

    MOVS    R5,  #0x22              ;GRAM_Addr REG. 
    STR     R5,  [R7, #ODR]         ;[LCD_DATA] = GRAM_Addr REG.
    STR     R3,  [R6, #OUT_L]       ;RS = 0
    STR     R4,  [R6, #OUT_L]       ;WR = 0
    
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL
    
    STR     R4,  [R6, #OUT_H]       ;WR = 1
    STR     R3,  [R6, #OUT_H]       ;RS = 1

    MOVS    R5,  #0x44444444        ;INPUT MODE  
    STR     R5,  [R7, #CRL]
    STR     R5,  [R7, #CRH]

    MOVS    R0,  #LCD_RD 
    STR     R0,  [R6, #OUT_L]       ;RD=0
    
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL 
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL 
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL 
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL 
    
    STR     R0,  [R6, #OUT_H]       ;RD=1 

    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL  
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL  
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL  
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL  
    
    STR     R0,  [R6, #OUT_L]       ;RD = 0

    MOVW    R6,  #LCD_DATA_PORT
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL
    MOVW    R6,  #LCD_DATA_PORT
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL  
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL  
    
    LDR     R3,  [R7, #IDR]         ;R3 = [LCD_DATA]
    
    STR     R0,  [R6, #OUT_H]       ;RD = 1

    MOVS    R5,  #0x33333333        ;OUTPUT MODE  
    STR     R5,  [R7, #CRL]
    STR     R5,  [R7, #CRH]

    MOVS    R0,  #LCD_RS 
    MOVS    R5,  #0x20              ;Y_Addr REG. 
    STR     R5,  [R7, #ODR]         ;[LCD_DATA] = Y_Addr REG.
    STR     R0,  [R6, #OUT_L]       ;RS = 0
    STR     R4,  [R6, #OUT_L]       ;WR = 0
    
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL
    
    STR     R4,  [R6, #OUT_H]       ;WR = 1
    STR     R0,  [R6, #OUT_H]       ;RS = 1
    STR     R1,  [R7, #ODR]         ;[LCD_DATA] = Y_Addr
    STR     R4,  [R6, #OUT_L]       ;WR = 0
    
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL
    
    STR     R4,  [R6, #OUT_H]       ;WR = 1

    MOVS    R5,  #0x22              ;GRAM_Addr REG. 
    STR     R5,  [R7, #ODR]         ;[LCD_DATA] = GRAM_Addr REG.
    STR     R0,  [R6, #OUT_L]       ;RS = 0
    STR     R4,  [R6, #OUT_L]       ;WR = 0
    
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL
    
    STR     R4,  [R6, #OUT_H]       ;WR = 1
    STR     R0,  [R6, #OUT_H]       ;RS = 1

    MOVW    R5,  #C_GROUP
    ANDS    R0,  R3,  R5
    BEQ     ADD_1                   ;If original color is not curve then add color
    ANDS    R0,  R2,  R5            
    BEQ     ADD_2                   ;If new color is not curve then keep color as is
ADD_1:
    MOVW    R5,  #F_SELEC
    AND     R3,  R3,  R5            ;Erase color, keep flags
    ORR     R3,  R3,  R2            ;Add new color
    B       ADD_3
ADD_2:
    MOVW    R5,  #F_SELEC
    AND     R2,  R2,  R5            ;Erase new color, keep flags
    ORR     R3,  R3,  R2            ;Add flag
ADD_3:
    STR     R3,  [R7, #ODR]         ;[LCD_DATA] = R3
    STR     R4,  [R6, #OUT_L]       ;WR = 0
    
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL
    
    STR     R4,  [R6, #OUT_H]       ;WR = 1
    
    LDMIA   SP!, {R4-R7}           
    BX      LR


;*******************************************************************************
__Erase_Color:            ;void __Erase_Color(unsigned short x,unsigned short y,unsigned short Color)

    STMDB   SP!, {R4-R7}       

    MOVW    R7,  #LCD_DATA_PORT  
    MOVT    R7,  #GPIO  
    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO  
    MOVS    R3,  #LCD_RS 
    MOVS    R4,  #LCD_WR
    
    MOVS    R5,  #0x20              ;Y_Addr REG. 
    STR     R5,  [R7, #ODR]         ;[LCD_DATA] = Y_Addr REG.
    STR     R3,  [R6, #OUT_L]       ;RS = 0
    STR     R4,  [R6, #OUT_L]       ;WR = 0
    
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL
    
    STR     R4,  [R6, #OUT_H]       ;WR = 1
    STR     R3,  [R6, #OUT_H]       ;RS = 1
    STR     R1,  [R7, #ODR]         ;[LCD_DATA] = Y_Addr
    STR     R4,  [R6, #OUT_L]       ;WR = 0
    
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL
    
    STR     R4,  [R6, #OUT_H]       ;WR = 1

    MOVS    R5,  #0x21              ;X_Addr REG. 
    STR     R5,  [R7, #ODR]         ;[LCD_DATA] = X_Addr REG.
    STR     R3,  [R6, #OUT_L]       ;RS = 0
    STR     R4,  [R6, #OUT_L]       ;WR = 0
    
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL
    
    STR     R4,  [R6, #OUT_H]       ;WR = 1
    STR     R3,  [R6, #OUT_H]       ;RS = 1
    STR     R0,  [R7, #ODR]         ;[LCD_DATA] = X_Addr
    STR     R4,  [R6, #OUT_L]       ;WR = 0
    
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL
    
    STR     R4,  [R6, #OUT_H]       ;WR = 1

    MOVS    R5,  #0x22              ;GRAM_Addr REG. 
    STR     R5,  [R7, #ODR]         ;[LCD_DATA] = GRAM_Addr REG.
    STR     R3,  [R6, #OUT_L]       ;RS = 0
    STR     R4,  [R6, #OUT_L]       ;WR = 0
    
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL
    
    STR     R4,  [R6, #OUT_H]       ;WR = 1
    STR     R3,  [R6, #OUT_H]       ;RS = 1

    MOVS    R5,  #0x44444444        ;INPUT MODE  
    STR     R5,  [R7, #CRL]
    STR     R5,  [R7, #CRH]

    MOVS    R0,  #LCD_RD 
    STR     R0,  [R6, #OUT_L]       ;RD=0
    
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL 
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL 
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL 
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL 
    
    STR     R0,  [R6, #OUT_H]       ;RD=1 

    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL  
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL  
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL  
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL  
    
    STR     R0,  [R6, #OUT_L]       ;RD = 0

    MOVW    R6,  #LCD_DATA_PORT
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL
    MOVW    R6,  #LCD_DATA_PORT
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL  
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL  
    
    LDR     R3,  [R7, #IDR]         ;R3 = [LCD_DATA]
    
    STR     R0,  [R6, #OUT_H]       ;RD = 1

    MOVS    R5,  #0x33333333        ;OUTPUT MODE  
    STR     R5,  [R7, #CRL]
    STR     R5,  [R7, #CRH]

    MOVS    R0,  #LCD_RS 
    MOVS    R5,  #0x20              ;Y_Addr REG. 
    STR     R5,  [R7, #ODR]         ;[LCD_DATA] = Y_Addr REG.
    STR     R0,  [R6, #OUT_L]       ;RS = 0
    STR     R4,  [R6, #OUT_L]       ;WR = 0
    
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL
    
    STR     R4,  [R6, #OUT_H]       ;WR = 1
    STR     R0,  [R6, #OUT_H]       ;RS = 1
    STR     R1,  [R7, #ODR]         ;[LCD_DATA] = Y_Addr
    STR     R4,  [R6, #OUT_L]       ;WR = 0
    
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL
    
    STR     R4,  [R6, #OUT_H]       ;WR = 1

    MOVS    R5,  #0x22              ;GRAM_Addr REG. 
    STR     R5,  [R7, #ODR]         ;[LCD_DATA] = GRAM_Addr REG.
    STR     R0,  [R6, #OUT_L]       ;RS = 0
    STR     R4,  [R6, #OUT_L]       ;WR = 0
    
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL
    
    STR     R4,  [R6, #OUT_H]       ;WR = 1
    STR     R0,  [R6, #OUT_H]       ;RS = 1

    MOVW    R5,  #F_SELEC
    AND     R0,  R3, R5             ;extract color flags from pixel
    ANDS    R0,  R0, R2             ;check if color flag is in erase color
    BEQ     ERASE_3                 ;color not present, nothing to erase

    AND     R3,  R3,  R5            ;Extract flags for original color
    AND     R2,  R2,  R5            ;Extract flags for erase color
    BICS    R3,  R3,  R2            ;Remove erase color flag
    BEQ     ERASE_3                 ;No bits left, restore background

ERASE_CH1:
    MOVW    R5,  #WAV_FLAG
    ANDS    R5,  R5,  R3
    BEQ     ERASE_CH2
    MOVW    R5,  #WAV_COLOR
    ORR     R3,  R3,  R5            ;Restore ch1 color
    B       ERASE_3
ERASE_CH2:
    MOVW    R5,  #CH2_FLAG
    ANDS    R5,  R5,  R3
    BEQ     ERASE_CH3
    MOVW    R5,  #CH2_COLOR
    ORR     R3,  R3,  R5            ;Restore ch2 color
    B       ERASE_3
ERASE_CH3:
    MOVW    R5,  #REF_FLAG
    ANDS    R5,  R5,  R3
    BEQ     ERASE_LN1
    MOVW    R5,  #REF_COLOR
    ORR     R3,  R3,  R5            ;Restore ch3 color
    B       ERASE_3
ERASE_LN1:
    MOVW    R5,  #LN1_FLAG
    ANDS    R5,  R5,  R3
    BEQ     ERASE_LN2
    MOVW    R5,  #LN1_COLOR
    ORR     R3,  R3,  R5            ;Restore ln1 color
    B       ERASE_3
ERASE_LN2:
    MOVW    R5,  #LN2_FLAG
    ANDS    R5,  R5,  R3
    BEQ     ERASE_GRD
    MOVW    R5,  #LN2_COLOR
    ORR     R3,  R3,  R5            ;Restore ln2 color
    B       ERASE_3
ERASE_GRD:
    MOVW    R5,  #GRD_FLAG
    ANDS    R5,  R5,  R3
    BEQ     ERASE_3                 ;restores background
    MOVW    R5,  #GRD_COLOR
    ORR     R3,  R3,  R5            ;Restore grd color
ERASE_3:    
    STR     R3,  [R7, #ODR]         ;[LCD_DATA] = R3
    STR     R4,  [R6, #OUT_L]       ;WR = 0
    
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL
    
    STR     R4,  [R6, #OUT_H]       ;WR = 1
    
    LDMIA   SP!, {R4-R7}           
    BX      LR

;*******************************************************************************
__Get_Pixel:    ;unsigned short __Get_Pixel(unsigned short x0,unsigned short y0)

    STMDB   SP!, {R4-R7}       

    MOVW    R7,  #LCD_DATA_PORT  
    MOVT    R7,  #GPIO  
    MOVW    R6,  #LCD_Ctrl_PORT
    MOVT    R6,  #GPIO  
    MOVS    R3,  #LCD_RS 
    MOVS    R4,  #LCD_WR
    
    MOVS    R5,  #0x20              ;Y_Addr REG. 
    STR     R5,  [R7, #ODR]         ;[LCD_DATA] = Y_Addr REG.
    STR     R3,  [R6, #OUT_L]       ;RS = 0
    STR     R4,  [R6, #OUT_L]       ;WR = 0
    
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL
    
    STR     R4,  [R6, #OUT_H]       ;WR = 1
    STR     R3,  [R6, #OUT_H]       ;RS = 1
    STR     R1,  [R7, #ODR]         ;[LCD_DATA] = Y_Addr
    STR     R4,  [R6, #OUT_L]       ;WR = 0
    
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL
    
    STR     R4,  [R6, #OUT_H]       ;WR = 1

    MOVS    R5,  #0x21              ;X_Addr REG. 
    STR     R5,  [R7, #ODR]         ;[LCD_DATA] = X_Addr REG.
    STR     R3,  [R6, #OUT_L]       ;RS = 0
    STR     R4,  [R6, #OUT_L]       ;WR = 0
    
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL
    
    STR     R4,  [R6, #OUT_H]       ;WR = 1
    STR     R3,  [R6, #OUT_H]       ;RS = 1
    STR     R0,  [R7, #ODR]         ;[LCD_DATA] = X_Addr
    STR     R4,  [R6, #OUT_L]       ;WR = 0
    
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL
    
    STR     R4,  [R6, #OUT_H]       ;WR = 1

    MOVS    R5,  #0x22              ;GRAM_Addr REG. 
    STR     R5,  [R7, #ODR]         ;[LCD_DATA] = GRAM_Addr REG.
    STR     R3,  [R6, #OUT_L]       ;RS = 0
    STR     R4,  [R6, #OUT_L]       ;WR = 0
    
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL
    
    STR     R4,  [R6, #OUT_H]       ;WR = 1
    STR     R3,  [R6, #OUT_H]       ;RS = 1

    MOVS    R5,  #0x44444444        ;INPUT MODE  
    STR     R5,  [R7, #CRL]
    STR     R5,  [R7, #CRH]

    MOVS    R1,  #LCD_RD 
    STR     R1,  [R6, #OUT_L]       ;RD=0
    
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL 
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL 
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL 
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL 
    
    STR     R1,  [R6, #OUT_H]       ;RD=1 

    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL  
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL  
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL  
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL  
    
    STR     R1,  [R6, #OUT_L]       ;RD = 0

    MOVW    R6,  #LCD_DATA_PORT
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL
    MOVW    R6,  #LCD_DATA_PORT
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL  
    MOVW    R6,  #LCD_Ctrl_PORT  
    MOVT    R6,  #GPIO              ;DELAY 2 CYCEL  
    
    LDR     R0,  [R7, #IDR]         ;R3 = [LCD_DATA]
    
    STR     R1,  [R6, #OUT_H]       ;RD = 1

    MOVS    R5,  #0x33333333        ;OUTPUT MODE  
    STR     R5,  [R7, #CRL]
    STR     R5,  [R7, #CRH]

    
    LDMIA   SP!, {R4-R7}           
    BX      LR


#ifdef __GNUC__
  .end
#else
  END
#endif

;******************************* END OF FILE ***********************************
  

