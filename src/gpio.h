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

#define BTN_A_MASK ((1 << CONFIG_PIN_BTN1) | (1 << CONFIG_PIN_BTN2))

enum btn_action_e {
	NONE = 0,
	PLAY_PAUSE,
	NEXT_TRACK,
	VOLUME_UP,
	VOLUME_DOWN,
	NEXT_TRACK_VOL_UP,
	PREV_TRACK_VOL_DN,
};

void button_set(uint8_t pins, uint8_t value);
void led_update_all(bool force = false);
void gpio_update(bool force = false);
void hs_btn_run(btn_action_e action);

#if CONFIG_FEAT_ANALOG
void analog_enable(bool enable);
#endif

#if CONFIG_FEAT_AUXDET
bool auxdet_read();
#endif

#if CONFIG_FEAT_HS_BTN
void aux_play();
void aux_pause();
void aux_next();
// void aux_prev();
#endif
