# Project name
set (PROJECT_NAME "rkDevelopTool_Mac")

# The version number.
set (RKDevelopTool_Mac_PACKAGE_VERSION "1.3")

file (GLOB LOCAL_CPP_FILES *.cpp)
set (SOURCE_FILES ${SOURCE_FILES} ${LOCAL_CPP_FILES})

file (GLOB LOCAL_H_FILES *.h)
set (HEADER_FILES ${HEADER_FILES} ${LOCAL_H_FILES})

# Set your libusb path
set (libusb_INCLUDE_DIR "/usr/local/Cellar/libusb/1.0.22/include/libusb-1.0/")
set (libusb_STATIC "/usr/local/Cellar/libusb/1.0.22/lib/")

# Set your libiconv path
set (libiconv_INCLUDE_DIR "/usr/local/opt/libiconv/include/")
set (libiconv_STATIC "/usr/local/opt/libiconv/lib/")

cmake_minimum_required (VERSION 2.6)
project (${PROJECT_NAME})


# Turn off build log
# set (CMAKE_VERBOSE_MAKEFILE ON)

# Configure a header file to pass some of the CMake settings
# to the source code
configure_file (
    "${PROJECT_SOURCE_DIR}/config.h.in"
    "${PROJECT_BINARY_DIR}/config.h"
    )

# add the binary tree to the search path for include files
# so that we will find config.h
include_directories (
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}"
    "${libusb_INCLUDE_DIR}"
    "${libiconv_INCLUDE_DIR}"
    )

link_directories (
    ${libusb_STATIC}
    ${libiconv_STATIC}
    )

add_executable (${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries (
    "${PROJECT_NAME}"
    "${libusb_STATIC}/libusb-1.0.dylib"
    "${libiconv_STATIC}/libiconv.dylib"
    )

