cmake_minimum_required(VERSION 2.8.4)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/ArduinoToolchain.cmake)
set(PROJECT_NAME CDCEmu_Arduino)
project(${PROJECT_NAME})

set(${CMAKE_PROJECT_NAME}_SKETCH CDCEmu_Arduino.ino)

#### Uncomment below additional settings as needed.
set(${CMAKE_PROJECT_NAME}_BOARD uno)
set(${CMAKE_PROJECT_NAME}_PORT com9)
link_directories("c:\\Program Files (x86)\\Arduino\\libraries")
link_directories("c:\\Program Files (x86)\\Arduino\\hardware\\arduino\\avr\\libraries")
link_directories("d:\\Arduino\\libraries")
# set(mega.build.mcu atmega2560)
# set(mega.upload.protocol wiring)
# set(mega.upload.speed 115200)

generate_arduino_firmware(${CMAKE_PROJECT_NAME})
