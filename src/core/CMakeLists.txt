
set(LIBRARY awsmockcore)
set(LIBRARY_STATIC awsmockcore_static)
set(HAS_REMOTE_API 0)

set(UTILS_SOURCES src/utils/StringUtils.cpp src/utils/FileUtils.cpp src/utils/DirUtils.cpp src/utils/DateTimeUtils.cpp src/utils/CryptoUtils.cpp src/utils/AwsUtils.cpp
        src/utils/SystemUtils.cpp src/utils/TarUtils.cpp src/utils/RandomUtils.cpp src/utils/JsonUtils.cpp src/config/Configuration.cpp src/utils/TestUtils.cpp
        src/utils/HttpUtils.cpp src/utils/NumberUtils.cpp src/utils/MemoryMappedFile.cpp src/utils/Task.cpp src/utils/XmlUtils.cpp src/utils/Timer.cpp
        src/utils/TaskPool.cpp src/utils/DomainSocket.cpp src/utils/HttpSocket.cpp src/utils/JwtUtils.cpp src/date/tz.cpp)
set(EXCEPTION_SOURCES src/exception/CoreException.cpp src/exception/DatabaseException.cpp src/exception/ServiceException.cpp
        src/exception/JsonException.cpp src/exception/NotFoundException.cpp
        src/exception/ForbiddenException.cpp src/exception/UnauthorizedException.cpp src/exception/BadRequestException.cpp)
set(LOGGING_SOURCES src/logging/LogStream.cpp)

set(SOURCES ${UTILS_SOURCES} ${EXCEPTION_SOURCES} ${LOGGING_SOURCES})

# Test directory
add_subdirectory(tst)

# Includes
include_directories(./include /usr/local/include)

add_library(${LIBRARY} SHARED ${SOURCES}
        include/awsmock/core/ThreadPool.h
        include/awsmock/core/scheduler/PeriodicTask.h
        include/awsmock/core/scheduler/PeriodicScheduler.h)
add_library(${LIBRARY_STATIC} STATIC ${SOURCES})
set_target_properties(${LIBRARY_STATIC} PROPERTIES OUTPUT_NAME ${LIBRARY})

install(TARGETS ${LIBRARY} DESTINATION lib)
