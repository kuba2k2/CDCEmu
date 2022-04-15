#include "gpio.h"

#include <avr/pgmspace.h>
#include <pcf8574.h>
//#include <uart.h>
#include "data.h"
#include "main.h"
#include "timers.h"

#define LED_DEF_SIZE  3 // led def size in bytes
#define LED_DEF_COUNT 8 // number of led defs

uint8_t led_timer = 0;
uint8_t leds[3] = {0, 0, 0};
uint8_t leds_set = 0;

bool analog_power = false;

const PROGMEM uint8_t leds_meta[] = {
	// Red LED - radio enabled: 2000ms, 5%
	LED_DEF(PIN_LED_RED, LED_INVERTED, 2000, 100, DATA_RADIO_ENABLED, LED_NORMAL, LED_NO_DEFAULT),
	// Red LED - ignition ON: 100%
	LED_DEF(PIN_LED_RED, LED_NORMAL, 1000, 0, DATA_IGNITION, LED_NORMAL, LED_NO_DEFAULT),
	// Red LED - ignition ON (Power Save): 600ms, 50%
	LED_DEF(PIN_LED_RED, LED_NORMAL, 600, 300, DATA_POWERSAVE, LED_NORMAL, LED_DEFAULT_OFF),

	// Green LED - radio source set: 100%
	LED_DEF(PIN_LED_GREEN, LED_NORMAL, 1000, 0, DATA_RADIO_PLAYING, LED_NORMAL, LED_NO_DEFAULT),
	// Green LED - audio playing: 1000ms, 50%
	LED_DEF(PIN_LED_GREEN, LED_NORMAL, 1000, 500, DATA_AUDIO_PLAYING, LED_NORMAL, LED_DEFAULT_OFF),

	// Blue LED - BT pairing mode: 600ms, 50%
	LED_DEF(PIN_LED_BLUE, LED_NORMAL, 600, 300, DATA_BT_PAIRING, LED_NORMAL, LED_NO_DEFAULT),
	// Blue LED - BT connected: 100%
	LED_DEF(PIN_LED_BLUE, LED_NORMAL, 1000, 0, DATA_BT_CONNECTED, LED_NORMAL, LED_NO_DEFAULT),
	// Blue LED - BT playing: 1000ms, 50%
	LED_DEF(PIN_LED_BLUE, LED_NORMAL, 1000, 500, DATA_BT_PLAYING, LED_NORMAL, LED_DEFAULT_OFF),
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

void led_update_all(bool force) {
	if (!timer_check(TIMER_LEDS, 1) && !force)
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

void analog_enable(bool enable) {
	ensure_i2c();
	analog_power = enable;
	pcf_write(CONFIG_PIN_ANALOG_PWR, !enable);
}
