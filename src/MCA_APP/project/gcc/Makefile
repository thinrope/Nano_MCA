NAME = mca-app

# Directory layout
BASE    = ../../../..
PRJ     = $(BASE)/src/MCA_APP
STM_LIB = $(BASE)/lib/library
USB_LIB = $(BASE)/lib/USBLib
FIX_LIB = $(BASE)/lib/libfixmath

INCLUDES = -I $(PRJ)/include -I $(STM_LIB)/inc -I $(USB_LIB)/inc -I $(FIX_LIB)/inc

# Toolchain setup
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld -v
AR = arm-none-eabi-ar
AS = arm-none-eabi-as
CP = arm-none-eabi-objcopy
OD = arm-none-eabi-objdump

LNKSCRIPT = stm32f10x_flash_offset.ld
MFLAGS  = -mcpu=cortex-m3 -mthumb
ASFLAGS = $(MFLAGS)
CFLAGS  = $(MFLAGS) $(INCLUDES) -DARM -c -fno-common -Os -g -pipe -DFIXMATH_NO_64BIT -DFIXMATH_NO_OVERFLOW
LFLAGS  = $(MFLAGS) -T $(LNKSCRIPT) -nostartfiles

CSRC = \
	$(PRJ)/source/stm32f10x_vector.c \
	$(PRJ)/source/stm32f10x_it.c \
	$(PRJ)/source/main.c \
	$(PRJ)/source/HW_V1_Config.c \
	$(PRJ)/source/Lcd.c \
	$(PRJ)/source/Files.c \
	$(PRJ)/source/Function.c \
	$(PRJ)/source/Menu.c \
	$(PRJ)/source/Sig_Acq.c \
	$(PRJ)/source/Display.c \
	$(STM_LIB)/src/stm32f10x_flash.c \
	$(STM_LIB)/src/stm32f10x_nvic.c \
	$(STM_LIB)/src/stm32f10x_rcc.c \
	$(FIX_LIB)/src/fix16.c

COBJ = $(CSRC:.c=.o)

ASRC = ASM_Function-gcc.s cortexm3_macro.s startup_stm32f10x_md.s

AOBJ = $(ASRC:.s=.o)

all: $(NAME).bin

# NOTE: ASM_Funct-gcc.s is not up to date with changes in ASM_Function-gcc.S
# The assembly has to be converted from IAR style
#ASM_Function-gcc.S: $(PRJ)/source/ASM_Function.s
#	sed -f iar2gas < $(PRJ)/source/ASM_Function.s > ASM_Function-gcc.S

%.elf: $(AOBJ) $(COBJ) $(LNKSCRIPT)
	$(CC) $(LFLAGS) -o $@ $(AOBJ) $(COBJ)

%.hex: %.elf
	$(CP) -O ihex $< $@

%.list: %.elf
	$(OD) -S $< > $@

%.bin: %.elf
	$(CP) -O binary $< $@

clean:
	rm -f *.o $(PRJ)/source/gcc/*.o $(PRJ)/source/*.o $(STM_LIB)/src/*.o ${USB_LIB}/src/*.o ${FIX_LIB}/src/*.o

distclean:	clean
	rm -f $(NAME).elf $(NAME).bin $(NAME).hex $(NAME).lst
#	ASM_Function-gcc.[sS]
