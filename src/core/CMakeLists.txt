
set(LIBRARY awsmockcore)
set(LIBRARY_STATIC awsmockcore_static)

set(UTILS_SOURCES src/utils/StringUtils.cpp src/utils/FileUtils.cpp src/utils/DirUtils.cpp src/utils/DateTimeUtils.cpp src/utils/CryptoUtils.cpp src/utils/AwsUtils.cpp
        src/utils/SystemUtils.cpp src/utils/TarUtils.cpp src/utils/RandomUtils.cpp src/utils/TestUtils.cpp src/utils/HttpUtils.cpp src/utils/NumberUtils.cpp
        src/utils/MemoryMappedFile.cpp src/utils/XmlUtils.cpp src/utils/DomainSocket.cpp src/utils/HttpSocket.cpp src/utils/JwtUtils.cpp src/scheduler/PeriodicTask.cpp
        src/scheduler/PeriodicScheduler.cpp src/config/Configuration.cpp src/utils/SortColumn.cpp src/utils/SrpUtils.cpp)
set(EXCEPTION_SOURCES src/exception/CoreException.cpp src/exception/DatabaseException.cpp src/exception/ServiceException.cpp
        src/exception/JsonException.cpp src/exception/NotFoundException.cpp
        src/exception/ForbiddenException.cpp src/exception/UnauthorizedException.cpp src/exception/BadRequestException.cpp)
set(LOGGING_SOURCES src/logging/LogStream.cpp)

set(SOURCES ${UTILS_SOURCES} ${EXCEPTION_SOURCES} ${LOGGING_SOURCES})

# Test directory
add_subdirectory(tst)

include_directories(./include /usr/local/include ${INC_BSON_CXX} ${INC_MONGO_CXX})
link_directories(/usr/local/lib)

add_library(${LIBRARY} SHARED ${SOURCES})
set_target_properties(${LIBRARY} PROPERTIES OUTPUT_NAME ${LIBRARY})
target_link_libraries(${LIBRARY} PUBLIC ${STATIC_LIB} ${LIB_BSON_CXX} ${LIB_MONGO_CXX} boost_locale boost_filesystem boost_url yaml-cpp gtest pthread ssl crypto
        archive z prometheus-cpp-core prometheus-cpp-pull magic)
add_library(${LIBRARY_STATIC} STATIC ${SOURCES})
set_target_properties(${LIBRARY_STATIC} PROPERTIES OUTPUT_NAME ${LIBRARY})
target_link_libraries(${LIBRARY_STATIC} PUBLIC ${STATIC_LIB} ${LIB_BSON_CXX} ${LIB_MONGO_CXX} boost_locale boost_filesystem boost_url yaml-cpp gtest pthread ssl crypto
        archive z prometheus-cpp-core prometheus-cpp-pull magic)
install(TARGETS ${LIBRARY} DESTINATION lib)
