set(BINARY dto_tst)
set(STATIC_LIB awsmockdto)

set(S3_SOURCES s3/S3DtoTest.cpp)
set(SQS_SOURCES sqs/MessageAttributeTest.cpp)
set(TRANSFER_SOURCES transfer/CreateServerRequestTest.cpp transfer/CreateServerResponseTest.cpp
        transfer/CreateUserRequestTest.cpp transfer/CreateServerResponseTest.cpp)
set(MAIN_SOURCES main.cpp)

add_executable(${BINARY} ${S3_SOURCES} ${SQS_SOURCES} ${TRANSFER_SOURCES} ${MAIN_SOURCES})
target_include_directories(${BINARY} PRIVATE ../include ../../core/include ../../db/include ${BSONCXX_INCLUDE_DIR}
        ${MONGOCXX_INCLUDE_DIR} ${EXTERNAL_INCLUDE_DIR})
target_link_directories(${BINARY} PRIVATE ${BSONCXX_LIB_DIR} ${MONGOCXX_LIB_DIR} ${EXTERNAL_LIB_DIR})
target_link_libraries(${BINARY} PUBLIC awsmockcore awsmockdto boost_filesystem boost_process boost_program_options
        boost_log boost_log_setup ssl crypto archive magic)

add_test(NAME ${BINARY} COMMAND ${BINARY} WORKING_DIRECTORY ${CMAKE_CURRENT_SRC_DIR})
