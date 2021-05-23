#ifndef MCP2515_H
#define MCP2515_H

#include <tinySPI.h>
#include "mcp_config.h"

void mcp_init();
bool mcp_reset();
uint8_t mcp_get_status();
bool mcp_mode_config();
bool mcp_mode_listen_only();
bool mcp_mode_sleep();
bool mcp_mode_loopback();
bool mcp_mode_normal();
bool mcp_set_config(uint8_t cnf1, uint8_t cnf2, uint8_t cnf3);
bool mcp_set_bitrate();
void mcp_set_clk_out(const uint8_t divisor);
bool mcp_set_filter_mask(const uint8_t mask_num, const bool extended, const uint32_t data);
bool mcp_set_filter(const uint8_t filter_num, const bool extended, const uint32_t data);

#endif
