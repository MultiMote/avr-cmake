cmake_minimum_required(VERSION 2.8.4)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER avr-gcc)
set(CMAKE_CXX_COMPILER avr-g++)

set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Only release build type for AVR" FORCE)

#============================================================================================

PROJECT(avr-cmake)

set(AVR_DEVICE atmega8)
set(AVR_FREQ 16000000)
set(AVR_EXTRACT_EEP false)

set(AVRDUDE_DEVICE m8)
set(AVRDUDE_PROGRAMMER usbasp)
set(AVRDUDE_PORT usb)

set(SOURCE_FILES
    main.c
    )

#============================================================================================

find_program(AVR_OBJCOPY_EXECUTABLE avr-objcopy)
find_program(AVR_SIZE_EXECUTABLE avr-size)

if(NOT AVR_OBJCOPY_EXECUTABLE)
    message(FATAL_ERROR "avr-objcopy not found!")
endif()

if(NOT AVR_SIZE_EXECUTABLE)
    message(FATAL_ERROR "avr-size not found!")
endif()

set(CMAKE_C_FLAGS  "-mmcu=${AVR_DEVICE} -DF_CPU=${AVR_FREQ}UL -Wl,--gc-sections")
set(CMAKE_CXX_FLAGS "-mmcu=${AVR_DEVICE} -DF_CPU=${AVR_FREQ}UL -Wl,--gc-sections")

#============================================================================================

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 99)

set(OUTPUT_FILE_PATH ${PROJECT_BINARY_DIR}/${PROJECT_NAME})
set(HEX_PATH ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
# Generate HEX
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${AVR_OBJCOPY_EXECUTABLE} -O ihex -R.eeprom ${OUTPUT_FILE_PATH} ${HEX_PATH})

# Generate eeprom firmware
if(AVR_EXTRACT_EEP)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                       COMMAND ${AVR_OBJCOPY_EXECUTABLE} -O ihex -j .eeprom --set-section-flags=.eeprom="alloc,load"  --change-section-lma .eeprom=0 ${OUTPUT_FILE_PATH} ${OUTPUT_FILE_PATH}.eep)
endif()

# Print memory usage
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${AVR_SIZE_EXECUTABLE} ${OUTPUT_FILE_PATH} --mcu=${AVR_DEVICE} --format=avr)

#============================================================================================

find_program(AVRDUDE_EXECUTABLE avrdude)

# Add flash target only if avrdude found
if (AVRDUDE_EXECUTABLE)
    add_custom_target(
        flash
        COMMAND ${AVRDUDE_EXECUTABLE} -s -c ${AVRDUDE_PROGRAMMER} -p ${AVRDUDE_DEVICE} -V -P ${AVRDUDE_PORT} -U "flash:w:${HEX_PATH}:a"
        DEPENDS ${PROJECT_NAME}
    )
else()
    message(WARNING "AVRDude not found, flash target is not available")
endif()
