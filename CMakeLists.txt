cmake_minimum_required(VERSION 3.22)

project(awsmock VERSION 1.0 LANGUAGES CXX)

# ----------------------------
# Global settings
# ----------------------------
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_DOCUMENTATION "Build API docs" OFF)
option(BUILD_SAMPLES "Build sample lambdas" OFF)

# ----------------------------
# Platform setup
# ----------------------------
if (WIN32)
    add_definitions(-D_WIN32_WINNT=0x0A00)
    add_compile_options(/MP16 "/Zc:__cplusplus" /EHsc)
else()
    add_compile_options("$<$<CONFIG:RELEASE>:-O3>" "$<$<CONFIG:DEBUG>:-Og>")
endif()

if(APPLE)
    set(CMAKE_MACOSX_RPATH TRUE)
endif()

# ----------------------------
# Dependencies (modern)
# ----------------------------
find_package(Boost REQUIRED COMPONENTS log log_setup filesystem locale url thread)
find_package(OpenSSL REQUIRED)
find_package(LibArchive REQUIRED)
find_package(Threads REQUIRED)

# Mongo C++ driver (bsoncxx/mongocxx)
find_path(BSONCXX_INCLUDE_DIR bsoncxx/v_noabi REQUIRED)
find_path(MONGOCXX_INCLUDE_DIR mongocxx/v_noabi REQUIRED)
find_library(MONGOCXX_LIB mongocxx REQUIRED)
find_library(BSONCXX_LIB bsoncxx REQUIRED)

# ----------------------------
# Version header
# ----------------------------
file(READ "${CMAKE_SOURCE_DIR}/version.txt" PROJECT_VER_RAW)
string(STRIP "${PROJECT_VER_RAW}" PROJECT_VERSION)
string(TIMESTAMP PROJECT_BUILDDATE "%d-%m-%Y")

configure_file(
        "${CMAKE_SOURCE_DIR}/src/core/include/awsmock/core/Version.h.in"
        "${CMAKE_BINARY_DIR}/generated/Version.h"
        @ONLY
)

include_directories(${CMAKE_BINARY_DIR}/generated)

# ----------------------------
# Subdirectories
# ----------------------------
add_subdirectory(src/core)
add_subdirectory(src/db)
add_subdirectory(src/dto)
add_subdirectory(src/service)
add_subdirectory(src/manager)
add_subdirectory(src/controller)
add_subdirectory(src/awslocal)

# ----------------------------
# A CLion-friendly dummy target
# ----------------------------
add_custom_target(awsmock_all)

# ----------------------------
# Installation dirs
# (cleaned, condensed)
# ----------------------------
foreach(dir bin lib etc log data)
    install(DIRECTORY DESTINATION ${CMAKE_INSTALL_PREFIX}/${dir})
endforeach()

