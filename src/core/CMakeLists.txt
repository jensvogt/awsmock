
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

include_directories(./include ${EXT_INCLUDE} ${INC_BSON_CXX} ${INC_MONGO_CXX})
link_directories(${EXT_LIB})

# Shared library
if (NOT WIN32)
    add_library(${LIBRARY} SHARED ${SOURCES})
    target_link_libraries(${LIBRARY} ${LIB_MONGO_CXX} ${LIB_BSON_CXX} ${LIB_MONGO_C} ${LIB_BSON_C} ${LIB_BOOST_LOCALE}
            ${LIB_BOOST_FILESYSTEM} ${LIB_BOOST_URL} ${LIB_YAML} ${LIB_GTEST} ${LIB_SSL} ${LIB_CRYPTO} ${LIB_ARCHIVE}
            ${LIB_PROMETHEUS} ${LIB_MAGIC})
endif ()

# Static library
add_library(${LIBRARY_STATIC} STATIC ${SOURCES})
target_link_libraries(${LIBRARY_STATIC} ${LIB_MONGO_CXX} ${LIB_BSON_CXX} ${LIB_MONGO_C} ${LIB_BSON_C} ${LIB_BOOST_LOCALE}
        ${LIB_BOOST_FILESYSTEM} ${LIB_BOOST_URL} ${LIB_YAML} ${LIB_GTEST} ${LIB_SSL} ${LIB_CRYPTO} ${LIB_ARCHIVE}
        ${LIB_PROMETHEUS} ${LIB_MAGIC})

# Installation
if (NOT WIN32)
    install(TARGETS ${LIBRARY} DESTINATION ${LIB_INSTALL_DIR})
endif ()
