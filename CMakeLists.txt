cmake_minimum_required(VERSION 3.22)
project(base_sbu C)

SET(MCU "ATtiny816")
SET(F_CPU "8000000")
SET(CMAKE_SYSTEM_NAME Generic)# For some reason, these paths have to be absolute, otherwise
# CLion won't be able to find headers etc.
SET(CMAKE_C_COMPILER /usr/bin/avr-gcc)

SET(CMAKE_C_FLAGS "-mmcu=${MCU} -DF_CPU=${F_CPU} -Os")
SET(CMAKE_C_LINK_FLAGS "-mmcu=${MCU}")

target_include_directories(AVR PUBLIC /usr/lib/avr/include)

add_subdirectory(lib/flags)
add_subdirectory(lib/hal)
add_subdirectory(lib/logging)
add_subdirectory(lib/statemachine)
add_subdirectory(lib/usart)


SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_C_STANDARD 99)
set(TARGET_CPU attiny816)

add_executable(base_sbu src/main.c)
