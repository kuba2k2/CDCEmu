#pragma once

#define MCP_8MHZ            8000000
#define MCP_16MHZ           16000000
#define MCP_20MHZ           20000000

#define MCP_5KBPS           5000
#define MCP_10KBPS          10000
#define MCP_20KBPS          20000
#define MCP_31K25BPS        31250
#define MCP_33KBPS          33000
#define MCP_40KBPS          40000
#define MCP_50KBPS          50000
#define MCP_80KBPS          80000
#define MCP_83K3BPS         83300
#define MCP_95KBPS          95000
#define MCP_100KBPS         100000
#define MCP_125KBPS         125000
#define MCP_200KBPS         200000
#define MCP_250KBPS         250000
#define MCP_500KBPS         500000
#define MCP_1000KBPS        1000000

#define MCP_CLKOUT_DISABLE  -1
#define MCP_CLKOUT_DIV1     0x0
#define MCP_CLKOUT_DIV2     0x1
#define MCP_CLKOUT_DIV4     0x2
#define MCP_CLKOUT_DIV8     0x3

#ifndef MCP_CS_DDR
#define MCP_CS_DDR          DDRB
#endif
#ifndef MCP_CS_PORT
#define MCP_CS_PORT         PORTB
#endif
#ifndef MCP_CS_PIN
#define MCP_CS_PIN          3
#endif

#ifndef MCP_CLOCK
#define MCP_CLOCK           MCP_8MHZ
#endif

#ifndef MCP_SPEED
#define MCP_SPEED           MCP_500KBPS
#endif



#if MCP_CLOCK == MCP_8MHZ
    #if MCP_SPEED == MCP_1000KBPS
        #define MCP_CFG     0x00,0x80,0x80
    #elif MCP_SPEED == MCP_500KBPS
        #define MCP_CFG     0x00,0x90,0x82
    #elif MCP_SPEED == MCP_250KBPS
        #define MCP_CFG     0x00,0xB1,0x85
    #elif MCP_SPEED == MCP_200KBPS
        #define MCP_CFG     0x00,0xB4,0x86
    #elif MCP_SPEED == MCP_125KBPS
        #define MCP_CFG     0x01,0xB1,0x85
    #elif MCP_SPEED == MCP_100KBPS
        #define MCP_CFG     0x01,0xB4,0x86
    #elif MCP_SPEED == MCP_80KBPS
        #define MCP_CFG     0x01,0xBF,0x87
    #elif MCP_SPEED == MCP_50KBPS
        #define MCP_CFG     0x03,0xB4,0x86
    #elif MCP_SPEED == MCP_40KBPS
        #define MCP_CFG     0x03,0xBF,0x87
    #elif MCP_SPEED == MCP_33K3BPS
        #define MCP_CFG     0x47,0xE2,0x85
    #elif MCP_SPEED == MCP_31K25BPS
        #define MCP_CFG     0x07,0xA4,0x84
    #elif MCP_SPEED == MCP_20KBPS
        #define MCP_CFG     0x07,0xBF,0x87
    #elif MCP_SPEED == MCP_10KBPS
        #define MCP_CFG     0x0F,0xBF,0x87
    #elif MCP_SPEED == MCP_5KBPS
        #define MCP_CFG     0x1F,0xBF,0x87
    #endif
#endif

#if MCP_CLOCK == MCP_16MHZ
    #if MCP_SPEED == MCP_1000KBPS
        #define MCP_CFG     0x00,0xD0,0x82
    #elif MCP_SPEED == MCP_500KBPS
        #define MCP_CFG     0x00,0xF0,0x86
    #elif MCP_SPEED == MCP_250KBPS
        #define MCP_CFG     0x41,0xF1,0x85
    #elif MCP_SPEED == MCP_200KBPS
        #define MCP_CFG     0x01,0xFA,0x87
    #elif MCP_SPEED == MCP_125KBPS
        #define MCP_CFG     0x03,0xF0,0x86
    #elif MCP_SPEED == MCP_100KBPS
        #define MCP_CFG     0x03,0xFA,0x87
    #elif MCP_SPEED == MCP_80KBPS
        #define MCP_CFG     0x03,0xFF,0x87
    #elif MCP_SPEED == MCP_83K3BPS
        #define MCP_CFG     0x03,0xBE,0x07
    #elif MCP_SPEED == MCP_50KBPS
        #define MCP_CFG     0x07,0xFA,0x87
    #elif MCP_SPEED == MCP_40KBPS
        #define MCP_CFG     0x07,0xFF,0x87
    #elif MCP_SPEED == MCP_33K3BPS
        #define MCP_CFG     0x4E,0xF1,0x85
    #elif MCP_SPEED == MCP_20KBPS
        #define MCP_CFG     0x0F,0xFF,0x87
    #elif MCP_SPEED == MCP_10KBPS
        #define MCP_CFG     0x1F,0xFF,0x87
    #elif MCP_SPEED == MCP_5KBPS
        #define MCP_CFG     0x3F,0xFF,0x87
    #endif
#endif

#if MCP_CLOCK == MCP_20MHZ
    #if MCP_SPEED == MCP_1000KBPS
        #define MCP_CFG     0x00,0xD9,0x82
    #elif MCP_SPEED == MCP_500KBPS
        #define MCP_CFG     0x00,0xFA,0x87
    #elif MCP_SPEED == MCP_250KBPS
        #define MCP_CFG     0x41,0xFB,0x86
    #elif MCP_SPEED == MCP_200KBPS
        #define MCP_CFG     0x01,0xFF,0x87
    #elif MCP_SPEED == MCP_125KBPS
        #define MCP_CFG     0x03,0xFA,0x87
    #elif MCP_SPEED == MCP_100KBPS
        #define MCP_CFG     0x04,0xFA,0x87
    #elif MCP_SPEED == MCP_83K3BPS
        #define MCP_CFG     0x04,0xFE,0x87
    #elif MCP_SPEED == MCP_80KBPS
        #define MCP_CFG     0x04,0xFF,0x87
    #elif MCP_SPEED == MCP_50KBPS
        #define MCP_CFG     0x09,0xFA,0x87
    #elif MCP_SPEED == MCP_40KBPS
        #define MCP_CFG     0x09,0xFF,0x87
    #elif MCP_SPEED == MCP_33K3BPS
        #define MCP_CFG     0x0B,0xFF,0x87
    #endif
#endif
