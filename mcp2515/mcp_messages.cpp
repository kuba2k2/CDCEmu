#include "mcp2515.h"
#include "mcp_defines.h"
#include "mcp_private.h"
#include "mcp_messages.h"

#include <string.h>

uint8_t mcp_get_tx_buf() {
    for (uint8_t tx_buf = 0; tx_buf < 3; tx_buf++) {
        uint8_t reg_ctrl = MCP_TXB0CTRL + tx_buf * 0x10;
        if ((mcp_read_reg(reg_ctrl) & TXB_TXREQ) == 0)
            return tx_buf;
    }
    return -1;
}

uint8_t mcp_get_rx_buf() {
    uint8_t status = mcp_get_status();
    if (status & STAT_RX0IF)
        return 0;
    if (status & STAT_RX1IF)
        return 1;
    return -1;
}

uint8_t mcp_get_error() {
    uint8_t error = mcp_read_reg(MCP_EFLG);
    return error & EFLG_ERRORMASK;
}

void mcp_clear_rxb_overflow() {
    mcp_modify_reg(MCP_EFLG, EFLG_RX0OVR | EFLG_RX1OVR, 0);
}

void mcp_clear_tx_interrupts() {
    mcp_modify_reg(MCP_CANINTF, CANINTF_TX0IF | CANINTF_TX1IF | CANINTF_TX2IF, 0);
}

void mcp_clear_interrupts() {
    mcp_write_reg(MCP_CANINTF, 0);
}

void mcp_clear_merrf() {
    mcp_modify_reg(MCP_CANINTF, CANINTF_MERRF, 0);
}

void mcp_clear_errif() {
    mcp_modify_reg(MCP_CANINTF, CANINTF_ERRIF, 0);
}

uint8_t mcp_get_interrupt_mask() {
    return mcp_read_reg(MCP_CANINTE);
}

bool mcp_send(const struct can_message *msg) {
    if (msg->can_dlc > 8)
        return false;
    uint8_t tx_buf = mcp_get_tx_buf();
    if (tx_buf == -1)
        return false;

    uint8_t reg_ctrl = MCP_TXB0CTRL + tx_buf * 0x10;
    uint8_t reg_sidh = MCP_TXB0SIDH + tx_buf * 0x10;

    uint8_t data[13];

    bool extended = msg->can_id & CAN_EFF_FLAG;
    bool rtr = msg->can_id & CAN_RTR_FLAG;
    uint32_t can_id = msg->can_id & (extended ? CAN_EFF_MASK : CAN_SFF_MASK);

    mcp_prepare_id(data, extended, can_id);

    data[MCP_DLC] = msg->can_dlc | (rtr << 6);

    memcpy(&data[MCP_DATA], msg->data, msg->can_dlc);

    mcp_write_regs(reg_sidh, data, 5 + msg->can_dlc);
    mcp_modify_reg(reg_ctrl, TXB_TXREQ, TXB_TXREQ);

    uint8_t tx_ctrl = mcp_read_reg(reg_ctrl);
    if ((tx_ctrl & (TXB_ABTF | TXB_MLOA | TXB_TXERR)) != 0)
        return false;
    return true;
}
