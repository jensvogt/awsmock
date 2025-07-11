cmake_minimum_required(VERSION 3.22)

project(awsmock)

# Defaults
set(CMAKE_CXX_STANDARD 23)
if (WIN32)
    add_definitions(-D_WIN32_WINNT=0x0A00)
    add_compile_options(/MP16 "/Zc:__cplusplus" /EHsc "$<$<CONFIG:RELEASE>:/MT>" "$<$<CONFIG:DEBUG>:/MTd>")
    add_compile_options("$<$<CONFIG:RELEASE>:/O2iy>" "$<$<CONFIG:DEBUG>:/Od>" /bigobj)
    set(BSONCXX_INCLUDE_DIR d:/work/external/include/bsoncxx/v_noabi)
    set(MONGOCXX_INCLUDE_DIR d:/work/external/include/mongocxx/v_noabi)
    set(EXTERNAL_INCLUDE_DIR d:/work/external/include)
    set(EXTERNAL_LIB_DIR d:/work/external/lib)
else ()
    set(CMAKE_CXX_FLAGS "-DBOOST_LOG_DYN_LINK")
    set(BSONCXX_INCLUDE_DIR /usr/local/include/bsoncxx/v_noabi)
    set(MONGOCXX_INCLUDE_DIR /usr/local/include/mongocxx/v_noabi)
    set(EXTERNAL_INCLUDE_DIR /usr/local/include)
    set(EXTERNAL_LIB_DIR /usr/local/lib)
    add_compile_options("$<$<CONFIG:RELEASE>:-O3>" "$<$<CONFIG:DEBUG>:-g>")
    if (APPLE)
        set(CMAKE_MACOSX_RPATH TRUE)
        add_compile_options("-Wno-missing-template-arg-list-after-template-kw")
        add_link_options("-Wl,-no_warn_duplicate_libraries")
    endif ()
endif ()

# Doxygen
if (WIN32)
    option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen and dot) " OFF)
else ()
    option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen and dot) " ON)
endif ()
option(BUILD_SAMPLES "Create and install the samples" OFF)

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

# Build documentation
if (BUILD_DOCUMENTATION)
    add_subdirectory(docs)
endif (BUILD_DOCUMENTATION)

# Build samples lambdas
if (BUILD_SAMPLES)
    #    add_subdirectory(src/samples/lambda/s3-notification-lambda)
endif (BUILD_SAMPLES)

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
install(DIRECTORY DESTINATION ${CMAKE_INSTALL_PREFIX}/data/dynamodb)
install(DIRECTORY DESTINATION ${CMAKE_INSTALL_PREFIX}/data/backup)
install(DIRECTORY DESTINATION ${CMAKE_INSTALL_PREFIX}/data/application)
install(DIRECTORY DESTINATION ${CMAKE_INSTALL_PREFIX}/data/tmp)
install(DIRECTORY dist/samples/ DESTINATION ${CMAKE_INSTALL_PREFIX}/samples)
install(FILES dist/etc/ssh_host_key DESTINATION ${CMAKE_INSTALL_PREFIX}/etc)

if (WIN32)
    install(FILES dist/etc/awsmock_win32.json DESTINATION ${CMAKE_INSTALL_PREFIX}/etc RENAME awsmock.json)
    install(FILES dist/etc/magic.mgc DESTINATION ${CMAKE_INSTALL_PREFIX}/etc)
elseif (LINUX)
    install(FILES dist/etc/awsmock_linux.json DESTINATION ${CMAKE_INSTALL_PREFIX}/etc RENAME awsmock.json)
    install(FILES dist/etc/systemd/system/awsmock.service DESTINATION /etc/systemd/system)
    install(FILES dist/etc/magic_linux.mgc DESTINATION ${CMAKE_INSTALL_PREFIX}/etc RENAME magic.mgc)
elseif (APPLE)
    install(FILES dist/etc/awsmock_macos.json DESTINATION ${CMAKE_INSTALL_PREFIX}/etc RENAME awsmock.json)
    install(FILES dist/macos/de.jensvogt.awsmock.plist DESTINATION /Library/LaunchDaemons)
    install(FILES dist/etc/magic_linux.mgc DESTINATION ${CMAKE_INSTALL_PREFIX}/etc RENAME magic.mgc)
endif ()

