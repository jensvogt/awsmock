
set(LIBRARY awsmockcore)
set(LIBRARY_STATIC awsmockcore_static)

set(UTILS_SOURCES src/utils/StringUtils.cpp src/utils/FileUtils.cpp src/utils/DirUtils.cpp src/utils/DateTimeUtils.cpp
        src/utils/CryptoUtils.cpp src/utils/AwsUtils.cpp src/utils/SystemUtils.cpp src/utils/TarUtils.cpp
        src/utils/RandomUtils.cpp src/utils/TestUtils.cpp src/utils/HttpUtils.cpp src/utils/NumberUtils.cpp
        src/utils/MemoryMappedFile.cpp src/utils/XmlUtils.cpp src/utils/UnixSocket.cpp src/utils/HttpSocket.cpp
        src/utils/JwtUtils.cpp src/scheduler/PeriodicTask.cpp src/scheduler/PeriodicScheduler.cpp
        src/config/Configuration.cpp src/utils/SortColumn.cpp src/utils/SrpUtils.cpp src/utils/DomainSocket.cpp
        src/utils/WindowsSocket.cpp)
set(EXCEPTION_SOURCES src/exception/CoreException.cpp src/exception/DatabaseException.cpp src/exception/ServiceException.cpp
        src/exception/JsonException.cpp src/exception/NotFoundException.cpp src/exception/ForbiddenException.cpp
        src/exception/UnauthorizedException.cpp src/exception/BadRequestException.cpp)
set(LOGGING_SOURCES src/logging/LogStream.cpp)

set(SOURCES ${UTILS_SOURCES} ${EXCEPTION_SOURCES} ${LOGGING_SOURCES})

# Test directory
if (NOT WIN32)
    add_subdirectory(tst)
endif ()

# Shared library
if (NOT WIN32)
    add_library(${LIBRARY} SHARED ${SOURCES})
    target_include_directories(${LIBRARY} PUBLIC ./include ${BSONCXX_INCLUDE_DIR} ${MONGOCXX_INCLUDE_DIR} ${EXTERNAL_INCLUDE_DIR})
    target_link_libraries(${LIBRARY} PRIVATE mongocxx bsoncxx boost_filesystem boost_url boost_thread boost_locale
            boost_log boost_log_setup yaml-cpp ssl crypto archive magic)
    install(TARGETS ${LIBRARY} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
endif ()

# Static library
add_library(${LIBRARY_STATIC} STATIC ${SOURCES})
target_include_directories(${LIBRARY_STATIC} PUBLIC ./include ${BSONCXX_INCLUDE_DIR} ${MONGOCXX_INCLUDE_DIR}
        ${EXTERNAL_INCLUDE_DIR})
if (NOT WIN32)
    target_link_libraries(${LIBRARY_STATIC} mongocxx bsoncxx boost_filesystem boost_url boost_thread boost_locale
            boost_log boost_log_setup yaml-cpp ssl crypto archive magic)
endif ()
