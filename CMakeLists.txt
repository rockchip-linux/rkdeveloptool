cmake_minimum_required(VERSION 3.10.2)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug, Release, or MinSizeRel." FORCE)
    message(STATUS "CMAKE_BUILD_TYPE not set, defaulting to Release.")
endif ()

project(rkdeveloptool
    VERSION "1.32"
    DESCRIPTION "RockChip Development Tool"
    LANGUAGES CXX
    )

find_package(PkgConfig REQUIRED)
pkg_check_modules(USB libusb-1.0 REQUIRED)

set(SRC_FILES
    crc.cpp
    main.cpp
    RKBoot.cpp
    RKComm.cpp
    RKDevice.cpp
    RKImage.cpp
    RKLog.cpp
    RKScan.cpp
    )

set(RKDevelopTool_PACKAGE_VERSION ${PROJECT_VERSION})
configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h" @ONLY)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${USB_LINK_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${USB_INCLUDE_DIRS})

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
