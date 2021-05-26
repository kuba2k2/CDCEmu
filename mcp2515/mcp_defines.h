#ifndef MCP_DEFINES_H
#define MCP_DEFINES_H

#define INSTRUCTION_WRITE           0x02
#define INSTRUCTION_READ            0x03
#define INSTRUCTION_BITMOD          0x05
#define INSTRUCTION_LOAD_TX0        0x40
#define INSTRUCTION_LOAD_TX1        0x42
#define INSTRUCTION_LOAD_TX2        0x44
#define INSTRUCTION_RTS_TX0         0x81
#define INSTRUCTION_RTS_TX1         0x82
#define INSTRUCTION_RTS_TX2         0x84
#define INSTRUCTION_RTS_ALL         0x87
#define INSTRUCTION_READ_RX0        0x90
#define INSTRUCTION_READ_RX1        0x94
#define INSTRUCTION_READ_STATUS     0xA0
#define INSTRUCTION_RX_STATUS       0xB0
#define INSTRUCTION_RESET           0xC0

#define CANINTF_RX0IF               0x01
#define CANINTF_RX1IF               0x02
#define CANINTF_TX0IF               0x04
#define CANINTF_TX1IF               0x08
#define CANINTF_TX2IF               0x10
#define CANINTF_ERRIF               0x20
#define CANINTF_WAKIF               0x40
#define CANINTF_MERRF               0x80

#define EFLG_RX1OVR                 (1<<7)
#define EFLG_RX0OVR                 (1<<6)
#define EFLG_TXBO                   (1<<5)
#define EFLG_TXEP                   (1<<4)
#define EFLG_RXEP                   (1<<3)
#define EFLG_TXWAR                  (1<<2)
#define EFLG_RXWAR                  (1<<1)
#define EFLG_EWARN                  (1<<0)

#define CANCTRL_REQOP               0xE0
#define CANCTRL_ABAT                0x10
#define CANCTRL_OSM                 0x08
#define CANCTRL_CLKEN               0x04
#define CANCTRL_CLKPRE              0x03

#define CANCTRL_REQOP_NORMAL        0x00
#define CANCTRL_REQOP_SLEEP         0x20
#define CANCTRL_REQOP_LOOPBACK      0x40
#define CANCTRL_REQOP_LISTENONLY    0x60
#define CANCTRL_REQOP_CONFIG        0x80
#define CANCTRL_REQOP_POWERUP       0xE0

#define CANSTAT_OPMOD               0xE0
#define CANSTAT_ICOD                0x0E

#define CNF3_SOF                    0x80

#define TXB_EXIDE_MASK              0x08
#define DLC_MASK                    0x0F
#define RTR_MASK                    0x40

#define RXBnCTRL_RXM_STD            0x20
#define RXBnCTRL_RXM_EXT            0x40
#define RXBnCTRL_RXM_STDEXT         0x00
#define RXBnCTRL_RXM_MASK           0x60
#define RXBnCTRL_RTR                0x08
#define RXB0CTRL_BUKT               0x04
#define RXB0CTRL_FILHIT_MASK        0x03
#define RXB1CTRL_FILHIT_MASK        0x07
#define RXB0CTRL_FILHIT             0x00
#define RXB1CTRL_FILHIT             0x01

#define MCP_SIDH                    0
#define MCP_SIDL                    1
#define MCP_EID8                    2
#define MCP_EID0                    3
#define MCP_DLC                     4
#define MCP_DATA                    5

#define STAT_RX0IF                  (1<<0)
#define STAT_RX1IF                  (1<<1)

#define STAT_RXIF_MASK              STAT_RX0IF | STAT_RX1IF

#define TXB_ABTF                    0x40
#define TXB_MLOA                    0x20
#define TXB_TXERR                   0x10
#define TXB_TXREQ                   0x08
#define TXB_TXIE                    0x04
#define TXB_TXP                     0x03

#define EFLG_ERRORMASK              EFLG_RX1OVR | EFLG_RX0OVR | EFLG_TXBO | EFLG_TXEP | EFLG_RXEP

#define MCP_RXF0SIDH                0x00
#define MCP_RXF0SIDL                0x01
#define MCP_RXF0EID8                0x02
#define MCP_RXF0EID0                0x03
#define MCP_RXF1SIDH                0x04
#define MCP_RXF1SIDL                0x05
#define MCP_RXF1EID8                0x06
#define MCP_RXF1EID0                0x07
#define MCP_RXF2SIDH                0x08
#define MCP_RXF2SIDL                0x09
#define MCP_RXF2EID8                0x0A
#define MCP_RXF2EID0                0x0B
#define MCP_CANSTAT                 0x0E
#define MCP_CANCTRL                 0x0F
#define MCP_RXF3SIDH                0x10
#define MCP_RXF3SIDL                0x11
#define MCP_RXF3EID8                0x12
#define MCP_RXF3EID0                0x13
#define MCP_RXF4SIDH                0x14
#define MCP_RXF4SIDL                0x15
#define MCP_RXF4EID8                0x16
#define MCP_RXF4EID0                0x17
#define MCP_RXF5SIDH                0x18
#define MCP_RXF5SIDL                0x19
#define MCP_RXF5EID8                0x1A
#define MCP_RXF5EID0                0x1B
#define MCP_TEC                     0x1C
#define MCP_REC                     0x1D
#define MCP_RXM0SIDH                0x20
#define MCP_RXM0SIDL                0x21
#define MCP_RXM0EID8                0x22
#define MCP_RXM0EID0                0x23
#define MCP_RXM1SIDH                0x24
#define MCP_RXM1SIDL                0x25
#define MCP_RXM1EID8                0x26
#define MCP_RXM1EID0                0x27
#define MCP_CNF3                    0x28
#define MCP_CNF2                    0x29
#define MCP_CNF1                    0x2A
#define MCP_CANINTE                 0x2B
#define MCP_CANINTF                 0x2C
#define MCP_EFLG                    0x2D
#define MCP_TXB0CTRL                0x30
#define MCP_TXB0SIDH                0x31
#define MCP_TXB0SIDL                0x32
#define MCP_TXB0EID8                0x33
#define MCP_TXB0EID0                0x34
#define MCP_TXB0DLC                 0x35
#define MCP_TXB0DATA                0x36
#define MCP_TXB1CTRL                0x40
#define MCP_TXB1SIDH                0x41
#define MCP_TXB1SIDL                0x42
#define MCP_TXB1EID8                0x43
#define MCP_TXB1EID0                0x44
#define MCP_TXB1DLC                 0x45
#define MCP_TXB1DATA                0x46
#define MCP_TXB2CTRL                0x50
#define MCP_TXB2SIDH                0x51
#define MCP_TXB2SIDL                0x52
#define MCP_TXB2EID8                0x53
#define MCP_TXB2EID0                0x54
#define MCP_TXB2DLC                 0x55
#define MCP_TXB2DATA                0x56
#define MCP_RXB0CTRL                0x60
#define MCP_RXB0SIDH                0x61
#define MCP_RXB0SIDL                0x62
#define MCP_RXB0EID8                0x63
#define MCP_RXB0EID0                0x64
#define MCP_RXB0DLC                 0x65
#define MCP_RXB0DATA                0x66
#define MCP_RXB1CTRL                0x70
#define MCP_RXB1SIDH                0x71
#define MCP_RXB1SIDL                0x72
#define MCP_RXB1EID8                0x73
#define MCP_RXB1EID0                0x74
#define MCP_RXB1DLC                 0x75
#define MCP_RXB1DATA                0x76

#endif
