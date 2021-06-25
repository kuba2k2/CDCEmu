#ifndef MCP_PRIVATE_H
#define MCP_PRIVATE_H

void mcp_cs_set();
void mcp_cs_unset();
void mcp_command(const uint8_t command);
uint8_t mcp_read_reg(const uint8_t reg);
void mcp_read_regs(const uint8_t reg, uint8_t buf[], const uint8_t count);
void mcp_write_reg(const uint8_t reg, const uint8_t value);
void mcp_write_regs(const uint8_t reg, const uint8_t buf[], const uint8_t count);
void mcp_modify_reg(const uint8_t reg, const uint8_t mask, const uint8_t data);
void mcp_clear_interrupts();
bool mcp_set_mode(const uint8_t mode);
void mcp_prepare_id(uint8_t *buf, const bool extended, const uint32_t can_id);
bool mcp_write_filter(const uint8_t reg, const bool extended, const uint32_t data);

#endif
