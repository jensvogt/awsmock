cmake_minimum_required(VERSION 3.22)

project(awsmock)

# Defaults
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_PARALLEL_LEVEL 16)
set(X_VCPKG_APPLOCAL_DEPS_INSTALL ON)
if (WIN32)
    add_definitions(-D_WIN32_WINNT=0x0A00)
    add_compile_options(/MP16 "/Zc:__cplusplus" /EHsc)
else ()
    #    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
    set(BSONCXX_INCLUDE_DIR /usr/local/include/bsoncxx/v_noabi)
    set(MONGOCXX_INCLUDE_DIR /usr/local/include/mongocxx/v_noabi)
    if (${CMAKE_BUILD_TYPE} MATCHES Debug)
        add_compile_options("$<$<CONFIG:Debug>:-Og>")
    else ()
        add_compile_options("-O3")
    endif ()
    if (APPLE)
        set(CMAKE_MACOSX_RPATH true)
    endif ()
endif ()

# Doxygen
if (WIN32)
    option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen and dot) " OFF)
else ()
    option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen and dot) " ON)
endif ()

# Current version
file(READ "${CMAKE_SOURCE_DIR}/version.txt" PROJECT_VER)
string(STRIP "${PROJECT_VER}" PROJECT_VERSION)
string(TIMESTAMP PROJECT_BUILDDATE "%d-%m-%Y")
configure_file("${CMAKE_SOURCE_DIR}/src/core/include/awsmock/core/Version.h.in" "${CMAKE_SOURCE_DIR}/src/core/include/awsmock/core/Version.h" @ONLY)

# Build message
message("-- Building ${PROJECT_NAME} v${PROJECT_VERSION} type: ${CMAKE_BUILD_TYPE}, install prefix: ${CMAKE_INSTALL_PREFIX}")

# Set default build type release
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE PATH "" FORCE)
endif ()

# Testing
enable_testing()

add_subdirectory(src/core)
add_subdirectory(src/db)
add_subdirectory(src/dto)
add_subdirectory(src/service)
add_subdirectory(src/manager)
add_subdirectory(src/controller)
add_subdirectory(src/awslocal)
add_subdirectory(docs)

install(DIRECTORY DESTINATION ${CMAKE_INSTALL_PREFIX})
install(DIRECTORY DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(DIRECTORY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(DIRECTORY DESTINATION ${CMAKE_INSTALL_PREFIX}/etc)
install(DIRECTORY DESTINATION ${CMAKE_INSTALL_PREFIX}/log)
install(DIRECTORY DESTINATION ${CMAKE_INSTALL_PREFIX}/frontend)
install(DIRECTORY DESTINATION ${CMAKE_INSTALL_PREFIX}/samples)
install(DIRECTORY DESTINATION ${CMAKE_INSTALL_PREFIX}/data)
install(DIRECTORY DESTINATION ${CMAKE_INSTALL_PREFIX}/data/s3)
install(DIRECTORY DESTINATION ${CMAKE_INSTALL_PREFIX}/data/sqs)
install(DIRECTORY DESTINATION ${CMAKE_INSTALL_PREFIX}/data/sns)
install(DIRECTORY DESTINATION ${CMAKE_INSTALL_PREFIX}/data/lambda)
install(DIRECTORY DESTINATION ${CMAKE_INSTALL_PREFIX}/data/transfer)
install(DIRECTORY DESTINATION ${CMAKE_INSTALL_PREFIX}/data/tmp)
install(DIRECTORY dist/samples/ DESTINATION ${CMAKE_INSTALL_PREFIX}/samples)
install(FILES ${unofficial-libmagic_DICTIONARY} DESTINATION ${CMAKE_INSTALL_PREFIX}/etc)

if (WIN32)
    install(FILES dist/etc/awsmock_win32.yml DESTINATION ${CMAKE_INSTALL_PREFIX}/etc RENAME awsmock.yml)
elseif (LINUX)
    install(FILES dist/etc/awsmock_linux.yml DESTINATION ${CMAKE_INSTALL_PREFIX}/etc RENAME awsmock.yml)
elseif (APPLE)
    install(FILES dist/etc/awsmock_macos.yml DESTINATION ${CMAKE_INSTALL_PREFIX}/etc RENAME awsmock.yml)
endif ()

# Service installation
if (LINUX)
    install(FILES dist/etc/systemd/system/awsmock.service DESTINATION /etc/systemd/system)
elseif (APPLE)
    install(FILES dist/macos/de.jensvogt.awsmock.plist DESTINATION /Library/LaunchDaemons)
endif ()
