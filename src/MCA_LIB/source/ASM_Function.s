;*******************************************************************************
; File name  : ASM_Function.s 
; File description: start addresses for exported functions
;*******************************************************************************

#ifdef __GNUC__
#define EXPORT .globl
#define EXTERN .globl
#endif

  EXTERN USB_Istr
  EXTERN CTR_HP
  EXTERN MSD_WriteBlock
  EXTERN MSD_ReadBlock
  EXTERN Get_Font_8x14
  EXTERN Get_Ref_Wave
  EXTERN SD_Set_Changed
  
  EXPORT __BRANCH_TABLE

#ifdef __GNUC__
.cpu cortex-m3
.syntax unified
.thumb
.text
#else
  RSEG branchtab:CODE(2)
#endif
  
;*******************************************************************************
; Table of library function start addresses
;*******************************************************************************
__BRANCH_TABLE             ; dummy
    B       __BRANCH_TABLE
__USB_Istr                 ;
    B       USB_Istr
__CTR_HP                   ;
    B       CTR_HP
__MSD_WriteBlock           ;
    B       MSD_WriteBlock
__MSD_ReadBlock            ;
    B       MSD_ReadBlock
__Get_Font_8x14            ;
    B       Get_Font_8x14
__Get_Ref_Wave             ;
    B       Get_Ref_Wave
__SD_Set_Changed           ;void SD_Set_Changed(void);
    B       SD_Set_Changed
#ifdef __GNUC__
  .end
#else
  END
#endif

;******************************* END OF FILE ***********************************
  

