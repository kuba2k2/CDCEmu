#include "timers.h"

#include <avr/interrupt.h>

uint8_t TOVF1 = 0;
uint8_t TCNTG = 0;
uint8_t timers[TIMERS];

ISR(TIM1_OVF_vect) {
    TOVF1++; // increment the overflow count
}

ISR(TIM1_COMPA_vect) {
    if (TOVF1 > TIMER_OVF_CNT) {
        TOVF1 = 0; // reset the overflow count
        TCNT1 = 0; // reset the tick count
        TCNTG++; // increment the global timer
        if (TCNTG >= 200) {
            TCNTG -= 200; // reset the timer after 200 ticks
        }
    }
}

void timer_start() {
    PLLCSR |= (1<<PCKE); // enable asynchronous PCK clock source (64MHz)
    TCCR1 &= ~(CTC1); // normal port operation, CTC disabled
    TCCR1 |= (1<<CS13) | (1<<CS11) | (1<<CS10); // prescaler PCK/1024
    OCR1A = TIMER_TICK_CNT; // set the comparator count
    TIMSK |= (1<<OCIE1A) | (1<<TOIE1); // enable TIM1_COMPA_vect and TIM1_OVF_vect
    sei();
    timer_reset_all();
}

void timer_reset(uint8_t num) {
    timers[num] = 0;
}

void timer_reset_all() {
    TCNT1 = 0;
    TOVF1 = 0;
    TCNTG = 0;
    for (uint8_t i = 0; i < TIMERS; i++) {
        timers[i] = 0;
    }
}

uint8_t timer_read_global() {
    return TCNTG;
}

bool timer_check(uint8_t num, uint8_t delay) {
    return timer_check(num, delay, TCNTG);
}

bool timer_check(uint8_t num, uint8_t delay, uint8_t tcntg) {
    if (tcntg - timers[num] >= delay || tcntg < timers[num]) {
        timers[num] = tcntg;
        return true;
    }
    return false;
}
