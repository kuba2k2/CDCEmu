#include "timers.h"

#include <avr/interrupt.h>

uint8_t TOVF0 = 0;
uint8_t TCNTG = 0;
uint8_t timers[TIMERS];

// on tick 0
ISR(TIM0_OVF_vect) {
    TOVF0++; // increment the overflow count
}

// on tick 106
ISR(TIM0_COMPA_vect) {
    if (TOVF0 > TIMER_OVF_CNT) {
        TOVF0 = 0; // reset the overflow count
        TCNT0 = 0; // reset the tick count
        TCNTG++; // increment the global timer
        if (TCNTG >= 200) {
            TCNTG -= 200; // reset the timer after 200 ticks
        }
    }
}

void timer_start() {
    TCCR0A = 0x00; // normal port operation, CTC disabled
    TCCR0B |= TIMER_CS;
    OCR0A = TIMER_TICK_CNT; // set the comparator count
    TIMSK |= (1<<OCIE0A) | (1<<TOIE0); // enable TIM0_COMPA_vect and TIM0_OVF_vect
    sei();
    timer_reset_all();
}

void timer_reset(uint8_t num) {
    timers[num] = 0;
}

void timer_reset_all() {
    TCNT0 = 0;
    TOVF0 = 0;
    TCNTG = 0;
    for (uint8_t i = 0; i < TIMERS; i++) {
        timers[i] = 0;
    }
}

bool timer_check(uint8_t num, uint8_t delay) {
    if (TCNTG - timers[num] >= delay || TCNTG < timers[num]) {
        timers[num] = TCNTG;
        return true;
    }
    return false;
}
