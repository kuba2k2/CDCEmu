#include <avr/io.h>
#include <util/delay.h>

int main() {
    while (1) {
        PORTB ^= (1<<PB0);
        _delay_ms(250);
    }
    return 0;
}
