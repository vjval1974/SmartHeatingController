###############################################################################
# Makefile for the project ATMegatest
###############################################################################
MCU=atmega128
CC=avr-gcc
OBJCOPY=avr-objcopy -v
# optimize for size:
CFLAGS=-g -mmcu=$(MCU) -Wall -Os -mcall-prologues -DF_CPU=16000000 -DF_OSC=16000000 -D__AVR_ATmega128__ -lm 

OBJS = main.o \
	 hd44780.o \
	 delay.o \
	 UART_routines.o \
	 RTC_routines.o \
	 i2c_routines.o \
	 keypad.o \
	 Interrupts.o \
	 timer.o  \
	 timer2.o \
	 adc.o   \
	 pwm.o \
	 AutoProgram.o \
	 Applet.o \
	 eepromStruct.o \
	 mcucsr.o \
	 console.o \
	 ds1820.o \
	 Program.o \
	 menu.o

#-------------------
all: proto.hex
#-------------------
proto.hex : proto.elf
	$(OBJCOPY) -R .eeprom -O ihex proto.elf proto.hex;
	$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 --no-change-warnings -O ihex proto.elf eeprom.hex;
	avr-size -C --mcu=$(MCU) proto.elf
proto.elf : $(OBJS)
	$(CC) $(CFLAGS) -o proto.elf -Wl,-Map,proto.map $(OBJS)
%.o : %.c
	$(CC) $(CFLAGS) -Os -c $<

install: all
	sudo avrdude -p $(MCU) -P usb -v -V -c usbasp -U flash:w:proto.hex

installeeprom:	all
	sudo avrdude -p $(MCU) -P usb -v -V -c usbasp -U eeprom:w:eeprom.hex

seteepromnoeraseonupload:
	avrdude -p $(MCU) -u -P usb -c usbasp -q -U hfuse:w:0x97:m


#-------------------
clean:
	rm -f *.o *.map *.elf *.hex
#-------------------
checkin: clean
	ci -l ./*.[ch]
	ci -l ./Makefile
