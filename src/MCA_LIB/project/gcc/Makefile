NAME = mca-lib

# Directory layout
BASE    = ../../../..
PRJ     = $(BASE)/src/MCA_LIB
STM_LIB = $(BASE)/lib/library
USB_LIB = $(BASE)/lib/USBLib

INCLUDES = -I $(PRJ)/include -I $(STM_LIB)/inc -I $(USB_LIB)/inc

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
CFLAGS  = $(MFLAGS) $(INCLUDES) -c -fno-common -Os -g
LFLAGS  = $(MFLAGS) -T $(LNKSCRIPT) -nostartfiles

CSRC = \
	$(PRJ)/source/stm32f10x_vector.c \
	$(PRJ)/source/stm32f10x_it.c \
	$(PRJ)/source/main.c \
	$(PRJ)/source/HW_V1_Config.c \
	$(PRJ)/source/Lcd.c \
	$(PRJ)/source/msd.c \
	$(PRJ)/source/scsi_data.c \
	$(PRJ)/source/usb_bot.c \
	$(PRJ)/source/usb_desc.c \
	$(PRJ)/source/usb_endp.c \
	$(PRJ)/source/usb_istr.c \
	$(PRJ)/source/usb_prop.c \
	$(PRJ)/source/usb_pwr.c \
	$(PRJ)/source/usb_scsi.c \
	$(USB_LIB)/src/usb_core.c \
	$(USB_LIB)/src/usb_init.c \
	$(USB_LIB)/src/usb_int.c \
	$(USB_LIB)/src/usb_mem.c \
	$(USB_LIB)/src/usb_regs.c \
	$(STM_LIB)/src/stm32f10x_flash.c \
	$(STM_LIB)/src/stm32f10x_nvic.c \
	$(STM_LIB)/src/stm32f10x_rcc.c \
	$(STM_LIB)/src/stm32f10x_spi.c

COBJ = $(CSRC:.c=.o)

ASRC = ASM_Function-gcc.s cortexm3_macro.s startup_stm32f10x_md.s

AOBJ = $(ASRC:.s=.o)

all: binaries

# NOTE: ASM_Funct-gcc.s is not up to date with changes in ASM_Function-gcc.S
# The assembly has to be converted from IAR style
#ASM_Function-gcc.S: $(PRJ)/source/ASM_Function.s
#	sed -f iar2gas < $(PRJ)/source/ASM_Function.s > ASM_Function-gcc.S

$(NAME).elf:	$(AOBJ) $(COBJ) $(LNKSCRIPT)
	$(CC) $(LFLAGS) -o $(NAME).elf $(AOBJ) $(COBJ)

binaries:	$(NAME).elf
	$(OD) -S $(NAME).elf > $(NAME).lst
	$(CP) -O binary $(NAME).elf $(NAME).bin
	$(CP) -O ihex $(NAME).elf $(NAME).hex

clean:
	rm -f *.o $(PRJ)/source/gcc/*.o $(PRJ)/source/*.o $(STM_LIB)/src/*.o ${USB_LIB}/src/*.o

distclean:	clean
	rm -f $(NAME).elf $(NAME).bin $(NAME).hex $(NAME).lst
#	ASM_Function-gcc.[sS]
