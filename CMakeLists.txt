cmake_minimum_required(VERSION 3.13)

# initialize pico-sdk from GIT
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_FETCH_FROM_GIT on)

# pico_sdk_import.cmake is a single file copied from this SDK
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(pico_ns2jb_bootstrap)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_compile_options(-Werror -Wall)

##############################

add_executable(pico_ns2jb_bootstrap)

target_sources(pico_ns2jb_bootstrap PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usb_descriptors.c
    #${CMAKE_CURRENT_SOURCE_DIR}/src/bootstrap.bas.h
)

#add_custom_command(
#  OUTPUT ${PROJECT_BINARY_DIR}/src/bootstrap.bas.h
#  COMMAND bash -c "xxd -i ${CMAKE_CURRENT_SOURCE_DIR}/src/bootstrap.bas > ${CMAKE_CURRENT_SOURCE_DIR}/src/bootstrap.bas.h"
#  VERBATIM
#)


target_include_directories(pico_ns2jb_bootstrap PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

#target_compile_definitions(pico_ns2jb_bootstrap PUBLIC CFG_TUSB_OS=OPT_OS_PICO)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(pico_ns2jb_bootstrap PUBLIC pico_stdlib tinyusb_device tinyusb_board)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(pico_ns2jb_bootstrap)
