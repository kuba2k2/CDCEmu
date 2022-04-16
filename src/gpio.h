#pragma once

#include <stdint.h>

#define LED_NORMAL		0
#define LED_INVERTED	1
#define LED_NO_DEFAULT	0
#define LED_DEFAULT_OFF 1
#define LED_DEFAULT_ON	2

#define LED_DEF(pin, inverted, t_on, t_off, flag, flag_inverted, led_default)                                          \
	T_MS(t_on), (pin << 5) | (T_MS(t_off) * 20 / T_MS(t_on)),                                                          \
		(flag << 4) | (flag_inverted << 3) | (inverted << 2) | (led_default)

#define PIN_LED_MIN 4

void led_update_all(bool force = false);

#if CONFIG_FEAT_ANALOG
void analog_enable(bool enable);
#endif

#if CONFIG_FEAT_AUXDET
bool auxdet_read();
#endif

#if CONFIG_PAUSE_AUX_ON_RADIO || CONFIG_PAUSE_AUX_ON_BT
void aux_btn_play_pause();
#endif

#if CONFIG_FEAT_AUX_RADIO_CONTROLS
void aux_btn_next();
void aux_btn_prev();
#endif

// void aux_btn_vol_up();
// void aux_btn_vol_down();
