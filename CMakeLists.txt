cmake_minimum_required(VERSION 3.22)
project(awsmock)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_BUILD_PARALLEL_LEVEL 30)

# Doxygen
option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen and dot) " ON)

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

# Set includes and libraries
set(INC_BSON_CXX /usr/local/include/bsoncxx/v_noabi)
set(INC_MONGO_CXX /usr/local/include/mongocxx/v_noabi)
set(LIB_BSON_CXX bsoncxx)
set(LIB_MONGO_CXX mongocxx)
if (APPLE)
    set(CMAKE_MACOSX_RPATH true)
endif ()

add_subdirectory(src/core)
add_subdirectory(src/db)
add_subdirectory(src/dto)
add_subdirectory(src/service)
add_subdirectory(src/manager)
add_subdirectory(src/controller)
add_subdirectory(docs)

install(DIRECTORY DESTINATION /usr/local/awsmock)
install(DIRECTORY DESTINATION /usr/local/awsmock/bin)
install(DIRECTORY DESTINATION /usr/local/awsmock/lib)
install(DIRECTORY DESTINATION /usr/local/awsmock/etc)
install(DIRECTORY DESTINATION /usr/local/awsmock/frontend)
install(DIRECTORY DESTINATION /usr/local/awsmock/samples)
install(DIRECTORY DESTINATION /usr/local/awsmock/data)
install(DIRECTORY DESTINATION /usr/local/awsmock/data/s3)
install(DIRECTORY DESTINATION /usr/local/awsmock/data/sqs)
install(DIRECTORY DESTINATION /usr/local/awsmock/data/sns)
install(DIRECTORY DESTINATION /usr/local/awsmock/data/lambda)
install(DIRECTORY DESTINATION /usr/local/awsmock/data/transfer)
install(DIRECTORY DESTINATION /usr/local/awsmock/data/tmp)
install(FILES dist/etc/awsmock.yml DESTINATION /usr/local/awsmock/etc)
install(FILES dist/bin/awslocal DESTINATION /usr/local/awsmock/bin PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE)
install(DIRECTORY dist/samples/ DESTINATION /usr/local/awsmock/samples)

if (LINUX)
    install(FILES dist/etc/systemd/system/awsmock.service DESTINATION /etc/systemd/system)
elseif (APPLE)
    install(FILES dist/macos/de.jensvogt.awsmock.plist DESTINATION /Library/LaunchDaemons)
endif ()