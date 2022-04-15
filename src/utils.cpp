// Copyright (c) Kuba Szczodrzyński 2022-4-15.

#include "utils.h"

#include <mcp2515.h>
#include <pcf8574.h>
#include <util/delay.h>

#include "can.h"
#include "data.h"
#include "gpio.h"

#define IO_SPI 1
#define IO_I2C 2

uint8_t io_mode = 0;

void ensure_spi() {
	if (io_mode != IO_SPI) {
		mcp_init();
		spi_begin();
		io_mode = IO_SPI;
	}
}

void ensure_i2c() {
	if (io_mode != IO_I2C) {
		spi_end();
		i2c_init();
		io_mode = IO_I2C;
	}
}

void enter_sleep() {
	led_update_all(true);

	// ensure_spi();
	// mcp_sleep_wait();
	while (!data[DATA_IGNITION]) {
		can_receive_all();
		_delay_ms(100);
	}

	led_update_all(true);
}
