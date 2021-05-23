export MCU=attiny85
export F_CPU=16000000
export CC=avr-gcc
export OBJCOPY=avr-objcopy
export SIZE=avr-size

TARGET=main
SUBDIRS=i2c pcf8574 uart spi mcp2515
SRCS=main.cpp

OBJS=$(patsubst %,%/*.o, ${SUBDIRS})
INCLUDES=$(patsubst %,-I${CURDIR}/%, ${SUBDIRS})
INCLUDES+=-I${CURDIR}
INCLUDES+=-include config.h
export CFLAGS=-Wall -g -Os -fdata-sections -ffunction-sections -flto -mmcu=${MCU} -DF_CPU=${F_CPU} ${INCLUDES}

all: $(SUBDIRS)
	mkdir -p .out
	${CC} ${CFLAGS} -Wl,--gc-sections -o .out/${TARGET}.o ${OBJS} ${SRCS}
	${OBJCOPY} -j .text -j .data -O ihex .out/${TARGET}.o .out/${TARGET}.hex
	${SIZE} -C --mcu=${MCU} .out/${TARGET}.o

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

flash:
	avrdude -p ${MCU} -c usbasp -U flash:w:.out/${TARGET}.hex:i -F -P usb

clean: $(SUBDIRS)
	rm -f .out/*.o .out/*.hex

.PHONY: all $(SUBDIRS)
