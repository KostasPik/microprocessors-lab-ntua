MCU = atmega328pb
F_CPU = 16000000UL

CC = avr-gcc
LD = avr-ld
OBJCOPY = avr-objcopy

CFLAGS = -Wall -Wextra -Wconversion -B/home/andreas/src/avr -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Wl,--gc-sections -Os -ffunction-sections -fdata-sections -fshort-enums -funsigned-char -funsigned-bitfields -gdwarf-3 
LDFLAGS = -B/home/andreas/src/avr -mmcu=$(MCU)
all: main

../common/lcd.o: CFLAGS += -DLCD_PCA9555

main: ../common/keypad.o ../common/twi_pca9555.o ../common/lcd.o main.o

main.hex: main
	avr-objcopy -O ihex main main.hex

install: main.hex
	avrdude -p m328pb -c xplainedmini -U flash:w:main.hex

clean:
	rm -f main main.hex main.o ../common/keypad.o ../common/twi_pca9555.o ../common/lcd.o

.PHONY: all install clean

