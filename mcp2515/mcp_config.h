#pragma once

#define MCP_CLKOUT_DISABLE  -1
#define MCP_CLKOUT_DIV1     0x0
#define MCP_CLKOUT_DIV2     0x1
#define MCP_CLKOUT_DIV4     0x2
#define MCP_CLKOUT_DIV8     0x3

#if CONFIG_MCP_8M
    #if CONFIG_MCP_1000K
        #define MCP_CFG     0x00,0x80,0x80
    #elif CONFIG_MCP_500K
        #define MCP_CFG     0x00,0x90,0x82
    #elif CONFIG_MCP_250K
        #define MCP_CFG     0x00,0xB1,0x85
    #elif CONFIG_MCP_200K
        #define MCP_CFG     0x00,0xB4,0x86
    #elif CONFIG_MCP_125K
        #define MCP_CFG     0x01,0xB1,0x85
    #elif CONFIG_MCP_100K
        #define MCP_CFG     0x01,0xB4,0x86
    #elif CONFIG_MCP_80K
        #define MCP_CFG     0x01,0xBF,0x87
    #elif CONFIG_MCP_50K
        #define MCP_CFG     0x03,0xB4,0x86
    #elif CONFIG_MCP_40K
        #define MCP_CFG     0x03,0xBF,0x87
    #elif CONFIG_MCP_33K3
        #define MCP_CFG     0x47,0xE2,0x85
    #elif CONFIG_MCP_31K25
        #define MCP_CFG     0x07,0xA4,0x84
    #elif CONFIG_MCP_20K
        #define MCP_CFG     0x07,0xBF,0x87
    #elif CONFIG_MCP_10K
        #define MCP_CFG     0x0F,0xBF,0x87
    #elif CONFIG_MCP_5K
        #define MCP_CFG     0x1F,0xBF,0x87
    #endif
#endif

#if CONFIG_MCP_16M
    #if CONFIG_MCP_1000K
        #define MCP_CFG     0x00,0xD0,0x82
    #elif CONFIG_MCP_500K
        #define MCP_CFG     0x00,0xF0,0x86
    #elif CONFIG_MCP_250K
        #define MCP_CFG     0x41,0xF1,0x85
    #elif CONFIG_MCP_200K
        #define MCP_CFG     0x01,0xFA,0x87
    #elif CONFIG_MCP_125K
        #define MCP_CFG     0x03,0xF0,0x86
    #elif CONFIG_MCP_100K
        #define MCP_CFG     0x03,0xFA,0x87
    #elif CONFIG_MCP_80K
        #define MCP_CFG     0x03,0xFF,0x87
    #elif CONFIG_MCP_83K3
        #define MCP_CFG     0x03,0xBE,0x07
    #elif CONFIG_MCP_50K
        #define MCP_CFG     0x07,0xFA,0x87
    #elif CONFIG_MCP_40K
        #define MCP_CFG     0x07,0xFF,0x87
    #elif CONFIG_MCP_33K3
        #define MCP_CFG     0x4E,0xF1,0x85
    #elif CONFIG_MCP_20K
        #define MCP_CFG     0x0F,0xFF,0x87
    #elif CONFIG_MCP_10K
        #define MCP_CFG     0x1F,0xFF,0x87
    #elif CONFIG_MCP_5K
        #define MCP_CFG     0x3F,0xFF,0x87
    #endif
#endif

#if CONFIG_MCP_20M
    #if CONFIG_MCP_1000K
        #define MCP_CFG     0x00,0xD9,0x82
    #elif CONFIG_MCP_500K
        #define MCP_CFG     0x00,0xFA,0x87
    #elif CONFIG_MCP_250K
        #define MCP_CFG     0x41,0xFB,0x86
    #elif CONFIG_MCP_200K
        #define MCP_CFG     0x01,0xFF,0x87
    #elif CONFIG_MCP_125K
        #define MCP_CFG     0x03,0xFA,0x87
    #elif CONFIG_MCP_100K
        #define MCP_CFG     0x04,0xFA,0x87
    #elif CONFIG_MCP_83K3
        #define MCP_CFG     0x04,0xFE,0x87
    #elif CONFIG_MCP_80K
        #define MCP_CFG     0x04,0xFF,0x87
    #elif CONFIG_MCP_50K
        #define MCP_CFG     0x09,0xFA,0x87
    #elif CONFIG_MCP_40K
        #define MCP_CFG     0x09,0xFF,0x87
    #elif CONFIG_MCP_33K3
        #define MCP_CFG     0x0B,0xFF,0x87
    #endif
#endif
