cmake_minimum_required(VERSION 3.19)

project(rkdeveloptool VERSION 1.32 LANGUAGES CXX)

set(UDEV_RULES_DIR "lib/udev/rules.d"
    CACHE PATH "Directory to install udev rules.")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(GNUInstallDirs)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBUSB1 REQUIRED libusb-1.0)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/config.h"
)

add_executable(rkdeveloptool
    main.cpp
    crc.cpp
    RKBoot.cpp
    RKComm.cpp
    RKDevice.cpp
    RKImage.cpp
    RKLog.cpp
    RKScan.cpp
)

target_compile_options(rkdeveloptool PRIVATE
    -Wall -Werror -Wextra -Wreturn-type
    -fno-strict-aliasing
    -D_FILE_OFFSET_BITS=64 -D_LARGE_FILE
)

target_include_directories(rkdeveloptool PRIVATE
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${LIBUSB1_INCLUDE_DIRS}"
)

target_link_libraries(rkdeveloptool PRIVATE ${LIBUSB1_LIBRARIES})

install(TARGETS rkdeveloptool RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    install(FILES 99-rk-rockusb.rules DESTINATION "${UDEV_RULES_DIR}")
endif()
