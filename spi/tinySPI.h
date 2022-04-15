// Arduino tinySPI Library Copyright (C) 2018 by Jack Christensen and
// licensed under GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html
//
// Arduino hardware SPI master library for
// ATtiny24/44/84, ATtiny25/45/85, ATtiny261/461/861, ATtiny2313/4313.
//
// https://github.com/JChristensen/tinySPI
// Jack Christensen 24Oct2013
//
// Modified to use with CDCEmu by kuba2k2, 23May2021

#pragma once

#include <util/atomic.h>

// SPI data modes
#define SPI_MODE0 0x00
#define SPI_MODE1 0x04

void spi_begin();
void spi_set_data_mode(uint8_t spiDataMode);
uint8_t spi_transfer(uint8_t spiData);
void spi_end();
