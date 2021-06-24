#ifndef TIMERS_H
#define TIMERS_H

#include <stdint.h>

#define TIMER_PACKET_100MS      0
#define TIMER_PACKET_500MS      1
#define TIMER_PACKET_1000MS     2
#define TIMER_BUFFER_FLUSH      3
#define TIMER_TRACK_TIME        4

#define TIMERS          8
#define TIMER_OVF_CNT   24        // total 6250 ticks, 100ms
#define TIMER_TICK_CNT  106

#define T_MS(x)     (x/100)

void timer_start();
void timer_reset(uint8_t num);
void timer_reset_all();
uint8_t timer_read_global();
bool timer_check(uint8_t num, uint8_t delay);
bool timer_check(uint8_t num, uint8_t delay, uint8_t tctng);

#endif
