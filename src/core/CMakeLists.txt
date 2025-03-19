
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

include_directories(./include ${BSONCXX_INCLUDE_DIRS} ${MONGOCXX_INCLUDE_DIRS})

# Shared library
if (NOT WIN32)
    add_library(${LIBRARY} SHARED ${SOURCES})
    target_link_libraries(${LIBRARY} PRIVATE
            $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>
            $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>
            Boost::filesystem Boost::url Boost::thread Boost::locale yaml-cpp::yaml-cpp OpenSSL::SSL OpenSSL::SSL
            LibArchive::LibArchive unofficial::libmagic::libmagic)
endif ()

# Static library
add_library(${LIBRARY_STATIC} STATIC ${SOURCES})
target_link_libraries(${LIBRARY_STATIC}
        $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>
        $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>
        Boost::filesystem Boost::url Boost::thread Boost::locale yaml-cpp::yaml-cpp OpenSSL::SSL OpenSSL::SSL
        LibArchive::LibArchive unofficial::libmagic::libmagic)

# Installation
if (NOT WIN32)
    install(TARGETS ${LIBRARY} DESTINATION ${LIB_INSTALL_DIR})
endif ()
