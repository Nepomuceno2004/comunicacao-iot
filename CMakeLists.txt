# Generated Cmake Pico project file
# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PICO_BOARD pico_w CACHE STRING "Board type")
include(pico_sdk_import.cmake)

project(comunicacao-iot C CXX ASM)

pico_sdk_init()

include_directories(${CMAKE_SOURCE_DIR}/lib)

add_executable(comunicacao-iot 
        comunicacao-iot.c 
        lib/matrizLed.c
)

pico_set_program_name(comunicacao-iot "comunicacao-iot")
pico_set_program_version(comunicacao-iot "0.1")

pico_enable_stdio_uart(comunicacao-iot 0)
pico_enable_stdio_usb(comunicacao-iot 1)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/generated)
pico_generate_pio_header(comunicacao-iot
        ${CMAKE_CURRENT_LIST_DIR}/lib/ws2812.pio OUTPUT_DIR 
        ${CMAKE_CURRENT_LIST_DIR}/generated)

target_link_libraries(comunicacao-iot
        pico_stdlib
        hardware_pio
        hardware_gpio
        pico_cyw43_arch_lwip_threadsafe_background
)

target_include_directories(comunicacao-iot PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${PICO_SDK_PATH}/lib/lwip/src/include
        ${PICO_SDK_PATH}/lib/lwip/src/include/arch
        ${PICO_SDK_PATH}/lib/lwip/src/include/lwip
)

target_sources(comunicacao-iot PRIVATE
    ${PICO_SDK_PATH}/lib/lwip/src/apps/http/httpd.c
    ${PICO_SDK_PATH}/lib/lwip/src/apps/http/fs.c
)

pico_add_extra_outputs(comunicacao-iot)