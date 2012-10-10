#
# GNU Makefile for Gamma Grapher V2
# This file is cross development for STM32 target.
#

NAME = mca-app

# Directory layout
BASE    = ../../..
PRJ     = $(BASE)/GGV2_APP
STM_LIB = $(BASE)/library
FIX_LIB = $(BASE)/libfixmath
USB_LIB = $(BASE)/USBLib

INCLUDES =	-I $(PRJ)/include \
          	-I $(STM_LIB)/inc \
			-I $(FIX_LIB)/inc \
			-I $(USB_LIB)/inc

# Toolchain setup
CROSS_COMPILE = ~/Hobbies/Electronics/ARMCortex3/build_chain/bin/arm-none-eabi-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld -v
AR = $(CROSS_COMPILE)ar
AS = $(CROSS_COMPILE)as
CP = $(CROSS_COMPILE)objcopy
OD = $(CROSS_COMPILE)objdump

LNKSCRIPT = stm32f10x_flash_offset.ld
MFLAGS  = -mcpu=cortex-m3 -mthumb
ASFLAGS = $(MFLAGS)
CFLAGS  = $(MFLAGS) $(INCLUDES) -DARM -c -fno-common -Os -g -pipe -DFIXMATH_NO_64BIT -DFIXMATH_NO_OVERFLOW
LFLAGS  = $(MFLAGS) -T $(LNKSCRIPT) -nostartfiles

CSRC = \
		$(PRJ)/source/stm32f10x_vector.c \
		$(PRJ)/source/stm32f10x_it.c \
		$(PRJ)/source/Main.c \
		$(PRJ)/source/HW_V1_Config.c \
		$(PRJ)/source/Lcd.c \
		$(PRJ)/source/Files.c \
		$(PRJ)/source/Function.c \
		$(PRJ)/source/Menu.c \
		$(PRJ)/source/Sig_Acq.c \
		$(PRJ)/source/Display.c

CSRC += \
		$(STM_LIB)/src/stm32f10x_flash.c \
		$(STM_LIB)/src/stm32f10x_nvic.c \
		$(STM_LIB)/src/stm32f10x_rcc.c \
		$(FIX_LIB)/src/fix16.c

COBJ = $(CSRC:.c=.o)

ASRC = ASM_Function-gcc.s \
       cortexm3_macro.s \
       startup_stm32f10x_md.s

AOBJ = $(ASRC:.s=.o)

all: binaries

# NOTE: ASM_Funct-gcc.s is not up to date with changes in ASM_Function-gcc.S
# The assembly has to be converted from IAR style
#ASM_Function-gcc.S: $(PRJ)/source/ASM_Function.s
#	sed -f iar2gas < $(PRJ)/source/ASM_Function.s > ASM_Function-gcc.S

$(NAME).elf: $(AOBJ) $(COBJ) $(LNKSCRIPT)
	$(CC) $(LFLAGS) -o $(NAME).elf $(AOBJ) $(COBJ)

binaries: $(NAME).elf
	$(OD) -S $(NAME).elf > $(NAME).lst
	$(CP) -O binary $(NAME).elf $(NAME).bin
	$(CP) -O ihex $(NAME).elf $(NAME).hex

clean:
	rm -f *.o $(PRJ)/source/gcc/*.o $(PRJ)/source/*.o $(STM_LIB)/src/*.o *~ $(PRJ)/source/*~

clean_all:
	rm -f *.o $(PRJ)/source/*.o $(STM_LIB)/src/*.o *~ $(PRJ)/source/*.~\
	    $(NAME).elf $(NAME).bin $(NAME).hex $(NAME).lst
#	    ASM_Function-gcc.[sS]
