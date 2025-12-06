set(LIBRARY awsmockcore)

file(GLOB_RECURSE SOURCES
        src/utils/*.cpp
        src/exception/*.cpp
        src/logging/*.cpp
        src/monitoring/*.cpp
        src/scheduler/*.cpp
        src/config/*.cpp
)

add_library(${LIBRARY} SHARED ${SOURCES})

target_include_directories(${LIBRARY}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${BSONCXX_INCLUDE_DIR}
        ${MONGOCXX_INCLUDE_DIR}
)

target_link_libraries(${LIBRARY}
        PUBLIC
        Boost::log
        Boost::log_setup
        Boost::filesystem
        Boost::url
        Boost::locale
        Boost::thread
        OpenSSL::SSL
        OpenSSL::Crypto
        Threads::Threads
        LibArchive::LibArchive
        ${MONGOCXX_LIB}
        ${BSONCXX_LIB}
)

install(TARGETS ${LIBRARY} DESTINATION lib)
