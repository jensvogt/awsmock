set(BINARY core_tst)
set(STATIC_LIB awsmockcore)

set(SOURCES CryptoTests.cpp FileUtilsTests.cpp DirUtilsTests.cpp StringUtilsTests.cpp ConfigurationTests.cpp
        RandomUtilsTests.cpp AwsUtilsTests.cpp HttpUtilsTests.cpp SystemUtilsTests.cpp XmlUtilsTests.cpp
        JwtUtilsTests.cpp TarUtilsTests.cpp DateTimeUtilsTests.cpp main.cpp)

add_executable(${BINARY} ${SOURCES})
target_include_directories(${BINARY} PUBLIC ./include ${BSONCXX_INCLUDE_DIR} ${MONGOCXX_INCLUDE_DIR})
target_link_directories(${BINARY} PUBLIC ${EXTERNAL_LIB_DIR})
target_link_libraries(${BINARY} awsmockcore mongocxx bsoncxx boost_filesystem boost_url boost_thread boost_locale
        boost_program_options boost_log boost_log_setup boost_process yaml-cpp ssl crypto archive magic gtest gmock)

add_test(NAME ${BINARY} COMMAND ${BINARY} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

