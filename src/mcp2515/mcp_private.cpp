#include "mcp2515.h"
#include "mcp_defines.h"
#include "mcp_private.h"

#include <util/delay.h>
#include <string.h>

void mcp_cs_set() {
    MCP_CS_PORT &= ~(1<<MCP_CS_PIN);
}

void mcp_cs_unset() {
    MCP_CS_PORT |= (1<<MCP_CS_PIN);
}

void mcp_command(const uint8_t command) {
    mcp_cs_set();
    spi_transfer(command);
    mcp_cs_unset();
}

uint8_t mcp_read_reg(const uint8_t reg) {
    mcp_cs_set();
    spi_transfer(INSTRUCTION_READ);
    spi_transfer(reg);
    uint8_t ret = spi_transfer(0x00);
    mcp_cs_unset();
    return ret;
}

void mcp_read_regs(const uint8_t reg, uint8_t buf[], const uint8_t count) {
    mcp_cs_set();
    spi_transfer(INSTRUCTION_READ);
    spi_transfer(reg);
    for (uint8_t i = 0; i < count; i++) {
        buf[i] = spi_transfer(0x00);
    }
    mcp_cs_unset();
}

void mcp_write_reg(const uint8_t reg, const uint8_t value) {
    mcp_cs_set();
    spi_transfer(INSTRUCTION_WRITE);
    spi_transfer(reg);
    spi_transfer(value);
    mcp_cs_unset();
}

void mcp_write_regs(const uint8_t reg, const uint8_t buf[], const uint8_t count) {
    mcp_cs_set();
    spi_transfer(INSTRUCTION_WRITE);
    spi_transfer(reg);
    for (uint8_t i = 0; i < count; i++) {
        spi_transfer(buf[i]);
    }
    mcp_cs_unset();
}

void mcp_modify_reg(const uint8_t reg, const uint8_t mask, const uint8_t data) {
    mcp_cs_set();
    spi_transfer(INSTRUCTION_BITMOD);
    spi_transfer(reg);
    spi_transfer(mask);
    spi_transfer(data);
    mcp_cs_unset();
}

void mcp_clear_interrupts() {
    mcp_write_reg(MCP_CANINTF, 0);
}

bool mcp_set_mode(const uint8_t mode) {
    mcp_modify_reg(MCP_CANCTRL, CANCTRL_REQOP, mode);

    uint8_t tries = 0;
    while ((mcp_read_reg(MCP_CANSTAT) & CANSTAT_OPMOD) != mode && tries++ < 20) {
        _delay_ms(10);
    }
    return tries < 20;
}

void mcp_prepare_id(uint8_t *buf, const bool extended, const uint32_t can_id) {
    uint16_t id = (uint16_t)(can_id & 0xFFFF);

    if (extended) {
        buf[MCP_EID0] = (uint8_t)(id & 0xFF);
        buf[MCP_EID8] = (uint8_t)(id >> 8);
        id = (uint16_t)(can_id >> 16);
        buf[MCP_SIDL] = (uint8_t)(id & 0x03);
        buf[MCP_SIDL] += (uint8_t)((id & 0x1C) << 3);
        buf[MCP_SIDL] |= TXB_EXIDE_MASK;
        buf[MCP_SIDH] = (uint8_t)(id >> 5);
    } else {
        buf[MCP_SIDH] = (uint8_t)(id >> 3);
        buf[MCP_SIDL] = (uint8_t)((id & 0x07) << 5);
        buf[MCP_EID0] = 0;
        buf[MCP_EID8] = 0;
    }
}

bool mcp_write_filter(const uint8_t reg, const bool extended, const uint32_t data) {
    if (!mcp_mode_config())
        return false;

    uint8_t buf[4];
    mcp_prepare_id(buf, extended, data);
    mcp_write_regs(reg, buf, 4);

    uint8_t buf2[4];
    mcp_read_regs(reg, buf2, 4);

    // ignore the EXIDE bit of SIDL when comparing filter mask registers
    buf[MCP_SIDL] &= ~TXB_EXIDE_MASK;
    buf2[MCP_SIDL] &= ~TXB_EXIDE_MASK;

    return memcmp(buf, buf2, 4) == 0;
}
