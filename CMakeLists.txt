cmake_minimum_required(VERSION 3.20)

set(CDCEMU_VERSION "3.0.0")
set(CDCEMU_MCU "attiny85" CACHE STRING "Target MCU name (default attiny85)")
set(CDCEMU_F_CPU "16000000" CACHE STRING "Target MCU clock")

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR avr)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(CDCEmu)

set(DIRS . i2c mcp2515 pcf8574 spi uart)
set(SRCS "")
set(INCLUDES "")
set(CFLAGS "-Wall -g -Os -fdata-sections -ffunction-sections -flto -mmcu=${CDCEMU_MCU} -DF_CPU=${CDCEMU_F_CPU} -DVERSION=${CDCEMU_VERSION} -include ${CMAKE_CURRENT_BINARY_DIR}/autoconf.h -include ${CMAKE_CURRENT_SOURCE_DIR}/src/config.h")
set(LDFLAGS "-Wl,--gc-sections")
enable_language(ASM)

foreach(DIR ${DIRS})
    file(GLOB C_SRCS "src/${DIR}/*.c")
    file(GLOB CPP_SRCS "src/${DIR}/*.cpp")
    file(GLOB ASM_SRCS "src/${DIR}/*.S")
    list(APPEND SRCS ${C_SRCS} ${CPP_SRCS} ${ASM_SRCS})
    list(APPEND INCLUDES "src/${DIR}")
endforeach()

set_source_files_properties(${SRCS} PROPERTIES COMPILE_FLAGS ${CFLAGS})
set(CMAKE_EXE_LINKER_FLAGS "${CFLAGS} ${LDFLAGS}")

set(TARGET "CDCEmu_v${CDCEMU_VERSION}_${CDCEMU_MCU}")
set(ELF "${TARGET}.elf")
set(HEX "${TARGET}.hex")
set(SIZE "${TARGET}_size")
set(CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/.config")
set(AUTOCONF "${CMAKE_CURRENT_BINARY_DIR}/autoconf.h")

add_executable(${ELF} ${SRCS})
target_include_directories(${ELF} PUBLIC ${INCLUDES})

execute_process(
        COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/scripts/kconfig_to_h.py ${CONFIG} ${AUTOCONF}
)

add_custom_command(
        OUTPUT ${HEX}
        COMMAND ${CMAKE_OBJCOPY} -j .text -j .data -O ihex ${ELF} ${HEX}
        DEPENDS ${ELF} ${AUTOCONF}
)

add_custom_command(
        OUTPUT ${SIZE}
        COMMAND avr-size -C --mcu=${CDCEMU_MCU} ${ELF}
        DEPENDS ${HEX}
)

add_custom_target(
        ${TARGET}
        DEPENDS ${HEX} ${SIZE}
        SOURCES ${SRCS}
)

add_custom_command(
        OUTPUT "avrdude"
        COMMAND avrdude -p ${CDCEMU_MCU} -c usbasp -U flash:w:${HEX}:i -F -P usb
        DEPENDS ${TARGET}
)

add_custom_target(
        "avrdude_${TARGET}"
        DEPENDS "avrdude"
)
