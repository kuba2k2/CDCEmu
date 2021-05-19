#include "Arduino.h"
#include "uart.h"

void print(char *fmt, ... ) {
    char buf[128];
    va_list args;
    va_start (args, fmt );
    vsnprintf(buf, 128, fmt, args);
    va_end (args);

    char* s = buf;
    PCMSK &= ~(1<<PURXBIT);
    while (*s) putx(*s++);
    PCMSK |= 1<<PURXBIT;
}
