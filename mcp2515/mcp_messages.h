#ifndef MCP_MESSAGES_H
#define MCP_MESSAGES_H

/*
 * Controller Area Network Identifier structure
 *
 * bit 0-28 : CAN identifier (11/29 bit)
 * bit 29   : error message frame flag (0 = data frame, 1 = error message)
 * bit 30   : remote transmission request flag (1 = rtr frame)
 * bit 31   : frame format flag (0 = standard 11 bit, 1 = extended 29 bit)
 */
struct can_message {
    uint32_t    can_id; /* 32 bit CAN_ID + EFF/RTR/ERR flags */
    uint8_t     can_dlc; /* frame payload length in byte (0..8) */
    uint8_t     data[8] __attribute__((aligned(8)));
};

/* special address description flags for the CAN_ID */
#define CAN_EFF_FLAG 0x80000000UL /* EFF/SFF is set in the MSB */
#define CAN_RTR_FLAG 0x40000000UL /* remote transmission request */
#define CAN_ERR_FLAG 0x20000000UL /* error message frame */

/* valid bits in CAN ID for frame formats */
#define CAN_SFF_MASK 0x000007FFUL /* standard frame format (SFF) */
#define CAN_EFF_MASK 0x1FFFFFFFUL /* extended frame format (EFF) */

bool mcp_send(const struct can_message *msg);
bool mcp_can_receive();
bool mcp_receive(struct can_message *msg);

#endif
