#include "gpio.h"

#include <avr/pgmspace.h>
#include <pcf8574.h>
#include <uart.h>

#include "data.h"
#include "timers.h"
#include "utils.h"
#include "events.h"

#define LED_DEF_SIZE  3 // led def size in bytes
#define LED_DEF_COUNT 8 // number of led defs

uint8_t led_timer = 0;
uint8_t leds[3] = {0, 0, 0};
uint8_t leds_set = 0;

btn_action_e btn_action = NONE; // the currently performed action
uint8_t btn_action_length = 0;	// elapsed time of this action, in ticks
uint8_t gpio_tick_cnt = 0;

const PROGMEM uint8_t leds_meta[] = {
	// Red LED - radio enabled: 2000ms, 5%
	LED_DEF(CONFIG_PIN_LED_RED, LED_INVERTED, 2000, 100, DATA_RADIO_ENABLED, LED_NORMAL, LED_NO_DEFAULT),
	// Red LED - ignition ON: 100%
	LED_DEF(CONFIG_PIN_LED_RED, LED_NORMAL, 1000, 0, DATA_IGNITION, LED_NORMAL, LED_NO_DEFAULT),
	// Red LED - ignition ON (Power Save): 600ms, 50%
	LED_DEF(CONFIG_PIN_LED_RED, LED_NORMAL, 600, 300, DATA_POWERSAVE, LED_NORMAL, LED_DEFAULT_OFF),

	// Green LED - audio playing: 1000ms, 50%
	LED_DEF(CONFIG_PIN_LED_GREEN, LED_NORMAL, 1000, 500, DATA_AUDIO_PLAYING, LED_NORMAL, LED_NO_DEFAULT),
	// Green LED - radio source set: 100%
	LED_DEF(CONFIG_PIN_LED_GREEN, LED_NORMAL, 1000, 0, DATA_RADIO_PLAYING, LED_NORMAL, LED_DEFAULT_OFF),

	// Blue LED - BT pairing mode: 600ms, 50%
	LED_DEF(CONFIG_PIN_LED_BLUE, LED_NORMAL, 600, 300, DATA_BT_PAIRING, LED_NORMAL, LED_NO_DEFAULT),
	// Blue LED - BT playing: 1000ms, 50%
	LED_DEF(CONFIG_PIN_LED_BLUE, LED_NORMAL, 1000, 500, DATA_BT_PLAYING, LED_NORMAL, LED_NO_DEFAULT),
	// Blue LED - BT connected: 100%
	LED_DEF(CONFIG_PIN_LED_BLUE, LED_NORMAL, 1000, 0, DATA_BT_CONNECTED, LED_NORMAL, LED_DEFAULT_OFF),
};

void led_set(uint8_t pin, bool state, bool inverted) {
	ensure_i2c();
	pcf_write(pin, state ^ inverted ^ 1);
	leds[pin - PIN_LED_MIN] = (state << 7) | led_timer;
	// uart_puts_P("led ");
	// uart_putc(0x2C + pin);
	// uart_puts_P(" state ");
	// uart_putc(0x30 + (inverted ^ state));
	// uart_putc('\n');
}

void button_set(uint8_t pins, uint8_t value) {
	ensure_i2c();
	pcf_write_mask(pins, value);
}

void led_update_all(bool force) {
	if (!timer_check(TIMER_LEDS, T_MS(100)) && !force)
		return;
	if (++led_timer >= 100)
		led_timer = 0;

	leds_set = 0;

	for (uint8_t i = 0; i < LED_DEF_COUNT; i++) {
		uint8_t *led = (uint8_t *)leds_meta + LED_DEF_SIZE * i;
		uint8_t meta = pgm_read_byte(&led[1]);
		uint8_t flags = pgm_read_byte(&led[2]);

		uint8_t pin = meta >> 5;
		bool inverted = flags & 1 << 2;

		// already handled in this round
		if (leds_set & (1 << pin))
			continue;

		// get the current led state
		uint8_t timer = leds[pin - PIN_LED_MIN];
		bool is_on = timer & 1 << 7;
		timer &= 0x7f;

		// check the flag
		uint8_t flag = flags >> 4;
		bool flag_inverted = flags & 1 << 3;
		if (!(data[flag] ^ flag_inverted)) {
			uint8_t led_default = flags & 0b11;
			if (led_default && is_on != (--led_default ^ inverted)) {
				led_set(pin, led_default ^ inverted, inverted);
			}
			continue; // skip to the next LED_DEF
		}

		leds_set |= (1 << pin);

		// read the cycle times
		uint8_t t_off = meta & 0b11111;
		uint8_t t_on = pgm_read_byte(&led[0]);
		t_off = t_on * t_off / 20;
		t_on -= t_off;

		bool is_reset = led_timer < timer;
		uint8_t elapsed = led_timer - timer;
		// enable the led at first round
		if (timer == 0 || !t_off) {
			led_set(pin, true, inverted);
		}
		// disable the led when t_on elapses
		else if (is_on && (elapsed >= t_on || is_reset)) {
			led_set(pin, false, inverted);
		}
		// enable the led when t_off elapses
		else if (!is_on && (elapsed >= t_off || is_reset)) {
			led_set(pin, true, inverted);
		}
	}
}

void gpio_update(bool force) {
	if (!timer_check(TIMER_GPIO, T_MS(CONFIG_GPIO_TICK_MS)) && !force)
		return;
#if CONFIG_FEAT_AUXDET
	gpio_tick_cnt++;
	if (gpio_tick_cnt % 6 == 0 && !btn_action) {
		// check audio state only if no action is running
		if (DATA(AUDIO_PLAYING) != auxdet_read()) {
			DATA(AUDIO_PLAYING) ^= 1;
			on_aux_playing(DATA(AUDIO_PLAYING));
		}
		// release all buttons periodically, just in case
		button_set(BTN_A_MASK, 0x00);
		gpio_tick_cnt = 1;
	}
#endif
#if CONFIG_FEAT_HS_BTN
	switch (btn_action) {
		case NONE:
			break;
		case PLAY_PAUSE:
			if (btn_action_length == 0) {
				button_set(BTN_A_MASK, 0xff); // press the A button
			} else {
				button_set(BTN_A_MASK, 0x00); // release the A button
				btn_action = NONE;
			}
			gpio_tick_cnt = 0; // delay auxdet by 500ms more
			break;
		case NEXT_TRACK:
			if (btn_action_length & 1) {
				// 1, 3
				button_set(BTN_A_MASK, 0x00); // release the A button
			} else if (btn_action_length < 4) {
				// 0, 2
				button_set(BTN_A_MASK, 0xff); // press the A button
			} else {
				// any other value
				btn_action = NONE;
			}
			break;
		case VOLUME_UP:
		case VOLUME_DOWN:
		case NEXT_TRACK_VOL_UP:
		case PREV_TRACK_VOL_DN:
			break;
	}
	btn_action_length++;
#endif
}

void hs_btn_run(btn_action_e action) {
	btn_action = action;
	btn_action_length = 0;
	gpio_update(true);
	timer_reset(TIMER_GPIO);
}

void analog_enable(bool enable) {
	if (DATA(ANALOG_POWER) == enable)
		return;
	ensure_i2c();
	if (!enable) {
		DATA(BT_PAIRING) = false;
		DATA(BT_CONNECTED) = false;
		DATA(BT_PLAYING) = false;
	}
	debug_puts_P("analog pwr ");
	debug_putc(enable + '0');
	debug_nl();
	pcf_write(CONFIG_PIN_ANALOG_PWR, !enable);
	DATA(ANALOG_POWER) = enable;
}

bool auxdet_read() {
	ensure_i2c();
	return !pcf_read(CONFIG_PIN_AUXDET);
}

void aux_play() {
	if (DATA(AUDIO_PLAYING) || btn_action)
		return;
	DATA(AUDIO_PLAYING) = true;
	hs_btn_run(PLAY_PAUSE);
}

void aux_pause() {
	if (!DATA(AUDIO_PLAYING) || btn_action)
		return;
	DATA(AUDIO_PLAYING) = false;
	hs_btn_run(PLAY_PAUSE);
}

void aux_next() {
	if (!DATA(AUDIO_PLAYING) || btn_action)
		return;
	hs_btn_run(NEXT_TRACK);
}
