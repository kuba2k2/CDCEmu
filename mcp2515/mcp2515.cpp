#include "mcp2515.h"
#include "mcp_defines.h"
#include "mcp_private.h"

#include <util/delay.h>
#include <string.h>

void mcp_init() {
    MCP_CS_DDR |= (1<<MCP_CS_PIN);
    mcp_cs_unset();
}

bool mcp_reset() {
    mcp_command(INSTRUCTION_RESET);
    _delay_ms(10);

    uint8_t zeros[14];
    memset(zeros, 0, 14);
    mcp_write_regs(MCP_TXB0CTRL, zeros, 14);
    mcp_write_regs(MCP_TXB1CTRL, zeros, 14);
    mcp_write_regs(MCP_TXB2CTRL, zeros, 14);

    mcp_write_reg(MCP_RXB0CTRL, 0);
    mcp_write_reg(MCP_RXB1CTRL, 0);

    mcp_write_reg(MCP_CANINTE, CANINTF_RX0IF | CANINTF_RX1IF | CANINTF_ERRIF | CANINTF_MERRF | CANINTF_WAKIF);

    mcp_modify_reg(MCP_RXB0CTRL, RXBnCTRL_RXM_MASK | RXB0CTRL_BUKT | RXB0CTRL_FILHIT_MASK, RXBnCTRL_RXM_STDEXT | RXB0CTRL_BUKT | RXB0CTRL_FILHIT);
    mcp_modify_reg(MCP_RXB1CTRL, RXBnCTRL_RXM_MASK | RXB1CTRL_FILHIT_MASK, RXBnCTRL_RXM_STDEXT | RXB1CTRL_FILHIT);

    for (uint8_t i = 0; i < 6; i++) {
        if (!mcp_set_filter(i, i == 1, 0))
            return false;
    }

    for (uint8_t i = 0; i < 2; i++) {
        if (!mcp_set_filter_mask(i, true, 0))
            return false;
    }

    return true;
}

uint8_t mcp_get_status() {
    mcp_cs_set();
    spi_transfer(INSTRUCTION_READ_STATUS);
    uint8_t ret = spi_transfer(0x00);
    mcp_cs_unset();
    return ret;
}

uint8_t mcp_get_mode() {
    return mcp_read_reg(MCP_CANSTAT) & CANSTAT_OPMOD;
}

bool mcp_mode_config() {
    return mcp_set_mode(CANCTRL_REQOP_CONFIG);
}

bool mcp_mode_listen_only() {
    return mcp_set_mode(CANCTRL_REQOP_LISTENONLY);
}

bool mcp_mode_sleep() {
    return mcp_set_mode(CANCTRL_REQOP_SLEEP);
}

bool mcp_mode_loopback() {
    return mcp_set_mode(CANCTRL_REQOP_LOOPBACK);
}

bool mcp_mode_normal() {
    return mcp_set_mode(CANCTRL_REQOP_NORMAL);
}

void mcp_sleep_wait() {
    mcp_clear_interrupts();

    // wait to enter the sleep mode (as per the datasheet)
    while (mcp_get_mode() != MCP_MODE_SLEEP) {
        mcp_mode_sleep();
        _delay_ms(500);
    }

    // wait to exit the sleep mode (incoming packets)
    while (mcp_get_mode() == MCP_MODE_SLEEP) {
        _delay_ms(100);
    }

    mcp_clear_interrupts();
    mcp_mode_normal();
}

bool mcp_set_config(uint8_t cnf1, uint8_t cnf2, uint8_t cnf3) {
    if (!mcp_mode_config())
        return false;

    uint8_t vals[] = {cnf3, cnf2, cnf1};
    mcp_write_regs(MCP_CNF3, vals, 3);
    mcp_read_regs(MCP_CNF3, vals, 3);
    return vals[0] == cnf3 && vals[1] == cnf2 && vals[2] == cnf1;
}

bool mcp_set_bitrate() {
    return mcp_set_config(MCP_CFG);
}

void mcp_set_clk_out(const uint8_t divisor) {
    if (divisor == MCP_CLKOUT_DISABLE) {
        mcp_modify_reg(MCP_CANCTRL, CANCTRL_CLKEN, 0x00);
        mcp_modify_reg(MCP_CNF3, CNF3_SOF, CNF3_SOF);
        return;
    }

    mcp_modify_reg(MCP_CANCTRL, CANCTRL_CLKPRE, divisor);
    mcp_modify_reg(MCP_CANCTRL, CANCTRL_CLKEN, CANCTRL_CLKEN);
    mcp_modify_reg(MCP_CNF3, CNF3_SOF, 0x00);
}

bool mcp_set_filter_mask(const uint8_t mask_num, const bool extended, const uint32_t data) {
    uint8_t reg = MCP_RXM0SIDH + mask_num * 4U;
    return mcp_write_filter(reg, extended, data);
}

bool mcp_set_filter(const uint8_t filter_num, const bool extended, const uint32_t data) {
    uint8_t reg = MCP_RXF0SIDH + filter_num * 4U;
    if (filter_num >= 3)
        reg += 4U;
    return mcp_write_filter(reg, extended, data);
}
