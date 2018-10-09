cmake_minimum_required(VERSION 3.2)
project(bbr18_rk_mbed_firmware)

set(MBED_PATH mbed/7130f322cb7e)

# It's best to hide all the details of setting up the variable SRCS in a CMake
# macro. The macro can then be called in all the project CMake list files to add
# sources.
#
# The macro first computes the path of the source file relative to the project
# root for each argument. If the macro is invoked from inside a project sub
# directory the new value of the variable SRCS needs to be propagated to the
# parent folder by using the PARENT_SCOPE option.
#
# Source: http://stackoverflow.com/questions/7046956/populating-srcs-from-cmakelists-txt-in-subdirectories
macro (add_sources)
    file (RELATIVE_PATH _relPath "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
    foreach (_src ${ARGN})
        if (_relPath)
            list (APPEND SRCS "${_relPath}/${_src}")
        else()
            list (APPEND SRCS "${_src}")
        endif()
    endforeach()
    if (_relPath)
        # propagate to parent directory
        set (SRCS ${SRCS} PARENT_SCOPE)
    endif()
endmacro()

set(TOOLCHAIN armgcc)

set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(SIZE_COMMAND arm-none-eabi-size)
set(OBJCOPY_COMMAND arm-none-eabi-objcopy)

message(STATUS "C compiler  : ${CMAKE_C_COMPILER}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "Size command: ${SIZE_COMMAND}")
message(STATUS "Main target : ${MAIN_TARGET}")

set(MAIN_TARGET ${PROJECT_NAME}.elf)
enable_language(ASM)

add_sources(main.cpp)
add_sources(CisecoManager/CisecoManager.cpp)
add_sources(PixelArray/neopixel.cpp)
add_sources(MotorDriverManagerRS485/MotorDriverManagerRS485.cpp)
add_sources(LedManager/LedManager.cpp)
add_sources(QED/qed.cpp)
add_sources(Motor/motor.cpp)

include_directories(
        CisecoManager
        TFMini
        LedManager
        MotorDriverManagerRS485
        PixelArray
        QED
        Motor
        .
        mbed-os
        mbed-os/cmsis
        mbed-os/cmsis/TARGET_CORTEX_M
        mbed-os/cmsis/TARGET_CORTEX_M/TOOLCHAIN_GCC
        mbed-os/drivers
        mbed-os/events
        mbed-os/events/equeue
        mbed-os/features
        mbed-os/features/FEATURE_LWIP
        mbed-os/features/FEATURE_LWIP/lwip-interface
        mbed-os/features/FEATURE_LWIP/lwip-interface/lwip
        mbed-os/features/FEATURE_LWIP/lwip-interface/lwip-eth
        mbed-os/features/FEATURE_LWIP/lwip-interface/lwip-eth/arch
        mbed-os/features/FEATURE_LWIP/lwip-interface/lwip-eth/arch/TARGET_NXP
        mbed-os/features/FEATURE_LWIP/lwip-interface/lwip-sys
        mbed-os/features/FEATURE_LWIP/lwip-interface/lwip-sys/arch
        mbed-os/features/FEATURE_LWIP/lwip-interface/lwip/src
        mbed-os/features/FEATURE_LWIP/lwip-interface/lwip/src/api
        mbed-os/features/FEATURE_LWIP/lwip-interface/lwip/src/core
        mbed-os/features/FEATURE_LWIP/lwip-interface/lwip/src/core/ipv4
        mbed-os/features/FEATURE_LWIP/lwip-interface/lwip/src/core/ipv6
        mbed-os/features/FEATURE_LWIP/lwip-interface/lwip/src/include
        mbed-os/features/FEATURE_LWIP/lwip-interface/lwip/src/include/lwip
        mbed-os/features/FEATURE_LWIP/lwip-interface/lwip/src/include/lwip/priv
        mbed-os/features/FEATURE_LWIP/lwip-interface/lwip/src/include/lwip/prot
        mbed-os/features/FEATURE_LWIP/lwip-interface/lwip/src/include/netif
        mbed-os/features/FEATURE_LWIP/lwip-interface/lwip/src/include/netif/ppp
        mbed-os/features/FEATURE_LWIP/lwip-interface/lwip/src/include/netif/ppp/polarssl
        mbed-os/features/FEATURE_LWIP/lwip-interface/lwip/src/netif
        mbed-os/features/FEATURE_LWIP/lwip-interface/lwip/src/netif/ppp
        mbed-os/features/FEATURE_LWIP/lwip-interface/lwip/src/netif/ppp/polarssl
        mbed-os/features/filesystem
        mbed-os/features/filesystem/bd
        mbed-os/features/filesystem/fat
        mbed-os/features/filesystem/fat/ChaN
        mbed-os/features/frameworks
        mbed-os/features/frameworks/greentea-client
        mbed-os/features/frameworks/greentea-client/greentea-client
        mbed-os/features/frameworks/greentea-client/source
        mbed-os/features/frameworks/unity
        mbed-os/features/frameworks/unity/source
        mbed-os/features/frameworks/unity/unity
        mbed-os/features/frameworks/utest
        mbed-os/features/frameworks/utest/source
        mbed-os/features/frameworks/utest/utest
        mbed-os/features/mbedtls
        mbed-os/features/mbedtls/importer
        mbed-os/features/mbedtls/inc
        mbed-os/features/mbedtls/inc/mbedtls
        mbed-os/features/mbedtls/platform
        mbed-os/features/mbedtls/platform/inc
        mbed-os/features/mbedtls/platform/src
        mbed-os/features/mbedtls/src
        mbed-os/features/mbedtls/targets
        mbed-os/features/nanostack
        mbed-os/features/netsocket
        mbed-os/features/netsocket/cellular
        mbed-os/features/netsocket/cellular/generic_modem_driver
        mbed-os/features/netsocket/cellular/utils
        mbed-os/features/storage
        mbed-os/hal
        mbed-os/hal/storage_abstraction
        mbed-os/platform
        mbed-os/rtos
        mbed-os/rtos/rtx4
        mbed-os/rtos/rtx5
        mbed-os/rtos/rtx5/TARGET_CORTEX_M
        mbed-os/rtos/rtx5/TARGET_CORTEX_M/TARGET_M3
        mbed-os/rtos/rtx5/TARGET_CORTEX_M/TARGET_M3/TOOLCHAIN_GCC
        mbed-os/targets
        mbed-os/targets/TARGET_NXP
        mbed-os/targets/TARGET_NXP/TARGET_LPC176X
        mbed-os/targets/TARGET_NXP/TARGET_LPC176X/TARGET_MBED_LPC1768
        mbed-os/targets/TARGET_NXP/TARGET_LPC176X/device
        mbed-os/targets/TARGET_NXP/TARGET_LPC176X/device/TOOLCHAIN_GCC_ARM
)

add_definitions(-DDEVICE_SERIAL)
add_definitions(-DDEVICE_PWMOUT)

add_executable(${MAIN_TARGET} ${SRCS})