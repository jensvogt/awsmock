set(BINARY core_tst)
set(STATIC_LIB awsmockcore)

set(SOURCES CryptoTests.cpp FileUtilsTests.cpp DirUtilsTests.cpp StringUtilsTests.cpp ConfigurationTests.cpp
        RandomUtilsTests.cpp AwsUtilsTests.cpp HttpUtilsTests.cpp SystemUtilsTests.cpp XmlUtilsTests.cpp
        JwtUtilsTests.cpp TarUtilsTests.cpp DateTimeUtilsTests.cpp main.cpp)

include_directories(../include ${BSONCXX_INCLUDE_DIRS} ${MONGOCXX_INCLUDE_DIRS})

add_executable(${BINARY} ${SOURCES})
target_link_libraries(${BINARY} awsmockcore
        $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>
        $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>
        Boost::filesystem Boost::url Boost::thread Boost::locale yaml-cpp::yaml-cpp OpenSSL::SSL OpenSSL::SSL
        LibArchive::LibArchive unofficial::libmagic::libmagic GTest::gtest GTest::gtest_main GTest::gmock
        GTest::gmock_main)

add_test(NAME ${BINARY} COMMAND ${BINARY} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

