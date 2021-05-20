export MCU=attiny85
export F_CPU=16000000
export CC=avr-gcc
export OBJCOPY=avr-objcopy
export SIZE=avr-size

TARGET=main
SUBDIRS=
SRCS=main.cpp

OBJS=$(patsubst %,%/*.o, ${SUBDIRS})
INCLUDES=$(patsubst %, -I%, ${SUBDIRS})
export CFLAGS=-Wall -g -Os -mmcu=${MCU} -DF_CPU=${F_CPU} -I. ${INCLUDES} -include config.h

all: $(SUBDIRS)
	mkdir -p .out
	${CC} ${CFLAGS} -o .out/${TARGET}.o ${OBJS} ${SRCS}
	${OBJCOPY} -j .text -j .data -O ihex .out/${TARGET}.o .out/${TARGET}.hex
	${SIZE} -C --mcu=${MCU} .out/${TARGET}.o

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

flash:
	avrdude -p ${MCU} -c usbasp -U flash:w:.out/${TARGET}.hex:i -F -P usb

clean: $(SUBDIRS)
	rm -f .out/*.o .out/*.hex

.PHONY: all $(SUBDIRS)
