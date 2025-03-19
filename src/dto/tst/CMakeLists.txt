set(BINARY dto_tst)
set(STATIC_LIB awsmockdto)

set(SQS_SOURCES sqs/MessageAttributeTest.cpp)
set(TRANSFER_SOURCES transfer/CreateServerRequestTest.cpp transfer/CreateServerResponseTest.cpp transfer/CreateUserRequestTest.cpp transfer/CreateServerResponseTest.cpp)
set(MAIN_SOURCES main.cpp)

# Includes
include_directories(../include ../../core/include ../../db/include ${BSONCXX_INCLUDE_DIRS} ${MONGOCXX_INCLUDE_DIRS})

add_executable(${BINARY} ${SQS_SOURCES} ${TRANSFER_SOURCES} ${MAIN_SOURCES})
target_link_libraries(${BINARY} PUBLIC ${STATIC_LIB} awsmockcore awsmockdto awsmockdb
        $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>
        $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>
        Boost::filesystem Boost::url Boost::thread Boost::locale yaml-cpp::yaml-cpp OpenSSL::SSL OpenSSL::SSL
        LibArchive::LibArchive unofficial::libmagic::libmagic GTest::gtest GTest::gtest_main GTest::gmock
        GTest::gmock_main)

add_test(NAME ${BINARY} COMMAND ${BINARY} WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})
