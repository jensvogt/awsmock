
set(LIBRARY awsmockcore)
set(LIBRARY_STATIC awsmockcore_static)

set(UTILS_SOURCES src/utils/StringUtils.cpp src/utils/FileUtils.cpp src/utils/DirUtils.cpp src/utils/DateTimeUtils.cpp
        src/utils/CryptoUtils.cpp src/utils/AwsUtils.cpp src/utils/SystemUtils.cpp src/utils/TarUtils.cpp
        src/utils/RandomUtils.cpp src/utils/TestUtils.cpp src/utils/HttpUtils.cpp src/utils/NumberUtils.cpp
        src/utils/MemoryMappedFile.cpp src/utils/XmlUtils.cpp src/utils/UnixSocket.cpp src/utils/HttpSocket.cpp
        src/utils/JwtUtils.cpp src/scheduler/PeriodicTask.cpp src/scheduler/Scheduler.cpp src/utils/BackupUtils.cpp
        src/config/Configuration.cpp src/utils/SrpUtils.cpp src/utils/DomainSocket.cpp src/scheduler/CronTask.cpp
        src/utils/WindowsSocket.cpp src/utils/CronUtils.cpp src/scheduler/OneTimeTask.cpp src/utils/ZipUtils.cpp)
set(EXCEPTION_SOURCES src/exception/CoreException.cpp src/exception/DatabaseException.cpp src/exception/ServiceException.cpp
        src/exception/JsonException.cpp src/exception/NotFoundException.cpp src/exception/ForbiddenException.cpp
        src/exception/UnauthorizedException.cpp src/exception/BadRequestException.cpp)
set(LOGGING_SOURCES src/logging/LogStream.cpp src/logging/LogWebsocketSink.cpp)
set(BACKUP_SOURCES src/logging/LogStream.cpp src/logging/LogWebsocketSink.cpp)
set(MONITORING_SOURCES src/monitoring/MonitoringCollector.cpp)

set(SOURCES ${UTILS_SOURCES} ${EXCEPTION_SOURCES} ${LOGGING_SOURCES} ${MONITORING_SOURCES})

# Test directory
if (NOT WIN32)
    add_subdirectory(tst)
endif ()

# Shared library
if (WIN32)
    add_library(${LIBRARY_STATIC} STATIC ${SOURCES})
    target_include_directories(${LIBRARY_STATIC} PUBLIC ./include ${BSONCXX_INCLUDE_DIR} ${MONGOCXX_INCLUDE_DIR}
            ${BOOST_INCLUDE_DIR} ${LIBMAGIC_INCLUDE_DIR} ${LIBARCHIVE_INCLUDE_DIR} ${LIBJWT_INCLUDE_DIR}
            ${LIBSSL_INCLUDE_DIR} ${LIBZIP_INCLUDE_DIR})
    target_link_directories(${LIBRARY_STATIC} PUBLIC ${BOOST_LIB_DIR} ${LIBSSL_LIB_DIR} ${LIBARCHIVE_LIB_DIR} ${LIBMAGIC_LIB_DIR})
    target_link_libraries(${LIBRARY_STATIC} PUBLIC boost_filesystem-vc143-mt-x64-1_89.lib boost_url-vc143-mt-x64-1_89.lib
            boost_locale-vc143-mt-x64-1_89.lib boost_log-vc143-mt-x64-1_89.lib boost_log_setup-vc143-mt-x64-1_89.lib
            boost_process-vc143-mt-x64-1_89.lib libssl.lib libcrypto.lib archive.lib magic.lib)
else ()
    add_library(${LIBRARY} SHARED ${SOURCES})
    target_include_directories(${LIBRARY} PRIVATE ./include ${BSONCXX_INCLUDE_DIR} ${MONGOCXX_INCLUDE_DIR}
            ${EXTERNAL_INCLUDE_DIR})
    target_link_directories(${LIBRARY} PRIVATE ${EXTERNAL_LIB_DIR})
    target_link_libraries(${LIBRARY} PRIVATE boost_filesystem boost_url boost_thread boost_locale boost_log
            boost_log_setup ssl crypto archive magic)
    install(TARGETS ${LIBRARY} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
endif ()
