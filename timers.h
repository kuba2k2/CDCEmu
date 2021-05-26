#ifndef TIMERS_H
#define TIMERS_H

#include <stdint.h>

#define T_MS(x)     (x/100)

void timer_start();
void timer_reset(uint8_t num);
void timer_reset_all();
bool timer_check(uint8_t num, uint8_t delay);

#endif
