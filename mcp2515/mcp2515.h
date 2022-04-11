#pragma once

#include <tinySPI.h>
#include "mcp_config.h"
#include "mcp_messages.h"

#define MCP_MODE_NORMAL         0x00
#define MCP_MODE_SLEEP          0x20
#define MCP_MODE_LOOPBACK       0x40
#define MCP_MODE_LISTENONLY     0x60
#define MCP_MODE_CONFIG         0x80

void mcp_init();
bool mcp_reset();
uint8_t mcp_get_status();
uint8_t mcp_get_mode();
bool mcp_mode_config();
bool mcp_mode_listen_only();
bool mcp_mode_sleep();
bool mcp_mode_loopback();
bool mcp_mode_normal();
void mcp_sleep_wait();
bool mcp_set_config(uint8_t cnf1, uint8_t cnf2, uint8_t cnf3);
bool mcp_set_bitrate();
void mcp_set_clk_out(const uint8_t divisor);
bool mcp_set_filter_mask(const uint8_t mask_num, const bool extended, const uint32_t data);
bool mcp_set_filter(const uint8_t filter_num, const bool extended, const uint32_t data);
